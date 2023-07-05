#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Math/Rational.h>
#include <Foundation/Strings/StringBuilder.h>

WD_CREATE_SIMPLE_TEST(Math, Rational)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Rational")
  {
    wdRational r1(100, 1);

    WD_TEST_BOOL(r1.IsValid());
    WD_TEST_BOOL(r1.IsIntegral());

    wdRational r2(100, 0);
    WD_TEST_BOOL(!r2.IsValid());

    WD_TEST_BOOL(r1 != r2);

    wdRational r3(100, 1);
    WD_TEST_BOOL(r3 == r1);

    wdRational r4(0, 0);
    WD_TEST_BOOL(r4.IsValid());


    wdRational r5(30, 6);
    WD_TEST_BOOL(r5.IsIntegral());
    WD_TEST_INT(r5.GetIntegralResult(), 5);
    WD_TEST_FLOAT(r5.GetFloatingPointResult(), 5, wdMath::SmallEpsilon<double>());

    wdRational reducedTest(5, 1);
    WD_TEST_BOOL(r5.ReduceIntegralFraction() == reducedTest);

    wdRational r6(31, 6);
    WD_TEST_BOOL(!r6.IsIntegral());
    WD_TEST_FLOAT(r6.GetFloatingPointResult(), 5.16666666666, wdMath::SmallEpsilon<double>());


    WD_TEST_INT(r6.GetDenominator(), 6);
    WD_TEST_INT(r6.GetNumerator(), 31);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Rational String Formatting")
  {
    wdRational r1(50, 25);

    wdStringBuilder sb;
    sb.Format("Rational: {}", r1);
    WD_TEST_STRING(sb, "Rational: 2");


    wdRational r2(233, 76);
    sb.Format("Rational: {}", r2);
    WD_TEST_STRING(sb, "Rational: 233/76");
  }
}
