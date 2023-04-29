#pragma once

#include <Core/CoreDLL.h>
#include <Core/World/SpatialData.h>
#include <Foundation/Communication/Message.h>
#include <Foundation/Math/BoundingBoxSphere.h>

struct WD_CORE_DLL wdMsgUpdateLocalBounds : public wdMessage
{
  WD_DECLARE_MESSAGE_TYPE(wdMsgUpdateLocalBounds, wdMessage);

  WD_ALWAYS_INLINE void AddBounds(const wdBoundingBoxSphere& bounds, wdSpatialData::Category category)
  {
    m_ResultingLocalBounds.ExpandToInclude(bounds);
    m_uiSpatialDataCategoryBitmask |= category.GetBitmask();
  }

  ///\brief Enforces the object to be always visible. Note that you can't set this flag to false again,
  ///  because the same message is sent to multiple components and should accumulate the bounds.
  WD_ALWAYS_INLINE void SetAlwaysVisible(wdSpatialData::Category category)
  {
    m_bAlwaysVisible = true;
    m_uiSpatialDataCategoryBitmask |= category.GetBitmask();
  }

private:
  friend class wdGameObject;

  wdBoundingBoxSphere m_ResultingLocalBounds;
  wdUInt32 m_uiSpatialDataCategoryBitmask = 0;
  bool m_bAlwaysVisible = false;
};
