#pragma once

#include <Core/World/Declarations.h>
#include <Utilities/UtilitiesDLL.h>

class wdWorld;
class wdDGMLGraph;

/// \brief This class encapsulates creating graphs from various core engine structures (like the game object graph etc.)
class WD_UTILITIES_DLL wdDGMLGraphCreator
{
public:
  /// \brief Adds the world hierarchy (game objects and components) to the given graph object.
  static void FillGraphFromWorld(wdWorld* pWorld, wdDGMLGraph& ref_graph);
};
