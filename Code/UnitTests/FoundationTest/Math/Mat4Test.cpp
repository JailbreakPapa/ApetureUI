#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Math/Implementation/AllClasses_inl.h>
#include <Foundation/Math/Mat4.h>

WD_CREATE_SIMPLE_TEST(Math, Mat4)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Default Constructor")
  {
#if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
    if (wdMath::SupportsNaN<wdMat3T::ComponentType>())
    {
      // In debug the default constructor initializes everything with NaN.
      wdMat4T m;
      WD_TEST_BOOL(wdMath::IsNaN(m.m_fElementsCM[0]) && wdMath::IsNaN(m.m_fElementsCM[1]) && wdMath::IsNaN(m.m_fElementsCM[2]) &&
                   wdMath::IsNaN(m.m_fElementsCM[3]) && wdMath::IsNaN(m.m_fElementsCM[4]) && wdMath::IsNaN(m.m_fElementsCM[5]) &&
                   wdMath::IsNaN(m.m_fElementsCM[6]) && wdMath::IsNaN(m.m_fElementsCM[7]) && wdMath::IsNaN(m.m_fElementsCM[8]) &&
                   wdMath::IsNaN(m.m_fElementsCM[9]) && wdMath::IsNaN(m.m_fElementsCM[10]) && wdMath::IsNaN(m.m_fElementsCM[11]) &&
                   wdMath::IsNaN(m.m_fElementsCM[12]) && wdMath::IsNaN(m.m_fElementsCM[13]) && wdMath::IsNaN(m.m_fElementsCM[14]) &&
                   wdMath::IsNaN(m.m_fElementsCM[15]));
    }
#else
    // Placement new of the default constructor should not have any effect on the previous data.
    wdMat4T::ComponentType testBlock[16] = {(wdMat4T::ComponentType)1, (wdMat4T::ComponentType)2, (wdMat4T::ComponentType)3,
      (wdMat4T::ComponentType)4, (wdMat4T::ComponentType)5, (wdMat4T::ComponentType)6, (wdMat4T::ComponentType)7, (wdMat4T::ComponentType)8,
      (wdMat4T::ComponentType)9, (wdMat4T::ComponentType)10, (wdMat4T::ComponentType)11, (wdMat4T::ComponentType)12, (wdMat4T::ComponentType)13,
      (wdMat4T::ComponentType)14, (wdMat4T::ComponentType)15, (wdMat4T::ComponentType)16};
    wdMat4T* m = ::new ((void*)&testBlock[0]) wdMat4T;

    WD_TEST_BOOL(m->m_fElementsCM[0] == 1.0f && m->m_fElementsCM[1] == 2.0f && m->m_fElementsCM[2] == 3.0f && m->m_fElementsCM[3] == 4.0f &&
                 m->m_fElementsCM[4] == 5.0f && m->m_fElementsCM[5] == 6.0f && m->m_fElementsCM[6] == 7.0f && m->m_fElementsCM[7] == 8.0f &&
                 m->m_fElementsCM[8] == 9.0f && m->m_fElementsCM[9] == 10.0f && m->m_fElementsCM[10] == 11.0f && m->m_fElementsCM[11] == 12.0f &&
                 m->m_fElementsCM[12] == 13.0f && m->m_fElementsCM[13] == 14.0f && m->m_fElementsCM[14] == 15.0f && m->m_fElementsCM[15] == 16.0f);
#endif
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor (Array Data)")
  {
    const wdMathTestType data[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

    {
      wdMat4T m(data, wdMatrixLayout::ColumnMajor);

      WD_TEST_BOOL(m.m_fElementsCM[0] == 1.0f && m.m_fElementsCM[1] == 2.0f && m.m_fElementsCM[2] == 3.0f && m.m_fElementsCM[3] == 4.0f &&
                   m.m_fElementsCM[4] == 5.0f && m.m_fElementsCM[5] == 6.0f && m.m_fElementsCM[6] == 7.0f && m.m_fElementsCM[7] == 8.0f &&
                   m.m_fElementsCM[8] == 9.0f && m.m_fElementsCM[9] == 10.0f && m.m_fElementsCM[10] == 11.0f && m.m_fElementsCM[11] == 12.0f &&
                   m.m_fElementsCM[12] == 13.0f && m.m_fElementsCM[13] == 14.0f && m.m_fElementsCM[14] == 15.0f && m.m_fElementsCM[15] == 16.0f);
    }

    {
      wdMat4T m(data, wdMatrixLayout::RowMajor);

      WD_TEST_BOOL(m.m_fElementsCM[0] == 1.0f && m.m_fElementsCM[1] == 5.0f && m.m_fElementsCM[2] == 9.0f && m.m_fElementsCM[3] == 13.0f &&
                   m.m_fElementsCM[4] == 2.0f && m.m_fElementsCM[5] == 6.0f && m.m_fElementsCM[6] == 10.0f && m.m_fElementsCM[7] == 14.0f &&
                   m.m_fElementsCM[8] == 3.0f && m.m_fElementsCM[9] == 7.0f && m.m_fElementsCM[10] == 11.0f && m.m_fElementsCM[11] == 15.0f &&
                   m.m_fElementsCM[12] == 4.0f && m.m_fElementsCM[13] == 8.0f && m.m_fElementsCM[14] == 12.0f && m.m_fElementsCM[15] == 16.0f);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor (Elements)")
  {
    wdMat4T m(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    WD_TEST_FLOAT(m.Element(0, 0), 1, 0.00001f);
    WD_TEST_FLOAT(m.Element(1, 0), 2, 0.00001f);
    WD_TEST_FLOAT(m.Element(2, 0), 3, 0.00001f);
    WD_TEST_FLOAT(m.Element(3, 0), 4, 0.00001f);
    WD_TEST_FLOAT(m.Element(0, 1), 5, 0.00001f);
    WD_TEST_FLOAT(m.Element(1, 1), 6, 0.00001f);
    WD_TEST_FLOAT(m.Element(2, 1), 7, 0.00001f);
    WD_TEST_FLOAT(m.Element(3, 1), 8, 0.00001f);
    WD_TEST_FLOAT(m.Element(0, 2), 9, 0.00001f);
    WD_TEST_FLOAT(m.Element(1, 2), 10, 0.00001f);
    WD_TEST_FLOAT(m.Element(2, 2), 11, 0.00001f);
    WD_TEST_FLOAT(m.Element(3, 2), 12, 0.00001f);
    WD_TEST_FLOAT(m.Element(0, 3), 13, 0.00001f);
    WD_TEST_FLOAT(m.Element(1, 3), 14, 0.00001f);
    WD_TEST_FLOAT(m.Element(2, 3), 15, 0.00001f);
    WD_TEST_FLOAT(m.Element(3, 3), 16, 0.00001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor (composite)")
  {
    wdMat3T mr(1, 2, 3, 4, 5, 6, 7, 8, 9);
    wdVec3T vt(10, 11, 12);

    wdMat4T m(mr, vt);

    WD_TEST_FLOAT(m.Element(0, 0), 1, 0);
    WD_TEST_FLOAT(m.Element(1, 0), 2, 0);
    WD_TEST_FLOAT(m.Element(2, 0), 3, 0);
    WD_TEST_FLOAT(m.Element(3, 0), 10, 0);
    WD_TEST_FLOAT(m.Element(0, 1), 4, 0);
    WD_TEST_FLOAT(m.Element(1, 1), 5, 0);
    WD_TEST_FLOAT(m.Element(2, 1), 6, 0);
    WD_TEST_FLOAT(m.Element(3, 1), 11, 0);
    WD_TEST_FLOAT(m.Element(0, 2), 7, 0);
    WD_TEST_FLOAT(m.Element(1, 2), 8, 0);
    WD_TEST_FLOAT(m.Element(2, 2), 9, 0);
    WD_TEST_FLOAT(m.Element(3, 2), 12, 0);
    WD_TEST_FLOAT(m.Element(0, 3), 0, 0);
    WD_TEST_FLOAT(m.Element(1, 3), 0, 0);
    WD_TEST_FLOAT(m.Element(2, 3), 0, 0);
    WD_TEST_FLOAT(m.Element(3, 3), 1, 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetFromArray")
  {
    const wdMathTestType data[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

    {
      wdMat4T m;
      m.SetFromArray(data, wdMatrixLayout::ColumnMajor);

      WD_TEST_BOOL(m.m_fElementsCM[0] == 1.0f && m.m_fElementsCM[1] == 2.0f && m.m_fElementsCM[2] == 3.0f && m.m_fElementsCM[3] == 4.0f &&
                   m.m_fElementsCM[4] == 5.0f && m.m_fElementsCM[5] == 6.0f && m.m_fElementsCM[6] == 7.0f && m.m_fElementsCM[7] == 8.0f &&
                   m.m_fElementsCM[8] == 9.0f && m.m_fElementsCM[9] == 10.0f && m.m_fElementsCM[10] == 11.0f && m.m_fElementsCM[11] == 12.0f &&
                   m.m_fElementsCM[12] == 13.0f && m.m_fElementsCM[13] == 14.0f && m.m_fElementsCM[14] == 15.0f && m.m_fElementsCM[15] == 16.0f);
    }

    {
      wdMat4T m;
      m.SetFromArray(data, wdMatrixLayout::RowMajor);

      WD_TEST_BOOL(m.m_fElementsCM[0] == 1.0f && m.m_fElementsCM[1] == 5.0f && m.m_fElementsCM[2] == 9.0f && m.m_fElementsCM[3] == 13.0f &&
                   m.m_fElementsCM[4] == 2.0f && m.m_fElementsCM[5] == 6.0f && m.m_fElementsCM[6] == 10.0f && m.m_fElementsCM[7] == 14.0f &&
                   m.m_fElementsCM[8] == 3.0f && m.m_fElementsCM[9] == 7.0f && m.m_fElementsCM[10] == 11.0f && m.m_fElementsCM[11] == 15.0f &&
                   m.m_fElementsCM[12] == 4.0f && m.m_fElementsCM[13] == 8.0f && m.m_fElementsCM[14] == 12.0f && m.m_fElementsCM[15] == 16.0f);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetElements")
  {
    wdMat4T m;
    m.SetElements(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    WD_TEST_FLOAT(m.Element(0, 0), 1, 0.00001f);
    WD_TEST_FLOAT(m.Element(1, 0), 2, 0.00001f);
    WD_TEST_FLOAT(m.Element(2, 0), 3, 0.00001f);
    WD_TEST_FLOAT(m.Element(3, 0), 4, 0.00001f);
    WD_TEST_FLOAT(m.Element(0, 1), 5, 0.00001f);
    WD_TEST_FLOAT(m.Element(1, 1), 6, 0.00001f);
    WD_TEST_FLOAT(m.Element(2, 1), 7, 0.00001f);
    WD_TEST_FLOAT(m.Element(3, 1), 8, 0.00001f);
    WD_TEST_FLOAT(m.Element(0, 2), 9, 0.00001f);
    WD_TEST_FLOAT(m.Element(1, 2), 10, 0.00001f);
    WD_TEST_FLOAT(m.Element(2, 2), 11, 0.00001f);
    WD_TEST_FLOAT(m.Element(3, 2), 12, 0.00001f);
    WD_TEST_FLOAT(m.Element(0, 3), 13, 0.00001f);
    WD_TEST_FLOAT(m.Element(1, 3), 14, 0.00001f);
    WD_TEST_FLOAT(m.Element(2, 3), 15, 0.00001f);
    WD_TEST_FLOAT(m.Element(3, 3), 16, 0.00001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetTransformationMatrix")
  {
    wdMat3T mr(1, 2, 3, 4, 5, 6, 7, 8, 9);
    wdVec3T vt(10, 11, 12);

    wdMat4T m;
    m.SetTransformationMatrix(mr, vt);

    WD_TEST_FLOAT(m.Element(0, 0), 1, 0);
    WD_TEST_FLOAT(m.Element(1, 0), 2, 0);
    WD_TEST_FLOAT(m.Element(2, 0), 3, 0);
    WD_TEST_FLOAT(m.Element(3, 0), 10, 0);
    WD_TEST_FLOAT(m.Element(0, 1), 4, 0);
    WD_TEST_FLOAT(m.Element(1, 1), 5, 0);
    WD_TEST_FLOAT(m.Element(2, 1), 6, 0);
    WD_TEST_FLOAT(m.Element(3, 1), 11, 0);
    WD_TEST_FLOAT(m.Element(0, 2), 7, 0);
    WD_TEST_FLOAT(m.Element(1, 2), 8, 0);
    WD_TEST_FLOAT(m.Element(2, 2), 9, 0);
    WD_TEST_FLOAT(m.Element(3, 2), 12, 0);
    WD_TEST_FLOAT(m.Element(0, 3), 0, 0);
    WD_TEST_FLOAT(m.Element(1, 3), 0, 0);
    WD_TEST_FLOAT(m.Element(2, 3), 0, 0);
    WD_TEST_FLOAT(m.Element(3, 3), 1, 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetAsArray")
  {
    wdMat4T m(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    wdMathTestType data[16];

    m.GetAsArray(data, wdMatrixLayout::ColumnMajor);
    WD_TEST_FLOAT(data[0], 1, 0.0001f);
    WD_TEST_FLOAT(data[1], 5, 0.0001f);
    WD_TEST_FLOAT(data[2], 9, 0.0001f);
    WD_TEST_FLOAT(data[3], 13, 0.0001f);
    WD_TEST_FLOAT(data[4], 2, 0.0001f);
    WD_TEST_FLOAT(data[5], 6, 0.0001f);
    WD_TEST_FLOAT(data[6], 10, 0.0001f);
    WD_TEST_FLOAT(data[7], 14, 0.0001f);
    WD_TEST_FLOAT(data[8], 3, 0.0001f);
    WD_TEST_FLOAT(data[9], 7, 0.0001f);
    WD_TEST_FLOAT(data[10], 11, 0.0001f);
    WD_TEST_FLOAT(data[11], 15, 0.0001f);
    WD_TEST_FLOAT(data[12], 4, 0.0001f);
    WD_TEST_FLOAT(data[13], 8, 0.0001f);
    WD_TEST_FLOAT(data[14], 12, 0.0001f);
    WD_TEST_FLOAT(data[15], 16, 0.0001f);

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
    WD_TEST_FLOAT(data[9], 10, 0.0001f);
    WD_TEST_FLOAT(data[10], 11, 0.0001f);
    WD_TEST_FLOAT(data[11], 12, 0.0001f);
    WD_TEST_FLOAT(data[12], 13, 0.0001f);
    WD_TEST_FLOAT(data[13], 14, 0.0001f);
    WD_TEST_FLOAT(data[14], 15, 0.0001f);
    WD_TEST_FLOAT(data[15], 16, 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetZero")
  {
    wdMat4T m;
    m.SetZero();

    for (wdUInt32 i = 0; i < 16; ++i)
      WD_TEST_FLOAT(m.m_fElementsCM[i], 0.0f, 0.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetIdentity")
  {
    wdMat4T m;
    m.SetIdentity();

    WD_TEST_FLOAT(m.Element(0, 0), 1, 0);
    WD_TEST_FLOAT(m.Element(1, 0), 0, 0);
    WD_TEST_FLOAT(m.Element(2, 0), 0, 0);
    WD_TEST_FLOAT(m.Element(3, 0), 0, 0);
    WD_TEST_FLOAT(m.Element(0, 1), 0, 0);
    WD_TEST_FLOAT(m.Element(1, 1), 1, 0);
    WD_TEST_FLOAT(m.Element(2, 1), 0, 0);
    WD_TEST_FLOAT(m.Element(3, 1), 0, 0);
    WD_TEST_FLOAT(m.Element(0, 2), 0, 0);
    WD_TEST_FLOAT(m.Element(1, 2), 0, 0);
    WD_TEST_FLOAT(m.Element(2, 2), 1, 0);
    WD_TEST_FLOAT(m.Element(3, 2), 0, 0);
    WD_TEST_FLOAT(m.Element(0, 3), 0, 0);
    WD_TEST_FLOAT(m.Element(1, 3), 0, 0);
    WD_TEST_FLOAT(m.Element(2, 3), 0, 0);
    WD_TEST_FLOAT(m.Element(3, 3), 1, 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetTranslationMatrix")
  {
    wdMat4T m;
    m.SetTranslationMatrix(wdVec3T(2, 3, 4));

    WD_TEST_FLOAT(m.Element(0, 0), 1, 0);
    WD_TEST_FLOAT(m.Element(1, 0), 0, 0);
    WD_TEST_FLOAT(m.Element(2, 0), 0, 0);
    WD_TEST_FLOAT(m.Element(3, 0), 2, 0);
    WD_TEST_FLOAT(m.Element(0, 1), 0, 0);
    WD_TEST_FLOAT(m.Element(1, 1), 1, 0);
    WD_TEST_FLOAT(m.Element(2, 1), 0, 0);
    WD_TEST_FLOAT(m.Element(3, 1), 3, 0);
    WD_TEST_FLOAT(m.Element(0, 2), 0, 0);
    WD_TEST_FLOAT(m.Element(1, 2), 0, 0);
    WD_TEST_FLOAT(m.Element(2, 2), 1, 0);
    WD_TEST_FLOAT(m.Element(3, 2), 4, 0);
    WD_TEST_FLOAT(m.Element(0, 3), 0, 0);
    WD_TEST_FLOAT(m.Element(1, 3), 0, 0);
    WD_TEST_FLOAT(m.Element(2, 3), 0, 0);
    WD_TEST_FLOAT(m.Element(3, 3), 1, 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetScalingMatrix")
  {
    wdMat4T m;
    m.SetScalingMatrix(wdVec3T(2, 3, 4));

    WD_TEST_FLOAT(m.Element(0, 0), 2, 0);
    WD_TEST_FLOAT(m.Element(1, 0), 0, 0);
    WD_TEST_FLOAT(m.Element(2, 0), 0, 0);
    WD_TEST_FLOAT(m.Element(3, 0), 0, 0);
    WD_TEST_FLOAT(m.Element(0, 1), 0, 0);
    WD_TEST_FLOAT(m.Element(1, 1), 3, 0);
    WD_TEST_FLOAT(m.Element(2, 1), 0, 0);
    WD_TEST_FLOAT(m.Element(3, 1), 0, 0);
    WD_TEST_FLOAT(m.Element(0, 2), 0, 0);
    WD_TEST_FLOAT(m.Element(1, 2), 0, 0);
    WD_TEST_FLOAT(m.Element(2, 2), 4, 0);
    WD_TEST_FLOAT(m.Element(3, 2), 0, 0);
    WD_TEST_FLOAT(m.Element(0, 3), 0, 0);
    WD_TEST_FLOAT(m.Element(1, 3), 0, 0);
    WD_TEST_FLOAT(m.Element(2, 3), 0, 0);
    WD_TEST_FLOAT(m.Element(3, 3), 1, 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetRotationMatrixX")
  {
    wdMat4T m;

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
    wdMat4T m;

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
    wdMat4T m;

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
    wdMat4T m;

    m.SetRotationMatrix(wdVec3T(1, 0, 0), wdAngle::Degree(90));
    WD_TEST_BOOL((m * wdVec3T(1, 2, 3)).IsEqual(wdVec3T(1, -3, 2), wdMath::DefaultEpsilon<wdMat3T::ComponentType>()));

    m.SetRotationMatrix(wdVec3T(1, 0, 0), wdAngle::Degree(180));
    WD_TEST_BOOL((m * wdVec3T(1, 2, 3)).IsEqual(wdVec3T(1, -2, -3), wdMath::DefaultEpsilon<wdMat3T::ComponentType>()));

    m.SetRotationMatrix(wdVec3T(1, 0, 0), wdAngle::Degree(270));
    WD_TEST_BOOL((m * wdVec3T(1, 2, 3)).IsEqual(wdVec3T(1, 3, -2), wdMath::DefaultEpsilon<wdMat3T::ComponentType>()));

    m.SetRotationMatrix(wdVec3T(0, 1, 0), wdAngle::Degree(90));
    WD_TEST_BOOL((m * wdVec3T(1, 2, 3)).IsEqual(wdVec3T(3, 2, -1), wdMath::DefaultEpsilon<wdMat3T::ComponentType>()));

    m.SetRotationMatrix(wdVec3T(0, 1, 0), wdAngle::Degree(180));
    WD_TEST_BOOL((m * wdVec3T(1, 2, 3)).IsEqual(wdVec3T(-1, 2, -3), wdMath::DefaultEpsilon<wdMat3T::ComponentType>()));

    m.SetRotationMatrix(wdVec3T(0, 1, 0), wdAngle::Degree(270));
    WD_TEST_BOOL((m * wdVec3T(1, 2, 3)).IsEqual(wdVec3T(-3, 2, 1), wdMath::DefaultEpsilon<wdMat3T::ComponentType>()));

    m.SetRotationMatrix(wdVec3T(0, 0, 1), wdAngle::Degree(90));
    WD_TEST_BOOL((m * wdVec3T(1, 2, 3)).IsEqual(wdVec3T(-2, 1, 3), wdMath::DefaultEpsilon<wdMat3T::ComponentType>()));

    m.SetRotationMatrix(wdVec3T(0, 0, 1), wdAngle::Degree(180));
    WD_TEST_BOOL((m * wdVec3T(1, 2, 3)).IsEqual(wdVec3T(-1, -2, 3), wdMath::DefaultEpsilon<wdMat3T::ComponentType>()));

    m.SetRotationMatrix(wdVec3T(0, 0, 1), wdAngle::Degree(270));
    WD_TEST_BOOL((m * wdVec3T(1, 2, 3)).IsEqual(wdVec3T(2, -1, 3), wdMath::DefaultEpsilon<wdMat3T::ComponentType>()));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IdentityMatrix")
  {
    wdMat4T m = wdMat4T::IdentityMatrix();

    WD_TEST_FLOAT(m.Element(0, 0), 1, 0);
    WD_TEST_FLOAT(m.Element(1, 0), 0, 0);
    WD_TEST_FLOAT(m.Element(2, 0), 0, 0);
    WD_TEST_FLOAT(m.Element(3, 0), 0, 0);
    WD_TEST_FLOAT(m.Element(0, 1), 0, 0);
    WD_TEST_FLOAT(m.Element(1, 1), 1, 0);
    WD_TEST_FLOAT(m.Element(2, 1), 0, 0);
    WD_TEST_FLOAT(m.Element(3, 1), 0, 0);
    WD_TEST_FLOAT(m.Element(0, 2), 0, 0);
    WD_TEST_FLOAT(m.Element(1, 2), 0, 0);
    WD_TEST_FLOAT(m.Element(2, 2), 1, 0);
    WD_TEST_FLOAT(m.Element(3, 2), 0, 0);
    WD_TEST_FLOAT(m.Element(0, 3), 0, 0);
    WD_TEST_FLOAT(m.Element(1, 3), 0, 0);
    WD_TEST_FLOAT(m.Element(2, 3), 0, 0);
    WD_TEST_FLOAT(m.Element(3, 3), 1, 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ZeroMatrix")
  {
    wdMat4T m = wdMat4T::ZeroMatrix();

    WD_TEST_FLOAT(m.Element(0, 0), 0, 0);
    WD_TEST_FLOAT(m.Element(1, 0), 0, 0);
    WD_TEST_FLOAT(m.Element(2, 0), 0, 0);
    WD_TEST_FLOAT(m.Element(3, 0), 0, 0);
    WD_TEST_FLOAT(m.Element(0, 1), 0, 0);
    WD_TEST_FLOAT(m.Element(1, 1), 0, 0);
    WD_TEST_FLOAT(m.Element(2, 1), 0, 0);
    WD_TEST_FLOAT(m.Element(3, 1), 0, 0);
    WD_TEST_FLOAT(m.Element(0, 2), 0, 0);
    WD_TEST_FLOAT(m.Element(1, 2), 0, 0);
    WD_TEST_FLOAT(m.Element(2, 2), 0, 0);
    WD_TEST_FLOAT(m.Element(3, 2), 0, 0);
    WD_TEST_FLOAT(m.Element(0, 3), 0, 0);
    WD_TEST_FLOAT(m.Element(1, 3), 0, 0);
    WD_TEST_FLOAT(m.Element(2, 3), 0, 0);
    WD_TEST_FLOAT(m.Element(3, 3), 0, 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Transpose")
  {
    wdMat4T m(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    m.Transpose();

    WD_TEST_FLOAT(m.Element(0, 0), 1, 0);
    WD_TEST_FLOAT(m.Element(1, 0), 5, 0);
    WD_TEST_FLOAT(m.Element(2, 0), 9, 0);
    WD_TEST_FLOAT(m.Element(3, 0), 13, 0);
    WD_TEST_FLOAT(m.Element(0, 1), 2, 0);
    WD_TEST_FLOAT(m.Element(1, 1), 6, 0);
    WD_TEST_FLOAT(m.Element(2, 1), 10, 0);
    WD_TEST_FLOAT(m.Element(3, 1), 14, 0);
    WD_TEST_FLOAT(m.Element(0, 2), 3, 0);
    WD_TEST_FLOAT(m.Element(1, 2), 7, 0);
    WD_TEST_FLOAT(m.Element(2, 2), 11, 0);
    WD_TEST_FLOAT(m.Element(3, 2), 15, 0);
    WD_TEST_FLOAT(m.Element(0, 3), 4, 0);
    WD_TEST_FLOAT(m.Element(1, 3), 8, 0);
    WD_TEST_FLOAT(m.Element(2, 3), 12, 0);
    WD_TEST_FLOAT(m.Element(3, 3), 16, 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetTranspose")
  {
    wdMat4T m0(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    wdMat4T m = m0.GetTranspose();

    WD_TEST_FLOAT(m.Element(0, 0), 1, 0);
    WD_TEST_FLOAT(m.Element(1, 0), 5, 0);
    WD_TEST_FLOAT(m.Element(2, 0), 9, 0);
    WD_TEST_FLOAT(m.Element(3, 0), 13, 0);
    WD_TEST_FLOAT(m.Element(0, 1), 2, 0);
    WD_TEST_FLOAT(m.Element(1, 1), 6, 0);
    WD_TEST_FLOAT(m.Element(2, 1), 10, 0);
    WD_TEST_FLOAT(m.Element(3, 1), 14, 0);
    WD_TEST_FLOAT(m.Element(0, 2), 3, 0);
    WD_TEST_FLOAT(m.Element(1, 2), 7, 0);
    WD_TEST_FLOAT(m.Element(2, 2), 11, 0);
    WD_TEST_FLOAT(m.Element(3, 2), 15, 0);
    WD_TEST_FLOAT(m.Element(0, 3), 4, 0);
    WD_TEST_FLOAT(m.Element(1, 3), 8, 0);
    WD_TEST_FLOAT(m.Element(2, 3), 12, 0);
    WD_TEST_FLOAT(m.Element(3, 3), 16, 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Invert")
  {
    for (float x = 1.0f; x < 360.0f; x += 10.0f)
    {
      for (float y = 2.0f; y < 360.0f; y += 17.0f)
      {
        for (float z = 3.0f; z < 360.0f; z += 23.0f)
        {
          wdMat4T m, inv;
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
          wdMat4T m, inv;
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
    wdMat4T m;

    m.SetIdentity();
    WD_TEST_BOOL(!m.IsZero());

    m.SetZero();
    WD_TEST_BOOL(m.IsZero());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsIdentity")
  {
    wdMat4T m;

    m.SetIdentity();
    WD_TEST_BOOL(m.IsIdentity());

    m.SetZero();
    WD_TEST_BOOL(!m.IsIdentity());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsValid")
  {
    if (wdMath::SupportsNaN<wdMat3T::ComponentType>())
    {
      wdMat4T m;

      m.SetZero();
      WD_TEST_BOOL(m.IsValid());

      m.m_fElementsCM[0] = wdMath::NaN<wdMat4T::ComponentType>();
      WD_TEST_BOOL(!m.IsValid());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetRow")
  {
    wdMat4T m(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    WD_TEST_VEC4(m.GetRow(0), wdVec4T(1, 2, 3, 4), 0.0f);
    WD_TEST_VEC4(m.GetRow(1), wdVec4T(5, 6, 7, 8), 0.0f);
    WD_TEST_VEC4(m.GetRow(2), wdVec4T(9, 10, 11, 12), 0.0f);
    WD_TEST_VEC4(m.GetRow(3), wdVec4T(13, 14, 15, 16), 0.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetRow")
  {
    wdMat4T m;
    m.SetZero();

    m.SetRow(0, wdVec4T(1, 2, 3, 4));
    WD_TEST_VEC4(m.GetRow(0), wdVec4T(1, 2, 3, 4), 0.0f);

    m.SetRow(1, wdVec4T(5, 6, 7, 8));
    WD_TEST_VEC4(m.GetRow(1), wdVec4T(5, 6, 7, 8), 0.0f);

    m.SetRow(2, wdVec4T(9, 10, 11, 12));
    WD_TEST_VEC4(m.GetRow(2), wdVec4T(9, 10, 11, 12), 0.0f);

    m.SetRow(3, wdVec4T(13, 14, 15, 16));
    WD_TEST_VEC4(m.GetRow(3), wdVec4T(13, 14, 15, 16), 0.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetColumn")
  {
    wdMat4T m(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    WD_TEST_VEC4(m.GetColumn(0), wdVec4T(1, 5, 9, 13), 0.0f);
    WD_TEST_VEC4(m.GetColumn(1), wdVec4T(2, 6, 10, 14), 0.0f);
    WD_TEST_VEC4(m.GetColumn(2), wdVec4T(3, 7, 11, 15), 0.0f);
    WD_TEST_VEC4(m.GetColumn(3), wdVec4T(4, 8, 12, 16), 0.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetColumn")
  {
    wdMat4T m;
    m.SetZero();

    m.SetColumn(0, wdVec4T(1, 2, 3, 4));
    WD_TEST_VEC4(m.GetColumn(0), wdVec4T(1, 2, 3, 4), 0.0f);

    m.SetColumn(1, wdVec4T(5, 6, 7, 8));
    WD_TEST_VEC4(m.GetColumn(1), wdVec4T(5, 6, 7, 8), 0.0f);

    m.SetColumn(2, wdVec4T(9, 10, 11, 12));
    WD_TEST_VEC4(m.GetColumn(2), wdVec4T(9, 10, 11, 12), 0.0f);

    m.SetColumn(3, wdVec4T(13, 14, 15, 16));
    WD_TEST_VEC4(m.GetColumn(3), wdVec4T(13, 14, 15, 16), 0.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetDiagonal")
  {
    wdMat4T m(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    WD_TEST_VEC4(m.GetDiagonal(), wdVec4T(1, 6, 11, 16), 0.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetDiagonal")
  {
    wdMat4T m;
    m.SetZero();

    m.SetDiagonal(wdVec4T(1, 2, 3, 4));
    WD_TEST_VEC4(m.GetColumn(0), wdVec4T(1, 0, 0, 0), 0.0f);
    WD_TEST_VEC4(m.GetColumn(1), wdVec4T(0, 2, 0, 0), 0.0f);
    WD_TEST_VEC4(m.GetColumn(2), wdVec4T(0, 0, 3, 0), 0.0f);
    WD_TEST_VEC4(m.GetColumn(3), wdVec4T(0, 0, 0, 4), 0.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetTranslationVector")
  {
    wdMat4T m(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    WD_TEST_VEC3(m.GetTranslationVector(), wdVec3T(4, 8, 12), 0.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetTranslationVector")
  {
    wdMat4T m(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    m.SetTranslationVector(wdVec3T(17, 18, 19));
    WD_TEST_VEC4(m.GetRow(0), wdVec4T(1, 2, 3, 17), 0.0f);
    WD_TEST_VEC4(m.GetRow(1), wdVec4T(5, 6, 7, 18), 0.0f);
    WD_TEST_VEC4(m.GetRow(2), wdVec4T(9, 10, 11, 19), 0.0f);
    WD_TEST_VEC4(m.GetRow(3), wdVec4T(13, 14, 15, 16), 0.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetRotationalPart")
  {
    wdMat4T m(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    wdMat3T r(17, 18, 19, 20, 21, 22, 23, 24, 25);

    m.SetRotationalPart(r);
    WD_TEST_VEC4(m.GetRow(0), wdVec4T(17, 18, 19, 4), 0.0f);
    WD_TEST_VEC4(m.GetRow(1), wdVec4T(20, 21, 22, 8), 0.0f);
    WD_TEST_VEC4(m.GetRow(2), wdVec4T(23, 24, 25, 12), 0.0f);
    WD_TEST_VEC4(m.GetRow(3), wdVec4T(13, 14, 15, 16), 0.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetRotationalPart")
  {
    wdMat4T m(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    wdMat3T r = m.GetRotationalPart();
    WD_TEST_VEC3(r.GetRow(0), wdVec3T(1, 2, 3), 0.0f);
    WD_TEST_VEC3(r.GetRow(1), wdVec3T(5, 6, 7), 0.0f);
    WD_TEST_VEC3(r.GetRow(2), wdVec3T(9, 10, 11), 0.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetScalingFactors")
  {
    wdMat4T m(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    wdVec3T s = m.GetScalingFactors();
    WD_TEST_VEC3(s,
      wdVec3T(wdMath::Sqrt((wdMathTestType)(1 * 1 + 5 * 5 + 9 * 9)), wdMath::Sqrt((wdMathTestType)(2 * 2 + 6 * 6 + 10 * 10)),
        wdMath::Sqrt((wdMathTestType)(3 * 3 + 7 * 7 + 11 * 11))),
      0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetScalingFactors")
  {
    wdMat4T m(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    WD_TEST_BOOL(m.SetScalingFactors(wdVec3T(1, 2, 3)) == WD_SUCCESS);

    wdVec3T s = m.GetScalingFactors();
    WD_TEST_VEC3(s, wdVec3T(1, 2, 3), 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "TransformDirection")
  {
    wdMat4T m(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    const wdVec3T r = m.TransformDirection(wdVec3T(1, 2, 3));

    WD_TEST_VEC3(r, wdVec3T(1 * 1 + 2 * 2 + 3 * 3, 1 * 5 + 2 * 6 + 3 * 7, 1 * 9 + 2 * 10 + 3 * 11), 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "TransformDirection(array)")
  {
    wdMat4T m(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    wdVec3T data[3] = {wdVec3T(1, 2, 3), wdVec3T(4, 5, 6), wdVec3T(7, 8, 9)};

    m.TransformDirection(data, 2);

    WD_TEST_VEC3(data[0], wdVec3T(1 * 1 + 2 * 2 + 3 * 3, 1 * 5 + 2 * 6 + 3 * 7, 1 * 9 + 2 * 10 + 3 * 11), 0.0001f);
    WD_TEST_VEC3(data[1], wdVec3T(4 * 1 + 5 * 2 + 6 * 3, 4 * 5 + 5 * 6 + 6 * 7, 4 * 9 + 5 * 10 + 6 * 11), 0.0001f);
    WD_TEST_VEC3(data[2], wdVec3T(7, 8, 9), 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "TransformPosition")
  {
    wdMat4T m(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    const wdVec3T r = m.TransformPosition(wdVec3T(1, 2, 3));

    WD_TEST_VEC3(r, wdVec3T(1 * 1 + 2 * 2 + 3 * 3 + 4, 1 * 5 + 2 * 6 + 3 * 7 + 8, 1 * 9 + 2 * 10 + 3 * 11 + 12), 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "TransformPosition(array)")
  {
    wdMat4T m(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    wdVec3T data[3] = {wdVec3T(1, 2, 3), wdVec3T(4, 5, 6), wdVec3T(7, 8, 9)};

    m.TransformPosition(data, 2);

    WD_TEST_VEC3(data[0], wdVec3T(1 * 1 + 2 * 2 + 3 * 3 + 4, 1 * 5 + 2 * 6 + 3 * 7 + 8, 1 * 9 + 2 * 10 + 3 * 11 + 12), 0.0001f);
    WD_TEST_VEC3(data[1], wdVec3T(4 * 1 + 5 * 2 + 6 * 3 + 4, 4 * 5 + 5 * 6 + 6 * 7 + 8, 4 * 9 + 5 * 10 + 6 * 11 + 12), 0.0001f);
    WD_TEST_VEC3(data[2], wdVec3T(7, 8, 9), 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Transform")
  {
    wdMat4T m(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    const wdVec4T r = m.Transform(wdVec4T(1, 2, 3, 4));

    WD_TEST_VEC4(r,
      wdVec4T(1 * 1 + 2 * 2 + 3 * 3 + 4 * 4, 1 * 5 + 2 * 6 + 3 * 7 + 8 * 4, 1 * 9 + 2 * 10 + 3 * 11 + 12 * 4, 1 * 13 + 2 * 14 + 3 * 15 + 4 * 16),
      0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Transform(array)")
  {
    wdMat4T m(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    wdVec4T data[3] = {wdVec4T(1, 2, 3, 4), wdVec4T(5, 6, 7, 8), wdVec4T(9, 10, 11, 12)};

    m.Transform(data, 2);

    WD_TEST_VEC4(data[0],
      wdVec4T(1 * 1 + 2 * 2 + 3 * 3 + 4 * 4, 1 * 5 + 2 * 6 + 3 * 7 + 8 * 4, 1 * 9 + 2 * 10 + 3 * 11 + 12 * 4, 1 * 13 + 2 * 14 + 3 * 15 + 4 * 16),
      0.0001f);
    WD_TEST_VEC4(data[1],
      wdVec4T(5 * 1 + 6 * 2 + 7 * 3 + 8 * 4, 5 * 5 + 6 * 6 + 7 * 7 + 8 * 8, 5 * 9 + 6 * 10 + 7 * 11 + 12 * 8, 5 * 13 + 6 * 14 + 7 * 15 + 8 * 16),
      0.0001f);
    WD_TEST_VEC4(data[2], wdVec4T(9, 10, 11, 12), 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator*=")
  {
    wdMat4T m(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    m *= 2.0f;

    WD_TEST_VEC4(m.GetRow(0), wdVec4T(2, 4, 6, 8), 0.0001f);
    WD_TEST_VEC4(m.GetRow(1), wdVec4T(10, 12, 14, 16), 0.0001f);
    WD_TEST_VEC4(m.GetRow(2), wdVec4T(18, 20, 22, 24), 0.0001f);
    WD_TEST_VEC4(m.GetRow(3), wdVec4T(26, 28, 30, 32), 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator/=")
  {
    wdMat4T m(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    m *= 4.0f;
    m /= 2.0f;

    WD_TEST_VEC4(m.GetRow(0), wdVec4T(2, 4, 6, 8), 0.0001f);
    WD_TEST_VEC4(m.GetRow(1), wdVec4T(10, 12, 14, 16), 0.0001f);
    WD_TEST_VEC4(m.GetRow(2), wdVec4T(18, 20, 22, 24), 0.0001f);
    WD_TEST_VEC4(m.GetRow(3), wdVec4T(26, 28, 30, 32), 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsIdentical")
  {
    wdMat4T m(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    wdMat4T m2 = m;

    WD_TEST_BOOL(m.IsIdentical(m2));

    m2.m_fElementsCM[0] += 0.00001f;
    WD_TEST_BOOL(!m.IsIdentical(m2));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsEqual")
  {
    wdMat4T m(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    wdMat4T m2 = m;

    WD_TEST_BOOL(m.IsEqual(m2, 0.0001f));

    m2.m_fElementsCM[0] += 0.00001f;
    WD_TEST_BOOL(m.IsEqual(m2, 0.0001f));
    WD_TEST_BOOL(!m.IsEqual(m2, 0.000001f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator*(mat, mat)")
  {
    wdMat4T m1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    wdMat4T m2(-1, -2, -3, -4, -5, -6, -7, -8, -9, -10, -11, -12, -13, -14, -15, -16);

    wdMat4T r = m1 * m2;

    WD_TEST_VEC4(r.GetColumn(0),
      wdVec4T(-1 * 1 + -5 * 2 + -9 * 3 + -13 * 4, -1 * 5 + -5 * 6 + -9 * 7 + -13 * 8, -1 * 9 + -5 * 10 + -9 * 11 + -13 * 12,
        -1 * 13 + -5 * 14 + -9 * 15 + -13 * 16),
      0.001f);
    WD_TEST_VEC4(r.GetColumn(1),
      wdVec4T(-2 * 1 + -6 * 2 + -10 * 3 + -14 * 4, -2 * 5 + -6 * 6 + -10 * 7 + -14 * 8, -2 * 9 + -6 * 10 + -10 * 11 + -14 * 12,
        -2 * 13 + -6 * 14 + -10 * 15 + -14 * 16),
      0.001f);
    WD_TEST_VEC4(r.GetColumn(2),
      wdVec4T(-3 * 1 + -7 * 2 + -11 * 3 + -15 * 4, -3 * 5 + -7 * 6 + -11 * 7 + -15 * 8, -3 * 9 + -7 * 10 + -11 * 11 + -15 * 12,
        -3 * 13 + -7 * 14 + -11 * 15 + -15 * 16),
      0.001f);
    WD_TEST_VEC4(r.GetColumn(3),
      wdVec4T(-4 * 1 + -8 * 2 + -12 * 3 + -16 * 4, -4 * 5 + -8 * 6 + -12 * 7 + -16 * 8, -4 * 9 + -8 * 10 + -12 * 11 + -16 * 12,
        -4 * 13 + -8 * 14 + -12 * 15 + -16 * 16),
      0.001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator*(mat, vec3)")
  {
    wdMat4T m(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    const wdVec3T r = m * wdVec3T(1, 2, 3);

    WD_TEST_VEC3(r, wdVec3T(1 * 1 + 2 * 2 + 3 * 3 + 4, 1 * 5 + 2 * 6 + 3 * 7 + 8, 1 * 9 + 2 * 10 + 3 * 11 + 12), 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator*(mat, vec4)")
  {
    wdMat4T m(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    const wdVec4T r = m * wdVec4T(1, 2, 3, 4);

    WD_TEST_VEC4(r,
      wdVec4T(1 * 1 + 2 * 2 + 3 * 3 + 4 * 4, 1 * 5 + 2 * 6 + 3 * 7 + 4 * 8, 1 * 9 + 2 * 10 + 3 * 11 + 4 * 12, 1 * 13 + 2 * 14 + 3 * 15 + 4 * 16),
      0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator*(mat, float) | operator*(float, mat)")
  {
    wdMat4T m0(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    wdMat4T m = m0 * (wdMathTestType)2;
    wdMat4T m2 = (wdMathTestType)2 * m0;

    WD_TEST_VEC4(m.GetRow(0), wdVec4T(2, 4, 6, 8), 0.0001f);
    WD_TEST_VEC4(m.GetRow(1), wdVec4T(10, 12, 14, 16), 0.0001f);
    WD_TEST_VEC4(m.GetRow(2), wdVec4T(18, 20, 22, 24), 0.0001f);
    WD_TEST_VEC4(m.GetRow(3), wdVec4T(26, 28, 30, 32), 0.0001f);

    WD_TEST_VEC4(m2.GetRow(0), wdVec4T(2, 4, 6, 8), 0.0001f);
    WD_TEST_VEC4(m2.GetRow(1), wdVec4T(10, 12, 14, 16), 0.0001f);
    WD_TEST_VEC4(m2.GetRow(2), wdVec4T(18, 20, 22, 24), 0.0001f);
    WD_TEST_VEC4(m2.GetRow(3), wdVec4T(26, 28, 30, 32), 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator/(mat, float)")
  {
    wdMat4T m0(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    m0 *= (wdMathTestType)4;

    wdMat4T m = m0 / (wdMathTestType)2;

    WD_TEST_VEC4(m.GetRow(0), wdVec4T(2, 4, 6, 8), 0.0001f);
    WD_TEST_VEC4(m.GetRow(1), wdVec4T(10, 12, 14, 16), 0.0001f);
    WD_TEST_VEC4(m.GetRow(2), wdVec4T(18, 20, 22, 24), 0.0001f);
    WD_TEST_VEC4(m.GetRow(3), wdVec4T(26, 28, 30, 32), 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator+(mat, mat) | operator-(mat, mat)")
  {
    wdMat4T m0(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    wdMat4T m1(-1, -2, -3, -4, -5, -6, -7, -8, -9, -10, -11, -12, -13, -14, -15, -16);

    WD_TEST_BOOL((m0 + m1).IsZero());
    WD_TEST_BOOL((m0 - m1).IsEqual(m0 * (wdMathTestType)2, 0.0001f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator== (mat, mat) | operator!= (mat, mat)")
  {
    wdMat4T m(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    wdMat4T m2 = m;

    WD_TEST_BOOL(m == m2);

    m2.m_fElementsCM[0] += 0.00001f;

    WD_TEST_BOOL(m != m2);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsNaN")
  {
    if (wdMath::SupportsNaN<wdMathTestType>())
    {
      wdMat4T m;

      m.SetIdentity();
      WD_TEST_BOOL(!m.IsNaN());

      for (wdUInt32 i = 0; i < 16; ++i)
      {
        m.SetIdentity();
        m.m_fElementsCM[i] = wdMath::NaN<wdMathTestType>();

        WD_TEST_BOOL(m.IsNaN());
      }
    }
  }
}
