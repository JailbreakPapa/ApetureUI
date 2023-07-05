#include <ToolsFoundationTest/ToolsFoundationTestPCH.h>

#include <ToolsFoundationTest/Object/TestObjectManager.h>

WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdTestDocument, 1, wdRTTINoAllocator)
WD_END_DYNAMIC_REFLECTED_TYPE;


wdTestDocumentObjectManager::wdTestDocumentObjectManager() {}

wdTestDocumentObjectManager::~wdTestDocumentObjectManager() {}

wdTestDocument::wdTestDocument(const char* szDocumentPath, bool bUseIPCObjectMirror /*= false*/)
  : wdDocument(szDocumentPath, WD_DEFAULT_NEW(wdTestDocumentObjectManager))
  , m_bUseIPCObjectMirror(bUseIPCObjectMirror)
{
}

wdTestDocument::~wdTestDocument()
{
  if (m_bUseIPCObjectMirror)
  {
    m_ObjectMirror.Clear();
    m_ObjectMirror.DeInit();
  }
}

void wdTestDocument::InitializeAfterLoading(bool bFirstTimeCreation)
{
  SUPER::InitializeAfterLoading(bFirstTimeCreation);

  if (m_bUseIPCObjectMirror)
  {
    m_ObjectMirror.InitSender(GetObjectManager());
    m_ObjectMirror.InitReceiver(&m_Context);
    m_ObjectMirror.SendDocument();
  }
}

void wdTestDocument::ApplyNativePropertyChangesToObjectManager(wdDocumentObject* pObject)
{
  // Create native object graph
  wdAbstractObjectGraph graph;
  wdAbstractObjectNode* pRootNode = nullptr;
  {
    wdRttiConverterWriter rttiConverter(&graph, &m_Context, true, true);
    pRootNode = rttiConverter.AddObjectToGraph(pObject->GetType(), m_ObjectMirror.GetNativeObjectPointer(pObject), "Object");
  }

  // Create object manager graph
  wdAbstractObjectGraph origGraph;
  wdAbstractObjectNode* pOrigRootNode = nullptr;
  {
    wdDocumentObjectConverterWriter writer(&origGraph, GetObjectManager());
    pOrigRootNode = writer.AddObjectToGraph(pObject);
  }

  // Remap native guids so they match the object manager (stuff like embedded classes will not have a guid on the native side).
  graph.ReMapNodeGuidsToMatchGraph(pRootNode, origGraph, pOrigRootNode);
  wdDeque<wdAbstractGraphDiffOperation> diffResult;

  graph.CreateDiffWithBaseGraph(origGraph, diffResult);

  // As we messed up the native side the object mirror is no longer synced and needs to be destroyed.
  m_ObjectMirror.Clear();
  m_ObjectMirror.DeInit();

  // Apply diff while object mirror is down.
  GetObjectAccessor()->StartTransaction("Apply Native Property Changes to Object");
  wdDocumentObjectConverterReader::ApplyDiffToObject(GetObjectAccessor(), pObject, diffResult);
  GetObjectAccessor()->FinishTransaction();

  // Restart mirror from scratch.
  m_ObjectMirror.InitSender(GetObjectManager());
  m_ObjectMirror.InitReceiver(&m_Context);
  m_ObjectMirror.SendDocument();
}

wdDocumentInfo* wdTestDocument::CreateDocumentInfo()
{
  return WD_DEFAULT_NEW(wdDocumentInfo);
}
