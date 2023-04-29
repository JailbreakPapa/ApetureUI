#pragma once

#include <Utilities/DataStructures/Implementation/DynamicTree.h>

/// \brief A loose Octree implementation that is very lightweight on RAM.
///
/// This Octree does not store any bookkeeping information per node.\n
/// Memory usage is linear in the number of objects inserted.\n
/// An empty tree only needs few bytes. This is accomplished by making the tree
/// static in it's dimensions and maximum subdivisions, such that each node can be assigned
/// a unique index. A map is then used to store objects through the node-index in
/// which they are located.\n
/// At traversals each node's bounding-box needs to be computed on-the-fly thus adding some
/// CPU overhead (though, fewer memory use usually also means fewer cache-misses).
/// \n
/// Inserting an object is O(log d), with d being the tree-depth.\n
/// Removing an object is either O(1) or O(n), with n being the number of objects inserted,
/// depending on whether an iterator to the object is available.\n
/// \n
/// The nodes get indices in such a way that when it is detected, that a whole subtree is
/// visible, all objects can be returned quickly, without further traversal.\n
/// If a subtree does not contain any data, traversal is not continued further, either.\n
/// \n
/// In general this octree implementation is made to be very flexible and easily usable for
/// many kinds of problems. All it stores are two integers for an object (GroupID, InstanceID).
/// The object data itself must be stored somewhere else. You can easily store very different
/// types of objects in the same tree.\n
/// Once objects are inserted, you can do range queries to find all objects in some location.
/// Since removal is usually O(1) and insertion is O(d) the tree can be used for very dynamic
/// data that changes frequently at run-time.
class WD_UTILITIES_DLL wdDynamicOctree
{
  /// \brief The amount that cells overlap (this is a loose octree). Typically set to 10%.
  static const float s_fLooseOctreeFactor;

public:
  wdDynamicOctree();

  /// \brief Initializes the tree with a fixed size and minimum node dimensions.
  ///
  /// \param vCenter
  ///   The center position of the tree.
  /// \param vHalfExtents
  ///   The dimensions along each axis. The tree is always axis aligned. The tree will enclose all space from
  ///   (vCenter - vHalfExtents) to (vCenter + vHalfExtents).
  ///   Note that the tree will always use the maximum extent for all three extents, making the tree square,
  ///   which affects the total number of nodes.\n
  ///   However, the original (non-square) bounding box is used to determine whether some objects is inside the tree, at all,
  ///   which might affect whether they are rejected during tree insertion.
  /// \param fMinNodeSize
  ///   The length of the cell's edges at the finest level. For a typical game world, where your level might
  ///   have extents of 100 to 1000 meters, the min node size should not be smaller than 1 meter.
  ///   The smaller the node size, the more cells the tree has. The limit of nodes in the tree is 2^32.
  ///   A tree with 100 meters extents in X, Y and Z direction and a min node size of 1 meter, will have 1000000 nodes
  ///   on the finest level (and roughly 1500000 nodes in total).
  void CreateTree(const wdVec3& vCenter, const wdVec3& vHalfExtents, float fMinNodeSize); // [tested]

  /// \brief Returns true when there are no objects stored inside the tree.
  bool IsEmpty() const { return m_NodeMap.IsEmpty(); } // [tested]

  /// \brief Returns the number of objects that have been inserted into the tree.
  wdUInt32 GetCount() const { return m_NodeMap.GetCount(); } // [tested]

  /// \brief Adds an object at position vCenter with bounding-box dimensions vHalfExtents to the tree. If the object is outside the tree and
  /// bOnlyIfInside is true, nothing will be inserted.
  ///
  /// Returns WD_SUCCESS when an object is inserted, WD_FAILURE when the object was rejected. The latter can only happen when bOnlyIfInside
  /// is set to true. Through out_Object the exact identifier for the object in the tree is returned, which allows for removing the object
  /// with O(1) complexity later. iObjectType and iObjectInstance are the two user values that will be stored for the object. With
  /// RemoveObjectsOfType() one can also remove all objects with the same iObjectType value, if needed.
  wdResult InsertObject(const wdVec3& vCenter, const wdVec3& vHalfExtents, wdInt32 iObjectType, wdInt32 iObjectInstance,
    wdDynamicTreeObject* out_pObject = nullptr, bool bOnlyIfInside = false); // [tested]

  /// \brief Calls the Callback for every object that is inside the View-frustum. pPassThrough is passed to the Callback for custom
  /// purposes.
  void FindVisibleObjects(const wdFrustum& viewfrustum, WD_VISIBLE_OBJ_CALLBACK callback, void* pPassThrough) const;

  /// \brief Returns all objects that are located in a node that overlaps with the given point.
  ///
  /// \note This function will most likely also return objects that do not overlap with the point itself, because they are located
  /// in a node that overlaps with the point. You might need to do more thorough overlap checks to filter those out.
  void FindObjectsInRange(const wdVec3& vPoint, WD_VISIBLE_OBJ_CALLBACK callback, void* pPassThrough = nullptr) const; // [tested]

  /// \brief Returns all objects that are located in a node that overlaps with the rectangle with center vPoint and half edge length
  /// fRadius.
  ///
  /// \note This function will most likely also return objects that do not overlap with the rectangle itself, because they are located
  /// in a node that overlaps with the rectangle. You might need to do more thorough overlap checks to filter those out.
  void FindObjectsInRange(const wdVec3& vPoint, float fRadius, WD_VISIBLE_OBJ_CALLBACK callback,
    void* pPassThrough = nullptr) const; // [tested]

  /// \brief Removes the given Object. Attention: This is an O(n) operation.
  void RemoveObject(wdInt32 iObjectType, wdInt32 iObjectInstance); // [tested]

  /// \brief Removes the given Object. This is an O(1) operation.
  void RemoveObject(wdDynamicTreeObject obj); // [tested]

  /// \brief Removes all Objects of the given Type. This is an O(n) operation.
  void RemoveObjectsOfType(wdInt32 iObjectType); // [tested]

  /// \brief Removes all Objects, but the tree stays intact.
  void RemoveAllObjects()
  {
    m_NodeMap.Clear();
    m_uiMultiMapCounter = 1;
  } // [tested]

  /// \brief Returns the tree's adjusted (square) AABB.
  const wdBoundingBox& GetBoundingBox() const { return m_BBox; } // [tested]

private:
  /// \brief Recursively checks in which node an object is located and stores it at the node where it fits best.
  bool InsertObject(const wdVec3& vCenter, const wdVec3& vHalfExtents, const wdDynamicTree::wdObjectData& Obj, float minx, float maxx, float miny,
    float maxy, float minz, float maxz, wdUInt32 uiNodeID, wdUInt32 uiAddID, wdUInt32 uiSubAddID, wdDynamicTreeObject* out_Object);

  /// \brief Recursively checks which nodes are visible and calls the callback for each object at those nodes.
  void FindVisibleObjects(const wdFrustum& Viewfrustum, WD_VISIBLE_OBJ_CALLBACK Callback, void* pPassThrough, float minx, float maxx, float miny,
    float maxy, float minz, float maxz, wdUInt32 uiNodeID, wdUInt32 uiAddID, wdUInt32 uiSubAddID, wdUInt32 uiNextNodeID) const;

  /// \brief Recursively checks in which node a point is located and calls the callback for all objects at those nodes.
  bool FindObjectsInRange(const wdVec3& vPoint, WD_VISIBLE_OBJ_CALLBACK Callback, void* pPassThrough, float minx, float maxx, float miny, float maxy,
    float minz, float maxz, wdUInt32 uiNodeID, wdUInt32 uiAddID, wdUInt32 uiSubAddID, wdUInt32 uiNextNodeID) const;

  /// \brief Recursively checks which node(s) a circle touches and calls the callback for all objects at those nodes.
  bool FindObjectsInRange(const wdVec3& vPoint, float fRadius, WD_VISIBLE_OBJ_CALLBACK Callback, void* pPassThrough, float minx, float maxx,
    float miny, float maxy, float minz, float maxz, wdUInt32 uiNodeID, wdUInt32 uiAddID, wdUInt32 uiSubAddID, wdUInt32 uiNextNodeID) const;

  /// \brief The tree depth, used for finding a nodes unique ID
  wdUInt32 m_uiMaxTreeDepth;

  // \brief Also used for finding a nodes unique ID
  wdUInt32 m_uiAddIDTopLevel;

  /// \brief The square bounding Box (to prevent long thin nodes)
  wdBoundingBox m_BBox;

  /// \brief The actual bounding box (to discard objects that are outside the world)
  float m_fRealMinX, m_fRealMaxX, m_fRealMinY, m_fRealMaxY, m_fRealMinZ, m_fRealMaxZ;

  /// \brief Used to turn the map into a multi-map.
  wdUInt32 m_uiMultiMapCounter;

  /// \brief Every node has a unique index, the map allows to store many objects at each node, using that index
  wdMap<wdDynamicTree::wdMultiMapKey, wdDynamicTree::wdObjectData> m_NodeMap;
};
