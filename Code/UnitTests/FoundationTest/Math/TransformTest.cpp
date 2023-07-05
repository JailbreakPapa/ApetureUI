#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Math/Transform.h>

WD_CREATE_SIMPLE_TEST(Math, Transform)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructors")
  {
    wdTransformT t0;

    {
      wdTransformT t(wdVec3T(1, 2, 3));
      WD_TEST_VEC3(t.m_vPosition, wdVec3T(1, 2, 3), 0);
    }

    {
      wdQuat qRot;
      qRot.SetFromAxisAndAngle(wdVec3T(1, 2, 3).GetNormalized(), wdAngle::Degree(42.0f));

      wdTransformT t(wdVec3T(4, 5, 6), qRot);
      WD_TEST_VEC3(t.m_vPosition, wdVec3T(4, 5, 6), 0);
      WD_TEST_BOOL(t.m_qRotation == qRot);
    }

    {
      wdMat3 mRot;
      mRot.SetRotationMatrix(wdVec3T(1, 2, 3).GetNormalized(), wdAngle::Degree(42.0f));

      wdQuat q;
      q.SetFromMat3(mRot);

      wdTransformT t(wdVec3T(4, 5, 6), q);
      WD_TEST_VEC3(t.m_vPosition, wdVec3T(4, 5, 6), 0);
      WD_TEST_BOOL(t.m_qRotation.GetAsMat3().IsEqual(mRot, 0.0001f));
    }

    {
      wdQuat qRot;
      qRot.SetIdentity();

      wdTransformT t(wdVec3T(4, 5, 6), qRot, wdVec3T(2, 3, 4));
      WD_TEST_VEC3(t.m_vPosition, wdVec3T(4, 5, 6), 0);
      WD_TEST_BOOL(t.m_qRotation.GetAsMat3().IsEqual(wdMat3(1, 0, 0, 0, 1, 0, 0, 0, 1), 0.001f));
      WD_TEST_VEC3(t.m_vScale, wdVec3T(2, 3, 4), 0);
    }

    {
      wdMat3T mRot;
      mRot.SetRotationMatrix(wdVec3T(1, 2, 3).GetNormalized(), wdAngle::Degree(42.0f));
      wdMat4T mTrans;
      mTrans.SetTransformationMatrix(mRot, wdVec3T(1, 2, 3));

      wdTransformT t;
      t.SetFromMat4(mTrans);
      WD_TEST_VEC3(t.m_vPosition, wdVec3T(1, 2, 3), 0);
      WD_TEST_BOOL(t.m_qRotation.GetAsMat3().IsEqual(mRot, 0.001f));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetIdentity")
  {
    wdTransformT t;
    t.SetIdentity();

    WD_TEST_VEC3(t.m_vPosition, wdVec3T(0), 0);
    WD_TEST_BOOL(t.m_qRotation == wdQuat::IdentityQuaternion());
    WD_TEST_BOOL(t.m_vScale == wdVec3T(1.0f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetAsMat4")
  {
    wdQuat qRot;
    qRot.SetIdentity();

    wdTransformT t(wdVec3T(4, 5, 6), qRot, wdVec3T(2, 3, 4));
    WD_TEST_BOOL(t.GetAsMat4() == wdMat4(2, 0, 0, 4, 0, 3, 0, 5, 0, 0, 4, 6, 0, 0, 0, 1));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator + / -")
  {
    wdTransformT t0, t1;
    t0.SetIdentity();
    t1.SetIdentity();

    t1 = t0 + wdVec3T(2, 3, 4);
    WD_TEST_VEC3(t1.m_vPosition, wdVec3T(2, 3, 4), 0.0001f);

    t1 = t1 - wdVec3T(4, 2, 1);
    WD_TEST_VEC3(t1.m_vPosition, wdVec3T(-2, 1, 3), 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator * (quat)")
  {
    wdQuat qRotX, qRotY;
    qRotX.SetFromAxisAndAngle(wdVec3T(1, 0, 0), wdAngle::Radian(1.57079637f));
    qRotY.SetFromAxisAndAngle(wdVec3T(0, 1, 0), wdAngle::Radian(1.57079637f));

    wdTransformT t0, t1;
    t0.SetIdentity();
    t1.SetIdentity();

    t1 = qRotX * t0;
    WD_TEST_VEC3(t1.m_vPosition, wdVec3T(0, 0, 0), 0.0001f);

    wdQuat q;
    q.SetFromMat3(wdMat3(1, 0, 0, 0, 0, -1, 0, 1, 0));
    WD_TEST_BOOL(t1.m_qRotation.IsEqualRotation(q, 0.0001f));

    t1 = qRotY * t1;
    WD_TEST_VEC3(t1.m_vPosition, wdVec3T(0, 0, 0), 0.0001f);
    q.SetFromMat3(wdMat3(0, 1, 0, 0, 0, -1, -1, 0, 0));
    WD_TEST_BOOL(t1.m_qRotation.IsEqualRotation(q, 0.0001f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator * (vec3)")
  {
    wdQuat qRotX, qRotY;
    qRotX.SetFromAxisAndAngle(wdVec3T(1, 0, 0), wdAngle::Radian(1.57079637f));
    qRotY.SetFromAxisAndAngle(wdVec3T(0, 1, 0), wdAngle::Radian(1.57079637f));

    wdTransformT t;
    t.SetIdentity();

    t = qRotX * t;

    WD_TEST_BOOL(t.m_qRotation.IsEqualRotation(qRotX, 0.0001f));
    WD_TEST_VEC3(t.m_vPosition, wdVec3T(0, 0, 0), 0.0001f);
    WD_TEST_VEC3(t.m_vScale, wdVec3T(1, 1, 1), 0.0001f);

    t = t + wdVec3T(1, 2, 3);

    WD_TEST_BOOL(t.m_qRotation.IsEqualRotation(qRotX, 0.0001f));
    WD_TEST_VEC3(t.m_vPosition, wdVec3T(1, 2, 3), 0.0001f);
    WD_TEST_VEC3(t.m_vScale, wdVec3T(1, 1, 1), 0.0001f);

    t = qRotY * t;

    WD_TEST_BOOL(t.m_qRotation.IsEqualRotation(qRotY * qRotX, 0.0001f));
    WD_TEST_VEC3(t.m_vPosition, wdVec3T(1, 2, 3), 0.0001f);
    WD_TEST_VEC3(t.m_vScale, wdVec3T(1, 1, 1), 0.0001f);

    wdQuat q;
    q.SetFromMat3(wdMat3(0, 1, 0, 0, 0, -1, -1, 0, 0));
    WD_TEST_BOOL(t.m_qRotation.IsEqualRotation(q, 0.0001f));

    wdVec3T v;
    v = t * wdVec3T(4, 5, 6);

    WD_TEST_VEC3(v, wdVec3T(5 + 1, -6 + 2, -4 + 3), 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsIdentical")
  {
    wdTransformT t(wdVec3T(1, 2, 3));
    t.m_qRotation.SetFromAxisAndAngle(wdVec3T(0, 1, 0), wdAngle::Degree(90));

    WD_TEST_BOOL(t.IsIdentical(t));

    wdTransformT t2(wdVec3T(1, 2, 4));
    t2.m_qRotation.SetFromAxisAndAngle(wdVec3T(0, 1, 0), wdAngle::Degree(90));

    WD_TEST_BOOL(!t.IsIdentical(t2));

    wdTransformT t3(wdVec3T(1, 2, 3));
    t3.m_qRotation.SetFromAxisAndAngle(wdVec3T(0, 1, 0), wdAngle::Degree(91));

    WD_TEST_BOOL(!t.IsIdentical(t3));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator == / !=")
  {
    wdTransformT t(wdVec3T(1, 2, 3));
    t.m_qRotation.SetFromAxisAndAngle(wdVec3T(0, 1, 0), wdAngle::Degree(90));

    WD_TEST_BOOL(t == t);

    wdTransformT t2(wdVec3T(1, 2, 4));
    t2.m_qRotation.SetFromAxisAndAngle(wdVec3T(0, 1, 0), wdAngle::Degree(90));

    WD_TEST_BOOL(t != t2);

    wdTransformT t3(wdVec3T(1, 2, 3));
    t3.m_qRotation.SetFromAxisAndAngle(wdVec3T(0, 1, 0), wdAngle::Degree(91));

    WD_TEST_BOOL(t != t3);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsEqual")
  {
    wdTransformT t(wdVec3T(1, 2, 3));
    t.m_qRotation.SetFromAxisAndAngle(wdVec3T(0, 1, 0), wdAngle::Degree(90));

    WD_TEST_BOOL(t.IsEqual(t, 0.0001f));

    wdTransformT t2(wdVec3T(1, 2, 3.0002f));
    t2.m_qRotation.SetFromAxisAndAngle(wdVec3T(0, 1, 0), wdAngle::Degree(90));

    WD_TEST_BOOL(t.IsEqual(t2, 0.001f));
    WD_TEST_BOOL(!t.IsEqual(t2, 0.0001f));

    wdTransformT t3(wdVec3T(1, 2, 3));
    t3.m_qRotation.SetFromAxisAndAngle(wdVec3T(0, 1, 0), wdAngle::Degree(90.01f));

    WD_TEST_BOOL(t.IsEqual(t3, 0.01f));
    WD_TEST_BOOL(!t.IsEqual(t3, 0.0001f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator*(wdTransformT, wdTransformT)")
  {
    wdTransformT tParent(wdVec3T(1, 2, 3));
    tParent.m_qRotation.SetFromAxisAndAngle(wdVec3T(0, 1, 0), wdAngle::Radian(1.57079637f));
    tParent.m_vScale.Set(2);

    wdTransformT tToChild(wdVec3T(4, 5, 6));
    tToChild.m_qRotation.SetFromAxisAndAngle(wdVec3T(0, 0, 1), wdAngle::Radian(1.57079637f));
    tToChild.m_vScale.Set(4);

    // this is exactly the same as SetGlobalTransform
    wdTransformT tChild;
    tChild = tParent * tToChild;

    WD_TEST_VEC3(tChild.m_vPosition, wdVec3T(13, 12, -5), 0.003f);
    WD_TEST_BOOL(tChild.m_qRotation.GetAsMat3().IsEqual(wdMat3(0, 0, 1, 1, 0, 0, 0, 1, 0), 0.0001f));
    WD_TEST_VEC3(tChild.m_vScale, wdVec3T(8, 8, 8), 0.0001f);

    // verify that it works exactly like a 4x4 matrix
    const wdMat4 mParent = tParent.GetAsMat4();
    const wdMat4 mToChild = tToChild.GetAsMat4();
    const wdMat4 mChild = mParent * mToChild;

    WD_TEST_BOOL(mChild.IsEqual(tChild.GetAsMat4(), 0.0001f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator*(wdTransformT, wdMat4)")
  {
    wdTransformT tParent(wdVec3T(1, 2, 3));
    tParent.m_qRotation.SetFromAxisAndAngle(wdVec3T(0, 1, 0), wdAngle::Degree(90));
    tParent.m_vScale.Set(2);

    wdTransformT tToChild(wdVec3T(4, 5, 6));
    tToChild.m_qRotation.SetFromAxisAndAngle(wdVec3T(0, 0, 1), wdAngle::Degree(90));
    tToChild.m_vScale.Set(4);

    // this is exactly the same as SetGlobalTransform
    wdTransformT tChild;
    tChild = tParent * tToChild;

    WD_TEST_VEC3(tChild.m_vPosition, wdVec3T(13, 12, -5), 0.0001f);
    WD_TEST_BOOL(tChild.m_qRotation.GetAsMat3().IsEqual(wdMat3(0, 0, 1, 1, 0, 0, 0, 1, 0), 0.0001f));
    WD_TEST_VEC3(tChild.m_vScale, wdVec3T(8, 8, 8), 0.0001f);

    // verify that it works exactly like a 4x4 matrix
    const wdMat4 mParent = tParent.GetAsMat4();
    const wdMat4 mToChild = tToChild.GetAsMat4();
    const wdMat4 mChild = mParent * mToChild;

    WD_TEST_BOOL(mChild.IsEqual(tChild.GetAsMat4(), 0.0001f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator*(wdMat4, wdTransformT)")
  {
    wdTransformT tParent(wdVec3T(1, 2, 3));
    tParent.m_qRotation.SetFromAxisAndAngle(wdVec3T(0, 1, 0), wdAngle::Degree(90));
    tParent.m_vScale.Set(2);

    wdTransformT tToChild(wdVec3T(4, 5, 6));
    tToChild.m_qRotation.SetFromAxisAndAngle(wdVec3T(0, 0, 1), wdAngle::Degree(90));
    tToChild.m_vScale.Set(4);

    // this is exactly the same as SetGlobalTransform
    wdTransformT tChild;
    tChild = tParent * tToChild;

    WD_TEST_VEC3(tChild.m_vPosition, wdVec3T(13, 12, -5), 0.0001f);
    WD_TEST_BOOL(tChild.m_qRotation.GetAsMat3().IsEqual(wdMat3(0, 0, 1, 1, 0, 0, 0, 1, 0), 0.0001f));
    WD_TEST_VEC3(tChild.m_vScale, wdVec3T(8, 8, 8), 0.0001f);

    // verify that it works exactly like a 4x4 matrix
    const wdMat4 mParent = tParent.GetAsMat4();
    const wdMat4 mToChild = tToChild.GetAsMat4();
    const wdMat4 mChild = mParent * mToChild;

    WD_TEST_BOOL(mChild.IsEqual(tChild.GetAsMat4(), 0.0001f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Invert / GetInverse")
  {
    wdTransformT tParent(wdVec3T(1, 2, 3));
    tParent.m_qRotation.SetFromAxisAndAngle(wdVec3T(0, 1, 0), wdAngle::Degree(90));
    tParent.m_vScale.Set(2);

    wdTransformT tToChild(wdVec3T(4, 5, 6));
    tToChild.m_qRotation.SetFromAxisAndAngle(wdVec3T(0, 0, 1), wdAngle::Degree(90));
    tToChild.m_vScale.Set(4);

    wdTransformT tChild;
    tChild.SetGlobalTransform(tParent, tToChild);

    // negate twice -> get back original
    tToChild.Invert();
    tToChild.Invert();

    wdTransformT tInvToChild = tToChild.GetInverse();

    wdTransformT tParentFromChild;
    tParentFromChild.SetGlobalTransform(tChild, tInvToChild);

    WD_TEST_BOOL(tParent.IsEqual(tParentFromChild, 0.0001f));
  }

  //////////////////////////////////////////////////////////////////////////
  // Tests copied and ported over from wdSimdTransform
  //////////////////////////////////////////////////////////////////////////

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor")
  {
    wdTransform t0;

    {
      wdQuat qRot;
      qRot.SetFromAxisAndAngle(wdVec3(1, 2, 3).GetNormalized(), wdAngle::Degree(42.0f));

      wdVec3 pos(4, 5, 6);
      wdVec3 scale(7, 8, 9);

      wdTransform t(pos);
      WD_TEST_BOOL((t.m_vPosition == pos));
      WD_TEST_BOOL(t.m_qRotation == wdQuat::IdentityQuaternion());
      WD_TEST_BOOL((t.m_vScale == wdVec3(1)));

      t = wdTransform(pos, qRot);
      WD_TEST_BOOL((t.m_vPosition == pos));
      WD_TEST_BOOL(t.m_qRotation == qRot);
      WD_TEST_BOOL((t.m_vScale == wdVec3(1)));

      t = wdTransform(pos, qRot, scale);
      WD_TEST_BOOL((t.m_vPosition == pos));
      WD_TEST_BOOL(t.m_qRotation == qRot);
      WD_TEST_BOOL((t.m_vScale == scale));

      t = wdTransform(wdVec3::ZeroVector(), qRot);
      WD_TEST_BOOL(t.m_vPosition.IsZero());
      WD_TEST_BOOL(t.m_qRotation == qRot);
      WD_TEST_BOOL((t.m_vScale == wdVec3(1)));
    }

    {
      wdTransform t;
      t.SetIdentity();

      WD_TEST_BOOL(t.m_vPosition.IsZero());
      WD_TEST_BOOL(t.m_qRotation == wdQuat::IdentityQuaternion());
      WD_TEST_BOOL((t.m_vScale == wdVec3(1)));

      WD_TEST_BOOL(t == wdTransform::IdentityTransform());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Inverse")
  {
    wdTransform tParent(wdVec3(1, 2, 3));
    tParent.m_qRotation.SetFromAxisAndAngle(wdVec3(0, 1, 0), wdAngle::Degree(90));
    tParent.m_vScale = wdVec3(2);

    wdTransform tToChild(wdVec3(4, 5, 6));
    tToChild.m_qRotation.SetFromAxisAndAngle(wdVec3(0, 0, 1), wdAngle::Degree(90));
    tToChild.m_vScale = wdVec3(4);

    wdTransform tChild;
    tChild = tParent * tToChild;

    // invert twice -> get back original
    wdTransform t2 = tToChild;
    t2.Invert();
    t2.Invert();
    WD_TEST_BOOL(t2.IsEqual(tToChild, 0.0001f));

    wdTransform tInvToChild = tToChild.GetInverse();

    wdTransform tParentFromChild;
    tParentFromChild = tChild * tInvToChild;

    WD_TEST_BOOL(tParent.IsEqual(tParentFromChild, 0.0001f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetLocalTransform")
  {
    wdQuat q;
    q.SetFromAxisAndAngle(wdVec3(0, 0, 1), wdAngle::Degree(90));

    wdTransform tParent(wdVec3(1, 2, 3));
    tParent.m_qRotation.SetFromAxisAndAngle(wdVec3(0, 1, 0), wdAngle::Degree(90));
    tParent.m_vScale = wdVec3(2);

    wdTransform tChild;
    tChild.m_vPosition = wdVec3(13, 12, -5);
    tChild.m_qRotation = tParent.m_qRotation * q;
    tChild.m_vScale = wdVec3(8);

    wdTransform tToChild;
    tToChild.SetLocalTransform(tParent, tChild);

    WD_TEST_BOOL(tToChild.m_vPosition.IsEqual(wdVec3(4, 5, 6), 0.0001f));
    WD_TEST_BOOL(tToChild.m_qRotation.IsEqualRotation(q, 0.0001f));
    WD_TEST_BOOL((tToChild.m_vScale == wdVec3(4)));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetGlobalTransform")
  {
    wdTransform tParent(wdVec3(1, 2, 3));
    tParent.m_qRotation.SetFromAxisAndAngle(wdVec3(0, 1, 0), wdAngle::Degree(90));
    tParent.m_vScale = wdVec3(2);

    wdTransform tToChild(wdVec3(4, 5, 6));
    tToChild.m_qRotation.SetFromAxisAndAngle(wdVec3(0, 0, 1), wdAngle::Degree(90));
    tToChild.m_vScale = wdVec3(4);

    wdTransform tChild;
    tChild.SetGlobalTransform(tParent, tToChild);

    WD_TEST_BOOL(tChild.m_vPosition.IsEqual(wdVec3(13, 12, -5), 0.0001f));
    WD_TEST_BOOL(tChild.m_qRotation.IsEqualRotation(tParent.m_qRotation * tToChild.m_qRotation, 0.0001f));
    WD_TEST_BOOL((tChild.m_vScale == wdVec3(8)));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetAsMat4")
  {
    wdTransform t(wdVec3(1, 2, 3));
    t.m_qRotation.SetFromAxisAndAngle(wdVec3(0, 1, 0), wdAngle::Degree(34));
    t.m_vScale = wdVec3(2, -1, 5);

    wdMat4 m = t.GetAsMat4();

    wdMat4 refM;
    refM.SetZero();
    {
      wdQuat q;
      q.SetFromAxisAndAngle(wdVec3(0, 1, 0), wdAngle::Degree(34));

      wdTransform referenceTransform(wdVec3(1, 2, 3), q, wdVec3(2, -1, 5));
      wdMat4 tmp = referenceTransform.GetAsMat4();
      refM.SetFromArray(tmp.m_fElementsCM, wdMatrixLayout::ColumnMajor);
    }
    WD_TEST_BOOL(m.IsEqual(refM, 0.00001f));

    wdVec3 p[8] = {
      wdVec3(-4, 0, 0), wdVec3(5, 0, 0), wdVec3(0, -6, 0), wdVec3(0, 7, 0), wdVec3(0, 0, -8), wdVec3(0, 0, 9), wdVec3(1, -2, 3), wdVec3(-4, 5, 7)};

    for (wdUInt32 i = 0; i < WD_ARRAY_SIZE(p); ++i)
    {
      wdVec3 pt = t.TransformPosition(p[i]);
      wdVec3 pm = m.TransformPosition(p[i]);

      WD_TEST_BOOL(pt.IsEqual(pm, 0.00001f));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "TransformPos / Dir / operator*")
  {
    wdQuat qRotX, qRotY;
    qRotX.SetFromAxisAndAngle(wdVec3(1, 0, 0), wdAngle::Degree(90.0f));
    qRotY.SetFromAxisAndAngle(wdVec3(0, 1, 0), wdAngle::Degree(90.0f));

    wdTransform t(wdVec3(1, 2, 3), qRotY * qRotX, wdVec3(2, -2, 4));

    wdVec3 v;
    v = t.TransformPosition(wdVec3(4, 5, 6));
    WD_TEST_BOOL(v.IsEqual(wdVec3((5 * -2) + 1, (-6 * 4) + 2, (-4 * 2) + 3), 0.0001f));

    v = t.TransformDirection(wdVec3(4, 5, 6));
    WD_TEST_BOOL(v.IsEqual(wdVec3((5 * -2), (-6 * 4), (-4 * 2)), 0.0001f));

    v = t * wdVec3(4, 5, 6);
    WD_TEST_BOOL(v.IsEqual(wdVec3((5 * -2) + 1, (-6 * 4) + 2, (-4 * 2) + 3), 0.0001f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Operators")
  {
    {
      wdTransform tParent(wdVec3(1, 2, 3));
      tParent.m_qRotation.SetFromAxisAndAngle(wdVec3(0, 1, 0), wdAngle::Degree(90));
      tParent.m_vScale = wdVec3(2);

      wdTransform tToChild(wdVec3(4, 5, 6));
      tToChild.m_qRotation.SetFromAxisAndAngle(wdVec3(0, 0, 1), wdAngle::Degree(90));
      tToChild.m_vScale = wdVec3(4);

      // this is exactly the same as SetGlobalTransform
      wdTransform tChild;
      tChild = tParent * tToChild;

      WD_TEST_BOOL(tChild.m_vPosition.IsEqual(wdVec3(13, 12, -5), 0.0001f));
      WD_TEST_BOOL(tChild.m_qRotation.IsEqualRotation(tParent.m_qRotation * tToChild.m_qRotation, 0.0001f));
      WD_TEST_BOOL((tChild.m_vScale == wdVec3(8)));

      tChild = tParent;
      tChild = tChild * tToChild;

      WD_TEST_BOOL(tChild.m_vPosition.IsEqual(wdVec3(13, 12, -5), 0.0001f));
      WD_TEST_BOOL(tChild.m_qRotation.IsEqualRotation(tParent.m_qRotation * tToChild.m_qRotation, 0.0001f));
      WD_TEST_BOOL((tChild.m_vScale == wdVec3(8)));

      wdVec3 a(7, 8, 9);
      wdVec3 b;
      b = tToChild.TransformPosition(a);
      b = tParent.TransformPosition(b);

      wdVec3 c;
      c = tChild.TransformPosition(a);

      WD_TEST_BOOL(b.IsEqual(c, 0.0001f));

      // verify that it works exactly like a 4x4 matrix
      const wdMat4 mParent = tParent.GetAsMat4();
      const wdMat4 mToChild = tToChild.GetAsMat4();
      const wdMat4 mChild = mParent * mToChild;

      WD_TEST_BOOL(mChild.IsEqual(tChild.GetAsMat4(), 0.0001f));
    }

    {
      wdTransform t(wdVec3(1, 2, 3));
      t.m_qRotation.SetFromAxisAndAngle(wdVec3(0, 1, 0), wdAngle::Degree(90));
      t.m_vScale = wdVec3(2);

      wdQuat q;
      q.SetFromAxisAndAngle(wdVec3(0, 0, 1), wdAngle::Degree(90));

      wdTransform t2 = t * q;
      wdTransform t4 = q * t;

      wdTransform t3 = t;
      t3 = t3 * q;
      WD_TEST_BOOL(t2 == t3);
      WD_TEST_BOOL(t3 != t4);

      wdVec3 a(7, 8, 9);
      wdVec3 b;
      b = t2.TransformPosition(a);

      wdVec3 c = q * a;
      c = t.TransformPosition(c);

      WD_TEST_BOOL(b.IsEqual(c, 0.0001f));
    }

    {
      wdTransform t(wdVec3(1, 2, 3));
      t.m_qRotation.SetFromAxisAndAngle(wdVec3(0, 1, 0), wdAngle::Degree(90));
      t.m_vScale = wdVec3(2);

      wdVec3 p(4, 5, 6);

      wdTransform t2 = t + p;
      wdTransform t3 = t;
      t3 += p;
      WD_TEST_BOOL(t2 == t3);

      wdVec3 a(7, 8, 9);
      wdVec3 b;
      b = t2.TransformPosition(a);

      wdVec3 c = t.TransformPosition(a) + p;

      WD_TEST_BOOL(b.IsEqual(c, 0.0001f));
    }

    {
      wdTransform t(wdVec3(1, 2, 3));
      t.m_qRotation.SetFromAxisAndAngle(wdVec3(0, 1, 0), wdAngle::Degree(90));
      t.m_vScale = wdVec3(2);

      wdVec3 p(4, 5, 6);

      wdTransform t2 = t - p;
      wdTransform t3 = t;
      t3 -= p;
      WD_TEST_BOOL(t2 == t3);

      wdVec3 a(7, 8, 9);
      wdVec3 b;
      b = t2.TransformPosition(a);

      wdVec3 c = t.TransformPosition(a) - p;

      WD_TEST_BOOL(b.IsEqual(c, 0.0001f));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Comparison")
  {
    wdTransform t(wdVec3(1, 2, 3));
    t.m_qRotation.SetFromAxisAndAngle(wdVec3(0, 1, 0), wdAngle::Degree(90));

    WD_TEST_BOOL(t == t);

    wdTransform t2(wdVec3(1, 2, 4));
    t2.m_qRotation.SetFromAxisAndAngle(wdVec3(0, 1, 0), wdAngle::Degree(90));

    WD_TEST_BOOL(t != t2);

    wdTransform t3(wdVec3(1, 2, 3));
    t3.m_qRotation.SetFromAxisAndAngle(wdVec3(0, 1, 0), wdAngle::Degree(91));

    WD_TEST_BOOL(t != t3);
  }
}
