#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Reflection/ReflectionUtils.h>
#include <FoundationTest/Reflection/ReflectionTestClasses.h>

struct FunctionTest
{
  int StandardTypeFunction(int v, const wdVec2 vCv, wdVec3& ref_vRv, const wdVec4& vCrv, wdVec2U32* pPv, const wdVec3U32* pCpv)
  {
    WD_TEST_BOOL(m_values[0] == v);
    WD_TEST_BOOL(m_values[1] == vCv);
    WD_TEST_BOOL(m_values[2] == ref_vRv);
    WD_TEST_BOOL(m_values[3] == vCrv);
    if (m_bPtrAreNull)
    {
      WD_TEST_BOOL(!pPv);
      WD_TEST_BOOL(!pCpv);
    }
    else
    {
      WD_TEST_BOOL(m_values[4] == *pPv);
      WD_TEST_BOOL(m_values[5] == *pCpv);
    }
    ref_vRv.Set(1, 2, 3);
    if (pPv)
    {
      pPv->Set(1, 2);
    }
    return 5;
  }

  wdVarianceTypeAngle CustomTypeFunction(wdVarianceTypeAngle v, const wdVarianceTypeAngle cv, wdVarianceTypeAngle& ref_rv, const wdVarianceTypeAngle& crv, wdVarianceTypeAngle* pPv, const wdVarianceTypeAngle* pCpv)
  {
    WD_TEST_BOOL(m_values[0] == v);
    WD_TEST_BOOL(m_values[1] == cv);
    WD_TEST_BOOL(m_values[2] == ref_rv);
    WD_TEST_BOOL(m_values[3] == crv);
    if (m_bPtrAreNull)
    {
      WD_TEST_BOOL(!pPv);
      WD_TEST_BOOL(!pCpv);
    }
    else
    {
      WD_TEST_BOOL(m_values[4] == *pPv);
      WD_TEST_BOOL(m_values[5] == *pCpv);
    }
    ref_rv = {2.0f, wdAngle::Degree(200.0f)};
    if (pPv)
    {
      *pPv = {4.0f, wdAngle::Degree(400.0f)};
    }
    return {0.6f, wdAngle::Degree(60.0f)};
  }

  wdVarianceTypeAngle CustomTypeFunction2(wdVarianceTypeAngle v, const wdVarianceTypeAngle cv, wdVarianceTypeAngle& ref_rv, const wdVarianceTypeAngle& crv, wdVarianceTypeAngle* pPv, const wdVarianceTypeAngle* pCpv)
  {
    WD_TEST_BOOL(*m_values[0].Get<wdVarianceTypeAngle*>() == v);
    WD_TEST_BOOL(*m_values[1].Get<wdVarianceTypeAngle*>() == cv);
    WD_TEST_BOOL(*m_values[2].Get<wdVarianceTypeAngle*>() == ref_rv);
    WD_TEST_BOOL(*m_values[3].Get<wdVarianceTypeAngle*>() == crv);
    if (m_bPtrAreNull)
    {
      WD_TEST_BOOL(!pPv);
      WD_TEST_BOOL(!pCpv);
    }
    else
    {
      WD_TEST_BOOL(*m_values[4].Get<wdVarianceTypeAngle*>() == *pPv);
      WD_TEST_BOOL(*m_values[5].Get<wdVarianceTypeAngle*>() == *pCpv);
    }
    ref_rv = {2.0f, wdAngle::Degree(200.0f)};
    if (pPv)
    {
      *pPv = {4.0f, wdAngle::Degree(400.0f)};
    }
    return {0.6f, wdAngle::Degree(60.0f)};
  }

  const char* StringTypeFunction(const char* szString, wdString& ref_sString, wdStringView sView)
  {
    if (m_bPtrAreNull)
    {
      WD_TEST_BOOL(!szString);
    }
    else
    {
      WD_TEST_BOOL(m_values[0] == szString);
    }
    WD_TEST_BOOL(m_values[1] == ref_sString);
    WD_TEST_BOOL(m_values[2] == sView);
    return "StringRet";
  }

  wdEnum<wdExampleEnum> EnumFunction(
    wdEnum<wdExampleEnum> e, wdEnum<wdExampleEnum>& ref_re, const wdEnum<wdExampleEnum>& cre, wdEnum<wdExampleEnum>* pPe, const wdEnum<wdExampleEnum>* pCpe)
  {
    WD_TEST_BOOL(m_values[0].Get<wdInt64>() == e.GetValue());
    WD_TEST_BOOL(m_values[1].Get<wdInt64>() == ref_re.GetValue());
    WD_TEST_BOOL(m_values[2].Get<wdInt64>() == cre.GetValue());
    if (m_bPtrAreNull)
    {
      WD_TEST_BOOL(!pPe);
      WD_TEST_BOOL(!pCpe);
    }
    else
    {
      WD_TEST_BOOL(m_values[3].Get<wdInt64>() == pPe->GetValue());
      WD_TEST_BOOL(m_values[4].Get<wdInt64>() == pCpe->GetValue());
    }
    return wdExampleEnum::Value1;
  }

  wdBitflags<wdExampleBitflags> BitflagsFunction(wdBitflags<wdExampleBitflags> e, wdBitflags<wdExampleBitflags>& ref_re,
    const wdBitflags<wdExampleBitflags>& cre, wdBitflags<wdExampleBitflags>* pPe, const wdBitflags<wdExampleBitflags>* pCpe)
  {
    WD_TEST_BOOL(e == m_values[0].Get<wdInt64>());
    WD_TEST_BOOL(ref_re == m_values[1].Get<wdInt64>());
    WD_TEST_BOOL(cre == m_values[2].Get<wdInt64>());
    if (m_bPtrAreNull)
    {
      WD_TEST_BOOL(!pPe);
      WD_TEST_BOOL(!pCpe);
    }
    else
    {
      WD_TEST_BOOL(*pPe == m_values[3].Get<wdInt64>());
      WD_TEST_BOOL(*pCpe == m_values[4].Get<wdInt64>());
    }
    return wdExampleBitflags::Value1 | wdExampleBitflags::Value2;
  }

  wdTestStruct3 StructFunction(
    wdTestStruct3 s, const wdTestStruct3 cs, wdTestStruct3& ref_rs, const wdTestStruct3& crs, wdTestStruct3* pPs, const wdTestStruct3* pCps)
  {
    WD_TEST_BOOL(*static_cast<wdTestStruct3*>(m_values[0].Get<void*>()) == s);
    WD_TEST_BOOL(*static_cast<wdTestStruct3*>(m_values[1].Get<void*>()) == cs);
    WD_TEST_BOOL(*static_cast<wdTestStruct3*>(m_values[2].Get<void*>()) == ref_rs);
    WD_TEST_BOOL(*static_cast<wdTestStruct3*>(m_values[3].Get<void*>()) == crs);
    if (m_bPtrAreNull)
    {
      WD_TEST_BOOL(!pPs);
      WD_TEST_BOOL(!pCps);
    }
    else
    {
      WD_TEST_BOOL(*static_cast<wdTestStruct3*>(m_values[4].Get<void*>()) == *pPs);
      WD_TEST_BOOL(*static_cast<wdTestStruct3*>(m_values[5].Get<void*>()) == *pCps);
    }
    ref_rs.m_fFloat1 = 999.0f;
    ref_rs.m_UInt8 = 666;
    if (pPs)
    {
      pPs->m_fFloat1 = 666.0f;
      pPs->m_UInt8 = 999;
    }
    wdTestStruct3 retS;
    retS.m_fFloat1 = 42;
    retS.m_UInt8 = 42;
    return retS;
  }

  wdTestClass1 ReflectedClassFunction(
    wdTestClass1 s, const wdTestClass1 cs, wdTestClass1& ref_rs, const wdTestClass1& crs, wdTestClass1* pPs, const wdTestClass1* pCps)
  {
    WD_TEST_BOOL(*static_cast<wdTestClass1*>(m_values[0].ConvertTo<void*>()) == s);
    WD_TEST_BOOL(*static_cast<wdTestClass1*>(m_values[1].ConvertTo<void*>()) == cs);
    WD_TEST_BOOL(*static_cast<wdTestClass1*>(m_values[2].ConvertTo<void*>()) == ref_rs);
    WD_TEST_BOOL(*static_cast<wdTestClass1*>(m_values[3].ConvertTo<void*>()) == crs);
    if (m_bPtrAreNull)
    {
      WD_TEST_BOOL(!pPs);
      WD_TEST_BOOL(!pCps);
    }
    else
    {
      WD_TEST_BOOL(*static_cast<wdTestClass1*>(m_values[4].ConvertTo<void*>()) == *pPs);
      WD_TEST_BOOL(*static_cast<wdTestClass1*>(m_values[5].ConvertTo<void*>()) == *pCps);
    }
    ref_rs.m_Color.SetRGB(1, 2, 3);
    ref_rs.m_MyVector.Set(1, 2, 3);
    if (pPs)
    {
      pPs->m_Color.SetRGB(1, 2, 3);
      pPs->m_MyVector.Set(1, 2, 3);
    }
    wdTestClass1 retS;
    retS.m_Color.SetRGB(42, 42, 42);
    retS.m_MyVector.Set(42, 42, 42);
    return retS;
  }

  wdVariant VariantFunction(wdVariant v, const wdVariant cv, wdVariant& ref_rv, const wdVariant& crv, wdVariant* pPv, const wdVariant* pCpv)
  {
    WD_TEST_BOOL(m_values[0] == v);
    WD_TEST_BOOL(m_values[1] == cv);
    WD_TEST_BOOL(m_values[2] == ref_rv);
    WD_TEST_BOOL(m_values[3] == crv);
    if (m_bPtrAreNull)
    {
      // Can't have variant as nullptr as it must exist in the array and there is no further
      // way of distinguishing a between a wdVariant* and a wdVariant that is invalid.
      WD_TEST_BOOL(!pPv->IsValid());
      WD_TEST_BOOL(!pCpv->IsValid());
    }
    else
    {
      WD_TEST_BOOL(m_values[4] == *pPv);
      WD_TEST_BOOL(m_values[5] == *pCpv);
    }
    ref_rv = wdVec3(1, 2, 3);
    if (pPv)
    {
      *pPv = wdVec2U32(1, 2);
    }
    return 5;
  }

  static void StaticFunction(bool b, wdVariant v)
  {
    WD_TEST_BOOL(b == true);
    WD_TEST_BOOL(v == 4.0f);
  }

  static int StaticFunction2() { return 42; }

  bool m_bPtrAreNull = false;
  wdDynamicArray<wdVariant> m_values;
};

typedef std::tuple<const wdRTTI*, wdBitflags<wdPropertyFlags>> ParamSig;

void VerifyFunctionSignature(const wdAbstractFunctionProperty* pFunc, wdArrayPtr<ParamSig> params, ParamSig ret)
{
  WD_TEST_INT(params.GetCount(), pFunc->GetArgumentCount());
  for (wdUInt32 i = 0; i < wdMath::Min(params.GetCount(), pFunc->GetArgumentCount()); i++)
  {
    WD_TEST_BOOL(pFunc->GetArgumentType(i) == std::get<0>(params[i]));
    WD_TEST_BOOL(pFunc->GetArgumentFlags(i) == std::get<1>(params[i]));
  }
  WD_TEST_BOOL(pFunc->GetReturnType() == std::get<0>(ret));
  WD_TEST_BOOL(pFunc->GetReturnFlags() == std::get<1>(ret));
}

WD_CREATE_SIMPLE_TEST(Reflection, Functions)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Member Functions - StandardTypes")
  {
    wdFunctionProperty<decltype(&FunctionTest::StandardTypeFunction)> funccall("", &FunctionTest::StandardTypeFunction);
    ParamSig testSet[] = {
      ParamSig(wdGetStaticRTTI<int>(), wdPropertyFlags::StandardType),
      ParamSig(wdGetStaticRTTI<wdVec2>(), wdPropertyFlags::StandardType),
      ParamSig(wdGetStaticRTTI<wdVec3>(), wdPropertyFlags::StandardType | wdPropertyFlags::Reference),
      ParamSig(wdGetStaticRTTI<wdVec4>(), wdPropertyFlags::StandardType | wdPropertyFlags::Const | wdPropertyFlags::Reference),
      ParamSig(wdGetStaticRTTI<wdVec2U32>(), wdPropertyFlags::StandardType | wdPropertyFlags::Pointer),
      ParamSig(wdGetStaticRTTI<wdVec3U32>(), wdPropertyFlags::StandardType | wdPropertyFlags::Const | wdPropertyFlags::Pointer),
    };
    VerifyFunctionSignature(&funccall, wdArrayPtr<ParamSig>(testSet), ParamSig(wdGetStaticRTTI<int>(), wdPropertyFlags::StandardType));
    WD_TEST_BOOL(funccall.GetFunctionType() == wdFunctionType::Member);

    FunctionTest test;
    test.m_values.PushBack(1);
    test.m_values.PushBack(wdVec2(2));
    test.m_values.PushBack(wdVec3(3));
    test.m_values.PushBack(wdVec4(4));
    test.m_values.PushBack(wdVec2U32(5));
    test.m_values.PushBack(wdVec3U32(6));

    wdVariant ret;
    funccall.Execute(&test, test.m_values, ret);
    WD_TEST_BOOL(ret.GetType() == wdVariantType::Int32);
    WD_TEST_BOOL(ret == 5);
    WD_TEST_BOOL(test.m_values[2] == wdVec3(1, 2, 3));
    WD_TEST_BOOL(test.m_values[4] == wdVec2U32(1, 2));

    test.m_bPtrAreNull = true;
    test.m_values[4] = wdVariant();
    test.m_values[5] = wdVariant();
    ret = wdVariant();
    funccall.Execute(&test, test.m_values, ret);
    WD_TEST_BOOL(ret.GetType() == wdVariantType::Int32);
    WD_TEST_BOOL(ret == 5);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Member Functions - CustomType")
  {
    wdFunctionProperty<decltype(&FunctionTest::CustomTypeFunction)> funccall("", &FunctionTest::CustomTypeFunction);
    ParamSig testSet[] = {
      ParamSig(wdGetStaticRTTI<wdVarianceTypeAngle>(), wdPropertyFlags::Class),
      ParamSig(wdGetStaticRTTI<wdVarianceTypeAngle>(), wdPropertyFlags::Class),
      ParamSig(wdGetStaticRTTI<wdVarianceTypeAngle>(), wdPropertyFlags::Class | wdPropertyFlags::Reference),
      ParamSig(wdGetStaticRTTI<wdVarianceTypeAngle>(), wdPropertyFlags::Class | wdPropertyFlags::Const | wdPropertyFlags::Reference),
      ParamSig(wdGetStaticRTTI<wdVarianceTypeAngle>(), wdPropertyFlags::Class | wdPropertyFlags::Pointer),
      ParamSig(wdGetStaticRTTI<wdVarianceTypeAngle>(), wdPropertyFlags::Class | wdPropertyFlags::Const | wdPropertyFlags::Pointer),
    };
    VerifyFunctionSignature(&funccall, wdArrayPtr<ParamSig>(testSet), ParamSig(wdGetStaticRTTI<wdVarianceTypeAngle>(), wdPropertyFlags::Class));
    WD_TEST_BOOL(funccall.GetFunctionType() == wdFunctionType::Member);

    {
      FunctionTest test;
      test.m_values.PushBack(wdVarianceTypeAngle{0.0f, wdAngle::Degree(0.0f)});
      test.m_values.PushBack(wdVarianceTypeAngle{0.1f, wdAngle::Degree(10.0f)});
      test.m_values.PushBack(wdVarianceTypeAngle{0.2f, wdAngle::Degree(20.0f)});
      test.m_values.PushBack(wdVarianceTypeAngle{0.3f, wdAngle::Degree(30.0f)});
      test.m_values.PushBack(wdVarianceTypeAngle{0.4f, wdAngle::Degree(40.0f)});
      test.m_values.PushBack(wdVarianceTypeAngle{0.5f, wdAngle::Degree(50.0f)});

      wdVariant ret;
      funccall.Execute(&test, test.m_values, ret);
      WD_TEST_BOOL(ret.GetType() == wdVariantType::TypedObject);
      WD_TEST_BOOL(ret == wdVariant(wdVarianceTypeAngle{0.6f, wdAngle::Degree(60.0f)}));
      WD_TEST_BOOL(test.m_values[2] == wdVariant(wdVarianceTypeAngle{2.0f, wdAngle::Degree(200.0f)}));
      WD_TEST_BOOL(test.m_values[4] == wdVariant(wdVarianceTypeAngle{4.0f, wdAngle::Degree(400.0f)}));

      test.m_bPtrAreNull = true;
      test.m_values[4] = wdVariant();
      test.m_values[5] = wdVariant();
      ret = wdVariant();
      funccall.Execute(&test, test.m_values, ret);
      WD_TEST_BOOL(ret.GetType() == wdVariantType::TypedObject);
      WD_TEST_BOOL(ret == wdVariant(wdVarianceTypeAngle{0.6f, wdAngle::Degree(60.0f)}));
    }

    {
      wdFunctionProperty<decltype(&FunctionTest::CustomTypeFunction2)> funccall2("", &FunctionTest::CustomTypeFunction2);

      FunctionTest test;
      wdVarianceTypeAngle v0{0.0f, wdAngle::Degree(0.0f)};
      wdVarianceTypeAngle v1{0.1f, wdAngle::Degree(10.0f)};
      wdVarianceTypeAngle v2{0.2f, wdAngle::Degree(20.0f)};
      wdVarianceTypeAngle v3{0.3f, wdAngle::Degree(30.0f)};
      wdVarianceTypeAngle v4{0.4f, wdAngle::Degree(40.0f)};
      wdVarianceTypeAngle v5{0.5f, wdAngle::Degree(50.0f)};
      test.m_values.PushBack(&v0);
      test.m_values.PushBack(&v1);
      test.m_values.PushBack(&v2);
      test.m_values.PushBack(&v3);
      test.m_values.PushBack(&v4);
      test.m_values.PushBack(&v5);

      wdVariant ret;
      funccall2.Execute(&test, test.m_values, ret);
      WD_TEST_BOOL(ret.GetType() == wdVariantType::TypedObject);
      WD_TEST_BOOL(ret == wdVariant(wdVarianceTypeAngle{0.6f, wdAngle::Degree(60.0f)}));
      WD_TEST_BOOL((*test.m_values[2].Get<wdVarianceTypeAngle*>() == wdVarianceTypeAngle{2.0f, wdAngle::Degree(200.0f)}));
      WD_TEST_BOOL((*test.m_values[4].Get<wdVarianceTypeAngle*>() == wdVarianceTypeAngle{4.0f, wdAngle::Degree(400.0f)}));

      test.m_bPtrAreNull = true;
      test.m_values[4] = wdVariant();
      test.m_values[5] = wdVariant();
      ret = wdVariant();
      funccall2.Execute(&test, test.m_values, ret);
      WD_TEST_BOOL(ret.GetType() == wdVariantType::TypedObject);
      WD_TEST_BOOL(ret == wdVariant(wdVarianceTypeAngle{0.6f, wdAngle::Degree(60.0f)}));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Member Functions - Strings")
  {
    wdFunctionProperty<decltype(&FunctionTest::StringTypeFunction)> funccall("", &FunctionTest::StringTypeFunction);
    ParamSig testSet[] = {
      ParamSig(wdGetStaticRTTI<const char*>(), wdPropertyFlags::StandardType | wdPropertyFlags::Const),
      ParamSig(wdGetStaticRTTI<wdString>(), wdPropertyFlags::StandardType | wdPropertyFlags::Reference),
      ParamSig(wdGetStaticRTTI<wdStringView>(), wdPropertyFlags::StandardType),
    };
    VerifyFunctionSignature(
      &funccall, wdArrayPtr<ParamSig>(testSet), ParamSig(wdGetStaticRTTI<const char*>(), wdPropertyFlags::StandardType | wdPropertyFlags::Const));
    WD_TEST_BOOL(funccall.GetFunctionType() == wdFunctionType::Member);

    FunctionTest test;
    test.m_values.PushBack(wdVariant(wdString("String0")));
    test.m_values.PushBack(wdVariant(wdString("String1")));
    test.m_values.PushBack(wdStringView("String2"));

    wdVariant ret;
    funccall.Execute(&test, test.m_values, ret);
    WD_TEST_BOOL(ret.GetType() == wdVariantType::String);
    WD_TEST_BOOL(ret == wdString("StringRet"));

    test.m_bPtrAreNull = true;
    test.m_values[0] = wdVariant();
    ret = wdVariant();
    funccall.Execute(&test, test.m_values, ret);
    WD_TEST_BOOL(ret.GetType() == wdVariantType::String);
    WD_TEST_BOOL(ret == wdString("StringRet"));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Member Functions - Enum")
  {
    wdFunctionProperty<decltype(&FunctionTest::EnumFunction)> funccall("", &FunctionTest::EnumFunction);
    ParamSig testSet[] = {
      ParamSig(wdGetStaticRTTI<wdExampleEnum>(), wdPropertyFlags::IsEnum),
      ParamSig(wdGetStaticRTTI<wdExampleEnum>(), wdPropertyFlags::IsEnum | wdPropertyFlags::Reference),
      ParamSig(wdGetStaticRTTI<wdExampleEnum>(), wdPropertyFlags::IsEnum | wdPropertyFlags::Const | wdPropertyFlags::Reference),
      ParamSig(wdGetStaticRTTI<wdExampleEnum>(), wdPropertyFlags::IsEnum | wdPropertyFlags::Pointer),
      ParamSig(wdGetStaticRTTI<wdExampleEnum>(), wdPropertyFlags::IsEnum | wdPropertyFlags::Const | wdPropertyFlags::Pointer),
    };
    VerifyFunctionSignature(&funccall, wdArrayPtr<ParamSig>(testSet), ParamSig(wdGetStaticRTTI<wdExampleEnum>(), wdPropertyFlags::IsEnum));
    WD_TEST_BOOL(funccall.GetFunctionType() == wdFunctionType::Member);

    FunctionTest test;
    test.m_values.PushBack((wdInt64)wdExampleEnum::Value1);
    test.m_values.PushBack((wdInt64)wdExampleEnum::Value2);
    test.m_values.PushBack((wdInt64)wdExampleEnum::Value3);
    test.m_values.PushBack((wdInt64)wdExampleEnum::Default);
    test.m_values.PushBack((wdInt64)wdExampleEnum::Value3);

    wdVariant ret;
    funccall.Execute(&test, test.m_values, ret);
    WD_TEST_BOOL(ret.GetType() == wdVariantType::Int64);
    WD_TEST_BOOL(ret == (wdInt64)wdExampleEnum::Value1);

    test.m_bPtrAreNull = true;
    test.m_values[3] = wdVariant();
    test.m_values[4] = wdVariant();
    ret = wdVariant();
    funccall.Execute(&test, test.m_values, ret);
    WD_TEST_BOOL(ret.GetType() == wdVariantType::Int64);
    WD_TEST_BOOL(ret == (wdInt64)wdExampleEnum::Value1);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Member Functions - Bitflags")
  {
    wdFunctionProperty<decltype(&FunctionTest::BitflagsFunction)> funccall("", &FunctionTest::BitflagsFunction);
    ParamSig testSet[] = {
      ParamSig(wdGetStaticRTTI<wdExampleBitflags>(), wdPropertyFlags::Bitflags),
      ParamSig(wdGetStaticRTTI<wdExampleBitflags>(), wdPropertyFlags::Bitflags | wdPropertyFlags::Reference),
      ParamSig(wdGetStaticRTTI<wdExampleBitflags>(), wdPropertyFlags::Bitflags | wdPropertyFlags::Const | wdPropertyFlags::Reference),
      ParamSig(wdGetStaticRTTI<wdExampleBitflags>(), wdPropertyFlags::Bitflags | wdPropertyFlags::Pointer),
      ParamSig(wdGetStaticRTTI<wdExampleBitflags>(), wdPropertyFlags::Bitflags | wdPropertyFlags::Const | wdPropertyFlags::Pointer),
    };
    VerifyFunctionSignature(&funccall, wdArrayPtr<ParamSig>(testSet), ParamSig(wdGetStaticRTTI<wdExampleBitflags>(), wdPropertyFlags::Bitflags));
    WD_TEST_BOOL(funccall.GetFunctionType() == wdFunctionType::Member);

    FunctionTest test;
    test.m_values.PushBack((wdInt64)(0));
    test.m_values.PushBack((wdInt64)(wdExampleBitflags::Value2));
    test.m_values.PushBack((wdInt64)(wdExampleBitflags::Value3 | wdExampleBitflags::Value2).GetValue());
    test.m_values.PushBack((wdInt64)(wdExampleBitflags::Value1 | wdExampleBitflags::Value2 | wdExampleBitflags::Value3).GetValue());
    test.m_values.PushBack((wdInt64)(wdExampleBitflags::Value3));

    wdVariant ret;
    funccall.Execute(&test, test.m_values, ret);
    WD_TEST_BOOL(ret.GetType() == wdVariantType::Int64);
    WD_TEST_BOOL(ret == (wdInt64)(wdExampleBitflags::Value1 | wdExampleBitflags::Value2).GetValue());

    test.m_bPtrAreNull = true;
    test.m_values[3] = wdVariant();
    test.m_values[4] = wdVariant();
    ret = wdVariant();
    funccall.Execute(&test, test.m_values, ret);
    WD_TEST_BOOL(ret.GetType() == wdVariantType::Int64);
    WD_TEST_BOOL(ret == (wdInt64)(wdExampleBitflags::Value1 | wdExampleBitflags::Value2).GetValue());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Member Functions - Structs")
  {
    wdFunctionProperty<decltype(&FunctionTest::StructFunction)> funccall("", &FunctionTest::StructFunction);
    ParamSig testSet[] = {
      ParamSig(wdGetStaticRTTI<wdTestStruct3>(), wdPropertyFlags::Class),
      ParamSig(wdGetStaticRTTI<wdTestStruct3>(), wdPropertyFlags::Class),
      ParamSig(wdGetStaticRTTI<wdTestStruct3>(), wdPropertyFlags::Class | wdPropertyFlags::Reference),
      ParamSig(wdGetStaticRTTI<wdTestStruct3>(), wdPropertyFlags::Class | wdPropertyFlags::Const | wdPropertyFlags::Reference),
      ParamSig(wdGetStaticRTTI<wdTestStruct3>(), wdPropertyFlags::Class | wdPropertyFlags::Pointer),
      ParamSig(wdGetStaticRTTI<wdTestStruct3>(), wdPropertyFlags::Class | wdPropertyFlags::Const | wdPropertyFlags::Pointer),
    };
    VerifyFunctionSignature(&funccall, wdArrayPtr<ParamSig>(testSet), ParamSig(wdGetStaticRTTI<wdTestStruct3>(), wdPropertyFlags::Class));
    WD_TEST_BOOL(funccall.GetFunctionType() == wdFunctionType::Member);

    FunctionTest test;
    wdTestStruct3 retS;
    retS.m_fFloat1 = 0;
    retS.m_UInt8 = 0;
    wdTestStruct3 value;
    value.m_fFloat1 = 0;
    value.m_UInt8 = 0;
    wdTestStruct3 rs;
    rs.m_fFloat1 = 42;
    wdTestStruct3 ps;
    ps.m_fFloat1 = 18;

    test.m_values.PushBack(wdVariant(&value));
    test.m_values.PushBack(wdVariant(&value));
    test.m_values.PushBack(wdVariant(&rs));
    test.m_values.PushBack(wdVariant(&value));
    test.m_values.PushBack(wdVariant(&ps));
    test.m_values.PushBack(wdVariant(&value));

    // wdVariantAdapter<wdTestStruct3 const*> aa(wdVariant(&value));
    //auto bla = wdIsStandardType<wdTestStruct3 const*>::value;

    wdVariant ret(&retS);
    funccall.Execute(&test, test.m_values, ret);
    WD_TEST_FLOAT(retS.m_fFloat1, 42, 0);
    WD_TEST_INT(retS.m_UInt8, 42);

    WD_TEST_FLOAT(rs.m_fFloat1, 999, 0);
    WD_TEST_INT(rs.m_UInt8, 666);

    WD_TEST_DOUBLE(ps.m_fFloat1, 666, 0);
    WD_TEST_INT(ps.m_UInt8, 999);

    test.m_bPtrAreNull = true;
    test.m_values[4] = wdVariant();
    test.m_values[5] = wdVariant();
    funccall.Execute(&test, test.m_values, ret);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Member Functions - Reflected Classes")
  {
    wdFunctionProperty<decltype(&FunctionTest::ReflectedClassFunction)> funccall("", &FunctionTest::ReflectedClassFunction);
    ParamSig testSet[] = {
      ParamSig(wdGetStaticRTTI<wdTestClass1>(), wdPropertyFlags::Class),
      ParamSig(wdGetStaticRTTI<wdTestClass1>(), wdPropertyFlags::Class),
      ParamSig(wdGetStaticRTTI<wdTestClass1>(), wdPropertyFlags::Class | wdPropertyFlags::Reference),
      ParamSig(wdGetStaticRTTI<wdTestClass1>(), wdPropertyFlags::Class | wdPropertyFlags::Const | wdPropertyFlags::Reference),
      ParamSig(wdGetStaticRTTI<wdTestClass1>(), wdPropertyFlags::Class | wdPropertyFlags::Pointer),
      ParamSig(wdGetStaticRTTI<wdTestClass1>(), wdPropertyFlags::Class | wdPropertyFlags::Const | wdPropertyFlags::Pointer),
    };
    VerifyFunctionSignature(&funccall, wdArrayPtr<ParamSig>(testSet), ParamSig(wdGetStaticRTTI<wdTestClass1>(), wdPropertyFlags::Class));
    WD_TEST_BOOL(funccall.GetFunctionType() == wdFunctionType::Member);

    FunctionTest test;
    wdTestClass1 retS;
    retS.m_Color = wdColor::Chocolate;
    wdTestClass1 value;
    value.m_Color = wdColor::AliceBlue;
    wdTestClass1 rs;
    rs.m_Color = wdColor::Beige;
    wdTestClass1 ps;
    ps.m_Color = wdColor::DarkBlue;

    test.m_values.PushBack(wdVariant(&value));
    test.m_values.PushBack(wdVariant(&value));
    test.m_values.PushBack(wdVariant(&rs));
    test.m_values.PushBack(wdVariant(&value));
    test.m_values.PushBack(wdVariant(&ps));
    test.m_values.PushBack(wdVariant(&value));

    rs.m_Color.SetRGB(1, 2, 3);
    rs.m_MyVector.Set(1, 2, 3);


    wdVariant ret(&retS);
    funccall.Execute(&test, test.m_values, ret);
    WD_TEST_BOOL(retS.m_Color == wdColor(42, 42, 42));
    WD_TEST_BOOL(retS.m_MyVector == wdVec3(42, 42, 42));

    WD_TEST_BOOL(rs.m_Color == wdColor(1, 2, 3));
    WD_TEST_BOOL(rs.m_MyVector == wdVec3(1, 2, 3));

    WD_TEST_BOOL(ps.m_Color == wdColor(1, 2, 3));
    WD_TEST_BOOL(ps.m_MyVector == wdVec3(1, 2, 3));

    test.m_bPtrAreNull = true;
    test.m_values[4] = wdVariant();
    test.m_values[5] = wdVariant();
    funccall.Execute(&test, test.m_values, ret);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Member Functions - Variant")
  {
    wdFunctionProperty<decltype(&FunctionTest::VariantFunction)> funccall("", &FunctionTest::VariantFunction);
    ParamSig testSet[] = {
      ParamSig(wdGetStaticRTTI<wdVariant>(), wdPropertyFlags::StandardType),
      ParamSig(wdGetStaticRTTI<wdVariant>(), wdPropertyFlags::StandardType),
      ParamSig(wdGetStaticRTTI<wdVariant>(), wdPropertyFlags::StandardType | wdPropertyFlags::Reference),
      ParamSig(wdGetStaticRTTI<wdVariant>(), wdPropertyFlags::StandardType | wdPropertyFlags::Const | wdPropertyFlags::Reference),
      ParamSig(wdGetStaticRTTI<wdVariant>(), wdPropertyFlags::StandardType | wdPropertyFlags::Pointer),
      ParamSig(wdGetStaticRTTI<wdVariant>(), wdPropertyFlags::StandardType | wdPropertyFlags::Const | wdPropertyFlags::Pointer),
    };
    VerifyFunctionSignature(&funccall, wdArrayPtr<ParamSig>(testSet), ParamSig(wdGetStaticRTTI<wdVariant>(), wdPropertyFlags::StandardType));
    WD_TEST_BOOL(funccall.GetFunctionType() == wdFunctionType::Member);

    FunctionTest test;
    test.m_values.PushBack(1);
    test.m_values.PushBack(wdVec2(2));
    test.m_values.PushBack(wdVec3(3));
    test.m_values.PushBack(wdVec4(4));
    test.m_values.PushBack(wdVec2U32(5));
    test.m_values.PushBack(wdVec3U32(6));

    wdVariant ret;
    funccall.Execute(&test, test.m_values, ret);
    WD_TEST_BOOL(ret.GetType() == wdVariantType::Int32);
    WD_TEST_BOOL(ret == 5);
    WD_TEST_BOOL(test.m_values[2] == wdVec3(1, 2, 3));
    WD_TEST_BOOL(test.m_values[4] == wdVec2U32(1, 2));

    test.m_bPtrAreNull = true;
    test.m_values[4] = wdVariant();
    test.m_values[5] = wdVariant();
    ret = wdVariant();
    funccall.Execute(&test, test.m_values, ret);
    WD_TEST_BOOL(ret.GetType() == wdVariantType::Int32);
    WD_TEST_BOOL(ret == 5);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Static Functions")
  {
    // Void return
    wdFunctionProperty<decltype(&FunctionTest::StaticFunction)> funccall("", &FunctionTest::StaticFunction);
    ParamSig testSet[] = {
      ParamSig(wdGetStaticRTTI<bool>(), wdPropertyFlags::StandardType),
      ParamSig(wdGetStaticRTTI<wdVariant>(), wdPropertyFlags::StandardType),
    };
    VerifyFunctionSignature(&funccall, wdArrayPtr<ParamSig>(testSet), ParamSig(wdGetStaticRTTI<void>(), wdPropertyFlags::Void));
    WD_TEST_BOOL(funccall.GetFunctionType() == wdFunctionType::StaticMember);

    wdDynamicArray<wdVariant> values;
    values.PushBack(true);
    values.PushBack(4.0f);
    wdVariant ret;
    funccall.Execute(nullptr, values, ret);
    WD_TEST_BOOL(ret.GetType() == wdVariantType::Invalid);

    // Zero parameter
    wdFunctionProperty<decltype(&FunctionTest::StaticFunction2)> funccall2("", &FunctionTest::StaticFunction2);
    VerifyFunctionSignature(&funccall2, wdArrayPtr<ParamSig>(), ParamSig(wdGetStaticRTTI<int>(), wdPropertyFlags::StandardType));
    WD_TEST_BOOL(funccall.GetFunctionType() == wdFunctionType::StaticMember);
    values.Clear();
    funccall2.Execute(nullptr, values, ret);
    WD_TEST_BOOL(ret.GetType() == wdVariantType::Int32);
    WD_TEST_BOOL(ret == 42);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor Functions - StandardTypes")
  {
    wdConstructorFunctionProperty<wdVec4, float, float, float, float> funccall;
    ParamSig testSet[] = {
      ParamSig(wdGetStaticRTTI<float>(), wdPropertyFlags::StandardType),
      ParamSig(wdGetStaticRTTI<float>(), wdPropertyFlags::StandardType),
      ParamSig(wdGetStaticRTTI<float>(), wdPropertyFlags::StandardType),
      ParamSig(wdGetStaticRTTI<float>(), wdPropertyFlags::StandardType),
    };
    VerifyFunctionSignature(
      &funccall, wdArrayPtr<ParamSig>(testSet), ParamSig(wdGetStaticRTTI<wdVec4>(), wdPropertyFlags::StandardType | wdPropertyFlags::Pointer));
    WD_TEST_BOOL(funccall.GetFunctionType() == wdFunctionType::Constructor);

    wdDynamicArray<wdVariant> values;
    values.PushBack(1.0f);
    values.PushBack(2.0f);
    values.PushBack(3.0f);
    values.PushBack(4.0f);
    wdVariant ret;
    funccall.Execute(nullptr, values, ret);
    WD_TEST_BOOL(ret.GetType() == wdVariantType::Vector4);
    WD_TEST_BOOL(ret == wdVec4(1.0f, 2.0f, 3.0f, 4.0f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor Functions - Struct")
  {
    wdConstructorFunctionProperty<wdTestStruct3, double, wdInt16> funccall;
    ParamSig testSet[] = {
      ParamSig(wdGetStaticRTTI<double>(), wdPropertyFlags::StandardType),
      ParamSig(wdGetStaticRTTI<wdInt16>(), wdPropertyFlags::StandardType),
    };
    VerifyFunctionSignature(
      &funccall, wdArrayPtr<ParamSig>(testSet), ParamSig(wdGetStaticRTTI<wdTestStruct3>(), wdPropertyFlags::Class | wdPropertyFlags::Pointer));
    WD_TEST_BOOL(funccall.GetFunctionType() == wdFunctionType::Constructor);

    wdDynamicArray<wdVariant> values;
    values.PushBack(59.0);
    values.PushBack((wdInt16)666);
    wdVariant ret;
    funccall.Execute(nullptr, values, ret);
    WD_TEST_BOOL(ret.GetType() == wdVariantType::TypedPointer);
    wdTestStruct3* pRet = static_cast<wdTestStruct3*>(ret.ConvertTo<void*>());
    WD_TEST_BOOL(pRet != nullptr);

    WD_TEST_FLOAT(pRet->m_fFloat1, 59.0, 0);
    WD_TEST_INT(pRet->m_UInt8, 666);
    WD_TEST_INT(pRet->GetIntPublic(), 32);

    WD_DEFAULT_DELETE(pRet);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor Functions - Reflected Classes")
  {
    // The function signature does not actually need to match the ctor 100% as long as implicit conversion is possible.
    wdConstructorFunctionProperty<wdTestClass1, const wdColor&, const wdTestStruct&> funccall;
    ParamSig testSet[] = {
      ParamSig(wdGetStaticRTTI<wdColor>(), wdPropertyFlags::StandardType | wdPropertyFlags::Const | wdPropertyFlags::Reference),
      ParamSig(wdGetStaticRTTI<wdTestStruct>(), wdPropertyFlags::Class | wdPropertyFlags::Const | wdPropertyFlags::Reference),
    };
    VerifyFunctionSignature(
      &funccall, wdArrayPtr<ParamSig>(testSet), ParamSig(wdGetStaticRTTI<wdTestClass1>(), wdPropertyFlags::Class | wdPropertyFlags::Pointer));
    WD_TEST_BOOL(funccall.GetFunctionType() == wdFunctionType::Constructor);

    wdDynamicArray<wdVariant> values;
    wdTestStruct s;
    s.m_fFloat1 = 1.0f;
    s.m_UInt8 = 255;
    values.PushBack(wdColor::CornflowerBlue);
    values.PushBack(wdVariant(&s));
    wdVariant ret;
    funccall.Execute(nullptr, values, ret);
    WD_TEST_BOOL(ret.GetType() == wdVariantType::TypedPointer);
    wdTestClass1* pRet = static_cast<wdTestClass1*>(ret.ConvertTo<void*>());
    WD_TEST_BOOL(pRet != nullptr);

    WD_TEST_BOOL(pRet->m_Color == wdColor::CornflowerBlue);
    WD_TEST_BOOL(pRet->m_Struct == s);
    WD_TEST_BOOL(pRet->m_MyVector == wdVec3(1, 2, 3));

    WD_DEFAULT_DELETE(pRet);
  }
}
