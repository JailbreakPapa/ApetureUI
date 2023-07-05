#include <ToolsFoundationTest/ToolsFoundationTestPCH.h>

#include <Foundation/Reflection/Reflection.h>
#include <Foundation/Types/VariantTypeRegistry.h>
#include <ToolsFoundation/Reflection/PhantomRttiManager.h>
#include <ToolsFoundation/Reflection/ReflectedTypeStorageAccessor.h>
#include <ToolsFoundation/Reflection/ToolsReflectionUtils.h>
#include <ToolsFoundationTest/Object/TestObjectManager.h>
#include <ToolsFoundationTest/Reflection/ReflectionTestClasses.h>

WD_CREATE_SIMPLE_TEST_GROUP(Reflection);


void VariantToPropertyTest(void* pIntStruct, const wdRTTI* pRttiInt, const char* szPropName, wdVariant::Type::Enum type)
{
  wdAbstractMemberProperty* pProp = wdReflectionUtils::GetMemberProperty(pRttiInt, szPropName);
  WD_TEST_BOOL(pProp != nullptr);
  if (pProp)
  {
    wdVariant oldValue = wdReflectionUtils::GetMemberPropertyValue(pProp, pIntStruct);
    WD_TEST_BOOL(oldValue.IsValid());
    WD_TEST_BOOL(oldValue.GetType() == type);

    wdVariant defaultValue = wdReflectionUtils::GetDefaultValue(pProp);
    WD_TEST_BOOL(defaultValue.GetType() == type);
    wdReflectionUtils::SetMemberPropertyValue(pProp, pIntStruct, defaultValue);

    wdVariant newValue = wdReflectionUtils::GetMemberPropertyValue(pProp, pIntStruct);
    WD_TEST_BOOL(newValue.IsValid());
    WD_TEST_BOOL(newValue.GetType() == type);
    WD_TEST_BOOL(newValue == defaultValue);
    WD_TEST_BOOL(newValue != oldValue);
  }
}

WD_CREATE_SIMPLE_TEST(Reflection, ReflectionUtils)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Integer Properties")
  {
    wdIntegerStruct intStruct;
    const wdRTTI* pRttiInt = wdRTTI::FindTypeByName("wdIntegerStruct");
    WD_TEST_BOOL(pRttiInt != nullptr);

    VariantToPropertyTest(&intStruct, pRttiInt, "Int8", wdVariant::Type::Int8);
    WD_TEST_INT(0, intStruct.GetInt8());
    VariantToPropertyTest(&intStruct, pRttiInt, "UInt8", wdVariant::Type::UInt8);
    WD_TEST_INT(0, intStruct.GetUInt8());

    VariantToPropertyTest(&intStruct, pRttiInt, "Int16", wdVariant::Type::Int16);
    WD_TEST_INT(0, intStruct.m_iInt16);
    VariantToPropertyTest(&intStruct, pRttiInt, "UInt16", wdVariant::Type::UInt16);
    WD_TEST_INT(0, intStruct.m_iUInt16);

    VariantToPropertyTest(&intStruct, pRttiInt, "Int32", wdVariant::Type::Int32);
    WD_TEST_INT(0, intStruct.GetInt32());
    VariantToPropertyTest(&intStruct, pRttiInt, "UInt32", wdVariant::Type::UInt32);
    WD_TEST_INT(0, intStruct.GetUInt32());

    VariantToPropertyTest(&intStruct, pRttiInt, "Int64", wdVariant::Type::Int64);
    WD_TEST_INT(0, intStruct.m_iInt64);
    VariantToPropertyTest(&intStruct, pRttiInt, "UInt64", wdVariant::Type::UInt64);
    WD_TEST_INT(0, intStruct.m_iUInt64);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Float Properties")
  {
    wdFloatStruct floatStruct;
    wdRTTI* pRttiFloat = wdRTTI::FindTypeByName("wdFloatStruct");
    WD_TEST_BOOL(pRttiFloat != nullptr);

    VariantToPropertyTest(&floatStruct, pRttiFloat, "Float", wdVariant::Type::Float);
    WD_TEST_FLOAT(0, floatStruct.GetFloat(), 0);
    VariantToPropertyTest(&floatStruct, pRttiFloat, "Double", wdVariant::Type::Double);
    WD_TEST_FLOAT(0, floatStruct.GetDouble(), 0);
    VariantToPropertyTest(&floatStruct, pRttiFloat, "Time", wdVariant::Type::Time);
    WD_TEST_FLOAT(0, floatStruct.GetTime().GetSeconds(), 0);
    VariantToPropertyTest(&floatStruct, pRttiFloat, "Angle", wdVariant::Type::Angle);
    WD_TEST_FLOAT(0, floatStruct.GetAngle().GetDegree(), 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Misc Properties")
  {
    wdPODClass podClass;
    wdRTTI* pRttiPOD = wdRTTI::FindTypeByName("wdPODClass");
    WD_TEST_BOOL(pRttiPOD != nullptr);

    VariantToPropertyTest(&podClass, pRttiPOD, "Bool", wdVariant::Type::Bool);
    WD_TEST_BOOL(podClass.GetBool() == false);
    VariantToPropertyTest(&podClass, pRttiPOD, "Color", wdVariant::Type::Color);
    WD_TEST_BOOL(podClass.GetColor() == wdColor(1.0f, 1.0f, 1.0f, 1.0f));
    VariantToPropertyTest(&podClass, pRttiPOD, "String", wdVariant::Type::String);
    WD_TEST_STRING(podClass.GetString(), "");
    VariantToPropertyTest(&podClass, pRttiPOD, "Buffer", wdVariant::Type::DataBuffer);
    WD_TEST_BOOL(podClass.GetBuffer() == wdDataBuffer());
    VariantToPropertyTest(&podClass, pRttiPOD, "VarianceAngle", wdVariant::Type::TypedObject);
    WD_TEST_BOOL(podClass.GetCustom() == wdVarianceTypeAngle{});
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Math Properties")
  {
    wdMathClass mathClass;
    wdRTTI* pRttiMath = wdRTTI::FindTypeByName("wdMathClass");
    WD_TEST_BOOL(pRttiMath != nullptr);

    VariantToPropertyTest(&mathClass, pRttiMath, "Vec2", wdVariant::Type::Vector2);
    WD_TEST_BOOL(mathClass.GetVec2() == wdVec2(0.0f, 0.0f));
    VariantToPropertyTest(&mathClass, pRttiMath, "Vec3", wdVariant::Type::Vector3);
    WD_TEST_BOOL(mathClass.GetVec3() == wdVec3(0.0f, 0.0f, 0.0f));
    VariantToPropertyTest(&mathClass, pRttiMath, "Vec4", wdVariant::Type::Vector4);
    WD_TEST_BOOL(mathClass.GetVec4() == wdVec4(0.0f, 0.0f, 0.0f, 0.0f));
    VariantToPropertyTest(&mathClass, pRttiMath, "Vec2I", wdVariant::Type::Vector2I);
    WD_TEST_BOOL(mathClass.m_Vec2I == wdVec2I32(0, 0));
    VariantToPropertyTest(&mathClass, pRttiMath, "Vec3I", wdVariant::Type::Vector3I);
    WD_TEST_BOOL(mathClass.m_Vec3I == wdVec3I32(0, 0, 0));
    VariantToPropertyTest(&mathClass, pRttiMath, "Vec4I", wdVariant::Type::Vector4I);
    WD_TEST_BOOL(mathClass.m_Vec4I == wdVec4I32(0, 0, 0, 0));
    VariantToPropertyTest(&mathClass, pRttiMath, "Quat", wdVariant::Type::Quaternion);
    WD_TEST_BOOL(mathClass.GetQuat() == wdQuat(0.0f, 0.0f, 0.0f, 1.0f));
    VariantToPropertyTest(&mathClass, pRttiMath, "Mat3", wdVariant::Type::Matrix3);
    WD_TEST_BOOL(mathClass.GetMat3() == wdMat3::IdentityMatrix());
    VariantToPropertyTest(&mathClass, pRttiMath, "Mat4", wdVariant::Type::Matrix4);
    WD_TEST_BOOL(mathClass.GetMat4() == wdMat4::IdentityMatrix());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Enumeration Properties")
  {
    wdEnumerationsClass enumClass;
    wdRTTI* pRttiEnum = wdRTTI::FindTypeByName("wdEnumerationsClass");
    WD_TEST_BOOL(pRttiEnum != nullptr);

    VariantToPropertyTest(&enumClass, pRttiEnum, "Enum", wdVariant::Type::Int64);
    WD_TEST_BOOL(enumClass.GetEnum() == wdExampleEnum::Value1);
    VariantToPropertyTest(&enumClass, pRttiEnum, "Bitflags", wdVariant::Type::Int64);
    WD_TEST_BOOL(enumClass.GetBitflags() == wdExampleBitflags::Value1);
  }
}

void AccessorPropertyTest(wdIReflectedTypeAccessor& ref_accessor, const char* szProperty, wdVariant::Type::Enum type)
{
  wdVariant oldValue = ref_accessor.GetValue(szProperty);
  WD_TEST_BOOL(oldValue.IsValid());
  WD_TEST_BOOL(oldValue.GetType() == type);

  wdAbstractProperty* pProp = ref_accessor.GetType()->FindPropertyByName(szProperty);
  wdVariant defaultValue = wdReflectionUtils::GetDefaultValue(pProp);
  WD_TEST_BOOL(defaultValue.GetType() == type);
  bool bSetSuccess = ref_accessor.SetValue(szProperty, defaultValue);
  WD_TEST_BOOL(bSetSuccess);

  wdVariant newValue = ref_accessor.GetValue(szProperty);
  WD_TEST_BOOL(newValue.IsValid());
  WD_TEST_BOOL(newValue.GetType() == type);
  WD_TEST_BOOL(newValue == defaultValue);
}

wdUInt32 AccessorPropertiesTest(wdIReflectedTypeAccessor& ref_accessor, const wdRTTI* pType)
{
  wdUInt32 uiPropertiesSet = 0;
  WD_TEST_BOOL(pType != nullptr);

  // Call for base class
  if (pType->GetParentType() != nullptr)
  {
    uiPropertiesSet += AccessorPropertiesTest(ref_accessor, pType->GetParentType());
  }

  // Test properties
  wdUInt32 uiPropCount = pType->GetProperties().GetCount();
  for (wdUInt32 i = 0; i < uiPropCount; ++i)
  {
    wdAbstractProperty* pProp = pType->GetProperties()[i];
    const bool bIsValueType = wdReflectionUtils::IsValueType(pProp);

    switch (pProp->GetCategory())
    {
      case wdPropertyCategory::Member:
      {
        wdAbstractMemberProperty* pProp3 = static_cast<wdAbstractMemberProperty*>(pProp);
        if (pProp->GetFlags().IsSet(wdPropertyFlags::IsEnum))
        {
          AccessorPropertyTest(ref_accessor, pProp->GetPropertyName(), wdVariant::Type::Int64);
          uiPropertiesSet++;
        }
        else if (pProp->GetFlags().IsSet(wdPropertyFlags::Bitflags))
        {
          AccessorPropertyTest(ref_accessor, pProp->GetPropertyName(), wdVariant::Type::Int64);
          uiPropertiesSet++;
        }
        else if (bIsValueType)
        {
          AccessorPropertyTest(ref_accessor, pProp->GetPropertyName(), pProp3->GetSpecificType()->GetVariantType());
          uiPropertiesSet++;
        }
        else // wdPropertyFlags::Class
        {
          // Recurs into sub-classes
          const wdUuid& subObjectGuid = ref_accessor.GetValue(pProp->GetPropertyName()).Get<wdUuid>();
          wdDocumentObject* pEmbeddedClassObject = const_cast<wdDocumentObject*>(ref_accessor.GetOwner()->GetChild(subObjectGuid));
          uiPropertiesSet += AccessorPropertiesTest(pEmbeddedClassObject->GetTypeAccessor(), pProp3->GetSpecificType());
        }
      }
      break;
      case wdPropertyCategory::Array:
      {
        // wdAbstractArrayProperty* pProp3 = static_cast<wdAbstractArrayProperty*>(pProp);
        // TODO
      }
      break;

      default:
        WD_ASSERT_NOT_IMPLEMENTED;
        break;
    }
  }
  return uiPropertiesSet;
}

wdUInt32 AccessorPropertiesTest(wdIReflectedTypeAccessor& ref_accessor)
{
  const wdRTTI* handle = ref_accessor.GetType();
  return AccessorPropertiesTest(ref_accessor, handle);
}

static wdUInt32 GetTypeCount()
{
  wdUInt32 uiCount = 0;
  wdRTTI* pType = wdRTTI::GetFirstInstance();
  while (pType != nullptr)
  {
    uiCount++;
    pType = pType->GetNextInstance();
  }
  return uiCount;
}

static const wdRTTI* RegisterType(const char* szTypeName)
{
  const wdRTTI* pRtti = wdRTTI::FindTypeByName(szTypeName);
  WD_TEST_BOOL(pRtti != nullptr);

  wdReflectedTypeDescriptor desc;
  wdToolsReflectionUtils::GetReflectedTypeDescriptorFromRtti(pRtti, desc);
  return wdPhantomRttiManager::RegisterType(desc);
}

WD_CREATE_SIMPLE_TEST(Reflection, ReflectedType)
{
  wdTestDocumentObjectManager manager;

  /*const wdRTTI* pRttiBase =*/RegisterType("wdReflectedClass");
  /*const wdRTTI* pRttiEnumBase =*/RegisterType("wdEnumBase");
  /*const wdRTTI* pRttiBitflagsBase =*/RegisterType("wdBitflagsBase");

  const wdRTTI* pRttiInt = RegisterType("wdIntegerStruct");
  const wdRTTI* pRttiFloat = RegisterType("wdFloatStruct");
  const wdRTTI* pRttiPOD = RegisterType("wdPODClass");
  const wdRTTI* pRttiMath = RegisterType("wdMathClass");
  /*const wdRTTI* pRttiEnum =*/RegisterType("wdExampleEnum");
  /*const wdRTTI* pRttiFlags =*/RegisterType("wdExampleBitflags");
  const wdRTTI* pRttiEnumerations = RegisterType("wdEnumerationsClass");

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdReflectedTypeStorageAccessor")
  {
    {
      wdDocumentObject* pObject = manager.CreateObject(pRttiInt);
      WD_TEST_INT(AccessorPropertiesTest(pObject->GetTypeAccessor()), 8);
      manager.DestroyObject(pObject);
    }
    {
      wdDocumentObject* pObject = manager.CreateObject(pRttiFloat);
      WD_TEST_INT(AccessorPropertiesTest(pObject->GetTypeAccessor()), 4);
      manager.DestroyObject(pObject);
    }
    {
      wdDocumentObject* pObject = manager.CreateObject(pRttiPOD);
      WD_TEST_INT(AccessorPropertiesTest(pObject->GetTypeAccessor()), 18);
      manager.DestroyObject(pObject);
    }
    {
      wdDocumentObject* pObject = manager.CreateObject(pRttiMath);
      WD_TEST_INT(AccessorPropertiesTest(pObject->GetTypeAccessor()), 27);
      manager.DestroyObject(pObject);
    }
    {
      wdDocumentObject* pObject = manager.CreateObject(pRttiEnumerations);
      WD_TEST_INT(AccessorPropertiesTest(pObject->GetTypeAccessor()), 2);
      manager.DestroyObject(pObject);
    }
  }
}


WD_CREATE_SIMPLE_TEST(Reflection, ReflectedTypeReloading)
{
  wdTestDocumentObjectManager manager;

  const wdRTTI* pRttiInner = wdRTTI::FindTypeByName("InnerStruct");
  const wdRTTI* pRttiInnerP = nullptr;
  wdReflectedTypeDescriptor descInner;

  const wdRTTI* pRttiOuter = wdRTTI::FindTypeByName("OuterClass");
  const wdRTTI* pRttiOuterP = nullptr;
  wdReflectedTypeDescriptor descOuter;

  wdUInt32 uiRegisteredBaseTypes = GetTypeCount();
  WD_TEST_BLOCK(wdTestBlock::Enabled, "RegisterType")
  {
    WD_TEST_BOOL(pRttiInner != nullptr);
    wdToolsReflectionUtils::GetReflectedTypeDescriptorFromRtti(pRttiInner, descInner);
    descInner.m_sTypeName = "InnerStructP";
    pRttiInnerP = wdPhantomRttiManager::RegisterType(descInner);
    WD_TEST_BOOL(pRttiInnerP != nullptr);

    WD_TEST_BOOL(pRttiOuter != nullptr);
    wdToolsReflectionUtils::GetReflectedTypeDescriptorFromRtti(pRttiOuter, descOuter);
    descOuter.m_sTypeName = "OuterClassP";
    descOuter.m_Properties[0].m_sType = "InnerStructP";
    pRttiOuterP = wdPhantomRttiManager::RegisterType(descOuter);
    WD_TEST_BOOL(pRttiOuterP != nullptr);
  }

  {
    wdDocumentObject* pInnerObject = manager.CreateObject(pRttiInnerP);
    manager.AddObject(pInnerObject, nullptr, "Children", -1);
    wdIReflectedTypeAccessor& innerAccessor = pInnerObject->GetTypeAccessor();

    wdDocumentObject* pOuterObject = manager.CreateObject(pRttiOuterP);
    manager.AddObject(pOuterObject, nullptr, "Children", -1);
    wdIReflectedTypeAccessor& outerAccessor = pOuterObject->GetTypeAccessor();

    wdUuid innerGuid = outerAccessor.GetValue("Inner").Get<wdUuid>();
    wdDocumentObject* pEmbeddedInnerObject = manager.GetObject(innerGuid);
    wdIReflectedTypeAccessor& embeddedInnerAccessor = pEmbeddedInnerObject->GetTypeAccessor();

    WD_TEST_BLOCK(wdTestBlock::Enabled, "SetValues")
    {
      // Just set a few values to make sure they don't get messed up by the following operations.
      WD_TEST_BOOL(innerAccessor.SetValue("IP1", 1.4f));
      WD_TEST_BOOL(outerAccessor.SetValue("OP1", 0.9f));
      WD_TEST_BOOL(embeddedInnerAccessor.SetValue("IP1", 1.4f));
    }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "AddProperty")
    {
      // Say we reload the engine and the InnerStruct now has a second property: IP2.
      descInner.m_Properties.PushBack(wdReflectedPropertyDescriptor(wdPropertyCategory::Member, "IP2", "wdVec4",
        wdBitflags<wdPropertyFlags>(wdPropertyFlags::StandardType), wdArrayPtr<wdPropertyAttribute* const>()));
      const wdRTTI* NewInnerHandle = wdPhantomRttiManager::RegisterType(descInner);
      WD_TEST_BOOL(NewInnerHandle == pRttiInnerP);

      // Check that the new property is present.
      AccessorPropertyTest(innerAccessor, "IP2", wdVariant::Type::Vector4);

      AccessorPropertyTest(embeddedInnerAccessor, "IP2", wdVariant::Type::Vector4);

      // Test that the old properties are still valid.
      WD_TEST_BOOL(innerAccessor.GetValue("IP1") == 1.4f);
      WD_TEST_BOOL(outerAccessor.GetValue("OP1") == 0.9f);
      WD_TEST_BOOL(embeddedInnerAccessor.GetValue("IP1") == 1.4f);
    }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "ChangeProperty")
    {
      // Out original inner float now is a Int32!
      descInner.m_Properties[0].m_sType = "wdInt32";
      const wdRTTI* NewInnerHandle = wdPhantomRttiManager::RegisterType(descInner);
      WD_TEST_BOOL(NewInnerHandle == pRttiInnerP);

      // Test if the previous value was converted correctly to its new type.
      wdVariant innerValue = innerAccessor.GetValue("IP1");
      WD_TEST_BOOL(innerValue.IsValid());
      WD_TEST_BOOL(innerValue.GetType() == wdVariant::Type::Int32);
      WD_TEST_INT(innerValue.Get<wdInt32>(), 1);

      wdVariant outerValue = embeddedInnerAccessor.GetValue("IP1");
      WD_TEST_BOOL(outerValue.IsValid());
      WD_TEST_BOOL(outerValue.GetType() == wdVariant::Type::Int32);
      WD_TEST_INT(outerValue.Get<wdInt32>(), 1);

      // Test that the old properties are still valid.
      WD_TEST_BOOL(outerAccessor.GetValue("OP1") == 0.9f);

      AccessorPropertyTest(innerAccessor, "IP2", wdVariant::Type::Vector4);
      AccessorPropertyTest(embeddedInnerAccessor, "IP2", wdVariant::Type::Vector4);
    }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "DeleteProperty")
    {
      // Lets now delete the original inner property IP1.
      descInner.m_Properties.RemoveAtAndCopy(0);
      const wdRTTI* NewInnerHandle = wdPhantomRttiManager::RegisterType(descInner);
      WD_TEST_BOOL(NewInnerHandle == pRttiInnerP);

      // Check that IP1 is really gone.
      WD_TEST_BOOL(!innerAccessor.GetValue("IP1").IsValid());
      WD_TEST_BOOL(!embeddedInnerAccessor.GetValue("IP1").IsValid());

      // Test that the old properties are still valid.
      WD_TEST_BOOL(outerAccessor.GetValue("OP1") == 0.9f);

      AccessorPropertyTest(innerAccessor, "IP2", wdVariant::Type::Vector4);
      AccessorPropertyTest(embeddedInnerAccessor, "IP2", wdVariant::Type::Vector4);
    }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "RevertProperties")
    {
      // Reset all classes to their initial state.
      wdToolsReflectionUtils::GetReflectedTypeDescriptorFromRtti(pRttiInner, descInner);
      descInner.m_sTypeName = "InnerStructP";
      wdPhantomRttiManager::RegisterType(descInner);

      wdToolsReflectionUtils::GetReflectedTypeDescriptorFromRtti(pRttiOuter, descOuter);
      descInner.m_sTypeName = "OuterStructP";
      descOuter.m_Properties[0].m_sType = "InnerStructP";
      wdPhantomRttiManager::RegisterType(descOuter);

      // Test that the old properties are back again.
      wdStringBuilder path = "IP1";
      wdVariant innerValue = innerAccessor.GetValue(path);
      WD_TEST_BOOL(innerValue.IsValid());
      WD_TEST_BOOL(innerValue.GetType() == wdVariant::Type::Float);
      WD_TEST_FLOAT(innerValue.Get<float>(), 1.0f, 0.0f);

      wdVariant outerValue = embeddedInnerAccessor.GetValue("IP1");
      WD_TEST_BOOL(outerValue.IsValid());
      WD_TEST_BOOL(outerValue.GetType() == wdVariant::Type::Float);
      WD_TEST_FLOAT(outerValue.Get<float>(), 1.0f, 0.0f);
      WD_TEST_BOOL(outerAccessor.GetValue("OP1") == 0.9f);
    }

    manager.RemoveObject(pInnerObject);
    manager.DestroyObject(pInnerObject);

    manager.RemoveObject(pOuterObject);
    manager.DestroyObject(pOuterObject);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "UnregisterType")
  {
    WD_TEST_INT(GetTypeCount(), uiRegisteredBaseTypes + 2);
    wdPhantomRttiManager::UnregisterType(pRttiOuterP);
    wdPhantomRttiManager::UnregisterType(pRttiInnerP);
    WD_TEST_INT(GetTypeCount(), uiRegisteredBaseTypes);
  }
}
