#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/IO/MemoryStream.h>
#include <Foundation/Reflection/ReflectionUtils.h>
#include <Foundation/Serialization/AbstractObjectGraph.h>
#include <Foundation/Serialization/BinarySerializer.h>
#include <Foundation/Serialization/DdlSerializer.h>
#include <Foundation/Serialization/ReflectionSerializer.h>
#include <Foundation/Serialization/RttiConverter.h>
#include <FoundationTest/Reflection/ReflectionTestClasses.h>

WD_CREATE_SIMPLE_TEST_GROUP(Serialization);

class TestContext : public wdRttiConverterContext
{
public:
  virtual wdInternal::NewInstance<void> CreateObject(const wdUuid& guid, const wdRTTI* pRtti) override
  {
    auto pObj = pRtti->GetAllocator()->Allocate<void>();
    RegisterObject(guid, pRtti, pObj);
    return pObj;
  }

  virtual void DeleteObject(const wdUuid& guid) override
  {
    auto object = GetObjectByGUID(guid);
    object.m_pType->GetAllocator()->Deallocate(object.m_pObject);

    UnregisterObject(guid);
  }
};

template <typename T>
void TestSerialize(T* pObject)
{
  wdAbstractObjectGraph graph;
  TestContext context;
  wdRttiConverterWriter conv(&graph, &context, true, true);

  const wdRTTI* pRtti = wdGetStaticRTTI<T>();
  wdUuid guid;
  guid.CreateNewUuid();

  context.RegisterObject(guid, pRtti, pObject);
  wdAbstractObjectNode* pNode = conv.AddObjectToGraph(pRtti, pObject, "root");

  WD_TEST_BOOL(pNode->GetGuid() == guid);
  WD_TEST_STRING(pNode->GetType(), pRtti->GetTypeName());
  WD_TEST_INT(pNode->GetProperties().GetCount(), pNode->GetProperties().GetCount());

  {
    wdContiguousMemoryStreamStorage storage;
    wdMemoryStreamWriter writer(&storage);
    wdMemoryStreamReader reader(&storage);

    wdAbstractGraphDdlSerializer::Write(writer, &graph);

    wdStringBuilder sData, sData2;
    sData.SetSubString_ElementCount((const char*)storage.GetData(), storage.GetStorageSize32());


    wdRttiConverterReader convRead(&graph, &context);
    auto* pRootNode = graph.GetNodeByName("root");
    WD_TEST_BOOL(pRootNode != nullptr);

    T target;
    convRead.ApplyPropertiesToObject(pRootNode, pRtti, &target);
    WD_TEST_BOOL(target == *pObject);

    // Overwrite again to test for leaks as existing values have to be removed first by wdRttiConverterReader.
    convRead.ApplyPropertiesToObject(pRootNode, pRtti, &target);
    WD_TEST_BOOL(target == *pObject);

    {
      T clone;
      wdReflectionSerializer::Clone(pObject, &clone, pRtti);
      WD_TEST_BOOL(clone == *pObject);
      WD_TEST_BOOL(wdReflectionUtils::IsEqual(&clone, pObject, pRtti));
    }

    {
      T* pClone = wdReflectionSerializer::Clone(pObject);
      WD_TEST_BOOL(*pClone == *pObject);
      WD_TEST_BOOL(wdReflectionUtils::IsEqual(pClone, pObject));
      // Overwrite again to test for leaks as existing values have to be removed first by clone.
      wdReflectionSerializer::Clone(pObject, pClone, pRtti);
      WD_TEST_BOOL(*pClone == *pObject);
      WD_TEST_BOOL(wdReflectionUtils::IsEqual(pClone, pObject, pRtti));
      pRtti->GetAllocator()->Deallocate(pClone);
    }

    wdAbstractObjectGraph graph2;
    wdAbstractGraphDdlSerializer::Read(reader, &graph2).IgnoreResult();

    wdContiguousMemoryStreamStorage storage2;
    wdMemoryStreamWriter writer2(&storage2);

    wdAbstractGraphDdlSerializer::Write(writer2, &graph2);
    sData2.SetSubString_ElementCount((const char*)storage2.GetData(), storage2.GetStorageSize32());

    WD_TEST_BOOL(sData == sData2);
  }

  {
    wdContiguousMemoryStreamStorage storage;
    wdMemoryStreamWriter writer(&storage);
    wdMemoryStreamReader reader(&storage);

    wdAbstractGraphBinarySerializer::Write(writer, &graph);

    wdRttiConverterReader convRead(&graph, &context);
    auto* pRootNode = graph.GetNodeByName("root");
    WD_TEST_BOOL(pRootNode != nullptr);

    T target;
    convRead.ApplyPropertiesToObject(pRootNode, pRtti, &target);
    WD_TEST_BOOL(target == *pObject);

    wdAbstractObjectGraph graph2;
    wdAbstractGraphBinarySerializer::Read(reader, &graph2);

    wdContiguousMemoryStreamStorage storage2;
    wdMemoryStreamWriter writer2(&storage2);

    wdAbstractGraphBinarySerializer::Write(writer2, &graph2);

    WD_TEST_INT(storage.GetStorageSize32(), storage2.GetStorageSize32());

    if (storage.GetStorageSize32() == storage2.GetStorageSize32())
    {
      WD_TEST_BOOL(wdMemoryUtils::RawByteCompare(storage.GetData(), storage2.GetData(), storage.GetStorageSize32()) == 0);
    }
  }
}

WD_CREATE_SIMPLE_TEST(Serialization, RttiConverter)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "PODs")
  {
    wdTestStruct t1;
    t1.m_fFloat1 = 5.0f;
    t1.m_UInt8 = 222;
    t1.m_variant = "A";
    t1.m_Angle = wdAngle::Degree(5);
    t1.m_DataBuffer.PushBack(1);
    t1.m_DataBuffer.PushBack(5);
    t1.m_vVec3I = wdVec3I32(0, 1, 333);
    TestSerialize(&t1);

    {
      wdTestStruct clone;
      wdReflectionSerializer::Clone(&t1, &clone, wdGetStaticRTTI<wdTestStruct>());
      WD_TEST_BOOL(t1 == clone);
      WD_TEST_BOOL(wdReflectionUtils::IsEqual(&t1, &clone, wdGetStaticRTTI<wdTestStruct>()));
      clone.m_variant = "Test";
      WD_TEST_BOOL(!wdReflectionUtils::IsEqual(&t1, &clone, wdGetStaticRTTI<wdTestStruct>()));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "EmbededStruct")
  {
    wdTestClass1 t1;
    t1.m_Color = wdColor::Yellow;
    t1.m_Struct.m_fFloat1 = 5.0f;
    t1.m_Struct.m_UInt8 = 222;
    t1.m_Struct.m_variant = "A";
    t1.m_Struct.m_Angle = wdAngle::Degree(5);
    t1.m_Struct.m_DataBuffer.PushBack(1);
    t1.m_Struct.m_DataBuffer.PushBack(5);
    t1.m_Struct.m_vVec3I = wdVec3I32(0, 1, 333);
    TestSerialize(&t1);

    {
      wdTestClass1 clone;
      wdReflectionSerializer::Clone(&t1, &clone, wdGetStaticRTTI<wdTestClass1>());
      WD_TEST_BOOL(t1 == clone);
      WD_TEST_BOOL(wdReflectionUtils::IsEqual(&t1, &clone, wdGetStaticRTTI<wdTestClass1>()));
      clone.m_Struct.m_DataBuffer[1] = 6;
      WD_TEST_BOOL(!wdReflectionUtils::IsEqual(&t1, &clone, wdGetStaticRTTI<wdTestClass1>()));
      clone.m_Struct.m_DataBuffer[1] = 5;
      clone.m_Struct.m_variant = wdVec3(1, 2, 3);
      WD_TEST_BOOL(!wdReflectionUtils::IsEqual(&t1, &clone, wdGetStaticRTTI<wdTestClass1>()));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Enum")
  {
    wdTestEnumStruct t1;
    t1.m_enum = wdExampleEnum::Value2;
    t1.m_enumClass = wdExampleEnum::Value3;
    t1.SetEnum(wdExampleEnum::Value2);
    t1.SetEnumClass(wdExampleEnum::Value3);
    TestSerialize(&t1);

    {
      wdTestEnumStruct clone;
      wdReflectionSerializer::Clone(&t1, &clone, wdGetStaticRTTI<wdTestEnumStruct>());
      WD_TEST_BOOL(t1 == clone);
      WD_TEST_BOOL(wdReflectionUtils::IsEqual(&t1, &clone, wdGetStaticRTTI<wdTestEnumStruct>()));
      clone.m_enum = wdExampleEnum::Value3;
      WD_TEST_BOOL(!wdReflectionUtils::IsEqual(&t1, &clone, wdGetStaticRTTI<wdTestEnumStruct>()));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Bitflags")
  {
    wdTestBitflagsStruct t1;
    t1.m_bitflagsClass.SetValue(0);
    t1.SetBitflagsClass(wdExampleBitflags::Value1 | wdExampleBitflags::Value2);
    TestSerialize(&t1);

    {
      wdTestBitflagsStruct clone;
      wdReflectionSerializer::Clone(&t1, &clone, wdGetStaticRTTI<wdTestBitflagsStruct>());
      WD_TEST_BOOL(t1 == clone);
      WD_TEST_BOOL(wdReflectionUtils::IsEqual(&t1, &clone, wdGetStaticRTTI<wdTestBitflagsStruct>()));
      clone.m_bitflagsClass = wdExampleBitflags::Value1;
      WD_TEST_BOOL(!wdReflectionUtils::IsEqual(&t1, &clone, wdGetStaticRTTI<wdTestBitflagsStruct>()));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Derived Class")
  {
    wdTestClass2 t1;
    t1.m_Color = wdColor::Yellow;
    t1.m_Struct.m_fFloat1 = 5.0f;
    t1.m_Struct.m_UInt8 = 222;
    t1.m_Struct.m_variant = "A";
    t1.m_Struct.m_Angle = wdAngle::Degree(5);
    t1.m_Struct.m_DataBuffer.PushBack(1);
    t1.m_Struct.m_DataBuffer.PushBack(5);
    t1.m_Struct.m_vVec3I = wdVec3I32(0, 1, 333);
    t1.m_Time = wdTime::Seconds(22.2f);
    t1.m_enumClass = wdExampleEnum::Value3;
    t1.m_bitflagsClass = wdExampleBitflags::Value1 | wdExampleBitflags::Value2;
    t1.m_array.PushBack(40.0f);
    t1.m_array.PushBack(-1.5f);
    t1.m_Variant = wdVec4(1, 2, 3, 4);
    t1.SetText("LALALALA");
    TestSerialize(&t1);

    {
      wdTestClass2 clone;
      wdReflectionSerializer::Clone(&t1, &clone, wdGetStaticRTTI<wdTestClass2>());
      WD_TEST_BOOL(t1 == clone);
      WD_TEST_BOOL(wdReflectionUtils::IsEqual(&t1, &clone, wdGetStaticRTTI<wdTestClass2>()));
      clone.m_Struct.m_DataBuffer[1] = 6;
      WD_TEST_BOOL(!wdReflectionUtils::IsEqual(&t1, &clone, wdGetStaticRTTI<wdTestClass2>()));
      clone.m_Struct.m_DataBuffer[1] = 5;
      t1.m_array.PushBack(-1.33f);
      WD_TEST_BOOL(!wdReflectionUtils::IsEqual(&t1, &clone, wdGetStaticRTTI<wdTestClass2>()));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Arrays")
  {
    wdTestArrays t1;
    t1.m_Hybrid.PushBack(4.5f);
    t1.m_Hybrid.PushBack(2.3f);
    t1.m_HybridChar.PushBack("Test");

    wdTestStruct3 ts;
    ts.m_fFloat1 = 5.0f;
    ts.m_UInt8 = 22;
    t1.m_Dynamic.PushBack(ts);
    t1.m_Dynamic.PushBack(ts);
    t1.m_Deque.PushBack(wdTestArrays());
    TestSerialize(&t1);

    {
      wdTestArrays clone;
      wdReflectionSerializer::Clone(&t1, &clone, wdGetStaticRTTI<wdTestArrays>());
      WD_TEST_BOOL(t1 == clone);
      WD_TEST_BOOL(wdReflectionUtils::IsEqual(&t1, &clone, wdGetStaticRTTI<wdTestArrays>()));
      clone.m_Dynamic.PushBack(wdTestStruct3());
      WD_TEST_BOOL(!wdReflectionUtils::IsEqual(&t1, &clone, wdGetStaticRTTI<wdTestArrays>()));
      clone.m_Dynamic.PopBack();
      clone.m_Hybrid.PushBack(444.0f);
      WD_TEST_BOOL(!wdReflectionUtils::IsEqual(&t1, &clone, wdGetStaticRTTI<wdTestArrays>()));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Sets")
  {
    wdTestSets t1;
    t1.m_SetMember.Insert(0);
    t1.m_SetMember.Insert(5);
    t1.m_SetMember.Insert(-33);
    t1.m_SetAccessor.Insert(-0.0f);
    t1.m_SetAccessor.Insert(5.4f);
    t1.m_SetAccessor.Insert(-33.0f);
    t1.m_Deque.PushBack(3);
    t1.m_Deque.PushBack(33);
    t1.m_Array.PushBack("Test");
    t1.m_Array.PushBack("Bla");
    TestSerialize(&t1);

    {
      wdTestSets clone;
      wdReflectionSerializer::Clone(&t1, &clone, wdGetStaticRTTI<wdTestSets>());
      WD_TEST_BOOL(t1 == clone);
      WD_TEST_BOOL(wdReflectionUtils::IsEqual(&t1, &clone, wdGetStaticRTTI<wdTestSets>()));
      clone.m_SetMember.Insert(12);
      WD_TEST_BOOL(!wdReflectionUtils::IsEqual(&t1, &clone, wdGetStaticRTTI<wdTestSets>()));
      clone.m_SetMember.Remove(12);
      clone.m_Array.PushBack("Bla2");
      WD_TEST_BOOL(!wdReflectionUtils::IsEqual(&t1, &clone, wdGetStaticRTTI<wdTestSets>()));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Pointer")
  {
    wdTestPtr t1;
    t1.m_sString = "Ttttest";
    t1.m_pArrays = WD_DEFAULT_NEW(wdTestArrays);
    t1.m_pArraysDirect = WD_DEFAULT_NEW(wdTestArrays);
    t1.m_ArrayPtr.PushBack(WD_DEFAULT_NEW(wdTestArrays));
    t1.m_SetPtr.Insert(WD_DEFAULT_NEW(wdTestSets));
    TestSerialize(&t1);

    {
      wdTestPtr clone;
      wdReflectionSerializer::Clone(&t1, &clone, wdGetStaticRTTI<wdTestPtr>());
      WD_TEST_BOOL(t1 == clone);
      WD_TEST_BOOL(wdReflectionUtils::IsEqual(&t1, &clone, wdGetStaticRTTI<wdTestPtr>()));
      clone.m_SetPtr.GetIterator().Key()->m_Deque.PushBack(42);
      WD_TEST_BOOL(!wdReflectionUtils::IsEqual(&t1, &clone, wdGetStaticRTTI<wdTestPtr>()));
      clone.m_SetPtr.GetIterator().Key()->m_Deque.PopBack();
      clone.m_ArrayPtr[0]->m_Hybrid.PushBack(123.0f);
      WD_TEST_BOOL(!wdReflectionUtils::IsEqual(&t1, &clone, wdGetStaticRTTI<wdTestPtr>()));
    }
  }
}
