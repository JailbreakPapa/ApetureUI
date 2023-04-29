#pragma once

#include <Foundation/Containers/Map.h>
#include <Foundation/Math/BoundingBox.h>
#include <Foundation/Math/Frustum.h>
#include <Foundation/Math/Vec3.h>
#include <Utilities/UtilitiesDLL.h>

struct wdDynamicTree
{
  struct wdObjectData
  {
    wdInt32 m_iObjectType;
    wdInt32 m_iObjectInstance;
  };

  struct wdMultiMapKey
  {
    wdUInt32 m_uiKey;
    wdUInt32 m_uiCounter;

    wdMultiMapKey()
    {
      m_uiKey = 0;
      m_uiCounter = 0;
    }

    inline bool operator<(const wdMultiMapKey& rhs) const
    {
      if (m_uiKey == rhs.m_uiKey)
        return m_uiCounter < rhs.m_uiCounter;

      return m_uiKey < rhs.m_uiKey;
    }

    inline bool operator==(const wdMultiMapKey& rhs) const { return (m_uiCounter == rhs.m_uiCounter && m_uiKey == rhs.m_uiKey); }
  };
};

typedef wdMap<wdDynamicTree::wdMultiMapKey, wdDynamicTree::wdObjectData>::Iterator wdDynamicTreeObject;
typedef wdMap<wdDynamicTree::wdMultiMapKey, wdDynamicTree::wdObjectData>::ConstIterator wdDynamicTreeObjectConst;

/// \brief Callback type for object queries. Return "false" to abort a search (e.g. when the desired element has been found).
typedef bool (*WD_VISIBLE_OBJ_CALLBACK)(void* pPassThrough, wdDynamicTreeObjectConst Object);

class wdDynamicOctree;
class wdDynamicQuadtree;
