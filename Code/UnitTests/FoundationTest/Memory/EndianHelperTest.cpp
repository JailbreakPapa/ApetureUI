#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Memory/EndianHelper.h>

namespace
{
  struct TempStruct
  {
    float fVal;
    wdUInt32 uiDVal;
    wdUInt16 uiWVal1;
    wdUInt16 uiWVal2;
    char pad[4];
  };

  struct FloatAndInt
  {
    union
    {
      float fVal;
      wdUInt32 uiVal;
    };
  };
} // namespace


WD_CREATE_SIMPLE_TEST(Memory, Endian)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Basics")
  {
// Test if the IsBigEndian() delivers the same result as the #define
#if WD_ENABLED(WD_PLATFORM_LITTLE_ENDIAN)
    WD_TEST_BOOL(!wdEndianHelper::IsBigEndian());
#elif WD_ENABLED(WD_PLATFORM_BIG_ENDIAN)
    WD_TEST_BOOL(wdEndianHelper::IsBigEndian());
#endif

    // Test conversion functions for single elements
    WD_TEST_BOOL(wdEndianHelper::Switch(static_cast<wdUInt16>(0x15FF)) == 0xFF15);
    WD_TEST_BOOL(wdEndianHelper::Switch(static_cast<wdUInt32>(0x34AA12FF)) == 0xFF12AA34);
    WD_TEST_BOOL(wdEndianHelper::Switch(static_cast<wdUInt64>(0x34AA12FFABC3421E)) == 0x1E42C3ABFF12AA34);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Switching Arrays")
  {
    wdArrayPtr<wdUInt16> p16BitArray = WD_DEFAULT_NEW_ARRAY(wdUInt16, 1024);
    wdArrayPtr<wdUInt16> p16BitArrayCopy = WD_DEFAULT_NEW_ARRAY(wdUInt16, 1024);

    wdArrayPtr<wdUInt32> p32BitArray = WD_DEFAULT_NEW_ARRAY(wdUInt32, 1024);
    wdArrayPtr<wdUInt32> p32BitArrayCopy = WD_DEFAULT_NEW_ARRAY(wdUInt32, 1024);

    wdArrayPtr<wdUInt64> p64BitArray = WD_DEFAULT_NEW_ARRAY(wdUInt64, 1024);
    wdArrayPtr<wdUInt64> p64BitArrayCopy = WD_DEFAULT_NEW_ARRAY(wdUInt64, 1024);

    for (wdUInt32 i = 0; i < 1024; i++)
    {
      wdInt32 iRand = rand();
      p16BitArray[i] = static_cast<wdUInt16>(iRand);
      p32BitArray[i] = static_cast<wdUInt32>(iRand);
      p64BitArray[i] = static_cast<wdUInt64>(iRand | static_cast<wdUInt64>((iRand % 3)) << 32);
    }

    p16BitArrayCopy.CopyFrom(p16BitArray);
    p32BitArrayCopy.CopyFrom(p32BitArray);
    p64BitArrayCopy.CopyFrom(p64BitArray);

    wdEndianHelper::SwitchWords(p16BitArray.GetPtr(), 1024);
    wdEndianHelper::SwitchDWords(p32BitArray.GetPtr(), 1024);
    wdEndianHelper::SwitchQWords(p64BitArray.GetPtr(), 1024);

    for (wdUInt32 i = 0; i < 1024; i++)
    {
      WD_TEST_BOOL(p16BitArray[i] == wdEndianHelper::Switch(p16BitArrayCopy[i]));
      WD_TEST_BOOL(p32BitArray[i] == wdEndianHelper::Switch(p32BitArrayCopy[i]));
      WD_TEST_BOOL(p64BitArray[i] == wdEndianHelper::Switch(p64BitArrayCopy[i]));

      // Test in place switcher
      wdEndianHelper::SwitchInPlace(&p16BitArrayCopy[i]);
      WD_TEST_BOOL(p16BitArray[i] == p16BitArrayCopy[i]);

      wdEndianHelper::SwitchInPlace(&p32BitArrayCopy[i]);
      WD_TEST_BOOL(p32BitArray[i] == p32BitArrayCopy[i]);

      wdEndianHelper::SwitchInPlace(&p64BitArrayCopy[i]);
      WD_TEST_BOOL(p64BitArray[i] == p64BitArrayCopy[i]);
    }


    WD_DEFAULT_DELETE_ARRAY(p16BitArray);
    WD_DEFAULT_DELETE_ARRAY(p16BitArrayCopy);

    WD_DEFAULT_DELETE_ARRAY(p32BitArray);
    WD_DEFAULT_DELETE_ARRAY(p32BitArrayCopy);

    WD_DEFAULT_DELETE_ARRAY(p64BitArray);
    WD_DEFAULT_DELETE_ARRAY(p64BitArrayCopy);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Switching Structs")
  {
    TempStruct instance = {42.0f, 0x34AA12FF, 0x15FF, 0x23FF, {'E', 'Z', 'F', 'T'}};

    wdEndianHelper::SwitchStruct(&instance, "ddwwcccc");

    wdIntFloatUnion floatHelper(42.0f);
    wdIntFloatUnion floatHelper2(instance.fVal);

    WD_TEST_BOOL(floatHelper2.i == wdEndianHelper::Switch(floatHelper.i));
    WD_TEST_BOOL(instance.uiDVal == wdEndianHelper::Switch(static_cast<wdUInt32>(0x34AA12FF)));
    WD_TEST_BOOL(instance.uiWVal1 == wdEndianHelper::Switch(static_cast<wdUInt16>(0x15FF)));
    WD_TEST_BOOL(instance.uiWVal2 == wdEndianHelper::Switch(static_cast<wdUInt16>(0x23FF)));
    WD_TEST_BOOL(instance.pad[0] == 'E');
    WD_TEST_BOOL(instance.pad[1] == 'Z');
    WD_TEST_BOOL(instance.pad[2] == 'F');
    WD_TEST_BOOL(instance.pad[3] == 'T');
  }
}
