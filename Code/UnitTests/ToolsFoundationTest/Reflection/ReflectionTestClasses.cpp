#include <ToolsFoundationTest/ToolsFoundationTestPCH.h>

#include <Foundation/Reflection/Reflection.h>
#include <ToolsFoundationTest/Reflection/ReflectionTestClasses.h>

// clang-format off
WD_BEGIN_STATIC_REFLECTED_TYPE(wdIntegerStruct, wdNoBase, 1, wdRTTIDefaultAllocator<wdIntegerStruct>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_ACCESSOR_PROPERTY("Int8", GetInt8, SetInt8),
    WD_ACCESSOR_PROPERTY("UInt8", GetUInt8, SetUInt8),
    WD_MEMBER_PROPERTY("Int16", m_iInt16),
    WD_MEMBER_PROPERTY("UInt16", m_iUInt16),
    WD_ACCESSOR_PROPERTY("Int32", GetInt32, SetInt32),
    WD_ACCESSOR_PROPERTY("UInt32", GetUInt32, SetUInt32),
    WD_MEMBER_PROPERTY("Int64", m_iInt64),
    WD_MEMBER_PROPERTY("UInt64", m_iUInt64),
  }
  WD_END_PROPERTIES;
}
WD_END_STATIC_REFLECTED_TYPE;


WD_BEGIN_STATIC_REFLECTED_TYPE(wdFloatStruct, wdNoBase, 1, wdRTTIDefaultAllocator<wdFloatStruct>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_ACCESSOR_PROPERTY("Float", GetFloat, SetFloat),
    WD_ACCESSOR_PROPERTY("Double", GetDouble, SetDouble),
    WD_ACCESSOR_PROPERTY("Time", GetTime, SetTime),
    WD_ACCESSOR_PROPERTY("Angle", GetAngle, SetAngle),
  }
  WD_END_PROPERTIES;
}
WD_END_STATIC_REFLECTED_TYPE;


WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdPODClass, 1, wdRTTIDefaultAllocator<wdPODClass>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_MEMBER_PROPERTY("Integer", m_IntegerStruct),
    WD_MEMBER_PROPERTY("Float", m_FloatStruct),
    WD_ACCESSOR_PROPERTY("Bool", GetBool, SetBool),
    WD_ACCESSOR_PROPERTY("Color", GetColor, SetColor),
    WD_MEMBER_PROPERTY("ColorUB", m_Color2),
    WD_ACCESSOR_PROPERTY("String", GetString, SetString),
    WD_ACCESSOR_PROPERTY("Buffer", GetBuffer, SetBuffer),
    WD_ACCESSOR_PROPERTY("VarianceAngle", GetCustom, SetCustom),
  }
  WD_END_PROPERTIES;
}
WD_END_DYNAMIC_REFLECTED_TYPE;


WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdMathClass, 1, wdRTTIDefaultAllocator<wdMathClass>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_ACCESSOR_PROPERTY("Vec2", GetVec2, SetVec2),
    WD_ACCESSOR_PROPERTY("Vec3", GetVec3, SetVec3),
    WD_ACCESSOR_PROPERTY("Vec4", GetVec4, SetVec4),
    WD_MEMBER_PROPERTY("Vec2I", m_Vec2I),
    WD_MEMBER_PROPERTY("Vec3I", m_Vec3I),
    WD_MEMBER_PROPERTY("Vec4I", m_Vec4I),
    WD_ACCESSOR_PROPERTY("Quat", GetQuat, SetQuat),
    WD_ACCESSOR_PROPERTY("Mat3", GetMat3, SetMat3),
    WD_ACCESSOR_PROPERTY("Mat4", GetMat4, SetMat4),
  }
  WD_END_PROPERTIES;
}
WD_END_DYNAMIC_REFLECTED_TYPE;


WD_BEGIN_STATIC_REFLECTED_ENUM(wdExampleEnum, 1)
  WD_ENUM_CONSTANTS(wdExampleEnum::Value1, wdExampleEnum::Value2)
  WD_ENUM_CONSTANT(wdExampleEnum::Value3),
WD_END_STATIC_REFLECTED_ENUM;


WD_BEGIN_STATIC_REFLECTED_BITFLAGS(wdExampleBitflags, 1)
  WD_BITFLAGS_CONSTANTS(wdExampleBitflags::Value1, wdExampleBitflags::Value2)
  WD_BITFLAGS_CONSTANT(wdExampleBitflags::Value3),
WD_END_STATIC_REFLECTED_BITFLAGS;


WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdEnumerationsClass, 1, wdRTTIDefaultAllocator<wdEnumerationsClass>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_ENUM_ACCESSOR_PROPERTY("Enum", wdExampleEnum, GetEnum, SetEnum),
    WD_BITFLAGS_ACCESSOR_PROPERTY("Bitflags", wdExampleBitflags, GetBitflags, SetBitflags),
  }
  WD_END_PROPERTIES;
}
WD_END_DYNAMIC_REFLECTED_TYPE;


WD_BEGIN_STATIC_REFLECTED_TYPE(InnerStruct, wdNoBase, 1, wdRTTIDefaultAllocator<InnerStruct>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_MEMBER_PROPERTY("IP1", m_fP1),
  }
  WD_END_PROPERTIES;
}
WD_END_STATIC_REFLECTED_TYPE;


WD_BEGIN_DYNAMIC_REFLECTED_TYPE(OuterClass, 1, wdRTTIDefaultAllocator<OuterClass>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_MEMBER_PROPERTY("Inner", m_Inner1),
    WD_MEMBER_PROPERTY("OP1", m_fP1),
  }
  WD_END_PROPERTIES;
}
WD_END_DYNAMIC_REFLECTED_TYPE;


WD_BEGIN_DYNAMIC_REFLECTED_TYPE(ExtendedOuterClass, 1, wdRTTIDefaultAllocator<ExtendedOuterClass>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_MEMBER_PROPERTY("MORE", m_more),
  }
  WD_END_PROPERTIES;
}
WD_END_DYNAMIC_REFLECTED_TYPE;


WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdObjectTest, 1, wdRTTIDefaultAllocator<wdObjectTest>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_MEMBER_PROPERTY("MemberClass", m_MemberClass),
    WD_ARRAY_MEMBER_PROPERTY("StandardTypeArray", m_StandardTypeArray),
    WD_ARRAY_MEMBER_PROPERTY("ClassArray", m_ClassArray),
    WD_ARRAY_MEMBER_PROPERTY("ClassPtrArray", m_ClassPtrArray)->AddFlags(wdPropertyFlags::PointerOwner),
    WD_SET_ACCESSOR_PROPERTY("StandardTypeSet", GetStandardTypeSet, StandardTypeSetInsert, StandardTypeSetRemove),
    WD_SET_MEMBER_PROPERTY("SubObjectSet", m_SubObjectSet)->AddFlags(wdPropertyFlags::PointerOwner),
    WD_MAP_MEMBER_PROPERTY("StandardTypeMap", m_StandardTypeMap),
    WD_MAP_MEMBER_PROPERTY("ClassMap", m_ClassMap),
    WD_MAP_MEMBER_PROPERTY("ClassPtrMap", m_ClassPtrMap)->AddFlags(wdPropertyFlags::PointerOwner),
  }
  WD_END_PROPERTIES;
}
WD_END_DYNAMIC_REFLECTED_TYPE;

WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdMirrorTest, 1, wdRTTIDefaultAllocator<wdMirrorTest>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_MEMBER_PROPERTY("Math", m_math),
    WD_MEMBER_PROPERTY("Object", m_object),
  }
  WD_END_PROPERTIES;
}
WD_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

wdArrayPtr<const wdString> wdObjectTest::GetStandardTypeSet() const
{
  return m_StandardTypeSet;
}

void wdObjectTest::StandardTypeSetInsert(const wdString& value)
{
  if (!m_StandardTypeSet.Contains(value))
    m_StandardTypeSet.PushBack(value);
}

void wdObjectTest::StandardTypeSetRemove(const wdString& value)
{
  m_StandardTypeSet.RemoveAndCopy(value);
}
