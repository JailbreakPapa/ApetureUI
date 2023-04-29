#include <Utilities/UtilitiesPCH.h>

#include <Utilities/GridAlgorithms/Rasterization.h>

wdRasterizationResult::Enum wd2DGridUtils::ComputePointsOnLine(
  wdInt32 iStartX, wdInt32 iStartY, wdInt32 iEndX, wdInt32 iEndY, WD_RASTERIZED_POINT_CALLBACK callback, void* pPassThrough /* = nullptr */)
{
  // Implements Bresenham's line algorithm:
  // http://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

  wdInt32 dx = wdMath::Abs(iEndX - iStartX);
  wdInt32 dy = wdMath::Abs(iEndY - iStartY);

  wdInt32 sx = (iStartX < iEndX) ? 1 : -1;
  wdInt32 sy = (iStartY < iEndY) ? 1 : -1;

  wdInt32 err = dx - dy;

  while (true)
  {
    // The user callback can stop the algorithm at any point, if no further points on the line are required
    if (callback(iStartX, iStartY, pPassThrough) == wdCallbackResult::Stop)
      return wdRasterizationResult::Aborted;

    if ((iStartX == iEndX) && (iStartY == iEndY))
      return wdRasterizationResult::Finished;

    wdInt32 e2 = 2 * err;

    if (e2 > -dy)
    {
      err = err - dy;
      iStartX = iStartX + sx;
    }

    if (e2 < dx)
    {
      err = err + dx;
      iStartY = iStartY + sy;
    }
  }
}

wdRasterizationResult::Enum wd2DGridUtils::ComputePointsOnLineConservative(wdInt32 iStartX, wdInt32 iStartY, wdInt32 iEndX, wdInt32 iEndY,
  WD_RASTERIZED_POINT_CALLBACK callback, void* pPassThrough /* = nullptr */, bool bVisitBothNeighbors /* = false */)
{
  wdInt32 dx = wdMath::Abs(iEndX - iStartX);
  wdInt32 dy = wdMath::Abs(iEndY - iStartY);

  wdInt32 sx = (iStartX < iEndX) ? 1 : -1;
  wdInt32 sy = (iStartY < iEndY) ? 1 : -1;

  wdInt32 err = dx - dy;

  wdInt32 iLastX = iStartX;
  wdInt32 iLastY = iStartY;

  while (true)
  {
    // if this is going to be a diagonal step, make sure to insert horizontal/vertical steps

    if ((wdMath::Abs(iLastX - iStartX) + wdMath::Abs(iLastY - iStartY)) == 2)
    {
      // This part is the difference to the non-conservative line algorithm

      if (callback(iLastX, iStartY, pPassThrough) == wdCallbackResult::Continue)
      {
        // first one succeeded, going to continue

        // if this is true, the user still wants a callback for the alternative, even though it does not change the outcome anymore
        if (bVisitBothNeighbors)
          callback(iStartX, iLastY, pPassThrough);
      }
      else
      {
        // first one failed, try the second
        if (callback(iStartX, iLastY, pPassThrough) == wdCallbackResult::Stop)
          return wdRasterizationResult::Aborted;
      }
    }

    iLastX = iStartX;
    iLastY = iStartY;

    // The user callback can stop the algorithm at any point, if no further points on the line are required
    if (callback(iStartX, iStartY, pPassThrough) == wdCallbackResult::Stop)
      return wdRasterizationResult::Aborted;

    if ((iStartX == iEndX) && (iStartY == iEndY))
      return wdRasterizationResult::Finished;

    wdInt32 e2 = 2 * err;

    if (e2 > -dy)
    {
      err = err - dy;
      iStartX = iStartX + sx;
    }

    if (e2 < dx)
    {
      err = err + dx;
      iStartY = iStartY + sy;
    }
  }
}


wdRasterizationResult::Enum wd2DGridUtils::ComputePointsOnCircle(
  wdInt32 iStartX, wdInt32 iStartY, wdUInt32 uiRadius, WD_RASTERIZED_POINT_CALLBACK callback, void* pPassThrough /* = nullptr */)
{
  int f = 1 - uiRadius;
  int ddF_x = 1;
  int ddF_y = -2 * uiRadius;
  int x = 0;
  int y = uiRadius;

  // report the four extremes
  if (callback(iStartX, iStartY + uiRadius, pPassThrough) == wdCallbackResult::Stop)
    return wdRasterizationResult::Aborted;
  if (callback(iStartX, iStartY - uiRadius, pPassThrough) == wdCallbackResult::Stop)
    return wdRasterizationResult::Aborted;
  if (callback(iStartX + uiRadius, iStartY, pPassThrough) == wdCallbackResult::Stop)
    return wdRasterizationResult::Aborted;
  if (callback(iStartX - uiRadius, iStartY, pPassThrough) == wdCallbackResult::Stop)
    return wdRasterizationResult::Aborted;

  // the loop iterates over an eighth of the circle (a 45 degree segment) and then mirrors each point 8 times to fill the entire circle
  while (x < y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    if (callback(iStartX + x, iStartY + y, pPassThrough) == wdCallbackResult::Stop)
      return wdRasterizationResult::Aborted;
    if (callback(iStartX - x, iStartY + y, pPassThrough) == wdCallbackResult::Stop)
      return wdRasterizationResult::Aborted;
    if (callback(iStartX + x, iStartY - y, pPassThrough) == wdCallbackResult::Stop)
      return wdRasterizationResult::Aborted;
    if (callback(iStartX - x, iStartY - y, pPassThrough) == wdCallbackResult::Stop)
      return wdRasterizationResult::Aborted;
    if (callback(iStartX + y, iStartY + x, pPassThrough) == wdCallbackResult::Stop)
      return wdRasterizationResult::Aborted;
    if (callback(iStartX - y, iStartY + x, pPassThrough) == wdCallbackResult::Stop)
      return wdRasterizationResult::Aborted;
    if (callback(iStartX + y, iStartY - x, pPassThrough) == wdCallbackResult::Stop)
      return wdRasterizationResult::Aborted;
    if (callback(iStartX - y, iStartY - x, pPassThrough) == wdCallbackResult::Stop)
      return wdRasterizationResult::Aborted;
  }

  return wdRasterizationResult::Finished;
}

wdUInt32 wd2DGridUtils::FloodFill(wdInt32 iStartX, wdInt32 iStartY, WD_RASTERIZED_POINT_CALLBACK callback, void* pPassThrough /* = nullptr */,
  wdDeque<wdVec2I32>* pTempArray /* = nullptr */)
{
  wdUInt32 uiFilled = 0;

  wdDeque<wdVec2I32> FallbackQueue;

  if (pTempArray == nullptr)
    pTempArray = &FallbackQueue;

  pTempArray->Clear();
  pTempArray->PushBack(wdVec2I32(iStartX, iStartY));

  while (!pTempArray->IsEmpty())
  {
    wdVec2I32 v = pTempArray->PeekBack();
    pTempArray->PopBack();

    if (callback(v.x, v.y, pPassThrough) == wdCallbackResult::Continue)
    {
      ++uiFilled;

      // put the four neighbors into the queue
      pTempArray->PushBack(wdVec2I32(v.x - 1, v.y));
      pTempArray->PushBack(wdVec2I32(v.x + 1, v.y));
      pTempArray->PushBack(wdVec2I32(v.x, v.y - 1));
      pTempArray->PushBack(wdVec2I32(v.x, v.y + 1));
    }
  }

  return uiFilled;
}

wdUInt32 wd2DGridUtils::FloodFillDiag(wdInt32 iStartX, wdInt32 iStartY, WD_RASTERIZED_POINT_CALLBACK callback, void* pPassThrough /*= nullptr*/,
  wdDeque<wdVec2I32>* pTempArray /*= nullptr*/)
{
  wdUInt32 uiFilled = 0;

  wdDeque<wdVec2I32> FallbackQueue;

  if (pTempArray == nullptr)
    pTempArray = &FallbackQueue;

  pTempArray->Clear();
  pTempArray->PushBack(wdVec2I32(iStartX, iStartY));

  while (!pTempArray->IsEmpty())
  {
    wdVec2I32 v = pTempArray->PeekBack();
    pTempArray->PopBack();

    if (callback(v.x, v.y, pPassThrough) == wdCallbackResult::Continue)
    {
      ++uiFilled;

      // put the eight neighbors into the queue
      pTempArray->PushBack(wdVec2I32(v.x - 1, v.y));
      pTempArray->PushBack(wdVec2I32(v.x + 1, v.y));
      pTempArray->PushBack(wdVec2I32(v.x, v.y - 1));
      pTempArray->PushBack(wdVec2I32(v.x, v.y + 1));

      pTempArray->PushBack(wdVec2I32(v.x - 1, v.y - 1));
      pTempArray->PushBack(wdVec2I32(v.x + 1, v.y - 1));
      pTempArray->PushBack(wdVec2I32(v.x + 1, v.y + 1));
      pTempArray->PushBack(wdVec2I32(v.x - 1, v.y + 1));
    }
  }

  return uiFilled;
}

// Lookup table that describes the shape of the circle
// When rasterizing circles with few pixels algorithms usually don't give nice shapes
// so this lookup table is handcrafted for better results
static const wdUInt8 OverlapCircle[15][15] = {{9, 9, 9, 9, 9, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9}, {9, 9, 9, 8, 8, 7, 7, 7, 7, 7, 8, 8, 9, 9, 9},
  {9, 9, 8, 8, 7, 6, 6, 6, 6, 6, 7, 8, 8, 9, 9}, {9, 8, 8, 7, 6, 6, 5, 5, 5, 6, 6, 7, 8, 8, 9}, {9, 8, 7, 6, 6, 5, 4, 4, 4, 5, 6, 6, 7, 8, 9},
  {8, 7, 6, 6, 5, 4, 3, 3, 3, 4, 5, 6, 6, 7, 8}, {8, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8}, {8, 7, 6, 5, 4, 3, 1, 0, 1, 3, 4, 5, 6, 7, 8},
  {8, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8}, {8, 7, 6, 6, 5, 4, 3, 3, 3, 4, 5, 6, 6, 7, 8}, {9, 8, 7, 6, 6, 5, 4, 4, 4, 5, 6, 6, 7, 8, 9},
  {9, 8, 8, 7, 6, 6, 5, 5, 5, 6, 6, 7, 8, 8, 9}, {9, 9, 8, 8, 7, 6, 6, 6, 6, 6, 7, 8, 8, 9, 9}, {9, 9, 9, 8, 8, 7, 7, 7, 7, 7, 8, 8, 9, 9, 9},
  {9, 9, 9, 9, 9, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9}};

static const wdInt32 CircleCenter = 7;
static const wdUInt8 CircleAreaMin[9] = {7, 6, 6, 5, 4, 3, 2, 1, 0};
static const wdUInt8 CircleAreaMax[9] = {7, 8, 8, 9, 10, 11, 12, 13, 14};

wdRasterizationResult::Enum wd2DGridUtils::RasterizeBlob(
  wdInt32 iPosX, wdInt32 iPosY, wdBlobType type, WD_RASTERIZED_POINT_CALLBACK callback, void* pPassThrough /* = nullptr */)
{
  const wdUInt8 uiCircleType = wdMath::Clamp<wdUInt8>(type, 0, 8);

  const wdInt32 iAreaMin = CircleAreaMin[uiCircleType];
  const wdInt32 iAreaMax = CircleAreaMax[uiCircleType];

  iPosX -= CircleCenter;
  iPosY -= CircleCenter;

  for (wdInt32 y = iAreaMin; y <= iAreaMax; ++y)
  {
    for (wdInt32 x = iAreaMin; x <= iAreaMax; ++x)
    {
      if (OverlapCircle[y][x] <= uiCircleType)
      {
        if (callback(iPosX + x, iPosY + y, pPassThrough) == wdCallbackResult::Stop)
          return wdRasterizationResult::Aborted;
      }
    }
  }

  return wdRasterizationResult::Finished;
}

wdRasterizationResult::Enum wd2DGridUtils::RasterizeBlobWithDistance(
  wdInt32 iPosX, wdInt32 iPosY, wdBlobType type, WD_RASTERIZED_BLOB_CALLBACK callback, void* pPassThrough /*= nullptr*/)
{
  const wdUInt8 uiCircleType = wdMath::Clamp<wdUInt8>(type, 0, 8);

  const wdInt32 iAreaMin = CircleAreaMin[uiCircleType];
  const wdInt32 iAreaMax = CircleAreaMax[uiCircleType];

  iPosX -= CircleCenter;
  iPosY -= CircleCenter;

  for (wdInt32 y = iAreaMin; y <= iAreaMax; ++y)
  {
    for (wdInt32 x = iAreaMin; x <= iAreaMax; ++x)
    {
      const wdUInt8 uiDistance = OverlapCircle[y][x];

      if (uiDistance <= uiCircleType)
      {
        if (callback(iPosX + x, iPosY + y, pPassThrough, uiDistance) == wdCallbackResult::Stop)
          return wdRasterizationResult::Aborted;
      }
    }
  }

  return wdRasterizationResult::Finished;
}

wdRasterizationResult::Enum wd2DGridUtils::RasterizeCircle(
  wdInt32 iPosX, wdInt32 iPosY, float fRadius, WD_RASTERIZED_POINT_CALLBACK callback, void* pPassThrough /* = nullptr */)
{
  const wdVec2 vCenter((float)iPosX, (float)iPosY);

  const wdInt32 iRadius = (wdInt32)fRadius;
  const float fRadiusSqr = wdMath::Square(fRadius);

  for (wdInt32 y = iPosY - iRadius; y <= iPosY + iRadius; ++y)
  {
    for (wdInt32 x = iPosX - iRadius; x <= iPosX + iRadius; ++x)
    {
      const wdVec2 v((float)x, (float)y);

      if ((v - vCenter).GetLengthSquared() > fRadiusSqr)
        continue;

      if (callback(x, y, pPassThrough) == wdCallbackResult::Stop)
        return wdRasterizationResult::Aborted;
    }
  }

  return wdRasterizationResult::Finished;
}


struct VisibilityLine
{
  wdDynamicArray<wdUInt8>* m_pVisible;
  wdUInt32 m_uiSize;
  wdUInt32 m_uiRadius;
  wdInt32 m_iCenterX;
  wdInt32 m_iCenterY;
  wd2DGridUtils::WD_RASTERIZED_POINT_CALLBACK m_VisCallback;
  void* m_pUserPassThrough;
  wdUInt32 m_uiWidth;
  wdUInt32 m_uiHeight;
  wdVec2 m_vDirection;
  wdAngle m_ConeAngle;
};

struct CellFlags
{
  enum Enum
  {
    NotVisited = 0,
    Visited = WD_BIT(0),
    Visible = Visited | WD_BIT(1),
    Invisible = Visited,
  };
};

static wdCallbackResult::Enum MarkPointsOnLineVisible(wdInt32 x, wdInt32 y, void* pPassThrough)
{
  VisibilityLine* VisLine = (VisibilityLine*)pPassThrough;

  // if the reported point is outside the playing field, don't continue
  if (x < 0 || y < 0 || x >= (wdInt32)VisLine->m_uiWidth || y >= (wdInt32)VisLine->m_uiHeight)
    return wdCallbackResult::Stop;

  // compute the point position inside our virtual grid (where the start position is at the center)
  const wdUInt32 VisX = x - VisLine->m_iCenterX + VisLine->m_uiRadius;
  const wdUInt32 VisY = y - VisLine->m_iCenterY + VisLine->m_uiRadius;

  // if we are outside our virtual grid, stop
  if (VisX >= (wdInt32)VisLine->m_uiSize || VisY >= (wdInt32)VisLine->m_uiSize)
    return wdCallbackResult::Stop;

  // We actually only need two bits for each cell (visited + visible)
  // so we pack the information for four cells into one byte
  const wdUInt32 uiCellIndex = VisY * VisLine->m_uiSize + VisX;
  const wdUInt32 uiBitfieldByte = uiCellIndex >> 2;                   // division by four
  const wdUInt32 uiBitfieldBiteOff = uiBitfieldByte << 2;             // modulo to determine where in the byte this cell is stored
  const wdUInt32 uiMaskShift = (uiCellIndex - uiBitfieldBiteOff) * 2; // times two because we use two bits

  wdUInt8& CellFlagsRef = (*VisLine->m_pVisible)[uiBitfieldByte];    // for writing into the byte later
  const wdUInt8 ThisCellsFlags = (CellFlagsRef >> uiMaskShift) & 3U; // the decoded flags value for reading (3U == lower two bits)

  // if this point on the line was already visited and determined to be invisible, don't continue
  if (ThisCellsFlags == CellFlags::Invisible)
    return wdCallbackResult::Stop;

  // this point has been visited already and the point was determined to be visible, so just continue
  if (ThisCellsFlags == CellFlags::Visible)
    return wdCallbackResult::Continue;

  // apparently this cell has not been visited yet, so ask the user callback what to do
  if (VisLine->m_VisCallback(x, y, VisLine->m_pUserPassThrough) == wdCallbackResult::Continue)
  {
    // the callback reported this cell as visible, so flag it and continue
    CellFlagsRef |= ((wdUInt8)CellFlags::Visible) << uiMaskShift;
    return wdCallbackResult::Continue;
  }

  // the callback reported this flag as invisible, flag it and stop the line
  CellFlagsRef |= ((wdUInt8)CellFlags::Invisible) << uiMaskShift;
  return wdCallbackResult::Stop;
}

static wdCallbackResult::Enum MarkPointsInCircleVisible(wdInt32 x, wdInt32 y, void* pPassThrough)
{
  VisibilityLine* ld = (VisibilityLine*)pPassThrough;

  wd2DGridUtils::ComputePointsOnLineConservative(ld->m_iCenterX, ld->m_iCenterY, x, y, MarkPointsOnLineVisible, pPassThrough, false);

  return wdCallbackResult::Continue;
}

void wd2DGridUtils::ComputeVisibleArea(wdInt32 iPosX, wdInt32 iPosY, wdUInt16 uiRadius, wdUInt32 uiWidth, wdUInt32 uiHeight,
  WD_RASTERIZED_POINT_CALLBACK callback, void* pPassThrough /* = nullptr */, wdDynamicArray<wdUInt8>* pTempArray /* = nullptr */)
{
  const wdUInt32 uiSize = uiRadius * 2 + 1;

  wdDynamicArray<wdUInt8> VisiblityFlags;

  // if we don't get a temp array, use our own array, with blackjack etc.
  if (pTempArray == nullptr)
    pTempArray = &VisiblityFlags;

  pTempArray->Clear();
  pTempArray->SetCount(wdMath::Square(uiSize) / 4); // we store only two bits per cell, so we can pack four values into each byte

  VisibilityLine ld;
  ld.m_uiSize = uiSize;
  ld.m_uiRadius = uiRadius;
  ld.m_pVisible = pTempArray;
  ld.m_iCenterX = iPosX;
  ld.m_iCenterY = iPosY;
  ld.m_VisCallback = callback;
  ld.m_pUserPassThrough = pPassThrough;
  ld.m_uiWidth = uiWidth;
  ld.m_uiHeight = uiHeight;

  // from the center, trace lines to all points on the circle around it
  // each line determines for each cell whether it is visible
  // once an invisible cell is encountered, a line will stop further tracing
  // no cell is ever reported twice to the user callback
  wd2DGridUtils::ComputePointsOnCircle(iPosX, iPosY, uiRadius, MarkPointsInCircleVisible, &ld);
}

static wdCallbackResult::Enum MarkPointsInConeVisible(wdInt32 x, wdInt32 y, void* pPassThrough)
{
  VisibilityLine* ld = (VisibilityLine*)pPassThrough;

  const wdVec2 vPos((float)x, (float)y);
  const wdVec2 vDirToPos = (vPos - wdVec2((float)ld->m_iCenterX, (float)ld->m_iCenterY)).GetNormalized();

  const wdAngle angle = wdMath::ACos(vDirToPos.Dot(ld->m_vDirection));

  if (angle.GetRadian() < ld->m_ConeAngle.GetRadian())
    wd2DGridUtils::ComputePointsOnLineConservative(ld->m_iCenterX, ld->m_iCenterY, x, y, MarkPointsOnLineVisible, pPassThrough, false);

  return wdCallbackResult::Continue;
}

void wd2DGridUtils::ComputeVisibleAreaInCone(wdInt32 iPosX, wdInt32 iPosY, wdUInt16 uiRadius, const wdVec2& vDirection, wdAngle coneAngle,
  wdUInt32 uiWidth, wdUInt32 uiHeight, WD_RASTERIZED_POINT_CALLBACK callback, void* pPassThrough /* = nullptr */,
  wdDynamicArray<wdUInt8>* pTempArray /* = nullptr */)
{
  const wdUInt32 uiSize = uiRadius * 2 + 1;

  wdDynamicArray<wdUInt8> VisiblityFlags;

  // if we don't get a temp array, use our own array, with blackjack etc.
  if (pTempArray == nullptr)
    pTempArray = &VisiblityFlags;

  pTempArray->Clear();
  pTempArray->SetCount(wdMath::Square(uiSize) / 4); // we store only two bits per cell, so we can pack four values into each byte


  VisibilityLine ld;
  ld.m_uiSize = uiSize;
  ld.m_uiRadius = uiRadius;
  ld.m_pVisible = pTempArray;
  ld.m_iCenterX = iPosX;
  ld.m_iCenterY = iPosY;
  ld.m_VisCallback = callback;
  ld.m_pUserPassThrough = pPassThrough;
  ld.m_uiWidth = uiWidth;
  ld.m_uiHeight = uiHeight;
  ld.m_vDirection = vDirection;
  ld.m_ConeAngle = coneAngle;

  wd2DGridUtils::ComputePointsOnCircle(iPosX, iPosY, uiRadius, MarkPointsInConeVisible, &ld);
}



WD_STATICLINK_FILE(Utilities, Utilities_GridAlgorithms_Implementation_Rasterization);
