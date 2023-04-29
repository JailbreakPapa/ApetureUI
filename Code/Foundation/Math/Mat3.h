#pragma once

#include <Foundation/Math/Angle.h>
#include <Foundation/Math/Vec3.h>

/// \brief A 3x3 component matrix class.
template <typename Type>
class wdMat3Template
{
public:
  WD_DECLARE_POD_TYPE();

  using ComponentType = Type;

  // *** Data ***
public:
  // The elements are stored in column-major order.
  // That means first is column 0 (with elements of row 0, row 1, row 2),
  // then column 1, then column 2

  /// \brief The matrix as a 9-element Type array (column-major)
  Type m_fElementsCM[9];

  WD_ALWAYS_INLINE Type& Element(wdInt32 iColumn, wdInt32 iRow) { return m_fElementsCM[iColumn * 3 + iRow]; }
  WD_ALWAYS_INLINE Type Element(wdInt32 iColumn, wdInt32 iRow) const { return m_fElementsCM[iColumn * 3 + iRow]; }

  // *** Constructors ***
public:
  /// \brief Default Constructor DOES NOT INITIALIZE the matrix, at all.
  wdMat3Template(); // [tested]

  /// \brief Copies 9 values from pData into the matrix. Can handle the data in row-major or column-major order.
  ///
  /// \param pData
  ///   The array of Type values from which to set the matrix data.
  /// \param layout
  ///   The layout in which pData stores the matrix. The data will get transposed, if necessary.
  ///   The data should be in column-major format, if you want to prevent unnecessary transposes.
  wdMat3Template(const Type* const pData, wdMatrixLayout::Enum layout); // [tested]

  /// \brief Sets each element manually: Naming is "column-n row-m"
  wdMat3Template(Type c1r1, Type c2r1, Type c3r1, Type c1r2, Type c2r2, Type c3r2, Type c1r3, Type c2r3, Type c3r3); // [tested]

#if WD_ENABLED(WD_MATH_CHECK_FOR_NAN)
  void AssertNotNaN() const
  {
    WD_ASSERT_ALWAYS(!IsNaN(), "This object contains NaN values. This can happen when you forgot to initialize it before using it. Please check that "
                               "all code-paths properly initialize this object.");
  }
#endif

  /// \brief Copies 9 values from pData into the matrix. Can handle the data in row-major or column-major order.
  ///
  /// \param pData
  ///   The array of Type values from which to set the matrix data.
  /// \param layout
  ///   The layout in which pData stores the matrix. The data will get transposed, if necessary.
  ///   The data should be in column-major format, if you want to prevent unnecessary transposes.
  void SetFromArray(const Type* const pData, wdMatrixLayout::Enum layout); // [tested]

  /// \brief Copies the 9 values of this matrix into the given array. 'layout' defines whether the data should end up in column-major or row-major
  /// format.
  void GetAsArray(Type* out_pData, wdMatrixLayout::Enum layout) const; // [tested]

  /// \brief Sets each element manually: Naming is "column-n row-m"
  void SetElements(Type c1r1, Type c2r1, Type c3r1, Type c1r2, Type c2r2, Type c3r2, Type c1r3, Type c2r3, Type c3r3); // [tested]

  // *** Special matrix constructors ***
public:
  /// \brief Sets all elements to zero.
  void SetZero(); // [tested]

  /// \brief Sets all elements to zero, except the diagonal, which is set to one.
  void SetIdentity(); // [tested]

  /// \brief Sets the matrix to all zero, except the diagonal, which is set to x,y,z,1
  void SetScalingMatrix(const wdVec3Template<Type>& vScale); // [tested]

  /// \brief Sets this matrix to be a rotation matrix around the X-axis.
  void SetRotationMatrixX(wdAngle angle); // [tested]

  /// \brief Sets this matrix to be a rotation matrix around the Y-axis.
  void SetRotationMatrixY(wdAngle angle); // [tested]

  /// \brief Sets this matrix to be a rotation matrix around the Z-axis.
  void SetRotationMatrixZ(wdAngle angle); // [tested]

  /// \brief Sets this matrix to be a rotation matrix around the given axis.
  void SetRotationMatrix(const wdVec3Template<Type>& vAxis, wdAngle angle); // [tested]

  // *** Common Matrix Operations ***
public:
  /// \brief Returns an Identity Matrix.
  static const wdMat3Template<Type> IdentityMatrix(); // [tested]

  /// \brief Returns a Zero Matrix.
  static const wdMat3Template<Type> ZeroMatrix(); // [tested]

  /// \brief Transposes this matrix.
  void Transpose(); // [tested]

  /// \brief Returns the transpose of this matrix.
  const wdMat3Template<Type> GetTranspose() const; // [tested]

  /// \brief Inverts this matrix. Return value indicates whether the matrix could be Inverted.
  wdResult Invert(Type fEpsilon = wdMath::SmallEpsilon<Type>()); // [tested]

  /// \brief Returns the inverse of this matrix.
  const wdMat3Template<Type> GetInverse(Type fEpsilon = wdMath::SmallEpsilon<Type>()) const; // [tested]

  // *** Checks ***
public:
  /// \brief Checks whether all elements are zero.
  bool IsZero(Type fEpsilon = wdMath::DefaultEpsilon<Type>()) const; // [tested]

  /// \brief Checks whether this is an identity matrix.
  bool IsIdentity(Type fEpsilon = wdMath::DefaultEpsilon<Type>()) const; // [tested]

  /// \brief Checks whether all components are finite numbers.
  bool IsValid() const; // [tested]

  /// \brief Checks whether any component is NaN.
  bool IsNaN() const; // [tested]

  // *** Special Accessors ***
public:
  /// \brief Returns all 3 components of the i-th row.
  wdVec3Template<Type> GetRow(wdUInt32 uiRow) const; // [tested]

  /// \brief Sets all 3 components of the i-th row.
  void SetRow(wdUInt32 uiRow, const wdVec3Template<Type>& vRow); // [tested]

  /// \brief Returns all 3 components of the i-th column.
  wdVec3Template<Type> GetColumn(wdUInt32 uiColumn) const; // [tested]

  /// \brief Sets all 3 components of the i-th column.
  void SetColumn(wdUInt32 uiColumn, const wdVec3Template<Type>& vColumn); // [tested]

  /// \brief Returns all 3 components on the diagonal of the matrix.
  wdVec3Template<Type> GetDiagonal() const; // [tested]

  /// \brief Sets all 3 components on the diagonal of the matrix.
  void SetDiagonal(const wdVec3Template<Type>& vDiag); // [tested]

  /// \brief Returns the 3 scaling factors that are encoded in the matrix.
  const wdVec3Template<Type> GetScalingFactors() const; // [tested]

  /// \brief Tries to set the three scaling factors in the matrix. Returns WD_FAILURE if the matrix columns cannot be normalized and thus no rescaling
  /// is possible.
  wdResult SetScalingFactors(const wdVec3Template<Type>& vXYZ, Type fEpsilon = wdMath::DefaultEpsilon<Type>()); // [tested]

  /// \brief Computes the determinant of the matix.
  Type GetDeterminant() const;

  // *** Operators ***
public:
  /// \brief Matrix-vector multiplication, assuming the 4th component of the vector is zero. So, rotation/scaling only. Useful as an optimization.
  const wdVec3Template<Type> TransformDirection(const wdVec3Template<Type>& v) const; // [tested]

  /// \brief Component-wise multiplication (commutative)
  void operator*=(Type f);

  /// \brief Component-wise division.
  void operator/=(Type f); // [tested]

  /// \brief Equality Check.
  bool IsIdentical(const wdMat3Template<Type>& rhs) const; // [tested]

  /// \brief Equality Check with epsilon.
  bool IsEqual(const wdMat3Template<Type>& rhs, Type fEpsilon) const; // [tested]
};


// *** free functions ***

/// \brief Matrix-Matrix multiplication
template <typename Type>
const wdMat3Template<Type> operator*(const wdMat3Template<Type>& m1, const wdMat3Template<Type>& m2); // [tested]

/// \brief Matrix-vector multiplication
template <typename Type>
const wdVec3Template<Type> operator*(const wdMat3Template<Type>& m, const wdVec3Template<Type>& v); // [tested]

/// \brief Component-wise multiplication (commutative)
template <typename Type>
const wdMat3Template<Type> operator*(const wdMat3Template<Type>& m1, Type f); // [tested]

/// \brief Component-wise multiplication (commutative)
template <typename Type>
const wdMat3Template<Type> operator*(Type f, const wdMat3Template<Type>& m1); // [tested]

/// \brief Component-wise division
template <typename Type>
const wdMat3Template<Type> operator/(const wdMat3Template<Type>& m1, Type f); // [tested]

/// \brief Adding two matrices (component-wise)
template <typename Type>
const wdMat3Template<Type> operator+(const wdMat3Template<Type>& m1, const wdMat3Template<Type>& m2); // [tested]

/// \brief Subtracting two matrices (component-wise)
template <typename Type>
const wdMat3Template<Type> operator-(const wdMat3Template<Type>& m1, const wdMat3Template<Type>& m2); // [tested]

/// \brief Comparison Operator ==
template <typename Type>
bool operator==(const wdMat3Template<Type>& lhs, const wdMat3Template<Type>& rhs); // [tested]

/// \brief Comparison Operator !=
template <typename Type>
bool operator!=(const wdMat3Template<Type>& lhs, const wdMat3Template<Type>& rhs); // [tested]

#include <Foundation/Math/Implementation/Mat3_inl.h>
