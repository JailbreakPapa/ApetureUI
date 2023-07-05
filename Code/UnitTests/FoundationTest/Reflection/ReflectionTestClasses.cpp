#include <FoundationTest/FoundationTestPCH.h>

#include <FoundationTest/Reflection/ReflectionTestClasses.h>

// clang-format off
WD_BEGIN_STATIC_REFLECTED_ENUM(wdExampleEnum, 1)
  WD_ENUM_CONSTANTS(wdExampleEnum::Value1, wdExampleEnum::Value2)
  WD_ENUM_CONSTANT(wdExampleEnum::Value3),
WD_END_STATIC_REFLECTED_ENUM;

WD_BEGIN_STATIC_REFLECTED_BITFLAGS(wdExampleBitflags, 1)
  WD_BITFLAGS_CONSTANTS(wdExampleBitflags::Value1, wdExampleBitflags::Value2)
  WD_BITFLAGS_CONSTANT(wdExampleBitflags::Value3),
WD_END_STATIC_REFLECTED_BITFLAGS;


WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdAbstractTestClass, 1, wdRTTINoAllocator)
WD_END_DYNAMIC_REFLECTED_TYPE;


WD_BEGIN_STATIC_REFLECTED_TYPE(wdAbstractTestStruct, wdNoBase, 1, wdRTTINoAllocator);
WD_END_STATIC_REFLECTED_TYPE;


WD_BEGIN_STATIC_REFLECTED_TYPE(wdTestStruct, wdNoBase, 7, wdRTTIDefaultAllocator<wdTestStruct>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_MEMBER_PROPERTY("Float", m_fFloat1)->AddAttributes(new wdDefaultValueAttribute(1.1f)),
    WD_MEMBER_PROPERTY_READ_ONLY("Vector", m_vProperty3)->AddAttributes(new wdDefaultValueAttribute(wdVec3(3.0f,4.0f,5.0f))),
    WD_ACCESSOR_PROPERTY("Int", GetInt, SetInt)->AddAttributes(new wdDefaultValueAttribute(2)),
    WD_MEMBER_PROPERTY("UInt8", m_UInt8)->AddAttributes(new wdDefaultValueAttribute(6)),
    WD_MEMBER_PROPERTY("Variant", m_variant)->AddAttributes(new wdDefaultValueAttribute("Test")),
    WD_MEMBER_PROPERTY("Angle", m_Angle)->AddAttributes(new wdDefaultValueAttribute(wdAngle::Degree(0.5))),
    WD_MEMBER_PROPERTY("DataBuffer", m_DataBuffer)->AddAttributes(new wdDefaultValueAttribute(wdTestStruct::GetDefaultDataBuffer())),
    WD_MEMBER_PROPERTY("vVec3I", m_vVec3I)->AddAttributes(new wdDefaultValueAttribute(wdVec3I32(1,2,3))),
    WD_MEMBER_PROPERTY("VarianceAngle", m_VarianceAngle)->AddAttributes(new wdDefaultValueAttribute(wdVarianceTypeAngle{0.5f, wdAngle::Degree(90.0f)})),
  }
  WD_END_PROPERTIES;
}
WD_END_STATIC_REFLECTED_TYPE;

WD_BEGIN_STATIC_REFLECTED_TYPE(wdTestStruct3, wdNoBase, 71, wdRTTIDefaultAllocator<wdTestStruct3>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_MEMBER_PROPERTY("Float", m_fFloat1)->AddAttributes(new wdDefaultValueAttribute(33.3f)),
    WD_ACCESSOR_PROPERTY("Int", GetInt, SetInt),
    WD_MEMBER_PROPERTY("UInt8", m_UInt8),
  }
  WD_END_PROPERTIES;
  WD_BEGIN_FUNCTIONS
  {
    WD_CONSTRUCTOR_PROPERTY(),
    WD_CONSTRUCTOR_PROPERTY(double, wdInt16),
  }
  WD_END_FUNCTIONS;
}
WD_END_STATIC_REFLECTED_TYPE;

WD_BEGIN_STATIC_REFLECTED_TYPE(wdTypedObjectStruct, wdNoBase, 1, wdRTTIDefaultAllocator<wdTypedObjectStruct>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_MEMBER_PROPERTY("Float", m_fFloat1)->AddAttributes(new wdDefaultValueAttribute(33.3f)),
    WD_MEMBER_PROPERTY("Int", m_iInt32),
    WD_MEMBER_PROPERTY("UInt8", m_UInt8),
  }
  WD_END_PROPERTIES;
}
WD_END_STATIC_REFLECTED_TYPE;

WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdTestClass1, 11, wdRTTIDefaultAllocator<wdTestClass1>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_MEMBER_PROPERTY("SubStruct", m_Struct),
    // WD_MEMBER_PROPERTY("MyVector", m_MyVector), Intentionally not reflected
    WD_MEMBER_PROPERTY("Color", m_Color),
    WD_ACCESSOR_PROPERTY_READ_ONLY("SubVector", GetVector)->AddAttributes(new wdDefaultValueAttribute(wdVec3(3, 4, 5)))
  }
    WD_END_PROPERTIES;
}
WD_END_DYNAMIC_REFLECTED_TYPE;

wdInt32 wdTestClass2Allocator::m_iAllocs = 0;
wdInt32 wdTestClass2Allocator::m_iDeallocs = 0;

WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdTestClass2, 22, wdTestClass2Allocator)
{
  WD_BEGIN_PROPERTIES
  {
    WD_ACCESSOR_PROPERTY("Text", GetText, SetText)->AddAttributes(new wdDefaultValueAttribute("Legen")),
    WD_MEMBER_PROPERTY("Time", m_Time),
    WD_ENUM_MEMBER_PROPERTY("Enum", wdExampleEnum, m_enumClass),
    WD_BITFLAGS_MEMBER_PROPERTY("Bitflags", wdExampleBitflags, m_bitflagsClass),
    WD_ARRAY_MEMBER_PROPERTY("Array", m_array),
    WD_MEMBER_PROPERTY("Variant", m_Variant),
  }
  WD_END_PROPERTIES;
}
WD_END_DYNAMIC_REFLECTED_TYPE;

WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdTestClass2b, 24, wdRTTIDefaultAllocator<wdTestClass2b>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_ACCESSOR_PROPERTY("Text2b", GetText, SetText),
    WD_MEMBER_PROPERTY("SubStruct", m_Struct),
    WD_MEMBER_PROPERTY("Color", m_Color),
  }
  WD_END_PROPERTIES;
}
WD_END_DYNAMIC_REFLECTED_TYPE;

WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdTestArrays, 1, wdRTTIDefaultAllocator<wdTestArrays>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_ARRAY_MEMBER_PROPERTY("Hybrid", m_Hybrid),
    WD_ARRAY_MEMBER_PROPERTY("HybridChar", m_HybridChar),
    WD_ARRAY_MEMBER_PROPERTY("Dynamic", m_Dynamic),
    WD_ARRAY_MEMBER_PROPERTY("Deque", m_Deque),
    WD_ARRAY_MEMBER_PROPERTY("Custom", m_CustomVariant),

    WD_ARRAY_MEMBER_PROPERTY_READ_ONLY("HybridRO", m_Hybrid),
    WD_ARRAY_MEMBER_PROPERTY_READ_ONLY("HybridCharRO", m_HybridChar),
    WD_ARRAY_MEMBER_PROPERTY_READ_ONLY("DynamicRO", m_Dynamic),
    WD_ARRAY_MEMBER_PROPERTY_READ_ONLY("DequeRO", m_Deque),
    WD_ARRAY_MEMBER_PROPERTY_READ_ONLY("CustomRO", m_CustomVariant),

    WD_ARRAY_ACCESSOR_PROPERTY("AcHybrid", GetCount, GetValue, SetValue, Insert, Remove),
    WD_ARRAY_ACCESSOR_PROPERTY_READ_ONLY("AcHybridRO", GetCount, GetValue),
    WD_ARRAY_ACCESSOR_PROPERTY("AcHybridChar", GetCountChar, GetValueChar, SetValueChar, InsertChar, RemoveChar),
    WD_ARRAY_ACCESSOR_PROPERTY_READ_ONLY("AcHybridCharRO", GetCountChar, GetValueChar),
    WD_ARRAY_ACCESSOR_PROPERTY("AcDynamic", GetCountDyn, GetValueDyn, SetValueDyn, InsertDyn, RemoveDyn),
    WD_ARRAY_ACCESSOR_PROPERTY_READ_ONLY("AcDynamicRO", GetCountDyn, GetValueDyn),
    WD_ARRAY_ACCESSOR_PROPERTY("AcDeque", GetCountDeq, GetValueDeq, SetValueDeq, InsertDeq, RemoveDeq),
    WD_ARRAY_ACCESSOR_PROPERTY_READ_ONLY("AcDequeRO", GetCountDeq, GetValueDeq),
    WD_ARRAY_ACCESSOR_PROPERTY("AcCustom", GetCountCustom, GetValueCustom, SetValueCustom, InsertCustom, RemoveCustom),
    WD_ARRAY_ACCESSOR_PROPERTY_READ_ONLY("AcCustomRO", GetCountCustom, GetValueCustom),
  }
  WD_END_PROPERTIES;
}
WD_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

wdUInt32 wdTestArrays::GetCount() const
{
  return m_Hybrid.GetCount();
}
double wdTestArrays::GetValue(wdUInt32 uiIndex) const
{
  return m_Hybrid[uiIndex];
}
void wdTestArrays::SetValue(wdUInt32 uiIndex, double value)
{
  m_Hybrid[uiIndex] = value;
}
void wdTestArrays::Insert(wdUInt32 uiIndex, double value)
{
  m_Hybrid.Insert(value, uiIndex);
}
void wdTestArrays::Remove(wdUInt32 uiIndex)
{
  m_Hybrid.RemoveAtAndCopy(uiIndex);
}

wdUInt32 wdTestArrays::GetCountChar() const
{
  return m_HybridChar.GetCount();
}
const char* wdTestArrays::GetValueChar(wdUInt32 uiIndex) const
{
  return m_HybridChar[uiIndex];
}
void wdTestArrays::SetValueChar(wdUInt32 uiIndex, const char* value)
{
  m_HybridChar[uiIndex] = value;
}
void wdTestArrays::InsertChar(wdUInt32 uiIndex, const char* value)
{
  m_HybridChar.Insert(value, uiIndex);
}
void wdTestArrays::RemoveChar(wdUInt32 uiIndex)
{
  m_HybridChar.RemoveAtAndCopy(uiIndex);
}

wdUInt32 wdTestArrays::GetCountDyn() const
{
  return m_Dynamic.GetCount();
}
const wdTestStruct3& wdTestArrays::GetValueDyn(wdUInt32 uiIndex) const
{
  return m_Dynamic[uiIndex];
}
void wdTestArrays::SetValueDyn(wdUInt32 uiIndex, const wdTestStruct3& value)
{
  m_Dynamic[uiIndex] = value;
}
void wdTestArrays::InsertDyn(wdUInt32 uiIndex, const wdTestStruct3& value)
{
  m_Dynamic.Insert(value, uiIndex);
}
void wdTestArrays::RemoveDyn(wdUInt32 uiIndex)
{
  m_Dynamic.RemoveAtAndCopy(uiIndex);
}

wdUInt32 wdTestArrays::GetCountDeq() const
{
  return m_Deque.GetCount();
}
const wdTestArrays& wdTestArrays::GetValueDeq(wdUInt32 uiIndex) const
{
  return m_Deque[uiIndex];
}
void wdTestArrays::SetValueDeq(wdUInt32 uiIndex, const wdTestArrays& value)
{
  m_Deque[uiIndex] = value;
}
void wdTestArrays::InsertDeq(wdUInt32 uiIndex, const wdTestArrays& value)
{
  m_Deque.Insert(value, uiIndex);
}
void wdTestArrays::RemoveDeq(wdUInt32 uiIndex)
{
  m_Deque.RemoveAtAndCopy(uiIndex);
}

wdUInt32 wdTestArrays::GetCountCustom() const
{
  return m_CustomVariant.GetCount();
}
wdVarianceTypeAngle wdTestArrays::GetValueCustom(wdUInt32 uiIndex) const
{
  return m_CustomVariant[uiIndex];
}
void wdTestArrays::SetValueCustom(wdUInt32 uiIndex, wdVarianceTypeAngle value)
{
  m_CustomVariant[uiIndex] = value;
}
void wdTestArrays::InsertCustom(wdUInt32 uiIndex, wdVarianceTypeAngle value)
{
  m_CustomVariant.Insert(value, uiIndex);
}
void wdTestArrays::RemoveCustom(wdUInt32 uiIndex)
{
  m_CustomVariant.RemoveAtAndCopy(uiIndex);
}

// clang-format off
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdTestSets, 1, wdRTTIDefaultAllocator<wdTestSets>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_SET_MEMBER_PROPERTY("Set", m_SetMember),
    WD_SET_MEMBER_PROPERTY_READ_ONLY("SetRO", m_SetMember),
    WD_SET_ACCESSOR_PROPERTY("AcSet", GetSet, Insert, Remove),
    WD_SET_ACCESSOR_PROPERTY_READ_ONLY("AcSetRO", GetSet),
    WD_SET_MEMBER_PROPERTY("HashSet", m_HashSetMember),
    WD_SET_MEMBER_PROPERTY_READ_ONLY("HashSetRO", m_HashSetMember),
    WD_SET_ACCESSOR_PROPERTY("HashAcSet", GetHashSet, HashInsert, HashRemove),
    WD_SET_ACCESSOR_PROPERTY_READ_ONLY("HashAcSetRO", GetHashSet),
    WD_SET_ACCESSOR_PROPERTY("AcPseudoSet", GetPseudoSet, PseudoInsert, PseudoRemove),
    WD_SET_ACCESSOR_PROPERTY_READ_ONLY("AcPseudoSetRO", GetPseudoSet),
    WD_SET_ACCESSOR_PROPERTY("AcPseudoSet2", GetPseudoSet2, PseudoInsert2, PseudoRemove2),
    WD_SET_ACCESSOR_PROPERTY_READ_ONLY("AcPseudoSet2RO", GetPseudoSet2),
    WD_SET_ACCESSOR_PROPERTY("AcPseudoSet2b", GetPseudoSet2, PseudoInsert2b, PseudoRemove2b),
    WD_SET_MEMBER_PROPERTY("CustomHashSet", m_CustomVariant),
    WD_SET_MEMBER_PROPERTY_READ_ONLY("CustomHashSetRO", m_CustomVariant),
    WD_SET_ACCESSOR_PROPERTY("CustomHashAcSet", GetCustomHashSet, CustomHashInsert, CustomHashRemove),
    WD_SET_ACCESSOR_PROPERTY_READ_ONLY("CustomHashAcSetRO", GetCustomHashSet),
  }
  WD_END_PROPERTIES;
}
WD_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

const wdSet<double>& wdTestSets::GetSet() const
{
  return m_SetAccessor;
}

void wdTestSets::Insert(double value)
{
  m_SetAccessor.Insert(value);
}

void wdTestSets::Remove(double value)
{
  m_SetAccessor.Remove(value);
}


const wdHashSet<wdInt64>& wdTestSets::GetHashSet() const
{
  return m_HashSetAccessor;
}

void wdTestSets::HashInsert(wdInt64 value)
{
  m_HashSetAccessor.Insert(value);
}

void wdTestSets::HashRemove(wdInt64 value)
{
  m_HashSetAccessor.Remove(value);
}

const wdDeque<int>& wdTestSets::GetPseudoSet() const
{
  return m_Deque;
}

void wdTestSets::PseudoInsert(int value)
{
  if (!m_Deque.Contains(value))
    m_Deque.PushBack(value);
}

void wdTestSets::PseudoRemove(int value)
{
  m_Deque.RemoveAndCopy(value);
}


wdArrayPtr<const wdString> wdTestSets::GetPseudoSet2() const
{
  return m_Array;
}

void wdTestSets::PseudoInsert2(const wdString& value)
{
  if (!m_Array.Contains(value))
    m_Array.PushBack(value);
}

void wdTestSets::PseudoRemove2(const wdString& value)
{
  m_Array.RemoveAndCopy(value);
}

void wdTestSets::PseudoInsert2b(const char* value)
{
  if (!m_Array.Contains(value))
    m_Array.PushBack(value);
}

void wdTestSets::PseudoRemove2b(const char* value)
{
  m_Array.RemoveAndCopy(value);
}

const wdHashSet<wdVarianceTypeAngle>& wdTestSets::GetCustomHashSet() const
{
  return m_CustomVariant;
}

void wdTestSets::CustomHashInsert(wdVarianceTypeAngle value)
{
  m_CustomVariant.Insert(value);
}

void wdTestSets::CustomHashRemove(wdVarianceTypeAngle value)
{
  m_CustomVariant.Remove(value);
}

// clang-format off
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdTestMaps, 1, wdRTTIDefaultAllocator<wdTestMaps>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_MAP_MEMBER_PROPERTY("Map", m_MapMember),
    WD_MAP_MEMBER_PROPERTY_READ_ONLY("MapRO", m_MapMember),
    WD_MAP_WRITE_ACCESSOR_PROPERTY("AcMap", GetContainer, Insert, Remove),
    WD_MAP_MEMBER_PROPERTY("HashTable", m_HashTableMember),
    WD_MAP_MEMBER_PROPERTY_READ_ONLY("HashTableRO", m_HashTableMember),
    WD_MAP_WRITE_ACCESSOR_PROPERTY("AcHashTable", GetContainer2, Insert2, Remove2),
    WD_MAP_ACCESSOR_PROPERTY("Accessor", GetKeys3, GetValue3, Insert3, Remove3),
    WD_MAP_ACCESSOR_PROPERTY_READ_ONLY("AccessorRO", GetKeys3, GetValue3),
    WD_MAP_MEMBER_PROPERTY("CustomVariant", m_CustomVariant),
    WD_MAP_MEMBER_PROPERTY_READ_ONLY("CustomVariantRO", m_CustomVariant),
  }
  WD_END_PROPERTIES;
}
WD_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

bool wdTestMaps::operator==(const wdTestMaps& rhs) const
{
  for (wdUInt32 i = 0; i < m_Accessor3.GetCount(); i++)
  {
    bool bRes = false;
    for (wdUInt32 j = 0; j < rhs.m_Accessor3.GetCount(); j++)
    {
      if (m_Accessor3[i].m_Key == rhs.m_Accessor3[j].m_Key)
      {
        if (m_Accessor3[i].m_Value == rhs.m_Accessor3[j].m_Value)
          bRes = true;
      }
    }
    if (!bRes)
      return false;
  }
  return m_MapMember == rhs.m_MapMember && m_MapAccessor == rhs.m_MapAccessor && m_HashTableMember == rhs.m_HashTableMember && m_HashTableAccessor == rhs.m_HashTableAccessor && m_CustomVariant == rhs.m_CustomVariant;
}

const wdMap<wdString, wdInt64>& wdTestMaps::GetContainer() const
{
  return m_MapAccessor;
}

void wdTestMaps::Insert(const char* szKey, wdInt64 value)
{
  m_MapAccessor.Insert(szKey, value);
}

void wdTestMaps::Remove(const char* szKey)
{
  m_MapAccessor.Remove(szKey);
}

const wdHashTable<wdString, wdString>& wdTestMaps::GetContainer2() const
{
  return m_HashTableAccessor;
}

void wdTestMaps::Insert2(const char* szKey, const wdString& value)
{
  m_HashTableAccessor.Insert(szKey, value);
}


void wdTestMaps::Remove2(const char* szKey)
{
  m_HashTableAccessor.Remove(szKey);
}

const wdRangeView<const char*, wdUInt32> wdTestMaps::GetKeys3() const
{
  return wdRangeView<const char*, wdUInt32>([this]() -> wdUInt32 { return 0; }, [this]() -> wdUInt32 { return m_Accessor3.GetCount(); }, [this](wdUInt32& ref_uiIt) { ++ref_uiIt; }, [this](const wdUInt32& uiIt) -> const char* { return m_Accessor3[uiIt].m_Key; });
}

void wdTestMaps::Insert3(const char* szKey, const wdVariant& value)
{
  for (auto&& t : m_Accessor3)
  {
    if (t.m_Key == szKey)
    {
      t.m_Value = value;
      return;
    }
  }
  auto&& t = m_Accessor3.ExpandAndGetRef();
  t.m_Key = szKey;
  t.m_Value = value;
}

void wdTestMaps::Remove3(const char* szKey)
{
  for (wdUInt32 i = 0; i < m_Accessor3.GetCount(); i++)
  {
    const Tuple& t = m_Accessor3[i];
    if (t.m_Key == szKey)
    {
      m_Accessor3.RemoveAtAndSwap(i);
      break;
    }
  }
}

bool wdTestMaps::GetValue3(const char* szKey, wdVariant& out_value) const
{
  for (const auto& t : m_Accessor3)
  {
    if (t.m_Key == szKey)
    {
      out_value = t.m_Value;
      return true;
    }
  }
  return false;
}

// clang-format off
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdTestPtr, 1, wdRTTIDefaultAllocator<wdTestPtr>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_ACCESSOR_PROPERTY("ConstCharPtr", GetString, SetString),
    WD_ACCESSOR_PROPERTY("ArraysPtr", GetArrays, SetArrays)->AddFlags(wdPropertyFlags::PointerOwner),
    WD_MEMBER_PROPERTY("ArraysPtrDirect", m_pArraysDirect)->AddFlags(wdPropertyFlags::PointerOwner),
    WD_ARRAY_MEMBER_PROPERTY("PtrArray", m_ArrayPtr)->AddFlags(wdPropertyFlags::PointerOwner),
    WD_SET_MEMBER_PROPERTY("PtrSet", m_SetPtr)->AddFlags(wdPropertyFlags::PointerOwner),
  }
  WD_END_PROPERTIES;
}
WD_END_DYNAMIC_REFLECTED_TYPE;


WD_BEGIN_STATIC_REFLECTED_TYPE(wdTestEnumStruct, wdNoBase, 1, wdRTTIDefaultAllocator<wdTestEnumStruct>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_ENUM_MEMBER_PROPERTY("m_enum", wdExampleEnum, m_enum),
    WD_ENUM_MEMBER_PROPERTY("m_enumClass", wdExampleEnum, m_enumClass),
    WD_ENUM_ACCESSOR_PROPERTY("m_enum2", wdExampleEnum, GetEnum, SetEnum),
    WD_ENUM_ACCESSOR_PROPERTY("m_enumClass2", wdExampleEnum,  GetEnumClass, SetEnumClass),
  }
  WD_END_PROPERTIES;
}
WD_END_STATIC_REFLECTED_TYPE;

WD_BEGIN_STATIC_REFLECTED_TYPE(wdTestBitflagsStruct, wdNoBase, 1, wdRTTIDefaultAllocator<wdTestBitflagsStruct>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_BITFLAGS_MEMBER_PROPERTY("m_bitflagsClass", wdExampleBitflags, m_bitflagsClass),
    WD_BITFLAGS_ACCESSOR_PROPERTY("m_bitflagsClass2", wdExampleBitflags, GetBitflagsClass, SetBitflagsClass),
  }
  WD_END_PROPERTIES;
}
WD_END_STATIC_REFLECTED_TYPE;
// clang-format on
