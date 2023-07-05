#include <ToolsFoundationTest/ToolsFoundationTestPCH.h>

#include <ToolsFoundation/Object/ObjectAccessorBase.h>
#include <ToolsFoundationTest/Object/TestObjectManager.h>
#include <ToolsFoundationTest/Reflection/ReflectionTestClasses.h>

void MirrorCheck(wdTestDocument* pDoc, const wdDocumentObject* pObject)
{
  // Create native object graph
  wdAbstractObjectGraph graph;
  wdAbstractObjectNode* pRootNode = nullptr;
  {
    wdRttiConverterWriter rttiConverter(&graph, &pDoc->m_Context, true, true);
    pRootNode = rttiConverter.AddObjectToGraph(pObject->GetType(), pDoc->m_ObjectMirror.GetNativeObjectPointer(pObject), "Object");
  }

  // Create object manager graph
  wdAbstractObjectGraph origGraph;
  wdAbstractObjectNode* pOrigRootNode = nullptr;
  {
    wdDocumentObjectConverterWriter writer(&origGraph, pDoc->GetObjectManager());
    pOrigRootNode = writer.AddObjectToGraph(pObject);
  }

  // Remap native guids so they match the object manager (stuff like embedded classes will not have a guid on the native side).
  graph.ReMapNodeGuidsToMatchGraph(pRootNode, origGraph, pOrigRootNode);
  wdDeque<wdAbstractGraphDiffOperation> diffResult;

  graph.CreateDiffWithBaseGraph(origGraph, diffResult);

  WD_TEST_BOOL(diffResult.GetCount() == 0);
}


wdVariant GetVariantFromType(wdVariant::Type::Enum type)
{
  switch (type)
  {
    case wdVariant::Type::Invalid:
      return wdVariant();
    case wdVariant::Type::Bool:
      return wdVariant(true);
    case wdVariant::Type::Int8:
      return wdVariant((wdInt8)-55);
    case wdVariant::Type::UInt8:
      return wdVariant((wdUInt8)44);
    case wdVariant::Type::Int16:
      return wdVariant((wdInt16)-444);
    case wdVariant::Type::UInt16:
      return wdVariant((wdUInt16)666);
    case wdVariant::Type::Int32:
      return wdVariant((wdInt32)-88880);
    case wdVariant::Type::UInt32:
      return wdVariant((wdUInt32)123445);
    case wdVariant::Type::Int64:
      return wdVariant((wdInt64)-888800000);
    case wdVariant::Type::UInt64:
      return wdVariant((wdUInt64)123445000);
    case wdVariant::Type::Float:
      return wdVariant(1024.0f);
    case wdVariant::Type::Double:
      return wdVariant(-2048.0f);
    case wdVariant::Type::Color:
      return wdVariant(wdColor(0.5f, 33.0f, 2.0f, 0.3f));
    case wdVariant::Type::ColorGamma:
      return wdVariant(wdColorGammaUB(wdColor(0.5f, 33.0f, 2.0f, 0.3f)));
    case wdVariant::Type::Vector2:
      return wdVariant(wdVec2(2.0f, 4.0f));
    case wdVariant::Type::Vector3:
      return wdVariant(wdVec3(2.0f, 4.0f, -8.0f));
    case wdVariant::Type::Vector4:
      return wdVariant(wdVec4(1.0f, 7.0f, 8.0f, -10.0f));
    case wdVariant::Type::Vector2I:
      return wdVariant(wdVec2I32(1, 2));
    case wdVariant::Type::Vector3I:
      return wdVariant(wdVec3I32(3, 4, 5));
    case wdVariant::Type::Vector4I:
      return wdVariant(wdVec4I32(6, 7, 8, 9));
    case wdVariant::Type::Quaternion:
    {
      wdQuat quat;
      quat.SetFromEulerAngles(wdAngle::Degree(30), wdAngle::Degree(-15), wdAngle::Degree(20));
      return wdVariant(quat);
    }
    case wdVariant::Type::Matrix3:
    {
      wdMat3 mat = wdMat3::IdentityMatrix();

      mat.SetRotationMatrix(wdVec3(1.0f, 0.0f, 0.0f), wdAngle::Degree(30));
      return wdVariant(mat);
    }
    case wdVariant::Type::Matrix4:
    {
      wdMat4 mat = wdMat4::IdentityMatrix();

      mat.SetRotationMatrix(wdVec3(0.0f, 1.0f, 0.0f), wdAngle::Degree(30));
      mat.SetTranslationVector(wdVec3(1.0f, 2.0f, 3.0f));
      return wdVariant(mat);
    }
    case wdVariant::Type::String:
      return wdVariant("Test");
    case wdVariant::Type::StringView:
      return wdVariant("Test");
    case wdVariant::Type::Time:
      return wdVariant(wdTime::Seconds(123.0f));
    case wdVariant::Type::Uuid:
    {
      wdUuid guid;
      guid.CreateNewUuid();
      return wdVariant(guid);
    }
    case wdVariant::Type::Angle:
      return wdVariant(wdAngle::Degree(30.0f));
    case wdVariant::Type::DataBuffer:
    {
      wdDataBuffer data;
      data.PushBack(12);
      data.PushBack(55);
      data.PushBack(88);
      return wdVariant(data);
    }
    case wdVariant::Type::VariantArray:
      return wdVariantArray();
    case wdVariant::Type::VariantDictionary:
      return wdVariantDictionary();
    case wdVariant::Type::TypedPointer:
      return wdVariant(wdTypedPointer(nullptr, nullptr));
    case wdVariant::Type::TypedObject:
      WD_ASSERT_NOT_IMPLEMENTED;

    default:
      WD_REPORT_FAILURE("Invalid case statement");
      return wdVariant();
  }
  return wdVariant();
}

void RecursiveModifyProperty(const wdDocumentObject* pObject, const wdAbstractProperty* pProp, wdObjectAccessorBase* pObjectAccessor)
{
  if (pProp->GetCategory() == wdPropertyCategory::Member)
  {
    if (pProp->GetFlags().IsSet(wdPropertyFlags::Pointer))
    {
      if (pProp->GetFlags().IsSet(wdPropertyFlags::PointerOwner))
      {
        const wdUuid oldGuid = pObjectAccessor->Get<wdUuid>(pObject, pProp);
        wdUuid newGuid;
        newGuid.CreateNewUuid();
        if (oldGuid.IsValid())
        {
          WD_TEST_BOOL(pObjectAccessor->RemoveObject(pObjectAccessor->GetObject(oldGuid)).m_Result.Succeeded());
        }

        WD_TEST_BOOL(pObjectAccessor->AddObject(pObject, pProp, wdVariant(), pProp->GetSpecificType(), newGuid).m_Result.Succeeded());

        const wdDocumentObject* pChild = pObject->GetChild(newGuid);
        WD_ASSERT_DEV(pChild != nullptr, "References child object does not exist!");
      }
      else
      {
        wdVariant value = GetVariantFromType(pProp->GetSpecificType()->GetVariantType());
        WD_TEST_BOOL(pObjectAccessor->SetValue(pObject, pProp, value).m_Result.Succeeded());
      }
    }
    else
    {
      if (pProp->GetFlags().IsAnySet(wdPropertyFlags::IsEnum | wdPropertyFlags::Bitflags | wdPropertyFlags::StandardType))
      {
        wdVariant value = GetVariantFromType(pProp->GetSpecificType()->GetVariantType());
        WD_TEST_BOOL(pObjectAccessor->SetValue(pObject, pProp, value).m_Result.Succeeded());
      }
      else if (pProp->GetFlags().IsSet(wdPropertyFlags::Class))
      {
        // Noting to do here, value cannot change
      }
    }
  }
  else if (pProp->GetCategory() == wdPropertyCategory::Array || pProp->GetCategory() == wdPropertyCategory::Set)
  {
    if (pProp->GetFlags().IsAnySet(wdPropertyFlags::StandardType | wdPropertyFlags::Pointer) &&
        !pProp->GetFlags().IsSet(wdPropertyFlags::PointerOwner))
    {
      wdInt32 iCurrentCount = pObjectAccessor->GetCount(pObject, pProp);
      for (wdInt32 i = iCurrentCount - 1; i >= 0; --i)
      {
        pObjectAccessor->RemoveValue(pObject, pProp, i);
      }

      wdVariant value1 = wdReflectionUtils::GetDefaultValue(pProp, 0);
      wdVariant value2 = GetVariantFromType(pProp->GetSpecificType()->GetVariantType());
      WD_TEST_BOOL(pObjectAccessor->InsertValue(pObject, pProp, value1, 0).m_Result.Succeeded());
      WD_TEST_BOOL(pObjectAccessor->InsertValue(pObject, pProp, value2, 1).m_Result.Succeeded());
    }
    else if (pProp->GetFlags().IsSet(wdPropertyFlags::Class))
    {
      wdInt32 iCurrentCount = pObject->GetTypeAccessor().GetCount(pProp->GetPropertyName());
      wdHybridArray<wdVariant, 16> currentValues;
      pObject->GetTypeAccessor().GetValues(pProp->GetPropertyName(), currentValues);
      for (wdInt32 i = iCurrentCount - 1; i >= 0; --i)
      {
        WD_TEST_BOOL(pObjectAccessor->RemoveObject(pObjectAccessor->GetObject(currentValues[i].Get<wdUuid>())).m_Result.Succeeded());
      }

      if (pProp->GetCategory() == wdPropertyCategory::Array)
      {
        wdUuid newGuid;
        newGuid.CreateNewUuid();
        WD_TEST_BOOL(pObjectAccessor->AddObject(pObject, pProp, 0, pProp->GetSpecificType(), newGuid).m_Result.Succeeded());
      }
    }
  }
  else if (pProp->GetCategory() == wdPropertyCategory::Map)
  {
    if (pProp->GetFlags().IsAnySet(wdPropertyFlags::StandardType | wdPropertyFlags::Pointer) &&
        !pProp->GetFlags().IsSet(wdPropertyFlags::PointerOwner))
    {
      wdInt32 iCurrentCount = pObjectAccessor->GetCount(pObject, pProp);
      wdHybridArray<wdVariant, 16> keys;
      pObjectAccessor->GetKeys(pObject, pProp, keys);
      for (const wdVariant& key : keys)
      {
        pObjectAccessor->RemoveValue(pObject, pProp, key);
      }

      wdVariant value1 = wdReflectionUtils::GetDefaultValue(pProp, "Dummy");
      wdVariant value2 = GetVariantFromType(pProp->GetSpecificType()->GetVariantType());
      WD_TEST_BOOL(pObjectAccessor->InsertValue(pObject, pProp, value1, "value1").m_Result.Succeeded());
      WD_TEST_BOOL(pObjectAccessor->InsertValue(pObject, pProp, value2, "value2").m_Result.Succeeded());
    }
    else if (pProp->GetFlags().IsSet(wdPropertyFlags::Class))
    {
      wdInt32 iCurrentCount = pObject->GetTypeAccessor().GetCount(pProp->GetPropertyName());
      wdHybridArray<wdVariant, 16> currentValues;
      pObject->GetTypeAccessor().GetValues(pProp->GetPropertyName(), currentValues);
      for (wdInt32 i = iCurrentCount - 1; i >= 0; --i)
      {
        WD_TEST_BOOL(pObjectAccessor->RemoveObject(pObjectAccessor->GetObject(currentValues[i].Get<wdUuid>())).m_Result.Succeeded());
      }

      wdUuid newGuid;
      newGuid.CreateNewUuid();
      WD_TEST_BOOL(pObjectAccessor->AddObject(pObject, pProp, "value1", pProp->GetSpecificType(), newGuid).m_Result.Succeeded());
    }
  }
}

void RecursiveModifyObject(const wdDocumentObject* pObject, wdObjectAccessorBase* pAccessor)
{
  wdHybridArray<wdAbstractProperty*, 32> Properties;
  pObject->GetTypeAccessor().GetType()->GetAllProperties(Properties);
  for (const auto* pProp : Properties)
  {
    RecursiveModifyProperty(pObject, pProp, pAccessor);
  }

  for (const wdDocumentObject* pSubObject : pObject->GetChildren())
  {
    RecursiveModifyObject(pSubObject, pAccessor);
  }
}

WD_CREATE_SIMPLE_TEST(DocumentObject, ObjectMirror)
{
  wdTestDocument doc("Test", true);
  doc.InitializeAfterLoading(false);
  wdObjectAccessorBase* pAccessor = doc.GetObjectAccessor();
  wdUuid mirrorGuid;

  pAccessor->StartTransaction("Init");
  wdStatus status = pAccessor->AddObject(nullptr, (const wdAbstractProperty*)nullptr, -1, wdGetStaticRTTI<wdMirrorTest>(), mirrorGuid);
  const wdDocumentObject* pObject = pAccessor->GetObject(mirrorGuid);
  WD_TEST_BOOL(status.m_Result.Succeeded());
  pAccessor->FinishTransaction();

  MirrorCheck(&doc, pObject);

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Document Changes")
  {
    pAccessor->StartTransaction("Document Changes");
    RecursiveModifyObject(pObject, pAccessor);
    pAccessor->FinishTransaction();

    MirrorCheck(&doc, pObject);
  }
  {
    pAccessor->StartTransaction("Document Changes");
    RecursiveModifyObject(pObject, pAccessor);
    pAccessor->FinishTransaction();

    MirrorCheck(&doc, pObject);
  }
}
