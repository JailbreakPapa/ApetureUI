#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Containers/List.h>

WD_CREATE_SIMPLE_TEST(Containers, List)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor") { wdList<wdInt32> l; }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "PushBack() / PeekBack")
  {
    wdList<wdInt32> l;
    l.PushBack();

    WD_TEST_INT(l.GetCount(), 1);
    WD_TEST_INT(l.PeekBack(), 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "PushBack(i) / GetCount")
  {
    wdList<wdInt32> l;
    WD_TEST_BOOL(l.GetHeapMemoryUsage() == 0);

    for (wdUInt32 i = 0; i < 1000; ++i)
    {
      l.PushBack(i);

      WD_TEST_INT(l.GetCount(), i + 1);
      WD_TEST_INT(l.PeekBack(), i);
    }

    WD_TEST_BOOL(l.GetHeapMemoryUsage() >= sizeof(wdInt32) * 1000);

    wdUInt32 i = 0;
    for (wdList<wdInt32>::Iterator it = l.GetIterator(); it != l.GetEndIterator(); ++it)
    {
      WD_TEST_INT(*it, i);
      ++i;
    }

    WD_TEST_INT(i, 1000);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "PopBack()")
  {
    wdList<wdInt32> l;

    wdInt32 i = 0;
    for (; i < 1000; ++i)
      l.PushBack(i);

    while (!l.IsEmpty())
    {
      --i;
      WD_TEST_INT(l.PeekBack(), i);
      l.PopBack();
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "PushFront() / PeekFront")
  {
    wdList<wdInt32> l;
    l.PushFront();

    WD_TEST_INT(l.GetCount(), 1);
    WD_TEST_INT(l.PeekFront(), 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "PushFront(i) / PeekFront")
  {
    wdList<wdInt32> l;

    for (wdUInt32 i = 0; i < 1000; ++i)
    {
      l.PushFront(i);

      WD_TEST_INT(l.GetCount(), i + 1);
      WD_TEST_INT(l.PeekFront(), i);
    }

    wdUInt32 i2 = 1000;
    for (wdList<wdInt32>::Iterator it = l.GetIterator(); it != l.GetEndIterator(); ++it)
    {
      --i2;
      WD_TEST_INT(*it, i2);
    }

    WD_TEST_INT(i2, 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "PopFront()")
  {
    wdList<wdInt32> l;

    wdInt32 i = 0;
    for (; i < 1000; ++i)
      l.PushFront(i);

    while (!l.IsEmpty())
    {
      --i;
      WD_TEST_INT(l.PeekFront(), i);
      l.PopFront();
    }
  }


  WD_TEST_BLOCK(wdTestBlock::Enabled, "Clear / IsEmpty")
  {
    wdList<wdInt32> l;

    WD_TEST_BOOL(l.IsEmpty());

    for (wdUInt32 i = 0; i < 1000; ++i)
      l.PushBack(i);

    WD_TEST_BOOL(!l.IsEmpty());

    l.Clear();
    WD_TEST_BOOL(l.IsEmpty());

    for (wdUInt32 i = 0; i < 1000; ++i)
    {
      l.PushBack(i);
      WD_TEST_BOOL(!l.IsEmpty());

      l.Clear();
      WD_TEST_BOOL(l.IsEmpty());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator=")
  {
    wdList<wdInt32> l, l2;

    for (wdUInt32 i = 0; i < 1000; ++i)
      l.PushBack(i);

    l2 = l;

    wdUInt32 i = 0;
    for (wdList<wdInt32>::Iterator it = l2.GetIterator(); it != l2.GetEndIterator(); ++it)
    {
      WD_TEST_INT(*it, i);
      ++i;
    }

    WD_TEST_INT(i, 1000);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Copy Constructor")
  {
    wdList<wdInt32> l;

    for (wdUInt32 i = 0; i < 1000; ++i)
      l.PushBack(i);

    wdList<wdInt32> l2(l);

    wdUInt32 i = 0;
    for (wdList<wdInt32>::Iterator it = l2.GetIterator(); it != l2.GetEndIterator(); ++it)
    {
      WD_TEST_INT(*it, i);
      ++i;
    }

    WD_TEST_INT(i, 1000);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetCount")
  {
    wdList<wdInt32> l;
    l.SetCount(1000);
    WD_TEST_INT(l.GetCount(), 1000);

    wdInt32 i = 1;
    for (wdList<wdInt32>::Iterator it = l.GetIterator(); it != l.GetEndIterator(); ++it)
    {
      WD_TEST_INT(*it, 0);
      *it = i;
      ++i;
    }

    l.SetCount(2000);
    i = 1;
    for (wdList<wdInt32>::Iterator it = l.GetIterator(); it != l.GetEndIterator(); ++it)
    {
      if (i > 1000)
        WD_TEST_INT(*it, 0);
      else
        WD_TEST_INT(*it, i);

      ++i;
    }

    l.SetCount(500);
    i = 1;
    for (wdList<wdInt32>::Iterator it = l.GetIterator(); it != l.GetEndIterator(); ++it)
    {
      WD_TEST_INT(*it, i);
      ++i;
    }

    WD_TEST_INT(i, 501);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Insert(item)")
  {
    wdList<wdInt32> l;

    for (wdUInt32 i = 1; i < 1000; ++i)
      l.PushBack(i);

    // create an interleaved array of values of i and i+10000
    for (wdList<wdInt32>::Iterator it = l.GetIterator(); it != l.GetEndIterator(); ++it)
    {
      // insert before this element
      l.Insert(it, *it + 10000);
    }

    wdInt32 i = 1;
    for (wdList<wdInt32>::Iterator it = l.GetIterator(); it != l.GetEndIterator(); ++it)
    {
      WD_TEST_INT(*it, i + 10000);
      ++it;

      WD_TEST_BOOL(it.IsValid());
      WD_TEST_INT(*it, i);

      ++i;
    }

    WD_TEST_INT(i, 1000);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Remove(item)")
  {
    wdList<wdInt32> l;

    wdUInt32 i = 1;
    for (; i < 1000; ++i)
      l.PushBack(i);

    // create an interleaved array of values of i and i+10000
    for (wdList<wdInt32>::Iterator it = l.GetIterator(); it != l.GetEndIterator(); ++it)
    {
      // insert before this element
      l.Insert(it, *it + 10000);
    }

    // now remove every second element and only keep the larger values
    for (wdList<wdInt32>::Iterator it = l.GetLastIterator(); it.IsValid(); --it)
    {
      it = l.Remove(it);
      --it;
      --i;
      WD_TEST_INT(*it, i + 10000);
    }

    i = 1;
    for (wdList<wdInt32>::Iterator it = l.GetIterator(); it.IsValid(); ++it)
    {
      WD_TEST_INT(*it, i + 10000);
      ++i;
    }

    WD_TEST_INT(i, 1000);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Iterator::IsValid")
  {
    wdList<wdInt32> l;

    for (wdUInt32 i = 0; i < 1000; ++i)
      l.PushBack(i);

    wdUInt32 i = 0;
    for (wdList<wdInt32>::Iterator it = l.GetIterator(); it.IsValid(); ++it)
    {
      WD_TEST_INT(*it, i);
      ++i;
    }

    WD_TEST_BOOL(!l.GetEndIterator().IsValid());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Element Constructions / Destructions")
  {
    WD_TEST_BOOL(wdConstructionCounter::HasAllDestructed());

    wdList<wdConstructionCounter> l;

    WD_TEST_BOOL(wdConstructionCounter::HasAllDestructed());

    l.PushBack();
    WD_TEST_BOOL(wdConstructionCounter::HasDone(2, 1));

    l.PushBack(wdConstructionCounter(1));
    WD_TEST_BOOL(wdConstructionCounter::HasDone(2, 1));

    l.SetCount(4);
    WD_TEST_BOOL(wdConstructionCounter::HasDone(4, 2));

    l.Clear();
    WD_TEST_BOOL(wdConstructionCounter::HasDone(0, 4));

    WD_TEST_BOOL(wdConstructionCounter::HasAllDestructed());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator == / !=")
  {
    wdList<wdInt32> l, l2;

    WD_TEST_BOOL(l == l2);

    wdInt32 i = 0;
    for (; i < 1000; ++i)
      l.PushBack(i);

    WD_TEST_BOOL(l != l2);

    l2 = l;

    WD_TEST_BOOL(l == l2);
  }
}
