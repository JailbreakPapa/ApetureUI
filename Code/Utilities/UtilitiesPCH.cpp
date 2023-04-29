#include <Utilities/UtilitiesPCH.h>

WD_STATICLINK_LIBRARY(Utilities)
{
  if (bReturn)
    return;

  WD_STATICLINK_REFERENCE(Utilities_DGML_Implementation_DGMLCreator);
  WD_STATICLINK_REFERENCE(Utilities_DataStructures_Implementation_DynamicOctree);
  WD_STATICLINK_REFERENCE(Utilities_DataStructures_Implementation_DynamicQuadtree);
  WD_STATICLINK_REFERENCE(Utilities_DataStructures_Implementation_ObjectSelection);
  WD_STATICLINK_REFERENCE(Utilities_FileFormats_Implementation_OBJLoader);
  WD_STATICLINK_REFERENCE(Utilities_GridAlgorithms_Implementation_Rasterization);
  WD_STATICLINK_REFERENCE(Utilities_PathFinding_Implementation_GridNavmesh);
  WD_STATICLINK_REFERENCE(Utilities_Resources_ConfigFileResource);
}
