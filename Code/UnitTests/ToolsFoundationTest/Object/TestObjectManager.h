#pragma once

#include <Foundation/Serialization/RttiConverter.h>
#include <ToolsFoundation/Document/Document.h>
#include <ToolsFoundation/Object/DocumentObjectManager.h>
#include <ToolsFoundation/Object/DocumentObjectMirror.h>
#include <ToolsFoundation/Object/ObjectAccessorBase.h>
#include <ToolsFoundation/Serialization/DocumentObjectConverter.h>


class wdTestDocumentObjectManager : public wdDocumentObjectManager
{
public:
  wdTestDocumentObjectManager();
  ~wdTestDocumentObjectManager();
};


class wdTestDocument : public wdDocument
{
  WD_ADD_DYNAMIC_REFLECTION(wdTestDocument, wdDocument);

public:
  wdTestDocument(const char* szDocumentPath, bool bUseIPCObjectMirror = false);
  ~wdTestDocument();

  virtual void InitializeAfterLoading(bool bFirstTimeCreation) override;
  void ApplyNativePropertyChangesToObjectManager(wdDocumentObject* pObject);
  virtual wdDocumentInfo* CreateDocumentInfo() override;

  wdDocumentObjectMirror m_ObjectMirror;
  wdRttiConverterContext m_Context;



private:
  bool m_bUseIPCObjectMirror;
};
