#pragma once

#include <Foundation/Reflection/Reflection.h>
#include <Foundation/Types/RangeView.h>
#include <Foundation/Types/VarianceTypes.h>

struct wdExampleEnum
{
  typedef wdInt8 StorageType;
  enum Enum
  {
    Value1 = 1,      // normal value
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

WD_DECLARE_FLAGS_OPERATORS(wdExampleBitflags);

WD_DECLARE_REFLECTABLE_TYPE(WD_NO_LINKAGE, wdExampleBitflags);


class wdAbstractTestClass : public wdReflectedClass
{
  WD_ADD_DYNAMIC_REFLECTION(wdAbstractTestClass, wdReflectedClass);

  virtual void AbstractFunction() = 0;
};


struct wdAbstractTestStruct
{
  virtual void AbstractFunction() = 0;
};

WD_DECLARE_REFLECTABLE_TYPE(WD_NO_LINKAGE, wdAbstractTestStruct);


struct wdTestStruct
{
  WD_ALLOW_PRIVATE_PROPERTIES(wdTestStruct);

public:
  static wdDataBuffer GetDefaultDataBuffer()
  {
    wdDataBuffer data;
    data.PushBack(255);
    data.PushBack(0);
    data.PushBack(127);
    return data;
  }

  wdTestStruct()
  {
    m_fFloat1 = 1.1f;
    m_iInt2 = 2;
    m_vProperty3.Set(3, 4, 5);
    m_UInt8 = 6;
    m_variant = "Test";
    m_Angle = wdAngle::Degree(0.5);
    m_DataBuffer = GetDefaultDataBuffer();
    m_vVec3I = wdVec3I32(1, 2, 3);
    m_VarianceAngle.m_fVariance = 0.5f;
    m_VarianceAngle.m_Value = wdAngle::Degree(90.0f);
  }



  bool operator==(const wdTestStruct& rhs) const
  {
    return m_fFloat1 == rhs.m_fFloat1 && m_UInt8 == rhs.m_UInt8 && m_variant == rhs.m_variant && m_iInt2 == rhs.m_iInt2 && m_vProperty3 == rhs.m_vProperty3 && m_Angle == rhs.m_Angle && m_DataBuffer == rhs.m_DataBuffer && m_vVec3I == rhs.m_vVec3I && m_VarianceAngle == rhs.m_VarianceAngle;
  }

  float m_fFloat1;
  wdUInt8 m_UInt8;
  wdVariant m_variant;
  wdAngle m_Angle;
  wdDataBuffer m_DataBuffer;
  wdVec3I32 m_vVec3I;
  wdVarianceTypeAngle m_VarianceAngle;

private:
  void SetInt(wdInt32 i) { m_iInt2 = i; }
  wdInt32 GetInt() const { return m_iInt2; }

  wdInt32 m_iInt2;
  wdVec3 m_vProperty3;
};

WD_DECLARE_REFLECTABLE_TYPE(WD_NO_LINKAGE, wdTestStruct);


struct wdTestStruct3
{
  WD_ALLOW_PRIVATE_PROPERTIES(wdTestStruct3);

public:
  wdTestStruct3()
  {
    m_fFloat1 = 1.1f;
    m_UInt8 = 6;
    m_iInt32 = 2;
  }
  wdTestStruct3(double a, wdInt16 b)
  {
    m_fFloat1 = a;
    m_UInt8 = b;
    m_iInt32 = 32;
  }

  bool operator==(const wdTestStruct3& rhs) const { return m_fFloat1 == rhs.m_fFloat1 && m_iInt32 == rhs.m_iInt32 && m_UInt8 == rhs.m_UInt8; }

  bool operator!=(const wdTestStruct3& rhs) const { return !(*this == rhs); }

  double m_fFloat1;
  wdInt16 m_UInt8;

  wdUInt32 GetIntPublic() const { return m_iInt32; }

private:
  void SetInt(wdUInt32 i) { m_iInt32 = i; }
  wdUInt32 GetInt() const { return m_iInt32; }

  wdInt32 m_iInt32;
};

WD_DECLARE_REFLECTABLE_TYPE(WD_NO_LINKAGE, wdTestStruct3);

struct wdTypedObjectStruct
{
  WD_ALLOW_PRIVATE_PROPERTIES(wdTypedObjectStruct);

public:
  wdTypedObjectStruct()
  {
    m_fFloat1 = 1.1f;
    m_UInt8 = 6;
    m_iInt32 = 2;
  }
  wdTypedObjectStruct(double a, wdInt16 b)
  {
    m_fFloat1 = a;
    m_UInt8 = b;
    m_iInt32 = 32;
  }

  double m_fFloat1;
  wdInt16 m_UInt8;
  wdInt32 m_iInt32;
};
WD_DECLARE_REFLECTABLE_TYPE(WD_NO_LINKAGE, wdTypedObjectStruct);
WD_DECLARE_CUSTOM_VARIANT_TYPE(wdTypedObjectStruct);

class wdTestClass1 : public wdReflectedClass
{
  WD_ADD_DYNAMIC_REFLECTION(wdTestClass1, wdReflectedClass);

public:
  wdTestClass1()
  {
    m_MyVector.Set(3, 4, 5);

    m_Struct.m_fFloat1 = 33.3f;

    m_Color = wdColor::CornflowerBlue; // The Original!
  }

  wdTestClass1(const wdColor& c, const wdTestStruct& s)
  {
    m_Color = c;
    m_Struct = s;
    m_MyVector.Set(1, 2, 3);
  }

  bool operator==(const wdTestClass1& rhs) const { return m_Struct == rhs.m_Struct && m_MyVector == rhs.m_MyVector && m_Color == rhs.m_Color; }

  wdVec3 GetVector() const { return m_MyVector; }

  wdTestStruct m_Struct;
  wdVec3 m_MyVector;
  wdColor m_Color;
};


class wdTestClass2 : public wdTestClass1
{
  WD_ADD_DYNAMIC_REFLECTION(wdTestClass2, wdTestClass1);

public:
  wdTestClass2() { m_sText = "Legen"; }

  bool operator==(const wdTestClass2& rhs) const { return m_Time == rhs.m_Time && m_enumClass == rhs.m_enumClass && m_bitflagsClass == rhs.m_bitflagsClass && m_array == rhs.m_array && m_Variant == rhs.m_Variant && m_sText == rhs.m_sText; }

  const char* GetText() const { return m_sText.GetData(); }
  void SetText(const char* szSz) { m_sText = szSz; }

  wdTime m_Time;
  wdEnum<wdExampleEnum> m_enumClass;
  wdBitflags<wdExampleBitflags> m_bitflagsClass;
  wdHybridArray<float, 4> m_array;
  wdVariant m_Variant;

private:
  wdString m_sText;
};


struct wdTestClass2Allocator : public wdRTTIAllocator
{
  virtual wdInternal::NewInstance<void> AllocateInternal(wdAllocatorBase* pAllocator) override
  {
    ++m_iAllocs;

    return WD_DEFAULT_NEW(wdTestClass2);
  }

  virtual void Deallocate(void* pObject, wdAllocatorBase* pAllocator) override
  {
    ++m_iDeallocs;

    wdTestClass2* pPointer = (wdTestClass2*)pObject;
    WD_DEFAULT_DELETE(pPointer);
  }

  static wdInt32 m_iAllocs;
  static wdInt32 m_iDeallocs;
};


class wdTestClass2b : wdReflectedClass
{
  WD_ADD_DYNAMIC_REFLECTION(wdTestClass2b, wdReflectedClass);

public:
  wdTestClass2b() { m_sText = "Tut"; }

  const char* GetText() const { return m_sText.GetData(); }
  void SetText(const char* szSz) { m_sText = szSz; }

  wdTestStruct3 m_Struct;
  wdColor m_Color;

private:
  wdString m_sText;
};


class wdTestArrays : public wdReflectedClass
{
  WD_ADD_DYNAMIC_REFLECTION(wdTestArrays, wdReflectedClass);

public:
  wdTestArrays() {}

  bool operator==(const wdTestArrays& rhs) const
  {
    return m_Hybrid == rhs.m_Hybrid && m_Dynamic == rhs.m_Dynamic && m_Deque == rhs.m_Deque && m_HybridChar == rhs.m_HybridChar && m_CustomVariant == rhs.m_CustomVariant;
  }

  bool operator!=(const wdTestArrays& rhs) const { return !(*this == rhs); }

  wdUInt32 GetCount() const;
  double GetValue(wdUInt32 uiIndex) const;
  void SetValue(wdUInt32 uiIndex, double value);
  void Insert(wdUInt32 uiIndex, double value);
  void Remove(wdUInt32 uiIndex);

  wdUInt32 GetCountChar() const;
  const char* GetValueChar(wdUInt32 uiIndex) const;
  void SetValueChar(wdUInt32 uiIndex, const char* value);
  void InsertChar(wdUInt32 uiIndex, const char* value);
  void RemoveChar(wdUInt32 uiIndex);

  wdUInt32 GetCountDyn() const;
  const wdTestStruct3& GetValueDyn(wdUInt32 uiIndex) const;
  void SetValueDyn(wdUInt32 uiIndex, const wdTestStruct3& value);
  void InsertDyn(wdUInt32 uiIndex, const wdTestStruct3& value);
  void RemoveDyn(wdUInt32 uiIndex);

  wdUInt32 GetCountDeq() const;
  const wdTestArrays& GetValueDeq(wdUInt32 uiIndex) const;
  void SetValueDeq(wdUInt32 uiIndex, const wdTestArrays& value);
  void InsertDeq(wdUInt32 uiIndex, const wdTestArrays& value);
  void RemoveDeq(wdUInt32 uiIndex);

  wdUInt32 GetCountCustom() const;
  wdVarianceTypeAngle GetValueCustom(wdUInt32 uiIndex) const;
  void SetValueCustom(wdUInt32 uiIndex, wdVarianceTypeAngle value);
  void InsertCustom(wdUInt32 uiIndex, wdVarianceTypeAngle value);
  void RemoveCustom(wdUInt32 uiIndex);

  wdHybridArray<double, 5> m_Hybrid;
  wdHybridArray<wdString, 2> m_HybridChar;
  wdDynamicArray<wdTestStruct3> m_Dynamic;
  wdDeque<wdTestArrays> m_Deque;
  wdHybridArray<wdVarianceTypeAngle, 1> m_CustomVariant;
};


class wdTestSets : public wdReflectedClass
{
  WD_ADD_DYNAMIC_REFLECTION(wdTestSets, wdReflectedClass);

public:
  wdTestSets() {}

  bool operator==(const wdTestSets& rhs) const
  {
    return m_SetMember == rhs.m_SetMember && m_SetAccessor == rhs.m_SetAccessor && m_Deque == rhs.m_Deque && m_Array == rhs.m_Array && m_CustomVariant == rhs.m_CustomVariant;
  }

  bool operator!=(const wdTestSets& rhs) const { return !(*this == rhs); }

  const wdSet<double>& GetSet() const;
  void Insert(double value);
  void Remove(double value);

  const wdHashSet<wdInt64>& GetHashSet() const;
  void HashInsert(wdInt64 value);
  void HashRemove(wdInt64 value);

  const wdDeque<int>& GetPseudoSet() const;
  void PseudoInsert(int value);
  void PseudoRemove(int value);

  wdArrayPtr<const wdString> GetPseudoSet2() const;
  void PseudoInsert2(const wdString& value);
  void PseudoRemove2(const wdString& value);

  void PseudoInsert2b(const char* value);
  void PseudoRemove2b(const char* value);

  const wdHashSet<wdVarianceTypeAngle>& GetCustomHashSet() const;
  void CustomHashInsert(wdVarianceTypeAngle value);
  void CustomHashRemove(wdVarianceTypeAngle value);

  wdSet<wdInt8> m_SetMember;
  wdSet<double> m_SetAccessor;

  wdHashSet<wdInt32> m_HashSetMember;
  wdHashSet<wdInt64> m_HashSetAccessor;

  wdDeque<int> m_Deque;
  wdDynamicArray<wdString> m_Array;
  wdHashSet<wdVarianceTypeAngle> m_CustomVariant;
};


class wdTestMaps : public wdReflectedClass
{
  WD_ADD_DYNAMIC_REFLECTION(wdTestMaps, wdReflectedClass);

public:
  wdTestMaps() {}

  bool operator==(const wdTestMaps& rhs) const;

  const wdMap<wdString, wdInt64>& GetContainer() const;
  void Insert(const char* szKey, wdInt64 value);
  void Remove(const char* szKey);

  const wdHashTable<wdString, wdString>& GetContainer2() const;
  void Insert2(const char* szKey, const wdString& value);
  void Remove2(const char* szKey);

  const wdRangeView<const char*, wdUInt32> GetKeys3() const;
  void Insert3(const char* szKey, const wdVariant& value);
  void Remove3(const char* szKey);
  bool GetValue3(const char* szKey, wdVariant& out_value) const;

  wdMap<wdString, int> m_MapMember;
  wdMap<wdString, wdInt64> m_MapAccessor;

  wdHashTable<wdString, double> m_HashTableMember;
  wdHashTable<wdString, wdString> m_HashTableAccessor;

  wdMap<wdString, wdVarianceTypeAngle> m_CustomVariant;

  struct Tuple
  {
    wdString m_Key;
    wdVariant m_Value;
  };
  wdHybridArray<Tuple, 2> m_Accessor3;
};

class wdTestPtr : public wdReflectedClass
{
  WD_ADD_DYNAMIC_REFLECTION(wdTestPtr, wdReflectedClass);

public:
  wdTestPtr()
  {
    m_pArrays = nullptr;
    m_pArraysDirect = nullptr;
  }

  ~wdTestPtr()
  {
    WD_DEFAULT_DELETE(m_pArrays);
    WD_DEFAULT_DELETE(m_pArraysDirect);
    for (auto ptr : m_ArrayPtr)
    {
      WD_DEFAULT_DELETE(ptr);
    }
    m_ArrayPtr.Clear();
    for (auto ptr : m_SetPtr)
    {
      WD_DEFAULT_DELETE(ptr);
    }
    m_SetPtr.Clear();
  }

  bool operator==(const wdTestPtr& rhs) const
  {
    if (m_sString != rhs.m_sString || (m_pArrays != rhs.m_pArrays && *m_pArrays != *rhs.m_pArrays))
      return false;

    if (m_ArrayPtr.GetCount() != rhs.m_ArrayPtr.GetCount())
      return false;

    for (wdUInt32 i = 0; i < m_ArrayPtr.GetCount(); i++)
    {
      if (!(*m_ArrayPtr[i] == *rhs.m_ArrayPtr[i]))
        return false;
    }

    // only works for the test data if the test.
    if (m_SetPtr.IsEmpty() && rhs.m_SetPtr.IsEmpty())
      return true;

    if (m_SetPtr.GetCount() != 1 || rhs.m_SetPtr.GetCount() != 1)
      return true;

    return *m_SetPtr.GetIterator().Key() == *rhs.m_SetPtr.GetIterator().Key();
  }

  void SetString(const char* szValue) { m_sString = szValue; }
  const char* GetString() const { return m_sString; }

  void SetArrays(wdTestArrays* pValue) { m_pArrays = pValue; }
  wdTestArrays* GetArrays() const { return m_pArrays; }


  wdString m_sString;
  wdTestArrays* m_pArrays;
  wdTestArrays* m_pArraysDirect;
  wdDeque<wdTestArrays*> m_ArrayPtr;
  wdSet<wdTestSets*> m_SetPtr;
};


struct wdTestEnumStruct
{
  WD_ALLOW_PRIVATE_PROPERTIES(wdTestEnumStruct);

public:
  wdTestEnumStruct()
  {
    m_enum = wdExampleEnum::Value1;
    m_enumClass = wdExampleEnum::Value1;
    m_Enum2 = wdExampleEnum::Value1;
    m_EnumClass2 = wdExampleEnum::Value1;
  }

  bool operator==(const wdTestEnumStruct& rhs) const { return m_Enum2 == rhs.m_Enum2 && m_enum == rhs.m_enum && m_enumClass == rhs.m_enumClass && m_EnumClass2 == rhs.m_EnumClass2; }

  wdExampleEnum::Enum m_enum;
  wdEnum<wdExampleEnum> m_enumClass;

  void SetEnum(wdExampleEnum::Enum e) { m_Enum2 = e; }
  wdExampleEnum::Enum GetEnum() const { return m_Enum2; }
  void SetEnumClass(wdEnum<wdExampleEnum> e) { m_EnumClass2 = e; }
  wdEnum<wdExampleEnum> GetEnumClass() const { return m_EnumClass2; }

private:
  wdExampleEnum::Enum m_Enum2;
  wdEnum<wdExampleEnum> m_EnumClass2;
};

WD_DECLARE_REFLECTABLE_TYPE(WD_NO_LINKAGE, wdTestEnumStruct);


struct wdTestBitflagsStruct
{
  WD_ALLOW_PRIVATE_PROPERTIES(wdTestBitflagsStruct);

public:
  wdTestBitflagsStruct()
  {
    m_bitflagsClass = wdExampleBitflags::Value1;
    m_BitflagsClass2 = wdExampleBitflags::Value1;
  }

  bool operator==(const wdTestBitflagsStruct& rhs) const { return m_bitflagsClass == rhs.m_bitflagsClass && m_BitflagsClass2 == rhs.m_BitflagsClass2; }

  wdBitflags<wdExampleBitflags> m_bitflagsClass;

  void SetBitflagsClass(wdBitflags<wdExampleBitflags> e) { m_BitflagsClass2 = e; }
  wdBitflags<wdExampleBitflags> GetBitflagsClass() const { return m_BitflagsClass2; }

private:
  wdBitflags<wdExampleBitflags> m_BitflagsClass2;
};

WD_DECLARE_REFLECTABLE_TYPE(WD_NO_LINKAGE, wdTestBitflagsStruct);
