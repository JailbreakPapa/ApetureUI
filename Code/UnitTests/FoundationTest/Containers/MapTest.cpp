#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Containers/Map.h>
#include <Foundation/Memory/CommonAllocators.h>
#include <Foundation/Strings/String.h>
#include <algorithm>
#include <iterator>

WD_CREATE_SIMPLE_TEST(Containers, Map)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Iterator")
  {
    wdMap<wdUInt32, wdUInt32> m;
    for (wdUInt32 i = 0; i < 1000; ++i)
      m[i] = i + 1;

    // WD_TEST_INT(std::find(begin(m), end(m), 500).Key(), 499);

    auto itfound = std::find_if(begin(m), end(m), [](wdMap<wdUInt32, wdUInt32>::ConstIterator val) { return val.Value() == 500; });

    // WD_TEST_BOOL(std::find(begin(m), end(m), 500) == itfound);

    wdUInt32 prev = begin(m).Key();
    for (auto it : m)
    {
      WD_TEST_BOOL(it.Value() >= prev);
      prev = it.Value();
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor")
  {
    wdMap<wdUInt32, wdUInt32> m;
    wdMap<wdConstructionCounter, wdUInt32> m2;
    wdMap<wdConstructionCounter, wdConstructionCounter> m3;
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsEmpty")
  {
    wdMap<wdUInt32, wdUInt32> m;
    WD_TEST_BOOL(m.IsEmpty());

    m[1] = 2;
    WD_TEST_BOOL(!m.IsEmpty());

    m.Clear();
    WD_TEST_BOOL(m.IsEmpty());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetCount")
  {
    wdMap<wdUInt32, wdUInt32> m;
    WD_TEST_INT(m.GetCount(), 0);

    m[0] = 1;
    WD_TEST_INT(m.GetCount(), 1);

    m[1] = 2;
    WD_TEST_INT(m.GetCount(), 2);

    m[2] = 3;
    WD_TEST_INT(m.GetCount(), 3);

    m[0] = 1;
    WD_TEST_INT(m.GetCount(), 3);

    m.Clear();
    WD_TEST_INT(m.GetCount(), 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Clear")
  {
    WD_TEST_BOOL(wdConstructionCounter::HasAllDestructed());

    {
      wdMap<wdUInt32, wdConstructionCounter> m1;
      m1[0] = wdConstructionCounter(1);
      WD_TEST_BOOL(wdConstructionCounter::HasDone(3, 2)); // for inserting new elements 2 temporaries are created (and destroyed)

      m1[1] = wdConstructionCounter(3);
      WD_TEST_BOOL(wdConstructionCounter::HasDone(3, 2)); // for inserting new elements 2 temporaries are created (and destroyed)

      m1[0] = wdConstructionCounter(2);
      WD_TEST_BOOL(wdConstructionCounter::HasDone(1, 1)); // nothing new to create, so only the one temporary is used

      m1.Clear();
      WD_TEST_BOOL(wdConstructionCounter::HasDone(0, 2));
      WD_TEST_BOOL(wdConstructionCounter::HasAllDestructed());
    }

    {
      wdMap<wdConstructionCounter, wdUInt32> m1;
      m1[wdConstructionCounter(0)] = 1;
      WD_TEST_BOOL(wdConstructionCounter::HasDone(2, 1)); // one temporary

      m1[wdConstructionCounter(1)] = 3;
      WD_TEST_BOOL(wdConstructionCounter::HasDone(2, 1)); // one temporary

      m1[wdConstructionCounter(0)] = 2;
      WD_TEST_BOOL(wdConstructionCounter::HasDone(1, 1)); // nothing new to create, so only the one temporary is used

      m1.Clear();
      WD_TEST_BOOL(wdConstructionCounter::HasDone(0, 2));
      WD_TEST_BOOL(wdConstructionCounter::HasAllDestructed());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Insert")
  {
    wdMap<wdUInt32, wdUInt32> m;

    WD_TEST_BOOL(m.GetHeapMemoryUsage() == 0);

    WD_TEST_BOOL(m.Insert(1, 10).IsValid());
    WD_TEST_BOOL(m.Insert(1, 10).IsValid());
    m.Insert(3, 30);
    auto it7 = m.Insert(7, 70);
    m.Insert(9, 90);
    m.Insert(4, 40);
    m.Insert(2, 20);
    m.Insert(8, 80);
    m.Insert(5, 50);
    m.Insert(6, 60);

    WD_TEST_BOOL(m.Insert(7, 70).Value() == 70);
    WD_TEST_BOOL(m.Insert(7, 70) == it7);

    WD_TEST_BOOL(m.GetHeapMemoryUsage() >= sizeof(wdUInt32) * 2 * 9);

    WD_TEST_INT(m[1], 10);
    WD_TEST_INT(m[2], 20);
    WD_TEST_INT(m[3], 30);
    WD_TEST_INT(m[4], 40);
    WD_TEST_INT(m[5], 50);
    WD_TEST_INT(m[6], 60);
    WD_TEST_INT(m[7], 70);
    WD_TEST_INT(m[8], 80);
    WD_TEST_INT(m[9], 90);

    WD_TEST_INT(m.GetCount(), 9);

    for (wdUInt32 i = 0; i < 1000000; ++i)
      m[i] = i;

    WD_TEST_BOOL(m.GetHeapMemoryUsage() >= sizeof(wdUInt32) * 2 * 1000000);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Find")
  {
    wdMap<wdUInt32, wdUInt32> m;

    for (wdInt32 i = 0; i < 1000; ++i)
      m[i] = i * 10;

    for (wdInt32 i = 1000 - 1; i >= 0; --i)
      WD_TEST_INT(m.Find(i).Value(), i * 10);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetValue/TryGetValue")
  {
    wdMap<wdUInt32, wdUInt32> m;

    for (wdInt32 i = 0; i < 100; ++i)
      m[i] = i * 10;

    for (wdInt32 i = 100 - 1; i >= 0; --i)
    {
      WD_TEST_INT(*m.GetValue(i), i * 10);

      wdUInt32 v = 0;
      WD_TEST_BOOL(m.TryGetValue(i, v));
      WD_TEST_INT(v, i * 10);

      wdUInt32* pV = nullptr;
      WD_TEST_BOOL(m.TryGetValue(i, pV));
      WD_TEST_INT(*pV, i * 10);
    }

    WD_TEST_BOOL(m.GetValue(101) == nullptr);

    wdUInt32 v = 0;
    WD_TEST_BOOL(m.TryGetValue(101, v) == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetValue/TryGetValue (const)")
  {
    wdMap<wdUInt32, wdUInt32> m;

    for (wdInt32 i = 0; i < 100; ++i)
      m[i] = i * 10;

    const wdMap<wdUInt32, wdUInt32>& mConst = m;

    for (wdInt32 i = 100 - 1; i >= 0; --i)
    {
      WD_TEST_INT(*mConst.GetValue(i), i * 10);

      wdUInt32 v = 0;
      WD_TEST_BOOL(m.TryGetValue(i, v));
      WD_TEST_INT(v, i * 10);

      wdUInt32* pV = nullptr;
      WD_TEST_BOOL(m.TryGetValue(i, pV));
      WD_TEST_INT(*pV, i * 10);
    }

    WD_TEST_BOOL(mConst.GetValue(101) == nullptr);

    wdUInt32 v = 0;
    WD_TEST_BOOL(mConst.TryGetValue(101, v) == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetValueOrDefault")
  {
    wdMap<wdUInt32, wdUInt32> m;

    for (wdInt32 i = 0; i < 100; ++i)
      m[i] = i * 10;

    for (wdInt32 i = 100 - 1; i >= 0; --i)
      WD_TEST_INT(m.GetValueOrDefault(i, 999), i * 10);

    WD_TEST_BOOL(m.GetValueOrDefault(101, 999) == 999);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Contains")
  {
    wdMap<wdUInt32, wdUInt32> m;

    for (wdInt32 i = 0; i < 1000; i += 2)
      m[i] = i * 10;

    for (wdInt32 i = 0; i < 1000; i += 2)
    {
      WD_TEST_BOOL(m.Contains(i));
      WD_TEST_BOOL(!m.Contains(i + 1));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "FindOrAdd")
  {
    wdMap<wdUInt32, wdUInt32> m;

    for (wdInt32 i = 0; i < 1000; ++i)
    {
      bool bExisted = true;
      m.FindOrAdd(i, &bExisted).Value() = i * 10;
      WD_TEST_BOOL(!bExisted);
    }

    for (wdInt32 i = 1000 - 1; i >= 0; --i)
    {
      bool bExisted = false;
      WD_TEST_INT(m.FindOrAdd(i, &bExisted).Value(), i * 10);
      WD_TEST_BOOL(bExisted);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator[]")
  {
    wdMap<wdUInt32, wdUInt32> m;

    for (wdInt32 i = 0; i < 1000; ++i)
      m[i] = i * 10;

    for (wdInt32 i = 1000 - 1; i >= 0; --i)
      WD_TEST_INT(m[i], i * 10);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Remove (non-existing)")
  {
    wdMap<wdUInt32, wdUInt32> m;

    for (wdInt32 i = 0; i < 1000; ++i)
    {
      WD_TEST_BOOL(!m.Remove(i));
    }

    for (wdInt32 i = 0; i < 1000; ++i)
      m[i] = i * 10;

    for (wdInt32 i = 0; i < 1000; ++i)
    {
      WD_TEST_BOOL(m.Remove(i + 500) == (i < 500));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Remove (Iterator)")
  {
    wdMap<wdUInt32, wdUInt32> m;

    for (wdInt32 i = 0; i < 1000; ++i)
      m[i] = i * 10;

    for (wdInt32 i = 0; i < 1000 - 1; ++i)
    {
      wdMap<wdUInt32, wdUInt32>::Iterator itNext = m.Remove(m.Find(i));
      WD_TEST_BOOL(!m.Find(i).IsValid());
      WD_TEST_BOOL(itNext.Key() == i + 1);

      WD_TEST_INT(m.GetCount(), 1000 - 1 - i);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Remove (Key)")
  {
    wdMap<wdUInt32, wdUInt32> m;

    for (wdInt32 i = 0; i < 1000; ++i)
      m[i] = i * 10;

    for (wdInt32 i = 0; i < 1000; ++i)
    {
      WD_TEST_BOOL(m.Remove(i));
      WD_TEST_BOOL(!m.Find(i).IsValid());

      WD_TEST_INT(m.GetCount(), 1000 - 1 - i);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator=")
  {
    wdMap<wdUInt32, wdUInt32> m, m2;

    for (wdInt32 i = 0; i < 1000; ++i)
      m[i] = i * 10;

    m2 = m;

    for (wdInt32 i = 1000 - 1; i >= 0; --i)
      WD_TEST_INT(m2[i], i * 10);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Copy Constructor")
  {
    wdMap<wdUInt32, wdUInt32> m;

    for (wdInt32 i = 0; i < 1000; ++i)
      m[i] = i * 10;

    wdMap<wdUInt32, wdUInt32> m2(m);

    for (wdInt32 i = 1000 - 1; i >= 0; --i)
      WD_TEST_INT(m2[i], i * 10);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetIterator / Forward Iteration")
  {
    wdMap<wdUInt32, wdUInt32> m;

    for (wdInt32 i = 0; i < 1000; ++i)
      m[i] = i * 10;

    wdInt32 i = 0;
    for (wdMap<wdUInt32, wdUInt32>::Iterator it = m.GetIterator(); it.IsValid(); ++it)
    {
      WD_TEST_INT(it.Key(), i);
      WD_TEST_INT(it.Value(), i * 10);
      ++i;
    }

    WD_TEST_INT(i, 1000);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetIterator / Forward Iteration (const)")
  {
    wdMap<wdUInt32, wdUInt32> m;

    for (wdInt32 i = 0; i < 1000; ++i)
      m[i] = i * 10;

    const wdMap<wdUInt32, wdUInt32> m2(m);

    wdInt32 i = 0;
    for (wdMap<wdUInt32, wdUInt32>::ConstIterator it = m2.GetIterator(); it.IsValid(); ++it)
    {
      WD_TEST_INT(it.Key(), i);
      WD_TEST_INT(it.Value(), i * 10);
      ++i;
    }

    WD_TEST_INT(i, 1000);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetLastIterator / Backward Iteration")
  {
    wdMap<wdUInt32, wdUInt32> m;

    for (wdInt32 i = 0; i < 1000; ++i)
      m[i] = i * 10;

    wdInt32 i = 1000 - 1;
    for (wdMap<wdUInt32, wdUInt32>::Iterator it = m.GetLastIterator(); it.IsValid(); --it)
    {
      WD_TEST_INT(it.Key(), i);
      WD_TEST_INT(it.Value(), i * 10);
      --i;
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetLastIterator / Backward Iteration (const)")
  {
    wdMap<wdUInt32, wdUInt32> m;

    for (wdInt32 i = 0; i < 1000; ++i)
      m[i] = i * 10;

    const wdMap<wdUInt32, wdUInt32> m2(m);

    wdInt32 i = 1000 - 1;
    for (wdMap<wdUInt32, wdUInt32>::ConstIterator it = m2.GetLastIterator(); it.IsValid(); --it)
    {
      WD_TEST_INT(it.Key(), i);
      WD_TEST_INT(it.Value(), i * 10);
      --i;
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "LowerBound")
  {
    wdMap<wdInt32, wdInt32> m, m2;

    m[0] = 0;
    m[3] = 30;
    m[7] = 70;
    m[9] = 90;

    WD_TEST_INT(m.LowerBound(-1).Key(), 0);
    WD_TEST_INT(m.LowerBound(0).Key(), 0);
    WD_TEST_INT(m.LowerBound(1).Key(), 3);
    WD_TEST_INT(m.LowerBound(2).Key(), 3);
    WD_TEST_INT(m.LowerBound(3).Key(), 3);
    WD_TEST_INT(m.LowerBound(4).Key(), 7);
    WD_TEST_INT(m.LowerBound(5).Key(), 7);
    WD_TEST_INT(m.LowerBound(6).Key(), 7);
    WD_TEST_INT(m.LowerBound(7).Key(), 7);
    WD_TEST_INT(m.LowerBound(8).Key(), 9);
    WD_TEST_INT(m.LowerBound(9).Key(), 9);

    WD_TEST_BOOL(!m.LowerBound(10).IsValid());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "UpperBound")
  {
    wdMap<wdInt32, wdInt32> m, m2;

    m[0] = 0;
    m[3] = 30;
    m[7] = 70;
    m[9] = 90;

    WD_TEST_INT(m.UpperBound(-1).Key(), 0);
    WD_TEST_INT(m.UpperBound(0).Key(), 3);
    WD_TEST_INT(m.UpperBound(1).Key(), 3);
    WD_TEST_INT(m.UpperBound(2).Key(), 3);
    WD_TEST_INT(m.UpperBound(3).Key(), 7);
    WD_TEST_INT(m.UpperBound(4).Key(), 7);
    WD_TEST_INT(m.UpperBound(5).Key(), 7);
    WD_TEST_INT(m.UpperBound(6).Key(), 7);
    WD_TEST_INT(m.UpperBound(7).Key(), 9);
    WD_TEST_INT(m.UpperBound(8).Key(), 9);
    WD_TEST_BOOL(!m.UpperBound(9).IsValid());
    WD_TEST_BOOL(!m.UpperBound(10).IsValid());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Insert / Remove")
  {
    // Tests whether reusing of elements makes problems

    wdMap<wdInt32, wdInt32> m;

    for (wdUInt32 r = 0; r < 5; ++r)
    {
      // Insert
      for (wdUInt32 i = 0; i < 10000; ++i)
        m.Insert(i, i * 10);

      WD_TEST_INT(m.GetCount(), 10000);

      // Remove
      for (wdUInt32 i = 0; i < 5000; ++i)
        WD_TEST_BOOL(m.Remove(i));

      // Insert others
      for (wdUInt32 j = 1; j < 1000; ++j)
        m.Insert(20000 * j, j);

      // Remove
      for (wdUInt32 i = 0; i < 5000; ++i)
        WD_TEST_BOOL(m.Remove(5000 + i));

      // Remove others
      for (wdUInt32 j = 1; j < 1000; ++j)
      {
        WD_TEST_BOOL(m.Find(20000 * j).IsValid());
        WD_TEST_BOOL(m.Remove(20000 * j));
      }
    }

    WD_TEST_BOOL(m.IsEmpty());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator == / !=")
  {
    wdMap<wdUInt32, wdUInt32> m, m2;

    WD_TEST_BOOL(m == m2);

    for (wdInt32 i = 0; i < 1000; ++i)
      m[i] = i * 10;

    WD_TEST_BOOL(m != m2);

    m2 = m;

    WD_TEST_BOOL(m == m2);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "CompatibleKeyType")
  {
    {
      wdMap<wdString, int> stringTable;
      const char* szChar = "Char";
      const char* szString = "ViewBla";
      wdStringView sView(szString, szString + 4);
      wdStringBuilder sBuilder("Builder");
      wdString sString("String");
      stringTable.Insert(szChar, 1);
      stringTable.Insert(sView, 2);
      stringTable.Insert(sBuilder, 3);
      stringTable.Insert(sString, 4);

      WD_TEST_BOOL(stringTable.Contains(szChar));
      WD_TEST_BOOL(stringTable.Contains(sView));
      WD_TEST_BOOL(stringTable.Contains(sBuilder));
      WD_TEST_BOOL(stringTable.Contains(sString));

      WD_TEST_INT(*stringTable.GetValue(szChar), 1);
      WD_TEST_INT(*stringTable.GetValue(sView), 2);
      WD_TEST_INT(*stringTable.GetValue(sBuilder), 3);
      WD_TEST_INT(*stringTable.GetValue(sString), 4);

      WD_TEST_BOOL(stringTable.Remove(szChar));
      WD_TEST_BOOL(stringTable.Remove(sView));
      WD_TEST_BOOL(stringTable.Remove(sBuilder));
      WD_TEST_BOOL(stringTable.Remove(sString));
    }

    // dynamic array as key, check for allocations in comparisons
    {
      wdProxyAllocator testAllocator("Test", wdFoundation::GetDefaultAllocator());
      wdLocalAllocatorWrapper allocWrapper(&testAllocator);
      using TestDynArray = wdDynamicArray<int, wdLocalAllocatorWrapper>;
      TestDynArray a;
      TestDynArray b;
      for (int i = 0; i < 10; ++i)
      {
        a.PushBack(i);
        b.PushBack(i * 2);
      }

      wdMap<TestDynArray, int> arrayTable;
      arrayTable.Insert(a, 1);
      arrayTable.Insert(b, 2);

      wdArrayPtr<const int> aPtr = a.GetArrayPtr();
      wdArrayPtr<const int> bPtr = b.GetArrayPtr();

      wdUInt64 oldAllocCount = testAllocator.GetStats().m_uiNumAllocations;

      bool existed;
      auto it = arrayTable.FindOrAdd(aPtr, &existed);
      WD_TEST_BOOL(existed);

      WD_TEST_INT(testAllocator.GetStats().m_uiNumAllocations, oldAllocCount);

      WD_TEST_BOOL(arrayTable.Contains(aPtr));
      WD_TEST_BOOL(arrayTable.Contains(bPtr));
      WD_TEST_BOOL(arrayTable.Contains(a));

      WD_TEST_INT(testAllocator.GetStats().m_uiNumAllocations, oldAllocCount);

      WD_TEST_INT(*arrayTable.GetValue(aPtr), 1);
      WD_TEST_INT(*arrayTable.GetValue(bPtr), 2);
      WD_TEST_INT(*arrayTable.GetValue(a), 1);

      WD_TEST_INT(testAllocator.GetStats().m_uiNumAllocations, oldAllocCount);

      WD_TEST_BOOL(arrayTable.Remove(aPtr));
      WD_TEST_BOOL(arrayTable.Remove(bPtr));

      WD_TEST_INT(testAllocator.GetStats().m_uiNumAllocations, oldAllocCount);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Swap")
  {
    wdStringBuilder tmp;
    wdMap<wdString, wdInt32> map1;
    wdMap<wdString, wdInt32> map2;

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

  constexpr wdUInt32 uiMapSize = sizeof(wdMap<wdString, wdInt32>);

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Swap")
  {
    wdUInt8 map1Mem[uiMapSize];
    wdUInt8 map2Mem[uiMapSize];
    wdMemoryUtils::PatternFill(map1Mem, 0xCA, uiMapSize);
    wdMemoryUtils::PatternFill(map2Mem, 0xCA, uiMapSize);

    wdStringBuilder tmp;
    wdMap<wdString, wdInt32>* map1 = new (map1Mem)(wdMap<wdString, wdInt32>);
    wdMap<wdString, wdInt32>* map2 = new (map2Mem)(wdMap<wdString, wdInt32>);

    for (wdUInt32 i = 0; i < 1000; ++i)
    {
      tmp.Format("stuff{}bla", i);
      map1->Insert(tmp, i);

      tmp.Format("{0}{0}{0}", i);
      map2->Insert(tmp, i);
    }

    map1->Swap(*map2);

    // test swapped elements
    for (wdUInt32 i = 0; i < 1000; ++i)
    {
      tmp.Format("stuff{}bla", i);
      WD_TEST_BOOL(map2->Contains(tmp));
      WD_TEST_INT((*map2)[tmp], i);

      tmp.Format("{0}{0}{0}", i);
      WD_TEST_BOOL(map1->Contains(tmp));
      WD_TEST_INT((*map1)[tmp], i);
    }

    // test iterators after swap
    {
      for (auto it : *map1)
      {
        WD_TEST_BOOL(!map2->Contains(it.Key()));
      }

      for (auto it : *map2)
      {
        WD_TEST_BOOL(!map1->Contains(it.Key()));
      }
    }

    // due to a compiler bug in VS 2017, PatternFill cannot be called here, because it will move the memset BEFORE the destructor call!
    // seems to be fixed in VS 2019 though

    map1->~wdMap<wdString, wdInt32>();
    // wdMemoryUtils::PatternFill(map1Mem, 0xBA, uiSetSize);

    map2->~wdMap<wdString, wdInt32>();
    wdMemoryUtils::PatternFill(map2Mem, 0xBA, uiMapSize);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Swap Empty")
  {
    wdUInt8 map1Mem[uiMapSize];
    wdUInt8 map2Mem[uiMapSize];
    wdMemoryUtils::PatternFill(map1Mem, 0xCA, uiMapSize);
    wdMemoryUtils::PatternFill(map2Mem, 0xCA, uiMapSize);

    wdStringBuilder tmp;
    wdMap<wdString, wdInt32>* map1 = new (map1Mem)(wdMap<wdString, wdInt32>);
    wdMap<wdString, wdInt32>* map2 = new (map2Mem)(wdMap<wdString, wdInt32>);

    for (wdUInt32 i = 0; i < 100; ++i)
    {
      tmp.Format("stuff{}bla", i);
      map1->Insert(tmp, i);
    }

    map1->Swap(*map2);
    WD_TEST_BOOL(map1->IsEmpty());

    map1->~wdMap<wdString, wdInt32>();
    wdMemoryUtils::PatternFill(map1Mem, 0xBA, uiMapSize);

    // test swapped elements
    for (wdUInt32 i = 0; i < 100; ++i)
    {
      tmp.Format("stuff{}bla", i);
      WD_TEST_BOOL(map2->Contains(tmp));
    }

    // test iterators after swap
    {
      for (auto it : *map2)
      {
        WD_TEST_BOOL(map2->Contains(it.Key()));
      }
    }

    map2->~wdMap<wdString, wdInt32>();
    wdMemoryUtils::PatternFill(map2Mem, 0xBA, uiMapSize);
  }
}
