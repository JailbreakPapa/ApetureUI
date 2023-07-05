#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/IO/FileSystem/FileSystem.h>
#include <Foundation/IO/MemoryStream.h>
#include <Foundation/Reflection/Implementation/PropertyAttributes.h>
#include <Foundation/Reflection/ReflectionUtils.h>
#include <Foundation/Serialization/ReflectionSerializer.h>
#include <FoundationTest/Reflection/ReflectionTestClasses.h>

template <typename T>
static void SetComponentTest(wdVec2Template<T> vVector, T value)
{
  wdVariant var = vVector;
  wdReflectionUtils::SetComponent(var, 0, value);
  WD_TEST_BOOL(var.Get<wdVec2Template<T>>().x == value);
  wdReflectionUtils::SetComponent(var, 1, value);
  WD_TEST_BOOL(var.Get<wdVec2Template<T>>().y == value);
}

template <typename T>
static void SetComponentTest(wdVec3Template<T> vVector, T value)
{
  wdVariant var = vVector;
  wdReflectionUtils::SetComponent(var, 0, value);
  WD_TEST_BOOL(var.Get<wdVec3Template<T>>().x == value);
  wdReflectionUtils::SetComponent(var, 1, value);
  WD_TEST_BOOL(var.Get<wdVec3Template<T>>().y == value);
  wdReflectionUtils::SetComponent(var, 2, value);
  WD_TEST_BOOL(var.Get<wdVec3Template<T>>().z == value);
}

template <typename T>
static void SetComponentTest(wdVec4Template<T> vVector, T value)
{
  wdVariant var = vVector;
  wdReflectionUtils::SetComponent(var, 0, value);
  WD_TEST_BOOL(var.Get<wdVec4Template<T>>().x == value);
  wdReflectionUtils::SetComponent(var, 1, value);
  WD_TEST_BOOL(var.Get<wdVec4Template<T>>().y == value);
  wdReflectionUtils::SetComponent(var, 2, value);
  WD_TEST_BOOL(var.Get<wdVec4Template<T>>().z == value);
  wdReflectionUtils::SetComponent(var, 3, value);
  WD_TEST_BOOL(var.Get<wdVec4Template<T>>().w == value);
}

template <class T>
static void ClampValueTest(T tooSmall, T tooBig, T min, T max)
{
  wdClampValueAttribute minClamp(min, {});
  wdClampValueAttribute maxClamp({}, max);
  wdClampValueAttribute bothClamp(min, max);

  wdVariant value = tooSmall;
  WD_TEST_BOOL(wdReflectionUtils::ClampValue(value, &minClamp).Succeeded());
  WD_TEST_BOOL(value == min);

  value = tooSmall;
  WD_TEST_BOOL(wdReflectionUtils::ClampValue(value, &bothClamp).Succeeded());
  WD_TEST_BOOL(value == min);

  value = tooBig;
  WD_TEST_BOOL(wdReflectionUtils::ClampValue(value, &maxClamp).Succeeded());
  WD_TEST_BOOL(value == max);

  value = tooBig;
  WD_TEST_BOOL(wdReflectionUtils::ClampValue(value, &bothClamp).Succeeded());
  WD_TEST_BOOL(value == max);
}


WD_CREATE_SIMPLE_TEST(Reflection, Utils)
{
  wdDefaultMemoryStreamStorage StreamStorage;

  WD_TEST_BLOCK(wdTestBlock::Enabled, "WriteObjectToDDL")
  {
    wdMemoryStreamWriter FileOut(&StreamStorage);

    wdTestClass2 c2;
    c2.SetText("Hallo");
    c2.m_MyVector.Set(14, 16, 18);
    c2.m_Struct.m_fFloat1 = 128;
    c2.m_Struct.m_UInt8 = 234;
    c2.m_Struct.m_Angle = wdAngle::Degree(360);
    c2.m_Struct.m_vVec3I = wdVec3I32(9, 8, 7);
    c2.m_Struct.m_DataBuffer.Clear();
    c2.m_Color = wdColor(0.1f, 0.2f, 0.3f);
    c2.m_Time = wdTime::Seconds(91.0f);
    c2.m_enumClass = wdExampleEnum::Value3;
    c2.m_bitflagsClass = wdExampleBitflags::Value1 | wdExampleBitflags::Value2 | wdExampleBitflags::Value3;
    c2.m_array.PushBack(5.0f);
    c2.m_array.PushBack(10.0f);
    c2.m_Variant = wdVec3(1.0f, 2.0f, 3.0f);

    wdReflectionSerializer::WriteObjectToDDL(FileOut, c2.GetDynamicRTTI(), &c2, false, wdOpenDdlWriter::TypeStringMode::Compliant);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ReadObjectPropertiesFromDDL")
  {
    wdMemoryStreamReader FileIn(&StreamStorage);

    wdTestClass2 c2;

    wdReflectionSerializer::ReadObjectPropertiesFromDDL(FileIn, *c2.GetDynamicRTTI(), &c2);

    WD_TEST_STRING(c2.GetText(), "Hallo");
    WD_TEST_VEC3(c2.m_MyVector, wdVec3(3, 4, 5), 0.0f);
    WD_TEST_FLOAT(c2.m_Time.GetSeconds(), 91.0f, 0.0f);
    WD_TEST_FLOAT(c2.m_Color.r, 0.1f, 0.0f);
    WD_TEST_FLOAT(c2.m_Color.g, 0.2f, 0.0f);
    WD_TEST_FLOAT(c2.m_Color.b, 0.3f, 0.0f);
    WD_TEST_FLOAT(c2.m_Struct.m_fFloat1, 128, 0.0f);
    WD_TEST_INT(c2.m_Struct.m_UInt8, 234);
    WD_TEST_BOOL(c2.m_Struct.m_Angle == wdAngle::Degree(360));
    WD_TEST_BOOL(c2.m_Struct.m_vVec3I == wdVec3I32(9, 8, 7));
    WD_TEST_BOOL(c2.m_Struct.m_DataBuffer == wdDataBuffer());
    WD_TEST_BOOL(c2.m_enumClass == wdExampleEnum::Value3);
    WD_TEST_BOOL(c2.m_bitflagsClass == (wdExampleBitflags::Value1 | wdExampleBitflags::Value2 | wdExampleBitflags::Value3));
    WD_TEST_INT(c2.m_array.GetCount(), 2);
    if (c2.m_array.GetCount() == 2)
    {
      WD_TEST_FLOAT(c2.m_array[0], 5.0f, 0.0f);
      WD_TEST_FLOAT(c2.m_array[1], 10.0f, 0.0f);
    }
    WD_TEST_VEC3(c2.m_Variant.Get<wdVec3>(), wdVec3(1.0f, 2.0f, 3.0f), 0.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ReadObjectPropertiesFromDDL (different type)")
  {
    // here we restore the same properties into a different type of object which has properties that are named the same
    // but may have slightly different types (but which are compatible)

    wdMemoryStreamReader FileIn(&StreamStorage);

    wdTestClass2b c2;

    wdReflectionSerializer::ReadObjectPropertiesFromDDL(FileIn, *c2.GetDynamicRTTI(), &c2);

    WD_TEST_STRING(c2.GetText(), "Tut"); // not restored, different property name
    WD_TEST_FLOAT(c2.m_Color.r, 0.1f, 0.0f);
    WD_TEST_FLOAT(c2.m_Color.g, 0.2f, 0.0f);
    WD_TEST_FLOAT(c2.m_Color.b, 0.3f, 0.0f);
    WD_TEST_FLOAT(c2.m_Struct.m_fFloat1, 128, 0.0f);
    WD_TEST_INT(c2.m_Struct.m_UInt8, 234);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ReadObjectFromDDL")
  {
    wdMemoryStreamReader FileIn(&StreamStorage);

    const wdRTTI* pRtti;
    void* pObject = wdReflectionSerializer::ReadObjectFromDDL(FileIn, pRtti);

    wdTestClass2& c2 = *((wdTestClass2*)pObject);

    WD_TEST_STRING(c2.GetText(), "Hallo");
    WD_TEST_VEC3(c2.m_MyVector, wdVec3(3, 4, 5), 0.0f);
    WD_TEST_FLOAT(c2.m_Time.GetSeconds(), 91.0f, 0.0f);
    WD_TEST_FLOAT(c2.m_Color.r, 0.1f, 0.0f);
    WD_TEST_FLOAT(c2.m_Color.g, 0.2f, 0.0f);
    WD_TEST_FLOAT(c2.m_Color.b, 0.3f, 0.0f);
    WD_TEST_FLOAT(c2.m_Struct.m_fFloat1, 128, 0.0f);
    WD_TEST_INT(c2.m_Struct.m_UInt8, 234);
    WD_TEST_BOOL(c2.m_Struct.m_Angle == wdAngle::Degree(360));
    WD_TEST_BOOL(c2.m_Struct.m_vVec3I == wdVec3I32(9, 8, 7));
    WD_TEST_BOOL(c2.m_Struct.m_DataBuffer == wdDataBuffer());
    WD_TEST_BOOL(c2.m_enumClass == wdExampleEnum::Value3);
    WD_TEST_BOOL(c2.m_bitflagsClass == (wdExampleBitflags::Value1 | wdExampleBitflags::Value2 | wdExampleBitflags::Value3));
    WD_TEST_INT(c2.m_array.GetCount(), 2);
    if (c2.m_array.GetCount() == 2)
    {
      WD_TEST_FLOAT(c2.m_array[0], 5.0f, 0.0f);
      WD_TEST_FLOAT(c2.m_array[1], 10.0f, 0.0f);
    }
    WD_TEST_VEC3(c2.m_Variant.Get<wdVec3>(), wdVec3(1.0f, 2.0f, 3.0f), 0.0f);

    if (pObject)
    {
      pRtti->GetAllocator()->Deallocate(pObject);
    }
  }

  wdFileSystem::ClearAllDataDirectories();

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetComponent")
  {
    SetComponentTest(wdVec2(0.0f, 0.1f), -0.5f);
    SetComponentTest(wdVec3(0.0f, 0.1f, 0.2f), -0.5f);
    SetComponentTest(wdVec4(0.0f, 0.1f, 0.2f, 0.3f), -0.5f);
    SetComponentTest(wdVec2I32(0, 1), -4);
    SetComponentTest(wdVec3I32(0, 1, 2), -4);
    SetComponentTest(wdVec4I32(0, 1, 2, 3), -4);
    SetComponentTest(wdVec2U32(0, 1), 4u);
    SetComponentTest(wdVec3U32(0, 1, 2), 4u);
    SetComponentTest(wdVec4U32(0, 1, 2, 3), 4u);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ClampValue")
  {
    ClampValueTest<float>(-1, 1000, 2, 4);
    ClampValueTest<double>(-1, 1000, 2, 4);
    ClampValueTest<wdInt32>(-1, 1000, 2, 4);
    ClampValueTest<wdUInt64>(1, 1000, 2, 4);
    ClampValueTest<wdTime>(wdTime::Milliseconds(1), wdTime::Milliseconds(1000), wdTime::Milliseconds(2), wdTime::Milliseconds(4));
    ClampValueTest<wdAngle>(wdAngle::Degree(1), wdAngle::Degree(1000), wdAngle::Degree(2), wdAngle::Degree(4));
    ClampValueTest<wdVec3>(wdVec3(1), wdVec3(1000), wdVec3(2), wdVec3(4));
    ClampValueTest<wdVec4I32>(wdVec4I32(1), wdVec4I32(1000), wdVec4I32(2), wdVec4I32(4));
    ClampValueTest<wdVec4U32>(wdVec4U32(1), wdVec4U32(1000), wdVec4U32(2), wdVec4U32(4));

    wdVarianceTypeFloat vf = {1.0f, 2.0f};
    wdVariant variance = vf;
    WD_TEST_BOOL(wdReflectionUtils::ClampValue(variance, nullptr).Succeeded());

    wdVarianceTypeFloat clamp = {2.0f, 3.0f};
    wdClampValueAttribute minClamp(clamp, {});
    WD_TEST_BOOL(wdReflectionUtils::ClampValue(variance, &minClamp).Failed());
  }
}
