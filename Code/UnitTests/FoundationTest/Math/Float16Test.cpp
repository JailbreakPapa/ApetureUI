#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Math/Float16.h>
#include <Foundation/Strings/String.h>

WD_CREATE_SIMPLE_TEST(Math, Float16)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "From float and back")
  {
    // default constructor
    WD_TEST_BOOL(static_cast<float>(wdFloat16()) == 0.0f);

    // Border cases - exact matching needed.
    WD_TEST_FLOAT(static_cast<float>(wdFloat16(1.0f)), 1.0f, 0);
    WD_TEST_FLOAT(static_cast<float>(wdFloat16(-1.0f)), -1.0f, 0);
    WD_TEST_FLOAT(static_cast<float>(wdFloat16(0.0f)), 0.0f, 0);
    WD_TEST_FLOAT(static_cast<float>(wdFloat16(-0.0f)), -0.0f, 0);
    WD_TEST_BOOL(static_cast<float>(wdFloat16(wdMath::Infinity<float>())) == wdMath::Infinity<float>());
    WD_TEST_BOOL(static_cast<float>(wdFloat16(-wdMath::Infinity<float>())) == -wdMath::Infinity<float>());
    WD_TEST_BOOL(wdMath::IsNaN(static_cast<float>(wdFloat16(wdMath::NaN<float>()))));

    // Some random values.
    WD_TEST_FLOAT(static_cast<float>(wdFloat16(42.0f)), 42.0f, wdMath::LargeEpsilon<float>());
    WD_TEST_FLOAT(static_cast<float>(wdFloat16(1.e3f)), 1.e3f, wdMath::LargeEpsilon<float>());
    WD_TEST_FLOAT(static_cast<float>(wdFloat16(-1230.0f)), -1230.0f, wdMath::LargeEpsilon<float>());
    WD_TEST_FLOAT(static_cast<float>(wdFloat16(wdMath::Pi<float>())), wdMath::Pi<float>(), wdMath::HugeEpsilon<float>());

    // Denormalized float.
    WD_TEST_FLOAT(static_cast<float>(wdFloat16(1.e-40f)), 0.0f, 0);
    WD_TEST_FLOAT(static_cast<float>(wdFloat16(1.e-44f)), 0.0f, 0);

    // Clamping of too large/small values
    // Half only supports 2^-14 to 2^14 (in 10^x this is roughly 4.51) (see Wikipedia)
    WD_TEST_FLOAT(static_cast<float>(wdFloat16(1.e-10f)), 0.0f, 0);
    WD_TEST_BOOL(static_cast<float>(wdFloat16(1.e5f)) == wdMath::Infinity<float>());
    WD_TEST_BOOL(static_cast<float>(wdFloat16(-1.e5f)) == -wdMath::Infinity<float>());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator ==")
  {
    WD_TEST_BOOL(wdFloat16(1.0f) == wdFloat16(1.0f));
    WD_TEST_BOOL(wdFloat16(10000000.0f) == wdFloat16(10000000.0f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator !=")
  {
    WD_TEST_BOOL(wdFloat16(1.0f) != wdFloat16(-1.0f));
    WD_TEST_BOOL(wdFloat16(10000000.0f) != wdFloat16(10000.0f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetRawData / SetRawData")
  {
    wdFloat16 f;
    f.SetRawData(23);

    WD_TEST_INT(f.GetRawData(), 23);
  }
}
