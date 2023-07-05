#include <FoundationTest/FoundationTestPCH.h>

//////////////////////////////////////////////////////////////////////
// Start of the definition of a example Enum
// It takes quite some lines of code to define a enum,
// but it could be encapsulated into an preprocessor macro if wanted
struct wdTestEnumBase
{
  typedef wdUInt8 StorageType; // The storage type for the enum

  enum Enum
  {
    No = 0,
    Yes = 1,
    Default = No // Default initialization
  };
};

typedef wdEnum<wdTestEnumBase // The base for the enum
  >
  wdTestEnum; // The name of the final enum
// End of the definition of a example enum
///////////////////////////////////////////////////////////////////////

struct wdTestEnum2Base
{
  typedef wdUInt16 StorageType;

  enum Enum
  {
    Bit1 = WD_BIT(0),
    Bit2 = WD_BIT(1),
    Default = Bit1
  };
};

typedef wdEnum<wdTestEnum2Base> wdTestEnum2;

// Test if the type actually has the requested size
WD_CHECK_AT_COMPILETIME(sizeof(wdTestEnum) == sizeof(wdUInt8));
WD_CHECK_AT_COMPILETIME(sizeof(wdTestEnum2) == sizeof(wdUInt16));

WD_CREATE_SIMPLE_TEST_GROUP(Basics);

// This takes a c++ enum. Tests the implict conversion
void TakeEnum1(wdTestEnum::Enum value) {}

// This takes our own enum type
void TakeEnum2(wdTestEnum value) {}

WD_CREATE_SIMPLE_TEST(Basics, Enum)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Default initialized enum") { wdTestEnum e1; }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Enum with explicit initialization") { wdTestEnum e2(wdTestEnum::Yes); }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "This tests if the default initialization works and if the implicit conversion works")
  {
    wdTestEnum e1;
    wdTestEnum e2(wdTestEnum::Yes);

    WD_TEST_BOOL(e1 == wdTestEnum::No);
    WD_TEST_BOOL(e2 == wdTestEnum::Yes);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Function call tests")
  {
    wdTestEnum e1;

    TakeEnum1(e1);
    TakeEnum2(e1);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetValue and SetValue")
  {
    wdTestEnum e1;
    WD_TEST_INT(e1.GetValue(), 0);
    e1.SetValue(17);
    WD_TEST_INT(e1.GetValue(), 17);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Assignment of different values")
  {
    wdTestEnum e1, e2;

    e1 = wdTestEnum::Yes;
    e2 = wdTestEnum::No;
    WD_TEST_BOOL(e1 == wdTestEnum::Yes);
    WD_TEST_BOOL(e2 == wdTestEnum::No);

    e1 = e2;
    WD_TEST_BOOL(e1 == wdTestEnum::No);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Test the | operator")
  {
    wdTestEnum2 e3(wdTestEnum2::Bit1);
    wdTestEnum2 e4(wdTestEnum2::Bit2);
    wdUInt16 uiBits = (e3 | e4).GetValue();
    WD_TEST_BOOL(uiBits == (wdTestEnum2::Bit1 | wdTestEnum2::Bit2));
  }


  WD_TEST_BLOCK(wdTestBlock::Enabled, "Test the & operator")
  {
    wdTestEnum2 e3(wdTestEnum2::Bit1);
    wdTestEnum2 e4(wdTestEnum2::Bit2);
    wdUInt16 uiBits = ((e3 | e4) & e4).GetValue();
    WD_TEST_BOOL(uiBits == wdTestEnum2::Bit2);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Test conversion to int")
  {
    wdTestEnum e1;
    int iTest = e1.GetValue();
    WD_TEST_BOOL(iTest == wdTestEnum::No);
  }
}
