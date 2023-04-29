#include <Utilities/UtilitiesPCH.h>

#include <Utilities/DataStructures/ObjectSelection.h>

wdObjectSelection::wdObjectSelection()
{
  m_pWorld = nullptr;
}

void wdObjectSelection::SetWorld(wdWorld* pWorld)
{
  WD_ASSERT_DEV((m_pWorld == nullptr) || (m_pWorld == pWorld) || m_Objects.IsEmpty(), "You cannot change the world for this selection.");

  m_pWorld = pWorld;
}

void wdObjectSelection::RemoveDeadObjects()
{
  WD_ASSERT_DEV(m_pWorld != nullptr, "The world has not been set.");

  for (wdUInt32 i = m_Objects.GetCount(); i > 0; --i)
  {
    wdGameObject* pObject;
    if (!m_pWorld->TryGetObject(m_Objects[i - 1], pObject))
    {
      m_Objects.RemoveAtAndCopy(i - 1); // keep the order
    }
  }
}

void wdObjectSelection::AddObject(wdGameObjectHandle hObject, bool bDontAddTwice)
{
  WD_ASSERT_DEV(m_pWorld != nullptr, "The world has not been set.");

  // only insert valid objects
  wdGameObject* pObject;
  if (!m_pWorld->TryGetObject(hObject, pObject))
    return;

  if (m_Objects.IndexOf(hObject) != wdInvalidIndex)
    return;

  m_Objects.PushBack(hObject);
}

bool wdObjectSelection::RemoveObject(wdGameObjectHandle hObject)
{
  return m_Objects.RemoveAndCopy(hObject);
}

void wdObjectSelection::ToggleSelection(wdGameObjectHandle hObject)
{
  for (wdUInt32 i = 0; i < m_Objects.GetCount(); ++i)
  {
    if (m_Objects[i] == hObject)
    {
      m_Objects.RemoveAtAndCopy(i); // keep the order
      return;
    }
  }

  // ensures invalid objects don't get added
  AddObject(hObject);
}



WD_STATICLINK_FILE(Utilities, Utilities_DataStructures_Implementation_ObjectSelection);
