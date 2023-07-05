#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Containers/Set.h>
#include <Foundation/Memory/CommonAllocators.h>

WD_CREATE_SIMPLE_TEST(Containers, Set)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor")
  {
    wdSet<wdUInt32> m;
    wdSet<wdConstructionCounter, wdUInt32> m2;
    wdSet<wdConstructionCounter, wdConstructionCounter> m3;
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsEmpty")
  {
    wdSet<wdUInt32> m;
    WD_TEST_BOOL(m.IsEmpty());

    m.Insert(1);
    WD_TEST_BOOL(!m.IsEmpty());

    m.Clear();
    WD_TEST_BOOL(m.IsEmpty());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetCount")
  {
    wdSet<wdUInt32> m;
    WD_TEST_INT(m.GetCount(), 0);

    m.Insert(0);
    WD_TEST_INT(m.GetCount(), 1);

    m.Insert(1);
    WD_TEST_INT(m.GetCount(), 2);

    m.Insert(2);
    WD_TEST_INT(m.GetCount(), 3);

    m.Insert(1);
    WD_TEST_INT(m.GetCount(), 3);

    m.Clear();
    WD_TEST_INT(m.GetCount(), 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Clear")
  {
    WD_TEST_BOOL(wdConstructionCounter::HasAllDestructed());

    {
      wdSet<wdConstructionCounter> m1;
      m1.Insert(wdConstructionCounter(1));
      WD_TEST_BOOL(wdConstructionCounter::HasDone(2, 1));

      m1.Insert(wdConstructionCounter(3));
      WD_TEST_BOOL(wdConstructionCounter::HasDone(2, 1));

      m1.Insert(wdConstructionCounter(1));
      WD_TEST_BOOL(wdConstructionCounter::HasDone(1, 1)); // nothing new to create, so only the one temporary is used

      m1.Clear();
      WD_TEST_BOOL(wdConstructionCounter::HasDone(0, 2));
      WD_TEST_BOOL(wdConstructionCounter::HasAllDestructed());
    }

    {
      wdSet<wdConstructionCounter> m1;
      m1.Insert(wdConstructionCounter(0));
      WD_TEST_BOOL(wdConstructionCounter::HasDone(2, 1)); // one temporary

      m1.Insert(wdConstructionCounter(1));
      WD_TEST_BOOL(wdConstructionCounter::HasDone(2, 1)); // one temporary

      m1.Insert(wdConstructionCounter(0));
      WD_TEST_BOOL(wdConstructionCounter::HasDone(1, 1)); // nothing new to create, so only the one temporary is used

      m1.Clear();
      WD_TEST_BOOL(wdConstructionCounter::HasDone(0, 2));
      WD_TEST_BOOL(wdConstructionCounter::HasAllDestructed());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Insert")
  {
    wdSet<wdUInt32> m;
    WD_TEST_BOOL(m.GetHeapMemoryUsage() == 0);

    WD_TEST_BOOL(m.Insert(1).IsValid());
    WD_TEST_BOOL(m.Insert(1).IsValid());

    m.Insert(3);
    auto it7 = m.Insert(7);
    m.Insert(9);
    m.Insert(4);
    m.Insert(2);
    m.Insert(8);
    m.Insert(5);
    m.Insert(6);

    WD_TEST_BOOL(m.Insert(1).Key() == 1);
    WD_TEST_BOOL(m.Insert(3).Key() == 3);
    WD_TEST_BOOL(m.Insert(7) == it7);

    WD_TEST_BOOL(m.GetHeapMemoryUsage() >= sizeof(wdUInt32) * 1 * 9);

    WD_TEST_BOOL(m.Find(1).IsValid());
    WD_TEST_BOOL(m.Find(2).IsValid());
    WD_TEST_BOOL(m.Find(3).IsValid());
    WD_TEST_BOOL(m.Find(4).IsValid());
    WD_TEST_BOOL(m.Find(5).IsValid());
    WD_TEST_BOOL(m.Find(6).IsValid());
    WD_TEST_BOOL(m.Find(7).IsValid());
    WD_TEST_BOOL(m.Find(8).IsValid());
    WD_TEST_BOOL(m.Find(9).IsValid());

    WD_TEST_BOOL(!m.Find(0).IsValid());
    WD_TEST_BOOL(!m.Find(10).IsValid());

    WD_TEST_INT(m.GetCount(), 9);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Contains")
  {
    wdSet<wdUInt32> m;
    m.Insert(1);
    m.Insert(3);
    m.Insert(7);
    m.Insert(9);
    m.Insert(4);
    m.Insert(2);
    m.Insert(8);
    m.Insert(5);
    m.Insert(6);

    WD_TEST_BOOL(m.Contains(1));
    WD_TEST_BOOL(m.Contains(2));
    WD_TEST_BOOL(m.Contains(3));
    WD_TEST_BOOL(m.Contains(4));
    WD_TEST_BOOL(m.Contains(5));
    WD_TEST_BOOL(m.Contains(6));
    WD_TEST_BOOL(m.Contains(7));
    WD_TEST_BOOL(m.Contains(8));
    WD_TEST_BOOL(m.Contains(9));

    WD_TEST_BOOL(!m.Contains(0));
    WD_TEST_BOOL(!m.Contains(10));

    WD_TEST_INT(m.GetCount(), 9);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Set Operations")
  {
    wdSet<wdUInt32> base;
    base.Insert(1);
    base.Insert(3);
    base.Insert(5);

    wdSet<wdUInt32> empty;

    wdSet<wdUInt32> disjunct;
    disjunct.Insert(2);
    disjunct.Insert(4);
    disjunct.Insert(6);

    wdSet<wdUInt32> subSet;
    subSet.Insert(1);
    subSet.Insert(5);

    wdSet<wdUInt32> superSet;
    superSet.Insert(1);
    superSet.Insert(3);
    superSet.Insert(5);
    superSet.Insert(7);

    wdSet<wdUInt32> nonDisjunctNonEmptySubSet;
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
      wdSet<wdUInt32> res;

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
      wdSet<wdUInt32> res;
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
      wdSet<wdUInt32> res;
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

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Find")
  {
    wdSet<wdUInt32> m;

    for (wdInt32 i = 0; i < 1000; ++i)
      m.Insert(i);

    for (wdInt32 i = 1000 - 1; i >= 0; --i)
      WD_TEST_INT(m.Find(i).Key(), i);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Remove (non-existing)")
  {
    wdSet<wdUInt32> m;

    for (wdInt32 i = 0; i < 1000; ++i)
      WD_TEST_BOOL(!m.Remove(i));

    for (wdInt32 i = 0; i < 1000; ++i)
      m.Insert(i);

    for (wdInt32 i = 0; i < 1000; ++i)
      WD_TEST_BOOL(m.Remove(i + 500) == (i < 500));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Remove (Iterator)")
  {
    wdSet<wdUInt32> m;

    for (wdInt32 i = 0; i < 1000; ++i)
      m.Insert(i);

    for (wdInt32 i = 0; i < 1000 - 1; ++i)
    {
      wdSet<wdUInt32>::Iterator itNext = m.Remove(m.Find(i));
      WD_TEST_BOOL(!m.Find(i).IsValid());
      WD_TEST_BOOL(itNext.Key() == i + 1);

      WD_TEST_INT(m.GetCount(), 1000 - 1 - i);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Remove (Key)")
  {
    wdSet<wdUInt32> m;

    for (wdInt32 i = 0; i < 1000; ++i)
      m.Insert(i);

    for (wdInt32 i = 0; i < 1000; ++i)
    {
      WD_TEST_BOOL(m.Remove(i));
      WD_TEST_BOOL(!m.Find(i).IsValid());

      WD_TEST_INT(m.GetCount(), 1000 - 1 - i);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator=")
  {
    wdSet<wdUInt32> m, m2;

    for (wdInt32 i = 0; i < 1000; ++i)
      m.Insert(i);

    m2 = m;

    for (wdInt32 i = 1000 - 1; i >= 0; --i)
      WD_TEST_BOOL(m2.Find(i).IsValid());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Copy Constructor")
  {
    wdSet<wdUInt32> m;

    for (wdInt32 i = 0; i < 1000; ++i)
      m.Insert(i);

    wdSet<wdUInt32> m2(m);

    for (wdInt32 i = 1000 - 1; i >= 0; --i)
      WD_TEST_BOOL(m2.Find(i).IsValid());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetIterator / Forward Iteration")
  {
    wdSet<wdUInt32> m;

    for (wdInt32 i = 0; i < 1000; ++i)
      m.Insert(i);

    wdInt32 i = 0;
    for (wdSet<wdUInt32>::Iterator it = m.GetIterator(); it.IsValid(); ++it)
    {
      WD_TEST_INT(it.Key(), i);
      ++i;
    }

    WD_TEST_INT(i, 1000);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetIterator / Forward Iteration (const)")
  {
    wdSet<wdUInt32> m;

    for (wdInt32 i = 0; i < 1000; ++i)
      m.Insert(i);

    const wdSet<wdUInt32> m2(m);

    wdInt32 i = 0;
    for (wdSet<wdUInt32>::Iterator it = m2.GetIterator(); it.IsValid(); ++it)
    {
      WD_TEST_INT(it.Key(), i);
      ++i;
    }

    WD_TEST_INT(i, 1000);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetLastIterator / Backward Iteration")
  {
    wdSet<wdUInt32> m;

    for (wdInt32 i = 0; i < 1000; ++i)
      m.Insert(i);

    wdInt32 i = 1000 - 1;
    for (wdSet<wdUInt32>::Iterator it = m.GetLastIterator(); it.IsValid(); --it)
    {
      WD_TEST_INT(it.Key(), i);
      --i;
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetLastIterator / Backward Iteration (const)")
  {
    wdSet<wdUInt32> m;

    for (wdInt32 i = 0; i < 1000; ++i)
      m.Insert(i);

    const wdSet<wdUInt32> m2(m);

    wdInt32 i = 1000 - 1;
    for (wdSet<wdUInt32>::Iterator it = m2.GetLastIterator(); it.IsValid(); --it)
    {
      WD_TEST_INT(it.Key(), i);
      --i;
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "LowerBound")
  {
    wdSet<wdInt32> m, m2;

    m.Insert(0);
    m.Insert(3);
    m.Insert(7);
    m.Insert(9);

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
    wdSet<wdInt32> m, m2;

    m.Insert(0);
    m.Insert(3);
    m.Insert(7);
    m.Insert(9);

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

    wdSet<wdInt32> m;

    for (wdUInt32 r = 0; r < 5; ++r)
    {
      // Insert
      for (wdUInt32 i = 0; i < 10000; ++i)
        m.Insert(i);

      WD_TEST_INT(m.GetCount(), 10000);

      // Remove
      for (wdUInt32 i = 0; i < 5000; ++i)
        WD_TEST_BOOL(m.Remove(i));

      // Insert others
      for (wdUInt32 j = 1; j < 1000; ++j)
        m.Insert(20000 * j);

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

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Iterator")
  {
    wdSet<wdUInt32> m;
    for (wdUInt32 i = 0; i < 1000; ++i)
      m.Insert(i + 1);

    WD_TEST_INT(std::find(begin(m), end(m), 500).Key(), 500);

    auto itfound = std::find_if(begin(m), end(m), [](wdUInt32 uiVal) { return uiVal == 500; });

    WD_TEST_BOOL(std::find(begin(m), end(m), 500) == itfound);

    wdUInt32 prev = *begin(m);
    for (wdUInt32 val : m)
    {
      WD_TEST_BOOL(val >= prev);
      prev = val;
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator == / !=")
  {
    wdSet<wdUInt32> m, m2;

    WD_TEST_BOOL(m == m2);

    for (wdInt32 i = 0; i < 1000; ++i)
      m.Insert(i * 10);

    WD_TEST_BOOL(m != m2);

    m2 = m;

    WD_TEST_BOOL(m == m2);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "CompatibleKeyType")
  {
    {
      wdSet<wdString> stringSet;
      const char* szChar = "Char";
      const char* szString = "ViewBla";
      wdStringView sView(szString, szString + 4);
      wdStringBuilder sBuilder("Builder");
      wdString sString("String");
      stringSet.Insert(szChar);
      stringSet.Insert(sView);
      stringSet.Insert(sBuilder);
      stringSet.Insert(sString);

      WD_TEST_BOOL(stringSet.Contains(szChar));
      WD_TEST_BOOL(stringSet.Contains(sView));
      WD_TEST_BOOL(stringSet.Contains(sBuilder));
      WD_TEST_BOOL(stringSet.Contains(sString));

      WD_TEST_BOOL(stringSet.Remove(szChar));
      WD_TEST_BOOL(stringSet.Remove(sView));
      WD_TEST_BOOL(stringSet.Remove(sBuilder));
      WD_TEST_BOOL(stringSet.Remove(sString));
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

      wdSet<TestDynArray> arraySet;
      arraySet.Insert(a);
      arraySet.Insert(b);

      wdArrayPtr<const int> aPtr = a.GetArrayPtr();
      wdArrayPtr<const int> bPtr = b.GetArrayPtr();

      wdUInt64 oldAllocCount = testAllocator.GetStats().m_uiNumAllocations;

      WD_TEST_BOOL(arraySet.Contains(aPtr));
      WD_TEST_BOOL(arraySet.Contains(bPtr));
      WD_TEST_BOOL(arraySet.Contains(a));

      WD_TEST_INT(testAllocator.GetStats().m_uiNumAllocations, oldAllocCount);

      WD_TEST_BOOL(arraySet.Remove(aPtr));
      WD_TEST_BOOL(arraySet.Remove(bPtr));

      WD_TEST_INT(testAllocator.GetStats().m_uiNumAllocations, oldAllocCount);
    }
  }

  constexpr wdUInt32 uiSetSize = sizeof(wdSet<wdString>);

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Swap")
  {
    wdUInt8 set1Mem[uiSetSize];
    wdUInt8 set2Mem[uiSetSize];
    wdMemoryUtils::PatternFill(set1Mem, 0xCA, uiSetSize);
    wdMemoryUtils::PatternFill(set2Mem, 0xCA, uiSetSize);

    wdStringBuilder tmp;
    wdSet<wdString>* set1 = new (set1Mem)(wdSet<wdString>);
    wdSet<wdString>* set2 = new (set2Mem)(wdSet<wdString>);

    for (wdUInt32 i = 0; i < 1000; ++i)
    {
      tmp.Format("stuff{}bla", i);
      set1->Insert(tmp);

      tmp.Format("{0}{0}{0}", i);
      set2->Insert(tmp);
    }

    set1->Swap(*set2);

    // test swapped elements
    for (wdUInt32 i = 0; i < 1000; ++i)
    {
      tmp.Format("stuff{}bla", i);
      WD_TEST_BOOL(set2->Contains(tmp));

      tmp.Format("{0}{0}{0}", i);
      WD_TEST_BOOL(set1->Contains(tmp));
    }

    // test iterators after swap
    {
      for (const auto& element : *set1)
      {
        WD_TEST_BOOL(!set2->Contains(element));
      }

      for (const auto& element : *set2)
      {
        WD_TEST_BOOL(!set1->Contains(element));
      }
    }

    // due to a compiler bug in VS 2017, PatternFill cannot be called here, because it will move the memset BEFORE the destructor call!
    // seems to be fixed in VS 2019 though

    set1->~wdSet<wdString>();
    // wdMemoryUtils::PatternFill(set1Mem, 0xBA, uiSetSize);

    set2->~wdSet<wdString>();
    wdMemoryUtils::PatternFill(set2Mem, 0xBA, uiSetSize);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Swap Empty")
  {
    wdUInt8 set1Mem[uiSetSize];
    wdUInt8 set2Mem[uiSetSize];
    wdMemoryUtils::PatternFill(set1Mem, 0xCA, uiSetSize);
    wdMemoryUtils::PatternFill(set2Mem, 0xCA, uiSetSize);

    wdStringBuilder tmp;
    wdSet<wdString>* set1 = new (set1Mem)(wdSet<wdString>);
    wdSet<wdString>* set2 = new (set2Mem)(wdSet<wdString>);

    for (wdUInt32 i = 0; i < 100; ++i)
    {
      tmp.Format("stuff{}bla", i);
      set1->Insert(tmp);
    }

    set1->Swap(*set2);
    WD_TEST_BOOL(set1->IsEmpty());

    set1->~wdSet<wdString>();
    wdMemoryUtils::PatternFill(set1Mem, 0xBA, uiSetSize);

    // test swapped elements
    for (wdUInt32 i = 0; i < 100; ++i)
    {
      tmp.Format("stuff{}bla", i);
      WD_TEST_BOOL(set2->Contains(tmp));
    }

    // test iterators after swap
    {
      for (const auto& element : *set2)
      {
        WD_TEST_BOOL(set2->Contains(element));
      }
    }

    set2->~wdSet<wdString>();
    wdMemoryUtils::PatternFill(set2Mem, 0xBA, uiSetSize);
  }
}
