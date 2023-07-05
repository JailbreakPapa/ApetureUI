#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Containers/HashTable.h>
#include <Foundation/Containers/StaticArray.h>
#include <Foundation/Memory/CommonAllocators.h>
#include <Foundation/Strings/String.h>

namespace HashTableTestDetail
{
  typedef wdConstructionCounter st;

  struct Collision
  {
    wdUInt32 hash;
    int key;

    inline Collision(wdUInt32 uiHash, int iKey)
    {
      this->hash = uiHash;
      this->key = iKey;
    }

    inline bool operator==(const Collision& other) const { return key == other.key; }

    WD_DECLARE_POD_TYPE();
  };

  class OnlyMovable
  {
  public:
    OnlyMovable(wdUInt32 uiHash)
      : hash(uiHash)
      , m_NumTimesMoved(0)
    {
    }
    OnlyMovable(OnlyMovable&& other) { *this = std::move(other); }

    void operator=(OnlyMovable&& other)
    {
      hash = other.hash;
      m_NumTimesMoved = 0;
      ++other.m_NumTimesMoved;
    }

    bool operator==(const OnlyMovable& other) const { return hash == other.hash; }

    int m_NumTimesMoved;
    wdUInt32 hash;

  private:
    OnlyMovable(const OnlyMovable&);
    void operator=(const OnlyMovable&);
  };
} // namespace HashTableTestDetail

template <>
struct wdHashHelper<HashTableTestDetail::Collision>
{
  WD_ALWAYS_INLINE static wdUInt32 Hash(const HashTableTestDetail::Collision& value) { return value.hash; }

  WD_ALWAYS_INLINE static bool Equal(const HashTableTestDetail::Collision& a, const HashTableTestDetail::Collision& b) { return a == b; }
};

template <>
struct wdHashHelper<HashTableTestDetail::OnlyMovable>
{
  WD_ALWAYS_INLINE static wdUInt32 Hash(const HashTableTestDetail::OnlyMovable& value) { return value.hash; }

  WD_ALWAYS_INLINE static bool Equal(const HashTableTestDetail::OnlyMovable& a, const HashTableTestDetail::OnlyMovable& b)
  {
    return a.hash == b.hash;
  }
};

WD_CREATE_SIMPLE_TEST(Containers, HashTable)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor")
  {
    wdHashTable<wdInt32, HashTableTestDetail::st> table1;

    WD_TEST_BOOL(table1.GetCount() == 0);
    WD_TEST_BOOL(table1.IsEmpty());

    wdUInt32 counter = 0;
    for (wdHashTable<wdInt32, HashTableTestDetail::st>::ConstIterator it = table1.GetIterator(); it.IsValid(); ++it)
    {
      ++counter;
    }
    WD_TEST_INT(counter, 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Copy Constructor/Assignment/Iterator")
  {
    wdHashTable<wdInt32, HashTableTestDetail::st> table1;

    for (wdInt32 i = 0; i < 64; ++i)
    {
      wdInt32 key;

      do
      {
        key = rand() % 100000;
      } while (table1.Contains(key));

      table1.Insert(key, wdConstructionCounter(i));
    }

    // insert an element at the very end
    table1.Insert(47, wdConstructionCounter(64));

    wdHashTable<wdInt32, HashTableTestDetail::st> table2;
    table2 = table1;
    wdHashTable<wdInt32, HashTableTestDetail::st> table3(table1);

    WD_TEST_INT(table1.GetCount(), 65);
    WD_TEST_INT(table2.GetCount(), 65);
    WD_TEST_INT(table3.GetCount(), 65);

    wdUInt32 uiCounter = 0;
    for (wdHashTable<wdInt32, HashTableTestDetail::st>::ConstIterator it = table1.GetIterator(); it.IsValid(); ++it)
    {
      wdConstructionCounter value;

      WD_TEST_BOOL(table2.TryGetValue(it.Key(), value));
      WD_TEST_BOOL(it.Value() == value);
      WD_TEST_BOOL(*table2.GetValue(it.Key()) == it.Value());

      WD_TEST_BOOL(table3.TryGetValue(it.Key(), value));
      WD_TEST_BOOL(it.Value() == value);
      WD_TEST_BOOL(*table3.GetValue(it.Key()) == it.Value());

      ++uiCounter;
    }
    WD_TEST_INT(uiCounter, table1.GetCount());

    for (wdHashTable<wdInt32, HashTableTestDetail::st>::Iterator it = table1.GetIterator(); it.IsValid(); ++it)
    {
      it.Value() = HashTableTestDetail::st(42);
    }

    for (wdHashTable<wdInt32, HashTableTestDetail::st>::ConstIterator it = table1.GetIterator(); it.IsValid(); ++it)
    {
      wdConstructionCounter value;

      WD_TEST_BOOL(table1.TryGetValue(it.Key(), value));
      WD_TEST_BOOL(it.Value() == value);
      WD_TEST_BOOL(value.m_iData == 42);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Move Copy Constructor/Assignment")
  {
    wdHashTable<wdInt32, HashTableTestDetail::st> table1;
    for (wdInt32 i = 0; i < 64; ++i)
    {
      table1.Insert(i, wdConstructionCounter(i));
    }

    wdUInt64 memoryUsage = table1.GetHeapMemoryUsage();

    wdHashTable<wdInt32, HashTableTestDetail::st> table2;
    table2 = std::move(table1);

    WD_TEST_INT(table1.GetCount(), 0);
    WD_TEST_INT(table1.GetHeapMemoryUsage(), 0);
    WD_TEST_INT(table2.GetCount(), 64);
    WD_TEST_INT(table2.GetHeapMemoryUsage(), memoryUsage);

    wdHashTable<wdInt32, HashTableTestDetail::st> table3(std::move(table2));

    WD_TEST_INT(table2.GetCount(), 0);
    WD_TEST_INT(table2.GetHeapMemoryUsage(), 0);
    WD_TEST_INT(table3.GetCount(), 64);
    WD_TEST_INT(table3.GetHeapMemoryUsage(), memoryUsage);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Move Insert")
  {
    HashTableTestDetail::OnlyMovable noCopyObject(42);

    {
      wdHashTable<HashTableTestDetail::OnlyMovable, int> noCopyKey;
      // noCopyKey.Insert(noCopyObject, 10); // Should not compile
      noCopyKey.Insert(std::move(noCopyObject), 10);
      WD_TEST_INT(noCopyObject.m_NumTimesMoved, 1);
      WD_TEST_BOOL(noCopyKey.Contains(noCopyObject));
    }

    {
      wdHashTable<int, HashTableTestDetail::OnlyMovable> noCopyValue;
      // noCopyValue.Insert(10, noCopyObject); // Should not compile
      noCopyValue.Insert(10, std::move(noCopyObject));
      WD_TEST_INT(noCopyObject.m_NumTimesMoved, 2);
      WD_TEST_BOOL(noCopyValue.Contains(10));
    }

    {
      wdHashTable<HashTableTestDetail::OnlyMovable, HashTableTestDetail::OnlyMovable> noCopyAnything;
      // noCopyAnything.Insert(10, noCopyObject); // Should not compile
      // noCopyAnything.Insert(noCopyObject, 10); // Should not compile
      noCopyAnything.Insert(std::move(noCopyObject), std::move(noCopyObject));
      WD_TEST_INT(noCopyObject.m_NumTimesMoved, 4);
      WD_TEST_BOOL(noCopyAnything.Contains(noCopyObject));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Collision Tests")
  {
    wdHashTable<HashTableTestDetail::Collision, int> map2;

    map2[HashTableTestDetail::Collision(0, 0)] = 0;
    map2[HashTableTestDetail::Collision(1, 1)] = 1;
    map2[HashTableTestDetail::Collision(0, 2)] = 2;
    map2[HashTableTestDetail::Collision(1, 3)] = 3;
    map2[HashTableTestDetail::Collision(1, 4)] = 4;
    map2[HashTableTestDetail::Collision(0, 5)] = 5;

    WD_TEST_BOOL(map2[HashTableTestDetail::Collision(0, 0)] == 0);
    WD_TEST_BOOL(map2[HashTableTestDetail::Collision(1, 1)] == 1);
    WD_TEST_BOOL(map2[HashTableTestDetail::Collision(0, 2)] == 2);
    WD_TEST_BOOL(map2[HashTableTestDetail::Collision(1, 3)] == 3);
    WD_TEST_BOOL(map2[HashTableTestDetail::Collision(1, 4)] == 4);
    WD_TEST_BOOL(map2[HashTableTestDetail::Collision(0, 5)] == 5);

    WD_TEST_BOOL(map2.Contains(HashTableTestDetail::Collision(0, 0)));
    WD_TEST_BOOL(map2.Contains(HashTableTestDetail::Collision(1, 1)));
    WD_TEST_BOOL(map2.Contains(HashTableTestDetail::Collision(0, 2)));
    WD_TEST_BOOL(map2.Contains(HashTableTestDetail::Collision(1, 3)));
    WD_TEST_BOOL(map2.Contains(HashTableTestDetail::Collision(1, 4)));
    WD_TEST_BOOL(map2.Contains(HashTableTestDetail::Collision(0, 5)));

    WD_TEST_BOOL(map2.Remove(HashTableTestDetail::Collision(0, 0)));
    WD_TEST_BOOL(map2.Remove(HashTableTestDetail::Collision(1, 1)));

    WD_TEST_BOOL(map2[HashTableTestDetail::Collision(0, 2)] == 2);
    WD_TEST_BOOL(map2[HashTableTestDetail::Collision(1, 3)] == 3);
    WD_TEST_BOOL(map2[HashTableTestDetail::Collision(1, 4)] == 4);
    WD_TEST_BOOL(map2[HashTableTestDetail::Collision(0, 5)] == 5);

    WD_TEST_BOOL(!map2.Contains(HashTableTestDetail::Collision(0, 0)));
    WD_TEST_BOOL(!map2.Contains(HashTableTestDetail::Collision(1, 1)));
    WD_TEST_BOOL(map2.Contains(HashTableTestDetail::Collision(0, 2)));
    WD_TEST_BOOL(map2.Contains(HashTableTestDetail::Collision(1, 3)));
    WD_TEST_BOOL(map2.Contains(HashTableTestDetail::Collision(1, 4)));
    WD_TEST_BOOL(map2.Contains(HashTableTestDetail::Collision(0, 5)));

    map2[HashTableTestDetail::Collision(0, 6)] = 6;
    map2[HashTableTestDetail::Collision(1, 7)] = 7;

    WD_TEST_BOOL(map2[HashTableTestDetail::Collision(0, 2)] == 2);
    WD_TEST_BOOL(map2[HashTableTestDetail::Collision(1, 3)] == 3);
    WD_TEST_BOOL(map2[HashTableTestDetail::Collision(1, 4)] == 4);
    WD_TEST_BOOL(map2[HashTableTestDetail::Collision(0, 5)] == 5);
    WD_TEST_BOOL(map2[HashTableTestDetail::Collision(0, 6)] == 6);
    WD_TEST_BOOL(map2[HashTableTestDetail::Collision(1, 7)] == 7);

    WD_TEST_BOOL(map2.Contains(HashTableTestDetail::Collision(0, 2)));
    WD_TEST_BOOL(map2.Contains(HashTableTestDetail::Collision(1, 3)));
    WD_TEST_BOOL(map2.Contains(HashTableTestDetail::Collision(1, 4)));
    WD_TEST_BOOL(map2.Contains(HashTableTestDetail::Collision(0, 5)));
    WD_TEST_BOOL(map2.Contains(HashTableTestDetail::Collision(0, 6)));
    WD_TEST_BOOL(map2.Contains(HashTableTestDetail::Collision(1, 7)));

    WD_TEST_BOOL(map2.Remove(HashTableTestDetail::Collision(1, 4)));
    WD_TEST_BOOL(map2.Remove(HashTableTestDetail::Collision(0, 6)));

    WD_TEST_BOOL(map2[HashTableTestDetail::Collision(0, 2)] == 2);
    WD_TEST_BOOL(map2[HashTableTestDetail::Collision(1, 3)] == 3);
    WD_TEST_BOOL(map2[HashTableTestDetail::Collision(0, 5)] == 5);
    WD_TEST_BOOL(map2[HashTableTestDetail::Collision(1, 7)] == 7);

    WD_TEST_BOOL(!map2.Contains(HashTableTestDetail::Collision(1, 4)));
    WD_TEST_BOOL(!map2.Contains(HashTableTestDetail::Collision(0, 6)));
    WD_TEST_BOOL(map2.Contains(HashTableTestDetail::Collision(0, 2)));
    WD_TEST_BOOL(map2.Contains(HashTableTestDetail::Collision(1, 3)));
    WD_TEST_BOOL(map2.Contains(HashTableTestDetail::Collision(0, 5)));
    WD_TEST_BOOL(map2.Contains(HashTableTestDetail::Collision(1, 7)));

    map2[HashTableTestDetail::Collision(0, 2)] = 3;
    map2[HashTableTestDetail::Collision(0, 5)] = 6;
    map2[HashTableTestDetail::Collision(1, 3)] = 4;

    WD_TEST_BOOL(map2[HashTableTestDetail::Collision(0, 2)] == 3);
    WD_TEST_BOOL(map2[HashTableTestDetail::Collision(0, 5)] == 6);
    WD_TEST_BOOL(map2[HashTableTestDetail::Collision(1, 3)] == 4);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Clear")
  {
    WD_TEST_BOOL(HashTableTestDetail::st::HasAllDestructed());

    {
      wdHashTable<wdUInt32, HashTableTestDetail::st> m1;
      m1[0] = HashTableTestDetail::st(1);
      WD_TEST_BOOL(HashTableTestDetail::st::HasDone(2, 1)); // for inserting new elements 1 temporary is created (and destroyed)

      m1[1] = HashTableTestDetail::st(3);
      WD_TEST_BOOL(HashTableTestDetail::st::HasDone(2, 1)); // for inserting new elements 2 temporary is created (and destroyed)

      m1[0] = HashTableTestDetail::st(2);
      WD_TEST_BOOL(HashTableTestDetail::st::HasDone(1, 1)); // nothing new to create, so only the one temporary is used

      m1.Clear();
      WD_TEST_BOOL(HashTableTestDetail::st::HasDone(0, 2));
      WD_TEST_BOOL(HashTableTestDetail::st::HasAllDestructed());
    }

    {
      wdHashTable<HashTableTestDetail::st, wdUInt32> m1;
      m1[HashTableTestDetail::st(0)] = 1;
      WD_TEST_BOOL(HashTableTestDetail::st::HasDone(2, 1)); // one temporary

      m1[HashTableTestDetail::st(1)] = 3;
      WD_TEST_BOOL(HashTableTestDetail::st::HasDone(2, 1)); // one temporary

      m1[HashTableTestDetail::st(0)] = 2;
      WD_TEST_BOOL(HashTableTestDetail::st::HasDone(1, 1)); // nothing new to create, so only the one temporary is used

      m1.Clear();
      WD_TEST_BOOL(HashTableTestDetail::st::HasDone(0, 2));
      WD_TEST_BOOL(HashTableTestDetail::st::HasAllDestructed());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Insert/TryGetValue/GetValue")
  {
    wdHashTable<wdInt32, HashTableTestDetail::st> a1;

    for (wdInt32 i = 0; i < 10; ++i)
    {
      WD_TEST_BOOL(!a1.Insert(i, i - 20));
    }

    for (wdInt32 i = 0; i < 10; ++i)
    {
      HashTableTestDetail::st oldValue;
      WD_TEST_BOOL(a1.Insert(i, i, &oldValue));
      WD_TEST_INT(oldValue.m_iData, i - 20);
    }

    HashTableTestDetail::st value;
    WD_TEST_BOOL(a1.TryGetValue(9, value));
    WD_TEST_INT(value.m_iData, 9);
    WD_TEST_INT(a1.GetValue(9)->m_iData, 9);

    WD_TEST_BOOL(!a1.TryGetValue(11, value));
    WD_TEST_INT(value.m_iData, 9);
    WD_TEST_BOOL(a1.GetValue(11) == nullptr);

    HashTableTestDetail::st* pValue;
    WD_TEST_BOOL(a1.TryGetValue(9, pValue));
    WD_TEST_INT(pValue->m_iData, 9);

    pValue->m_iData = 20;
    WD_TEST_INT(a1[9].m_iData, 20);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Remove/Compact")
  {
    wdHashTable<wdInt32, HashTableTestDetail::st> a;

    WD_TEST_BOOL(a.GetHeapMemoryUsage() == 0);

    for (wdInt32 i = 0; i < 1000; ++i)
    {
      a.Insert(i, i);
      WD_TEST_INT(a.GetCount(), i + 1);
    }

    WD_TEST_BOOL(a.GetHeapMemoryUsage() >= 1000 * (sizeof(wdInt32) + sizeof(HashTableTestDetail::st)));

    a.Compact();

    for (wdInt32 i = 0; i < 1000; ++i)
      WD_TEST_INT(a[i].m_iData, i);


    for (wdInt32 i = 0; i < 250; ++i)
    {
      HashTableTestDetail::st oldValue;
      WD_TEST_BOOL(a.Remove(i, &oldValue));
      WD_TEST_INT(oldValue.m_iData, i);
    }
    WD_TEST_INT(a.GetCount(), 750);

    for (wdHashTable<wdInt32, HashTableTestDetail::st>::Iterator it = a.GetIterator(); it.IsValid();)
    {
      if (it.Key() < 500)
        it = a.Remove(it);
      else
        ++it;
    }
    WD_TEST_INT(a.GetCount(), 500);
    a.Compact();

    for (wdInt32 i = 500; i < 1000; ++i)
      WD_TEST_INT(a[i].m_iData, i);

    a.Clear();
    a.Compact();

    WD_TEST_BOOL(a.GetHeapMemoryUsage() == 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator[]")
  {
    wdHashTable<wdInt32, wdInt32> a;

    a.Insert(4, 20);
    a[2] = 30;

    WD_TEST_INT(a[4], 20);
    WD_TEST_INT(a[2], 30);
    WD_TEST_INT(a[1], 0); // new values are default constructed
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator==/!=")
  {
    wdStaticArray<wdInt32, 64> keys[2];

    for (wdUInt32 i = 0; i < 64; ++i)
    {
      keys[0].PushBack(rand());
    }

    keys[1] = keys[0];

    wdHashTable<wdInt32, HashTableTestDetail::st> t[2];

    for (wdUInt32 i = 0; i < 2; ++i)
    {
      while (!keys[i].IsEmpty())
      {
        const wdUInt32 uiIndex = rand() % keys[i].GetCount();
        const wdInt32 key = keys[i][uiIndex];
        t[i].Insert(key, HashTableTestDetail::st(key * 3456));

        keys[i].RemoveAtAndSwap(uiIndex);
      }
    }

    WD_TEST_BOOL(t[0] == t[1]);

    t[0].Insert(32, HashTableTestDetail::st(64));
    WD_TEST_BOOL(t[0] != t[1]);

    t[1].Insert(32, HashTableTestDetail::st(47));
    WD_TEST_BOOL(t[0] != t[1]);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "CompatibleKeyType")
  {
    wdProxyAllocator testAllocator("Test", wdFoundation::GetDefaultAllocator());
    wdLocalAllocatorWrapper allocWrapper(&testAllocator);
    using TestString = wdHybridString<32, wdLocalAllocatorWrapper>;

    wdHashTable<TestString, int> stringTable;
    const char* szChar = "VeryLongStringDefinitelyMoreThan32Chars1111elf!!!!";
    const char* szString = "AnotherVeryLongStringThisTimeUsedForStringView!!!!";
    wdStringView sView(szString);
    wdStringBuilder sBuilder("BuilderAlsoNeedsToBeAVeryLongStringToTriggerAllocation");
    wdString sString("String");
    WD_TEST_BOOL(!stringTable.Insert(szChar, 1));
    WD_TEST_BOOL(!stringTable.Insert(sView, 2));
    WD_TEST_BOOL(!stringTable.Insert(sBuilder, 3));
    WD_TEST_BOOL(!stringTable.Insert(sString, 4));
    WD_TEST_BOOL(stringTable.Insert(szString, 2));

    wdUInt64 oldAllocCount = testAllocator.GetStats().m_uiNumAllocations;

    WD_TEST_BOOL(stringTable.Contains(szChar));
    WD_TEST_BOOL(stringTable.Contains(sView));
    WD_TEST_BOOL(stringTable.Contains(sBuilder));
    WD_TEST_BOOL(stringTable.Contains(sString));

    WD_TEST_INT(testAllocator.GetStats().m_uiNumAllocations, oldAllocCount);

    WD_TEST_INT(*stringTable.GetValue(szChar), 1);
    WD_TEST_INT(*stringTable.GetValue(sView), 2);
    WD_TEST_INT(*stringTable.GetValue(sBuilder), 3);
    WD_TEST_INT(*stringTable.GetValue(sString), 4);

    WD_TEST_INT(testAllocator.GetStats().m_uiNumAllocations, oldAllocCount);

    WD_TEST_BOOL(stringTable.Remove(szChar));
    WD_TEST_BOOL(stringTable.Remove(sView));
    WD_TEST_BOOL(stringTable.Remove(sBuilder));
    WD_TEST_BOOL(stringTable.Remove(sString));

    WD_TEST_INT(testAllocator.GetStats().m_uiNumAllocations, oldAllocCount);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Swap")
  {
    wdStringBuilder tmp;
    wdHashTable<wdString, wdInt32> map1;
    wdHashTable<wdString, wdInt32> map2;

    for (wdUInt32 i = 0; i < 1000; ++i)
    {
      tmp.Format("stuff{}bla", i);
      map1[tmp] = i;

      tmp.Format("{0}{0}{0}", i);
      map2[tmp] = i;
    }

    map1.Swap(map2);

    for (wdUInt32 i = 0; i < 1000; ++i)
    {
      tmp.Format("stuff{}bla", i);
      WD_TEST_BOOL(map2.Contains(tmp));
      WD_TEST_INT(map2[tmp], i);

      tmp.Format("{0}{0}{0}", i);
      WD_TEST_BOOL(map1.Contains(tmp));
      WD_TEST_INT(map1[tmp], i);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "foreach")
  {
    wdStringBuilder tmp;
    wdHashTable<wdString, wdInt32> map;
    wdHashTable<wdString, wdInt32> map2;

    for (wdUInt32 i = 0; i < 1000; ++i)
    {
      tmp.Format("stuff{}bla", i);
      map[tmp] = i;
    }

    WD_TEST_INT(map.GetCount(), 1000);

    map2 = map;
    WD_TEST_INT(map2.GetCount(), map.GetCount());

    for (wdHashTable<wdString, wdInt32>::Iterator it = begin(map); it != end(map); ++it)
    {
      const wdString& k = it.Key();
      wdInt32 v = it.Value();

      map2.Remove(k);
    }

    WD_TEST_BOOL(map2.IsEmpty());
    map2 = map;

    for (auto it : map)
    {
      const wdString& k = it.Key();
      wdInt32 v = it.Value();

      map2.Remove(k);
    }

    WD_TEST_BOOL(map2.IsEmpty());
    map2 = map;

    // just check that this compiles
    for (auto it : static_cast<const wdHashTable<wdString, wdInt32>&>(map))
    {
      const wdString& k = it.Key();
      wdInt32 v = it.Value();

      map2.Remove(k);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Find")
  {
    wdStringBuilder tmp;
    wdHashTable<wdString, wdInt32> map;

    for (wdUInt32 i = 0; i < 1000; ++i)
    {
      tmp.Format("stuff{}bla", i);
      map[tmp] = i;
    }

    for (wdInt32 i = map.GetCount() - 1; i > 0; --i)
    {
      tmp.Format("stuff{}bla", i);

      auto it = map.Find(tmp);
      auto cit = static_cast<const wdHashTable<wdString, wdInt32>&>(map).Find(tmp);

      WD_TEST_STRING(it.Key(), tmp);
      WD_TEST_INT(it.Value(), i);

      WD_TEST_STRING(cit.Key(), tmp);
      WD_TEST_INT(cit.Value(), i);

      int allowedIterations = map.GetCount();
      for (auto it2 = it; it2.IsValid(); ++it2)
      {
        // just test that iteration is possible and terminates correctly
        --allowedIterations;
        WD_TEST_BOOL(allowedIterations >= 0);
      }

      allowedIterations = map.GetCount();
      for (auto cit2 = cit; cit2.IsValid(); ++cit2)
      {
        // just test that iteration is possible and terminates correctly
        --allowedIterations;
        WD_TEST_BOOL(allowedIterations >= 0);
      }

      map.Remove(it);
    }
  }
}
