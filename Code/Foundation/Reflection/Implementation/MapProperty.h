#pragma once

/// \file

#include <Foundation/Reflection/Implementation/AbstractProperty.h>

class wdRTTI;

template <typename Type>
class wdTypedMapProperty : public wdAbstractMapProperty
{
public:
  wdTypedMapProperty(const char* szPropertyName)
    : wdAbstractMapProperty(szPropertyName)
  {
    m_Flags = wdPropertyFlags::GetParameterFlags<Type>();
    WD_CHECK_AT_COMPILETIME_MSG(
      !std::is_pointer<Type>::value ||
        wdVariant::TypeDeduction<typename wdTypeTraits<Type>::NonConstReferencePointerType>::value == wdVariantType::Invalid,
      "Pointer to standard types are not supported.");
  }

  virtual const wdRTTI* GetSpecificType() const override { return wdGetStaticRTTI<typename wdTypeTraits<Type>::NonConstReferencePointerType>(); }
};


template <typename Class, typename Type, typename Container>
class wdAccessorMapProperty : public wdTypedMapProperty<Type>
{
public:
  using ContainerType = typename wdTypeTraits<Container>::NonConstReferenceType;
  using RealType = typename wdTypeTraits<Type>::NonConstReferenceType;

  using InsertFunc = void (Class::*)(const char* szKey, Type value);
  using RemoveFunc = void (Class::*)(const char* szKey);
  using GetValueFunc = bool (Class::*)(const char* szKey, RealType& value) const;
  using GetKeyRangeFunc = Container (Class::*)() const;

  wdAccessorMapProperty(const char* szPropertyName, GetKeyRangeFunc getKeys, GetValueFunc getValue, InsertFunc insert, RemoveFunc remove)
    : wdTypedMapProperty<Type>(szPropertyName)
  {
    WD_ASSERT_DEBUG(getKeys != nullptr, "The getKeys function of a map property cannot be nullptr.");
    WD_ASSERT_DEBUG(getValue != nullptr, "The GetValueFunc function of a map property cannot be nullptr.");

    m_GetKeyRange = getKeys;
    m_GetValue = getValue;
    m_Insert = insert;
    m_Remove = remove;

    if (m_Insert == nullptr || remove == nullptr)
      wdAbstractMapProperty::m_Flags.Add(wdPropertyFlags::ReadOnly);
  }

  virtual bool IsEmpty(const void* pInstance) const override
  {
    // this should be decltype(auto) c = ...; but MSVC 16 is too dumb for that (MSVC 15 works fine)
    decltype((static_cast<const Class*>(pInstance)->*m_GetKeyRange)()) c = (static_cast<const Class*>(pInstance)->*m_GetKeyRange)();

    return begin(c) == end(c);
  }

  virtual void Clear(void* pInstance) override
  {
    while (true)
    {
      // this should be decltype(auto) c = ...; but MSVC 16 is too dumb for that (MSVC 15 works fine)
      decltype((static_cast<const Class*>(pInstance)->*m_GetKeyRange)()) c = (static_cast<const Class*>(pInstance)->*m_GetKeyRange)();

      auto it = begin(c);
      if (it != end(c))
        Remove(pInstance, *it);
      else
        return;
    }
  }

  virtual void Insert(void* pInstance, const char* szKey, const void* pObject) override
  {
    WD_ASSERT_DEBUG(m_Insert != nullptr, "The property '{0}' has no insert function, thus it is read-only.", wdAbstractProperty::GetPropertyName());
    (static_cast<Class*>(pInstance)->*m_Insert)(szKey, *static_cast<const RealType*>(pObject));
  }

  virtual void Remove(void* pInstance, const char* szKey) override
  {
    WD_ASSERT_DEBUG(m_Remove != nullptr, "The property '{0}' has no remove function, thus it is read-only.", wdAbstractProperty::GetPropertyName());
    (static_cast<Class*>(pInstance)->*m_Remove)(szKey);
  }

  virtual bool Contains(const void* pInstance, const char* szKey) const override
  {
    RealType value;
    return (static_cast<const Class*>(pInstance)->*m_GetValue)(szKey, value);
  }

  virtual bool GetValue(const void* pInstance, const char* szKey, void* pObject) const override
  {
    return (static_cast<const Class*>(pInstance)->*m_GetValue)(szKey, *static_cast<RealType*>(pObject));
  }

  virtual void GetKeys(const void* pInstance, wdHybridArray<wdString, 16>& out_keys) const override
  {
    out_keys.Clear();
    decltype(auto) c = (static_cast<const Class*>(pInstance)->*m_GetKeyRange)();
    for (const auto& key : c)
    {
      out_keys.PushBack(key);
    }
  }

private:
  GetKeyRangeFunc m_GetKeyRange;
  GetValueFunc m_GetValue;
  InsertFunc m_Insert;
  RemoveFunc m_Remove;
};


template <typename Class, typename Type, typename Container>
class wdWriteAccessorMapProperty : public wdTypedMapProperty<Type>
{
public:
  using ContainerType = typename wdTypeTraits<Container>::NonConstReferenceType;
  using ContainerSubType = typename wdContainerSubTypeResolver<ContainerType>::Type;
  using RealType = typename wdTypeTraits<Type>::NonConstReferenceType;

  using InsertFunc = void (Class::*)(const char* szKey, Type value);
  using RemoveFunc = void (Class::*)(const char* szKey);
  using GetContainerFunc = Container (Class::*)() const;

  wdWriteAccessorMapProperty(const char* szPropertyName, GetContainerFunc getContainer, InsertFunc insert, RemoveFunc remove)
    : wdTypedMapProperty<Type>(szPropertyName)
  {
    WD_ASSERT_DEBUG(getContainer != nullptr, "The get count function of a map property cannot be nullptr.");

    m_GetContainer = getContainer;
    m_Insert = insert;
    m_Remove = remove;

    if (m_Insert == nullptr)
      wdAbstractMapProperty::m_Flags.Add(wdPropertyFlags::ReadOnly);
  }

  virtual bool IsEmpty(const void* pInstance) const override { return (static_cast<const Class*>(pInstance)->*m_GetContainer)().IsEmpty(); }

  virtual void Clear(void* pInstance) override
  {
    decltype(auto) c = (static_cast<const Class*>(pInstance)->*m_GetContainer)();
    while (!IsEmpty(pInstance))
    {
      auto it = c.GetIterator();
      Remove(pInstance, it.Key());
    }
  }

  virtual void Insert(void* pInstance, const char* szKey, const void* pObject) override
  {
    WD_ASSERT_DEBUG(m_Insert != nullptr, "The property '{0}' has no insert function, thus it is read-only.", wdAbstractProperty::GetPropertyName());
    (static_cast<Class*>(pInstance)->*m_Insert)(szKey, *static_cast<const RealType*>(pObject));
  }

  virtual void Remove(void* pInstance, const char* szKey) override
  {
    WD_ASSERT_DEBUG(m_Remove != nullptr, "The property '{0}' has no remove function, thus it is read-only.", wdAbstractProperty::GetPropertyName());
    (static_cast<Class*>(pInstance)->*m_Remove)(szKey);
  }

  virtual bool Contains(const void* pInstance, const char* szKey) const override
  {
    return (static_cast<const Class*>(pInstance)->*m_GetContainer)().Contains(szKey);
  }

  virtual bool GetValue(const void* pInstance, const char* szKey, void* pObject) const override
  {
    decltype(auto) c = (static_cast<const Class*>(pInstance)->*m_GetContainer)();
    const RealType* value = c.GetValue(szKey);
    if (value)
    {
      *static_cast<RealType*>(pObject) = *value;
    }
    return value != nullptr;
  }

  virtual void GetKeys(const void* pInstance, wdHybridArray<wdString, 16>& out_keys) const override
  {
    decltype(auto) c = (static_cast<const Class*>(pInstance)->*m_GetContainer)();
    out_keys.Clear();
    for (auto it = c.GetIterator(); it.IsValid(); ++it)
    {
      out_keys.PushBack(it.Key());
    }
  }

private:
  GetContainerFunc m_GetContainer;
  InsertFunc m_Insert;
  RemoveFunc m_Remove;
};



template <typename Class, typename Container, Container Class::*Member>
struct wdMapPropertyAccessor
{
  using ContainerType = typename wdTypeTraits<Container>::NonConstReferenceType;
  using Type = typename wdTypeTraits<typename wdContainerSubTypeResolver<ContainerType>::Type>::NonConstReferenceType;

  static const ContainerType& GetConstContainer(const Class* pInstance) { return (*pInstance).*Member; }

  static ContainerType& GetContainer(Class* pInstance) { return (*pInstance).*Member; }
};


template <typename Class, typename Container, typename Type>
class wdMemberMapProperty : public wdTypedMapProperty<typename wdTypeTraits<Type>::NonConstReferenceType>
{
public:
  using RealType = typename wdTypeTraits<Type>::NonConstReferenceType;
  using GetConstContainerFunc = const Container& (*)(const Class* pInstance);
  using GetContainerFunc = Container& (*)(Class* pInstance);

  wdMemberMapProperty(const char* szPropertyName, GetConstContainerFunc constGetter, GetContainerFunc getter)
    : wdTypedMapProperty<RealType>(szPropertyName)
  {
    WD_ASSERT_DEBUG(constGetter != nullptr, "The const get count function of an array property cannot be nullptr.");

    m_ConstGetter = constGetter;
    m_Getter = getter;

    if (m_Getter == nullptr)
      wdAbstractMapProperty::m_Flags.Add(wdPropertyFlags::ReadOnly);
  }

  virtual bool IsEmpty(const void* pInstance) const override { return m_ConstGetter(static_cast<const Class*>(pInstance)).IsEmpty(); }

  virtual void Clear(void* pInstance) override
  {
    WD_ASSERT_DEBUG(
      m_Getter != nullptr, "The property '{0}' has no non-const set accessor function, thus it is read-only.", wdAbstractProperty::GetPropertyName());
    m_Getter(static_cast<Class*>(pInstance)).Clear();
  }

  virtual void Insert(void* pInstance, const char* szKey, const void* pObject) override
  {
    WD_ASSERT_DEBUG(
      m_Getter != nullptr, "The property '{0}' has no non-const set accessor function, thus it is read-only.", wdAbstractProperty::GetPropertyName());
    m_Getter(static_cast<Class*>(pInstance)).Insert(szKey, *static_cast<const RealType*>(pObject));
  }

  virtual void Remove(void* pInstance, const char* szKey) override
  {
    WD_ASSERT_DEBUG(
      m_Getter != nullptr, "The property '{0}' has no non-const set accessor function, thus it is read-only.", wdAbstractProperty::GetPropertyName());
    m_Getter(static_cast<Class*>(pInstance)).Remove(szKey);
  }

  virtual bool Contains(const void* pInstance, const char* szKey) const override
  {
    return m_ConstGetter(static_cast<const Class*>(pInstance)).Contains(szKey);
  }

  virtual bool GetValue(const void* pInstance, const char* szKey, void* pObject) const override
  {
    const RealType* value = m_ConstGetter(static_cast<const Class*>(pInstance)).GetValue(szKey);
    if (value)
    {
      *static_cast<RealType*>(pObject) = *value;
    }
    return value != nullptr;
  }

  virtual void GetKeys(const void* pInstance, wdHybridArray<wdString, 16>& out_keys) const override
  {
    decltype(auto) c = m_ConstGetter(static_cast<const Class*>(pInstance));
    out_keys.Clear();
    for (auto it = c.GetIterator(); it.IsValid(); ++it)
    {
      out_keys.PushBack(it.Key());
    }
  }

private:
  GetConstContainerFunc m_ConstGetter;
  GetContainerFunc m_Getter;
};
