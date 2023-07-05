#include <ToolsFoundationTest/ToolsFoundationTestPCH.h>

#include <ToolsFoundationTest/Object/TestObjectManager.h>
#include <ToolsFoundationTest/Reflection/ReflectionTestClasses.h>

WD_CREATE_SIMPLE_TEST_GROUP(DocumentObject);

WD_CREATE_SIMPLE_TEST(DocumentObject, DocumentObjectManager)
{
  wdTestDocumentObjectManager manager;
  wdDocumentObject* pObject = nullptr;
  wdDocumentObject* pChildObject = nullptr;
  wdDocumentObject* pChildren[4] = {nullptr};
  wdDocumentObject* pSubElementObject[4] = {nullptr};

  WD_TEST_BLOCK(wdTestBlock::Enabled, "DocumentObject")
  {
    WD_TEST_BOOL(manager.CanAdd(wdObjectTest::GetStaticRTTI(), nullptr, "", 0).m_Result.Succeeded());
    pObject = manager.CreateObject(wdObjectTest::GetStaticRTTI());
    manager.AddObject(pObject, nullptr, "", 0);

    const char* szProperty = "SubObjectSet";
    WD_TEST_BOOL(manager.CanAdd(OuterClass::GetStaticRTTI(), pObject, szProperty, 0).m_Result.Failed());
    WD_TEST_BOOL(manager.CanAdd(wdObjectTest::GetStaticRTTI(), pObject, szProperty, 0).m_Result.Succeeded());
    pChildObject = manager.CreateObject(wdObjectTest::GetStaticRTTI());
    manager.AddObject(pChildObject, pObject, "SubObjectSet", 0);
    WD_TEST_INT(pObject->GetTypeAccessor().GetCount(szProperty), 1);

    WD_TEST_BOOL(manager.CanAdd(OuterClass::GetStaticRTTI(), pObject, "ClassPtrArray", 0).m_Result.Succeeded());
    WD_TEST_BOOL(manager.CanAdd(ExtendedOuterClass::GetStaticRTTI(), pObject, "ClassPtrArray", 0).m_Result.Succeeded());
    WD_TEST_BOOL(!manager.CanAdd(wdReflectedClass::GetStaticRTTI(), pObject, "ClassPtrArray", 0).m_Result.Succeeded());

    for (wdInt32 i = 0; i < WD_ARRAY_SIZE(pChildren); i++)
    {
      WD_TEST_BOOL(manager.CanAdd(wdObjectTest::GetStaticRTTI(), pChildObject, szProperty, i).m_Result.Succeeded());
      pChildren[i] = manager.CreateObject(wdObjectTest::GetStaticRTTI());
      manager.AddObject(pChildren[i], pChildObject, szProperty, i);
      WD_TEST_INT(pChildObject->GetTypeAccessor().GetCount(szProperty), i + 1);
    }
    WD_TEST_INT(pChildObject->GetTypeAccessor().GetCount(szProperty), 4);

    WD_TEST_BOOL_MSG(manager.CanMove(pObject, pChildObject, szProperty, 0).m_Result.Failed(), "Can't move to own child");
    WD_TEST_BOOL_MSG(manager.CanMove(pChildren[1], pChildObject, szProperty, 1).m_Result.Failed(), "Can't move before onself");
    WD_TEST_BOOL_MSG(manager.CanMove(pChildren[1], pChildObject, szProperty, 2).m_Result.Failed(), "Can't move after oneself");
    WD_TEST_BOOL_MSG(manager.CanMove(pChildren[1], pChildren[1], szProperty, 0).m_Result.Failed(), "Can't move into yourself");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "DocumentSubElementObject")
  {
    const char* szProperty = "ClassArray";
    for (wdInt32 i = 0; i < WD_ARRAY_SIZE(pSubElementObject); i++)
    {
      WD_TEST_BOOL(manager.CanAdd(OuterClass::GetStaticRTTI(), pObject, szProperty, i).m_Result.Succeeded());
      pSubElementObject[i] = manager.CreateObject(OuterClass::GetStaticRTTI());
      manager.AddObject(pSubElementObject[i], pObject, szProperty, i);
      WD_TEST_INT(pObject->GetTypeAccessor().GetCount(szProperty), i + 1);
    }

    WD_TEST_BOOL(manager.CanRemove(pSubElementObject[0]).m_Result.Succeeded());
    manager.RemoveObject(pSubElementObject[0]);
    manager.DestroyObject(pSubElementObject[0]);
    pSubElementObject[0] = nullptr;
    WD_TEST_INT(pObject->GetTypeAccessor().GetCount(szProperty), 3);

    wdVariant value = pObject->GetTypeAccessor().GetValue(szProperty, 0);
    WD_TEST_BOOL(value.IsA<wdUuid>() && value.Get<wdUuid>() == pSubElementObject[1]->GetGuid());
    value = pObject->GetTypeAccessor().GetValue(szProperty, 1);
    WD_TEST_BOOL(value.IsA<wdUuid>() && value.Get<wdUuid>() == pSubElementObject[2]->GetGuid());
    value = pObject->GetTypeAccessor().GetValue(szProperty, 2);
    WD_TEST_BOOL(value.IsA<wdUuid>() && value.Get<wdUuid>() == pSubElementObject[3]->GetGuid());

    WD_TEST_BOOL(manager.CanMove(pSubElementObject[1], pObject, szProperty, 2).m_Result.Succeeded());
    manager.MoveObject(pSubElementObject[1], pObject, szProperty, 2);
    WD_TEST_BOOL(manager.CanMove(pSubElementObject[3], pObject, szProperty, 0).m_Result.Succeeded());
    manager.MoveObject(pSubElementObject[3], pObject, szProperty, 0);

    value = pObject->GetTypeAccessor().GetValue(szProperty, 0);
    WD_TEST_BOOL(value.IsA<wdUuid>() && value.Get<wdUuid>() == pSubElementObject[3]->GetGuid());
    value = pObject->GetTypeAccessor().GetValue(szProperty, 1);
    WD_TEST_BOOL(value.IsA<wdUuid>() && value.Get<wdUuid>() == pSubElementObject[2]->GetGuid());
    value = pObject->GetTypeAccessor().GetValue(szProperty, 2);
    WD_TEST_BOOL(value.IsA<wdUuid>() && value.Get<wdUuid>() == pSubElementObject[1]->GetGuid());

    WD_TEST_BOOL(manager.CanRemove(pSubElementObject[3]).m_Result.Succeeded());
    manager.RemoveObject(pSubElementObject[3]);
    manager.DestroyObject(pSubElementObject[3]);
    pSubElementObject[3] = nullptr;
    WD_TEST_INT(pObject->GetTypeAccessor().GetCount(szProperty), 2);

    value = pObject->GetTypeAccessor().GetValue(szProperty, 0);
    WD_TEST_BOOL(value.IsA<wdUuid>() && value.Get<wdUuid>() == pSubElementObject[2]->GetGuid());
    value = pObject->GetTypeAccessor().GetValue(szProperty, 1);
    WD_TEST_BOOL(value.IsA<wdUuid>() && value.Get<wdUuid>() == pSubElementObject[1]->GetGuid());

    WD_TEST_BOOL(manager.CanMove(pSubElementObject[1], pChildObject, szProperty, 0).m_Result.Succeeded());
    manager.MoveObject(pSubElementObject[1], pChildObject, szProperty, 0);
    WD_TEST_BOOL(manager.CanMove(pSubElementObject[2], pChildObject, szProperty, 0).m_Result.Succeeded());
    manager.MoveObject(pSubElementObject[2], pChildObject, szProperty, 0);

    WD_TEST_INT(pObject->GetTypeAccessor().GetCount(szProperty), 0);
    WD_TEST_INT(pChildObject->GetTypeAccessor().GetCount(szProperty), 2);

    value = pChildObject->GetTypeAccessor().GetValue(szProperty, 0);
    WD_TEST_BOOL(value.IsA<wdUuid>() && value.Get<wdUuid>() == pSubElementObject[2]->GetGuid());
    value = pChildObject->GetTypeAccessor().GetValue(szProperty, 1);
    WD_TEST_BOOL(value.IsA<wdUuid>() && value.Get<wdUuid>() == pSubElementObject[1]->GetGuid());
  }

  manager.DestroyAllObjects();
}
