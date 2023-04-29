#include <Utilities/UtilitiesPCH.h>

#include <Core/World/GameObject.h>
#include <Core/World/World.h>
#include <Foundation/Utilities/DGMLWriter.h>
#include <Utilities/DGML/DGMLCreator.h>

void wdDGMLGraphCreator::FillGraphFromWorld(wdWorld* pWorld, wdDGMLGraph& ref_graph)
{
  if (!pWorld)
  {
    wdLog::Warning("wdDGMLGraphCreator::FillGraphFromWorld() called with null world!");
    return;
  }


  struct GraphVisitor
  {
    GraphVisitor(wdDGMLGraph& ref_graph)
      : m_Graph(ref_graph)
    {
      wdDGMLGraph::NodeDesc nd;
      nd.m_Color = wdColor::DarkRed;
      nd.m_Shape = wdDGMLGraph::NodeShape::Button;
      m_WorldNodeId = ref_graph.AddNode("World", &nd);
    }

    wdVisitorExecution::Enum Visit(wdGameObject* pObject)
    {
      wdStringBuilder name;
      name.Format("GameObject: \"{0}\"", pObject->GetName().IsEmpty() ? "<Unnamed>" : pObject->GetName());

      // Create node for game object
      wdDGMLGraph::NodeDesc gameobjectND;
      gameobjectND.m_Color = wdColor::CornflowerBlue;
      gameobjectND.m_Shape = wdDGMLGraph::NodeShape::Rectangle;
      auto gameObjectNodeId = m_Graph.AddNode(name.GetData(), &gameobjectND);

      m_VisitedObjects.Insert(pObject, gameObjectNodeId);

      // Add connection to parent if existent
      if (const wdGameObject* parent = pObject->GetParent())
      {
        auto it = m_VisitedObjects.Find(parent);

        if (it.IsValid())
        {
          m_Graph.AddConnection(gameObjectNodeId, it.Value());
        }
      }
      else
      {
        // No parent -> connect to world
        m_Graph.AddConnection(gameObjectNodeId, m_WorldNodeId);
      }

      // Add components
      for (auto component : pObject->GetComponents())
      {
        auto szComponentName = component->GetDynamicRTTI()->GetTypeName();

        wdDGMLGraph::NodeDesc componentND;
        componentND.m_Color = wdColor::LimeGreen;
        componentND.m_Shape = wdDGMLGraph::NodeShape::RoundedRectangle;
        auto componentNodeId = m_Graph.AddNode(szComponentName, &componentND);

        // And add the link to the game object

        m_Graph.AddConnection(componentNodeId, gameObjectNodeId);
      }

      return wdVisitorExecution::Continue;
    }

    wdDGMLGraph& m_Graph;

    wdDGMLGraph::NodeId m_WorldNodeId;
    wdMap<const wdGameObject*, wdDGMLGraph::NodeId> m_VisitedObjects;
  };

  GraphVisitor visitor(ref_graph);
  pWorld->Traverse(wdWorld::VisitorFunc(&GraphVisitor::Visit, &visitor), wdWorld::BreadthFirst);
}



WD_STATICLINK_FILE(Utilities, Utilities_DGML_Implementation_DGMLCreator);
