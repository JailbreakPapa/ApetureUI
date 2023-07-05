#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/IO/MemoryStream.h>
#include <Foundation/Reflection/ReflectionUtils.h>
#include <Foundation/Serialization/ReflectionSerializer.h>
#include <FoundationTest/Reflection/ReflectionTestClasses.h>


template <typename T>
void TestSerialization(const T& source)
{
  wdDefaultMemoryStreamStorage StreamStorage;

  WD_TEST_BLOCK(wdTestBlock::Enabled, "WriteObjectToDDL")
  {
    wdMemoryStreamWriter FileOut(&StreamStorage);

    wdReflectionSerializer::WriteObjectToDDL(FileOut, wdGetStaticRTTI<T>(), &source, false, wdOpenDdlWriter::TypeStringMode::Compliant);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ReadObjectPropertiesFromDDL")
  {
    wdMemoryStreamReader FileIn(&StreamStorage);
    T data;
    wdReflectionSerializer::ReadObjectPropertiesFromDDL(FileIn, *wdGetStaticRTTI<T>(), &data);

    WD_TEST_BOOL(data == source);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ReadObjectFromDDL")
  {
    wdMemoryStreamReader FileIn(&StreamStorage);

    const wdRTTI* pRtti;
    void* pObject = wdReflectionSerializer::ReadObjectFromDDL(FileIn, pRtti);

    T& c2 = *((T*)pObject);

    WD_TEST_BOOL(c2 == source);

    if (pObject)
    {
      pRtti->GetAllocator()->Deallocate(pObject);
    }
  }

  wdDefaultMemoryStreamStorage StreamStorageBinary;
  WD_TEST_BLOCK(wdTestBlock::Enabled, "WriteObjectToBinary")
  {
    wdMemoryStreamWriter FileOut(&StreamStorageBinary);

    wdReflectionSerializer::WriteObjectToBinary(FileOut, wdGetStaticRTTI<T>(), &source);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ReadObjectPropertiesFromBinary")
  {
    wdMemoryStreamReader FileIn(&StreamStorageBinary);
    T data;
    wdReflectionSerializer::ReadObjectPropertiesFromBinary(FileIn, *wdGetStaticRTTI<T>(), &data);

    WD_TEST_BOOL(data == source);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ReadObjectFromBinary")
  {
    wdMemoryStreamReader FileIn(&StreamStorageBinary);

    const wdRTTI* pRtti;
    void* pObject = wdReflectionSerializer::ReadObjectFromBinary(FileIn, pRtti);

    T& c2 = *((T*)pObject);

    WD_TEST_BOOL(c2 == source);

    if (pObject)
    {
      pRtti->GetAllocator()->Deallocate(pObject);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Clone")
  {
    {
      T clone;
      wdReflectionSerializer::Clone(&source, &clone, wdGetStaticRTTI<T>());
      WD_TEST_BOOL(clone == source);
      WD_TEST_BOOL(wdReflectionUtils::IsEqual(&clone, &source, wdGetStaticRTTI<T>()));
    }

    {
      T* pClone = wdReflectionSerializer::Clone(&source);
      WD_TEST_BOOL(*pClone == source);
      WD_TEST_BOOL(wdReflectionUtils::IsEqual(pClone, &source));
      wdGetStaticRTTI<T>()->GetAllocator()->Deallocate(pClone);
    }
  }
}


WD_CREATE_SIMPLE_TEST_GROUP(Reflection);

WD_CREATE_SIMPLE_TEST(Reflection, Types)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Iterate All")
  {
    bool bFoundStruct = false;
    bool bFoundClass1 = false;
    bool bFoundClass2 = false;

    wdRTTI* pRtti = wdRTTI::GetFirstInstance();

    while (pRtti)
    {
      if (wdStringUtils::IsEqual(pRtti->GetTypeName(), "wdTestStruct"))
        bFoundStruct = true;
      if (wdStringUtils::IsEqual(pRtti->GetTypeName(), "wdTestClass1"))
        bFoundClass1 = true;
      if (wdStringUtils::IsEqual(pRtti->GetTypeName(), "wdTestClass2"))
        bFoundClass2 = true;

      WD_TEST_STRING(pRtti->GetPluginName(), "Static");

      pRtti = pRtti->GetNextInstance();
    }

    WD_TEST_BOOL(bFoundStruct);
    WD_TEST_BOOL(bFoundClass1);
    WD_TEST_BOOL(bFoundClass2);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsDerivedFrom")
  {
    wdDynamicArray<const wdRTTI*> allTypes;
    for (const wdRTTI* pRtti = wdRTTI::GetFirstInstance(); pRtti; pRtti = pRtti->GetNextInstance())
    {
      allTypes.PushBack(pRtti);
    }

    // ground truth - traversing up the parent list
    auto ManualIsDerivedFrom = [](const wdRTTI* t, const wdRTTI* pBaseType) -> bool {
      while (t != nullptr)
      {
        if (t == pBaseType)
          return true;

        t = t->GetParentType();
      }

      return false;
    };

    // test each type against every other:
    for (const wdRTTI* typeA : allTypes)
    {
      for (const wdRTTI* typeB : allTypes)
      {
        bool derived = typeA->IsDerivedFrom(typeB);
        bool manualCheck = ManualIsDerivedFrom(typeA, typeB);
        WD_TEST_BOOL(derived == manualCheck);
      }
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "PropertyFlags")
  {
    WD_TEST_BOOL(wdPropertyFlags::GetParameterFlags<void>() == (wdPropertyFlags::Void));
    WD_TEST_BOOL(wdPropertyFlags::GetParameterFlags<const char*>() == (wdPropertyFlags::StandardType | wdPropertyFlags::Const));
    WD_TEST_BOOL(wdPropertyFlags::GetParameterFlags<int>() == wdPropertyFlags::StandardType);
    WD_TEST_BOOL(wdPropertyFlags::GetParameterFlags<int&>() == (wdPropertyFlags::StandardType | wdPropertyFlags::Reference));
    WD_TEST_BOOL(wdPropertyFlags::GetParameterFlags<int*>() == (wdPropertyFlags::StandardType | wdPropertyFlags::Pointer));

    WD_TEST_BOOL(wdPropertyFlags::GetParameterFlags<const int>() == (wdPropertyFlags::StandardType | wdPropertyFlags::Const));
    WD_TEST_BOOL(
      wdPropertyFlags::GetParameterFlags<const int&>() == (wdPropertyFlags::StandardType | wdPropertyFlags::Reference | wdPropertyFlags::Const));
    WD_TEST_BOOL(
      wdPropertyFlags::GetParameterFlags<const int*>() == (wdPropertyFlags::StandardType | wdPropertyFlags::Pointer | wdPropertyFlags::Const));

    WD_TEST_BOOL(wdPropertyFlags::GetParameterFlags<wdVariant>() == (wdPropertyFlags::StandardType));

    WD_TEST_BOOL(wdPropertyFlags::GetParameterFlags<wdExampleEnum::Enum>() == wdPropertyFlags::IsEnum);
    WD_TEST_BOOL(wdPropertyFlags::GetParameterFlags<wdEnum<wdExampleEnum>>() == wdPropertyFlags::IsEnum);
    WD_TEST_BOOL(wdPropertyFlags::GetParameterFlags<wdBitflags<wdExampleBitflags>>() == wdPropertyFlags::Bitflags);

    WD_TEST_BOOL(wdPropertyFlags::GetParameterFlags<wdTestStruct3>() == wdPropertyFlags::Class);
    WD_TEST_BOOL(wdPropertyFlags::GetParameterFlags<wdTestClass2>() == wdPropertyFlags::Class);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "TypeFlags")
  {
    WD_TEST_INT(wdGetStaticRTTI<bool>()->GetTypeFlags().GetValue(), wdTypeFlags::StandardType);
    WD_TEST_INT(wdGetStaticRTTI<wdUuid>()->GetTypeFlags().GetValue(), wdTypeFlags::StandardType);
    WD_TEST_INT(wdGetStaticRTTI<const char*>()->GetTypeFlags().GetValue(), wdTypeFlags::StandardType);
    WD_TEST_INT(wdGetStaticRTTI<wdString>()->GetTypeFlags().GetValue(), wdTypeFlags::StandardType);
    WD_TEST_INT(wdGetStaticRTTI<wdMat4>()->GetTypeFlags().GetValue(), wdTypeFlags::StandardType);
    WD_TEST_INT(wdGetStaticRTTI<wdVariant>()->GetTypeFlags().GetValue(), wdTypeFlags::StandardType);

    WD_TEST_INT(wdGetStaticRTTI<wdAbstractTestClass>()->GetTypeFlags().GetValue(), (wdTypeFlags::Class | wdTypeFlags::Abstract).GetValue());
    WD_TEST_INT(wdGetStaticRTTI<wdAbstractTestStruct>()->GetTypeFlags().GetValue(), (wdTypeFlags::Class | wdTypeFlags::Abstract).GetValue());

    WD_TEST_INT(wdGetStaticRTTI<wdTestStruct3>()->GetTypeFlags().GetValue(), wdTypeFlags::Class);
    WD_TEST_INT(wdGetStaticRTTI<wdTestClass2>()->GetTypeFlags().GetValue(), wdTypeFlags::Class);

    WD_TEST_INT(wdGetStaticRTTI<wdExampleEnum>()->GetTypeFlags().GetValue(), wdTypeFlags::IsEnum);
    WD_TEST_INT(wdGetStaticRTTI<wdExampleBitflags>()->GetTypeFlags().GetValue(), wdTypeFlags::Bitflags);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "FindTypeByName")
  {
    wdRTTI* pFloat = wdRTTI::FindTypeByName("float");
    WD_TEST_BOOL(pFloat != nullptr);
    WD_TEST_STRING(pFloat->GetTypeName(), "float");

    wdRTTI* pStruct = wdRTTI::FindTypeByName("wdTestStruct");
    WD_TEST_BOOL(pStruct != nullptr);
    WD_TEST_STRING(pStruct->GetTypeName(), "wdTestStruct");

    wdRTTI* pClass2 = wdRTTI::FindTypeByName("wdTestClass2");
    WD_TEST_BOOL(pClass2 != nullptr);
    WD_TEST_STRING(pClass2->GetTypeName(), "wdTestClass2");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "FindTypeByNameHash")
  {
    wdRTTI* pFloat = wdRTTI::FindTypeByName("float");
    wdRTTI* pFloat2 = wdRTTI::FindTypeByNameHash(pFloat->GetTypeNameHash());
    WD_TEST_BOOL(pFloat == pFloat2);

    wdRTTI* pStruct = wdRTTI::FindTypeByName("wdTestStruct");
    wdRTTI* pStruct2 = wdRTTI::FindTypeByNameHash(pStruct->GetTypeNameHash());
    WD_TEST_BOOL(pStruct == pStruct2);

    wdRTTI* pClass = wdRTTI::FindTypeByName("wdTestClass2");
    wdRTTI* pClass2 = wdRTTI::FindTypeByNameHash(pClass->GetTypeNameHash());
    WD_TEST_BOOL(pClass == pClass2);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetProperties")
  {
    {
      wdRTTI* pType = wdRTTI::FindTypeByName("wdTestStruct");

      auto Props = pType->GetProperties();
      WD_TEST_INT(Props.GetCount(), 9);
      WD_TEST_STRING(Props[0]->GetPropertyName(), "Float");
      WD_TEST_STRING(Props[1]->GetPropertyName(), "Vector");
      WD_TEST_STRING(Props[2]->GetPropertyName(), "Int");
      WD_TEST_STRING(Props[3]->GetPropertyName(), "UInt8");
      WD_TEST_STRING(Props[4]->GetPropertyName(), "Variant");
      WD_TEST_STRING(Props[5]->GetPropertyName(), "Angle");
      WD_TEST_STRING(Props[6]->GetPropertyName(), "DataBuffer");
      WD_TEST_STRING(Props[7]->GetPropertyName(), "vVec3I");
      WD_TEST_STRING(Props[8]->GetPropertyName(), "VarianceAngle");
    }

    {
      wdRTTI* pType = wdRTTI::FindTypeByName("wdTestClass2");

      auto Props = pType->GetProperties();
      WD_TEST_INT(Props.GetCount(), 6);
      WD_TEST_STRING(Props[0]->GetPropertyName(), "Text");
      WD_TEST_STRING(Props[1]->GetPropertyName(), "Time");
      WD_TEST_STRING(Props[2]->GetPropertyName(), "Enum");
      WD_TEST_STRING(Props[3]->GetPropertyName(), "Bitflags");
      WD_TEST_STRING(Props[4]->GetPropertyName(), "Array");
      WD_TEST_STRING(Props[5]->GetPropertyName(), "Variant");

      wdHybridArray<wdAbstractProperty*, 32> AllProps;
      pType->GetAllProperties(AllProps);

      WD_TEST_INT(AllProps.GetCount(), 9);
      WD_TEST_STRING(AllProps[0]->GetPropertyName(), "SubStruct");
      WD_TEST_STRING(AllProps[1]->GetPropertyName(), "Color");
      WD_TEST_STRING(AllProps[2]->GetPropertyName(), "SubVector");
      WD_TEST_STRING(AllProps[3]->GetPropertyName(), "Text");
      WD_TEST_STRING(AllProps[4]->GetPropertyName(), "Time");
      WD_TEST_STRING(AllProps[5]->GetPropertyName(), "Enum");
      WD_TEST_STRING(AllProps[6]->GetPropertyName(), "Bitflags");
      WD_TEST_STRING(AllProps[7]->GetPropertyName(), "Array");
      WD_TEST_STRING(AllProps[8]->GetPropertyName(), "Variant");
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Casts")
  {
    wdTestClass2 test;
    wdTestClass1* pTestClass1 = &test;
    const wdTestClass1* pConstTestClass1 = &test;

    wdTestClass2* pTestClass2 = wdStaticCast<wdTestClass2*>(pTestClass1);
    const wdTestClass2* pConstTestClass2 = wdStaticCast<const wdTestClass2*>(pConstTestClass1);

    pTestClass2 = wdDynamicCast<wdTestClass2*>(pTestClass1);
    pConstTestClass2 = wdDynamicCast<const wdTestClass2*>(pConstTestClass1);
    WD_TEST_BOOL(pTestClass2 != nullptr);
    WD_TEST_BOOL(pConstTestClass2 != nullptr);

    wdTestClass1 otherTest;
    pTestClass1 = &otherTest;
    pConstTestClass1 = &otherTest;

    pTestClass2 = wdDynamicCast<wdTestClass2*>(pTestClass1);
    pConstTestClass2 = wdDynamicCast<const wdTestClass2*>(pConstTestClass1);
    WD_TEST_BOOL(pTestClass2 == nullptr);
    WD_TEST_BOOL(pConstTestClass2 == nullptr);
  }

#if WD_ENABLED(WD_SUPPORTS_DYNAMIC_PLUGINS) && WD_ENABLED(WD_COMPILE_ENGINE_AS_DLL)

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Types From Plugin")
  {
    wdResult loadPlugin = wdPlugin::LoadPlugin(wdFoundationTest_Plugin1);
    WD_TEST_BOOL(loadPlugin == WD_SUCCESS);

    if (loadPlugin.Failed())
      return;

    wdRTTI* pStruct2 = wdRTTI::FindTypeByName("wdTestStruct2");
    WD_TEST_BOOL(pStruct2 != nullptr);

    if (pStruct2)
    {
      WD_TEST_STRING(pStruct2->GetTypeName(), "wdTestStruct2");
    }

    bool bFoundStruct2 = false;

    wdRTTI* pRtti = wdRTTI::GetFirstInstance();

    while (pRtti)
    {
      if (wdStringUtils::IsEqual(pRtti->GetTypeName(), "wdTestStruct2"))
      {
        bFoundStruct2 = true;

        WD_TEST_STRING(pRtti->GetPluginName(), wdFoundationTest_Plugin1);

        void* pInstance = pRtti->GetAllocator()->Allocate<void>();
        WD_TEST_BOOL(pInstance != nullptr);

        wdAbstractProperty* pProp = pRtti->FindPropertyByName("Float2");

        WD_TEST_BOOL(pProp != nullptr);

        WD_TEST_BOOL(pProp->GetCategory() == wdPropertyCategory::Member);
        wdAbstractMemberProperty* pAbsMember = (wdAbstractMemberProperty*)pProp;

        WD_TEST_BOOL(pAbsMember->GetSpecificType() == wdGetStaticRTTI<float>());

        wdTypedMemberProperty<float>* pMember = (wdTypedMemberProperty<float>*)pAbsMember;

        WD_TEST_FLOAT(pMember->GetValue(pInstance), 42.0f, 0);
        pMember->SetValue(pInstance, 43.0f);
        WD_TEST_FLOAT(pMember->GetValue(pInstance), 43.0f, 0);

        pRtti->GetAllocator()->Deallocate(pInstance);
      }
      else
      {
        WD_TEST_STRING(pRtti->GetPluginName(), "Static");
      }

      pRtti = pRtti->GetNextInstance();
    }

    WD_TEST_BOOL(bFoundStruct2);

    wdPlugin::UnloadAllPlugins();
  }
#endif
}


WD_CREATE_SIMPLE_TEST(Reflection, Hierarchies)
{
  wdTestClass2Allocator::m_iAllocs = 0;
  wdTestClass2Allocator::m_iDeallocs = 0;

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdTestStruct")
  {
    const wdRTTI* pRtti = wdGetStaticRTTI<wdTestStruct>();

    WD_TEST_STRING(pRtti->GetTypeName(), "wdTestStruct");
    WD_TEST_INT(pRtti->GetTypeSize(), sizeof(wdTestStruct));
    WD_TEST_BOOL(pRtti->GetVariantType() == wdVariant::Type::Invalid);

    WD_TEST_BOOL(pRtti->GetParentType() == nullptr);

    WD_TEST_BOOL(pRtti->GetAllocator()->CanAllocate());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdTestClass1")
  {
    const wdRTTI* pRtti = wdGetStaticRTTI<wdTestClass1>();

    WD_TEST_STRING(pRtti->GetTypeName(), "wdTestClass1");
    WD_TEST_INT(pRtti->GetTypeSize(), sizeof(wdTestClass1));
    WD_TEST_BOOL(pRtti->GetVariantType() == wdVariant::Type::Invalid);

    WD_TEST_BOOL(pRtti->GetParentType() == wdGetStaticRTTI<wdReflectedClass>());

    WD_TEST_BOOL(pRtti->GetAllocator()->CanAllocate());

    wdTestClass1* pInstance = pRtti->GetAllocator()->Allocate<wdTestClass1>();
    WD_TEST_BOOL(pInstance != nullptr);

    WD_TEST_BOOL(pInstance->GetDynamicRTTI() == wdGetStaticRTTI<wdTestClass1>());
    pInstance->GetDynamicRTTI()->GetAllocator()->Deallocate(pInstance);

    WD_TEST_BOOL(pRtti->IsDerivedFrom<wdReflectedClass>());
    WD_TEST_BOOL(pRtti->IsDerivedFrom(wdGetStaticRTTI<wdReflectedClass>()));

    WD_TEST_BOOL(pRtti->IsDerivedFrom<wdTestClass1>());
    WD_TEST_BOOL(pRtti->IsDerivedFrom(wdGetStaticRTTI<wdTestClass1>()));

    WD_TEST_BOOL(!pRtti->IsDerivedFrom<wdVec3>());
    WD_TEST_BOOL(!pRtti->IsDerivedFrom(wdGetStaticRTTI<wdVec3>()));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdTestClass2")
  {
    const wdRTTI* pRtti = wdGetStaticRTTI<wdTestClass2>();

    WD_TEST_STRING(pRtti->GetTypeName(), "wdTestClass2");
    WD_TEST_INT(pRtti->GetTypeSize(), sizeof(wdTestClass2));
    WD_TEST_BOOL(pRtti->GetVariantType() == wdVariant::Type::Invalid);

    WD_TEST_BOOL(pRtti->GetParentType() == wdGetStaticRTTI<wdTestClass1>());

    WD_TEST_BOOL(pRtti->GetAllocator()->CanAllocate());

    WD_TEST_INT(wdTestClass2Allocator::m_iAllocs, 0);
    WD_TEST_INT(wdTestClass2Allocator::m_iDeallocs, 0);

    wdTestClass2* pInstance = pRtti->GetAllocator()->Allocate<wdTestClass2>();
    WD_TEST_BOOL(pInstance != nullptr);

    WD_TEST_BOOL(pInstance->GetDynamicRTTI() == wdGetStaticRTTI<wdTestClass2>());

    WD_TEST_INT(wdTestClass2Allocator::m_iAllocs, 1);
    WD_TEST_INT(wdTestClass2Allocator::m_iDeallocs, 0);

    pInstance->GetDynamicRTTI()->GetAllocator()->Deallocate(pInstance);

    WD_TEST_INT(wdTestClass2Allocator::m_iAllocs, 1);
    WD_TEST_INT(wdTestClass2Allocator::m_iDeallocs, 1);

    WD_TEST_BOOL(pRtti->IsDerivedFrom<wdTestClass1>());
    WD_TEST_BOOL(pRtti->IsDerivedFrom(wdGetStaticRTTI<wdTestClass1>()));

    WD_TEST_BOOL(pRtti->IsDerivedFrom<wdTestClass2>());
    WD_TEST_BOOL(pRtti->IsDerivedFrom(wdGetStaticRTTI<wdTestClass2>()));

    WD_TEST_BOOL(pRtti->IsDerivedFrom<wdReflectedClass>());
    WD_TEST_BOOL(pRtti->IsDerivedFrom(wdGetStaticRTTI<wdReflectedClass>()));

    WD_TEST_BOOL(!pRtti->IsDerivedFrom<wdVec3>());
    WD_TEST_BOOL(!pRtti->IsDerivedFrom(wdGetStaticRTTI<wdVec3>()));
  }
}


template <typename T, typename T2>
void TestMemberProperty(const char* szPropName, void* pObject, const wdRTTI* pRtti, wdBitflags<wdPropertyFlags> expectedFlags, T2 expectedValue, T2 testValue, bool bTestDefaultValue = true)
{
  wdAbstractProperty* pProp = pRtti->FindPropertyByName(szPropName);
  WD_TEST_BOOL(pProp != nullptr);

  WD_TEST_BOOL(pProp->GetCategory() == wdPropertyCategory::Member);

  WD_TEST_BOOL(pProp->GetSpecificType() == wdGetStaticRTTI<T>());
  wdTypedMemberProperty<T>* pMember = (wdTypedMemberProperty<T>*)pProp;

  WD_TEST_INT(pMember->GetFlags().GetValue(), expectedFlags.GetValue());

  T value = pMember->GetValue(pObject);
  WD_TEST_BOOL(expectedValue == value);

  if (bTestDefaultValue)
  {
    // Default value
    wdVariant defaultValue = wdReflectionUtils::GetDefaultValue(pProp);
    WD_TEST_BOOL(wdVariant(expectedValue) == defaultValue);
  }

  if (!pMember->GetFlags().IsSet(wdPropertyFlags::ReadOnly))
  {
    pMember->SetValue(pObject, testValue);

    WD_TEST_BOOL(testValue == pMember->GetValue(pObject));

    wdReflectionUtils::SetMemberPropertyValue(pMember, pObject, wdVariant(expectedValue));
    wdVariant res = wdReflectionUtils::GetMemberPropertyValue(pMember, pObject);

    WD_TEST_BOOL(res == wdVariant(expectedValue));
    WD_TEST_BOOL(res != wdVariant(testValue));

    wdReflectionUtils::SetMemberPropertyValue(pMember, pObject, wdVariant(testValue));
    res = wdReflectionUtils::GetMemberPropertyValue(pMember, pObject);

    WD_TEST_BOOL(res != wdVariant(expectedValue));
    WD_TEST_BOOL(res == wdVariant(testValue));
  }
}

WD_CREATE_SIMPLE_TEST(Reflection, MemberProperties)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdTestStruct")
  {
    wdTestStruct data;
    const wdRTTI* pRtti = wdGetStaticRTTI<wdTestStruct>();

    TestMemberProperty<float>("Float", &data, pRtti, wdPropertyFlags::StandardType, 1.1f, 5.0f);
    TestMemberProperty<wdInt32>("Int", &data, pRtti, wdPropertyFlags::StandardType, 2, -8);
    TestMemberProperty<wdVec3>("Vector", &data, pRtti, wdPropertyFlags::StandardType | wdPropertyFlags::ReadOnly, wdVec3(3, 4, 5),
      wdVec3(0, -1.0f, 3.14f));
    TestMemberProperty<wdVariant>("Variant", &data, pRtti, wdPropertyFlags::StandardType, wdVariant("Test"),
      wdVariant(wdVec3(0, -1.0f, 3.14f)));
    TestMemberProperty<wdAngle>("Angle", &data, pRtti, wdPropertyFlags::StandardType, wdAngle::Degree(0.5f), wdAngle::Degree(1.0f));
    wdVarianceTypeAngle expectedVA = {0.5f, wdAngle::Degree(90.0f)};
    wdVarianceTypeAngle testVA = {0.1f, wdAngle::Degree(45.0f)};
    TestMemberProperty<wdVarianceTypeAngle>("VarianceAngle", &data, pRtti, wdPropertyFlags::Class, expectedVA, testVA);

    wdDataBuffer expected;
    expected.PushBack(255);
    expected.PushBack(0);
    expected.PushBack(127);

    wdDataBuffer newValue;
    newValue.PushBack(1);
    newValue.PushBack(2);

    TestMemberProperty<wdDataBuffer>("DataBuffer", &data, pRtti, wdPropertyFlags::StandardType, expected, newValue);
    TestMemberProperty<wdVec3I32>("vVec3I", &data, pRtti, wdPropertyFlags::StandardType, wdVec3I32(1, 2, 3), wdVec3I32(5, 6, 7));

    TestSerialization<wdTestStruct>(data);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdTestClass2")
  {
    wdTestClass2 Instance;
    const wdRTTI* pRtti = wdGetStaticRTTI<wdTestClass2>();

    {
      TestMemberProperty<const char*>("Text", &Instance, pRtti, wdPropertyFlags::StandardType | wdPropertyFlags::Const, wdString("Legen"), wdString("dary"));
      wdAbstractProperty* pProp = pRtti->FindPropertyByName("SubVector", false);
      WD_TEST_BOOL(pProp == nullptr);
    }

    {
      TestMemberProperty<wdVec3>("SubVector", &Instance, pRtti, wdPropertyFlags::StandardType | wdPropertyFlags::ReadOnly, wdVec3(3, 4, 5), wdVec3(3, 4, 5));
      wdAbstractProperty* pProp = pRtti->FindPropertyByName("SubStruct", false);
      WD_TEST_BOOL(pProp == nullptr);
    }

    {
      wdAbstractProperty* pProp = pRtti->FindPropertyByName("SubStruct");
      WD_TEST_BOOL(pProp != nullptr);

      WD_TEST_BOOL(pProp->GetCategory() == wdPropertyCategory::Member);
      wdAbstractMemberProperty* pAbs = (wdAbstractMemberProperty*)pProp;

      const wdRTTI* pStruct = pAbs->GetSpecificType();
      void* pSubStruct = pAbs->GetPropertyPointer(&Instance);

      WD_TEST_BOOL(pSubStruct != nullptr);

      TestMemberProperty<float>("Float", pSubStruct, pStruct, wdPropertyFlags::StandardType, 33.3f, 44.4f, false);
    }

    TestSerialization<wdTestClass2>(Instance);
  }
}


WD_CREATE_SIMPLE_TEST(Reflection, Enum)
{
  const wdRTTI* pEnumRTTI = wdGetStaticRTTI<wdExampleEnum>();
  const wdRTTI* pRTTI = wdGetStaticRTTI<wdTestEnumStruct>();

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Enum Constants")
  {
    WD_TEST_BOOL(pEnumRTTI->IsDerivedFrom<wdEnumBase>());
    auto props = pEnumRTTI->GetProperties();
    WD_TEST_INT(props.GetCount(), 4); // Default + 3

    for (auto pProp : props)
    {
      WD_TEST_BOOL(pProp->GetCategory() == wdPropertyCategory::Constant);
      wdAbstractConstantProperty* pConstantProp = static_cast<wdAbstractConstantProperty*>(pProp);
      WD_TEST_BOOL(pConstantProp->GetSpecificType() == wdGetStaticRTTI<wdInt8>());
    }
    WD_TEST_INT(wdExampleEnum::Default, wdReflectionUtils::DefaultEnumerationValue(pEnumRTTI));

    WD_TEST_STRING(props[0]->GetPropertyName(), "wdExampleEnum::Default");
    WD_TEST_STRING(props[1]->GetPropertyName(), "wdExampleEnum::Value1");
    WD_TEST_STRING(props[2]->GetPropertyName(), "wdExampleEnum::Value2");
    WD_TEST_STRING(props[3]->GetPropertyName(), "wdExampleEnum::Value3");

    auto pTypedConstantProp0 = static_cast<wdTypedConstantProperty<wdInt8>*>(props[0]);
    auto pTypedConstantProp1 = static_cast<wdTypedConstantProperty<wdInt8>*>(props[1]);
    auto pTypedConstantProp2 = static_cast<wdTypedConstantProperty<wdInt8>*>(props[2]);
    auto pTypedConstantProp3 = static_cast<wdTypedConstantProperty<wdInt8>*>(props[3]);
    WD_TEST_INT(pTypedConstantProp0->GetValue(), wdExampleEnum::Default);
    WD_TEST_INT(pTypedConstantProp1->GetValue(), wdExampleEnum::Value1);
    WD_TEST_INT(pTypedConstantProp2->GetValue(), wdExampleEnum::Value2);
    WD_TEST_INT(pTypedConstantProp3->GetValue(), wdExampleEnum::Value3);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Enum Property")
  {
    wdTestEnumStruct data;
    auto props = pRTTI->GetProperties();
    WD_TEST_INT(props.GetCount(), 4);

    for (auto pProp : props)
    {
      WD_TEST_BOOL(pProp->GetCategory() == wdPropertyCategory::Member);
      wdAbstractMemberProperty* pMemberProp = static_cast<wdAbstractMemberProperty*>(pProp);
      WD_TEST_INT(pMemberProp->GetFlags().GetValue(), wdPropertyFlags::IsEnum);
      WD_TEST_BOOL(pMemberProp->GetSpecificType() == pEnumRTTI);
      wdAbstractEnumerationProperty* pEnumProp = static_cast<wdAbstractEnumerationProperty*>(pProp);
      WD_TEST_BOOL(pEnumProp->GetValue(&data) == wdExampleEnum::Value1);

      const wdRTTI* pEnumPropertyRTTI = pEnumProp->GetSpecificType();
      // Set and get all valid enum values.
      for (auto pProp2 : pEnumPropertyRTTI->GetProperties().GetSubArray(1))
      {
        wdTypedConstantProperty<wdInt8>* pConstantProp = static_cast<wdTypedConstantProperty<wdInt8>*>(pProp2);
        pEnumProp->SetValue(&data, pConstantProp->GetValue());
        WD_TEST_INT(pEnumProp->GetValue(&data), pConstantProp->GetValue());

        // Enum <-> string
        wdStringBuilder sValue;
        WD_TEST_BOOL(wdReflectionUtils::EnumerationToString(pEnumPropertyRTTI, pConstantProp->GetValue(), sValue));
        WD_TEST_STRING(sValue, pConstantProp->GetPropertyName());

        // Setting the value via a string also works.
        pEnumProp->SetValue(&data, wdExampleEnum::Value1);
        wdReflectionUtils::SetMemberPropertyValue(pEnumProp, &data, sValue.GetData());
        WD_TEST_INT(pEnumProp->GetValue(&data), pConstantProp->GetValue());

        wdInt64 iValue = 0;
        WD_TEST_BOOL(wdReflectionUtils::StringToEnumeration(pEnumPropertyRTTI, sValue, iValue));
        WD_TEST_INT(iValue, pConstantProp->GetValue());

        // Testing the short enum name version
        WD_TEST_BOOL(wdReflectionUtils::EnumerationToString(
          pEnumPropertyRTTI, pConstantProp->GetValue(), sValue, wdReflectionUtils::EnumConversionMode::ValueNameOnly));
        WD_TEST_BOOL(sValue.IsEqual(pConstantProp->GetPropertyName()) ||
                     sValue.IsEqual(wdStringUtils::FindLastSubString(pConstantProp->GetPropertyName(), "::") + 2));

        WD_TEST_BOOL(wdReflectionUtils::StringToEnumeration(pEnumPropertyRTTI, sValue, iValue));
        WD_TEST_INT(iValue, pConstantProp->GetValue());

        // Testing the short enum name version
        WD_TEST_BOOL(wdReflectionUtils::EnumerationToString(
          pEnumPropertyRTTI, pConstantProp->GetValue(), sValue, wdReflectionUtils::EnumConversionMode::ValueNameOnly));
        WD_TEST_BOOL(sValue.IsEqual(pConstantProp->GetPropertyName()) ||
                     sValue.IsEqual(wdStringUtils::FindLastSubString(pConstantProp->GetPropertyName(), "::") + 2));

        WD_TEST_BOOL(wdReflectionUtils::StringToEnumeration(pEnumPropertyRTTI, sValue, iValue));
        WD_TEST_INT(iValue, pConstantProp->GetValue());

        WD_TEST_INT(iValue, wdReflectionUtils::MakeEnumerationValid(pEnumPropertyRTTI, iValue));
        WD_TEST_INT(wdExampleEnum::Default, wdReflectionUtils::MakeEnumerationValid(pEnumPropertyRTTI, iValue + 666));
      }
    }

    WD_TEST_BOOL(data.m_enum == wdExampleEnum::Value3);
    WD_TEST_BOOL(data.m_enumClass == wdExampleEnum::Value3);

    WD_TEST_BOOL(data.GetEnum() == wdExampleEnum::Value3);
    WD_TEST_BOOL(data.GetEnumClass() == wdExampleEnum::Value3);

    TestSerialization<wdTestEnumStruct>(data);
  }
}


WD_CREATE_SIMPLE_TEST(Reflection, Bitflags)
{
  const wdRTTI* pBitflagsRTTI = wdGetStaticRTTI<wdExampleBitflags>();
  const wdRTTI* pRTTI = wdGetStaticRTTI<wdTestBitflagsStruct>();

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Bitflags Constants")
  {
    WD_TEST_BOOL(pBitflagsRTTI->IsDerivedFrom<wdBitflagsBase>());
    auto props = pBitflagsRTTI->GetProperties();
    WD_TEST_INT(props.GetCount(), 4); // Default + 3

    for (auto pProp : props)
    {
      WD_TEST_BOOL(pProp->GetCategory() == wdPropertyCategory::Constant);
      WD_TEST_BOOL(pProp->GetSpecificType() == wdGetStaticRTTI<wdUInt64>());
    }
    WD_TEST_INT(wdExampleBitflags::Default, wdReflectionUtils::DefaultEnumerationValue(pBitflagsRTTI));

    WD_TEST_STRING(props[0]->GetPropertyName(), "wdExampleBitflags::Default");
    WD_TEST_STRING(props[1]->GetPropertyName(), "wdExampleBitflags::Value1");
    WD_TEST_STRING(props[2]->GetPropertyName(), "wdExampleBitflags::Value2");
    WD_TEST_STRING(props[3]->GetPropertyName(), "wdExampleBitflags::Value3");

    auto pTypedConstantProp0 = static_cast<wdTypedConstantProperty<wdUInt64>*>(props[0]);
    auto pTypedConstantProp1 = static_cast<wdTypedConstantProperty<wdUInt64>*>(props[1]);
    auto pTypedConstantProp2 = static_cast<wdTypedConstantProperty<wdUInt64>*>(props[2]);
    auto pTypedConstantProp3 = static_cast<wdTypedConstantProperty<wdUInt64>*>(props[3]);
    WD_TEST_BOOL(pTypedConstantProp0->GetValue() == wdExampleBitflags::Default);
    WD_TEST_BOOL(pTypedConstantProp1->GetValue() == wdExampleBitflags::Value1);
    WD_TEST_BOOL(pTypedConstantProp2->GetValue() == wdExampleBitflags::Value2);
    WD_TEST_BOOL(pTypedConstantProp3->GetValue() == wdExampleBitflags::Value3);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Bitflags Property")
  {
    wdTestBitflagsStruct data;
    auto props = pRTTI->GetProperties();
    WD_TEST_INT(props.GetCount(), 2);

    for (auto pProp : props)
    {
      WD_TEST_BOOL(pProp->GetCategory() == wdPropertyCategory::Member);
      WD_TEST_BOOL(pProp->GetSpecificType() == pBitflagsRTTI);
      WD_TEST_INT(pProp->GetFlags().GetValue(), wdPropertyFlags::Bitflags);
      wdAbstractEnumerationProperty* pBitflagsProp = static_cast<wdAbstractEnumerationProperty*>(pProp);
      WD_TEST_BOOL(pBitflagsProp->GetValue(&data) == wdExampleBitflags::Value1);

      const wdRTTI* pBitflagsPropertyRTTI = pBitflagsProp->GetSpecificType();

      // Set and get all valid bitflags values. (skip default value)
      wdUInt64 constants[] = {static_cast<wdTypedConstantProperty<wdUInt64>*>(pBitflagsPropertyRTTI->GetProperties()[1])->GetValue(),
        static_cast<wdTypedConstantProperty<wdUInt64>*>(pBitflagsPropertyRTTI->GetProperties()[2])->GetValue(),
        static_cast<wdTypedConstantProperty<wdUInt64>*>(pBitflagsPropertyRTTI->GetProperties()[3])->GetValue()};

      const char* stringValues[] = {"",
        "wdExampleBitflags::Value1",
        "wdExampleBitflags::Value2",
        "wdExampleBitflags::Value1|wdExampleBitflags::Value2",
        "wdExampleBitflags::Value3",
        "wdExampleBitflags::Value1|wdExampleBitflags::Value3",
        "wdExampleBitflags::Value2|wdExampleBitflags::Value3",
        "wdExampleBitflags::Value1|wdExampleBitflags::Value2|wdExampleBitflags::Value3"};

      const char* stringValuesShort[] = {"",
        "Value1",
        "Value2",
        "Value1|Value2",
        "Value3",
        "Value1|Value3",
        "Value2|Value3",
        "Value1|Value2|Value3"};
      for (wdInt32 i = 0; i < 8; ++i)
      {
        wdUInt64 uiBitflagValue = 0;
        uiBitflagValue |= (i & WD_BIT(0)) != 0 ? constants[0] : 0;
        uiBitflagValue |= (i & WD_BIT(1)) != 0 ? constants[1] : 0;
        uiBitflagValue |= (i & WD_BIT(2)) != 0 ? constants[2] : 0;

        pBitflagsProp->SetValue(&data, uiBitflagValue);
        WD_TEST_INT(pBitflagsProp->GetValue(&data), uiBitflagValue);

        // Bitflags <-> string
        wdStringBuilder sValue;
        WD_TEST_BOOL(wdReflectionUtils::EnumerationToString(pBitflagsPropertyRTTI, uiBitflagValue, sValue));
        WD_TEST_STRING(sValue, stringValues[i]);

        // Setting the value via a string also works.
        pBitflagsProp->SetValue(&data, 0);
        wdReflectionUtils::SetMemberPropertyValue(pBitflagsProp, &data, sValue.GetData());
        WD_TEST_INT(pBitflagsProp->GetValue(&data), uiBitflagValue);

        wdInt64 iValue = 0;
        WD_TEST_BOOL(wdReflectionUtils::StringToEnumeration(pBitflagsPropertyRTTI, sValue, iValue));
        WD_TEST_INT(iValue, uiBitflagValue);

        // Testing the short enum name version
        WD_TEST_BOOL(wdReflectionUtils::EnumerationToString(
          pBitflagsPropertyRTTI, uiBitflagValue, sValue, wdReflectionUtils::EnumConversionMode::ValueNameOnly));
        WD_TEST_BOOL(sValue.IsEqual(stringValuesShort[i]));

        WD_TEST_BOOL(wdReflectionUtils::StringToEnumeration(pBitflagsPropertyRTTI, sValue, iValue));
        WD_TEST_INT(iValue, uiBitflagValue);

        // Testing the short enum name version
        WD_TEST_BOOL(wdReflectionUtils::EnumerationToString(
          pBitflagsPropertyRTTI, uiBitflagValue, sValue, wdReflectionUtils::EnumConversionMode::ValueNameOnly));
        WD_TEST_BOOL(sValue.IsEqual(stringValuesShort[i]));

        WD_TEST_BOOL(wdReflectionUtils::StringToEnumeration(pBitflagsPropertyRTTI, sValue, iValue));
        WD_TEST_INT(iValue, uiBitflagValue);

        WD_TEST_INT(iValue, wdReflectionUtils::MakeEnumerationValid(pBitflagsPropertyRTTI, iValue));
        WD_TEST_INT(iValue, wdReflectionUtils::MakeEnumerationValid(pBitflagsPropertyRTTI, iValue | WD_BIT(16)));
      }
    }

    WD_TEST_BOOL(data.m_bitflagsClass == (wdExampleBitflags::Value1 | wdExampleBitflags::Value2 | wdExampleBitflags::Value3));
    WD_TEST_BOOL(data.GetBitflagsClass() == (wdExampleBitflags::Value1 | wdExampleBitflags::Value2 | wdExampleBitflags::Value3));
    TestSerialization<wdTestBitflagsStruct>(data);
  }
}


template <typename T>
void TestArrayPropertyVariant(wdAbstractArrayProperty* pArrayProp, void* pObject, const wdRTTI* pRtti, T& value)
{
  T temp = {};

  // Reflection Utils
  wdVariant value0 = wdReflectionUtils::GetArrayPropertyValue(pArrayProp, pObject, 0);
  WD_TEST_BOOL(value0 == wdVariant(value));
  // insert
  wdReflectionUtils::InsertArrayPropertyValue(pArrayProp, pObject, wdVariant(temp), 2);
  WD_TEST_INT(pArrayProp->GetCount(pObject), 3);
  wdVariant value2 = wdReflectionUtils::GetArrayPropertyValue(pArrayProp, pObject, 2);
  WD_TEST_BOOL(value0 != value2);
  wdReflectionUtils::SetArrayPropertyValue(pArrayProp, pObject, 2, value);
  value2 = wdReflectionUtils::GetArrayPropertyValue(pArrayProp, pObject, 2);
  WD_TEST_BOOL(value0 == value2);
  // remove again
  wdReflectionUtils::RemoveArrayPropertyValue(pArrayProp, pObject, 2);
  WD_TEST_INT(pArrayProp->GetCount(pObject), 2);
}

template <>
void TestArrayPropertyVariant<wdTestArrays>(wdAbstractArrayProperty* pArrayProp, void* pObject, const wdRTTI* pRtti, wdTestArrays& value)
{
}

template <>
void TestArrayPropertyVariant<wdTestStruct3>(wdAbstractArrayProperty* pArrayProp, void* pObject, const wdRTTI* pRtti, wdTestStruct3& value)
{
}

template <typename T>
void TestArrayProperty(const char* szPropName, void* pObject, const wdRTTI* pRtti, T& value)
{
  wdAbstractProperty* pProp = pRtti->FindPropertyByName(szPropName);
  WD_TEST_BOOL(pProp != nullptr);
  WD_TEST_BOOL(pProp->GetCategory() == wdPropertyCategory::Array);
  wdAbstractArrayProperty* pArrayProp = static_cast<wdAbstractArrayProperty*>(pProp);
  const wdRTTI* pElemRtti = pProp->GetSpecificType();
  WD_TEST_BOOL(pElemRtti == wdGetStaticRTTI<T>());
  if (!pArrayProp->GetFlags().IsSet(wdPropertyFlags::ReadOnly))
  {
    // If we don't know the element type T but we can allocate it, we can handle it anyway.
    if (pElemRtti->GetAllocator()->CanAllocate())
    {
      void* pData = pElemRtti->GetAllocator()->Allocate<void>();

      pArrayProp->SetCount(pObject, 2);
      WD_TEST_INT(pArrayProp->GetCount(pObject), 2);
      // Push default constructed object in both slots.
      pArrayProp->SetValue(pObject, 0, pData);
      pArrayProp->SetValue(pObject, 1, pData);

      // Retrieve it again and compare to function parameter, they should be different.
      pArrayProp->GetValue(pObject, 0, pData);
      WD_TEST_BOOL(*static_cast<T*>(pData) != value);
      pArrayProp->GetValue(pObject, 1, pData);
      WD_TEST_BOOL(*static_cast<T*>(pData) != value);

      pElemRtti->GetAllocator()->Deallocate(pData);
    }

    pArrayProp->Clear(pObject);
    WD_TEST_INT(pArrayProp->GetCount(pObject), 0);
    pArrayProp->SetCount(pObject, 2);
    pArrayProp->SetValue(pObject, 0, &value);
    pArrayProp->SetValue(pObject, 1, &value);

    // Insert default init values
    T temp = {};
    pArrayProp->Insert(pObject, 2, &temp);
    WD_TEST_INT(pArrayProp->GetCount(pObject), 3);
    pArrayProp->Insert(pObject, 0, &temp);
    WD_TEST_INT(pArrayProp->GetCount(pObject), 4);

    // Remove them again
    pArrayProp->Remove(pObject, 3);
    WD_TEST_INT(pArrayProp->GetCount(pObject), 3);
    pArrayProp->Remove(pObject, 0);
    WD_TEST_INT(pArrayProp->GetCount(pObject), 2);

    TestArrayPropertyVariant<T>(pArrayProp, pObject, pRtti, value);
  }

  // Assumes this function gets called first by a writeable property, and then immediately by the same data as a read-only property.
  // So the checks are valid for the read-only version, too.
  WD_TEST_INT(pArrayProp->GetCount(pObject), 2);

  T v1 = {};
  pArrayProp->GetValue(pObject, 0, &v1);
  if constexpr (std::is_same<const char*, T>::value)
  {
    WD_TEST_BOOL(wdStringUtils::IsEqual(v1, value));
  }
  else
  {
    WD_TEST_BOOL(v1 == value);
  }

  T v2 = {};
  pArrayProp->GetValue(pObject, 1, &v2);
  if constexpr (std::is_same<const char*, T>::value)
  {
    WD_TEST_BOOL(wdStringUtils::IsEqual(v2, value));
  }
  else
  {
    WD_TEST_BOOL(v2 == value);
  }

  if (pElemRtti->GetAllocator()->CanAllocate())
  {
    // Current values should be different from default constructed version.
    void* pData = pElemRtti->GetAllocator()->Allocate<void>();

    WD_TEST_BOOL(*static_cast<T*>(pData) != v1);
    WD_TEST_BOOL(*static_cast<T*>(pData) != v2);

    pElemRtti->GetAllocator()->Deallocate(pData);
  }
}

WD_CREATE_SIMPLE_TEST(Reflection, Arrays)
{
  wdTestArrays containers;
  const wdRTTI* pRtti = wdGetStaticRTTI<wdTestArrays>();
  WD_TEST_BOOL(pRtti != nullptr);

  WD_TEST_BLOCK(wdTestBlock::Enabled, "POD Array")
  {
    double fValue = 5;
    TestArrayProperty<double>("Hybrid", &containers, pRtti, fValue);
    TestArrayProperty<double>("HybridRO", &containers, pRtti, fValue);

    TestArrayProperty<double>("AcHybrid", &containers, pRtti, fValue);
    TestArrayProperty<double>("AcHybridRO", &containers, pRtti, fValue);

    const char* szValue = "Bla";
    const char* szValue2 = "LongString------------------------------------------------------------------------------------";
    wdString sValue = szValue;
    wdString sValue2 = szValue2;

    TestArrayProperty<wdString>("HybridChar", &containers, pRtti, sValue);
    TestArrayProperty<wdString>("HybridCharRO", &containers, pRtti, sValue);

    TestArrayProperty<const char*>("AcHybridChar", &containers, pRtti, szValue);
    TestArrayProperty<const char*>("AcHybridCharRO", &containers, pRtti, szValue);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Struct Array")
  {
    wdTestStruct3 data;
    data.m_fFloat1 = 99.0f;
    data.m_UInt8 = 127;

    TestArrayProperty<wdTestStruct3>("Dynamic", &containers, pRtti, data);
    TestArrayProperty<wdTestStruct3>("DynamicRO", &containers, pRtti, data);

    TestArrayProperty<wdTestStruct3>("AcDynamic", &containers, pRtti, data);
    TestArrayProperty<wdTestStruct3>("AcDynamicRO", &containers, pRtti, data);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdReflectedClass Array")
  {
    wdTestArrays data;
    data.m_Hybrid.PushBack(42.0);

    TestArrayProperty<wdTestArrays>("Deque", &containers, pRtti, data);
    TestArrayProperty<wdTestArrays>("DequeRO", &containers, pRtti, data);

    TestArrayProperty<wdTestArrays>("AcDeque", &containers, pRtti, data);
    TestArrayProperty<wdTestArrays>("AcDequeRO", &containers, pRtti, data);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Custom Variant Array")
  {
    wdVarianceTypeAngle data{0.1f, wdAngle::Degree(45.0f)};

    TestArrayProperty<wdVarianceTypeAngle>("Custom", &containers, pRtti, data);
    TestArrayProperty<wdVarianceTypeAngle>("CustomRO", &containers, pRtti, data);

    TestArrayProperty<wdVarianceTypeAngle>("AcCustom", &containers, pRtti, data);
    TestArrayProperty<wdVarianceTypeAngle>("AcCustomRO", &containers, pRtti, data);
  }

  TestSerialization<wdTestArrays>(containers);
}



template <typename T>
void TestSetProperty(const char* szPropName, void* pObject, const wdRTTI* pRtti, T& ref_value1, T& ref_value2)
{
  wdAbstractProperty* pProp = pRtti->FindPropertyByName(szPropName);
  if (!WD_TEST_BOOL(pProp != nullptr))
    return;

  WD_TEST_BOOL(pProp->GetCategory() == wdPropertyCategory::Set);
  wdAbstractSetProperty* pSetProp = static_cast<wdAbstractSetProperty*>(pProp);
  const wdRTTI* pElemRtti = pProp->GetSpecificType();
  WD_TEST_BOOL(pElemRtti == wdGetStaticRTTI<T>());

  if (!pSetProp->GetFlags().IsSet(wdPropertyFlags::ReadOnly))
  {
    pSetProp->Clear(pObject);
    WD_TEST_BOOL(pSetProp->IsEmpty(pObject));
    pSetProp->Insert(pObject, &ref_value1);
    WD_TEST_BOOL(!pSetProp->IsEmpty(pObject));
    WD_TEST_BOOL(pSetProp->Contains(pObject, &ref_value1));
    WD_TEST_BOOL(!pSetProp->Contains(pObject, &ref_value2));
    pSetProp->Insert(pObject, &ref_value2);
    WD_TEST_BOOL(!pSetProp->IsEmpty(pObject));
    WD_TEST_BOOL(pSetProp->Contains(pObject, &ref_value1));
    WD_TEST_BOOL(pSetProp->Contains(pObject, &ref_value2));

    // Insert default init value
    if (!wdIsPointer<T>::value)
    {
      T temp = T{};
      pSetProp->Insert(pObject, &temp);
      WD_TEST_BOOL(!pSetProp->IsEmpty(pObject));
      WD_TEST_BOOL(pSetProp->Contains(pObject, &ref_value1));
      WD_TEST_BOOL(pSetProp->Contains(pObject, &ref_value2));
      WD_TEST_BOOL(pSetProp->Contains(pObject, &temp));

      // Remove it again
      pSetProp->Remove(pObject, &temp);
      WD_TEST_BOOL(!pSetProp->IsEmpty(pObject));
      WD_TEST_BOOL(!pSetProp->Contains(pObject, &temp));
    }
  }

  // Assumes this function gets called first by a writeable property, and then immediately by the same data as a read-only property.
  // So the checks are valid for the read-only version, too.
  WD_TEST_BOOL(!pSetProp->IsEmpty(pObject));
  WD_TEST_BOOL(pSetProp->Contains(pObject, &ref_value1));
  WD_TEST_BOOL(pSetProp->Contains(pObject, &ref_value2));


  wdHybridArray<wdVariant, 16> keys;
  pSetProp->GetValues(pObject, keys);
  WD_TEST_INT(keys.GetCount(), 2);
}

WD_CREATE_SIMPLE_TEST(Reflection, Sets)
{
  wdTestSets containers;
  const wdRTTI* pRtti = wdGetStaticRTTI<wdTestSets>();
  WD_TEST_BOOL(pRtti != nullptr);

  // Disabled because MSVC 2017 has code generation issues in Release builds
  WD_TEST_BLOCK(wdTestBlock::Disabled, "wdSet")
  {
    wdInt8 iValue1 = -5;
    wdInt8 iValue2 = 127;
    TestSetProperty<wdInt8>("Set", &containers, pRtti, iValue1, iValue2);
    TestSetProperty<wdInt8>("SetRO", &containers, pRtti, iValue1, iValue2);

    double fValue1 = 5;
    double fValue2 = -3;
    TestSetProperty<double>("AcSet", &containers, pRtti, fValue1, fValue2);
    TestSetProperty<double>("AcSetRO", &containers, pRtti, fValue1, fValue2);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdHashSet")
  {
    wdInt32 iValue1 = -5;
    wdInt32 iValue2 = 127;
    TestSetProperty<wdInt32>("HashSet", &containers, pRtti, iValue1, iValue2);
    TestSetProperty<wdInt32>("HashSetRO", &containers, pRtti, iValue1, iValue2);

    wdInt64 fValue1 = 5;
    wdInt64 fValue2 = -3;
    TestSetProperty<wdInt64>("HashAcSet", &containers, pRtti, fValue1, fValue2);
    TestSetProperty<wdInt64>("HashAcSetRO", &containers, pRtti, fValue1, fValue2);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdDeque Pseudo Set")
  {
    int iValue1 = -5;
    int iValue2 = 127;

    TestSetProperty<int>("AcPseudoSet", &containers, pRtti, iValue1, iValue2);
    TestSetProperty<int>("AcPseudoSetRO", &containers, pRtti, iValue1, iValue2);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdSetPtr Pseudo Set")
  {
    wdString sValue1 = "TestString1";
    wdString sValue2 = "Test String Deus";

    TestSetProperty<wdString>("AcPseudoSet2", &containers, pRtti, sValue1, sValue2);
    TestSetProperty<wdString>("AcPseudoSet2RO", &containers, pRtti, sValue1, sValue2);

    const char* szValue1 = "TestString1";
    const char* szValue2 = "Test String Deus";
    TestSetProperty<const char*>("AcPseudoSet2b", &containers, pRtti, szValue1, szValue2);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Custom Variant HashSet")
  {
    wdVarianceTypeAngle value1{-0.1f, wdAngle::Degree(-45.0f)};
    wdVarianceTypeAngle value2{0.1f, wdAngle::Degree(45.0f)};

    TestSetProperty<wdVarianceTypeAngle>("CustomHashSet", &containers, pRtti, value1, value2);
    TestSetProperty<wdVarianceTypeAngle>("CustomHashSetRO", &containers, pRtti, value1, value2);

    wdVarianceTypeAngle value3{-0.2f, wdAngle::Degree(-90.0f)};
    wdVarianceTypeAngle value4{0.2f, wdAngle::Degree(90.0f)};
    TestSetProperty<wdVarianceTypeAngle>("CustomHashAcSet", &containers, pRtti, value3, value4);
    TestSetProperty<wdVarianceTypeAngle>("CustomHashAcSetRO", &containers, pRtti, value3, value4);
  }
  TestSerialization<wdTestSets>(containers);
}

template <typename T>
void TestMapProperty(const char* szPropName, void* pObject, const wdRTTI* pRtti, T& ref_value1, T& ref_value2)
{
  wdAbstractProperty* pProp = pRtti->FindPropertyByName(szPropName);
  WD_TEST_BOOL(pProp != nullptr);
  WD_TEST_BOOL(pProp->GetCategory() == wdPropertyCategory::Map);
  wdAbstractMapProperty* pMapProp = static_cast<wdAbstractMapProperty*>(pProp);
  const wdRTTI* pElemRtti = pProp->GetSpecificType();
  WD_TEST_BOOL(pElemRtti == wdGetStaticRTTI<T>());
  WD_TEST_BOOL(wdReflectionUtils::IsBasicType(pElemRtti) || pElemRtti == wdGetStaticRTTI<wdVariant>() || pElemRtti == wdGetStaticRTTI<wdVarianceTypeAngle>());

  if (!pMapProp->GetFlags().IsSet(wdPropertyFlags::ReadOnly))
  {
    pMapProp->Clear(pObject);
    WD_TEST_BOOL(pMapProp->IsEmpty(pObject));
    pMapProp->Insert(pObject, "value1", &ref_value1);
    WD_TEST_BOOL(!pMapProp->IsEmpty(pObject));
    WD_TEST_BOOL(pMapProp->Contains(pObject, "value1"));
    WD_TEST_BOOL(!pMapProp->Contains(pObject, "value2"));
    T getValue;
    WD_TEST_BOOL(!pMapProp->GetValue(pObject, "value2", &getValue));
    WD_TEST_BOOL(pMapProp->GetValue(pObject, "value1", &getValue));
    WD_TEST_BOOL(getValue == ref_value1);

    pMapProp->Insert(pObject, "value2", &ref_value2);
    WD_TEST_BOOL(!pMapProp->IsEmpty(pObject));
    WD_TEST_BOOL(pMapProp->Contains(pObject, "value1"));
    WD_TEST_BOOL(pMapProp->Contains(pObject, "value2"));
    WD_TEST_BOOL(pMapProp->GetValue(pObject, "value1", &getValue));
    WD_TEST_BOOL(getValue == ref_value1);
    WD_TEST_BOOL(pMapProp->GetValue(pObject, "value2", &getValue));
    WD_TEST_BOOL(getValue == ref_value2);
  }

  // Assumes this function gets called first by a writeable property, and then immediately by the same data as a read-only property.
  // So the checks are valid for the read-only version, too.
  T getValue2;
  WD_TEST_BOOL(!pMapProp->IsEmpty(pObject));
  WD_TEST_BOOL(pMapProp->Contains(pObject, "value1"));
  WD_TEST_BOOL(pMapProp->Contains(pObject, "value2"));
  WD_TEST_BOOL(pMapProp->GetValue(pObject, "value1", &getValue2));
  WD_TEST_BOOL(getValue2 == ref_value1);
  WD_TEST_BOOL(pMapProp->GetValue(pObject, "value2", &getValue2));
  WD_TEST_BOOL(getValue2 == ref_value2);

  wdHybridArray<wdString, 16> keys;
  pMapProp->GetKeys(pObject, keys);
  WD_TEST_INT(keys.GetCount(), 2);
  keys.Sort();
  WD_TEST_BOOL(keys[0] == "value1");
  WD_TEST_BOOL(keys[1] == "value2");
}

WD_CREATE_SIMPLE_TEST(Reflection, Maps)
{
  wdTestMaps containers;
  const wdRTTI* pRtti = wdGetStaticRTTI<wdTestMaps>();
  WD_TEST_BOOL(pRtti != nullptr);

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdMap")
  {
    int iValue1 = -5;
    int iValue2 = 127;
    TestMapProperty<int>("Map", &containers, pRtti, iValue1, iValue2);
    TestMapProperty<int>("MapRO", &containers, pRtti, iValue1, iValue2);

    wdInt64 iValue1b = 5;
    wdInt64 iValue2b = -3;
    TestMapProperty<wdInt64>("AcMap", &containers, pRtti, iValue1b, iValue2b);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdHashMap")
  {
    double fValue1 = -5;
    double fValue2 = 127;
    TestMapProperty<double>("HashTable", &containers, pRtti, fValue1, fValue2);
    TestMapProperty<double>("HashTableRO", &containers, pRtti, fValue1, fValue2);

    wdString sValue1 = "Bla";
    wdString sValue2 = "Test";
    TestMapProperty<wdString>("AcHashTable", &containers, pRtti, sValue1, sValue2);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Accessor")
  {
    wdVariant sValue1 = "Test";
    wdVariant sValue2 = wdVec4(1, 2, 3, 4);
    TestMapProperty<wdVariant>("Accessor", &containers, pRtti, sValue1, sValue2);
    TestMapProperty<wdVariant>("AccessorRO", &containers, pRtti, sValue1, sValue2);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "CustomVariant")
  {
    wdVarianceTypeAngle value1{-0.1f, wdAngle::Degree(-45.0f)};
    wdVarianceTypeAngle value2{0.1f, wdAngle::Degree(45.0f)};

    TestMapProperty<wdVarianceTypeAngle>("CustomVariant", &containers, pRtti, value1, value2);
    TestMapProperty<wdVarianceTypeAngle>("CustomVariantRO", &containers, pRtti, value1, value2);
  }
  TestSerialization<wdTestMaps>(containers);
}


template <typename T>
void TestPointerMemberProperty(const char* szPropName, void* pObject, const wdRTTI* pRtti, wdBitflags<wdPropertyFlags> expectedFlags, T* pExpectedValue)
{
  wdAbstractProperty* pProp = pRtti->FindPropertyByName(szPropName);
  WD_TEST_BOOL(pProp != nullptr);
  WD_TEST_BOOL(pProp->GetCategory() == wdPropertyCategory::Member);
  wdAbstractMemberProperty* pAbsMember = (wdAbstractMemberProperty*)pProp;
  WD_TEST_INT(pProp->GetFlags().GetValue(), expectedFlags.GetValue());
  WD_TEST_BOOL(pProp->GetSpecificType() == wdGetStaticRTTI<T>());
  void* pData = nullptr;
  pAbsMember->GetValuePtr(pObject, &pData);
  WD_TEST_BOOL(pData == pExpectedValue);

  // Set value to null.
  {
    void* pDataNull = nullptr;
    pAbsMember->SetValuePtr(pObject, &pDataNull);
    void* pDataNull2 = nullptr;
    pAbsMember->GetValuePtr(pObject, &pDataNull2);
    WD_TEST_BOOL(pDataNull == pDataNull2);
  }

  // Set value to new instance.
  {
    void* pNewData = pAbsMember->GetSpecificType()->GetAllocator()->Allocate<void>();
    pAbsMember->SetValuePtr(pObject, &pNewData);
    void* pData2 = nullptr;
    pAbsMember->GetValuePtr(pObject, &pData2);
    WD_TEST_BOOL(pNewData == pData2);
  }

  // Delete old value
  pAbsMember->GetSpecificType()->GetAllocator()->Deallocate(pData);
}

WD_CREATE_SIMPLE_TEST(Reflection, Pointer)
{
  const wdRTTI* pRtti = wdGetStaticRTTI<wdTestPtr>();
  WD_TEST_BOOL(pRtti != nullptr);

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Member Property Ptr")
  {
    wdTestPtr containers;
    {
      wdAbstractProperty* pProp = pRtti->FindPropertyByName("ConstCharPtr");
      WD_TEST_BOOL(pProp != nullptr);
      WD_TEST_BOOL(pProp->GetCategory() == wdPropertyCategory::Member);
      WD_TEST_INT(pProp->GetFlags().GetValue(), (wdPropertyFlags::StandardType | wdPropertyFlags::Const).GetValue());
      WD_TEST_BOOL(pProp->GetSpecificType() == wdGetStaticRTTI<const char*>());
    }

    TestPointerMemberProperty<wdTestArrays>(
      "ArraysPtr", &containers, pRtti, wdPropertyFlags::Class | wdPropertyFlags::Pointer | wdPropertyFlags::PointerOwner, containers.m_pArrays);
    TestPointerMemberProperty<wdTestArrays>("ArraysPtrDirect", &containers, pRtti,
      wdPropertyFlags::Class | wdPropertyFlags::Pointer | wdPropertyFlags::PointerOwner, containers.m_pArraysDirect);
  }

  wdTestPtr containers;
  wdDefaultMemoryStreamStorage StreamStorage;

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Serialize Property Ptr")
  {
    containers.m_sString = "Test";

    containers.m_pArrays = WD_DEFAULT_NEW(wdTestArrays);
    containers.m_pArrays->m_Deque.PushBack(wdTestArrays());

    containers.m_ArrayPtr.PushBack(WD_DEFAULT_NEW(wdTestArrays));
    containers.m_ArrayPtr[0]->m_Hybrid.PushBack(5.0);

    containers.m_SetPtr.Insert(WD_DEFAULT_NEW(wdTestSets));
    containers.m_SetPtr.GetIterator().Key()->m_Array.PushBack("BLA");
  }

  TestSerialization<wdTestPtr>(containers);
}
