#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Math/Transform.h>
#include <Foundation/SimdMath/SimdConversion.h>
#include <Foundation/SimdMath/SimdTransform.h>

WD_CREATE_SIMPLE_TEST(SimdMath, SimdTransform)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor")
  {
    wdSimdTransform t0;

    {
      wdSimdQuat qRot;
      qRot.SetFromAxisAndAngle(wdSimdVec4f(1, 2, 3).GetNormalized<3>(), wdAngle::Degree(42.0f));

      wdSimdVec4f pos(4, 5, 6);
      wdSimdVec4f scale(7, 8, 9);

      wdSimdTransform t(pos);
      WD_TEST_BOOL((t.m_Position == pos).AllSet<3>());
      WD_TEST_BOOL(t.m_Rotation == wdSimdQuat::IdentityQuaternion());
      WD_TEST_BOOL((t.m_Scale == wdSimdVec4f(1)).AllSet<3>());

      t = wdSimdTransform(pos, qRot);
      WD_TEST_BOOL((t.m_Position == pos).AllSet<3>());
      WD_TEST_BOOL(t.m_Rotation == qRot);
      WD_TEST_BOOL((t.m_Scale == wdSimdVec4f(1)).AllSet<3>());

      t = wdSimdTransform(pos, qRot, scale);
      WD_TEST_BOOL((t.m_Position == pos).AllSet<3>());
      WD_TEST_BOOL(t.m_Rotation == qRot);
      WD_TEST_BOOL((t.m_Scale == scale).AllSet<3>());

      t = wdSimdTransform(qRot);
      WD_TEST_BOOL(t.m_Position.IsZero<3>());
      WD_TEST_BOOL(t.m_Rotation == qRot);
      WD_TEST_BOOL((t.m_Scale == wdSimdVec4f(1)).AllSet<3>());
    }

    {
      wdSimdTransform t;
      t.SetIdentity();

      WD_TEST_BOOL(t.m_Position.IsZero<3>());
      WD_TEST_BOOL(t.m_Rotation == wdSimdQuat::IdentityQuaternion());
      WD_TEST_BOOL((t.m_Scale == wdSimdVec4f(1)).AllSet<3>());

      WD_TEST_BOOL(t == wdSimdTransform::IdentityTransform());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Inverse")
  {
    wdSimdTransform tParent(wdSimdVec4f(1, 2, 3));
    tParent.m_Rotation.SetFromAxisAndAngle(wdSimdVec4f(0, 1, 0), wdAngle::Degree(90));
    tParent.m_Scale = wdSimdVec4f(2);

    wdSimdTransform tToChild(wdSimdVec4f(4, 5, 6));
    tToChild.m_Rotation.SetFromAxisAndAngle(wdSimdVec4f(0, 0, 1), wdAngle::Degree(90));
    tToChild.m_Scale = wdSimdVec4f(4);

    wdSimdTransform tChild;
    tChild = tParent * tToChild;

    // invert twice -> get back original
    wdSimdTransform t2 = tToChild;
    t2.Invert();
    t2.Invert();
    WD_TEST_BOOL(t2.IsEqual(tToChild, 0.0001f));

    wdSimdTransform tInvToChild = tToChild.GetInverse();

    wdSimdTransform tParentFromChild;
    tParentFromChild = tChild * tInvToChild;

    WD_TEST_BOOL(tParent.IsEqual(tParentFromChild, 0.0001f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetLocalTransform")
  {
    wdSimdQuat q;
    q.SetFromAxisAndAngle(wdSimdVec4f(0, 0, 1), wdAngle::Degree(90));

    wdSimdTransform tParent(wdSimdVec4f(1, 2, 3));
    tParent.m_Rotation.SetFromAxisAndAngle(wdSimdVec4f(0, 1, 0), wdAngle::Degree(90));
    tParent.m_Scale = wdSimdVec4f(2);

    wdSimdTransform tChild;
    tChild.m_Position = wdSimdVec4f(13, 12, -5);
    tChild.m_Rotation = tParent.m_Rotation * q;
    tChild.m_Scale = wdSimdVec4f(8);

    wdSimdTransform tToChild;
    tToChild.SetLocalTransform(tParent, tChild);

    WD_TEST_BOOL(tToChild.m_Position.IsEqual(wdSimdVec4f(4, 5, 6), 0.0001f).AllSet<3>());
    WD_TEST_BOOL(tToChild.m_Rotation.IsEqualRotation(q, 0.0001f));
    WD_TEST_BOOL((tToChild.m_Scale == wdSimdVec4f(4)).AllSet<3>());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetGlobalTransform")
  {
    wdSimdTransform tParent(wdSimdVec4f(1, 2, 3));
    tParent.m_Rotation.SetFromAxisAndAngle(wdSimdVec4f(0, 1, 0), wdAngle::Degree(90));
    tParent.m_Scale = wdSimdVec4f(2);

    wdSimdTransform tToChild(wdSimdVec4f(4, 5, 6));
    tToChild.m_Rotation.SetFromAxisAndAngle(wdSimdVec4f(0, 0, 1), wdAngle::Degree(90));
    tToChild.m_Scale = wdSimdVec4f(4);

    wdSimdTransform tChild;
    tChild.SetGlobalTransform(tParent, tToChild);

    WD_TEST_BOOL(tChild.m_Position.IsEqual(wdSimdVec4f(13, 12, -5), 0.0001f).AllSet<3>());
    WD_TEST_BOOL(tChild.m_Rotation.IsEqualRotation(tParent.m_Rotation * tToChild.m_Rotation, 0.0001f));
    WD_TEST_BOOL((tChild.m_Scale == wdSimdVec4f(8)).AllSet<3>());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetAsMat4")
  {
    wdSimdTransform t(wdSimdVec4f(1, 2, 3));
    t.m_Rotation.SetFromAxisAndAngle(wdSimdVec4f(0, 1, 0), wdAngle::Degree(34));
    t.m_Scale = wdSimdVec4f(2, -1, 5);

    wdSimdMat4f m = t.GetAsMat4();

    // reference
    wdSimdMat4f refM;
    {
      wdQuat q;
      q.SetFromAxisAndAngle(wdVec3(0, 1, 0), wdAngle::Degree(34));

      wdTransform referenceTransform(wdVec3(1, 2, 3), q, wdVec3(2, -1, 5));
      wdMat4 tmp = referenceTransform.GetAsMat4();
      refM.SetFromArray(tmp.m_fElementsCM, wdMatrixLayout::ColumnMajor);
    }
    WD_TEST_BOOL(m.IsEqual(refM, 0.00001f));

    wdSimdVec4f p[8] = {wdSimdVec4f(-4, 0, 0), wdSimdVec4f(5, 0, 0), wdSimdVec4f(0, -6, 0), wdSimdVec4f(0, 7, 0), wdSimdVec4f(0, 0, -8),
      wdSimdVec4f(0, 0, 9), wdSimdVec4f(1, -2, 3), wdSimdVec4f(-4, 5, 7)};

    for (wdUInt32 i = 0; i < WD_ARRAY_SIZE(p); ++i)
    {
      wdSimdVec4f pt = t.TransformPosition(p[i]);
      wdSimdVec4f pm = m.TransformPosition(p[i]);

      WD_TEST_BOOL(pt.IsEqual(pm, 0.00001f).AllSet<3>());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "TransformPos / Dir / operator*")
  {
    wdSimdQuat qRotX, qRotY;
    qRotX.SetFromAxisAndAngle(wdSimdVec4f(1, 0, 0), wdAngle::Degree(90.0f));
    qRotY.SetFromAxisAndAngle(wdSimdVec4f(0, 1, 0), wdAngle::Degree(90.0f));

    wdSimdTransform t(wdSimdVec4f(1, 2, 3, 10), qRotY * qRotX, wdSimdVec4f(2, -2, 4, 11));

    wdSimdVec4f v;
    v = t.TransformPosition(wdSimdVec4f(4, 5, 6, 12));
    WD_TEST_BOOL(v.IsEqual(wdSimdVec4f((5 * -2) + 1, (-6 * 4) + 2, (-4 * 2) + 3), 0.0001f).AllSet<3>());

    v = t.TransformDirection(wdSimdVec4f(4, 5, 6, 13));
    WD_TEST_BOOL(v.IsEqual(wdSimdVec4f((5 * -2), (-6 * 4), (-4 * 2)), 0.0001f).AllSet<3>());

    v = t * wdSimdVec4f(4, 5, 6, 12);
    WD_TEST_BOOL(v.IsEqual(wdSimdVec4f((5 * -2) + 1, (-6 * 4) + 2, (-4 * 2) + 3), 0.0001f).AllSet<3>());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Operators")
  {
    {
      wdSimdTransform tParent(wdSimdVec4f(1, 2, 3));
      tParent.m_Rotation.SetFromAxisAndAngle(wdSimdVec4f(0, 1, 0), wdAngle::Degree(90));
      tParent.m_Scale = wdSimdVec4f(2);

      wdSimdTransform tToChild(wdSimdVec4f(4, 5, 6));
      tToChild.m_Rotation.SetFromAxisAndAngle(wdSimdVec4f(0, 0, 1), wdAngle::Degree(90));
      tToChild.m_Scale = wdSimdVec4f(4);

      // this is exactly the same as SetGlobalTransform
      wdSimdTransform tChild;
      tChild = tParent * tToChild;

      WD_TEST_BOOL(tChild.m_Position.IsEqual(wdSimdVec4f(13, 12, -5), 0.0001f).AllSet<3>());
      WD_TEST_BOOL(tChild.m_Rotation.IsEqualRotation(tParent.m_Rotation * tToChild.m_Rotation, 0.0001f));
      WD_TEST_BOOL((tChild.m_Scale == wdSimdVec4f(8)).AllSet<3>());

      tChild = tParent;
      tChild *= tToChild;

      WD_TEST_BOOL(tChild.m_Position.IsEqual(wdSimdVec4f(13, 12, -5), 0.0001f).AllSet<3>());
      WD_TEST_BOOL(tChild.m_Rotation.IsEqualRotation(tParent.m_Rotation * tToChild.m_Rotation, 0.0001f));
      WD_TEST_BOOL((tChild.m_Scale == wdSimdVec4f(8)).AllSet<3>());

      wdSimdVec4f a(7, 8, 9);
      wdSimdVec4f b;
      b = tToChild.TransformPosition(a);
      b = tParent.TransformPosition(b);

      wdSimdVec4f c;
      c = tChild.TransformPosition(a);

      WD_TEST_BOOL(b.IsEqual(c, 0.0001f).AllSet());

      // verify that it works exactly like a 4x4 matrix
      /*const wdMat4 mParent = tParent.GetAsMat4();
      const wdMat4 mToChild = tToChild.GetAsMat4();
      const wdMat4 mChild = mParent * mToChild;

      WD_TEST_BOOL(mChild.IsEqual(tChild.GetAsMat4(), 0.0001f));*/
    }

    {
      wdSimdTransform t(wdSimdVec4f(1, 2, 3));
      t.m_Rotation.SetFromAxisAndAngle(wdSimdVec4f(0, 1, 0), wdAngle::Degree(90));
      t.m_Scale = wdSimdVec4f(2);

      wdSimdQuat q;
      q.SetFromAxisAndAngle(wdSimdVec4f(0, 0, 1), wdAngle::Degree(90));

      wdSimdTransform t2 = t * q;
      wdSimdTransform t4 = q * t;

      wdSimdTransform t3 = t;
      t3 *= q;
      WD_TEST_BOOL(t2 == t3);
      WD_TEST_BOOL(t3 != t4);

      wdSimdVec4f a(7, 8, 9);
      wdSimdVec4f b;
      b = t2.TransformPosition(a);

      wdSimdVec4f c = q * a;
      c = t.TransformPosition(c);

      WD_TEST_BOOL(b.IsEqual(c, 0.0001f).AllSet());
    }

    {
      wdSimdTransform t(wdSimdVec4f(1, 2, 3));
      t.m_Rotation.SetFromAxisAndAngle(wdSimdVec4f(0, 1, 0), wdAngle::Degree(90));
      t.m_Scale = wdSimdVec4f(2);

      wdSimdVec4f p(4, 5, 6);

      wdSimdTransform t2 = t + p;
      wdSimdTransform t3 = t;
      t3 += p;
      WD_TEST_BOOL(t2 == t3);

      wdSimdVec4f a(7, 8, 9);
      wdSimdVec4f b;
      b = t2.TransformPosition(a);

      wdSimdVec4f c = t.TransformPosition(a) + p;

      WD_TEST_BOOL(b.IsEqual(c, 0.0001f).AllSet());
    }

    {
      wdSimdTransform t(wdSimdVec4f(1, 2, 3));
      t.m_Rotation.SetFromAxisAndAngle(wdSimdVec4f(0, 1, 0), wdAngle::Degree(90));
      t.m_Scale = wdSimdVec4f(2);

      wdSimdVec4f p(4, 5, 6);

      wdSimdTransform t2 = t - p;
      wdSimdTransform t3 = t;
      t3 -= p;
      WD_TEST_BOOL(t2 == t3);

      wdSimdVec4f a(7, 8, 9);
      wdSimdVec4f b;
      b = t2.TransformPosition(a);

      wdSimdVec4f c = t.TransformPosition(a) - p;

      WD_TEST_BOOL(b.IsEqual(c, 0.0001f).AllSet());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Comparison")
  {
    wdSimdTransform t(wdSimdVec4f(1, 2, 3));
    t.m_Rotation.SetFromAxisAndAngle(wdSimdVec4f(0, 1, 0), wdAngle::Degree(90));

    WD_TEST_BOOL(t == t);

    wdSimdTransform t2(wdSimdVec4f(1, 2, 4));
    t2.m_Rotation.SetFromAxisAndAngle(wdSimdVec4f(0, 1, 0), wdAngle::Degree(90));

    WD_TEST_BOOL(t != t2);

    wdSimdTransform t3(wdSimdVec4f(1, 2, 3));
    t3.m_Rotation.SetFromAxisAndAngle(wdSimdVec4f(0, 1, 0), wdAngle::Degree(91));

    WD_TEST_BOOL(t != t3);
  }
}
