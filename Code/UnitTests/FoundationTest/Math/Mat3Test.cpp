#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Math/Implementation/AllClasses_inl.h>
#include <Foundation/Math/Mat3.h>

WD_CREATE_SIMPLE_TEST(Math, Mat3)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Default Constructor")
  {
#if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
    if (wdMath::SupportsNaN<wdMathTestType>())
    {
      // In debug the default constructor initializes everything with NaN.
      wdMat3T m;
      WD_TEST_BOOL(wdMath::IsNaN(m.m_fElementsCM[0]) && wdMath::IsNaN(m.m_fElementsCM[1]) && wdMath::IsNaN(m.m_fElementsCM[2]) &&
                   wdMath::IsNaN(m.m_fElementsCM[3]) && wdMath::IsNaN(m.m_fElementsCM[4]) && wdMath::IsNaN(m.m_fElementsCM[5]) &&
                   wdMath::IsNaN(m.m_fElementsCM[6]) && wdMath::IsNaN(m.m_fElementsCM[7]) && wdMath::IsNaN(m.m_fElementsCM[8]));
    }
#else
    // Placement new of the default constructor should not have any effect on the previous data.
    wdMat3T::ComponentType testBlock[9] = {(wdMat3T::ComponentType)1, (wdMat3T::ComponentType)2, (wdMat3T::ComponentType)3, (wdMat3T::ComponentType)4,
      (wdMat3T::ComponentType)5, (wdMat3T::ComponentType)6, (wdMat3T::ComponentType)7, (wdMat3T::ComponentType)8, (wdMat3T::ComponentType)9};

    wdMat3T* m = ::new ((void*)&testBlock[0]) wdMat3T;

    WD_TEST_BOOL(m->m_fElementsCM[0] == (wdMat3T::ComponentType)1 && m->m_fElementsCM[1] == (wdMat3T::ComponentType)2 &&
                 m->m_fElementsCM[2] == (wdMat3T::ComponentType)3 && m->m_fElementsCM[3] == (wdMat3T::ComponentType)4 &&
                 m->m_fElementsCM[4] == (wdMat3T::ComponentType)5 && m->m_fElementsCM[5] == (wdMat3T::ComponentType)6 &&
                 m->m_fElementsCM[6] == (wdMat3T::ComponentType)7 && m->m_fElementsCM[7] == (wdMat3T::ComponentType)8 &&
                 m->m_fElementsCM[8] == (wdMat3T::ComponentType)9);
#endif
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor (Array Data)")
  {
    const wdMathTestType data[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    {
      wdMat3T m(data, wdMatrixLayout::ColumnMajor);

      WD_TEST_BOOL(m.m_fElementsCM[0] == 1.0f && m.m_fElementsCM[1] == 2.0f && m.m_fElementsCM[2] == 3.0f && m.m_fElementsCM[3] == 4.0f &&
                   m.m_fElementsCM[4] == 5.0f && m.m_fElementsCM[5] == 6.0f && m.m_fElementsCM[6] == 7.0f && m.m_fElementsCM[7] == 8.0f &&
                   m.m_fElementsCM[8] == 9.0f);
    }

    {
      wdMat3T m(data, wdMatrixLayout::RowMajor);

      WD_TEST_BOOL(m.m_fElementsCM[0] == 1.0f && m.m_fElementsCM[1] == 4.0f && m.m_fElementsCM[2] == 7.0f && m.m_fElementsCM[3] == 2.0f &&
                   m.m_fElementsCM[4] == 5.0f && m.m_fElementsCM[5] == 8.0f && m.m_fElementsCM[6] == 3.0f && m.m_fElementsCM[7] == 6.0f &&
                   m.m_fElementsCM[8] == 9.0f);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor (Elements)")
  {
    wdMat3T m(1, 2, 3, 4, 5, 6, 7, 8, 9);

    WD_TEST_FLOAT(m.Element(0, 0), 1, 0.00001f);
    WD_TEST_FLOAT(m.Element(1, 0), 2, 0.00001f);
    WD_TEST_FLOAT(m.Element(2, 0), 3, 0.00001f);
    WD_TEST_FLOAT(m.Element(0, 1), 4, 0.00001f);
    WD_TEST_FLOAT(m.Element(1, 1), 5, 0.00001f);
    WD_TEST_FLOAT(m.Element(2, 1), 6, 0.00001f);
    WD_TEST_FLOAT(m.Element(0, 2), 7, 0.00001f);
    WD_TEST_FLOAT(m.Element(1, 2), 8, 0.00001f);
    WD_TEST_FLOAT(m.Element(2, 2), 9, 0.00001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetFromArray")
  {
    const wdMathTestType data[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    {
      wdMat3T m;
      m.SetFromArray(data, wdMatrixLayout::ColumnMajor);

      WD_TEST_BOOL(m.m_fElementsCM[0] == 1.0f && m.m_fElementsCM[1] == 2.0f && m.m_fElementsCM[2] == 3.0f && m.m_fElementsCM[3] == 4.0f &&
                   m.m_fElementsCM[4] == 5.0f && m.m_fElementsCM[5] == 6.0f && m.m_fElementsCM[6] == 7.0f && m.m_fElementsCM[7] == 8.0f &&
                   m.m_fElementsCM[8] == 9.0f);
    }

    {
      wdMat3T m;
      m.SetFromArray(data, wdMatrixLayout::RowMajor);

      WD_TEST_BOOL(m.m_fElementsCM[0] == 1.0f && m.m_fElementsCM[1] == 4.0f && m.m_fElementsCM[2] == 7.0f && m.m_fElementsCM[3] == 2.0f &&
                   m.m_fElementsCM[4] == 5.0f && m.m_fElementsCM[5] == 8.0f && m.m_fElementsCM[6] == 3.0f && m.m_fElementsCM[7] == 6.0f &&
                   m.m_fElementsCM[8] == 9.0f);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetElements")
  {
    wdMat3T m;
    m.SetElements(1, 2, 3, 4, 5, 6, 7, 8, 9);

    WD_TEST_FLOAT(m.Element(0, 0), 1, 0.00001f);
    WD_TEST_FLOAT(m.Element(1, 0), 2, 0.00001f);
    WD_TEST_FLOAT(m.Element(2, 0), 3, 0.00001f);
    WD_TEST_FLOAT(m.Element(0, 1), 4, 0.00001f);
    WD_TEST_FLOAT(m.Element(1, 1), 5, 0.00001f);
    WD_TEST_FLOAT(m.Element(2, 1), 6, 0.00001f);
    WD_TEST_FLOAT(m.Element(0, 2), 7, 0.00001f);
    WD_TEST_FLOAT(m.Element(1, 2), 8, 0.00001f);
    WD_TEST_FLOAT(m.Element(2, 2), 9, 0.00001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetAsArray")
  {
    wdMat3T m(1, 2, 3, 4, 5, 6, 7, 8, 9);

    wdMathTestType data[9];

    m.GetAsArray(data, wdMatrixLayout::ColumnMajor);
    WD_TEST_FLOAT(data[0], 1, 0.0001f);
    WD_TEST_FLOAT(data[1], 4, 0.0001f);
    WD_TEST_FLOAT(data[2], 7, 0.0001f);
    WD_TEST_FLOAT(data[3], 2, 0.0001f);
    WD_TEST_FLOAT(data[4], 5, 0.0001f);
    WD_TEST_FLOAT(data[5], 8, 0.0001f);
    WD_TEST_FLOAT(data[6], 3, 0.0001f);
    WD_TEST_FLOAT(data[7], 6, 0.0001f);
    WD_TEST_FLOAT(data[8], 9, 0.0001f);

    m.GetAsArray(data, wdMatrixLayout::RowMajor);
    WD_TEST_FLOAT(data[0], 1, 0.0001f);
    WD_TEST_FLOAT(data[1], 2, 0.0001f);
    WD_TEST_FLOAT(data[2], 3, 0.0001f);
    WD_TEST_FLOAT(data[3], 4, 0.0001f);
    WD_TEST_FLOAT(data[4], 5, 0.0001f);
    WD_TEST_FLOAT(data[5], 6, 0.0001f);
    WD_TEST_FLOAT(data[6], 7, 0.0001f);
    WD_TEST_FLOAT(data[7], 8, 0.0001f);
    WD_TEST_FLOAT(data[8], 9, 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetZero")
  {
    wdMat3T m;
    m.SetZero();

    for (wdUInt32 i = 0; i < 9; ++i)
      WD_TEST_FLOAT(m.m_fElementsCM[i], 0.0f, 0.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetIdentity")
  {
    wdMat3T m;
    m.SetIdentity();

    WD_TEST_FLOAT(m.Element(0, 0), 1, 0);
    WD_TEST_FLOAT(m.Element(1, 0), 0, 0);
    WD_TEST_FLOAT(m.Element(2, 0), 0, 0);
    WD_TEST_FLOAT(m.Element(0, 1), 0, 0);
    WD_TEST_FLOAT(m.Element(1, 1), 1, 0);
    WD_TEST_FLOAT(m.Element(2, 1), 0, 0);
    WD_TEST_FLOAT(m.Element(0, 2), 0, 0);
    WD_TEST_FLOAT(m.Element(1, 2), 0, 0);
    WD_TEST_FLOAT(m.Element(2, 2), 1, 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetScalingMatrix")
  {
    wdMat3T m;
    m.SetScalingMatrix(wdVec3T(2, 3, 4));

    WD_TEST_FLOAT(m.Element(0, 0), 2, 0);
    WD_TEST_FLOAT(m.Element(1, 0), 0, 0);
    WD_TEST_FLOAT(m.Element(2, 0), 0, 0);
    WD_TEST_FLOAT(m.Element(0, 1), 0, 0);
    WD_TEST_FLOAT(m.Element(1, 1), 3, 0);
    WD_TEST_FLOAT(m.Element(2, 1), 0, 0);
    WD_TEST_FLOAT(m.Element(0, 2), 0, 0);
    WD_TEST_FLOAT(m.Element(1, 2), 0, 0);
    WD_TEST_FLOAT(m.Element(2, 2), 4, 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetRotationMatrixX")
  {
    wdMat3T m;

    m.SetRotationMatrixX(wdAngle::Degree(90));
    WD_TEST_BOOL((m * wdVec3T(1, 2, 3)).IsEqual(wdVec3T(1, -3, 2), 0.0001f));

    m.SetRotationMatrixX(wdAngle::Degree(180));
    WD_TEST_BOOL((m * wdVec3T(1, 2, 3)).IsEqual(wdVec3T(1, -2, -3), 0.0001f));

    m.SetRotationMatrixX(wdAngle::Degree(270));
    WD_TEST_BOOL((m * wdVec3T(1, 2, 3)).IsEqual(wdVec3T(1, 3, -2), 0.0001f));

    m.SetRotationMatrixX(wdAngle::Degree(360));
    WD_TEST_BOOL((m * wdVec3T(1, 2, 3)).IsEqual(wdVec3T(1, 2, 3), 0.0001f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetRotationMatrixY")
  {
    wdMat3T m;

    m.SetRotationMatrixY(wdAngle::Degree(90));
    WD_TEST_BOOL((m * wdVec3T(1, 2, 3)).IsEqual(wdVec3T(3, 2, -1), 0.0001f));

    m.SetRotationMatrixY(wdAngle::Degree(180));
    WD_TEST_BOOL((m * wdVec3T(1, 2, 3)).IsEqual(wdVec3T(-1, 2, -3), 0.0001f));

    m.SetRotationMatrixY(wdAngle::Degree(270));
    WD_TEST_BOOL((m * wdVec3T(1, 2, 3)).IsEqual(wdVec3T(-3, 2, 1), 0.0001f));

    m.SetRotationMatrixY(wdAngle::Degree(360));
    WD_TEST_BOOL((m * wdVec3T(1, 2, 3)).IsEqual(wdVec3T(1, 2, 3), 0.0001f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetRotationMatrixZ")
  {
    wdMat3T m;

    m.SetRotationMatrixZ(wdAngle::Degree(90));
    WD_TEST_BOOL((m * wdVec3T(1, 2, 3)).IsEqual(wdVec3T(-2, 1, 3), 0.0001f));

    m.SetRotationMatrixZ(wdAngle::Degree(180));
    WD_TEST_BOOL((m * wdVec3T(1, 2, 3)).IsEqual(wdVec3T(-1, -2, 3), 0.0001f));

    m.SetRotationMatrixZ(wdAngle::Degree(270));
    WD_TEST_BOOL((m * wdVec3T(1, 2, 3)).IsEqual(wdVec3T(2, -1, 3), 0.0001f));

    m.SetRotationMatrixZ(wdAngle::Degree(360));
    WD_TEST_BOOL((m * wdVec3T(1, 2, 3)).IsEqual(wdVec3T(1, 2, 3), 0.0001f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetRotationMatrix")
  {
    wdMat3T m;

    m.SetRotationMatrix(wdVec3T(1, 0, 0), wdAngle::Degree(90));
    WD_TEST_BOOL((m * wdVec3T(1, 2, 3)).IsEqual(wdVec3T(1, -3, 2), wdMath::LargeEpsilon<wdMathTestType>()));

    m.SetRotationMatrix(wdVec3T(1, 0, 0), wdAngle::Degree(180));
    WD_TEST_BOOL((m * wdVec3T(1, 2, 3)).IsEqual(wdVec3T(1, -2, -3), wdMath::LargeEpsilon<wdMathTestType>()));

    m.SetRotationMatrix(wdVec3T(1, 0, 0), wdAngle::Degree(270));
    WD_TEST_BOOL((m * wdVec3T(1, 2, 3)).IsEqual(wdVec3T(1, 3, -2), wdMath::LargeEpsilon<wdMathTestType>()));

    m.SetRotationMatrix(wdVec3T(0, 1, 0), wdAngle::Degree(90));
    WD_TEST_BOOL((m * wdVec3T(1, 2, 3)).IsEqual(wdVec3T(3, 2, -1), wdMath::LargeEpsilon<wdMathTestType>()));

    m.SetRotationMatrix(wdVec3T(0, 1, 0), wdAngle::Degree(180));
    WD_TEST_BOOL((m * wdVec3T(1, 2, 3)).IsEqual(wdVec3T(-1, 2, -3), wdMath::LargeEpsilon<wdMathTestType>()));

    m.SetRotationMatrix(wdVec3T(0, 1, 0), wdAngle::Degree(270));
    WD_TEST_BOOL((m * wdVec3T(1, 2, 3)).IsEqual(wdVec3T(-3, 2, 1), wdMath::LargeEpsilon<wdMathTestType>()));

    m.SetRotationMatrix(wdVec3T(0, 0, 1), wdAngle::Degree(90));
    WD_TEST_BOOL((m * wdVec3T(1, 2, 3)).IsEqual(wdVec3T(-2, 1, 3), wdMath::LargeEpsilon<wdMathTestType>()));

    m.SetRotationMatrix(wdVec3T(0, 0, 1), wdAngle::Degree(180));
    WD_TEST_BOOL((m * wdVec3T(1, 2, 3)).IsEqual(wdVec3T(-1, -2, 3), wdMath::LargeEpsilon<wdMathTestType>()));

    m.SetRotationMatrix(wdVec3T(0, 0, 1), wdAngle::Degree(270));
    WD_TEST_BOOL((m * wdVec3T(1, 2, 3)).IsEqual(wdVec3T(2, -1, 3), wdMath::LargeEpsilon<wdMathTestType>()));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IdentityMatrix")
  {
    wdMat3T m = wdMat3T::IdentityMatrix();

    WD_TEST_FLOAT(m.Element(0, 0), 1, 0);
    WD_TEST_FLOAT(m.Element(1, 0), 0, 0);
    WD_TEST_FLOAT(m.Element(2, 0), 0, 0);
    WD_TEST_FLOAT(m.Element(0, 1), 0, 0);
    WD_TEST_FLOAT(m.Element(1, 1), 1, 0);
    WD_TEST_FLOAT(m.Element(2, 1), 0, 0);
    WD_TEST_FLOAT(m.Element(0, 2), 0, 0);
    WD_TEST_FLOAT(m.Element(1, 2), 0, 0);
    WD_TEST_FLOAT(m.Element(2, 2), 1, 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ZeroMatrix")
  {
    wdMat3T m = wdMat3T::ZeroMatrix();

    WD_TEST_FLOAT(m.Element(0, 0), 0, 0);
    WD_TEST_FLOAT(m.Element(1, 0), 0, 0);
    WD_TEST_FLOAT(m.Element(2, 0), 0, 0);
    WD_TEST_FLOAT(m.Element(0, 1), 0, 0);
    WD_TEST_FLOAT(m.Element(1, 1), 0, 0);
    WD_TEST_FLOAT(m.Element(2, 1), 0, 0);
    WD_TEST_FLOAT(m.Element(0, 2), 0, 0);
    WD_TEST_FLOAT(m.Element(1, 2), 0, 0);
    WD_TEST_FLOAT(m.Element(2, 2), 0, 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Transpose")
  {
    wdMat3T m(1, 2, 3, 4, 5, 6, 7, 8, 9);

    m.Transpose();

    WD_TEST_FLOAT(m.Element(0, 0), 1, 0.00001f);
    WD_TEST_FLOAT(m.Element(1, 0), 4, 0.00001f);
    WD_TEST_FLOAT(m.Element(2, 0), 7, 0.00001f);
    WD_TEST_FLOAT(m.Element(0, 1), 2, 0.00001f);
    WD_TEST_FLOAT(m.Element(1, 1), 5, 0.00001f);
    WD_TEST_FLOAT(m.Element(2, 1), 8, 0.00001f);
    WD_TEST_FLOAT(m.Element(0, 2), 3, 0.00001f);
    WD_TEST_FLOAT(m.Element(1, 2), 6, 0.00001f);
    WD_TEST_FLOAT(m.Element(2, 2), 9, 0.00001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetTranspose")
  {
    wdMat3T m0(1, 2, 3, 4, 5, 6, 7, 8, 9);

    wdMat3T m = m0.GetTranspose();

    WD_TEST_FLOAT(m.Element(0, 0), 1, 0.00001f);
    WD_TEST_FLOAT(m.Element(1, 0), 4, 0.00001f);
    WD_TEST_FLOAT(m.Element(2, 0), 7, 0.00001f);
    WD_TEST_FLOAT(m.Element(0, 1), 2, 0.00001f);
    WD_TEST_FLOAT(m.Element(1, 1), 5, 0.00001f);
    WD_TEST_FLOAT(m.Element(2, 1), 8, 0.00001f);
    WD_TEST_FLOAT(m.Element(0, 2), 3, 0.00001f);
    WD_TEST_FLOAT(m.Element(1, 2), 6, 0.00001f);
    WD_TEST_FLOAT(m.Element(2, 2), 9, 0.00001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Invert")
  {
    for (float x = 1.0f; x < 360.0f; x += 10.0f)
    {
      for (float y = 2.0f; y < 360.0f; y += 17.0f)
      {
        for (float z = 3.0f; z < 360.0f; z += 23.0f)
        {
          wdMat3T m, inv;
          m.SetRotationMatrix(wdVec3T(x, y, z).GetNormalized(), wdAngle::Degree(19.0f));
          inv = m;
          WD_TEST_BOOL(inv.Invert() == WD_SUCCESS);

          wdVec3T v = m * wdVec3T(1, 1, 1);
          wdVec3T vinv = inv * v;

          WD_TEST_VEC3(vinv, wdVec3T(1, 1, 1), wdMath::DefaultEpsilon<wdMathTestType>());
        }
      }
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetInverse")
  {
    for (float x = 1.0f; x < 360.0f; x += 9.0f)
    {
      for (float y = 2.0f; y < 360.0f; y += 19.0f)
      {
        for (float z = 3.0f; z < 360.0f; z += 21.0f)
        {
          wdMat3T m, inv;
          m.SetRotationMatrix(wdVec3T(x, y, z).GetNormalized(), wdAngle::Degree(83.0f));
          inv = m.GetInverse();

          wdVec3T v = m * wdVec3T(1, 1, 1);
          wdVec3T vinv = inv * v;

          WD_TEST_VEC3(vinv, wdVec3T(1, 1, 1), wdMath::DefaultEpsilon<wdMathTestType>());
        }
      }
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsZero")
  {
    wdMat3T m;

    m.SetIdentity();
    WD_TEST_BOOL(!m.IsZero());

    m.SetZero();
    WD_TEST_BOOL(m.IsZero());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsIdentity")
  {
    wdMat3T m;

    m.SetIdentity();
    WD_TEST_BOOL(m.IsIdentity());

    m.SetZero();
    WD_TEST_BOOL(!m.IsIdentity());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsValid")
  {
    if (wdMath::SupportsNaN<wdMat3T::ComponentType>())
    {
      wdMat3T m;

      m.SetZero();
      WD_TEST_BOOL(m.IsValid());

      m.m_fElementsCM[0] = wdMath::NaN<wdMat3T::ComponentType>();
      WD_TEST_BOOL(!m.IsValid());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetRow")
  {
    wdMat3T m(1, 2, 3, 4, 5, 6, 7, 8, 9);

    WD_TEST_VEC3(m.GetRow(0), wdVec3T(1, 2, 3), 0.0f);
    WD_TEST_VEC3(m.GetRow(1), wdVec3T(4, 5, 6), 0.0f);
    WD_TEST_VEC3(m.GetRow(2), wdVec3T(7, 8, 9), 0.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetRow")
  {
    wdMat3T m;
    m.SetZero();

    m.SetRow(0, wdVec3T(1, 2, 3));
    WD_TEST_VEC3(m.GetRow(0), wdVec3T(1, 2, 3), 0.0f);

    m.SetRow(1, wdVec3T(4, 5, 6));
    WD_TEST_VEC3(m.GetRow(1), wdVec3T(4, 5, 6), 0.0f);

    m.SetRow(2, wdVec3T(7, 8, 9));
    WD_TEST_VEC3(m.GetRow(2), wdVec3T(7, 8, 9), 0.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetColumn")
  {
    wdMat3T m(1, 2, 3, 4, 5, 6, 7, 8, 9);

    WD_TEST_VEC3(m.GetColumn(0), wdVec3T(1, 4, 7), 0.0f);
    WD_TEST_VEC3(m.GetColumn(1), wdVec3T(2, 5, 8), 0.0f);
    WD_TEST_VEC3(m.GetColumn(2), wdVec3T(3, 6, 9), 0.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetColumn")
  {
    wdMat3T m;
    m.SetZero();

    m.SetColumn(0, wdVec3T(1, 2, 3));
    WD_TEST_VEC3(m.GetColumn(0), wdVec3T(1, 2, 3), 0.0f);

    m.SetColumn(1, wdVec3T(4, 5, 6));
    WD_TEST_VEC3(m.GetColumn(1), wdVec3T(4, 5, 6), 0.0f);

    m.SetColumn(2, wdVec3T(7, 8, 9));
    WD_TEST_VEC3(m.GetColumn(2), wdVec3T(7, 8, 9), 0.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetDiagonal")
  {
    wdMat3T m(1, 2, 3, 4, 5, 6, 7, 8, 9);

    WD_TEST_VEC3(m.GetDiagonal(), wdVec3T(1, 5, 9), 0.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetDiagonal")
  {
    wdMat3T m;
    m.SetZero();

    m.SetDiagonal(wdVec3T(1, 2, 3));
    WD_TEST_VEC3(m.GetColumn(0), wdVec3T(1, 0, 0), 0.0f);
    WD_TEST_VEC3(m.GetColumn(1), wdVec3T(0, 2, 0), 0.0f);
    WD_TEST_VEC3(m.GetColumn(2), wdVec3T(0, 0, 3), 0.0f);
  }


  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetScalingFactors")
  {
    wdMat3T m(1, 2, 3, 5, 6, 7, 9, 10, 11);

    wdVec3T s = m.GetScalingFactors();
    WD_TEST_VEC3(s,
      wdVec3T(wdMath::Sqrt((wdMathTestType)(1 * 1 + 5 * 5 + 9 * 9)), wdMath::Sqrt((wdMathTestType)(2 * 2 + 6 * 6 + 10 * 10)),
        wdMath::Sqrt((wdMathTestType)(3 * 3 + 7 * 7 + 11 * 11))),
      0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetScalingFactors")
  {
    wdMat3T m(1, 2, 3, 5, 6, 7, 9, 10, 11);

    WD_TEST_BOOL(m.SetScalingFactors(wdVec3T(1, 2, 3)) == WD_SUCCESS);

    wdVec3T s = m.GetScalingFactors();
    WD_TEST_VEC3(s, wdVec3T(1, 2, 3), 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "TransformDirection")
  {
    wdMat3T m(1, 2, 3, 4, 5, 6, 7, 8, 9);

    const wdVec3T r = m.TransformDirection(wdVec3T(1, 2, 3));

    WD_TEST_VEC3(r, wdVec3T(1 * 1 + 2 * 2 + 3 * 3, 1 * 4 + 2 * 5 + 3 * 6, 1 * 7 + 2 * 8 + 3 * 9), 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator*=")
  {
    wdMat3T m(1, 2, 3, 4, 5, 6, 7, 8, 9);

    m *= 2.0f;

    WD_TEST_VEC3(m.GetRow(0), wdVec3T(2, 4, 6), 0.0001f);
    WD_TEST_VEC3(m.GetRow(1), wdVec3T(8, 10, 12), 0.0001f);
    WD_TEST_VEC3(m.GetRow(2), wdVec3T(14, 16, 18), 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator/=")
  {
    wdMat3T m(1, 2, 3, 4, 5, 6, 7, 8, 9);

    m *= 4.0f;
    m /= 2.0f;

    WD_TEST_VEC3(m.GetRow(0), wdVec3T(2, 4, 6), 0.0001f);
    WD_TEST_VEC3(m.GetRow(1), wdVec3T(8, 10, 12), 0.0001f);
    WD_TEST_VEC3(m.GetRow(2), wdVec3T(14, 16, 18), 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsIdentical")
  {
    wdMat3T m(1, 2, 3, 4, 5, 6, 7, 8, 9);

    wdMat3T m2 = m;

    WD_TEST_BOOL(m.IsIdentical(m2));

    m2.m_fElementsCM[0] += 0.00001f;
    WD_TEST_BOOL(!m.IsIdentical(m2));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsEqual")
  {
    wdMat3T m(1, 2, 3, 4, 5, 6, 7, 8, 9);

    wdMat3T m2 = m;

    WD_TEST_BOOL(m.IsEqual(m2, 0.0001f));

    m2.m_fElementsCM[0] += 0.00001f;
    WD_TEST_BOOL(m.IsEqual(m2, 0.0001f));
    WD_TEST_BOOL(!m.IsEqual(m2, 0.000001f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator*(mat, mat)")
  {
    wdMat3T m1(1, 2, 3, 4, 5, 6, 7, 8, 9);

    wdMat3T m2(-1, -2, -3, -4, -5, -6, -7, -8, -9);

    wdMat3T r = m1 * m2;

    WD_TEST_VEC3(r.GetColumn(0), wdVec3T(-1 * 1 + -4 * 2 + -7 * 3, -1 * 4 + -4 * 5 + -7 * 6, -1 * 7 + -4 * 8 + -7 * 9), 0.001f);
    WD_TEST_VEC3(r.GetColumn(1), wdVec3T(-2 * 1 + -5 * 2 + -8 * 3, -2 * 4 + -5 * 5 + -8 * 6, -2 * 7 + -5 * 8 + -8 * 9), 0.001f);
    WD_TEST_VEC3(r.GetColumn(2), wdVec3T(-3 * 1 + -6 * 2 + -9 * 3, -3 * 4 + -6 * 5 + -9 * 6, -3 * 7 + -6 * 8 + -9 * 9), 0.001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator*(mat, vec)")
  {
    wdMat3T m(1, 2, 3, 4, 5, 6, 7, 8, 9);

    const wdVec3T r = m * (wdVec3T(1, 2, 3));

    WD_TEST_VEC3(r, wdVec3T(1 * 1 + 2 * 2 + 3 * 3, 1 * 4 + 2 * 5 + 3 * 6, 1 * 7 + 2 * 8 + 3 * 9), 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator*(mat, float) | operator*(float, mat)")
  {
    wdMat3T m0(1, 2, 3, 4, 5, 6, 7, 8, 9);

    wdMat3T m = m0 * (wdMathTestType)2;
    wdMat3T m2 = (wdMathTestType)2 * m0;

    WD_TEST_VEC3(m.GetRow(0), wdVec3T(2, 4, 6), 0.0001f);
    WD_TEST_VEC3(m.GetRow(1), wdVec3T(8, 10, 12), 0.0001f);
    WD_TEST_VEC3(m.GetRow(2), wdVec3T(14, 16, 18), 0.0001f);

    WD_TEST_VEC3(m2.GetRow(0), wdVec3T(2, 4, 6), 0.0001f);
    WD_TEST_VEC3(m2.GetRow(1), wdVec3T(8, 10, 12), 0.0001f);
    WD_TEST_VEC3(m2.GetRow(2), wdVec3T(14, 16, 18), 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator/(mat, float)")
  {
    wdMat3T m0(1, 2, 3, 4, 5, 6, 7, 8, 9);

    m0 *= 4.0f;

    wdMat3T m = m0 / (wdMathTestType)2;

    WD_TEST_VEC3(m.GetRow(0), wdVec3T(2, 4, 6), 0.0001f);
    WD_TEST_VEC3(m.GetRow(1), wdVec3T(8, 10, 12), 0.0001f);
    WD_TEST_VEC3(m.GetRow(2), wdVec3T(14, 16, 18), 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator+(mat, mat) | operator-(mat, mat)")
  {
    wdMat3T m0(1, 2, 3, 4, 5, 6, 7, 8, 9);

    wdMat3T m1(-1, -2, -3, -4, -5, -6, -7, -8, -9);

    WD_TEST_BOOL((m0 + m1).IsZero());
    WD_TEST_BOOL((m0 - m1).IsEqual(m0 * (wdMathTestType)2, 0.0001f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator== (mat, mat) | operator!= (mat, mat)")
  {
    wdMat3T m(1, 2, 3, 4, 5, 6, 7, 8, 9);

    wdMat3T m2 = m;

    WD_TEST_BOOL(m == m2);

    m2.m_fElementsCM[0] += 0.00001f;

    WD_TEST_BOOL(m != m2);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsNaN")
  {
    if (wdMath::SupportsNaN<wdMathTestType>())
    {
      wdMat3T m;

      m.SetIdentity();
      WD_TEST_BOOL(!m.IsNaN());

      for (wdUInt32 i = 0; i < 9; ++i)
      {
        m.SetIdentity();
        m.m_fElementsCM[i] = wdMath::NaN<wdMathTestType>();

        WD_TEST_BOOL(m.IsNaN());
      }
    }
  }
}
