#pragma once

template <class CellData>
wdGameGrid<CellData>::wdGameGrid()
{
  m_uiGridSizeX = 0;
  m_uiGridSizeY = 0;

  m_mRotateToWorldspace.SetIdentity();
  m_mRotateToGridspace.SetIdentity();

  m_vWorldSpaceOrigin.SetZero();
  m_vLocalSpaceCellSize.Set(1.0f);
  m_vInverseLocalSpaceCellSize.Set(1.0f);
}

template <class CellData>
void wdGameGrid<CellData>::CreateGrid(wdUInt16 uiSizeX, wdUInt16 uiSizeY)
{
  m_Cells.Clear();

  m_uiGridSizeX = uiSizeX;
  m_uiGridSizeY = uiSizeY;

  m_Cells.SetCount(m_uiGridSizeX * m_uiGridSizeY);
}

template <class CellData>
void wdGameGrid<CellData>::SetWorldSpaceDimensions(const wdVec3& vLowerLeftCorner, const wdVec3& vCellSize, Orientation ori)
{
  wdMat3 mRot;

  switch (ori)
  {
    case InPlaneXY:
      mRot.SetIdentity();
      break;
    case InPlaneXZ:
      mRot.SetRotationMatrix(wdVec3(1, 0, 0), wdAngle::Degree(90.0f));
      break;
    case InPlaneXminusZ:
      mRot.SetRotationMatrix(wdVec3(1, 0, 0), wdAngle::Degree(-90.0f));
      break;
  }

  SetWorldSpaceDimensions(vLowerLeftCorner, vCellSize, mRot);
}

template <class CellData>
void wdGameGrid<CellData>::SetWorldSpaceDimensions(const wdVec3& vLowerLeftCorner, const wdVec3& vCellSize, const wdMat3& mRotation)
{
  m_vWorldSpaceOrigin = vLowerLeftCorner;
  m_vLocalSpaceCellSize = vCellSize;
  m_vInverseLocalSpaceCellSize = wdVec3(1.0f).CompDiv(vCellSize);

  m_mRotateToWorldspace = mRotation;
  m_mRotateToGridspace = mRotation.GetInverse();
}

template <class CellData>
wdVec2I32 wdGameGrid<CellData>::GetCellAtWorldPosition(const wdVec3& vWorldSpacePos) const
{
  const wdVec3 vCell = (m_mRotateToGridspace * ((vWorldSpacePos - m_vWorldSpaceOrigin)).CompMul(m_vInverseLocalSpaceCellSize));

  // Without the Floor, the border case when the position is outside (-1 / -1) is not immediately detected
  return wdVec2I32((wdInt32)wdMath::Floor(vCell.x), (wdInt32)wdMath::Floor(vCell.y));
}

template <class CellData>
wdVec3 wdGameGrid<CellData>::GetCellWorldSpaceOrigin(const wdVec2I32& vCoord) const
{
  return m_vWorldSpaceOrigin + m_mRotateToWorldspace * GetCellLocalSpaceOrigin(vCoord);
}

template <class CellData>
wdVec3 wdGameGrid<CellData>::GetCellLocalSpaceOrigin(const wdVec2I32& vCoord) const
{
  return m_vLocalSpaceCellSize.CompMul(wdVec3((float)vCoord.x, (float)vCoord.y, 0.0f));
}

template <class CellData>
wdVec3 wdGameGrid<CellData>::GetCellWorldSpaceCenter(const wdVec2I32& vCoord) const
{
  return m_vWorldSpaceOrigin + m_mRotateToWorldspace * GetCellLocalSpaceCenter(vCoord);
}

template <class CellData>
wdVec3 wdGameGrid<CellData>::GetCellLocalSpaceCenter(const wdVec2I32& vCoord) const
{
  return m_vLocalSpaceCellSize.CompMul(wdVec3((float)vCoord.x + 0.5f, (float)vCoord.y + 0.5f, 0.5f));
}

template <class CellData>
bool wdGameGrid<CellData>::IsValidCellCoordinate(const wdVec2I32& vCoord) const
{
  return (vCoord.x >= 0 && vCoord.x < m_uiGridSizeX && vCoord.y >= 0 && vCoord.y < m_uiGridSizeY);
}

template <class CellData>
bool wdGameGrid<CellData>::PickCell(const wdVec3& vRayStartPos, const wdVec3& vRayDirNorm, wdVec2I32* out_pCellCoord, wdVec3* out_pIntersection) const
{
  wdPlane p;
  p.SetFromNormalAndPoint(m_mRotateToWorldspace * wdVec3(0, 0, -1), m_vWorldSpaceOrigin);

  wdVec3 vPos;

  if (!p.GetRayIntersection(vRayStartPos, vRayDirNorm, nullptr, &vPos))
    return false;

  if (out_pIntersection)
    *out_pIntersection = vPos;

  if (out_pCellCoord)
    *out_pCellCoord = GetCellAtWorldPosition(vPos);

  return true;
}

template <class CellData>
wdBoundingBox wdGameGrid<CellData>::GetWorldBoundingBox() const
{
  wdVec3 vGridBox(m_uiGridSizeX, m_uiGridSizeY, 1.0f);

  vGridBox = m_mRotateToWorldspace * m_vLocalSpaceCellSize.CompMul(vGridBox);

  return wdBoundingBox(m_vWorldSpaceOrigin, m_vWorldSpaceOrigin + vGridBox);
}

template <class CellData>
bool wdGameGrid<CellData>::GetRayIntersection(const wdVec3& vRayStartWorldSpace, const wdVec3& vRayDirNormalizedWorldSpace, float fMaxLength,
  float& out_fIntersection, wdVec2I32& out_vCellCoord) const
{
  const wdVec3 vRayStart = m_mRotateToGridspace * (vRayStartWorldSpace - m_vWorldSpaceOrigin);
  const wdVec3 vRayDir = m_mRotateToGridspace * vRayDirNormalizedWorldSpace;

  wdVec3 vGridBox(m_uiGridSizeX, m_uiGridSizeY, 1.0f);

  const wdBoundingBox localBox(wdVec3(0.0f), m_vLocalSpaceCellSize.CompMul(vGridBox));

  if (localBox.Contains(vRayStart))
  {
    // if the ray is already inside the box, we know that a cell is hit
    out_fIntersection = 0.0f;
  }
  else
  {
    if (!localBox.GetRayIntersection(vRayStart, vRayDir, &out_fIntersection, nullptr))
      return false;

    if (out_fIntersection > fMaxLength)
      return false;
  }

  const wdVec3 vEnterPos = vRayStart + vRayDir * out_fIntersection;

  const wdVec3 vCell = vEnterPos.CompMul(m_vInverseLocalSpaceCellSize);

  // Without the Floor, the border case when the position is outside (-1 / -1) is not immediately detected
  out_vCellCoord = wdVec2I32((wdInt32)wdMath::Floor(vCell.x), (wdInt32)wdMath::Floor(vCell.y));
  out_vCellCoord.x = wdMath::Clamp(out_vCellCoord.x, 0, m_uiGridSizeX - 1);
  out_vCellCoord.y = wdMath::Clamp(out_vCellCoord.y, 0, m_uiGridSizeY - 1);

  return true;
}

template <class CellData>
bool wdGameGrid<CellData>::GetRayIntersectionExpandedBBox(const wdVec3& vRayStartWorldSpace, const wdVec3& vRayDirNormalizedWorldSpace,
  float fMaxLength, float& out_fIntersection, const wdVec3& vExpandBBoxByThis) const
{
  const wdVec3 vRayStart = m_mRotateToGridspace * (vRayStartWorldSpace - m_vWorldSpaceOrigin);
  const wdVec3 vRayDir = m_mRotateToGridspace * vRayDirNormalizedWorldSpace;

  wdVec3 vGridBox(m_uiGridSizeX, m_uiGridSizeY, 1.0f);

  wdBoundingBox localBox(wdVec3(0.0f), m_vLocalSpaceCellSize.CompMul(vGridBox));
  localBox.Grow(vExpandBBoxByThis);

  if (localBox.Contains(vRayStart))
  {
    // if the ray is already inside the box, we know that a cell is hit
    out_fIntersection = 0.0f;
  }
  else
  {
    if (!localBox.GetRayIntersection(vRayStart, vRayDir, &out_fIntersection, nullptr))
      return false;

    if (out_fIntersection > fMaxLength)
      return false;
  }

  return true;
}
