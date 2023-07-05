#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Reflection/Reflection.h>
#include <Foundation/Types/VarianceTypes.h>
#include <Foundation/Types/Variant.h>
#include <FoundationTest/Reflection/ReflectionTestClasses.h>

// this file takes ages to compile in a Release build
// since we don't care for runtime performance, just disable all optimizations
#pragma optimize("", off)

class Blubb : public wdReflectedClass
{
  WD_ADD_DYNAMIC_REFLECTION(Blubb, wdReflectedClass);

public:
  float u;
  float v;
};

// clang-format off
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(Blubb, 1, wdRTTINoAllocator)
{
  WD_BEGIN_PROPERTIES
  {
    WD_MEMBER_PROPERTY("u", u),
    WD_MEMBER_PROPERTY("v", v),
  }
  WD_END_PROPERTIES;
}
WD_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

template <typename T>
void TestVariant(wdVariant& v, wdVariantType::Enum type)
{
  WD_TEST_BOOL(v.IsValid());
  WD_TEST_BOOL(v.GetType() == type);
  WD_TEST_BOOL(v.CanConvertTo<T>());
  WD_TEST_BOOL(v.IsA<T>());
  WD_TEST_BOOL(v.GetReflectedType() == wdGetStaticRTTI<T>());

  wdTypedPointer ptr = v.GetWriteAccess();
  WD_TEST_BOOL(ptr.m_pObject == &v.Get<T>());
  WD_TEST_BOOL(ptr.m_pObject == &v.GetWritable<T>());
  WD_TEST_BOOL(ptr.m_pType == wdGetStaticRTTI<T>());

  WD_TEST_BOOL(ptr.m_pObject == v.GetData());

  wdVariant vCopy = v;
  wdTypedPointer ptr2 = vCopy.GetWriteAccess();
  WD_TEST_BOOL(ptr2.m_pObject == &vCopy.Get<T>());
  WD_TEST_BOOL(ptr2.m_pObject == &vCopy.GetWritable<T>());

  WD_TEST_BOOL(ptr2.m_pObject != ptr.m_pObject);
  WD_TEST_BOOL(ptr2.m_pType == wdGetStaticRTTI<T>());

  WD_TEST_BOOL(v.Get<T>() == vCopy.Get<T>());

  WD_TEST_BOOL(v.ComputeHash(0) != 0);
}

template <typename T>
inline void TestIntegerVariant(wdVariant::Type::Enum type)
{
  wdVariant b((T)23);
  TestVariant<T>(b, type);

  WD_TEST_BOOL(b.Get<T>() == 23);

  WD_TEST_BOOL(b == wdVariant(23));
  WD_TEST_BOOL(b != wdVariant(11));
  WD_TEST_BOOL(b == wdVariant((T)23));
  WD_TEST_BOOL(b != wdVariant((T)11));

  WD_TEST_BOOL(b == 23);
  WD_TEST_BOOL(b != 24);
  WD_TEST_BOOL(b == (T)23);
  WD_TEST_BOOL(b != (T)24);

  b = (T)17;
  WD_TEST_BOOL(b == (T)17);

  b = wdVariant((T)19);
  WD_TEST_BOOL(b == (T)19);

  WD_TEST_BOOL(b.IsNumber());
  WD_TEST_BOOL(b.IsFloatingPoint() == false);
  WD_TEST_BOOL(!b.IsString());
}

inline void TestNumberCanConvertTo(const wdVariant& v)
{
  WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Invalid) == false);
  WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Bool));
  WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Int8));
  WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::UInt8));
  WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Int16));
  WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::UInt16));
  WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Int32));
  WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::UInt32));
  WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Int64));
  WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::UInt64));
  WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Float));
  WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Double));
  WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Color) == false);
  WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Vector2) == false);
  WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Vector3) == false);
  WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Vector4) == false);
  WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Vector2I) == false);
  WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Vector3I) == false);
  WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Vector4I) == false);
  WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Quaternion) == false);
  WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Matrix3) == false);
  WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Matrix4) == false);
  WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Transform) == false);
  WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::String));
  WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::StringView) == false);
  WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::DataBuffer) == false);
  WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Time) == false);
  WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Angle) == false);
  WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Uuid) == false);
  WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::VariantArray) == false);
  WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::VariantDictionary) == false);
  WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::TypedPointer) == false);
  WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::TypedObject) == false);

  wdResult conversionResult = WD_FAILURE;
  WD_TEST_BOOL(v.ConvertTo<bool>(&conversionResult) == true);
  WD_TEST_BOOL(conversionResult.Succeeded());

  WD_TEST_BOOL(v.ConvertTo<wdInt8>(&conversionResult) == 3);
  WD_TEST_BOOL(conversionResult.Succeeded());

  WD_TEST_BOOL(v.ConvertTo<wdUInt8>(&conversionResult) == 3);
  WD_TEST_BOOL(conversionResult.Succeeded());

  WD_TEST_BOOL(v.ConvertTo<wdInt16>(&conversionResult) == 3);
  WD_TEST_BOOL(conversionResult.Succeeded());

  WD_TEST_BOOL(v.ConvertTo<wdUInt16>(&conversionResult) == 3);
  WD_TEST_BOOL(conversionResult.Succeeded());

  WD_TEST_BOOL(v.ConvertTo<wdInt32>(&conversionResult) == 3);
  WD_TEST_BOOL(conversionResult.Succeeded());

  WD_TEST_BOOL(v.ConvertTo<wdUInt32>(&conversionResult) == 3);
  WD_TEST_BOOL(conversionResult.Succeeded());

  WD_TEST_BOOL(v.ConvertTo<wdInt64>(&conversionResult) == 3);
  WD_TEST_BOOL(conversionResult.Succeeded());

  WD_TEST_BOOL(v.ConvertTo<wdUInt64>(&conversionResult) == 3);
  WD_TEST_BOOL(conversionResult.Succeeded());

  WD_TEST_BOOL(v.ConvertTo<float>(&conversionResult) == 3.0f);
  WD_TEST_BOOL(conversionResult.Succeeded());

  WD_TEST_BOOL(v.ConvertTo<double>(&conversionResult) == 3.0);
  WD_TEST_BOOL(conversionResult.Succeeded());

  WD_TEST_BOOL(v.ConvertTo<wdString>(&conversionResult) == "3");
  WD_TEST_BOOL(conversionResult.Succeeded());

  WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Bool).Get<bool>() == true);
  WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Int8).Get<wdInt8>() == 3);
  WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::UInt8).Get<wdUInt8>() == 3);
  WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Int16).Get<wdInt16>() == 3);
  WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::UInt16).Get<wdUInt16>() == 3);
  WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Int32).Get<wdInt32>() == 3);
  WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::UInt32).Get<wdUInt32>() == 3);
  WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Int64).Get<wdInt64>() == 3);
  WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::UInt64).Get<wdUInt64>() == 3);
  WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Float).Get<float>() == 3.0f);
  WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Double).Get<double>() == 3.0);
  WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::String).Get<wdString>() == "3");
}

inline void TestCanOnlyConvertToID(const wdVariant& v, wdVariant::Type::Enum type, bool bAndString = false)
{
  for (int iType = wdVariant::Type::FirstStandardType; iType < wdVariant::Type::LastExtendedType; ++iType)
  {
    if (iType == wdVariant::Type::LastStandardType)
      iType = wdVariant::Type::FirstExtendedType;

    if (iType == type)
    {
      WD_TEST_BOOL(v.CanConvertTo(type));
    }
    else if (bAndString && iType == wdVariant::Type::String)
    {
      WD_TEST_BOOL(v.CanConvertTo(type));
    }
    else
    {
      WD_TEST_BOOL(v.CanConvertTo((wdVariant::Type::Enum)iType) == false);
    }
  }
}

inline void TestCanOnlyConvertToStringAndID(const wdVariant& v, wdVariant::Type::Enum type, wdVariant::Type::Enum type2 = wdVariant::Type::Invalid,
  wdVariant::Type::Enum type3 = wdVariant::Type::Invalid)
{
  if (type2 == wdVariant::Type::Invalid)
    type2 = type;

  for (int iType = wdVariant::Type::FirstStandardType; iType < wdVariant::Type::LastExtendedType; ++iType)
  {
    if (iType == wdVariant::Type::LastStandardType)
      iType = wdVariant::Type::FirstExtendedType;

    if (iType == wdVariant::Type::String)
    {
      WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::String));
    }
    else if (iType == type || iType == type2 || iType == type3)
    {
      WD_TEST_BOOL(v.CanConvertTo(type));
    }
    else
    {
      WD_TEST_BOOL(v.CanConvertTo((wdVariant::Type::Enum)iType) == false);
    }
  }
}

WD_CREATE_SIMPLE_TEST(Basics, Variant)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Invalid")
  {
    wdVariant b;
    WD_TEST_BOOL(b.GetType() == wdVariant::Type::Invalid);
    WD_TEST_BOOL(b == wdVariant());
    WD_TEST_BOOL(b != wdVariant(0));
    WD_TEST_BOOL(!b.IsValid());
    WD_TEST_BOOL(!b[0].IsValid());
    WD_TEST_BOOL(!b["x"].IsValid());
    WD_TEST_BOOL(b.GetReflectedType() == nullptr);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "bool")
  {
    wdVariant b(true);
    TestVariant<bool>(b, wdVariantType::Bool);

    WD_TEST_BOOL(b.Get<bool>() == true);

    WD_TEST_BOOL(b == wdVariant(true));
    WD_TEST_BOOL(b != wdVariant(false));

    WD_TEST_BOOL(b == true);
    WD_TEST_BOOL(b != false);

    b = false;
    WD_TEST_BOOL(b == false);

    b = wdVariant(true);
    WD_TEST_BOOL(b == true);
    WD_TEST_BOOL(!b[0].IsValid());

    WD_TEST_BOOL(b.IsNumber());
    WD_TEST_BOOL(!b.IsString());
    WD_TEST_BOOL(b.IsFloatingPoint() == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdInt8")
  {
    TestIntegerVariant<wdInt8>(wdVariant::Type::Int8);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdUInt8")
  {
    TestIntegerVariant<wdUInt8>(wdVariant::Type::UInt8);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdInt16")
  {
    TestIntegerVariant<wdInt16>(wdVariant::Type::Int16);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdUInt16")
  {
    TestIntegerVariant<wdUInt16>(wdVariant::Type::UInt16);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdInt32")
  {
    TestIntegerVariant<wdInt32>(wdVariant::Type::Int32);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdUInt32")
  {
    TestIntegerVariant<wdUInt32>(wdVariant::Type::UInt32);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdInt64")
  {
    TestIntegerVariant<wdInt64>(wdVariant::Type::Int64);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdUInt64")
  {
    TestIntegerVariant<wdUInt64>(wdVariant::Type::UInt64);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "float")
  {
    wdVariant b(42.0f);
    TestVariant<float>(b, wdVariantType::Float);

    WD_TEST_BOOL(b.Get<float>() == 42.0f);

    WD_TEST_BOOL(b == wdVariant(42));
    WD_TEST_BOOL(b != wdVariant(11));
    WD_TEST_BOOL(b == wdVariant(42.0));
    WD_TEST_BOOL(b != wdVariant(11.0));
    WD_TEST_BOOL(b == wdVariant(42.0f));
    WD_TEST_BOOL(b != wdVariant(11.0f));

    WD_TEST_BOOL(b == 42);
    WD_TEST_BOOL(b != 41);
    WD_TEST_BOOL(b == 42.0);
    WD_TEST_BOOL(b != 41.0);
    WD_TEST_BOOL(b == 42.0f);
    WD_TEST_BOOL(b != 41.0f);

    b = 17.0f;
    WD_TEST_BOOL(b == 17.0f);

    b = wdVariant(19.0f);
    WD_TEST_BOOL(b == 19.0f);

    WD_TEST_BOOL(b.IsNumber());
    WD_TEST_BOOL(!b.IsString());
    WD_TEST_BOOL(b.IsFloatingPoint());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "double")
  {
    wdVariant b(42.0);
    TestVariant<double>(b, wdVariantType::Double);
    WD_TEST_BOOL(b.Get<double>() == 42.0);

    WD_TEST_BOOL(b == wdVariant(42));
    WD_TEST_BOOL(b != wdVariant(11));
    WD_TEST_BOOL(b == wdVariant(42.0));
    WD_TEST_BOOL(b != wdVariant(11.0));
    WD_TEST_BOOL(b == wdVariant(42.0f));
    WD_TEST_BOOL(b != wdVariant(11.0f));

    WD_TEST_BOOL(b == 42);
    WD_TEST_BOOL(b != 41);
    WD_TEST_BOOL(b == 42.0);
    WD_TEST_BOOL(b != 41.0);
    WD_TEST_BOOL(b == 42.0f);
    WD_TEST_BOOL(b != 41.0f);

    b = 17.0;
    WD_TEST_BOOL(b == 17.0);

    b = wdVariant(19.0);
    WD_TEST_BOOL(b == 19.0);

    WD_TEST_BOOL(b.IsNumber());
    WD_TEST_BOOL(!b.IsString());
    WD_TEST_BOOL(b.IsFloatingPoint());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdColor")
  {
    wdVariant v(wdColor(1, 2, 3, 1));
    TestVariant<wdColor>(v, wdVariantType::Color);

    WD_TEST_BOOL(v.CanConvertTo<wdColorGammaUB>());
    WD_TEST_BOOL(v.ConvertTo<wdColorGammaUB>() == static_cast<wdColorGammaUB>(wdColor(1, 2, 3, 1)));
    WD_TEST_BOOL(v.Get<wdColor>() == wdColor(1, 2, 3, 1));

    WD_TEST_BOOL(v == wdVariant(wdColor(1, 2, 3)));
    WD_TEST_BOOL(v != wdVariant(wdColor(1, 1, 1)));

    WD_TEST_BOOL(v == wdColor(1, 2, 3));
    WD_TEST_BOOL(v != wdColor(1, 4, 3));

    v = wdColor(5, 8, 9);
    WD_TEST_BOOL(v == wdColor(5, 8, 9));

    v = wdVariant(wdColor(7, 9, 4));
    WD_TEST_BOOL(v == wdColor(7, 9, 4));
    WD_TEST_BOOL(v[0] == 7);
    WD_TEST_BOOL(v[1] == 9);
    WD_TEST_BOOL(v[2] == 4);
    WD_TEST_BOOL(v[3] == 1);
    WD_TEST_BOOL(v[4] == wdVariant());
    WD_TEST_BOOL(!v[4].IsValid());
    WD_TEST_BOOL(v["r"] == 7);
    WD_TEST_BOOL(v["g"] == 9);
    WD_TEST_BOOL(v["b"] == 4);
    WD_TEST_BOOL(v["a"] == 1);
    WD_TEST_BOOL(v["x"] == wdVariant());
    WD_TEST_BOOL(!v["x"].IsValid());

    WD_TEST_BOOL(v.IsNumber() == false);
    WD_TEST_BOOL(!v.IsString());
    WD_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdColorGammaUB")
  {
    wdVariant v(wdColorGammaUB(64, 128, 255, 255));
    TestVariant<wdColorGammaUB>(v, wdVariantType::ColorGamma);

    WD_TEST_BOOL(v.CanConvertTo<wdColor>());
    WD_TEST_BOOL(v.Get<wdColorGammaUB>() == wdColorGammaUB(64, 128, 255, 255));

    WD_TEST_BOOL(v == wdVariant(wdColorGammaUB(64, 128, 255, 255)));
    WD_TEST_BOOL(v != wdVariant(wdColorGammaUB(255, 128, 255, 255)));

    WD_TEST_BOOL(v == wdColorGammaUB(64, 128, 255, 255));
    WD_TEST_BOOL(v != wdColorGammaUB(64, 42, 255, 255));

    v = wdColorGammaUB(10, 50, 200);
    WD_TEST_BOOL(v == wdColorGammaUB(10, 50, 200));

    v = wdVariant(wdColorGammaUB(17, 120, 200));
    WD_TEST_BOOL(v == wdColorGammaUB(17, 120, 200));
    WD_TEST_BOOL(v[0] == 17);
    WD_TEST_BOOL(v[1] == 120);
    WD_TEST_BOOL(v[2] == 200);
    WD_TEST_BOOL(v[3] == 255);
    WD_TEST_BOOL(v[4] == wdVariant());
    WD_TEST_BOOL(!v[4].IsValid());
    WD_TEST_BOOL(v["r"] == 17);
    WD_TEST_BOOL(v["g"] == 120);
    WD_TEST_BOOL(v["b"] == 200);
    WD_TEST_BOOL(v["a"] == 255);
    WD_TEST_BOOL(v["x"] == wdVariant());
    WD_TEST_BOOL(!v["x"].IsValid());

    WD_TEST_BOOL(v.IsNumber() == false);
    WD_TEST_BOOL(!v.IsString());
    WD_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdVec2")
  {
    wdVariant v(wdVec2(1, 2));
    TestVariant<wdVec2>(v, wdVariantType::Vector2);

    WD_TEST_BOOL(v.Get<wdVec2>() == wdVec2(1, 2));

    WD_TEST_BOOL(v == wdVariant(wdVec2(1, 2)));
    WD_TEST_BOOL(v != wdVariant(wdVec2(1, 1)));

    WD_TEST_BOOL(v == wdVec2(1, 2));
    WD_TEST_BOOL(v != wdVec2(1, 4));

    v = wdVec2(5, 8);
    WD_TEST_BOOL(v == wdVec2(5, 8));

    v = wdVariant(wdVec2(7, 9));
    WD_TEST_BOOL(v == wdVec2(7, 9));
    WD_TEST_BOOL(v[0] == 7);
    WD_TEST_BOOL(v[1] == 9);
    WD_TEST_BOOL(v["x"] == 7);
    WD_TEST_BOOL(v["y"] == 9);

    WD_TEST_BOOL(v.IsNumber() == false);
    WD_TEST_BOOL(!v.IsString());
    WD_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdVec3")
  {
    wdVariant v(wdVec3(1, 2, 3));
    TestVariant<wdVec3>(v, wdVariantType::Vector3);

    WD_TEST_BOOL(v.Get<wdVec3>() == wdVec3(1, 2, 3));

    WD_TEST_BOOL(v == wdVariant(wdVec3(1, 2, 3)));
    WD_TEST_BOOL(v != wdVariant(wdVec3(1, 1, 3)));

    WD_TEST_BOOL(v == wdVec3(1, 2, 3));
    WD_TEST_BOOL(v != wdVec3(1, 4, 3));

    v = wdVec3(5, 8, 9);
    WD_TEST_BOOL(v == wdVec3(5, 8, 9));

    v = wdVariant(wdVec3(7, 9, 8));
    WD_TEST_BOOL(v == wdVec3(7, 9, 8));
    WD_TEST_BOOL(v[0] == 7);
    WD_TEST_BOOL(v[1] == 9);
    WD_TEST_BOOL(v[2] == 8);
    WD_TEST_BOOL(v["x"] == 7);
    WD_TEST_BOOL(v["y"] == 9);
    WD_TEST_BOOL(v["z"] == 8);

    WD_TEST_BOOL(v.IsNumber() == false);
    WD_TEST_BOOL(!v.IsString());
    WD_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdVec4")
  {
    wdVariant v(wdVec4(1, 2, 3, 4));
    TestVariant<wdVec4>(v, wdVariantType::Vector4);

    WD_TEST_BOOL(v.Get<wdVec4>() == wdVec4(1, 2, 3, 4));

    WD_TEST_BOOL(v == wdVariant(wdVec4(1, 2, 3, 4)));
    WD_TEST_BOOL(v != wdVariant(wdVec4(1, 1, 3, 4)));

    WD_TEST_BOOL(v == wdVec4(1, 2, 3, 4));
    WD_TEST_BOOL(v != wdVec4(1, 4, 3, 4));

    v = wdVec4(5, 8, 9, 3);
    WD_TEST_BOOL(v == wdVec4(5, 8, 9, 3));

    v = wdVariant(wdVec4(7, 9, 8, 4));
    WD_TEST_BOOL(v == wdVec4(7, 9, 8, 4));
    WD_TEST_BOOL(v[0] == 7);
    WD_TEST_BOOL(v[1] == 9);
    WD_TEST_BOOL(v[2] == 8);
    WD_TEST_BOOL(v[3] == 4);
    WD_TEST_BOOL(v["x"] == 7);
    WD_TEST_BOOL(v["y"] == 9);
    WD_TEST_BOOL(v["z"] == 8);
    WD_TEST_BOOL(v["w"] == 4);

    WD_TEST_BOOL(v.IsNumber() == false);
    WD_TEST_BOOL(!v.IsString());
    WD_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdVec2I32")
  {
    wdVariant v(wdVec2I32(1, 2));
    TestVariant<wdVec2I32>(v, wdVariantType::Vector2I);

    WD_TEST_BOOL(v.Get<wdVec2I32>() == wdVec2I32(1, 2));

    WD_TEST_BOOL(v == wdVariant(wdVec2I32(1, 2)));
    WD_TEST_BOOL(v != wdVariant(wdVec2I32(1, 1)));

    WD_TEST_BOOL(v == wdVec2I32(1, 2));
    WD_TEST_BOOL(v != wdVec2I32(1, 4));

    v = wdVec2I32(5, 8);
    WD_TEST_BOOL(v == wdVec2I32(5, 8));

    v = wdVariant(wdVec2I32(7, 9));
    WD_TEST_BOOL(v == wdVec2I32(7, 9));
    WD_TEST_BOOL(v[0] == 7);
    WD_TEST_BOOL(v[1] == 9);
    WD_TEST_BOOL(v["x"] == 7);
    WD_TEST_BOOL(v["y"] == 9);

    WD_TEST_BOOL(v.IsNumber() == false);
    WD_TEST_BOOL(!v.IsString());
    WD_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdVec3I32")
  {
    wdVariant v(wdVec3I32(1, 2, 3));
    TestVariant<wdVec3I32>(v, wdVariantType::Vector3I);

    WD_TEST_BOOL(v.Get<wdVec3I32>() == wdVec3I32(1, 2, 3));

    WD_TEST_BOOL(v == wdVariant(wdVec3I32(1, 2, 3)));
    WD_TEST_BOOL(v != wdVariant(wdVec3I32(1, 1, 3)));

    WD_TEST_BOOL(v == wdVec3I32(1, 2, 3));
    WD_TEST_BOOL(v != wdVec3I32(1, 4, 3));

    v = wdVec3I32(5, 8, 9);
    WD_TEST_BOOL(v == wdVec3I32(5, 8, 9));

    v = wdVariant(wdVec3I32(7, 9, 8));
    WD_TEST_BOOL(v == wdVec3I32(7, 9, 8));
    WD_TEST_BOOL(v[0] == 7);
    WD_TEST_BOOL(v[1] == 9);
    WD_TEST_BOOL(v[2] == 8);
    WD_TEST_BOOL(v["x"] == 7);
    WD_TEST_BOOL(v["y"] == 9);
    WD_TEST_BOOL(v["z"] == 8);

    WD_TEST_BOOL(v.IsNumber() == false);
    WD_TEST_BOOL(!v.IsString());
    WD_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdVec4I32")
  {
    wdVariant v(wdVec4I32(1, 2, 3, 4));
    TestVariant<wdVec4I32>(v, wdVariantType::Vector4I);

    WD_TEST_BOOL(v.Get<wdVec4I32>() == wdVec4I32(1, 2, 3, 4));

    WD_TEST_BOOL(v == wdVariant(wdVec4I32(1, 2, 3, 4)));
    WD_TEST_BOOL(v != wdVariant(wdVec4I32(1, 1, 3, 4)));

    WD_TEST_BOOL(v == wdVec4I32(1, 2, 3, 4));
    WD_TEST_BOOL(v != wdVec4I32(1, 4, 3, 4));

    v = wdVec4I32(5, 8, 9, 3);
    WD_TEST_BOOL(v == wdVec4I32(5, 8, 9, 3));

    v = wdVariant(wdVec4I32(7, 9, 8, 4));
    WD_TEST_BOOL(v == wdVec4I32(7, 9, 8, 4));
    WD_TEST_BOOL(v[0] == 7);
    WD_TEST_BOOL(v[1] == 9);
    WD_TEST_BOOL(v[2] == 8);
    WD_TEST_BOOL(v[3] == 4);
    WD_TEST_BOOL(v["x"] == 7);
    WD_TEST_BOOL(v["y"] == 9);
    WD_TEST_BOOL(v["z"] == 8);
    WD_TEST_BOOL(v["w"] == 4);

    WD_TEST_BOOL(v.IsNumber() == false);
    WD_TEST_BOOL(!v.IsString());
    WD_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdQuat")
  {
    wdVariant v(wdQuat(1, 2, 3, 4));
    TestVariant<wdQuat>(v, wdVariantType::Quaternion);

    WD_TEST_BOOL(v.Get<wdQuat>() == wdQuat(1, 2, 3, 4));

    WD_TEST_BOOL(v == wdQuat(1, 2, 3, 4));
    WD_TEST_BOOL(v != wdQuat(1, 2, 3, 5));

    WD_TEST_BOOL(v == wdQuat(1, 2, 3, 4));
    WD_TEST_BOOL(v != wdQuat(1, 4, 3, 4));

    v = wdQuat(5, 8, 9, 3);
    WD_TEST_BOOL(v == wdQuat(5, 8, 9, 3));

    v = wdVariant(wdQuat(7, 9, 8, 4));
    WD_TEST_BOOL(v == wdQuat(7, 9, 8, 4));
    WD_TEST_BOOL(v[0][0] == 7);
    WD_TEST_BOOL(v[0][1] == 9);
    WD_TEST_BOOL(v[0][2] == 8);
    WD_TEST_BOOL(v[1] == 4);
    WD_TEST_BOOL(v["v"]["x"] == 7);
    WD_TEST_BOOL(v["v"]["y"] == 9);
    WD_TEST_BOOL(v["v"]["z"] == 8);
    WD_TEST_BOOL(v["w"] == 4);

    WD_TEST_BOOL(v.IsNumber() == false);
    WD_TEST_BOOL(!v.IsString());
    WD_TEST_BOOL(v.IsFloatingPoint() == false);

    wdTypedPointer ptr = v.GetWriteAccess();
    WD_TEST_BOOL(ptr.m_pObject == &v.Get<wdQuat>());
    WD_TEST_BOOL(ptr.m_pObject == &v.GetWritable<wdQuat>());
    WD_TEST_BOOL(ptr.m_pType == wdGetStaticRTTI<wdQuat>());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdMat3")
  {
    wdVariant v(wdMat3(1, 2, 3, 4, 5, 6, 7, 8, 9));
    TestVariant<wdMat3>(v, wdVariantType::Matrix3);

    WD_TEST_BOOL(v.Get<wdMat3>() == wdMat3(1, 2, 3, 4, 5, 6, 7, 8, 9));

    WD_TEST_BOOL(v == wdVariant(wdMat3(1, 2, 3, 4, 5, 6, 7, 8, 9)));
    WD_TEST_BOOL(v != wdVariant(wdMat3(1, 2, 3, 4, 5, 6, 7, 8, 8)));

    WD_TEST_BOOL(v == wdMat3(1, 2, 3, 4, 5, 6, 7, 8, 9));
    WD_TEST_BOOL(v != wdMat3(1, 2, 3, 4, 5, 6, 7, 8, 8));

    v = wdMat3(5, 8, 9, 3, 1, 2, 3, 4, 5);
    WD_TEST_BOOL(v == wdMat3(5, 8, 9, 3, 1, 2, 3, 4, 5));

    v = wdVariant(wdMat3(5, 8, 9, 3, 1, 2, 3, 4, 4));
    WD_TEST_BOOL(v == wdMat3(5, 8, 9, 3, 1, 2, 3, 4, 4));

    WD_TEST_BOOL(v.IsNumber() == false);
    WD_TEST_BOOL(!v.IsString());
    WD_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdMat4")
  {
    wdVariant v(wdMat4(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));
    TestVariant<wdMat4>(v, wdVariantType::Matrix4);

    WD_TEST_BOOL(v.Get<wdMat4>() == wdMat4(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));

    WD_TEST_BOOL(v == wdVariant(wdMat4(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16)));
    WD_TEST_BOOL(v != wdVariant(wdMat4(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 15)));

    WD_TEST_BOOL(v == wdMat4(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));
    WD_TEST_BOOL(v != wdMat4(1, 2, 3, 4, 5, 6, 2, 8, 9, 10, 11, 12, 13, 14, 15, 16));

    v = wdMat4(5, 8, 9, 3, 1, 2, 3, 4, 5, 3, 7, 3, 6, 8, 6, 8);
    WD_TEST_BOOL(v == wdMat4(5, 8, 9, 3, 1, 2, 3, 4, 5, 3, 7, 3, 6, 8, 6, 8));

    v = wdVariant(wdMat4(5, 8, 9, 3, 1, 2, 1, 4, 5, 3, 7, 3, 6, 8, 6, 8));
    WD_TEST_BOOL(v == wdMat4(5, 8, 9, 3, 1, 2, 1, 4, 5, 3, 7, 3, 6, 8, 6, 8));

    WD_TEST_BOOL(v.IsNumber() == false);
    WD_TEST_BOOL(!v.IsString());
    WD_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdTransform")
  {
    wdVariant v(wdTransform(wdVec3(1, 2, 3), wdQuat(4, 5, 6, 7), wdVec3(8, 9, 10)));
    TestVariant<wdTransform>(v, wdVariantType::Transform);

    WD_TEST_BOOL(v.Get<wdTransform>() == wdTransform(wdVec3(1, 2, 3), wdQuat(4, 5, 6, 7), wdVec3(8, 9, 10)));

    WD_TEST_BOOL(v == wdTransform(wdVec3(1, 2, 3), wdQuat(4, 5, 6, 7), wdVec3(8, 9, 10)));
    WD_TEST_BOOL(v != wdTransform(wdVec3(1, 2, 3), wdQuat(4, 5, 6, 7), wdVec3(8, 9, 11)));

    v = wdTransform(wdVec3(5, 8, 9), wdQuat(3, 1, 2, 3), wdVec3(4, 5, 3));
    WD_TEST_BOOL(v == wdTransform(wdVec3(5, 8, 9), wdQuat(3, 1, 2, 3), wdVec3(4, 5, 3)));

    WD_TEST_BOOL(v.IsNumber() == false);
    WD_TEST_BOOL(!v.IsString());
    WD_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "const char*")
  {
    wdVariant v("This is a const char array");
    TestVariant<wdString>(v, wdVariantType::String);

    WD_TEST_BOOL(v.IsA<const char*>());
    WD_TEST_BOOL(v.IsA<char*>());
    WD_TEST_BOOL(v.Get<wdString>() == wdString("This is a const char array"));

    WD_TEST_BOOL(v == wdVariant("This is a const char array"));
    WD_TEST_BOOL(v != wdVariant("This is something else"));

    WD_TEST_BOOL(v == wdString("This is a const char array"));
    WD_TEST_BOOL(v != wdString("This is another string"));

    WD_TEST_BOOL(v == "This is a const char array");
    WD_TEST_BOOL(v != "This is another string");

    WD_TEST_BOOL(v == (const char*)"This is a const char array");
    WD_TEST_BOOL(v != (const char*)"This is another string");

    v = "blurg!";
    WD_TEST_BOOL(v == wdString("blurg!"));

    v = wdVariant("blärg!");
    WD_TEST_BOOL(v == wdString("blärg!"));

    WD_TEST_BOOL(v.IsNumber() == false);
    WD_TEST_BOOL(v.IsString());
    WD_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdString")
  {
    wdVariant v(wdString("This is an wdString"));
    TestVariant<wdString>(v, wdVariantType::String);

    WD_TEST_BOOL(v.Get<wdString>() == wdString("This is an wdString"));

    WD_TEST_BOOL(v == wdVariant(wdString("This is an wdString")));
    WD_TEST_BOOL(v != wdVariant(wdString("This is something else")));

    WD_TEST_BOOL(v == wdString("This is an wdString"));
    WD_TEST_BOOL(v != wdString("This is another wdString"));

    v = wdString("blurg!");
    WD_TEST_BOOL(v == wdString("blurg!"));

    v = wdVariant(wdString("blärg!"));
    WD_TEST_BOOL(v == wdString("blärg!"));

    WD_TEST_BOOL(v.IsNumber() == false);
    WD_TEST_BOOL(v.IsString());
    WD_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdStringView")
  {
    const char* szTemp = "This is an wdStringView";
    wdStringView bla(szTemp);
    wdVariant v(bla);
    TestVariant<wdStringView>(v, wdVariantType::StringView);

    const wdString sCopy = szTemp;
    WD_TEST_BOOL(v.Get<wdStringView>() == sCopy);

    WD_TEST_BOOL(v == wdVariant(wdStringView(sCopy.GetData())));
    WD_TEST_BOOL(v != wdVariant(wdStringView("This is something else")));

    WD_TEST_BOOL(v == wdStringView(sCopy.GetData()));
    WD_TEST_BOOL(v != wdStringView("This is something else"));

    v = wdStringView("blurg!");
    WD_TEST_BOOL(v == wdStringView("blurg!"));

    WD_TEST_BOOL(v.IsNumber() == false);
    WD_TEST_BOOL(v.IsString());
    WD_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdDataBuffer")
  {
    wdDataBuffer a, a2;
    a.PushBack(wdUInt8(1));
    a.PushBack(wdUInt8(2));
    a.PushBack(wdUInt8(255));

    wdVariant va(a);
    TestVariant<wdDataBuffer>(va, wdVariantType::DataBuffer);

    const wdDataBuffer& b = va.Get<wdDataBuffer>();
    wdArrayPtr<const wdUInt8> b2 = va.Get<wdDataBuffer>();

    WD_TEST_BOOL(a == b);
    WD_TEST_BOOL(a == b2);

    WD_TEST_BOOL(a != a2);

    WD_TEST_BOOL(va == a);
    WD_TEST_BOOL(va != a2);

    WD_TEST_BOOL(va.IsNumber() == false);
    WD_TEST_BOOL(!va.IsString());
    WD_TEST_BOOL(va.IsFloatingPoint() == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdTime")
  {
    wdVariant v(wdTime::Seconds(1337));
    TestVariant<wdTime>(v, wdVariantType::Time);

    WD_TEST_BOOL(v.Get<wdTime>() == wdTime::Seconds(1337));

    WD_TEST_BOOL(v == wdVariant(wdTime::Seconds(1337)));
    WD_TEST_BOOL(v != wdVariant(wdTime::Seconds(1336)));

    WD_TEST_BOOL(v == wdTime::Seconds(1337));
    WD_TEST_BOOL(v != wdTime::Seconds(1338));

    v = wdTime::Seconds(8472);
    WD_TEST_BOOL(v == wdTime::Seconds(8472));

    v = wdVariant(wdTime::Seconds(13));
    WD_TEST_BOOL(v == wdTime::Seconds(13));

    WD_TEST_BOOL(v.IsNumber() == false);
    WD_TEST_BOOL(!v.IsString());
    WD_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdUuid")
  {
    wdUuid id;
    wdVariant v(id);
    TestVariant<wdUuid>(v, wdVariantType::Uuid);

    WD_TEST_BOOL(v.Get<wdUuid>() == wdUuid());

    wdUuid uuid;
    uuid.CreateNewUuid();
    WD_TEST_BOOL(v != wdVariant(uuid));
    WD_TEST_BOOL(wdVariant(uuid).Get<wdUuid>() == uuid);

    wdUuid uuid2;
    uuid2.CreateNewUuid();
    WD_TEST_BOOL(wdVariant(uuid) != wdVariant(uuid2));

    WD_TEST_BOOL(v.IsNumber() == false);
    WD_TEST_BOOL(!v.IsString());
    WD_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdAngle")
  {
    wdVariant v(wdAngle::Degree(1337));
    TestVariant<wdAngle>(v, wdVariantType::Angle);

    WD_TEST_BOOL(v.Get<wdAngle>() == wdAngle::Degree(1337));

    WD_TEST_BOOL(v == wdVariant(wdAngle::Degree(1337)));
    WD_TEST_BOOL(v != wdVariant(wdAngle::Degree(1336)));

    WD_TEST_BOOL(v == wdAngle::Degree(1337));
    WD_TEST_BOOL(v != wdAngle::Degree(1338));

    v = wdAngle::Degree(8472);
    WD_TEST_BOOL(v == wdAngle::Degree(8472));

    v = wdVariant(wdAngle::Degree(13));
    WD_TEST_BOOL(v == wdAngle::Degree(13));

    WD_TEST_BOOL(v.IsNumber() == false);
    WD_TEST_BOOL(!v.IsString());
    WD_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdVariantArray")
  {
    wdVariantArray a, a2;
    a.PushBack("This");
    a.PushBack("is a");
    a.PushBack("test");

    wdVariant va(a);
    WD_TEST_BOOL(va.IsValid());
    WD_TEST_BOOL(va.GetType() == wdVariant::Type::VariantArray);
    WD_TEST_BOOL(va.IsA<wdVariantArray>());
    WD_TEST_BOOL(va.GetReflectedType() == nullptr);

    const wdArrayPtr<const wdVariant>& b = va.Get<wdVariantArray>();
    wdArrayPtr<const wdVariant> b2 = va.Get<wdVariantArray>();

    WD_TEST_BOOL(a == b);
    WD_TEST_BOOL(a == b2);

    WD_TEST_BOOL(a != a2);

    WD_TEST_BOOL(va == a);
    WD_TEST_BOOL(va != a2);

    WD_TEST_BOOL(va[0] == wdString("This"));
    WD_TEST_BOOL(va[1] == wdString("is a"));
    WD_TEST_BOOL(va[2] == wdString("test"));
    WD_TEST_BOOL(va[4] == wdVariant());
    WD_TEST_BOOL(!va[4].IsValid());

    WD_TEST_BOOL(va.IsNumber() == false);
    WD_TEST_BOOL(!va.IsString());
    WD_TEST_BOOL(va.IsFloatingPoint() == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdVariantDictionary")
  {
    wdVariantDictionary a, a2;
    a["my"] = true;
    a["luv"] = 4;
    a["pon"] = "ies";

    wdVariant va(a);
    WD_TEST_BOOL(va.IsValid());
    WD_TEST_BOOL(va.GetType() == wdVariant::Type::VariantDictionary);
    WD_TEST_BOOL(va.IsA<wdVariantDictionary>());
    WD_TEST_BOOL(va.GetReflectedType() == nullptr);

    const wdVariantDictionary& d1 = va.Get<wdVariantDictionary>();
    wdVariantDictionary d2 = va.Get<wdVariantDictionary>();

    WD_TEST_BOOL(a == d1);
    WD_TEST_BOOL(a == d2);
    WD_TEST_BOOL(d1 == d2);

    WD_TEST_BOOL(va == a);
    WD_TEST_BOOL(va != a2);

    WD_TEST_BOOL(va["my"] == true);
    WD_TEST_BOOL(va["luv"] == 4);
    WD_TEST_BOOL(va["pon"] == wdString("ies"));
    WD_TEST_BOOL(va["x"] == wdVariant());
    WD_TEST_BOOL(!va["x"].IsValid());

    WD_TEST_BOOL(va.IsNumber() == false);
    WD_TEST_BOOL(!va.IsString());
    WD_TEST_BOOL(va.IsFloatingPoint() == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdTypedPointer")
  {
    Blubb blubb;
    blubb.u = 1.0f;
    blubb.v = 2.0f;

    Blubb blubb2;

    wdVariant v(&blubb);

    WD_TEST_BOOL(v.IsValid());
    WD_TEST_BOOL(v.GetType() == wdVariant::Type::TypedPointer);
    WD_TEST_BOOL(v.IsA<Blubb*>());
    WD_TEST_BOOL(v.Get<Blubb*>() == &blubb);
    WD_TEST_BOOL(v.IsA<wdReflectedClass*>());
    WD_TEST_BOOL(v.Get<wdReflectedClass*>() == &blubb);
    WD_TEST_BOOL(v.Get<wdReflectedClass*>() != &blubb2);
    WD_TEST_BOOL(wdDynamicCast<Blubb*>(v) == &blubb);
    WD_TEST_BOOL(wdDynamicCast<wdVec3*>(v) == nullptr);
    WD_TEST_BOOL(v.IsA<void*>());
    WD_TEST_BOOL(v.Get<void*>() == &blubb);
    WD_TEST_BOOL(v.IsA<const void*>());
    WD_TEST_BOOL(v.Get<const void*>() == &blubb);
    WD_TEST_BOOL(v.GetData() == &blubb);
    WD_TEST_BOOL(v.IsA<wdTypedPointer>());
    WD_TEST_BOOL(v.GetReflectedType() == wdGetStaticRTTI<Blubb>());
    WD_TEST_BOOL(!v.IsA<wdVec3*>());

    wdTypedPointer ptr = v.Get<wdTypedPointer>();
    WD_TEST_BOOL(ptr.m_pObject == &blubb);
    WD_TEST_BOOL(ptr.m_pType == wdGetStaticRTTI<Blubb>());

    wdTypedPointer ptr2 = v.GetWriteAccess();
    WD_TEST_BOOL(ptr2.m_pObject == &blubb);
    WD_TEST_BOOL(ptr2.m_pType == wdGetStaticRTTI<Blubb>());

    WD_TEST_BOOL(v[0] == 1.0f);
    WD_TEST_BOOL(v[1] == 2.0f);
    WD_TEST_BOOL(v["u"] == 1.0f);
    WD_TEST_BOOL(v["v"] == 2.0f);
    wdVariant v2 = &blubb;
    WD_TEST_BOOL(v == v2);
    wdVariant v3 = ptr;
    WD_TEST_BOOL(v == v3);

    WD_TEST_BOOL(v.IsNumber() == false);
    WD_TEST_BOOL(!v.IsString());
    WD_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdTypedPointer nullptr")
  {
    wdTypedPointer ptr = {nullptr, wdGetStaticRTTI<Blubb>()};
    wdVariant v = ptr;
    WD_TEST_BOOL(v.IsValid());
    WD_TEST_BOOL(v.GetType() == wdVariant::Type::TypedPointer);
    WD_TEST_BOOL(v.IsA<Blubb*>());
    WD_TEST_BOOL(v.Get<Blubb*>() == nullptr);
    WD_TEST_BOOL(v.IsA<wdReflectedClass*>());
    WD_TEST_BOOL(v.Get<wdReflectedClass*>() == nullptr);
    WD_TEST_BOOL(wdDynamicCast<Blubb*>(v) == nullptr);
    WD_TEST_BOOL(wdDynamicCast<wdVec3*>(v) == nullptr);
    WD_TEST_BOOL(v.IsA<void*>());
    WD_TEST_BOOL(v.Get<void*>() == nullptr);
    WD_TEST_BOOL(v.IsA<const void*>());
    WD_TEST_BOOL(v.Get<const void*>() == nullptr);
    WD_TEST_BOOL(v.IsA<wdTypedPointer>());
    WD_TEST_BOOL(v.GetReflectedType() == wdGetStaticRTTI<Blubb>());
    WD_TEST_BOOL(!v.IsA<wdVec3*>());

    wdTypedPointer ptr2 = v.Get<wdTypedPointer>();
    WD_TEST_BOOL(ptr2.m_pObject == nullptr);
    WD_TEST_BOOL(ptr2.m_pType == wdGetStaticRTTI<Blubb>());

    WD_TEST_BOOL(!v[0].IsValid());
    WD_TEST_BOOL(!v["u"].IsValid());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdTypedObject inline")
  {
    // wdAngle::Degree(90.0f) was replaced with radian as release builds generate a different float then debug.
    wdVarianceTypeAngle value = {0.1f, wdAngle::Radian(1.57079637f)};
    wdVarianceTypeAngle value2 = {0.2f, wdAngle::Radian(1.57079637f)};

    wdVariant v(value);
    TestVariant<wdVarianceTypeAngle>(v, wdVariantType::TypedObject);

    WD_TEST_BOOL(v.IsA<wdTypedObject>());
    WD_TEST_BOOL(!v.IsA<void*>());
    WD_TEST_BOOL(!v.IsA<const void*>());
    WD_TEST_BOOL(!v.IsA<wdVec3*>());
    WD_TEST_BOOL(wdDynamicCast<wdVec3*>(v) == nullptr);

    const wdVarianceTypeAngle& valueGet = v.Get<wdVarianceTypeAngle>();
    WD_TEST_BOOL(value == valueGet);

    wdVariant va = value;
    WD_TEST_BOOL(v == va);

    wdVariant v2 = value2;
    WD_TEST_BOOL(v != v2);

    wdUInt64 uiHash = v.ComputeHash(0);
    WD_TEST_INT(uiHash, 13667342936068485827ul);

    wdVarianceTypeAngle* pTypedAngle = WD_DEFAULT_NEW(wdVarianceTypeAngle, {0.1f, wdAngle::Radian(1.57079637f)});
    wdVariant copy;
    copy.CopyTypedObject(pTypedAngle, wdGetStaticRTTI<wdVarianceTypeAngle>());
    wdVariant move;
    move.MoveTypedObject(pTypedAngle, wdGetStaticRTTI<wdVarianceTypeAngle>());
    WD_TEST_BOOL(v == copy);
    WD_TEST_BOOL(v == move);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdTypedObject shared")
  {
    wdTypedObjectStruct data;
    wdVariant v = data;
    WD_TEST_BOOL(v.IsValid());
    WD_TEST_BOOL(v.GetType() == wdVariant::Type::TypedObject);
    WD_TEST_BOOL(v.IsA<wdTypedObject>());
    WD_TEST_BOOL(v.IsA<wdTypedObjectStruct>());
    WD_TEST_BOOL(!v.IsA<void*>());
    WD_TEST_BOOL(!v.IsA<const void*>());
    WD_TEST_BOOL(!v.IsA<wdVec3*>());
    WD_TEST_BOOL(wdDynamicCast<wdVec3*>(v) == nullptr);
    WD_TEST_BOOL(v.GetReflectedType() == wdGetStaticRTTI<wdTypedObjectStruct>());

    wdVariant v2 = v;

    wdTypedPointer ptr = v.GetWriteAccess();
    WD_TEST_BOOL(ptr.m_pObject == &v.Get<wdTypedObjectStruct>());
    WD_TEST_BOOL(ptr.m_pObject == &v.GetWritable<wdTypedObjectStruct>());
    WD_TEST_BOOL(ptr.m_pObject != &v2.Get<wdTypedObjectStruct>());
    WD_TEST_BOOL(ptr.m_pType == wdGetStaticRTTI<wdTypedObjectStruct>());

    WD_TEST_BOOL(wdReflectionUtils::IsEqual(ptr.m_pObject, &v2.Get<wdTypedObjectStruct>(), wdGetStaticRTTI<wdTypedObjectStruct>()));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "(Can)ConvertTo (bool)")
  {
    wdVariant v(true);

    WD_TEST_BOOL(v.CanConvertTo<bool>());
    WD_TEST_BOOL(v.CanConvertTo<wdInt32>());

    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Invalid) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Bool));
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Int8));
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::UInt8));
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Int16));
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::UInt16));
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Int32));
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::UInt32));
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Int64));
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::UInt64));
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Float));
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Double));
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Color) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Vector2) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Vector3) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Vector4) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Vector2I) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Vector3I) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Vector4I) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Quaternion) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Matrix3) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Matrix4) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::String));
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::StringView) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::DataBuffer) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Time) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Angle) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::VariantArray) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::VariantDictionary) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::TypedPointer) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::TypedObject) == false);

    WD_TEST_BOOL(v.ConvertTo<bool>() == true);
    WD_TEST_BOOL(v.ConvertTo<wdInt8>() == 1);
    WD_TEST_BOOL(v.ConvertTo<wdUInt8>() == 1);
    WD_TEST_BOOL(v.ConvertTo<wdInt16>() == 1);
    WD_TEST_BOOL(v.ConvertTo<wdUInt16>() == 1);
    WD_TEST_BOOL(v.ConvertTo<wdInt32>() == 1);
    WD_TEST_BOOL(v.ConvertTo<wdUInt32>() == 1);
    WD_TEST_BOOL(v.ConvertTo<wdInt64>() == 1);
    WD_TEST_BOOL(v.ConvertTo<wdUInt64>() == 1);
    WD_TEST_BOOL(v.ConvertTo<float>() == 1.0f);
    WD_TEST_BOOL(v.ConvertTo<double>() == 1.0);
    WD_TEST_BOOL(v.ConvertTo<wdString>() == "true");

    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Bool).Get<bool>() == true);
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Int8).Get<wdInt8>() == 1);
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::UInt8).Get<wdUInt8>() == 1);
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Int16).Get<wdInt16>() == 1);
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::UInt16).Get<wdUInt16>() == 1);
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Int32).Get<wdInt32>() == 1);
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::UInt32).Get<wdUInt32>() == 1);
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Int64).Get<wdInt64>() == 1);
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::UInt64).Get<wdUInt64>() == 1);
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Float).Get<float>() == 1.0f);
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Double).Get<double>() == 1.0);
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::String).Get<wdString>() == "true");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "(Can)ConvertTo (wdInt8)")
  {
    wdVariant v((wdInt8)3);
    TestNumberCanConvertTo(v);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "(Can)ConvertTo (wdUInt8)")
  {
    wdVariant v((wdUInt8)3);
    TestNumberCanConvertTo(v);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "(Can)ConvertTo (wdInt16)")
  {
    wdVariant v((wdInt16)3);
    TestNumberCanConvertTo(v);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "(Can)ConvertTo (wdUInt16)")
  {
    wdVariant v((wdUInt16)3);
    TestNumberCanConvertTo(v);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "(Can)ConvertTo (wdInt32)")
  {
    wdVariant v((wdInt32)3);
    TestNumberCanConvertTo(v);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "(Can)ConvertTo (wdUInt32)")
  {
    wdVariant v((wdUInt32)3);
    TestNumberCanConvertTo(v);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "(Can)ConvertTo (wdInt64)")
  {
    wdVariant v((wdInt64)3);
    TestNumberCanConvertTo(v);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "(Can)ConvertTo (wdUInt64)")
  {
    wdVariant v((wdUInt64)3);
    TestNumberCanConvertTo(v);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "(Can)ConvertTo (float)")
  {
    wdVariant v((float)3.0f);
    TestNumberCanConvertTo(v);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "(Can)ConvertTo (double)")
  {
    wdVariant v((double)3.0f);
    TestNumberCanConvertTo(v);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "(Can)ConvertTo (Color)")
  {
    wdColor c(3, 3, 4, 0);
    wdVariant v(c);

    TestCanOnlyConvertToStringAndID(v, wdVariant::Type::Color, wdVariant::Type::ColorGamma);

    wdResult conversionResult = WD_FAILURE;
    WD_TEST_BOOL(v.ConvertTo<wdColor>(&conversionResult) == c);
    WD_TEST_BOOL(conversionResult.Succeeded());

    WD_TEST_BOOL(v.ConvertTo<wdString>(&conversionResult) == "{ r=3, g=3, b=4, a=0 }");
    WD_TEST_BOOL(conversionResult.Succeeded());

    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Color).Get<wdColor>() == c);
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::String).Get<wdString>() == "{ r=3, g=3, b=4, a=0 }");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "(Can)ConvertTo (ColorGamma)")
  {
    wdColorGammaUB c(0, 128, 64, 255);
    wdVariant v(c);

    TestCanOnlyConvertToStringAndID(v, wdVariant::Type::ColorGamma, wdVariant::Type::Color);

    wdResult conversionResult = WD_FAILURE;
    WD_TEST_BOOL(v.ConvertTo<wdColorGammaUB>(&conversionResult) == c);
    WD_TEST_BOOL(conversionResult.Succeeded());

    wdString val = v.ConvertTo<wdString>(&conversionResult);
    WD_TEST_BOOL(val == "{ r=0, g=128, b=64, a=255 }");
    WD_TEST_BOOL(conversionResult.Succeeded());

    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::ColorGamma).Get<wdColorGammaUB>() == c);
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::String).Get<wdString>() == "{ r=0, g=128, b=64, a=255 }");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "(Can)ConvertTo (wdVec2)")
  {
    wdVec2 vec(3.0f, 4.0f);
    wdVariant v(vec);

    TestCanOnlyConvertToStringAndID(v, wdVariant::Type::Vector2, wdVariant::Type::Vector2I, wdVariant::Type::Vector2U);

    WD_TEST_BOOL(v.ConvertTo<wdVec2>() == vec);
    WD_TEST_BOOL(v.ConvertTo<wdVec2I32>() == wdVec2I32(3, 4));
    WD_TEST_BOOL(v.ConvertTo<wdVec2U32>() == wdVec2U32(3, 4));
    WD_TEST_BOOL(v.ConvertTo<wdString>() == "{ x=3, y=4 }");

    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Vector2).Get<wdVec2>() == vec);
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Vector2I).Get<wdVec2I32>() == wdVec2I32(3, 4));
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Vector2U).Get<wdVec2U32>() == wdVec2U32(3, 4));
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::String).Get<wdString>() == "{ x=3, y=4 }");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "(Can)ConvertTo (wdVec3)")
  {
    wdVec3 vec(3.0f, 4.0f, 6.0f);
    wdVariant v(vec);

    TestCanOnlyConvertToStringAndID(v, wdVariant::Type::Vector3, wdVariant::Type::Vector3I, wdVariant::Type::Vector3U);

    WD_TEST_BOOL(v.ConvertTo<wdVec3>() == vec);
    WD_TEST_BOOL(v.ConvertTo<wdVec3I32>() == wdVec3I32(3, 4, 6));
    WD_TEST_BOOL(v.ConvertTo<wdVec3U32>() == wdVec3U32(3, 4, 6));
    WD_TEST_BOOL(v.ConvertTo<wdString>() == "{ x=3, y=4, z=6 }");

    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Vector3).Get<wdVec3>() == vec);
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Vector3I).Get<wdVec3I32>() == wdVec3I32(3, 4, 6));
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Vector3U).Get<wdVec3U32>() == wdVec3U32(3, 4, 6));
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::String).Get<wdString>() == "{ x=3, y=4, z=6 }");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "(Can)ConvertTo (wdVec4)")
  {
    wdVec4 vec(3.0f, 4.0f, 3, 56);
    wdVariant v(vec);

    TestCanOnlyConvertToStringAndID(v, wdVariant::Type::Vector4, wdVariant::Type::Vector4I, wdVariant::Type::Vector4U);

    WD_TEST_BOOL(v.ConvertTo<wdVec4>() == vec);
    WD_TEST_BOOL(v.ConvertTo<wdVec4I32>() == wdVec4I32(3, 4, 3, 56));
    WD_TEST_BOOL(v.ConvertTo<wdVec4U32>() == wdVec4U32(3, 4, 3, 56));
    WD_TEST_BOOL(v.ConvertTo<wdString>() == "{ x=3, y=4, z=3, w=56 }");

    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Vector4).Get<wdVec4>() == vec);
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Vector4I).Get<wdVec4I32>() == wdVec4I32(3, 4, 3, 56));
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Vector4U).Get<wdVec4U32>() == wdVec4U32(3, 4, 3, 56));
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::String).Get<wdString>() == "{ x=3, y=4, z=3, w=56 }");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "(Can)ConvertTo (wdVec2I32)")
  {
    wdVec2I32 vec(3, 4);
    wdVariant v(vec);

    TestCanOnlyConvertToStringAndID(v, wdVariant::Type::Vector2I, wdVariant::Type::Vector2U, wdVariant::Type::Vector2);

    WD_TEST_BOOL(v.ConvertTo<wdVec2I32>() == vec);
    WD_TEST_BOOL(v.ConvertTo<wdVec2>() == wdVec2(3, 4));
    WD_TEST_BOOL(v.ConvertTo<wdVec2U32>() == wdVec2U32(3, 4));
    WD_TEST_BOOL(v.ConvertTo<wdString>() == "{ x=3, y=4 }");

    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Vector2I).Get<wdVec2I32>() == vec);
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Vector2).Get<wdVec2>() == wdVec2(3, 4));
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Vector2U).Get<wdVec2U32>() == wdVec2U32(3, 4));
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::String).Get<wdString>() == "{ x=3, y=4 }");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "(Can)ConvertTo (wdVec3I32)")
  {
    wdVec3I32 vec(3, 4, 6);
    wdVariant v(vec);

    TestCanOnlyConvertToStringAndID(v, wdVariant::Type::Vector3I, wdVariant::Type::Vector3U, wdVariant::Type::Vector3);

    WD_TEST_BOOL(v.ConvertTo<wdVec3I32>() == vec);
    WD_TEST_BOOL(v.ConvertTo<wdVec3>() == wdVec3(3, 4, 6));
    WD_TEST_BOOL(v.ConvertTo<wdVec3U32>() == wdVec3U32(3, 4, 6));
    WD_TEST_BOOL(v.ConvertTo<wdString>() == "{ x=3, y=4, z=6 }");

    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Vector3I).Get<wdVec3I32>() == vec);
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Vector3).Get<wdVec3>() == wdVec3(3, 4, 6));
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Vector3U).Get<wdVec3U32>() == wdVec3U32(3, 4, 6));
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::String).Get<wdString>() == "{ x=3, y=4, z=6 }");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "(Can)ConvertTo (wdVec4I32)")
  {
    wdVec4I32 vec(3, 4, 3, 56);
    wdVariant v(vec);

    TestCanOnlyConvertToStringAndID(v, wdVariant::Type::Vector4I, wdVariant::Type::Vector4U, wdVariant::Type::Vector4);

    WD_TEST_BOOL(v.ConvertTo<wdVec4I32>() == vec);
    WD_TEST_BOOL(v.ConvertTo<wdVec4>() == wdVec4(3, 4, 3, 56));
    WD_TEST_BOOL(v.ConvertTo<wdVec4U32>() == wdVec4U32(3, 4, 3, 56));
    WD_TEST_BOOL(v.ConvertTo<wdString>() == "{ x=3, y=4, z=3, w=56 }");

    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Vector4I).Get<wdVec4I32>() == vec);
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Vector4).Get<wdVec4>() == wdVec4(3, 4, 3, 56));
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Vector4U).Get<wdVec4U32>() == wdVec4U32(3, 4, 3, 56));
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::String).Get<wdString>() == "{ x=3, y=4, z=3, w=56 }");
  }
  WD_TEST_BLOCK(wdTestBlock::Enabled, "(Can)ConvertTo (wdQuat)")
  {
    wdQuat q(3.0f, 4.0f, 3, 56);
    wdVariant v(q);

    TestCanOnlyConvertToStringAndID(v, wdVariant::Type::Quaternion);

    WD_TEST_BOOL(v.ConvertTo<wdQuat>() == q);
    WD_TEST_BOOL(v.ConvertTo<wdString>() == "{ x=3, y=4, z=3, w=56 }");

    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Quaternion).Get<wdQuat>() == q);
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::String).Get<wdString>() == "{ x=3, y=4, z=3, w=56 }");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "(Can)ConvertTo (wdMat3)")
  {
    wdMat3 m(1, 2, 3, 4, 5, 6, 7, 8, 9);
    wdVariant v(m);

    TestCanOnlyConvertToStringAndID(v, wdVariant::Type::Matrix3);

    WD_TEST_BOOL(v.ConvertTo<wdMat3>() == m);
    WD_TEST_BOOL(v.ConvertTo<wdString>() == "{ c1r1=1, c2r1=2, c3r1=3, c1r2=4, c2r2=5, c3r2=6, c1r3=7, c2r3=8, c3r3=9 }");

    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Matrix3).Get<wdMat3>() == m);
    WD_TEST_BOOL(
      v.ConvertTo(wdVariant::Type::String).Get<wdString>() == "{ c1r1=1, c2r1=2, c3r1=3, c1r2=4, c2r2=5, c3r2=6, c1r3=7, c2r3=8, c3r3=9 }");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "(Can)ConvertTo (wdMat4)")
  {
    wdMat4 m(1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6);
    wdVariant v(m);

    TestCanOnlyConvertToStringAndID(v, wdVariant::Type::Matrix4);

    WD_TEST_BOOL(v.ConvertTo<wdMat4>() == m);
    WD_TEST_BOOL(v.ConvertTo<wdString>() == "{ c1r1=1, c2r1=2, c3r1=3, c4r1=4, c1r2=5, c2r2=6, c3r2=7, c4r2=8, c1r3=9, c2r3=0, c3r3=1, "
                                            "c4r3=2, c1r4=3, c2r4=4, c3r4=5, c4r4=6 }");

    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Matrix4).Get<wdMat4>() == m);
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::String).Get<wdString>() == "{ c1r1=1, c2r1=2, c3r1=3, c4r1=4, c1r2=5, c2r2=6, c3r2=7, "
                                                                         "c4r2=8, c1r3=9, c2r3=0, c3r3=1, c4r3=2, c1r4=3, c2r4=4, c3r4=5, "
                                                                         "c4r4=6 }");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "(Can)ConvertTo (wdString)")
  {
    wdVariant v("ich hab keine Lust mehr");

    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Invalid) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Bool));
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Int8));
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::UInt8));
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Int16));
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::UInt16));
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Int32));
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::UInt32));
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Int64));
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::UInt64));
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Float));
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Double));
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Color) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Vector2) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Vector3) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Vector4) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Vector2I) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Vector3I) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Vector4I) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Quaternion) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Matrix3) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Matrix4) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::String));
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::StringView) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::DataBuffer) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Time) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::Angle) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::VariantArray) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::VariantDictionary) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::TypedPointer) == false);
    WD_TEST_BOOL(v.CanConvertTo(wdVariant::Type::TypedObject) == false);

    {
      wdResult ConversionStatus = WD_SUCCESS;
      WD_TEST_BOOL(v.ConvertTo<bool>(&ConversionStatus) == false);
      WD_TEST_BOOL(ConversionStatus == WD_FAILURE);

      ConversionStatus = WD_SUCCESS;
      WD_TEST_BOOL(v.ConvertTo<wdInt8>(&ConversionStatus) == 0);
      WD_TEST_BOOL(ConversionStatus == WD_FAILURE);

      ConversionStatus = WD_SUCCESS;
      WD_TEST_BOOL(v.ConvertTo<wdUInt8>(&ConversionStatus) == 0);
      WD_TEST_BOOL(ConversionStatus == WD_FAILURE);

      ConversionStatus = WD_SUCCESS;
      WD_TEST_BOOL(v.ConvertTo<wdInt16>(&ConversionStatus) == 0);
      WD_TEST_BOOL(ConversionStatus == WD_FAILURE);

      ConversionStatus = WD_SUCCESS;
      WD_TEST_BOOL(v.ConvertTo<wdUInt16>(&ConversionStatus) == 0);
      WD_TEST_BOOL(ConversionStatus == WD_FAILURE);

      ConversionStatus = WD_SUCCESS;
      WD_TEST_BOOL(v.ConvertTo<wdInt32>(&ConversionStatus) == 0);
      WD_TEST_BOOL(ConversionStatus == WD_FAILURE);

      ConversionStatus = WD_SUCCESS;
      WD_TEST_BOOL(v.ConvertTo<wdUInt32>(&ConversionStatus) == 0);
      WD_TEST_BOOL(ConversionStatus == WD_FAILURE);

      ConversionStatus = WD_SUCCESS;
      WD_TEST_BOOL(v.ConvertTo<wdInt64>(&ConversionStatus) == 0);
      WD_TEST_BOOL(ConversionStatus == WD_FAILURE);

      ConversionStatus = WD_SUCCESS;
      WD_TEST_BOOL(v.ConvertTo<wdUInt64>(&ConversionStatus) == 0);
      WD_TEST_BOOL(ConversionStatus == WD_FAILURE);

      ConversionStatus = WD_SUCCESS;
      WD_TEST_BOOL(v.ConvertTo<float>(&ConversionStatus) == 0.0f);
      WD_TEST_BOOL(ConversionStatus == WD_FAILURE);

      ConversionStatus = WD_SUCCESS;
      WD_TEST_BOOL(v.ConvertTo<double>(&ConversionStatus) == 0.0);
      WD_TEST_BOOL(ConversionStatus == WD_FAILURE);
    }

    {
      v = "true";
      wdResult ConversionStatus = WD_FAILURE;
      WD_TEST_BOOL(v.ConvertTo<bool>(&ConversionStatus) == true);
      WD_TEST_BOOL(ConversionStatus == WD_SUCCESS);

      ConversionStatus = WD_FAILURE;
      WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Bool, &ConversionStatus).Get<bool>() == true);
      WD_TEST_BOOL(ConversionStatus == WD_SUCCESS);
    }

    {
      v = "-128";
      wdResult ConversionStatus = WD_FAILURE;
      WD_TEST_BOOL(v.ConvertTo<wdInt8>(&ConversionStatus) == -128);
      WD_TEST_BOOL(ConversionStatus == WD_SUCCESS);

      ConversionStatus = WD_FAILURE;
      WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Int8, &ConversionStatus).Get<wdInt8>() == -128);
      WD_TEST_BOOL(ConversionStatus == WD_SUCCESS);
    }

    {
      v = "255";
      wdResult ConversionStatus = WD_FAILURE;
      WD_TEST_BOOL(v.ConvertTo<wdUInt8>(&ConversionStatus) == 255);
      WD_TEST_BOOL(ConversionStatus == WD_SUCCESS);

      ConversionStatus = WD_FAILURE;
      WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::UInt8, &ConversionStatus).Get<wdUInt8>() == 255);
      WD_TEST_BOOL(ConversionStatus == WD_SUCCESS);
    }

    {
      v = "-5643";
      wdResult ConversionStatus = WD_FAILURE;
      WD_TEST_BOOL(v.ConvertTo<wdInt16>(&ConversionStatus) == -5643);
      WD_TEST_BOOL(ConversionStatus == WD_SUCCESS);

      ConversionStatus = WD_FAILURE;
      WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Int16, &ConversionStatus).Get<wdInt16>() == -5643);
      WD_TEST_BOOL(ConversionStatus == WD_SUCCESS);
    }

    {
      v = "9001";
      wdResult ConversionStatus = WD_FAILURE;
      WD_TEST_BOOL(v.ConvertTo<wdUInt16>(&ConversionStatus) == 9001);
      WD_TEST_BOOL(ConversionStatus == WD_SUCCESS);

      ConversionStatus = WD_FAILURE;
      WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::UInt16, &ConversionStatus).Get<wdUInt16>() == 9001);
      WD_TEST_BOOL(ConversionStatus == WD_SUCCESS);
    }

    {
      v = "46";
      wdResult ConversionStatus = WD_FAILURE;
      WD_TEST_BOOL(v.ConvertTo<wdInt32>(&ConversionStatus) == 46);
      WD_TEST_BOOL(ConversionStatus == WD_SUCCESS);

      ConversionStatus = WD_FAILURE;
      WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Int32, &ConversionStatus).Get<wdInt32>() == 46);
      WD_TEST_BOOL(ConversionStatus == WD_SUCCESS);
    }

    {
      v = "356";
      wdResult ConversionStatus = WD_FAILURE;
      WD_TEST_BOOL(v.ConvertTo<wdUInt32>(&ConversionStatus) == 356);
      WD_TEST_BOOL(ConversionStatus == WD_SUCCESS);

      ConversionStatus = WD_FAILURE;
      WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::UInt32, &ConversionStatus).Get<wdUInt32>() == 356);
      WD_TEST_BOOL(ConversionStatus == WD_SUCCESS);
    }

    {
      v = "64";
      wdResult ConversionStatus = WD_FAILURE;
      WD_TEST_BOOL(v.ConvertTo<wdInt64>(&ConversionStatus) == 64);
      WD_TEST_BOOL(ConversionStatus == WD_SUCCESS);

      ConversionStatus = WD_FAILURE;
      WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Int64, &ConversionStatus).Get<wdInt64>() == 64);
      WD_TEST_BOOL(ConversionStatus == WD_SUCCESS);
    }

    {
      v = "6464";
      wdResult ConversionStatus = WD_FAILURE;
      WD_TEST_BOOL(v.ConvertTo<wdUInt64>(&ConversionStatus) == 6464);
      WD_TEST_BOOL(ConversionStatus == WD_SUCCESS);

      ConversionStatus = WD_FAILURE;
      WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::UInt64, &ConversionStatus).Get<wdUInt64>() == 6464);
      WD_TEST_BOOL(ConversionStatus == WD_SUCCESS);
    }

    {
      v = "0.07564f";
      wdResult ConversionStatus = WD_FAILURE;
      WD_TEST_BOOL(v.ConvertTo<float>(&ConversionStatus) == 0.07564f);
      WD_TEST_BOOL(ConversionStatus == WD_SUCCESS);

      ConversionStatus = WD_FAILURE;
      WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Float, &ConversionStatus).Get<float>() == 0.07564f);
      WD_TEST_BOOL(ConversionStatus == WD_SUCCESS);
    }

    {
      v = "0.4453";
      wdResult ConversionStatus = WD_FAILURE;
      WD_TEST_BOOL(v.ConvertTo<double>(&ConversionStatus) == 0.4453);
      WD_TEST_BOOL(ConversionStatus == WD_SUCCESS);

      ConversionStatus = WD_FAILURE;
      WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Double, &ConversionStatus).Get<double>() == 0.4453);
      WD_TEST_BOOL(ConversionStatus == WD_SUCCESS);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "(Can)ConvertTo (wdStringView)")
  {
    wdStringView va("Test String");
    wdVariant v(va);

    TestCanOnlyConvertToStringAndID(v, wdVariant::Type::StringView);

    WD_TEST_BOOL(v.ConvertTo<wdStringView>() == va);
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::StringView).Get<wdStringView>() == va);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "(Can)ConvertTo (wdDataBuffer)")
  {
    wdDataBuffer va;
    va.PushBack(255);
    va.PushBack(4);
    wdVariant v(va);

    TestCanOnlyConvertToID(v, wdVariant::Type::DataBuffer);

    WD_TEST_BOOL(v.ConvertTo<wdDataBuffer>() == va);
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::DataBuffer).Get<wdDataBuffer>() == va);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "(Can)ConvertTo (wdTime)")
  {
    wdTime t = wdTime::Seconds(123.0);
    wdVariant v(t);

    TestCanOnlyConvertToStringAndID(v, wdVariant::Type::Time);

    WD_TEST_BOOL(v.ConvertTo<wdTime>() == t);
    // WD_TEST_BOOL(v.ConvertTo<wdString>() == "");

    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Time).Get<wdTime>() == t);
    // WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::String).Get<wdString>() == "");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "(Can)ConvertTo (wdUuid)")
  {
    wdUuid uuid;
    uuid.CreateNewUuid();
    wdVariant v(uuid);

    TestCanOnlyConvertToStringAndID(v, wdVariant::Type::Uuid);

    WD_TEST_BOOL(v.ConvertTo<wdUuid>() == uuid);
    // WD_TEST_BOOL(v.ConvertTo<wdString>() == "");

    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Uuid).Get<wdUuid>() == uuid);
    // WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::String).Get<wdString>() == "");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "(Can)ConvertTo (wdAngle)")
  {
    wdAngle t = wdAngle::Degree(123.0);
    wdVariant v(t);

    TestCanOnlyConvertToStringAndID(v, wdVariant::Type::Angle);

    WD_TEST_BOOL(v.ConvertTo<wdAngle>() == t);
    WD_TEST_BOOL(v.ConvertTo<wdString>() == "123.0°");

    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::Angle).Get<wdAngle>() == t);
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::String).Get<wdString>() == "123.0°");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "(Can)ConvertTo (VariantArray)")
  {
    wdVariantArray va;
    wdVariant v(va);

    TestCanOnlyConvertToID(v, wdVariant::Type::VariantArray, true);

    WD_TEST_BOOL(v.ConvertTo<wdVariantArray>() == va);
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::VariantArray).Get<wdVariantArray>() == va);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "(Can)ConvertTo (wdVariantDictionary)")
  {
    wdVariantDictionary va;
    wdVariant v(va);

    TestCanOnlyConvertToID(v, wdVariant::Type::VariantDictionary);

    WD_TEST_BOOL(v.ConvertTo<wdVariantDictionary>() == va);
    WD_TEST_BOOL(v.ConvertTo(wdVariant::Type::VariantDictionary).Get<wdVariantDictionary>() == va);
  }
}

#pragma optimize("", on)
