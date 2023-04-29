
WD_ALWAYS_INLINE wdWorld* wdWorldModule::GetWorld()
{
  return m_pWorld;
}

WD_ALWAYS_INLINE const wdWorld* wdWorldModule::GetWorld() const
{
  return m_pWorld;
}

//////////////////////////////////////////////////////////////////////////

template <typename ModuleType, typename RTTIType>
wdWorldModuleTypeId wdWorldModuleFactory::RegisterWorldModule()
{
  struct Helper
  {
    static wdWorldModule* Create(wdAllocatorBase* pAllocator, wdWorld* pWorld) { return WD_NEW(pAllocator, ModuleType, pWorld); }
  };

  const wdRTTI* pRtti = wdGetStaticRTTI<RTTIType>();
  return RegisterWorldModule(pRtti, &Helper::Create);
}
