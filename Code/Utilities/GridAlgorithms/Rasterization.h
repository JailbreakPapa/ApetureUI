#pragma once

#include <Foundation/Containers/Deque.h>
#include <Foundation/Math/Vec3.h>
#include <Utilities/UtilitiesDLL.h>

/// \brief Enum values for success and failure. To be used by functions as return values mostly, instead of bool.
struct wdCallbackResult
{
  enum Enum
  {
    Stop,     ///< The calling function should stop expanding in this direction (might mean it should abort entirely)
    Continue, ///< The calling function should continue further.
  };
};

/// \brief Enum values for the result of some rasterization functions.
struct wdRasterizationResult
{
  enum Enum
  {
    Aborted,  ///< The function was aborted before it reached the end.
    Finished, ///< The function rasterized all possible points.
  };
};

namespace wd2DGridUtils
{
  /// \brief The callback declaration for the function that needs to be passed to the various rasterization functions.
  typedef wdCallbackResult::Enum (*WD_RASTERIZED_POINT_CALLBACK)(wdInt32 x, wdInt32 y, void* pPassThrough);

  /// \brief The callback declaration for the function that needs to be passed to RasterizeBlobWithDistance().
  typedef wdCallbackResult::Enum (*WD_RASTERIZED_BLOB_CALLBACK)(wdInt32 x, wdInt32 y, void* pPassThrough, wdUInt8 uiDistanceFromCenter);

  /// \brief Computes all the points on a 2D line and calls a function to report every point.
  ///
  /// The function implements Bresenham's algorithm for line rasterization. The first point to be reported through the
  /// callback is always the start position, the last point is always the end position.
  /// pPassThrough is passed through to the user callback for custom data.
  ///
  /// The function returns wdRasterizationResult::Aborted if the callback returned wdCallbackResult::Stop at any time
  /// and the line will not be computed further in that case.
  /// It returns wdRasterizationResult::Finished if the entire line was rasterized.
  ///
  /// This function does not do any dynamic memory allocations internally.
  WD_UTILITIES_DLL wdRasterizationResult::Enum ComputePointsOnLine(
    wdInt32 iStartX, wdInt32 iStartY, wdInt32 iEndX, wdInt32 iEndY, WD_RASTERIZED_POINT_CALLBACK callback, void* pPassThrough = nullptr);

  /// \brief Computes all the points on a 2D line and calls a function to report every point.
  ///
  /// Contrary to ComputePointsOnLine() this function does not do diagonal steps but inserts horizontal or vertical steps, such that
  /// reported cells are always connected by an edge.
  /// However, since there are always two possibilities to go from one cell to a diagonal cell, this function tries both and as long
  /// as one of them reports wdCallbackResult::Continue, it will continue. Only if both cells are blocked will the algorithm abort.
  ///
  /// If bVisitBothNeighbors is false, the line will continue with the diagonal cell if the first tried neighbor cell is free.
  /// However, if bVisitBothNeighbors is true, the second alternative cell is also reported to the callback, even though its return value
  /// has no effect on whether the line continues or aborts.
  WD_UTILITIES_DLL wdRasterizationResult::Enum ComputePointsOnLineConservative(wdInt32 iStartX, wdInt32 iStartY, wdInt32 iEndX, wdInt32 iEndY,
    WD_RASTERIZED_POINT_CALLBACK callback, void* pPassThrough = nullptr, bool bVisitBothNeighbors = false);

  /// \brief Computes all the points on a 2D circle and calls a function to report every point.
  ///
  /// The points are reported in a rather chaotic order (ie. when one draws a line from point to point, it does not yield a circle shape).
  /// The callback may abort the operation by returning wdCallbackResult::Stop.
  ///
  /// This function does not do any dynamic memory allocations internally.
  WD_UTILITIES_DLL wdRasterizationResult::Enum ComputePointsOnCircle(
    wdInt32 iStartX, wdInt32 iStartY, wdUInt32 uiRadius, WD_RASTERIZED_POINT_CALLBACK callback, void* pPassThrough = nullptr);

  /// \brief Starts at the given point and then fills all surrounding cells until a border is detected.
  ///
  /// The callback should return wdCallbackResult::Continue for each cell that has not been visited so far and for which all four direct
  /// neighbors should be visited. If the flood-fill algorithm leaves the valid area, the callback must return wdCallbackResult::Stop to
  /// signal a border. Thus the callback must be able to handle point positions outside the valid range and it also needs to be able to
  /// detect which cells have been visited before, as the FloodFill function will not keep that state internally.
  ///
  /// The function returns the number of cells that were visited and returned wdCallbackResult::Continue (ie. which were not classified as
  /// border cells).
  ///
  /// Note that the FloodFill function requires an internal queue to store which cells still need to be visited, as such it will do
  /// dynamic memory allocations. You can pass in a queue that will be used as the temp buffer, thus you can reuse the same container for
  /// several operations, which will reduce the amount of memory allocations that need to be done.
  WD_UTILITIES_DLL wdUInt32 FloodFill(
    wdInt32 iStartX, wdInt32 iStartY, WD_RASTERIZED_POINT_CALLBACK callback, void* pPassThrough = nullptr, wdDeque<wdVec2I32>* pTempArray = nullptr);

  /// \brief Same as FloodFill() but also visits the diagonal neighbors, ie. all eight neighboring cells.
  WD_UTILITIES_DLL wdUInt32 FloodFillDiag(
    wdInt32 iStartX, wdInt32 iStartY, WD_RASTERIZED_POINT_CALLBACK callback, void* pPassThrough = nullptr, wdDeque<wdVec2I32>* pTempArray = nullptr);

  /// \brief Describes the different circle types that can be rasterized
  enum wdBlobType : wdUInt8
  {
    Point1x1,    ///< The circle has just one point at the center
    Cross3x3,    ///< The circle has 5 points, one at the center, 1 at each edge of that
    Block3x3,    ///< The 'circle' is just a 3x3 rectangle (9 points)
    Circle5x5,   ///< The circle is a rectangle with each of the 4 corner points missing (21 points)
    Circle7x7,   ///< The circle is a actually starts looking like a circle (37 points)
    Circle9x9,   ///< Circle with 57 points
    Circle11x11, ///< Circle with 97 points
    Circle13x13, ///< Circle with 129 points
    Circle15x15, ///< Circle with 177 points
  };

  /// \brief Rasterizes a circle of limited dimensions and calls the given callback for each point.
  ///
  /// See wdCircleType for the available circle types. Those circles are handcrafted to have good looking shapes at low resolutions.
  /// This type of circle is not meant for actually rendering circles, but for doing area operations and overlapping checks for game
  /// units, visibility determination etc. Basically everything that is usually small, but where a simple point might not suffice.
  /// For example most units in a strategy game might only occupy a single cell, but some units might be larger and thus need to occupy
  /// the surrounding cells as well. Using RasterizeBlob() you can compute the units footprint easily.
  ///
  /// RasterizeBlob() will stop immediately and return wdRasterizationResult::Aborted when the callback function returns
  /// wdCallbackResult::Stop.
  WD_UTILITIES_DLL wdRasterizationResult::Enum RasterizeBlob(
    wdInt32 iPosX, wdInt32 iPosY, wdBlobType type, WD_RASTERIZED_POINT_CALLBACK callback, void* pPassThrough = nullptr);

  /// \brief Same as RasterizeBlob(), but the distance from the center is passed through to the callback, which can use this information to
  /// adjust what it is doing.
  WD_UTILITIES_DLL wdRasterizationResult::Enum RasterizeBlobWithDistance(
    wdInt32 iPosX, wdInt32 iPosY, wdBlobType type, WD_RASTERIZED_BLOB_CALLBACK callback, void* pPassThrough = nullptr);

  /// \brief Rasterizes a circle of any size (unlike RasterizeBlob()), though finding the right radius values for nice looking small circles
  /// can be more difficult.
  ///
  /// This function rasterizes a full circle. The radius is a float value, ie. you can use fractional values to shave off cells at the
  /// borders bit by bit.
  ///
  /// RasterizeCircle() will stop immediately and return wdRasterizationResult::Aborted when the callback function returns
  /// wdCallbackResult::Stop.
  WD_UTILITIES_DLL wdRasterizationResult::Enum RasterizeCircle(
    wdInt32 iPosX, wdInt32 iPosY, float fRadius, WD_RASTERIZED_POINT_CALLBACK callback, void* pPassThrough = nullptr);


  /// \brief Computes which points are visible from the start position by tracing lines radially outwards.
  ///
  /// The center start position is at iPosX, iPosY and uiRadius defines the maximum distance that an object can see.
  /// uiWidth and uiHeight define the maximum coordinates at which the end of the grid is reached (and thus the line tracing can early out
  /// if it reaches those). For the minimum coordinate (0, 0) is assumed.
  ///
  /// The callback function must return wdCallbackResult::Continue for cells that are not blocking and wdCallbackResult::Stop for cells that
  /// block visibility.
  ///
  /// The algorithm requires internal state and thus needs to do dynamic memory allocations. If you want to reduce the number of
  /// allocations, you can pass in your own array, that can be reused for many queries.
  WD_UTILITIES_DLL void ComputeVisibleArea(wdInt32 iPosX, wdInt32 iPosY, wdUInt16 uiRadius, wdUInt32 uiWidth, wdUInt32 uiHeight,
    WD_RASTERIZED_POINT_CALLBACK callback, void* pPassThrough = nullptr, wdDynamicArray<wdUInt8>* pTempArray = nullptr);

  /// \brief Computes which points are visible from the start position by tracing lines radially outwards. Limits the computation to a cone.
  ///
  /// This function works exactly like ComputeVisibleArea() but limits the computation to a cone that is defined by vDirection and
  /// ConeAngle.
  WD_UTILITIES_DLL void ComputeVisibleAreaInCone(wdInt32 iPosX, wdInt32 iPosY, wdUInt16 uiRadius, const wdVec2& vDirection, wdAngle coneAngle,
    wdUInt32 uiWidth, wdUInt32 uiHeight, WD_RASTERIZED_POINT_CALLBACK callback, void* pPassThrough = nullptr,
    wdDynamicArray<wdUInt8>* pTempArray = nullptr);
} // namespace wd2DGridUtils
