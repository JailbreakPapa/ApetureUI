#pragma once

#include <Core/World/World.h>
#include <Foundation/IO/Stream.h>
#include <Foundation/Types/TagSet.h>

/// \brief Stores an entire wdWorld in a stream.
///
/// Used for exporting a world in binary form either as a level or as a prefab (though there is no
/// difference).
/// Can be used for saving a game, if the exact state of the world shall be stored (e.g. like in an FPS).
class WD_CORE_DLL wdWorldWriter
{
public:
  /// \brief Writes all content in \a world to \a stream.
  ///
  /// All game objects with tags that overlap with \a pExclude will be ignored.
  void WriteWorld(wdStreamWriter& inout_stream, wdWorld& ref_world, const wdTagSet* pExclude = nullptr);

  /// \brief Only writes the given root objects and all their children to the stream.
  void WriteObjects(wdStreamWriter& inout_stream, const wdDeque<const wdGameObject*>& rootObjects);

  /// \brief Only writes the given root objects and all their children to the stream.
  void WriteObjects(wdStreamWriter& inout_stream, wdArrayPtr<const wdGameObject*> rootObjects);

  /// \brief Writes the given game object handle to the stream.
  ///
  /// \note If the handle belongs to an object that is not part of the serialized scene, e.g. an object
  /// that was excluded by a tag, this function will assert.
  void WriteGameObjectHandle(const wdGameObjectHandle& hObject);

  /// \brief Writes the given component handle to the stream.
  ///
  /// \note If the handle belongs to a component that is not part of the serialized scene, e.g. an object
  /// that was excluded by a tag, this function will assert.
  void WriteComponentHandle(const wdComponentHandle& hComponent);

  /// \brief Accesses the stream to which data is written. Use this in component serialization functions
  /// to write data to the stream.
  wdStreamWriter& GetStream() const { return *m_pStream; }

  /// \brief Returns an array containing all game object pointers that were written to the stream as root objects
  const wdDeque<const wdGameObject*>& GetAllWrittenRootObjects() const { return m_AllRootObjects; }

  /// \brief Returns an array containing all game object pointers that were written to the stream as child objects
  const wdDeque<const wdGameObject*>& GetAllWrittenChildObjects() const { return m_AllChildObjects; }

private:
  void Clear();
  wdResult WriteToStream();
  void AssignGameObjectIndices();
  void AssignComponentHandleIndices(const wdMap<wdString, const wdRTTI*>& sortedTypes);
  void IncludeAllComponentBaseTypes();
  void IncludeAllComponentBaseTypes(const wdRTTI* pRtti);
  void Traverse(wdGameObject* pObject);

  wdVisitorExecution::Enum ObjectTraverser(wdGameObject* pObject);
  void WriteGameObject(const wdGameObject* pObject);
  void WriteComponentTypeInfo(const wdRTTI* pRtti);
  void WriteComponentCreationData(const wdDeque<const wdComponent*>& components);
  void WriteComponentSerializationData(const wdDeque<const wdComponent*>& components);

  wdStreamWriter* m_pStream = nullptr;
  const wdTagSet* m_pExclude = nullptr;

  wdDeque<const wdGameObject*> m_AllRootObjects;
  wdDeque<const wdGameObject*> m_AllChildObjects;
  wdMap<wdGameObjectHandle, wdUInt32> m_WrittenGameObjectHandles;

  struct Components
  {
    wdUInt16 m_uiSerializedTypeIndex = 0;
    wdDeque<const wdComponent*> m_Components;
    wdMap<wdComponentHandle, wdUInt32> m_HandleToIndex;
  };

  wdHashTable<const wdRTTI*, Components> m_AllComponents;
};
