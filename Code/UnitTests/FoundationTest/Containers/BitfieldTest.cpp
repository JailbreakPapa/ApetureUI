#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Containers/Bitfield.h>
#include <Foundation/Containers/Deque.h>
#include <Foundation/Strings/String.h>

WD_CREATE_SIMPLE_TEST(Containers, Bitfield)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetCount / IsEmpty / Clear")
  {
    wdDynamicBitfield bf; // using a dynamic array

    WD_TEST_INT(bf.GetCount(), 0);
    WD_TEST_BOOL(bf.IsEmpty());

    bf.SetCount(15, false);

    WD_TEST_INT(bf.GetCount(), 15);
    WD_TEST_BOOL(!bf.IsEmpty());

    bf.Clear();

    WD_TEST_INT(bf.GetCount(), 0);
    WD_TEST_BOOL(bf.IsEmpty());

    bf.SetCount(37, false);

    WD_TEST_INT(bf.GetCount(), 37);
    WD_TEST_BOOL(!bf.IsEmpty());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetCount / SetAllBits / ClearAllBits")
  {
    wdHybridBitfield<512> bf; // using a hybrid array

    bf.SetCount(249, false);
    WD_TEST_INT(bf.GetCount(), 249);

    for (wdUInt32 i = 0; i < bf.GetCount(); ++i)
      WD_TEST_BOOL(!bf.IsBitSet(i));

    bf.SetAllBits();
    WD_TEST_INT(bf.GetCount(), 249);

    for (wdUInt32 i = 0; i < bf.GetCount(); ++i)
      WD_TEST_BOOL(bf.IsBitSet(i));

    bf.ClearAllBits();
    WD_TEST_INT(bf.GetCount(), 249);

    for (wdUInt32 i = 0; i < bf.GetCount(); ++i)
      WD_TEST_BOOL(!bf.IsBitSet(i));


    bf.SetCount(349, true);
    WD_TEST_INT(bf.GetCount(), 349);

    for (wdUInt32 i = 0; i < 249; ++i)
      WD_TEST_BOOL(!bf.IsBitSet(i));

    for (wdUInt32 i = 249; i < bf.GetCount(); ++i)
      WD_TEST_BOOL(bf.IsBitSet(i));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetCount / SetBit / ClearBit / SetCountUninitialized")
  {
    wdHybridBitfield<512> bf; // using a hybrid array

    bf.SetCount(100, false);
    WD_TEST_INT(bf.GetCount(), 100);

    for (wdUInt32 i = 0; i < bf.GetCount(); ++i)
      WD_TEST_BOOL(!bf.IsBitSet(i));

    bf.SetCount(200, true);
    WD_TEST_INT(bf.GetCount(), 200);

    for (wdUInt32 i = 100; i < bf.GetCount(); ++i)
      WD_TEST_BOOL(bf.IsBitSet(i));

    bf.SetCountUninitialized(250);
    WD_TEST_INT(bf.GetCount(), 250);

    bf.ClearAllBits();

    for (wdUInt32 i = 0; i < bf.GetCount(); i += 2)
      bf.SetBit(i);

    for (wdUInt32 i = 0; i < bf.GetCount(); i += 2)
    {
      WD_TEST_BOOL(bf.IsBitSet(i));
      WD_TEST_BOOL(!bf.IsBitSet(i + 1));
    }

    for (wdUInt32 i = 0; i < bf.GetCount(); i += 2)
    {
      bf.ClearBit(i);
      bf.SetBit(i + 1);
    }

    for (wdUInt32 i = 0; i < bf.GetCount(); i += 2)
    {
      WD_TEST_BOOL(!bf.IsBitSet(i));
      WD_TEST_BOOL(bf.IsBitSet(i + 1));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetBitRange")
  {
    for (wdUInt32 size = 1; size < 1024; ++size)
    {
      wdBitfield<wdDeque<wdUInt32>> bf; // using a deque
      bf.SetCount(size, false);

      WD_TEST_INT(bf.GetCount(), size);

      for (wdUInt32 count = 0; count < bf.GetCount(); ++count)
        WD_TEST_BOOL(!bf.IsBitSet(count));

      wdUInt32 uiStart = size / 2;
      wdUInt32 uiEnd = wdMath::Min(uiStart + (size / 3 * 2), size - 1);

      bf.SetBitRange(uiStart, uiEnd - uiStart + 1);

      for (wdUInt32 count = 0; count < uiStart; ++count)
        WD_TEST_BOOL(!bf.IsBitSet(count));
      for (wdUInt32 count = uiStart; count <= uiEnd; ++count)
        WD_TEST_BOOL(bf.IsBitSet(count));
      for (wdUInt32 count = uiEnd + 1; count < bf.GetCount(); ++count)
        WD_TEST_BOOL(!bf.IsBitSet(count));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ClearBitRange")
  {
    for (wdUInt32 size = 1; size < 1024; ++size)
    {
      wdBitfield<wdDeque<wdUInt32>> bf; // using a deque
      bf.SetCount(size, true);

      WD_TEST_INT(bf.GetCount(), size);

      for (wdUInt32 count = 0; count < bf.GetCount(); ++count)
        WD_TEST_BOOL(bf.IsBitSet(count));

      wdUInt32 uiStart = size / 2;
      wdUInt32 uiEnd = wdMath::Min(uiStart + (size / 3 * 2), size - 1);

      bf.ClearBitRange(uiStart, uiEnd - uiStart + 1);

      for (wdUInt32 count = 0; count < uiStart; ++count)
        WD_TEST_BOOL(bf.IsBitSet(count));
      for (wdUInt32 count = uiStart; count <= uiEnd; ++count)
        WD_TEST_BOOL(!bf.IsBitSet(count));
      for (wdUInt32 count = uiEnd + 1; count < bf.GetCount(); ++count)
        WD_TEST_BOOL(bf.IsBitSet(count));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsAnyBitSet / IsNoBitSet / AreAllBitsSet")
  {
    wdHybridBitfield<512> bf; // using a hybrid array

    WD_TEST_BOOL(bf.IsEmpty() == true);
    WD_TEST_BOOL(bf.IsAnyBitSet() == false); // empty
    WD_TEST_BOOL(bf.IsNoBitSet() == true);
    WD_TEST_BOOL(bf.AreAllBitsSet() == false); // empty

    bf.SetCount(250, false);

    WD_TEST_BOOL(bf.IsEmpty() == false);
    WD_TEST_BOOL(bf.IsAnyBitSet() == false);
    WD_TEST_BOOL(bf.IsNoBitSet() == true);
    WD_TEST_BOOL(bf.AreAllBitsSet() == false);

    for (wdUInt32 i = 0; i < bf.GetCount(); i += 2)
      bf.SetBit(i);

    WD_TEST_BOOL(bf.IsEmpty() == false);
    WD_TEST_BOOL(bf.IsAnyBitSet() == true);
    WD_TEST_BOOL(bf.IsNoBitSet() == false);
    WD_TEST_BOOL(bf.AreAllBitsSet() == false);

    for (wdUInt32 i = 0; i < bf.GetCount(); i++)
      bf.SetBit(i);

    WD_TEST_BOOL(bf.IsAnyBitSet() == true);
    WD_TEST_BOOL(bf.IsNoBitSet() == false);
    WD_TEST_BOOL(bf.AreAllBitsSet() == true);
  }
}


WD_CREATE_SIMPLE_TEST(Containers, StaticBitfield)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetAllBits / ClearAllBits")
  {
    wdStaticBitfield64 bf;

    for (wdUInt32 i = 0; i < bf.GetNumBits(); ++i)
      WD_TEST_BOOL(!bf.IsBitSet(i));

    bf.SetAllBits();

    for (wdUInt32 i = 0; i < bf.GetNumBits(); ++i)
      WD_TEST_BOOL(bf.IsBitSet(i));

    bf.ClearAllBits();

    for (wdUInt32 i = 0; i < bf.GetNumBits(); ++i)
      WD_TEST_BOOL(!bf.IsBitSet(i));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetBit / ClearBit")
  {
    wdStaticBitfield32 bf;

    for (wdUInt32 i = 0; i < bf.GetNumBits(); ++i)
      WD_TEST_BOOL(!bf.IsBitSet(i));

    for (wdUInt32 i = 0; i < bf.GetNumBits(); i += 2)
      bf.SetBit(i);

    for (wdUInt32 i = 0; i < bf.GetNumBits(); i += 2)
    {
      WD_TEST_BOOL(bf.IsBitSet(i));
      WD_TEST_BOOL(!bf.IsBitSet(i + 1));
    }

    for (wdUInt32 i = 0; i < bf.GetNumBits(); i += 2)
    {
      bf.ClearBit(i);
      bf.SetBit(i + 1);
    }

    for (wdUInt32 i = 0; i < bf.GetNumBits(); i += 2)
    {
      WD_TEST_BOOL(!bf.IsBitSet(i));
      WD_TEST_BOOL(bf.IsBitSet(i + 1));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetBitRange")
  {
    for (wdUInt32 uiStart = 0; uiStart < 61; ++uiStart)
    {
      wdStaticBitfield64 bf;

      for (wdUInt32 count = 0; count < bf.GetNumBits(); ++count)
        WD_TEST_BOOL(!bf.IsBitSet(count));

      wdUInt32 uiEnd = uiStart + 3;

      bf.SetBitRange(uiStart, uiEnd - uiStart + 1);

      for (wdUInt32 count = 0; count < uiStart; ++count)
        WD_TEST_BOOL(!bf.IsBitSet(count));
      for (wdUInt32 count = uiStart; count <= uiEnd; ++count)
        WD_TEST_BOOL(bf.IsBitSet(count));
      for (wdUInt32 count = uiEnd + 1; count < bf.GetNumBits(); ++count)
        WD_TEST_BOOL(!bf.IsBitSet(count));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ClearBitRange")
  {
    for (wdUInt32 uiStart = 0; uiStart < 61; ++uiStart)
    {
      wdStaticBitfield64 bf;
      bf.SetAllBits();

      for (wdUInt32 count = 0; count < bf.GetNumBits(); ++count)
        WD_TEST_BOOL(bf.IsBitSet(count));

      wdUInt32 uiEnd = uiStart + 3;

      bf.ClearBitRange(uiStart, uiEnd - uiStart + 1);

      for (wdUInt32 count = 0; count < uiStart; ++count)
        WD_TEST_BOOL(bf.IsBitSet(count));
      for (wdUInt32 count = uiStart; count <= uiEnd; ++count)
        WD_TEST_BOOL(!bf.IsBitSet(count));
      for (wdUInt32 count = uiEnd + 1; count < bf.GetNumBits(); ++count)
        WD_TEST_BOOL(bf.IsBitSet(count));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsAnyBitSet / IsNoBitSet / AreAllBitsSet")
  {
    wdStaticBitfield8 bf;

    WD_TEST_BOOL(bf.IsAnyBitSet() == false); // empty
    WD_TEST_BOOL(bf.IsNoBitSet() == true);
    WD_TEST_BOOL(bf.AreAllBitsSet() == false); // empty

    for (wdUInt32 i = 0; i < bf.GetNumBits(); i += 2)
      bf.SetBit(i);

    WD_TEST_BOOL(bf.IsAnyBitSet() == true);
    WD_TEST_BOOL(bf.IsNoBitSet() == false);
    WD_TEST_BOOL(bf.AreAllBitsSet() == false);

    for (wdUInt32 i = 0; i < bf.GetNumBits(); i++)
      bf.SetBit(i);

    WD_TEST_BOOL(bf.IsAnyBitSet() == true);
    WD_TEST_BOOL(bf.IsNoBitSet() == false);
    WD_TEST_BOOL(bf.AreAllBitsSet() == true);
  }
}
