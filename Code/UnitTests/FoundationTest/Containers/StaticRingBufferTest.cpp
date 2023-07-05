#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Containers/StaticRingBuffer.h>

typedef wdConstructionCounter cc;

WD_CREATE_SIMPLE_TEST(Containers, StaticRingBuffer)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor")
  {
    WD_TEST_BOOL(wdConstructionCounter::HasAllDestructed());

    {
      wdStaticRingBuffer<wdInt32, 32> r1;
      wdStaticRingBuffer<wdInt32, 16> r2;
      wdStaticRingBuffer<cc, 2> r3;
    }

    WD_TEST_BOOL(wdConstructionCounter::HasAllDestructed());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Copy Constructor / Operator=")
  {
    WD_TEST_BOOL(wdConstructionCounter::HasAllDestructed());

    {
      wdStaticRingBuffer<cc, 16> r1;

      for (wdUInt32 i = 0; i < 16; ++i)
        r1.PushBack(cc(i));

      wdStaticRingBuffer<cc, 16> r2(r1);

      for (wdUInt32 i = 0; i < 16; ++i)
        WD_TEST_BOOL(r2[i] == cc(i));

      wdStaticRingBuffer<cc, 16> r3;
      r3 = r1;

      for (wdUInt32 i = 0; i < 16; ++i)
        WD_TEST_BOOL(r3[i] == cc(i));
    }

    WD_TEST_BOOL(wdConstructionCounter::HasAllDestructed());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Operator==")
  {
    WD_TEST_BOOL(wdConstructionCounter::HasAllDestructed());

    {
      wdStaticRingBuffer<cc, 16> r1;

      for (wdUInt32 i = 0; i < 16; ++i)
        r1.PushBack(cc(i));

      wdStaticRingBuffer<cc, 16> r2(r1);
      wdStaticRingBuffer<cc, 16> r3(r1);
      r3.PeekFront() = cc(3);

      WD_TEST_BOOL(r1 == r1);
      WD_TEST_BOOL(r2 == r2);
      WD_TEST_BOOL(r3 == r3);

      WD_TEST_BOOL(r1 == r2);
      WD_TEST_BOOL(r1 != r3);
      WD_TEST_BOOL(r2 != r3);
    }

    WD_TEST_BOOL(wdConstructionCounter::HasAllDestructed());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "PushBack / operator[] / CanAppend")
  {
    wdStaticRingBuffer<wdInt32, 16> r;

    for (wdUInt32 i = 0; i < 16; ++i)
    {
      WD_TEST_BOOL(r.CanAppend());
      r.PushBack(i);
    }

    WD_TEST_BOOL(!r.CanAppend());

    for (wdUInt32 i = 0; i < 16; ++i)
      WD_TEST_INT(r[i], i);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetCount / IsEmpty")
  {
    wdStaticRingBuffer<wdInt32, 16> r;

    WD_TEST_BOOL(r.IsEmpty());

    for (wdUInt32 i = 0; i < 16; ++i)
    {
      WD_TEST_INT(r.GetCount(), i);
      r.PushBack(i);
      WD_TEST_INT(r.GetCount(), i + 1);

      WD_TEST_BOOL(!r.IsEmpty());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Clear / IsEmpty")
  {
    wdStaticRingBuffer<wdInt32, 16> r;

    WD_TEST_BOOL(r.IsEmpty());

    for (wdUInt32 i = 0; i < 16; ++i)
      r.PushBack(i);

    WD_TEST_BOOL(!r.IsEmpty());

    r.Clear();

    WD_TEST_BOOL(r.IsEmpty());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Cycle Items / PeekFront")
  {
    WD_TEST_BOOL(wdConstructionCounter::HasAllDestructed());

    {
      wdStaticRingBuffer<wdConstructionCounter, 16> r;

      for (wdUInt32 i = 0; i < 16; ++i)
      {
        r.PushBack(wdConstructionCounter(i));
        WD_TEST_BOOL(wdConstructionCounter::HasDone(2, 1)); // one temporary
      }

      for (wdUInt32 i = 16; i < 1000; ++i)
      {
        WD_TEST_BOOL(r.PeekFront() == wdConstructionCounter(i - 16));
        WD_TEST_BOOL(wdConstructionCounter::HasDone(1, 1)); // one temporary

        WD_TEST_BOOL(!r.CanAppend());

        r.PopFront();
        WD_TEST_BOOL(wdConstructionCounter::HasDone(0, 1));

        WD_TEST_BOOL(r.CanAppend());

        r.PushBack(wdConstructionCounter(i));
        WD_TEST_BOOL(wdConstructionCounter::HasDone(2, 1)); // one temporary
      }

      for (wdUInt32 i = 1000; i < 1016; ++i)
      {
        WD_TEST_BOOL(r.PeekFront() == wdConstructionCounter(i - 16));
        WD_TEST_BOOL(wdConstructionCounter::HasDone(1, 1)); // one temporary

        r.PopFront();
        WD_TEST_BOOL(wdConstructionCounter::HasDone(0, 1)); // one temporary
      }

      WD_TEST_BOOL(r.IsEmpty());
    }

    WD_TEST_BOOL(wdConstructionCounter::HasAllDestructed());
  }
}
