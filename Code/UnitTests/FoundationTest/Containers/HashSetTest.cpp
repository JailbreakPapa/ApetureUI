#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Containers/HashSet.h>
#include <Foundation/Containers/StaticArray.h>
#include <Foundation/Memory/CommonAllocators.h>

namespace
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
} // namespace

template <>
struct wdHashHelper<Collision>
{
  WD_ALWAYS_INLINE static wdUInt32 Hash(const Collision& value) { return value.hash; }

  WD_ALWAYS_INLINE static bool Equal(const Collision& a, const Collision& b) { return a == b; }
};

template <>
struct wdHashHelper<OnlyMovable>
{
  WD_ALWAYS_INLINE static wdUInt32 Hash(const OnlyMovable& value) { return value.hash; }

  WD_ALWAYS_INLINE static bool Equal(const OnlyMovable& a, const OnlyMovable& b) { return a.hash == b.hash; }
};

WD_CREATE_SIMPLE_TEST(Containers, HashSet)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor")
  {
    wdHashSet<wdInt32> table1;

    WD_TEST_BOOL(table1.GetCount() == 0);
    WD_TEST_BOOL(table1.IsEmpty());

    wdUInt32 counter = 0;
    for (auto it = table1.GetIterator(); it.IsValid(); ++it)
    {
      ++counter;
    }
    WD_TEST_INT(counter, 0);

    WD_TEST_BOOL(begin(table1) == end(table1));
    WD_TEST_BOOL(cbegin(table1) == cend(table1));
    table1.Reserve(10);
    WD_TEST_BOOL(begin(table1) == end(table1));
    WD_TEST_BOOL(cbegin(table1) == cend(table1));

    for (auto value : table1)
    {
      ++counter;
    }
    WD_TEST_INT(counter, 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Copy Constructor/Assignment/Iterator")
  {
    wdHashSet<wdInt32> table1;

    for (wdInt32 i = 0; i < 64; ++i)
    {
      wdInt32 key;

      do
      {
        key = rand() % 100000;
      } while (table1.Contains(key));

      table1.Insert(key);
    }

    // insert an element at the very end
    table1.Insert(47);

    wdHashSet<wdInt32> table2;
    table2 = table1;
    wdHashSet<wdInt32> table3(table1);

    WD_TEST_INT(table1.GetCount(), 65);
    WD_TEST_INT(table2.GetCount(), 65);
    WD_TEST_INT(table3.GetCount(), 65);
    WD_TEST_BOOL(begin(table1) != end(table1));
    WD_TEST_BOOL(cbegin(table1) != cend(table1));

    wdUInt32 uiCounter = 0;
    for (auto it = table1.GetIterator(); it.IsValid(); ++it)
    {
      wdConstructionCounter value;
      WD_TEST_BOOL(table2.Contains(it.Key()));
      WD_TEST_BOOL(table3.Contains(it.Key()));
      ++uiCounter;
    }
    WD_TEST_INT(uiCounter, table1.GetCount());

    uiCounter = 0;
    for (const auto& value : table1)
    {
      WD_TEST_BOOL(table2.Contains(value));
      WD_TEST_BOOL(table3.Contains(value));
      ++uiCounter;
    }
    WD_TEST_INT(uiCounter, table1.GetCount());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Move Copy Constructor/Assignment")
  {
    wdHashSet<st> set1;
    for (wdInt32 i = 0; i < 64; ++i)
    {
      set1.Insert(wdConstructionCounter(i));
    }

    wdUInt64 memoryUsage = set1.GetHeapMemoryUsage();

    wdHashSet<st> set2;
    set2 = std::move(set1);

    WD_TEST_INT(set1.GetCount(), 0);
    WD_TEST_INT(set1.GetHeapMemoryUsage(), 0);
    WD_TEST_INT(set2.GetCount(), 64);
    WD_TEST_INT(set2.GetHeapMemoryUsage(), memoryUsage);

    wdHashSet<st> set3(std::move(set2));

    WD_TEST_INT(set2.GetCount(), 0);
    WD_TEST_INT(set2.GetHeapMemoryUsage(), 0);
    WD_TEST_INT(set3.GetCount(), 64);
    WD_TEST_INT(set3.GetHeapMemoryUsage(), memoryUsage);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Collision Tests")
  {
    wdHashSet<Collision> set2;

    set2.Insert(Collision(0, 0));
    set2.Insert(Collision(1, 1));
    set2.Insert(Collision(0, 2));
    set2.Insert(Collision(1, 3));
    set2.Insert(Collision(1, 4));
    set2.Insert(Collision(0, 5));

    WD_TEST_BOOL(set2.Contains(Collision(0, 0)));
    WD_TEST_BOOL(set2.Contains(Collision(1, 1)));
    WD_TEST_BOOL(set2.Contains(Collision(0, 2)));
    WD_TEST_BOOL(set2.Contains(Collision(1, 3)));
    WD_TEST_BOOL(set2.Contains(Collision(1, 4)));
    WD_TEST_BOOL(set2.Contains(Collision(0, 5)));

    WD_TEST_BOOL(set2.Remove(Collision(0, 0)));
    WD_TEST_BOOL(set2.Remove(Collision(1, 1)));

    WD_TEST_BOOL(!set2.Contains(Collision(0, 0)));
    WD_TEST_BOOL(!set2.Contains(Collision(1, 1)));
    WD_TEST_BOOL(set2.Contains(Collision(0, 2)));
    WD_TEST_BOOL(set2.Contains(Collision(1, 3)));
    WD_TEST_BOOL(set2.Contains(Collision(1, 4)));
    WD_TEST_BOOL(set2.Contains(Collision(0, 5)));

    set2.Insert(Collision(0, 6));
    set2.Insert(Collision(1, 7));

    WD_TEST_BOOL(set2.Contains(Collision(0, 2)));
    WD_TEST_BOOL(set2.Contains(Collision(1, 3)));
    WD_TEST_BOOL(set2.Contains(Collision(1, 4)));
    WD_TEST_BOOL(set2.Contains(Collision(0, 5)));
    WD_TEST_BOOL(set2.Contains(Collision(0, 6)));
    WD_TEST_BOOL(set2.Contains(Collision(1, 7)));

    WD_TEST_BOOL(set2.Remove(Collision(1, 4)));
    WD_TEST_BOOL(set2.Remove(Collision(0, 6)));

    WD_TEST_BOOL(!set2.Contains(Collision(1, 4)));
    WD_TEST_BOOL(!set2.Contains(Collision(0, 6)));
    WD_TEST_BOOL(set2.Contains(Collision(0, 2)));
    WD_TEST_BOOL(set2.Contains(Collision(1, 3)));
    WD_TEST_BOOL(set2.Contains(Collision(0, 5)));
    WD_TEST_BOOL(set2.Contains(Collision(1, 7)));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Clear")
  {
    WD_TEST_BOOL(st::HasAllDestructed());

    {
      wdHashSet<st> m1;
      m1.Insert(st(1));
      WD_TEST_BOOL(st::HasDone(2, 1)); // for inserting new elements 1 temporary is created (and destroyed)

      m1.Insert(st(3));
      WD_TEST_BOOL(st::HasDone(2, 1)); // for inserting new elements 2 temporary is created (and destroyed)

      m1.Insert(st(1));
      WD_TEST_BOOL(st::HasDone(1, 1)); // nothing new to create, so only the one temporary is used

      m1.Clear();
      WD_TEST_BOOL(st::HasDone(0, 2));
      WD_TEST_BOOL(st::HasAllDestructed());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Insert")
  {
    wdHashSet<wdInt32> a1;

    for (wdInt32 i = 0; i < 10; ++i)
    {
      WD_TEST_BOOL(!a1.Insert(i));
    }

    for (wdInt32 i = 0; i < 10; ++i)
    {
      WD_TEST_BOOL(a1.Insert(i));
    }
  }


  WD_TEST_BLOCK(wdTestBlock::Enabled, "Move Insert")
  {
    OnlyMovable noCopyObject(42);

    wdHashSet<OnlyMovable> noCopyKey;
    // noCopyKey.Insert(noCopyObject); // Should not compile
    noCopyKey.Insert(std::move(noCopyObject));
    WD_TEST_INT(noCopyObject.m_NumTimesMoved, 1);
    WD_TEST_BOOL(noCopyKey.Contains(noCopyObject));
  }


  WD_TEST_BLOCK(wdTestBlock::Enabled, "Remove/Compact")
  {
    wdHashSet<wdInt32> a;

    WD_TEST_BOOL(a.GetHeapMemoryUsage() == 0);

    for (wdInt32 i = 0; i < 1000; ++i)
    {
      a.Insert(i);
      WD_TEST_INT(a.GetCount(), i + 1);
    }

    WD_TEST_BOOL(a.GetHeapMemoryUsage() >= 1000 * (sizeof(wdInt32)));

    a.Compact();

    for (wdInt32 i = 0; i < 500; ++i)
    {
      WD_TEST_BOOL(a.Remove(i));
    }

    a.Compact();

    for (wdInt32 i = 500; i < 1000; ++i)
    {
      WD_TEST_BOOL(a.Contains(i));
    }

    a.Clear();
    a.Compact();

    WD_TEST_BOOL(a.GetHeapMemoryUsage() == 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Remove (Iterator)")
  {
    wdHashSet<wdInt32> a;

    WD_TEST_BOOL(a.GetHeapMemoryUsage() == 0);
    for (wdInt32 i = 0; i < 1000; ++i)
      a.Insert(i);

    wdHashSet<wdInt32>::ConstIterator it = a.GetIterator();

    for (wdInt32 i = 0; i < 1000 - 1; ++i)
    {
      wdInt32 value = it.Key();
      it = a.Remove(it);
      WD_TEST_BOOL(!a.Contains(value));
      WD_TEST_BOOL(it.IsValid());
      WD_TEST_INT(a.GetCount(), 1000 - 1 - i);
    }
    it = a.Remove(it);
    WD_TEST_BOOL(!it.IsValid());
    WD_TEST_BOOL(a.IsEmpty());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Set Operations")
  {
    wdHashSet<wdUInt32> base;
    base.Insert(1);
    base.Insert(3);
    base.Insert(5);

    wdHashSet<wdUInt32> empty;

    wdHashSet<wdUInt32> disjunct;
    disjunct.Insert(2);
    disjunct.Insert(4);
    disjunct.Insert(6);

    wdHashSet<wdUInt32> subSet;
    subSet.Insert(1);
    subSet.Insert(5);

    wdHashSet<wdUInt32> superSet;
    superSet.Insert(1);
    superSet.Insert(3);
    superSet.Insert(5);
    superSet.Insert(7);

    wdHashSet<wdUInt32> nonDisjunctNonEmptySubSet;
    nonDisjunctNonEmptySubSet.Insert(1);
    nonDisjunctNonEmptySubSet.Insert(4);
    nonDisjunctNonEmptySubSet.Insert(5);

    // ContainsSet
    WD_TEST_BOOL(base.ContainsSet(base));

    WD_TEST_BOOL(base.ContainsSet(empty));
    WD_TEST_BOOL(!empty.ContainsSet(base));

    WD_TEST_BOOL(!base.ContainsSet(disjunct));
    WD_TEST_BOOL(!disjunct.ContainsSet(base));

    WD_TEST_BOOL(base.ContainsSet(subSet));
    WD_TEST_BOOL(!subSet.ContainsSet(base));

    WD_TEST_BOOL(!base.ContainsSet(superSet));
    WD_TEST_BOOL(superSet.ContainsSet(base));

    WD_TEST_BOOL(!base.ContainsSet(nonDisjunctNonEmptySubSet));
    WD_TEST_BOOL(!nonDisjunctNonEmptySubSet.ContainsSet(base));

    // Union
    {
      wdHashSet<wdUInt32> res;

      res.Union(base);
      WD_TEST_BOOL(res.ContainsSet(base));
      WD_TEST_BOOL(base.ContainsSet(res));
      res.Union(subSet);
      WD_TEST_BOOL(res.ContainsSet(base));
      WD_TEST_BOOL(res.ContainsSet(subSet));
      WD_TEST_BOOL(base.ContainsSet(res));
      res.Union(superSet);
      WD_TEST_BOOL(res.ContainsSet(base));
      WD_TEST_BOOL(res.ContainsSet(subSet));
      WD_TEST_BOOL(res.ContainsSet(superSet));
      WD_TEST_BOOL(superSet.ContainsSet(res));
    }

    // Difference
    {
      wdHashSet<wdUInt32> res;
      res.Union(base);
      res.Difference(empty);
      WD_TEST_BOOL(res.ContainsSet(base));
      WD_TEST_BOOL(base.ContainsSet(res));
      res.Difference(disjunct);
      WD_TEST_BOOL(res.ContainsSet(base));
      WD_TEST_BOOL(base.ContainsSet(res));
      res.Difference(subSet);
      WD_TEST_INT(res.GetCount(), 1);
      WD_TEST_BOOL(res.Contains(3));
    }

    // Intersection
    {
      wdHashSet<wdUInt32> res;
      res.Union(base);
      res.Intersection(disjunct);
      WD_TEST_BOOL(res.IsEmpty());
      res.Union(base);
      res.Intersection(subSet);
      WD_TEST_BOOL(base.ContainsSet(subSet));
      WD_TEST_BOOL(res.ContainsSet(subSet));
      WD_TEST_BOOL(subSet.ContainsSet(res));
      res.Intersection(superSet);
      WD_TEST_BOOL(superSet.ContainsSet(res));
      WD_TEST_BOOL(res.ContainsSet(subSet));
      WD_TEST_BOOL(subSet.ContainsSet(res));
      res.Intersection(empty);
      WD_TEST_BOOL(res.IsEmpty());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator==/!=")
  {
    wdStaticArray<wdInt32, 64> keys[2];

    for (wdUInt32 i = 0; i < 64; ++i)
    {
      keys[0].PushBack(rand());
    }

    keys[1] = keys[0];

    wdHashSet<wdInt32> t[2];

    for (wdUInt32 i = 0; i < 2; ++i)
    {
      while (!keys[i].IsEmpty())
      {
        const wdUInt32 uiIndex = rand() % keys[i].GetCount();
        const wdInt32 key = keys[i][uiIndex];
        t[i].Insert(key);

        keys[i].RemoveAtAndSwap(uiIndex);
      }
    }

    WD_TEST_BOOL(t[0] == t[1]);

    t[0].Insert(32);
    WD_TEST_BOOL(t[0] != t[1]);

    t[1].Insert(32);
    WD_TEST_BOOL(t[0] == t[1]);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "CompatibleKeyType")
  {
    wdProxyAllocator testAllocator("Test", wdFoundation::GetDefaultAllocator());
    wdLocalAllocatorWrapper allocWrapper(&testAllocator);
    using TestString = wdHybridString<32, wdLocalAllocatorWrapper>;

    wdHashSet<TestString> stringSet;
    const char* szChar = "VeryLongStringDefinitelyMoreThan32Chars1111elf!!!!";
    const char* szString = "AnotherVeryLongStringThisTimeUsedForStringView!!!!";
    wdStringView sView(szString);
    wdStringBuilder sBuilder("BuilderAlsoNeedsToBeAVeryLongStringToTriggerAllocation");
    wdString sString("String");
    WD_TEST_BOOL(!stringSet.Insert(szChar));
    WD_TEST_BOOL(!stringSet.Insert(sView));
    WD_TEST_BOOL(!stringSet.Insert(sBuilder));
    WD_TEST_BOOL(!stringSet.Insert(sString));
    WD_TEST_BOOL(stringSet.Insert(szString));

    wdUInt64 oldAllocCount = testAllocator.GetStats().m_uiNumAllocations;

    WD_TEST_BOOL(stringSet.Contains(szChar));
    WD_TEST_BOOL(stringSet.Contains(sView));
    WD_TEST_BOOL(stringSet.Contains(sBuilder));
    WD_TEST_BOOL(stringSet.Contains(sString));

    WD_TEST_INT(testAllocator.GetStats().m_uiNumAllocations, oldAllocCount);

    WD_TEST_BOOL(stringSet.Remove(szChar));
    WD_TEST_BOOL(stringSet.Remove(sView));
    WD_TEST_BOOL(stringSet.Remove(sBuilder));
    WD_TEST_BOOL(stringSet.Remove(sString));

    WD_TEST_INT(testAllocator.GetStats().m_uiNumAllocations, oldAllocCount);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Swap")
  {
    wdStringBuilder tmp;
    wdHashSet<wdString> set1;
    wdHashSet<wdString> set2;

    for (wdUInt32 i = 0; i < 1000; ++i)
    {
      tmp.Format("stuff{}bla", i);
      set1.Insert(tmp);

      tmp.Format("{0}{0}{0}", i);
      set2.Insert(tmp);
    }

    set1.Swap(set2);

    for (wdUInt32 i = 0; i < 1000; ++i)
    {
      tmp.Format("stuff{}bla", i);
      WD_TEST_BOOL(set2.Contains(tmp));

      tmp.Format("{0}{0}{0}", i);
      WD_TEST_BOOL(set1.Contains(tmp));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "foreach")
  {
    wdStringBuilder tmp;
    wdHashSet<wdString> set;
    wdHashSet<wdString> set2;

    for (wdUInt32 i = 0; i < 1000; ++i)
    {
      tmp.Format("stuff{}bla", i);
      set.Insert(tmp);
    }

    WD_TEST_INT(set.GetCount(), 1000);

    set2 = set;
    WD_TEST_INT(set2.GetCount(), set.GetCount());

    for (wdHashSet<wdString>::ConstIterator it = begin(set); it != end(set); ++it)
    {
      const wdString& k = it.Key();
      set2.Remove(k);
    }

    WD_TEST_BOOL(set2.IsEmpty());
    set2 = set;

    for (auto key : set)
    {
      set2.Remove(key);
    }

    WD_TEST_BOOL(set2.IsEmpty());
  }
}
