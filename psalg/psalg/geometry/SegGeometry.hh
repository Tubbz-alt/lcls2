#ifndef PSALG_SEGGEOMETRY_H
#define PSALG_SEGGEOMETRY_H

//-------------------

#include "psalg/geometry/GeometryTypes.hh"

//#include <cstddef>  // for size_t
//#include <stdint.h>  // uint8_t, uint16_t, uint32_t, etc.

using namespace std;

//-------------------

namespace geometry {

/// @addtogroup geometry geometry

/**
 *  @ingroup geometry
 *
 *  @brief Abstract base class SegGeometry defines the interface to access segment pixel coordinates.
 *
 *  This software was developed for the LCLS project.
 *  If you use all or part of it, please give an appropriate acknowledgment.
 *
 *  @author Mikhail S. Dubrovin
 */

//-------------------

const static angle_t DEG_TO_RAD = 3.141592653589793238463 / 180;

//-------------------

class SegGeometry {
public:

  /// Enumerator for X, Y, and Z axes
  //enum AXIS {AXIS_X=0, AXIS_Y, AXIS_Z};

  SegGeometry();

  virtual ~SegGeometry();

  /// Prints segment info for selected bits
  virtual void print_seg_info(const unsigned& pbits=0) = 0;

  /// Returns segment size - total number of pixels in segment
  virtual const gsize_t size() = 0;

  /// Returns number of rows in segment
  virtual const gsize_t rows() = 0;

  /// Returns number of cols in segment
  virtual const gsize_t cols() = 0;

  /// Returns shape of the segment {rows, cols}
  virtual const gsize_t* shape() = 0;

  /// Returns pixel size in um for indexing
  virtual const pixel_coord_t pixel_scale_size() = 0;

  /// Returns pointer to the array of pixel areas normalized on minimal pixel area
  virtual const pixel_area_t* pixel_area_array() = 0;

  /// Returns pointer to the array of pixel size in um for AXIS
  virtual const pixel_coord_t* pixel_size_array(AXIS axis) = 0;

  /// Returns pointer to the array of segment pixel coordinates in um for AXIS
  virtual const pixel_coord_t* pixel_coord_array(AXIS axis) = 0;

  /// Returns minimal value in the array of segment pixel coordinates in um for AXIS
  virtual const pixel_coord_t pixel_coord_min(AXIS axis) = 0;

  /// Returns maximal value in the array of segment pixel coordinates in um for AXIS
  virtual const pixel_coord_t pixel_coord_max(AXIS axis) = 0;

  /// Returns pointer to the array of pixel mask
  virtual const pixel_mask_t* pixel_mask_array(const unsigned& mbits = 0377) = 0;
};

/// Global method, returns minimal value of the array of specified length 
template <typename T>
T min_of_arr(const T* arr, gsize_t size);

/// Global method, returns maximal value of the array of specified length 
template <typename T>
T max_of_arr(const T* arr, gsize_t size);

/// Copy constructor and assignment are disabled
  // SegGeometry(const SegGeometry&) = delete;
  // SegGeometry& operator = (const SegGeometry&) = delete;

} // namespace geometry

#endif // PSALG_SEGGEOMETRY_H

//-------------------
