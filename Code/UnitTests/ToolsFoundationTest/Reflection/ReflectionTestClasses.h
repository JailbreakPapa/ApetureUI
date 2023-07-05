#pragma once

#include <Foundation/Math/Color8UNorm.h>
#include <Foundation/Reflection/Reflection.h>
#include <Foundation/Types/VarianceTypes.h>

struct wdIntegerStruct
{
public:
  wdIntegerStruct()
  {
    m_iInt8 = 1;
    m_uiUInt8 = 1;
    m_iInt16 = 1;
    m_iUInt16 = 1;
    m_iInt32 = 1;
    m_uiUInt32 = 1;
    m_iInt64 = 1;
    m_iUInt64 = 1;
  }

  void SetInt8(wdInt8 i) { m_iInt8 = i; }
  wdInt8 GetInt8() const { return m_iInt8; }
  void SetUInt8(wdUInt8 i) { m_uiUInt8 = i; }
  wdUInt8 GetUInt8() const { return m_uiUInt8; }
  void SetInt32(wdInt32 i) { m_iInt32 = i; }
  wdInt32 GetInt32() const { return m_iInt32; }
  void SetUInt32(wdUInt32 i) { m_uiUInt32 = i; }
  wdUInt32 GetUInt32() const { return m_uiUInt32; }

  wdInt16 m_iInt16;
  wdUInt16 m_iUInt16;
  wdInt64 m_iInt64;
  wdUInt64 m_iUInt64;

private:
  wdInt8 m_iInt8;
  wdUInt8 m_uiUInt8;
  wdInt32 m_iInt32;
  wdUInt32 m_uiUInt32;
};
WD_DECLARE_REFLECTABLE_TYPE(WD_NO_LINKAGE, wdIntegerStruct);


struct wdFloatStruct
{
public:
  wdFloatStruct()
  {
    m_fFloat = 1.0f;
    m_fDouble = 1.0;
    m_Time = wdTime::Seconds(1.0);
    m_Angle = wdAngle::Degree(45.0f);
  }

  void SetFloat(float f) { m_fFloat = f; }
  float GetFloat() const { return m_fFloat; }
  void SetDouble(double d) { m_fDouble = d; }
  double GetDouble() const { return m_fDouble; }
  void SetTime(wdTime t) { m_Time = t; }
  wdTime GetTime() const { return m_Time; }
  wdAngle GetAngle() const { return m_Angle; }
  void SetAngle(wdAngle t) { m_Angle = t; }

private:
  float m_fFloat;
  double m_fDouble;
  wdTime m_Time;
  wdAngle m_Angle;
};
WD_DECLARE_REFLECTABLE_TYPE(WD_NO_LINKAGE, wdFloatStruct);


class wdPODClass : public wdReflectedClass
{
  WD_ADD_DYNAMIC_REFLECTION(wdPODClass, wdReflectedClass);

public:
  wdPODClass()
  {
    m_bBool = true;
    m_Color = wdColor(1.0f, 0.0f, 0.0f, 0.0f);
    m_Color2 = wdColorGammaUB(255, 10, 1);
    m_sString = "Test";
    m_Buffer.PushBack(0xFF);
    m_Buffer.PushBack(0x0);
    m_Buffer.PushBack(0xCD);
    m_VarianceAngle = {0.1f, wdAngle::Degree(90.0f)};
  }

  wdIntegerStruct m_IntegerStruct;
  wdFloatStruct m_FloatStruct;

  void SetBool(bool b) { m_bBool = b; }
  bool GetBool() const { return m_bBool; }
  void SetColor(wdColor c) { m_Color = c; }
  wdColor GetColor() const { return m_Color; }
  const char* GetString() const { return m_sString.GetData(); }
  void SetString(const char* szSz) { m_sString = szSz; }

  const wdDataBuffer& GetBuffer() const { return m_Buffer; }
  void SetBuffer(const wdDataBuffer& data) { m_Buffer = data; }

  wdVarianceTypeAngle GetCustom() const { return m_VarianceAngle; }
  void SetCustom(wdVarianceTypeAngle value) { m_VarianceAngle = value; }

private:
  bool m_bBool;
  wdColor m_Color;
  wdColorGammaUB m_Color2;
  wdString m_sString;
  wdDataBuffer m_Buffer;
  wdVarianceTypeAngle m_VarianceAngle;
};


class wdMathClass : public wdPODClass
{
  WD_ADD_DYNAMIC_REFLECTION(wdMathClass, wdPODClass);

public:
  wdMathClass()
  {
    m_vVec2 = wdVec2(1.0f, 1.0f);
    m_vVec3 = wdVec3(1.0f, 1.0f, 1.0f);
    m_vVec4 = wdVec4(1.0f, 1.0f, 1.0f, 1.0f);
    m_Vec2I = wdVec2I32(1, 1);
    m_Vec3I = wdVec3I32(1, 1, 1);
    m_Vec4I = wdVec4I32(1, 1, 1, 1);
    m_qQuat = wdQuat(1.0f, 1.0f, 1.0f, 1.0f);
    m_mMat3.SetZero();
    m_mMat4.SetZero();
  }

  void SetVec2(wdVec2 v) { m_vVec2 = v; }
  wdVec2 GetVec2() const { return m_vVec2; }
  void SetVec3(wdVec3 v) { m_vVec3 = v; }
  wdVec3 GetVec3() const { return m_vVec3; }
  void SetVec4(wdVec4 v) { m_vVec4 = v; }
  wdVec4 GetVec4() const { return m_vVec4; }
  void SetQuat(wdQuat q) { m_qQuat = q; }
  wdQuat GetQuat() const { return m_qQuat; }
  void SetMat3(wdMat3 m) { m_mMat3 = m; }
  wdMat3 GetMat3() const { return m_mMat3; }
  void SetMat4(wdMat4 m) { m_mMat4 = m; }
  wdMat4 GetMat4() const { return m_mMat4; }

  wdVec2I32 m_Vec2I;
  wdVec3I32 m_Vec3I;
  wdVec4I32 m_Vec4I;

private:
  wdVec2 m_vVec2;
  wdVec3 m_vVec3;
  wdVec4 m_vVec4;
  wdQuat m_qQuat;
  wdMat3 m_mMat3;
  wdMat4 m_mMat4;
};


struct wdExampleEnum
{
  typedef wdInt8 StorageType;
  enum Enum
  {
    Value1 = 0,      // normal value
    Value2 = -2,     // normal value
    Value3 = 4,      // normal value
    Default = Value1 // Default initialization value (required)
  };
};
WD_DECLARE_REFLECTABLE_TYPE(WD_NO_LINKAGE, wdExampleEnum);


struct wdExampleBitflags
{
  typedef wdUInt64 StorageType;
  enum Enum : wdUInt64
  {
    Value1 = WD_BIT(0),  // normal value
    Value2 = WD_BIT(31), // normal value
    Value3 = WD_BIT(63), // normal value
    Default = Value1     // Default initialization value (required)
  };

  struct Bits
  {
    StorageType Value1 : 1;
    StorageType Padding : 30;
    StorageType Value2 : 1;
    StorageType Padding2 : 31;
    StorageType Value3 : 1;
  };
};
WD_DECLARE_REFLECTABLE_TYPE(WD_NO_LINKAGE, wdExampleBitflags);


class wdEnumerationsClass : public wdReflectedClass
{
  WD_ADD_DYNAMIC_REFLECTION(wdEnumerationsClass, wdReflectedClass);

public:
  wdEnumerationsClass()
  {
    m_EnumClass = wdExampleEnum::Value2;
    m_BitflagsClass = wdExampleBitflags::Value2;
  }

  void SetEnum(wdExampleEnum::Enum e) { m_EnumClass = e; }
  wdExampleEnum::Enum GetEnum() const { return m_EnumClass; }
  void SetBitflags(wdBitflags<wdExampleBitflags> e) { m_BitflagsClass = e; }
  wdBitflags<wdExampleBitflags> GetBitflags() const { return m_BitflagsClass; }

private:
  wdEnum<wdExampleEnum> m_EnumClass;
  wdBitflags<wdExampleBitflags> m_BitflagsClass;
};


struct InnerStruct
{
  WD_DECLARE_POD_TYPE();

public:
  float m_fP1;
};
WD_DECLARE_REFLECTABLE_TYPE(WD_NO_LINKAGE, InnerStruct);


class OuterClass : public wdReflectedClass
{
  WD_ADD_DYNAMIC_REFLECTION(OuterClass, wdReflectedClass);

public:
  InnerStruct m_Inner1;
  float m_fP1;
};

class ExtendedOuterClass : public OuterClass
{
  WD_ADD_DYNAMIC_REFLECTION(ExtendedOuterClass, OuterClass);

public:
  wdString m_more;
};

class wdObjectTest : public wdReflectedClass
{
  WD_ADD_DYNAMIC_REFLECTION(wdObjectTest, wdReflectedClass);

public:
  wdObjectTest() {}
  ~wdObjectTest()
  {
    for (OuterClass* pTest : m_ClassPtrArray)
    {
      wdGetStaticRTTI<OuterClass>()->GetAllocator()->Deallocate(pTest);
    }
    for (wdObjectTest* pTest : m_SubObjectSet)
    {
      wdGetStaticRTTI<wdObjectTest>()->GetAllocator()->Deallocate(pTest);
    }
    for (auto it = m_ClassPtrMap.GetIterator(); it.IsValid(); ++it)
    {
      wdGetStaticRTTI<OuterClass>()->GetAllocator()->Deallocate(it.Value());
    }
  }

  wdArrayPtr<const wdString> GetStandardTypeSet() const;
  void StandardTypeSetInsert(const wdString& value);
  void StandardTypeSetRemove(const wdString& value);

  OuterClass m_MemberClass;

  wdDynamicArray<double> m_StandardTypeArray;
  wdDynamicArray<OuterClass> m_ClassArray;
  wdDeque<OuterClass*> m_ClassPtrArray;

  wdDynamicArray<wdString> m_StandardTypeSet;
  wdSet<wdObjectTest*> m_SubObjectSet;

  wdMap<wdString, double> m_StandardTypeMap;
  wdHashTable<wdString, OuterClass> m_ClassMap;
  wdMap<wdString, OuterClass*> m_ClassPtrMap;
};


class wdMirrorTest : public wdReflectedClass
{
  WD_ADD_DYNAMIC_REFLECTION(wdMirrorTest, wdReflectedClass);

public:
  wdMirrorTest() {}

  wdMathClass m_math;
  wdObjectTest m_object;
};
