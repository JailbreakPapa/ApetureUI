#include <ToolsFoundationTest/ToolsFoundationTestPCH.h>

#include <Foundation/Reflection/Reflection.h>
#include <Foundation/Serialization/AbstractObjectGraph.h>
#include <Foundation/Serialization/GraphVersioning.h>
#include <Foundation/Serialization/RttiConverter.h>
#include <ToolsFoundation/Reflection/PhantomRttiManager.h>
#include <ToolsFoundation/Reflection/ToolsReflectionUtils.h>
#include <ToolsFoundation/Serialization/ToolsSerializationUtils.h>
#include <ToolsFoundationTest/Reflection/ReflectionTestClasses.h>

WD_CREATE_SIMPLE_TEST_GROUP(Versioning);

struct wdPatchTestBase
{
public:
  wdPatchTestBase()
  {
    m_string = "Base";
    m_string2 = "";
  }

  wdString m_string;
  wdString m_string2;
};
WD_DECLARE_REFLECTABLE_TYPE(WD_NO_LINKAGE, wdPatchTestBase);

// clang-format off
WD_BEGIN_STATIC_REFLECTED_TYPE(wdPatchTestBase, wdNoBase, 1, wdRTTIDefaultAllocator<wdPatchTestBase>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_MEMBER_PROPERTY("String", m_string),
    WD_MEMBER_PROPERTY("String2", m_string2),
  }
  WD_END_PROPERTIES;
}
WD_END_STATIC_REFLECTED_TYPE;
// clang-format on

struct wdPatchTest : public wdPatchTestBase
{
public:
  wdPatchTest() { m_iInt32 = 1; }

  wdInt32 m_iInt32;
};
WD_DECLARE_REFLECTABLE_TYPE(WD_NO_LINKAGE, wdPatchTest);

// clang-format off
WD_BEGIN_STATIC_REFLECTED_TYPE(wdPatchTest, wdPatchTestBase, 1, wdRTTIDefaultAllocator<wdPatchTest>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_MEMBER_PROPERTY("Int", m_iInt32),
  }
  WD_END_PROPERTIES;
}
WD_END_STATIC_REFLECTED_TYPE;
// clang-format on

namespace
{
  /// Patch class
  class wdPatchTestP : public wdGraphPatch
  {
  public:
    wdPatchTestP()
      : wdGraphPatch("wdPatchTestP", 2)
    {
    }
    virtual void Patch(wdGraphPatchContext& ref_context, wdAbstractObjectGraph* pGraph, wdAbstractObjectNode* pNode) const override
    {
      pNode->RenameProperty("Int", "IntRenamed");
      pNode->ChangeProperty("IntRenamed", 2);
    }
  };
  wdPatchTestP g_wdPatchTestP;

  /// Patch base class
  class wdPatchTestBaseBP : public wdGraphPatch
  {
  public:
    wdPatchTestBaseBP()
      : wdGraphPatch("wdPatchTestBaseBP", 2)
    {
    }
    virtual void Patch(wdGraphPatchContext& ref_context, wdAbstractObjectGraph* pGraph, wdAbstractObjectNode* pNode) const override
    {
      pNode->ChangeProperty("String", "BaseClassPatched");
    }
  };
  wdPatchTestBaseBP g_wdPatchTestBaseBP;

  /// Rename class
  class wdPatchTestRN : public wdGraphPatch
  {
  public:
    wdPatchTestRN()
      : wdGraphPatch("wdPatchTestRN", 2)
    {
    }
    virtual void Patch(wdGraphPatchContext& ref_context, wdAbstractObjectGraph* pGraph, wdAbstractObjectNode* pNode) const override
    {
      ref_context.RenameClass("wdPatchTestRN2");
      pNode->ChangeProperty("String", "RenameExecuted");
    }
  };
  wdPatchTestRN g_wdPatchTestRN;

  /// Patch renamed class to v3
  class wdPatchTestRN2 : public wdGraphPatch
  {
  public:
    wdPatchTestRN2()
      : wdGraphPatch("wdPatchTestRN2", 3)
    {
    }
    virtual void Patch(wdGraphPatchContext& ref_context, wdAbstractObjectGraph* pGraph, wdAbstractObjectNode* pNode) const override
    {
      pNode->ChangeProperty("String2", "Patched");
    }
  };
  wdPatchTestRN2 g_wdPatchTestRN2;

  /// Change base class
  class wdPatchTestCB : public wdGraphPatch
  {
  public:
    wdPatchTestCB()
      : wdGraphPatch("wdPatchTestCB", 2)
    {
    }
    virtual void Patch(wdGraphPatchContext& ref_context, wdAbstractObjectGraph* pGraph, wdAbstractObjectNode* pNode) const override
    {
      wdVersionKey bases[] = {{"wdPatchTestBaseBP", 1}};
      ref_context.ChangeBaseClass(bases);
      pNode->ChangeProperty("String2", "ChangedBase");
    }
  };
  wdPatchTestCB g_wdPatchTestCB;

  void ReplaceTypeName(wdAbstractObjectGraph& ref_graph, wdAbstractObjectGraph& ref_typesGraph, const char* szOldName, const char* szNewName)
  {
    for (auto it : ref_graph.GetAllNodes())
    {
      auto* pNode = it.Value();

      if (wdStringUtils::IsEqual(szOldName, pNode->GetType()))
        pNode->SetType(szNewName);
    }

    for (auto it : ref_typesGraph.GetAllNodes())
    {
      auto* pNode = it.Value();

      if (wdStringUtils::IsEqual("wdReflectedTypeDescriptor", pNode->GetType()))
      {
        if (auto* pProp = pNode->FindProperty("TypeName"))
        {
          if (wdStringUtils::IsEqual(szOldName, pProp->m_Value.Get<wdString>()))
            pProp->m_Value = szNewName;
        }
        if (auto* pProp = pNode->FindProperty("ParentTypeName"))
        {
          if (wdStringUtils::IsEqual(szOldName, pProp->m_Value.Get<wdString>()))
            pProp->m_Value = szNewName;
        }
      }
    }
  }

  wdAbstractObjectNode* SerializeObject(wdAbstractObjectGraph& ref_graph, wdAbstractObjectGraph& ref_typesGraph, const wdRTTI* pRtti, void* pObject)
  {
    wdAbstractObjectNode* pNode = nullptr;
    {
      // Object
      wdRttiConverterContext context;
      wdRttiConverterWriter rttiConverter(&ref_graph, &context, true, true);
      context.RegisterObject(wdUuid::StableUuidForString(pRtti->GetTypeName()), pRtti, pObject);
      pNode = rttiConverter.AddObjectToGraph(pRtti, pObject, "ROOT");
    }
    {
      // Types
      wdSet<const wdRTTI*> types;
      types.Insert(pRtti);
      wdReflectionUtils::GatherDependentTypes(pRtti, types);
      wdToolsSerializationUtils::SerializeTypes(types, ref_typesGraph);
    }
    return pNode;
  }

  void PatchGraph(wdAbstractObjectGraph& ref_graph, wdAbstractObjectGraph& ref_typesGraph)
  {
    wdGraphVersioning::GetSingleton()->PatchGraph(&ref_typesGraph);
    wdGraphVersioning::GetSingleton()->PatchGraph(&ref_graph, &ref_typesGraph);
  }
} // namespace

WD_CREATE_SIMPLE_TEST(Versioning, GraphPatch)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "PatchClass")
  {
    wdAbstractObjectGraph graph;
    wdAbstractObjectGraph typesGraph;

    wdPatchTest data;
    data.m_iInt32 = 5;
    wdAbstractObjectNode* pNode = SerializeObject(graph, typesGraph, wdGetStaticRTTI<wdPatchTest>(), &data);
    ReplaceTypeName(graph, typesGraph, "wdPatchTest", "wdPatchTestP");
    PatchGraph(graph, typesGraph);

    wdAbstractObjectNode::Property* pInt = pNode->FindProperty("IntRenamed");
    WD_TEST_INT(2, pInt->m_Value.Get<wdInt32>());
    WD_TEST_BOOL(pNode->FindProperty("Int") == nullptr);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "PatchBaseClass")
  {
    wdAbstractObjectGraph graph;
    wdAbstractObjectGraph typesGraph;

    wdPatchTest data;
    data.m_string = "Unpatched";
    wdAbstractObjectNode* pNode = SerializeObject(graph, typesGraph, wdGetStaticRTTI<wdPatchTest>(), &data);
    ReplaceTypeName(graph, typesGraph, "wdPatchTestBase", "wdPatchTestBaseBP");
    PatchGraph(graph, typesGraph);

    wdAbstractObjectNode::Property* pString = pNode->FindProperty("String");
    WD_TEST_STRING(pString->m_Value.Get<wdString>(), "BaseClassPatched");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "RenameClass")
  {
    wdAbstractObjectGraph graph;
    wdAbstractObjectGraph typesGraph;

    wdPatchTest data;
    data.m_string = "NotRenamed";
    wdAbstractObjectNode* pNode = SerializeObject(graph, typesGraph, wdGetStaticRTTI<wdPatchTest>(), &data);
    ReplaceTypeName(graph, typesGraph, "wdPatchTest", "wdPatchTestRN");
    PatchGraph(graph, typesGraph);

    wdAbstractObjectNode::Property* pString = pNode->FindProperty("String");
    WD_TEST_BOOL(pString->m_Value.Get<wdString>() == "RenameExecuted");
    WD_TEST_STRING(pNode->GetType(), "wdPatchTestRN2");
    WD_TEST_INT(pNode->GetTypeVersion(), 3);
    wdAbstractObjectNode::Property* pString2 = pNode->FindProperty("String2");
    WD_TEST_BOOL(pString2->m_Value.Get<wdString>() == "Patched");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ChangeBaseClass")
  {
    wdAbstractObjectGraph graph;
    wdAbstractObjectGraph typesGraph;

    wdPatchTest data;
    data.m_string = "NotPatched";
    wdAbstractObjectNode* pNode = SerializeObject(graph, typesGraph, wdGetStaticRTTI<wdPatchTest>(), &data);
    ReplaceTypeName(graph, typesGraph, "wdPatchTest", "wdPatchTestCB");
    PatchGraph(graph, typesGraph);

    wdAbstractObjectNode::Property* pString = pNode->FindProperty("String");
    WD_TEST_STRING(pString->m_Value.Get<wdString>(), "BaseClassPatched");
    WD_TEST_INT(pNode->GetTypeVersion(), 2);
    wdAbstractObjectNode::Property* pString2 = pNode->FindProperty("String2");
    WD_TEST_STRING(pString2->m_Value.Get<wdString>(), "ChangedBase");
  }
}
