#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/SimdMath/SimdTransform.h>

WD_CREATE_SIMPLE_TEST(SimdMath, SimdMat4f)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor (Array Data)")
  {
    const float data[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

    {
      wdSimdMat4f m(data, wdMatrixLayout::ColumnMajor);

      WD_TEST_BOOL((m.m_col0 == wdSimdVec4f(1, 2, 3, 4)).AllSet());
      WD_TEST_BOOL((m.m_col1 == wdSimdVec4f(5, 6, 7, 8)).AllSet());
      WD_TEST_BOOL((m.m_col2 == wdSimdVec4f(9, 10, 11, 12)).AllSet());
      WD_TEST_BOOL((m.m_col3 == wdSimdVec4f(13, 14, 15, 16)).AllSet());
    }

    {
      wdSimdMat4f m(data, wdMatrixLayout::RowMajor);

      WD_TEST_BOOL((m.m_col0 == wdSimdVec4f(1, 5, 9, 13)).AllSet());
      WD_TEST_BOOL((m.m_col1 == wdSimdVec4f(2, 6, 10, 14)).AllSet());
      WD_TEST_BOOL((m.m_col2 == wdSimdVec4f(3, 7, 11, 15)).AllSet());
      WD_TEST_BOOL((m.m_col3 == wdSimdVec4f(4, 8, 12, 16)).AllSet());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor (Columns)")
  {
    wdSimdVec4f c0(1, 2, 3, 4);
    wdSimdVec4f c1(5, 6, 7, 8);
    wdSimdVec4f c2(9, 10, 11, 12);
    wdSimdVec4f c3(13, 14, 15, 16);

    wdSimdMat4f m(c0, c1, c2, c3);

    WD_TEST_BOOL((m.m_col0 == wdSimdVec4f(1, 2, 3, 4)).AllSet());
    WD_TEST_BOOL((m.m_col1 == wdSimdVec4f(5, 6, 7, 8)).AllSet());
    WD_TEST_BOOL((m.m_col2 == wdSimdVec4f(9, 10, 11, 12)).AllSet());
    WD_TEST_BOOL((m.m_col3 == wdSimdVec4f(13, 14, 15, 16)).AllSet());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetFromArray")
  {
    const float data[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

    {
      wdSimdMat4f m;
      m.SetFromArray(data, wdMatrixLayout::ColumnMajor);

      WD_TEST_BOOL((m.m_col0 == wdSimdVec4f(1, 2, 3, 4)).AllSet());
      WD_TEST_BOOL((m.m_col1 == wdSimdVec4f(5, 6, 7, 8)).AllSet());
      WD_TEST_BOOL((m.m_col2 == wdSimdVec4f(9, 10, 11, 12)).AllSet());
      WD_TEST_BOOL((m.m_col3 == wdSimdVec4f(13, 14, 15, 16)).AllSet());
    }

    {
      wdSimdMat4f m;
      m.SetFromArray(data, wdMatrixLayout::RowMajor);

      WD_TEST_BOOL((m.m_col0 == wdSimdVec4f(1, 5, 9, 13)).AllSet());
      WD_TEST_BOOL((m.m_col1 == wdSimdVec4f(2, 6, 10, 14)).AllSet());
      WD_TEST_BOOL((m.m_col2 == wdSimdVec4f(3, 7, 11, 15)).AllSet());
      WD_TEST_BOOL((m.m_col3 == wdSimdVec4f(4, 8, 12, 16)).AllSet());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetAsArray")
  {
    wdSimdMat4f m(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    float data[16];

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

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetIdentity")
  {
    wdSimdMat4f m;
    m.SetIdentity();

    WD_TEST_BOOL((m.m_col0 == wdSimdVec4f(1, 0, 0, 0)).AllSet());
    WD_TEST_BOOL((m.m_col1 == wdSimdVec4f(0, 1, 0, 0)).AllSet());
    WD_TEST_BOOL((m.m_col2 == wdSimdVec4f(0, 0, 1, 0)).AllSet());
    WD_TEST_BOOL((m.m_col3 == wdSimdVec4f(0, 0, 0, 1)).AllSet());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IdentityMatrix")
  {
    wdSimdMat4f m = wdSimdMat4f::IdentityMatrix();

    WD_TEST_BOOL((m.m_col0 == wdSimdVec4f(1, 0, 0, 0)).AllSet());
    WD_TEST_BOOL((m.m_col1 == wdSimdVec4f(0, 1, 0, 0)).AllSet());
    WD_TEST_BOOL((m.m_col2 == wdSimdVec4f(0, 0, 1, 0)).AllSet());
    WD_TEST_BOOL((m.m_col3 == wdSimdVec4f(0, 0, 0, 1)).AllSet());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Transpose")
  {
    wdSimdMat4f m(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    m.Transpose();

    WD_TEST_BOOL((m.m_col0 == wdSimdVec4f(1, 2, 3, 4)).AllSet());
    WD_TEST_BOOL((m.m_col1 == wdSimdVec4f(5, 6, 7, 8)).AllSet());
    WD_TEST_BOOL((m.m_col2 == wdSimdVec4f(9, 10, 11, 12)).AllSet());
    WD_TEST_BOOL((m.m_col3 == wdSimdVec4f(13, 14, 15, 16)).AllSet());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetTranspose")
  {
    wdSimdMat4f m0(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    wdSimdMat4f m = m0.GetTranspose();

    WD_TEST_BOOL((m.m_col0 == wdSimdVec4f(1, 2, 3, 4)).AllSet());
    WD_TEST_BOOL((m.m_col1 == wdSimdVec4f(5, 6, 7, 8)).AllSet());
    WD_TEST_BOOL((m.m_col2 == wdSimdVec4f(9, 10, 11, 12)).AllSet());
    WD_TEST_BOOL((m.m_col3 == wdSimdVec4f(13, 14, 15, 16)).AllSet());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Invert")
  {
    for (float x = 1.0f; x < 360.0f; x += 20.0f)
    {
      for (float y = 2.0f; y < 360.0f; y += 27.0f)
      {
        for (float z = 3.0f; z < 360.0f; z += 33.0f)
        {
          wdSimdQuat q;
          q.SetFromAxisAndAngle(wdSimdVec4f(x, y, z).GetNormalized<3>(), wdAngle::Degree(19.0f));

          wdSimdTransform t(q);

          wdSimdMat4f m, inv;
          m = t.GetAsMat4();
          inv = m;
          WD_TEST_BOOL(inv.Invert() == WD_SUCCESS);

          wdSimdVec4f v = m.TransformDirection(wdSimdVec4f(1, 3, -10));
          wdSimdVec4f vinv = inv.TransformDirection(v);

          WD_TEST_BOOL(vinv.IsEqual(wdSimdVec4f(1, 3, -10), wdMath::DefaultEpsilon<float>()).AllSet<3>());
        }
      }
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetInverse")
  {
    for (float x = 1.0f; x < 360.0f; x += 19.0f)
    {
      for (float y = 2.0f; y < 360.0f; y += 29.0f)
      {
        for (float z = 3.0f; z < 360.0f; z += 31.0f)
        {
          wdSimdQuat q;
          q.SetFromAxisAndAngle(wdSimdVec4f(x, y, z).GetNormalized<3>(), wdAngle::Degree(83.0f));

          wdSimdTransform t(q);

          wdSimdMat4f m, inv;
          m = t.GetAsMat4();
          inv = m.GetInverse();

          wdSimdVec4f v = m.TransformDirection(wdSimdVec4f(1, 3, -10));
          wdSimdVec4f vinv = inv.TransformDirection(v);

          WD_TEST_BOOL(vinv.IsEqual(wdSimdVec4f(1, 3, -10), wdMath::DefaultEpsilon<float>()).AllSet<3>());
        }
      }
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsEqual")
  {
    wdSimdMat4f m(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    wdSimdMat4f m2 = m;

    WD_TEST_BOOL(m.IsEqual(m2, 0.0001f));

    m2.m_col0 += wdSimdVec4f(0.00001f);
    WD_TEST_BOOL(m.IsEqual(m2, 0.0001f));
    WD_TEST_BOOL(!m.IsEqual(m2, 0.000001f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsIdentity")
  {
    wdSimdMat4f m;

    m.SetIdentity();
    WD_TEST_BOOL(m.IsIdentity());

    m.m_col0.SetZero();
    WD_TEST_BOOL(!m.IsIdentity());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsValid")
  {
    wdSimdMat4f m;

    m.SetIdentity();
    WD_TEST_BOOL(m.IsValid());

    m.m_col0.SetX(wdMath::NaN<float>());
    WD_TEST_BOOL(!m.IsValid());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsNaN")
  {
    wdSimdMat4f m;

    m.SetIdentity();
    WD_TEST_BOOL(!m.IsNaN());

    float data[16];

    for (wdUInt32 i = 0; i < 16; ++i)
    {
      m.SetIdentity();
      m.GetAsArray(data, wdMatrixLayout::ColumnMajor);
      data[i] = wdMath::NaN<float>();
      m.SetFromArray(data, wdMatrixLayout::ColumnMajor);

      WD_TEST_BOOL(m.IsNaN());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetRows")
  {
    wdSimdVec4f r0(1, 2, 3, 4);
    wdSimdVec4f r1(5, 6, 7, 8);
    wdSimdVec4f r2(9, 10, 11, 12);
    wdSimdVec4f r3(13, 14, 15, 16);

    wdSimdMat4f m;
    m.SetRows(r0, r1, r2, r3);

    WD_TEST_BOOL((m.m_col0 == wdSimdVec4f(1, 5, 9, 13)).AllSet());
    WD_TEST_BOOL((m.m_col1 == wdSimdVec4f(2, 6, 10, 14)).AllSet());
    WD_TEST_BOOL((m.m_col2 == wdSimdVec4f(3, 7, 11, 15)).AllSet());
    WD_TEST_BOOL((m.m_col3 == wdSimdVec4f(4, 8, 12, 16)).AllSet());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetRows")
  {
    wdSimdMat4f m(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    wdSimdVec4f r0, r1, r2, r3;
    m.GetRows(r0, r1, r2, r3);

    WD_TEST_BOOL((r0 == wdSimdVec4f(1, 2, 3, 4)).AllSet());
    WD_TEST_BOOL((r1 == wdSimdVec4f(5, 6, 7, 8)).AllSet());
    WD_TEST_BOOL((r2 == wdSimdVec4f(9, 10, 11, 12)).AllSet());
    WD_TEST_BOOL((r3 == wdSimdVec4f(13, 14, 15, 16)).AllSet());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "TransformPosition")
  {
    wdSimdMat4f m(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    const wdSimdVec4f r = m.TransformPosition(wdSimdVec4f(1, 2, 3));

    WD_TEST_BOOL(r.IsEqual(wdSimdVec4f(1 * 1 + 2 * 2 + 3 * 3 + 4, 1 * 5 + 2 * 6 + 3 * 7 + 8, 1 * 9 + 2 * 10 + 3 * 11 + 12), 0.0001f).AllSet<3>());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "TransformDirection")
  {
    wdSimdMat4f m(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    const wdSimdVec4f r = m.TransformDirection(wdSimdVec4f(1, 2, 3));

    WD_TEST_BOOL(r.IsEqual(wdSimdVec4f(1 * 1 + 2 * 2 + 3 * 3, 1 * 5 + 2 * 6 + 3 * 7, 1 * 9 + 2 * 10 + 3 * 11), 0.0001f).AllSet<3>());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator*(mat, mat)")
  {
    wdSimdMat4f m1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    wdSimdMat4f m2(-1, -2, -3, -4, -5, -6, -7, -8, -9, -10, -11, -12, -13, -14, -15, -16);

    wdSimdMat4f r = m1 * m2;

    WD_TEST_BOOL((r.m_col0 == wdSimdVec4f(-1 * 1 + -5 * 2 + -9 * 3 + -13 * 4, -1 * 5 + -5 * 6 + -9 * 7 + -13 * 8,
                                -1 * 9 + -5 * 10 + -9 * 11 + -13 * 12, -1 * 13 + -5 * 14 + -9 * 15 + -13 * 16))
                   .AllSet());
    WD_TEST_BOOL((r.m_col1 == wdSimdVec4f(-2 * 1 + -6 * 2 + -10 * 3 + -14 * 4, -2 * 5 + -6 * 6 + -10 * 7 + -14 * 8,
                                -2 * 9 + -6 * 10 + -10 * 11 + -14 * 12, -2 * 13 + -6 * 14 + -10 * 15 + -14 * 16))
                   .AllSet());
    WD_TEST_BOOL((r.m_col2 == wdSimdVec4f(-3 * 1 + -7 * 2 + -11 * 3 + -15 * 4, -3 * 5 + -7 * 6 + -11 * 7 + -15 * 8,
                                -3 * 9 + -7 * 10 + -11 * 11 + -15 * 12, -3 * 13 + -7 * 14 + -11 * 15 + -15 * 16))
                   .AllSet());
    WD_TEST_BOOL((r.m_col3 == wdSimdVec4f(-4 * 1 + -8 * 2 + -12 * 3 + -16 * 4, -4 * 5 + -8 * 6 + -12 * 7 + -16 * 8,
                                -4 * 9 + -8 * 10 + -12 * 11 + -16 * 12, -4 * 13 + -8 * 14 + -12 * 15 + -16 * 16))
                   .AllSet());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator== (mat, mat) | operator!= (mat, mat)")
  {
    wdSimdMat4f m(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    wdSimdMat4f m2 = m;

    WD_TEST_BOOL(m == m2);

    m2.m_col0 += wdSimdVec4f(0.00001f);

    WD_TEST_BOOL(m != m2);
  }
}
