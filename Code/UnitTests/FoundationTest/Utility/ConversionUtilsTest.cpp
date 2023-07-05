#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Math/Random.h>
#include <Foundation/Utilities/ConversionUtils.h>

WD_CREATE_SIMPLE_TEST_GROUP(Utility);

WD_CREATE_SIMPLE_TEST(Utility, ConversionUtils)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "StringToInt")
  {
    const char* szString = "1a";
    const char* szResultPos = nullptr;

    wdInt32 iRes = 42;
    szString = "01234";
    WD_TEST_BOOL(wdConversionUtils::StringToInt(szString, iRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_INT(iRes, 1234);
    WD_TEST_BOOL(szResultPos == szString + 5);

    iRes = 42;
    szString = "0";
    WD_TEST_BOOL(wdConversionUtils::StringToInt(szString, iRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_INT(iRes, 0);
    WD_TEST_BOOL(szResultPos == szString + wdStringUtils::GetStringElementCount(szString));

    iRes = 42;
    szString = "0000";
    WD_TEST_BOOL(wdConversionUtils::StringToInt(szString, iRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_INT(iRes, 0);
    WD_TEST_BOOL(szResultPos == szString + wdStringUtils::GetStringElementCount(szString));

    iRes = 42;
    szString = "-999999";
    WD_TEST_BOOL(wdConversionUtils::StringToInt(szString, iRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_INT(iRes, -999999);
    WD_TEST_BOOL(szResultPos == szString + wdStringUtils::GetStringElementCount(szString));

    iRes = 42;
    szString = "-+999999";
    WD_TEST_BOOL(wdConversionUtils::StringToInt(szString, iRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_INT(iRes, -999999);
    WD_TEST_BOOL(szResultPos == szString + wdStringUtils::GetStringElementCount(szString));

    iRes = 42;
    szString = "--999999";
    WD_TEST_BOOL(wdConversionUtils::StringToInt(szString, iRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_INT(iRes, 999999);
    WD_TEST_BOOL(szResultPos == szString + wdStringUtils::GetStringElementCount(szString));

    iRes = 42;
    szString = "++---+--+--999999";
    WD_TEST_BOOL(wdConversionUtils::StringToInt(szString, iRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_INT(iRes, -999999);
    WD_TEST_BOOL(szResultPos == szString + wdStringUtils::GetStringElementCount(szString));

    iRes = 42;
    szString = "++--+--+--999999";
    WD_TEST_BOOL(wdConversionUtils::StringToInt(szString, iRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_INT(iRes, 999999);
    WD_TEST_BOOL(szResultPos == szString + wdStringUtils::GetStringElementCount(szString));

    iRes = 42;
    szString = "123+456";
    WD_TEST_BOOL(wdConversionUtils::StringToInt(szString, iRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_INT(iRes, 123);
    WD_TEST_BOOL(szResultPos == szString + 3);

    iRes = 42;
    szString = "123_456";
    WD_TEST_BOOL(wdConversionUtils::StringToInt(szString, iRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_INT(iRes, 123);
    WD_TEST_BOOL(szResultPos == szString + 3);

    iRes = 42;
    szString = "-123-456";
    WD_TEST_BOOL(wdConversionUtils::StringToInt(szString, iRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_INT(iRes, -123);
    WD_TEST_BOOL(szResultPos == szString + 4);


    iRes = 42;
    WD_TEST_BOOL(wdConversionUtils::StringToInt(nullptr, iRes) == WD_FAILURE);
    WD_TEST_INT(iRes, 42);

    iRes = 42;
    WD_TEST_BOOL(wdConversionUtils::StringToInt("", iRes) == WD_FAILURE);
    WD_TEST_INT(iRes, 42);

    iRes = 42;
    WD_TEST_BOOL(wdConversionUtils::StringToInt("a", iRes) == WD_FAILURE);
    WD_TEST_INT(iRes, 42);

    iRes = 42;
    WD_TEST_BOOL(wdConversionUtils::StringToInt("a15", iRes) == WD_FAILURE);
    WD_TEST_INT(iRes, 42);

    iRes = 42;
    WD_TEST_BOOL(wdConversionUtils::StringToInt("+", iRes) == WD_FAILURE);
    WD_TEST_INT(iRes, 42);

    iRes = 42;
    WD_TEST_BOOL(wdConversionUtils::StringToInt("-", iRes) == WD_FAILURE);
    WD_TEST_INT(iRes, 42);

    iRes = 42;
    szString = "1a";
    WD_TEST_BOOL(wdConversionUtils::StringToInt(szString, iRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_INT(iRes, 1);
    WD_TEST_BOOL(szResultPos == szString + 1);

    iRes = 42;
    szString = "0 23";
    WD_TEST_BOOL(wdConversionUtils::StringToInt(szString, iRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_INT(iRes, 0);
    WD_TEST_BOOL(szResultPos == szString + 1);

    // overflow check

    iRes = 42;
    szString = "0002147483647"; // valid
    WD_TEST_BOOL(wdConversionUtils::StringToInt(szString, iRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_INT(iRes, 2147483647);
    WD_TEST_BOOL(szResultPos == szString + wdStringUtils::GetStringElementCount(szString));

    iRes = 42;
    szString = "-2147483648"; // valid
    WD_TEST_BOOL(wdConversionUtils::StringToInt(szString, iRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_INT(iRes, (wdInt32)0x80000000);
    WD_TEST_BOOL(szResultPos == szString + wdStringUtils::GetStringElementCount(szString));

    iRes = 42;
    szString = "0002147483648"; // invalid
    WD_TEST_BOOL(wdConversionUtils::StringToInt(szString, iRes, &szResultPos) == WD_FAILURE);
    WD_TEST_INT(iRes, 42);

    iRes = 42;
    szString = "-2147483649"; // invalid
    WD_TEST_BOOL(wdConversionUtils::StringToInt(szString, iRes, &szResultPos) == WD_FAILURE);
    WD_TEST_INT(iRes, 42);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "StringToUInt")
  {
    const char* szString = "1a";
    const char* szResultPos = nullptr;

    wdUInt32 uiRes = 42;
    szString = "01234";
    WD_TEST_BOOL(wdConversionUtils::StringToUInt(szString, uiRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_INT(uiRes, 1234);
    WD_TEST_BOOL(szResultPos == szString + 5);

    uiRes = 42;
    szString = "0";
    WD_TEST_BOOL(wdConversionUtils::StringToUInt(szString, uiRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_INT(uiRes, 0);
    WD_TEST_BOOL(szResultPos == szString + wdStringUtils::GetStringElementCount(szString));

    uiRes = 42;
    szString = "0000";
    WD_TEST_BOOL(wdConversionUtils::StringToUInt(szString, uiRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_INT(uiRes, 0);
    WD_TEST_BOOL(szResultPos == szString + wdStringUtils::GetStringElementCount(szString));

    uiRes = 42;
    szString = "-999999";
    WD_TEST_BOOL(wdConversionUtils::StringToUInt(szString, uiRes, &szResultPos) == WD_FAILURE);
    WD_TEST_INT(uiRes, 42);
    WD_TEST_BOOL(szResultPos == szString + wdStringUtils::GetStringElementCount(szString));

    uiRes = 42;
    szString = "-+999999";
    WD_TEST_BOOL(wdConversionUtils::StringToUInt(szString, uiRes, &szResultPos) == WD_FAILURE);
    WD_TEST_INT(uiRes, 42);
    WD_TEST_BOOL(szResultPos == szString + wdStringUtils::GetStringElementCount(szString));

    uiRes = 42;
    szString = "--999999";
    WD_TEST_BOOL(wdConversionUtils::StringToUInt(szString, uiRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_INT(uiRes, 999999);
    WD_TEST_BOOL(szResultPos == szString + wdStringUtils::GetStringElementCount(szString));

    uiRes = 42;
    szString = "++---+--+--999999";
    WD_TEST_BOOL(wdConversionUtils::StringToUInt(szString, uiRes, &szResultPos) == WD_FAILURE);
    WD_TEST_INT(uiRes, 42);
    WD_TEST_BOOL(szResultPos == szString + wdStringUtils::GetStringElementCount(szString));

    uiRes = 42;
    szString = "++--+--+--999999";
    WD_TEST_BOOL(wdConversionUtils::StringToUInt(szString, uiRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_INT(uiRes, 999999);
    WD_TEST_BOOL(szResultPos == szString + wdStringUtils::GetStringElementCount(szString));

    uiRes = 42;
    szString = "123+456";
    WD_TEST_BOOL(wdConversionUtils::StringToUInt(szString, uiRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_INT(uiRes, 123);
    WD_TEST_BOOL(szResultPos == szString + 3);

    uiRes = 42;
    szString = "123_456";
    WD_TEST_BOOL(wdConversionUtils::StringToUInt(szString, uiRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_INT(uiRes, 123);
    WD_TEST_BOOL(szResultPos == szString + 3);

    uiRes = 42;
    szString = "-123-456";
    WD_TEST_BOOL(wdConversionUtils::StringToUInt(szString, uiRes, &szResultPos) == WD_FAILURE);
    WD_TEST_INT(uiRes, 42);
    WD_TEST_BOOL(szResultPos == szString + 4);


    uiRes = 42;
    WD_TEST_BOOL(wdConversionUtils::StringToUInt(nullptr, uiRes) == WD_FAILURE);
    WD_TEST_INT(uiRes, 42);

    uiRes = 42;
    WD_TEST_BOOL(wdConversionUtils::StringToUInt("", uiRes) == WD_FAILURE);
    WD_TEST_INT(uiRes, 42);

    uiRes = 42;
    WD_TEST_BOOL(wdConversionUtils::StringToUInt("a", uiRes) == WD_FAILURE);
    WD_TEST_INT(uiRes, 42);

    uiRes = 42;
    WD_TEST_BOOL(wdConversionUtils::StringToUInt("a15", uiRes) == WD_FAILURE);
    WD_TEST_INT(uiRes, 42);

    uiRes = 42;
    WD_TEST_BOOL(wdConversionUtils::StringToUInt("+", uiRes) == WD_FAILURE);
    WD_TEST_INT(uiRes, 42);

    uiRes = 42;
    WD_TEST_BOOL(wdConversionUtils::StringToUInt("-", uiRes) == WD_FAILURE);
    WD_TEST_INT(uiRes, 42);

    uiRes = 42;
    szString = "1a";
    WD_TEST_BOOL(wdConversionUtils::StringToUInt(szString, uiRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_INT(uiRes, 1);
    WD_TEST_BOOL(szResultPos == szString + 1);

    uiRes = 42;
    szString = "0 23";
    WD_TEST_BOOL(wdConversionUtils::StringToUInt(szString, uiRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_INT(uiRes, 0);
    WD_TEST_BOOL(szResultPos == szString + 1);

    // overflow check

    uiRes = 42;
    szString = "0004294967295"; // valid
    WD_TEST_BOOL(wdConversionUtils::StringToUInt(szString, uiRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_INT(uiRes, 4294967295u);
    WD_TEST_BOOL(szResultPos == szString + wdStringUtils::GetStringElementCount(szString));

    uiRes = 42;
    szString = "0004294967296"; // invalid
    WD_TEST_BOOL(wdConversionUtils::StringToUInt(szString, uiRes, &szResultPos) == WD_FAILURE);
    WD_TEST_INT(uiRes, 42);

    uiRes = 42;
    szString = "-1"; // invalid
    WD_TEST_BOOL(wdConversionUtils::StringToUInt(szString, uiRes, &szResultPos) == WD_FAILURE);
    WD_TEST_INT(uiRes, 42);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "StringToInt64")
  {
    // overflow check
    wdInt64 iRes = 42;
    const char* szString = "0002147483639"; // valid
    const char* szResultPos = nullptr;

    WD_TEST_BOOL(wdConversionUtils::StringToInt64(szString, iRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_INT(iRes, 2147483639);
    WD_TEST_BOOL(szResultPos == szString + wdStringUtils::GetStringElementCount(szString));

    iRes = 42;
    szString = "0002147483640"; // also valid with 64bit
    WD_TEST_BOOL(wdConversionUtils::StringToInt64(szString, iRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_INT(iRes, 2147483640);
    WD_TEST_BOOL(szResultPos == szString + wdStringUtils::GetStringElementCount(szString));

    iRes = 42;
    szString = "0009223372036854775807"; // last valid positive number
    WD_TEST_BOOL(wdConversionUtils::StringToInt64(szString, iRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_INT(iRes, 9223372036854775807);
    WD_TEST_BOOL(szResultPos == szString + wdStringUtils::GetStringElementCount(szString));

    iRes = 42;
    szString = "0009223372036854775808"; // invalid
    WD_TEST_BOOL(wdConversionUtils::StringToInt64(szString, iRes, &szResultPos) == WD_FAILURE);
    WD_TEST_INT(iRes, 42);

    iRes = 42;
    szString = "-9223372036854775808"; // last valid negative number
    WD_TEST_BOOL(wdConversionUtils::StringToInt64(szString, iRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_INT(iRes, (wdInt64)0x8000000000000000);
    WD_TEST_BOOL(szResultPos == szString + wdStringUtils::GetStringElementCount(szString));

    iRes = 42;
    szString = "-9223372036854775809"; // invalid
    WD_TEST_BOOL(wdConversionUtils::StringToInt64(szString, iRes, &szResultPos) == WD_FAILURE);
    WD_TEST_INT(iRes, 42);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "StringToFloat")
  {
    const char* szString = nullptr;
    const char* szResultPos = nullptr;

    double fRes = 42;
    szString = "23.45";
    WD_TEST_BOOL(wdConversionUtils::StringToFloat(szString, fRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_DOUBLE(fRes, 23.45, 0.00001);
    WD_TEST_BOOL(szResultPos == szString + wdStringUtils::GetStringElementCount(szString));

    fRes = 42;
    szString = "-2345";
    WD_TEST_BOOL(wdConversionUtils::StringToFloat(szString, fRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_DOUBLE(fRes, -2345.0, 0.00001);
    WD_TEST_BOOL(szResultPos == szString + wdStringUtils::GetStringElementCount(szString));

    fRes = 42;
    szString = "-0";
    WD_TEST_BOOL(wdConversionUtils::StringToFloat(szString, fRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_DOUBLE(fRes, 0.0, 0.00001);
    WD_TEST_BOOL(szResultPos == szString + wdStringUtils::GetStringElementCount(szString));

    fRes = 42;
    szString = "0_0000.0_00000_";
    WD_TEST_BOOL(wdConversionUtils::StringToFloat(szString, fRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_DOUBLE(fRes, 0.0, 0.00001);
    WD_TEST_BOOL(szResultPos == szString + wdStringUtils::GetStringElementCount(szString));

    fRes = 42;
    szString = "_0_0000.0_00000_";
    WD_TEST_BOOL(wdConversionUtils::StringToFloat(szString, fRes, &szResultPos) == WD_FAILURE);

    fRes = 42;
    szString = ".123456789";
    WD_TEST_BOOL(wdConversionUtils::StringToFloat(szString, fRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_DOUBLE(fRes, 0.123456789, 0.00001);
    WD_TEST_BOOL(szResultPos == szString + wdStringUtils::GetStringElementCount(szString));

    fRes = 42;
    szString = "+123E1";
    WD_TEST_BOOL(wdConversionUtils::StringToFloat(szString, fRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_DOUBLE(fRes, 1230.0, 0.00001);
    WD_TEST_BOOL(szResultPos == szString + wdStringUtils::GetStringElementCount(szString));

    fRes = 42;
    szString = "  \r\t 123e0";
    WD_TEST_BOOL(wdConversionUtils::StringToFloat(szString, fRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_DOUBLE(fRes, 123.0, 0.00001);
    WD_TEST_BOOL(szResultPos == szString + wdStringUtils::GetStringElementCount(szString));

    fRes = 42;
    szString = "\n123e6";
    WD_TEST_BOOL(wdConversionUtils::StringToFloat(szString, fRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_DOUBLE(fRes, 123000000.0, 0.00001);
    WD_TEST_BOOL(szResultPos == szString + wdStringUtils::GetStringElementCount(szString));

    fRes = 42;
    szString = "\n1_2_3e+6";
    WD_TEST_BOOL(wdConversionUtils::StringToFloat(szString, fRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_DOUBLE(fRes, 123000000.0, 0.00001);
    WD_TEST_BOOL(szResultPos == szString + wdStringUtils::GetStringElementCount(szString));

    fRes = 42;
    szString = "  123E-6";
    WD_TEST_BOOL(wdConversionUtils::StringToFloat(szString, fRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_DOUBLE(fRes, 0.000123, 0.00001);
    WD_TEST_BOOL(szResultPos == szString + wdStringUtils::GetStringElementCount(szString));

    fRes = 42;
    szString = " + - -+-123.45e-10";
    WD_TEST_BOOL(wdConversionUtils::StringToFloat(szString, fRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_DOUBLE(fRes, -0.000000012345, 0.0000001);
    WD_TEST_BOOL(szResultPos == szString + wdStringUtils::GetStringElementCount(szString));

    fRes = 42;
    szString = nullptr;
    WD_TEST_BOOL(wdConversionUtils::StringToFloat(szString, fRes, &szResultPos) == WD_FAILURE);
    WD_TEST_DOUBLE(fRes, 42.0, 0.00001);

    fRes = 42;
    szString = "";
    WD_TEST_BOOL(wdConversionUtils::StringToFloat(szString, fRes, &szResultPos) == WD_FAILURE);
    WD_TEST_DOUBLE(fRes, 42.0, 0.00001);

    fRes = 42;
    szString = "-----";
    WD_TEST_BOOL(wdConversionUtils::StringToFloat(szString, fRes, &szResultPos) == WD_FAILURE);
    WD_TEST_DOUBLE(fRes, 42.0, 0.00001);

    fRes = 42;
    szString = " + - +++ - \r \n";
    WD_TEST_BOOL(wdConversionUtils::StringToFloat(szString, fRes, &szResultPos) == WD_FAILURE);
    WD_TEST_DOUBLE(fRes, 42.0, 0.00001);


    fRes = 42;
    szString = "65.345789xabc";
    WD_TEST_BOOL(wdConversionUtils::StringToFloat(szString, fRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_DOUBLE(fRes, 65.345789, 0.000001);
    WD_TEST_BOOL(szResultPos == szString + 9);

    fRes = 42;
    szString = " \n \r \t + - 2314565.345789ff xabc";
    WD_TEST_BOOL(wdConversionUtils::StringToFloat(szString, fRes, &szResultPos) == WD_SUCCESS);
    WD_TEST_DOUBLE(fRes, -2314565.345789, 0.000001);
    WD_TEST_BOOL(szResultPos == szString + 25);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "StringToBool")
  {
    const char* szString = "";
    const char* szResultPos = nullptr;
    bool bRes = false;

    // true / false
    {
      bRes = false;
      szString = "true,";
      szResultPos = nullptr;
      WD_TEST_BOOL(wdConversionUtils::StringToBool(szString, bRes, &szResultPos) == WD_SUCCESS);
      WD_TEST_BOOL(bRes);
      WD_TEST_BOOL(*szResultPos == ',');

      bRes = true;
      szString = "FALSe,";
      szResultPos = nullptr;
      WD_TEST_BOOL(wdConversionUtils::StringToBool(szString, bRes, &szResultPos) == WD_SUCCESS);
      WD_TEST_BOOL(!bRes);
      WD_TEST_BOOL(*szResultPos == ',');
    }

    // on / off
    {
      bRes = false;
      szString = "\n on,";
      szResultPos = nullptr;
      WD_TEST_BOOL(wdConversionUtils::StringToBool(szString, bRes, &szResultPos) == WD_SUCCESS);
      WD_TEST_BOOL(bRes);
      WD_TEST_BOOL(*szResultPos == ',');

      bRes = true;
      szString = "\t\t \toFf,";
      szResultPos = nullptr;
      WD_TEST_BOOL(wdConversionUtils::StringToBool(szString, bRes, &szResultPos) == WD_SUCCESS);
      WD_TEST_BOOL(!bRes);
      WD_TEST_BOOL(*szResultPos == ',');
    }

    // 1 / 0
    {
      bRes = false;
      szString = "\r1,";
      szResultPos = nullptr;
      WD_TEST_BOOL(wdConversionUtils::StringToBool(szString, bRes, &szResultPos) == WD_SUCCESS);
      WD_TEST_BOOL(bRes);
      WD_TEST_BOOL(*szResultPos == ',');

      bRes = true;
      szString = "0,";
      szResultPos = nullptr;
      WD_TEST_BOOL(wdConversionUtils::StringToBool(szString, bRes, &szResultPos) == WD_SUCCESS);
      WD_TEST_BOOL(!bRes);
      WD_TEST_BOOL(*szResultPos == ',');
    }

    // yes / no
    {
      bRes = false;
      szString = "yes,";
      szResultPos = nullptr;
      WD_TEST_BOOL(wdConversionUtils::StringToBool(szString, bRes, &szResultPos) == WD_SUCCESS);
      WD_TEST_BOOL(bRes);
      WD_TEST_BOOL(*szResultPos == ',');

      bRes = true;
      szString = "NO,";
      szResultPos = nullptr;
      WD_TEST_BOOL(wdConversionUtils::StringToBool(szString, bRes, &szResultPos) == WD_SUCCESS);
      WD_TEST_BOOL(!bRes);
      WD_TEST_BOOL(*szResultPos == ',');
    }

    // enable / disable
    {
      bRes = false;
      szString = "enable,";
      szResultPos = nullptr;
      WD_TEST_BOOL(wdConversionUtils::StringToBool(szString, bRes, &szResultPos) == WD_SUCCESS);
      WD_TEST_BOOL(bRes);
      WD_TEST_BOOL(*szResultPos == ',');

      bRes = true;
      szString = "disABle,";
      szResultPos = nullptr;
      WD_TEST_BOOL(wdConversionUtils::StringToBool(szString, bRes, &szResultPos) == WD_SUCCESS);
      WD_TEST_BOOL(!bRes);
      WD_TEST_BOOL(*szResultPos == ',');
    }

    bRes = false;

    szString = "of,";
    szResultPos = nullptr;
    WD_TEST_BOOL(wdConversionUtils::StringToBool(szString, bRes, &szResultPos) == WD_FAILURE);
    WD_TEST_BOOL(szResultPos == nullptr);

    szString = "aon";
    szResultPos = nullptr;
    WD_TEST_BOOL(wdConversionUtils::StringToBool(szString, bRes, &szResultPos) == WD_FAILURE);
    WD_TEST_BOOL(szResultPos == nullptr);

    szString = "";
    szResultPos = nullptr;
    WD_TEST_BOOL(wdConversionUtils::StringToBool(szString, bRes, &szResultPos) == WD_FAILURE);
    WD_TEST_BOOL(szResultPos == nullptr);

    szString = nullptr;
    szResultPos = nullptr;
    WD_TEST_BOOL(wdConversionUtils::StringToBool(szString, bRes, &szResultPos) == WD_FAILURE);
    WD_TEST_BOOL(szResultPos == nullptr);

    szString = "tut";
    szResultPos = nullptr;
    WD_TEST_BOOL(wdConversionUtils::StringToBool(szString, bRes, &szResultPos) == WD_FAILURE);
    WD_TEST_BOOL(szResultPos == nullptr);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "HexCharacterToIntValue")
  {
    WD_TEST_INT(wdConversionUtils::HexCharacterToIntValue('0'), 0);
    WD_TEST_INT(wdConversionUtils::HexCharacterToIntValue('1'), 1);
    WD_TEST_INT(wdConversionUtils::HexCharacterToIntValue('2'), 2);
    WD_TEST_INT(wdConversionUtils::HexCharacterToIntValue('3'), 3);
    WD_TEST_INT(wdConversionUtils::HexCharacterToIntValue('4'), 4);
    WD_TEST_INT(wdConversionUtils::HexCharacterToIntValue('5'), 5);
    WD_TEST_INT(wdConversionUtils::HexCharacterToIntValue('6'), 6);
    WD_TEST_INT(wdConversionUtils::HexCharacterToIntValue('7'), 7);
    WD_TEST_INT(wdConversionUtils::HexCharacterToIntValue('8'), 8);
    WD_TEST_INT(wdConversionUtils::HexCharacterToIntValue('9'), 9);

    WD_TEST_INT(wdConversionUtils::HexCharacterToIntValue('a'), 10);
    WD_TEST_INT(wdConversionUtils::HexCharacterToIntValue('b'), 11);
    WD_TEST_INT(wdConversionUtils::HexCharacterToIntValue('c'), 12);
    WD_TEST_INT(wdConversionUtils::HexCharacterToIntValue('d'), 13);
    WD_TEST_INT(wdConversionUtils::HexCharacterToIntValue('e'), 14);
    WD_TEST_INT(wdConversionUtils::HexCharacterToIntValue('f'), 15);

    WD_TEST_INT(wdConversionUtils::HexCharacterToIntValue('A'), 10);
    WD_TEST_INT(wdConversionUtils::HexCharacterToIntValue('B'), 11);
    WD_TEST_INT(wdConversionUtils::HexCharacterToIntValue('C'), 12);
    WD_TEST_INT(wdConversionUtils::HexCharacterToIntValue('D'), 13);
    WD_TEST_INT(wdConversionUtils::HexCharacterToIntValue('E'), 14);
    WD_TEST_INT(wdConversionUtils::HexCharacterToIntValue('F'), 15);

    WD_TEST_INT(wdConversionUtils::HexCharacterToIntValue('g'), -1);
    WD_TEST_INT(wdConversionUtils::HexCharacterToIntValue('h'), -1);
    WD_TEST_INT(wdConversionUtils::HexCharacterToIntValue('i'), -1);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ConvertHexStringToUInt32")
  {
    wdUInt32 res;

    WD_TEST_BOOL(wdConversionUtils::ConvertHexStringToUInt32("", res).Succeeded());
    WD_TEST_BOOL(res == 0);

    WD_TEST_BOOL(wdConversionUtils::ConvertHexStringToUInt32("0x", res).Succeeded());
    WD_TEST_BOOL(res == 0);

    WD_TEST_BOOL(wdConversionUtils::ConvertHexStringToUInt32("0", res).Succeeded());
    WD_TEST_BOOL(res == 0);

    WD_TEST_BOOL(wdConversionUtils::ConvertHexStringToUInt32("0x0", res).Succeeded());
    WD_TEST_BOOL(res == 0);

    WD_TEST_BOOL(wdConversionUtils::ConvertHexStringToUInt32("a", res).Succeeded());
    WD_TEST_BOOL(res == 10);

    WD_TEST_BOOL(wdConversionUtils::ConvertHexStringToUInt32("0xb", res).Succeeded());
    WD_TEST_BOOL(res == 11);

    WD_TEST_BOOL(wdConversionUtils::ConvertHexStringToUInt32("000c", res).Succeeded());
    WD_TEST_BOOL(res == 12);

    WD_TEST_BOOL(wdConversionUtils::ConvertHexStringToUInt32("AA", res).Succeeded());
    WD_TEST_BOOL(res == 170);

    WD_TEST_BOOL(wdConversionUtils::ConvertHexStringToUInt32("aAjbB", res).Failed());

    WD_TEST_BOOL(wdConversionUtils::ConvertHexStringToUInt32("aAbB", res).Succeeded());
    WD_TEST_BOOL(res == 43707);

    WD_TEST_BOOL(wdConversionUtils::ConvertHexStringToUInt32("FFFFffff", res).Succeeded());
    WD_TEST_BOOL(res == 0xFFFFFFFF);

    WD_TEST_BOOL(wdConversionUtils::ConvertHexStringToUInt32("0000FFFFffff", res).Succeeded());
    WD_TEST_BOOL(res == 0xFFFF);

    WD_TEST_BOOL(wdConversionUtils::ConvertHexStringToUInt32("100000000", res).Succeeded());
    WD_TEST_BOOL(res == 0x10000000);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ConvertHexStringToUInt64")
  {
    wdUInt64 res;

    WD_TEST_BOOL(wdConversionUtils::ConvertHexStringToUInt64("", res).Succeeded());
    WD_TEST_BOOL(res == 0);

    WD_TEST_BOOL(wdConversionUtils::ConvertHexStringToUInt64("0x", res).Succeeded());
    WD_TEST_BOOL(res == 0);

    WD_TEST_BOOL(wdConversionUtils::ConvertHexStringToUInt64("0", res).Succeeded());
    WD_TEST_BOOL(res == 0);

    WD_TEST_BOOL(wdConversionUtils::ConvertHexStringToUInt64("0x0", res).Succeeded());
    WD_TEST_BOOL(res == 0);

    WD_TEST_BOOL(wdConversionUtils::ConvertHexStringToUInt64("a", res).Succeeded());
    WD_TEST_BOOL(res == 10);

    WD_TEST_BOOL(wdConversionUtils::ConvertHexStringToUInt64("0xb", res).Succeeded());
    WD_TEST_BOOL(res == 11);

    WD_TEST_BOOL(wdConversionUtils::ConvertHexStringToUInt64("000c", res).Succeeded());
    WD_TEST_BOOL(res == 12);

    WD_TEST_BOOL(wdConversionUtils::ConvertHexStringToUInt64("AA", res).Succeeded());
    WD_TEST_BOOL(res == 170);

    WD_TEST_BOOL(wdConversionUtils::ConvertHexStringToUInt64("aAjbB", res).Failed());

    WD_TEST_BOOL(wdConversionUtils::ConvertHexStringToUInt64("aAbB", res).Succeeded());
    WD_TEST_BOOL(res == 43707);

    WD_TEST_BOOL(wdConversionUtils::ConvertHexStringToUInt64("FFFFffff", res).Succeeded());
    WD_TEST_BOOL(res == 4294967295);

    WD_TEST_BOOL(wdConversionUtils::ConvertHexStringToUInt64("0000FFFFffff", res).Succeeded());
    WD_TEST_BOOL(res == 4294967295);

    WD_TEST_BOOL(wdConversionUtils::ConvertHexStringToUInt64("0xfffffffffffffffy", res).Failed());

    WD_TEST_BOOL(wdConversionUtils::ConvertHexStringToUInt64("0xffffffffffffffffy", res).Succeeded());
    WD_TEST_BOOL(res == 0xFFFFFFFFFFFFFFFFllu);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ConvertBinaryToHex and ConvertHexStringToBinary")
  {
    wdDynamicArray<wdUInt8> binary;
    binary.SetCountUninitialized(1024);

    wdRandom r;
    r.InitializeFromCurrentTime();

    for (auto& val : binary)
    {
      val = static_cast<wdUInt8>(r.UIntInRange(256u));
    }

    wdStringBuilder sHex;
    wdConversionUtils::ConvertBinaryToHex(binary.GetData(), binary.GetCount(), [&sHex](const char* s) { sHex.Append(s); });

    wdDynamicArray<wdUInt8> binary2;
    binary2.SetCountUninitialized(1024);

    wdConversionUtils::ConvertHexToBinary(sHex, binary2.GetData(), binary2.GetCount());

    WD_TEST_BOOL(binary == binary2);
  }


  WD_TEST_BLOCK(wdTestBlock::Enabled, "ExtractFloatsFromString")
  {
    float v[16];

    const char* szText = "This 1 is 2.3 or 3.141 tests in 1.2 strings, maybe 4.5,6.78or9.101!";

    wdMemoryUtils::ZeroFill(v, 16);
    WD_TEST_INT(wdConversionUtils::ExtractFloatsFromString(szText, 0, v), 0);
    WD_TEST_FLOAT(v[0], 0.0f, 0.0f);

    wdMemoryUtils::ZeroFill(v, 16);
    WD_TEST_INT(wdConversionUtils::ExtractFloatsFromString(szText, 3, v), 3);
    WD_TEST_FLOAT(v[0], 1.0f, 0.0001f);
    WD_TEST_FLOAT(v[1], 2.3f, 0.0001f);
    WD_TEST_FLOAT(v[2], 3.141f, 0.0001f);
    WD_TEST_FLOAT(v[3], 0.0f, 0.0f);

    wdMemoryUtils::ZeroFill(v, 16);
    WD_TEST_INT(wdConversionUtils::ExtractFloatsFromString(szText, 6, v), 6);
    WD_TEST_FLOAT(v[0], 1.0f, 0.0001f);
    WD_TEST_FLOAT(v[1], 2.3f, 0.0001f);
    WD_TEST_FLOAT(v[2], 3.141f, 0.0001f);
    WD_TEST_FLOAT(v[3], 1.2f, 0.0001f);
    WD_TEST_FLOAT(v[4], 4.5f, 0.0001f);
    WD_TEST_FLOAT(v[5], 6.78f, 0.0001f);
    WD_TEST_FLOAT(v[6], 0.0f, 0.0f);

    wdMemoryUtils::ZeroFill(v, 16);
    WD_TEST_INT(wdConversionUtils::ExtractFloatsFromString(szText, 10, v), 7);
    WD_TEST_FLOAT(v[0], 1.0f, 0.0001f);
    WD_TEST_FLOAT(v[1], 2.3f, 0.0001f);
    WD_TEST_FLOAT(v[2], 3.141f, 0.0001f);
    WD_TEST_FLOAT(v[3], 1.2f, 0.0001f);
    WD_TEST_FLOAT(v[4], 4.5f, 0.0001f);
    WD_TEST_FLOAT(v[5], 6.78f, 0.0001f);
    WD_TEST_FLOAT(v[6], 9.101f, 0.0001f);
    WD_TEST_FLOAT(v[7], 0.0f, 0.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ConvertStringToUuid and IsStringUuid")
  {
    wdUuid guid;
    wdStringBuilder sGuid;

    for (wdUInt32 i = 0; i < 100; ++i)
    {
      guid.CreateNewUuid();

      wdConversionUtils::ToString(guid, sGuid);

      WD_TEST_BOOL(wdConversionUtils::IsStringUuid(sGuid));

      wdUuid guid2 = wdConversionUtils::ConvertStringToUuid(sGuid);

      WD_TEST_BOOL(guid == guid2);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetColorName")
  {
    WD_TEST_STRING(wdString(wdConversionUtils::GetColorName(wdColorGammaUB(1, 2, 3))), "#010203");
    WD_TEST_STRING(wdString(wdConversionUtils::GetColorName(wdColorGammaUB(10, 20, 30, 40))), "#0A141E28");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetColorByName")
  {
    WD_TEST_BOOL(wdConversionUtils::GetColorByName("#010203") == wdColorGammaUB(1, 2, 3));
    WD_TEST_BOOL(wdConversionUtils::GetColorByName("#0A141E28") == wdColorGammaUB(10, 20, 30, 40));

    WD_TEST_BOOL(wdConversionUtils::GetColorByName("#010203") == wdColorGammaUB(1, 2, 3));
    WD_TEST_BOOL(wdConversionUtils::GetColorByName("#0a141e28") == wdColorGammaUB(10, 20, 30, 40));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetColorByName and GetColorName")
  {
#define Check(name)                                                                  \
  {                                                                                  \
    bool valid = false;                                                              \
    const wdColor c = wdConversionUtils::GetColorByName(WD_STRINGIZE(name), &valid); \
    WD_TEST_BOOL(valid);                                                             \
    wdString sName = wdConversionUtils::GetColorName(c);                             \
    WD_TEST_STRING(sName, WD_STRINGIZE(name));                                       \
  }

#define Check2(name, otherName)                                                      \
  {                                                                                  \
    bool valid = false;                                                              \
    const wdColor c = wdConversionUtils::GetColorByName(WD_STRINGIZE(name), &valid); \
    WD_TEST_BOOL(valid);                                                             \
    wdString sName = wdConversionUtils::GetColorName(c);                             \
    WD_TEST_STRING(sName, WD_STRINGIZE(otherName));                                  \
  }

    Check(AliceBlue);
    Check(AntiqueWhite);
    Check(Aqua);
    Check(Aquamarine);
    Check(Azure);
    Check(Beige);
    Check(Bisque);
    Check(Black);
    Check(BlanchedAlmond);
    Check(Blue);
    Check(BlueViolet);
    Check(Brown);
    Check(BurlyWood);
    Check(CadetBlue);
    Check(Chartreuse);
    Check(Chocolate);
    Check(Coral);
    Check(CornflowerBlue); // The Original!
    Check(Cornsilk);
    Check(Crimson);
    Check2(Cyan, Aqua);
    Check(DarkBlue);
    Check(DarkCyan);
    Check(DarkGoldenRod);
    Check(DarkGray);
    Check2(DarkGrey, DarkGray);
    Check(DarkGreen);
    Check(DarkKhaki);
    Check(DarkMagenta);
    Check(DarkOliveGreen);
    Check(DarkOrange);
    Check(DarkOrchid);
    Check(DarkRed);
    Check(DarkSalmon);
    Check(DarkSeaGreen);
    Check(DarkSlateBlue);
    Check(DarkSlateGray);
    Check2(DarkSlateGrey, DarkSlateGray);
    Check(DarkTurquoise);
    Check(DarkViolet);
    Check(DeepPink);
    Check(DeepSkyBlue);
    Check(DimGray);
    Check2(DimGrey, DimGray);
    Check(DodgerBlue);
    Check(FireBrick);
    Check(FloralWhite);
    Check(ForestGreen);
    Check(Fuchsia);
    Check(Gainsboro);
    Check(GhostWhite);
    Check(Gold);
    Check(GoldenRod);
    Check(Gray);
    Check2(Grey, Gray);
    Check(Green);
    Check(GreenYellow);
    Check(HoneyDew);
    Check(HotPink);
    Check(IndianRed);
    Check(Indigo);
    Check(Ivory);
    Check(Khaki);
    Check(Lavender);
    Check(LavenderBlush);
    Check(LawnGreen);
    Check(LemonChiffon);
    Check(LightBlue);
    Check(LightCoral);
    Check(LightCyan);
    Check(LightGoldenRodYellow);
    Check(LightGray);
    Check2(LightGrey, LightGray);
    Check(LightGreen);
    Check(LightPink);
    Check(LightSalmon);
    Check(LightSeaGreen);
    Check(LightSkyBlue);
    Check(LightSlateGray);
    Check2(LightSlateGrey, LightSlateGray);
    Check(LightSteelBlue);
    Check(LightYellow);
    Check(Lime);
    Check(LimeGreen);
    Check(Linen);
    Check2(Magenta, Fuchsia);
    Check(Maroon);
    Check(MediumAquaMarine);
    Check(MediumBlue);
    Check(MediumOrchid);
    Check(MediumPurple);
    Check(MediumSeaGreen);
    Check(MediumSlateBlue);
    Check(MediumSpringGreen);
    Check(MediumTurquoise);
    Check(MediumVioletRed);
    Check(MidnightBlue);
    Check(MintCream);
    Check(MistyRose);
    Check(Moccasin);
    Check(NavajoWhite);
    Check(Navy);
    Check(OldLace);
    Check(Olive);
    Check(OliveDrab);
    Check(Orange);
    Check(OrangeRed);
    Check(Orchid);
    Check(PaleGoldenRod);
    Check(PaleGreen);
    Check(PaleTurquoise);
    Check(PaleVioletRed);
    Check(PapayaWhip);
    Check(PeachPuff);
    Check(Peru);
    Check(Pink);
    Check(Plum);
    Check(PowderBlue);
    Check(Purple);
    Check(RebeccaPurple);
    Check(Red);
    Check(RosyBrown);
    Check(RoyalBlue);
    Check(SaddleBrown);
    Check(Salmon);
    Check(SandyBrown);
    Check(SeaGreen);
    Check(SeaShell);
    Check(Sienna);
    Check(Silver);
    Check(SkyBlue);
    Check(SlateBlue);
    Check(SlateGray);
    Check2(SlateGrey, SlateGray);
    Check(Snow);
    Check(SpringGreen);
    Check(SteelBlue);
    Check(Tan);
    Check(Teal);
    Check(Thistle);
    Check(Tomato);
    Check(Turquoise);
    Check(Violet);
    Check(Wheat);
    Check(White);
    Check(WhiteSmoke);
    Check(Yellow);
    Check(YellowGreen);
  }
}
