#include <ToolsFoundationTest/ToolsFoundationTestPCH.h>

#include <ToolsFoundationTest/Object/TestObjectManager.h>
#include <ToolsFoundationTest/Reflection/ReflectionTestClasses.h>

WD_CREATE_SIMPLE_TEST(DocumentObject, CommandHistory)
{
  wdTestDocument doc("Test", true);
  doc.InitializeAfterLoading(false);
  wdObjectAccessorBase* pAccessor = doc.GetObjectAccessor();

  auto CreateObject = [&doc, &pAccessor](const wdRTTI* pType) -> const wdDocumentObject* {
    wdUuid objGuid;
    pAccessor->StartTransaction("Add Object");
    WD_TEST_STATUS(pAccessor->AddObject(nullptr, (const wdAbstractProperty*)nullptr, -1, pType, objGuid));
    pAccessor->FinishTransaction();
    return pAccessor->GetObject(objGuid);
  };

  auto StoreOriginalState = [&doc](wdAbstractObjectGraph& ref_graph, const wdDocumentObject* pRoot) {
    wdDocumentObjectConverterWriter writer(&ref_graph, doc.GetObjectManager(), [](const wdDocumentObject*, const wdAbstractProperty* p) { return p->GetAttributeByType<wdHiddenAttribute>() == nullptr; });
    wdAbstractObjectNode* pAbstractObj = writer.AddObjectToGraph(pRoot);
  };

  auto CompareAgainstOriginalState = [&doc](wdAbstractObjectGraph& ref_original, const wdDocumentObject* pRoot) {
    wdAbstractObjectGraph graph;
    wdDocumentObjectConverterWriter writer2(&graph, doc.GetObjectManager(), [](const wdDocumentObject*, const wdAbstractProperty* p) { return p->GetAttributeByType<wdHiddenAttribute>() == nullptr; });
    wdAbstractObjectNode* pAbstractObj2 = writer2.AddObjectToGraph(pRoot);

    wdDeque<wdAbstractGraphDiffOperation> diff;
    graph.CreateDiffWithBaseGraph(ref_original, diff);
    WD_TEST_BOOL(diff.GetCount() == 0);
  };

  const wdDocumentObject* pRoot = CreateObject(wdGetStaticRTTI<wdMirrorTest>());

  wdUuid mathGuid = pAccessor->Get<wdUuid>(pRoot, "Math");
  wdUuid objectGuid = pAccessor->Get<wdUuid>(pRoot, "Object");

  const wdDocumentObject* pMath = pAccessor->GetObject(mathGuid);
  const wdDocumentObject* pObjectTest = pAccessor->GetObject(objectGuid);

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetValue")
  {

    WD_TEST_INT(doc.GetCommandHistory()->GetUndoStackSize(), 1);

    auto TestSetValue = [&](const wdDocumentObject* pObject, const char* szProperty, wdVariant value) {
      wdAbstractObjectGraph graph;
      StoreOriginalState(graph, pObject);

      wdUInt32 uiUndoHistorySize = doc.GetCommandHistory()->GetUndoStackSize();

      pAccessor->StartTransaction("SetValue");
      WD_TEST_STATUS(pAccessor->SetValue(pObject, szProperty, value));
      pAccessor->FinishTransaction();
      WD_TEST_INT(doc.GetCommandHistory()->GetUndoStackSize(), uiUndoHistorySize + 1);
      WD_TEST_INT(doc.GetCommandHistory()->GetRedoStackSize(), 0);
      wdVariant newValue;
      WD_TEST_STATUS(pAccessor->GetValue(pObject, szProperty, newValue));
      WD_TEST_BOOL(newValue == value);

      WD_TEST_STATUS(doc.GetCommandHistory()->Undo());
      WD_TEST_INT(doc.GetCommandHistory()->GetUndoStackSize(), uiUndoHistorySize);
      WD_TEST_INT(doc.GetCommandHistory()->GetRedoStackSize(), 1);
      CompareAgainstOriginalState(graph, pObject);

      WD_TEST_STATUS(doc.GetCommandHistory()->Redo());
      WD_TEST_INT(doc.GetCommandHistory()->GetUndoStackSize(), uiUndoHistorySize + 1);
      WD_TEST_INT(doc.GetCommandHistory()->GetRedoStackSize(), 0);
      WD_TEST_STATUS(pAccessor->GetValue(pObject, szProperty, newValue));
      WD_TEST_BOOL(newValue == value);
    };

    // Math
    TestSetValue(pMath, "Vec2", wdVec2(1, 2));
    TestSetValue(pMath, "Vec3", wdVec3(1, 2, 3));
    TestSetValue(pMath, "Vec4", wdVec4(1, 2, 3, 4));
    TestSetValue(pMath, "Vec2I", wdVec2I32(1, 2));
    TestSetValue(pMath, "Vec3I", wdVec3I32(1, 2, 3));
    TestSetValue(pMath, "Vec4I", wdVec4I32(1, 2, 3, 4));
    wdQuat qValue;
    qValue.SetFromEulerAngles(wdAngle::Degree(30), wdAngle::Degree(30), wdAngle::Degree(30));
    TestSetValue(pMath, "Quat", qValue);
    wdMat3 mValue;
    mValue.SetRotationMatrixX(wdAngle::Degree(30));
    TestSetValue(pMath, "Mat3", mValue);
    wdMat4 mValue2;
    mValue2.SetIdentity();
    mValue2.SetRotationMatrixX(wdAngle::Degree(30));
    TestSetValue(pMath, "Mat4", mValue2);

    // Integer
    const wdDocumentObject* pInteger = CreateObject(wdGetStaticRTTI<wdIntegerStruct>());
    TestSetValue(pInteger, "Int8", wdInt8(-5));
    TestSetValue(pInteger, "UInt8", wdUInt8(5));
    TestSetValue(pInteger, "Int16", wdInt16(-5));
    TestSetValue(pInteger, "UInt16", wdUInt16(5));
    TestSetValue(pInteger, "Int32", wdInt32(-5));
    TestSetValue(pInteger, "UInt32", wdUInt32(5));
    TestSetValue(pInteger, "Int64", wdInt64(-5));
    TestSetValue(pInteger, "UInt64", wdUInt64(5));

    // Test automatic type conversions
    TestSetValue(pInteger, "Int8", wdInt16(-5));
    TestSetValue(pInteger, "Int8", wdInt32(-5));
    TestSetValue(pInteger, "Int8", wdInt64(-5));
    TestSetValue(pInteger, "Int8", float(-5));
    TestSetValue(pInteger, "Int8", wdUInt8(5));

    TestSetValue(pInteger, "Int64", wdInt32(-5));
    TestSetValue(pInteger, "Int64", wdInt16(-5));
    TestSetValue(pInteger, "Int64", wdInt8(-5));
    TestSetValue(pInteger, "Int64", float(-5));
    TestSetValue(pInteger, "Int64", wdUInt8(5));

    TestSetValue(pInteger, "UInt64", wdUInt32(5));
    TestSetValue(pInteger, "UInt64", wdUInt16(5));
    TestSetValue(pInteger, "UInt64", wdUInt8(5));
    TestSetValue(pInteger, "UInt64", float(5));
    TestSetValue(pInteger, "UInt64", wdInt8(5));

    // Float
    const wdDocumentObject* pFloat = CreateObject(wdGetStaticRTTI<wdFloatStruct>());
    TestSetValue(pFloat, "Float", -5.0f);
    TestSetValue(pFloat, "Double", -5.0);
    TestSetValue(pFloat, "Time", wdTime::Minutes(3.0f));
    TestSetValue(pFloat, "Angle", wdAngle::Degree(45.0f));

    TestSetValue(pFloat, "Float", 5.0);
    TestSetValue(pFloat, "Float", wdInt8(-5));
    TestSetValue(pFloat, "Float", wdUInt8(5));

    // Misc PODs
    const wdDocumentObject* pPOD = CreateObject(wdGetStaticRTTI<wdPODClass>());
    TestSetValue(pPOD, "Bool", true);
    TestSetValue(pPOD, "Bool", false);
    TestSetValue(pPOD, "Color", wdColor(1.0f, 2.0f, 3.0f, 4.0f));
    TestSetValue(pPOD, "ColorUB", wdColorGammaUB(200, 100, 255));
    TestSetValue(pPOD, "String", "Test");
    wdVarianceTypeAngle customFloat;
    customFloat.m_Value = wdAngle::Degree(45.0f);
    customFloat.m_fVariance = 1.0f;
    TestSetValue(pPOD, "VarianceAngle", customFloat);

    // Enumerations
    const wdDocumentObject* pEnum = CreateObject(wdGetStaticRTTI<wdEnumerationsClass>());
    TestSetValue(pEnum, "Enum", (wdInt8)wdExampleEnum::Value2);
    TestSetValue(pEnum, "Enum", (wdInt64)wdExampleEnum::Value2);
    TestSetValue(pEnum, "Bitflags", (wdUInt8)wdExampleBitflags::Value2);
    TestSetValue(pEnum, "Bitflags", (wdInt64)wdExampleBitflags::Value2);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "InsertValue")
  {
    auto TestInsertValue = [&](const wdDocumentObject* pObject, const char* szProperty, wdVariant value, wdVariant index) {
      wdAbstractObjectGraph graph;
      StoreOriginalState(graph, pObject);

      const wdUInt32 uiUndoHistorySize = doc.GetCommandHistory()->GetUndoStackSize();
      const wdInt32 iArraySize = pAccessor->GetCount(pObject, szProperty);

      pAccessor->StartTransaction("InsertValue");
      WD_TEST_STATUS(pAccessor->InsertValue(pObject, szProperty, value, index));
      pAccessor->FinishTransaction();
      WD_TEST_INT(doc.GetCommandHistory()->GetUndoStackSize(), uiUndoHistorySize + 1);
      WD_TEST_INT(doc.GetCommandHistory()->GetRedoStackSize(), 0);
      WD_TEST_INT(pAccessor->GetCount(pObject, szProperty), iArraySize + 1);
      wdVariant newValue;
      WD_TEST_STATUS(pAccessor->GetValue(pObject, szProperty, newValue, index));
      WD_TEST_BOOL(newValue == value);

      WD_TEST_STATUS(doc.GetCommandHistory()->Undo());
      WD_TEST_INT(doc.GetCommandHistory()->GetUndoStackSize(), uiUndoHistorySize);
      WD_TEST_INT(doc.GetCommandHistory()->GetRedoStackSize(), 1);
      WD_TEST_INT(pAccessor->GetCount(pObject, szProperty), iArraySize);
      CompareAgainstOriginalState(graph, pObject);

      WD_TEST_STATUS(doc.GetCommandHistory()->Redo());
      WD_TEST_INT(doc.GetCommandHistory()->GetUndoStackSize(), uiUndoHistorySize + 1);
      WD_TEST_INT(doc.GetCommandHistory()->GetRedoStackSize(), 0);
      WD_TEST_INT(pAccessor->GetCount(pObject, szProperty), iArraySize + 1);
      WD_TEST_STATUS(pAccessor->GetValue(pObject, szProperty, newValue, index));
      WD_TEST_BOOL(newValue == value);
    };

    TestInsertValue(pObjectTest, "StandardTypeArray", double(0), 0);
    TestInsertValue(pObjectTest, "StandardTypeArray", double(2), 1);
    TestInsertValue(pObjectTest, "StandardTypeArray", double(1), 1);

    TestInsertValue(pObjectTest, "StandardTypeSet", "A", 0);
    TestInsertValue(pObjectTest, "StandardTypeSet", "C", 1);
    TestInsertValue(pObjectTest, "StandardTypeSet", "B", 1);

    TestInsertValue(pObjectTest, "StandardTypeMap", double(0), "A");
    TestInsertValue(pObjectTest, "StandardTypeMap", double(2), "C");
    TestInsertValue(pObjectTest, "StandardTypeMap", double(1), "B");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "MoveValue")
  {
    auto TestMoveValue = [&](const wdDocumentObject* pObject, const char* szProperty, wdVariant oldIndex, wdVariant newIndex, wdArrayPtr<wdVariant> expectedOutcome) {
      wdAbstractObjectGraph graph;
      StoreOriginalState(graph, pObject);

      const wdUInt32 uiUndoHistorySize = doc.GetCommandHistory()->GetUndoStackSize();
      const wdInt32 iArraySize = pAccessor->GetCount(pObject, szProperty);
      WD_TEST_INT(iArraySize, expectedOutcome.GetCount());

      wdDynamicArray<wdVariant> values;
      WD_TEST_STATUS(pAccessor->GetValues(pObject, szProperty, values));
      WD_TEST_INT(iArraySize, values.GetCount());

      pAccessor->StartTransaction("MoveValue");
      WD_TEST_STATUS(pAccessor->MoveValue(pObject, szProperty, oldIndex, newIndex));
      pAccessor->FinishTransaction();
      WD_TEST_INT(doc.GetCommandHistory()->GetUndoStackSize(), uiUndoHistorySize + 1);
      WD_TEST_INT(doc.GetCommandHistory()->GetRedoStackSize(), 0);
      WD_TEST_INT(pAccessor->GetCount(pObject, szProperty), iArraySize);

      for (wdInt32 i = 0; i < iArraySize; i++)
      {
        wdVariant newValue;
        WD_TEST_STATUS(pAccessor->GetValue(pObject, szProperty, newValue, i));
        WD_TEST_BOOL(newValue == expectedOutcome[i]);
      }

      WD_TEST_STATUS(doc.GetCommandHistory()->Undo());
      WD_TEST_INT(doc.GetCommandHistory()->GetUndoStackSize(), uiUndoHistorySize);
      WD_TEST_INT(doc.GetCommandHistory()->GetRedoStackSize(), 1);
      WD_TEST_INT(pAccessor->GetCount(pObject, szProperty), iArraySize);
      CompareAgainstOriginalState(graph, pObject);

      WD_TEST_STATUS(doc.GetCommandHistory()->Redo());
      WD_TEST_INT(doc.GetCommandHistory()->GetUndoStackSize(), uiUndoHistorySize + 1);
      WD_TEST_INT(doc.GetCommandHistory()->GetRedoStackSize(), 0);
      WD_TEST_INT(pAccessor->GetCount(pObject, szProperty), iArraySize);

      for (wdInt32 i = 0; i < iArraySize; i++)
      {
        wdVariant newValue;
        WD_TEST_STATUS(pAccessor->GetValue(pObject, szProperty, newValue, i));
        WD_TEST_BOOL(newValue == expectedOutcome[i]);
      }
    };

    {
      wdVariant expectedValues[3] = {0, 1, 2};
      // Move first element before or after itself (no-op)
      TestMoveValue(pObjectTest, "StandardTypeArray", 0, 0, wdArrayPtr<wdVariant>(expectedValues));
      TestMoveValue(pObjectTest, "StandardTypeArray", 0, 1, wdArrayPtr<wdVariant>(expectedValues));
      // Move last element before or after itself (no-op)
      TestMoveValue(pObjectTest, "StandardTypeArray", 2, 2, wdArrayPtr<wdVariant>(expectedValues));
      TestMoveValue(pObjectTest, "StandardTypeArray", 2, 3, wdArrayPtr<wdVariant>(expectedValues));
    }
    {
      // Move first element to the end.
      wdVariant expectedValues[3] = {1, 2, 0};
      TestMoveValue(pObjectTest, "StandardTypeArray", 0, 3, wdArrayPtr<wdVariant>(expectedValues));
    }
    {
      // Move last element to the front.
      wdVariant expectedValues[3] = {0, 1, 2};
      TestMoveValue(pObjectTest, "StandardTypeArray", 2, 0, wdArrayPtr<wdVariant>(expectedValues));
    }
    {
      // Move first element to the middle
      wdVariant expectedValues[3] = {1, 0, 2};
      TestMoveValue(pObjectTest, "StandardTypeArray", 0, 2, wdArrayPtr<wdVariant>(expectedValues));
    }
    {
      // Move last element to the middle
      wdVariant expectedValues[3] = {1, 2, 0};
      TestMoveValue(pObjectTest, "StandardTypeArray", 2, 1, wdArrayPtr<wdVariant>(expectedValues));
    }

    {
      wdVariant expectedValues[3] = {"A", "B", "C"};
      // Move first element before or after itself (no-op)
      TestMoveValue(pObjectTest, "StandardTypeSet", 0, 0, wdArrayPtr<wdVariant>(expectedValues));
      TestMoveValue(pObjectTest, "StandardTypeSet", 0, 1, wdArrayPtr<wdVariant>(expectedValues));
      // Move last element before or after itself (no-op)
      TestMoveValue(pObjectTest, "StandardTypeSet", 2, 2, wdArrayPtr<wdVariant>(expectedValues));
      TestMoveValue(pObjectTest, "StandardTypeSet", 2, 3, wdArrayPtr<wdVariant>(expectedValues));
    }
    {
      // Move first element to the end.
      wdVariant expectedValues[3] = {"B", "C", "A"};
      TestMoveValue(pObjectTest, "StandardTypeSet", 0, 3, wdArrayPtr<wdVariant>(expectedValues));
    }
    {
      // Move last element to the front.
      wdVariant expectedValues[3] = {"A", "B", "C"};
      TestMoveValue(pObjectTest, "StandardTypeSet", 2, 0, wdArrayPtr<wdVariant>(expectedValues));
    }
    {
      // Move first element to the middle
      wdVariant expectedValues[3] = {"B", "A", "C"};
      TestMoveValue(pObjectTest, "StandardTypeSet", 0, 2, wdArrayPtr<wdVariant>(expectedValues));
    }
    {
      // Move last element to the middle
      wdVariant expectedValues[3] = {"B", "C", "A"};
      TestMoveValue(pObjectTest, "StandardTypeSet", 2, 1, wdArrayPtr<wdVariant>(expectedValues));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "RemoveValue")
  {
    auto TestRemoveValue = [&](const wdDocumentObject* pObject, const char* szProperty, wdVariant index, wdArrayPtr<wdVariant> expectedOutcome) {
      wdAbstractObjectGraph graph;
      StoreOriginalState(graph, pObject);

      const wdUInt32 uiUndoHistorySize = doc.GetCommandHistory()->GetUndoStackSize();
      const wdInt32 iArraySize = pAccessor->GetCount(pObject, szProperty);
      WD_TEST_INT(iArraySize - 1, expectedOutcome.GetCount());

      wdDynamicArray<wdVariant> values;
      wdDynamicArray<wdVariant> keys;
      {
        WD_TEST_STATUS(pAccessor->GetValues(pObject, szProperty, values));
        WD_TEST_INT(iArraySize, values.GetCount());

        WD_TEST_STATUS(pAccessor->GetKeys(pObject, szProperty, keys));
        WD_TEST_INT(iArraySize, keys.GetCount());
        wdUInt32 uiIndex = keys.IndexOf(index);
        keys.RemoveAtAndSwap(uiIndex);
        values.RemoveAtAndSwap(uiIndex);
        WD_TEST_INT(iArraySize - 1, keys.GetCount());
      }

      pAccessor->StartTransaction("RemoveValue");
      WD_TEST_STATUS(pAccessor->RemoveValue(pObject, szProperty, index));
      pAccessor->FinishTransaction();
      WD_TEST_INT(doc.GetCommandHistory()->GetUndoStackSize(), uiUndoHistorySize + 1);
      WD_TEST_INT(doc.GetCommandHistory()->GetRedoStackSize(), 0);
      WD_TEST_INT(pAccessor->GetCount(pObject, szProperty), iArraySize - 1);

      if (pObject->GetType()->FindPropertyByName(szProperty)->GetCategory() == wdPropertyCategory::Map)
      {
        for (wdInt32 i = 0; i < iArraySize - 1; i++)
        {
          const wdVariant& key = keys[i];
          const wdVariant& value = values[i];
          wdVariant newValue;
          WD_TEST_STATUS(pAccessor->GetValue(pObject, szProperty, newValue, key));
          WD_TEST_BOOL(newValue == value);
          WD_TEST_BOOL(expectedOutcome.Contains(newValue));
        }
      }
      else
      {
        for (wdInt32 i = 0; i < iArraySize - 1; i++)
        {
          wdVariant newValue;
          WD_TEST_STATUS(pAccessor->GetValue(pObject, szProperty, newValue, i));
          WD_TEST_BOOL(newValue == expectedOutcome[i]);
        }
      }

      WD_TEST_STATUS(doc.GetCommandHistory()->Undo());
      WD_TEST_INT(doc.GetCommandHistory()->GetUndoStackSize(), uiUndoHistorySize);
      WD_TEST_INT(doc.GetCommandHistory()->GetRedoStackSize(), 1);
      WD_TEST_INT(pAccessor->GetCount(pObject, szProperty), iArraySize);
      CompareAgainstOriginalState(graph, pObject);

      WD_TEST_STATUS(doc.GetCommandHistory()->Redo());
      WD_TEST_INT(doc.GetCommandHistory()->GetUndoStackSize(), uiUndoHistorySize + 1);
      WD_TEST_INT(doc.GetCommandHistory()->GetRedoStackSize(), 0);
      WD_TEST_INT(pAccessor->GetCount(pObject, szProperty), iArraySize - 1);

      if (pObject->GetType()->FindPropertyByName(szProperty)->GetCategory() == wdPropertyCategory::Map)
      {
        for (wdInt32 i = 0; i < iArraySize - 1; i++)
        {
          const wdVariant& key = keys[i];
          const wdVariant& value = values[i];
          wdVariant newValue;
          WD_TEST_STATUS(pAccessor->GetValue(pObject, szProperty, newValue, key));
          WD_TEST_BOOL(newValue == value);
          WD_TEST_BOOL(expectedOutcome.Contains(newValue));
        }
      }
      else
      {
        for (wdInt32 i = 0; i < iArraySize - 1; i++)
        {
          wdVariant newValue;
          WD_TEST_STATUS(pAccessor->GetValue(pObject, szProperty, newValue, i));
          WD_TEST_BOOL(newValue == expectedOutcome[i]);
        }
      }
    };

    // StandardTypeArray
    {
      wdVariant expectedValues[2] = {2, 0};
      TestRemoveValue(pObjectTest, "StandardTypeArray", 0, wdArrayPtr<wdVariant>(expectedValues));
    }
    {
      wdVariant expectedValues[1] = {2};
      TestRemoveValue(pObjectTest, "StandardTypeArray", 1, wdArrayPtr<wdVariant>(expectedValues));
    }
    {
      TestRemoveValue(pObjectTest, "StandardTypeArray", 0, wdArrayPtr<wdVariant>());
    }
    // StandardTypeSet
    {
      wdVariant expectedValues[2] = {"B", "C"};
      TestRemoveValue(pObjectTest, "StandardTypeSet", 2, wdArrayPtr<wdVariant>(expectedValues));
    }
    {
      wdVariant expectedValues[1] = {"C"};
      TestRemoveValue(pObjectTest, "StandardTypeSet", 0, wdArrayPtr<wdVariant>(expectedValues));
    }
    {
      TestRemoveValue(pObjectTest, "StandardTypeSet", 0, wdArrayPtr<wdVariant>());
    }
    // StandardTypeMap
    {
      wdVariant expectedValues[2] = {1, 2};
      TestRemoveValue(pObjectTest, "StandardTypeMap", "A", wdArrayPtr<wdVariant>(expectedValues));
    }
    {
      wdVariant expectedValues[1] = {1};
      TestRemoveValue(pObjectTest, "StandardTypeMap", "C", wdArrayPtr<wdVariant>(expectedValues));
    }
    {
      TestRemoveValue(pObjectTest, "StandardTypeMap", "B", wdArrayPtr<wdVariant>());
    }
  }

  auto CreateGuid = [](const char* szType, wdInt32 iIndex) -> wdUuid {
    wdUuid A = wdUuid::StableUuidForString(szType);
    wdUuid B = wdUuid::StableUuidForInt(iIndex);
    A.CombineWithSeed(B);
    return A;
  };

  WD_TEST_BLOCK(wdTestBlock::Enabled, "AddObject")
  {
    auto TestAddObject = [&](const wdDocumentObject* pObject, const char* szProperty, wdVariant index, const wdRTTI* pType, wdUuid& inout_object) {
      wdAbstractObjectGraph graph;
      StoreOriginalState(graph, pObject);

      const wdUInt32 uiUndoHistorySize = doc.GetCommandHistory()->GetUndoStackSize();
      const wdInt32 iArraySize = pAccessor->GetCount(pObject, szProperty);

      pAccessor->StartTransaction("TestAddObject");
      WD_TEST_STATUS(pAccessor->AddObject(pObject, szProperty, index, pType, inout_object));
      pAccessor->FinishTransaction();
      WD_TEST_INT(doc.GetCommandHistory()->GetUndoStackSize(), uiUndoHistorySize + 1);
      WD_TEST_INT(doc.GetCommandHistory()->GetRedoStackSize(), 0);
      WD_TEST_INT(pAccessor->GetCount(pObject, szProperty), iArraySize + 1);
      wdVariant newValue;
      WD_TEST_STATUS(pAccessor->GetValue(pObject, szProperty, newValue, index));
      WD_TEST_BOOL(newValue == inout_object);

      WD_TEST_STATUS(doc.GetCommandHistory()->Undo());
      WD_TEST_INT(doc.GetCommandHistory()->GetUndoStackSize(), uiUndoHistorySize);
      WD_TEST_INT(doc.GetCommandHistory()->GetRedoStackSize(), 1);
      WD_TEST_INT(pAccessor->GetCount(pObject, szProperty), iArraySize);
      CompareAgainstOriginalState(graph, pObject);

      WD_TEST_STATUS(doc.GetCommandHistory()->Redo());
      WD_TEST_INT(doc.GetCommandHistory()->GetUndoStackSize(), uiUndoHistorySize + 1);
      WD_TEST_INT(doc.GetCommandHistory()->GetRedoStackSize(), 0);
      WD_TEST_INT(pAccessor->GetCount(pObject, szProperty), iArraySize + 1);
      WD_TEST_STATUS(pAccessor->GetValue(pObject, szProperty, newValue, index));
      WD_TEST_BOOL(newValue == inout_object);
    };

    wdUuid A = CreateGuid("ClassArray", 0);
    wdUuid B = CreateGuid("ClassArray", 1);
    wdUuid C = CreateGuid("ClassArray", 2);

    TestAddObject(pObjectTest, "ClassArray", 0, wdGetStaticRTTI<OuterClass>(), A);
    TestAddObject(pObjectTest, "ClassArray", 1, wdGetStaticRTTI<OuterClass>(), C);
    TestAddObject(pObjectTest, "ClassArray", 1, wdGetStaticRTTI<OuterClass>(), B);

    A = CreateGuid("ClassPtrArray", 0);
    B = CreateGuid("ClassPtrArray", 1);
    C = CreateGuid("ClassPtrArray", 2);

    TestAddObject(pObjectTest, "ClassPtrArray", 0, wdGetStaticRTTI<OuterClass>(), A);
    TestAddObject(pObjectTest, "ClassPtrArray", 1, wdGetStaticRTTI<OuterClass>(), C);
    TestAddObject(pObjectTest, "ClassPtrArray", 1, wdGetStaticRTTI<OuterClass>(), B);

    A = CreateGuid("SubObjectSet", 0);
    B = CreateGuid("SubObjectSet", 1);
    C = CreateGuid("SubObjectSet", 2);

    TestAddObject(pObjectTest, "SubObjectSet", 0, wdGetStaticRTTI<wdObjectTest>(), A);
    TestAddObject(pObjectTest, "SubObjectSet", 1, wdGetStaticRTTI<wdObjectTest>(), C);
    TestAddObject(pObjectTest, "SubObjectSet", 1, wdGetStaticRTTI<wdObjectTest>(), B);

    A = CreateGuid("ClassMap", 0);
    B = CreateGuid("ClassMap", 1);
    C = CreateGuid("ClassMap", 2);

    TestAddObject(pObjectTest, "ClassMap", "A", wdGetStaticRTTI<OuterClass>(), A);
    TestAddObject(pObjectTest, "ClassMap", "C", wdGetStaticRTTI<OuterClass>(), C);
    TestAddObject(pObjectTest, "ClassMap", "B", wdGetStaticRTTI<OuterClass>(), B);

    A = CreateGuid("ClassPtrMap", 0);
    B = CreateGuid("ClassPtrMap", 1);
    C = CreateGuid("ClassPtrMap", 2);

    TestAddObject(pObjectTest, "ClassPtrMap", "A", wdGetStaticRTTI<OuterClass>(), A);
    TestAddObject(pObjectTest, "ClassPtrMap", "C", wdGetStaticRTTI<OuterClass>(), C);
    TestAddObject(pObjectTest, "ClassPtrMap", "B", wdGetStaticRTTI<OuterClass>(), B);
  }


  WD_TEST_BLOCK(wdTestBlock::Enabled, "MoveObject")
  {
    auto TestMoveObjectFailure = [&](const wdDocumentObject* pObject, const char* szProperty, wdVariant newIndex) {
      pAccessor->StartTransaction("MoveObject");
      WD_TEST_BOOL(pAccessor->MoveObject(pObject, pObject->GetParent(), szProperty, newIndex).Failed());
      pAccessor->CancelTransaction();
    };

    auto TestMoveObject = [&](const wdDocumentObject* pObject, const char* szProperty, wdVariant newIndex, wdArrayPtr<wdUuid> expectedOutcome) {
      wdAbstractObjectGraph graph;
      StoreOriginalState(graph, pObject->GetParent());

      const wdUInt32 uiUndoHistorySize = doc.GetCommandHistory()->GetUndoStackSize();
      const wdInt32 iArraySize = pAccessor->GetCount(pObject->GetParent(), szProperty);
      WD_TEST_INT(iArraySize, expectedOutcome.GetCount());

      wdDynamicArray<wdVariant> values;
      WD_TEST_STATUS(pAccessor->GetValues(pObject->GetParent(), szProperty, values));
      WD_TEST_INT(iArraySize, values.GetCount());

      pAccessor->StartTransaction("MoveObject");
      WD_TEST_STATUS(pAccessor->MoveObject(pObject, pObject->GetParent(), szProperty, newIndex));
      pAccessor->FinishTransaction();
      WD_TEST_INT(doc.GetCommandHistory()->GetUndoStackSize(), uiUndoHistorySize + 1);
      WD_TEST_INT(doc.GetCommandHistory()->GetRedoStackSize(), 0);
      WD_TEST_INT(pAccessor->GetCount(pObject->GetParent(), szProperty), iArraySize);

      for (wdInt32 i = 0; i < iArraySize; i++)
      {
        wdVariant newValue;
        WD_TEST_STATUS(pAccessor->GetValue(pObject->GetParent(), szProperty, newValue, i));
        WD_TEST_BOOL(newValue == expectedOutcome[i]);
      }

      WD_TEST_STATUS(doc.GetCommandHistory()->Undo());
      WD_TEST_INT(doc.GetCommandHistory()->GetUndoStackSize(), uiUndoHistorySize);
      WD_TEST_INT(doc.GetCommandHistory()->GetRedoStackSize(), 1);
      WD_TEST_INT(pAccessor->GetCount(pObject->GetParent(), szProperty), iArraySize);
      CompareAgainstOriginalState(graph, pObject->GetParent());

      WD_TEST_STATUS(doc.GetCommandHistory()->Redo());
      WD_TEST_INT(doc.GetCommandHistory()->GetUndoStackSize(), uiUndoHistorySize + 1);
      WD_TEST_INT(doc.GetCommandHistory()->GetRedoStackSize(), 0);
      WD_TEST_INT(pAccessor->GetCount(pObject->GetParent(), szProperty), iArraySize);

      for (wdInt32 i = 0; i < iArraySize; i++)
      {
        wdVariant newValue;
        WD_TEST_STATUS(pAccessor->GetValue(pObject->GetParent(), szProperty, newValue, i));
        WD_TEST_BOOL(newValue == expectedOutcome[i]);
      }
    };

    wdUuid A = CreateGuid("ClassArray", 0);
    wdUuid B = CreateGuid("ClassArray", 1);
    wdUuid C = CreateGuid("ClassArray", 2);
    const wdDocumentObject* pA = pAccessor->GetObject(A);
    const wdDocumentObject* pB = pAccessor->GetObject(B);
    const wdDocumentObject* pC = pAccessor->GetObject(C);

    {
      // Move first element before or after itself (no-op)
      TestMoveObjectFailure(pA, "ClassArray", 0);
      TestMoveObjectFailure(pA, "ClassArray", 1);
      // Move last element before or after itself (no-op)
      TestMoveObjectFailure(pC, "ClassArray", 2);
      TestMoveObjectFailure(pC, "ClassArray", 3);
    }
    {
      // Move first element to the end.
      wdUuid expectedValues[3] = {B, C, A};
      TestMoveObject(pA, "ClassArray", 3, wdArrayPtr<wdUuid>(expectedValues));
    }
    {
      // Move last element to the front.
      wdUuid expectedValues[3] = {A, B, C};
      TestMoveObject(pA, "ClassArray", 0, wdArrayPtr<wdUuid>(expectedValues));
    }
    {
      // Move first element to the middle
      wdUuid expectedValues[3] = {B, A, C};
      TestMoveObject(pA, "ClassArray", 2, wdArrayPtr<wdUuid>(expectedValues));
    }
    {
      // Move last element to the middle
      wdUuid expectedValues[3] = {B, C, A};
      TestMoveObject(pC, "ClassArray", 1, wdArrayPtr<wdUuid>(expectedValues));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "RemoveObject")
  {
    auto TestRemoveObject = [&](const wdDocumentObject* pObject, wdArrayPtr<wdUuid> expectedOutcome) {
      auto pParent = pObject->GetParent();
      wdString sProperty = pObject->GetParentProperty();

      wdAbstractObjectGraph graph;
      StoreOriginalState(graph, pParent);
      const wdUInt32 uiUndoHistorySize = doc.GetCommandHistory()->GetUndoStackSize();
      const wdInt32 iArraySize = pAccessor->GetCount(pParent, sProperty);
      WD_TEST_INT(iArraySize - 1, expectedOutcome.GetCount());


      wdDynamicArray<wdVariant> values;
      wdDynamicArray<wdVariant> keys;
      {
        WD_TEST_STATUS(pAccessor->GetValues(pParent, sProperty, values));
        WD_TEST_INT(iArraySize, values.GetCount());

        WD_TEST_STATUS(pAccessor->GetKeys(pParent, sProperty, keys));
        WD_TEST_INT(iArraySize, keys.GetCount());
        wdUInt32 uiIndex = keys.IndexOf(pObject->GetPropertyIndex());
        keys.RemoveAtAndSwap(uiIndex);
        values.RemoveAtAndSwap(uiIndex);
        WD_TEST_INT(iArraySize - 1, keys.GetCount());
      }

      pAccessor->StartTransaction("RemoveValue");
      WD_TEST_STATUS(pAccessor->RemoveObject(pObject));
      pAccessor->FinishTransaction();
      WD_TEST_INT(doc.GetCommandHistory()->GetUndoStackSize(), uiUndoHistorySize + 1);
      WD_TEST_INT(doc.GetCommandHistory()->GetRedoStackSize(), 0);
      WD_TEST_INT(pAccessor->GetCount(pParent, sProperty), iArraySize - 1);

      if (pParent->GetType()->FindPropertyByName(sProperty)->GetCategory() == wdPropertyCategory::Map)
      {
        for (wdInt32 i = 0; i < iArraySize - 1; i++)
        {
          const wdVariant& key = keys[i];
          const wdVariant& value = values[i];
          wdVariant newValue;
          WD_TEST_STATUS(pAccessor->GetValue(pParent, sProperty, newValue, key));
          WD_TEST_BOOL(newValue == value);
          WD_TEST_BOOL(expectedOutcome.Contains(newValue.Get<wdUuid>()));
        }
      }
      else
      {
        for (wdInt32 i = 0; i < iArraySize - 1; i++)
        {
          wdVariant newValue;
          WD_TEST_STATUS(pAccessor->GetValue(pParent, sProperty, newValue, i));
          WD_TEST_BOOL(newValue == expectedOutcome[i]);
        }
      }

      WD_TEST_STATUS(doc.GetCommandHistory()->Undo());
      WD_TEST_INT(doc.GetCommandHistory()->GetUndoStackSize(), uiUndoHistorySize);
      WD_TEST_INT(doc.GetCommandHistory()->GetRedoStackSize(), 1);
      WD_TEST_INT(pAccessor->GetCount(pParent, sProperty), iArraySize);
      CompareAgainstOriginalState(graph, pParent);

      WD_TEST_STATUS(doc.GetCommandHistory()->Redo());
      WD_TEST_INT(doc.GetCommandHistory()->GetUndoStackSize(), uiUndoHistorySize + 1);
      WD_TEST_INT(doc.GetCommandHistory()->GetRedoStackSize(), 0);
      WD_TEST_INT(pAccessor->GetCount(pParent, sProperty), iArraySize - 1);

      if (pParent->GetType()->FindPropertyByName(sProperty)->GetCategory() == wdPropertyCategory::Map)
      {
        for (wdInt32 i = 0; i < iArraySize - 1; i++)
        {
          const wdVariant& key = keys[i];
          const wdVariant& value = values[i];
          wdVariant newValue;
          WD_TEST_STATUS(pAccessor->GetValue(pParent, sProperty, newValue, key));
          WD_TEST_BOOL(newValue == value);
          WD_TEST_BOOL(expectedOutcome.Contains(newValue.Get<wdUuid>()));
        }
      }
      else
      {
        for (wdInt32 i = 0; i < iArraySize - 1; i++)
        {
          wdVariant newValue;
          WD_TEST_STATUS(pAccessor->GetValue(pParent, sProperty, newValue, i));
          WD_TEST_BOOL(newValue == expectedOutcome[i]);
        }
      }
    };

    auto ClearContainer = [&](const char* szContainer) {
      wdUuid A = CreateGuid(szContainer, 0);
      wdUuid B = CreateGuid(szContainer, 1);
      wdUuid C = CreateGuid(szContainer, 2);
      const wdDocumentObject* pA = pAccessor->GetObject(A);
      const wdDocumentObject* pB = pAccessor->GetObject(B);
      const wdDocumentObject* pC = pAccessor->GetObject(C);
      {
        wdUuid expectedValues[2] = {B, C};
        TestRemoveObject(pA, wdArrayPtr<wdUuid>(expectedValues));
      }
      {
        wdUuid expectedValues[1] = {C};
        TestRemoveObject(pB, wdArrayPtr<wdUuid>(expectedValues));
      }
      {
        TestRemoveObject(pC, wdArrayPtr<wdUuid>());
      }
    };

    ClearContainer("ClassArray");
    ClearContainer("ClassPtrArray");
    ClearContainer("SubObjectSet");
    ClearContainer("ClassMap");
    ClearContainer("ClassPtrMap");
  }
}
