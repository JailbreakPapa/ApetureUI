#pragma once

#include <Core/World/GameObject.h>
#include <Core/World/World.h>
#include <Foundation/Containers/Deque.h>
#include <Utilities/UtilitiesDLL.h>

/// \brief Stores a list of game objects as a 'selection'. Provides some common convenience functions for working with selections.
class WD_UTILITIES_DLL wdObjectSelection
{
public:
  wdObjectSelection();

  /// \brief The wdWorld in which the game objects are stored.
  void SetWorld(wdWorld* pWorld);

  /// \brief Returns the wdWorld in which the game objects live.
  const wdWorld* GetWorld() const { return m_pWorld; }

  /// \brief Clears the selection.
  void Clear() { m_Objects.Clear(); }

  /// \brief Iterates over all objects and removes the ones that have been destroyed from the selection.
  void RemoveDeadObjects();

  /// \brief Adds the given object to the selection, unless it is not valid anymore. Objects can be added multiple times.
  void AddObject(wdGameObjectHandle hObject, bool bDontAddTwice = true);

  /// \brief Removes the first occurrence of the given object from the selection. Returns false if the object did not exist in the
  /// selection.
  bool RemoveObject(wdGameObjectHandle hObject);

  /// \brief Removes the object from the selection if it exists already, otherwise adds it.
  void ToggleSelection(wdGameObjectHandle hObject);

  /// \brief Returns the number of objects in the selection.
  wdUInt32 GetCount() const { return m_Objects.GetCount(); }

  /// \brief Returns the n-th object in the selection.
  wdGameObjectHandle GetObject(wdUInt32 uiIndex) const { return m_Objects[uiIndex]; }

private:
  wdWorld* m_pWorld;
  wdDeque<wdGameObjectHandle> m_Objects;
};
