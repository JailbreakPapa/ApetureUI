#include <Core/CorePCH.h>

#include <Core/ResourceManager/Resource.h>

wdTypelessResourceHandle::wdTypelessResourceHandle(wdResource* pResource)
{
  m_pResource = pResource;

  if (m_pResource)
  {
    IncreaseResourceRefCount(m_pResource, this);
  }
}

void wdTypelessResourceHandle::Invalidate()
{
  if (m_pResource)
  {
    DecreaseResourceRefCount(m_pResource, this);
  }

  m_pResource = nullptr;
}

wdUInt64 wdTypelessResourceHandle::GetResourceIDHash() const
{
  return IsValid() ? m_pResource->GetResourceIDHash() : 0;
}

const wdString& wdTypelessResourceHandle::GetResourceID() const
{
  return m_pResource->GetResourceID();
}

void wdTypelessResourceHandle::operator=(const wdTypelessResourceHandle& rhs)
{
  WD_ASSERT_DEBUG(this != &rhs, "Cannot assign a resource handle to itself! This would invalidate the handle.");

  Invalidate();

  m_pResource = rhs.m_pResource;

  if (m_pResource)
  {
    IncreaseResourceRefCount(reinterpret_cast<wdResource*>(m_pResource), this);
  }
}

void wdTypelessResourceHandle::operator=(wdTypelessResourceHandle&& rhs)
{
  Invalidate();

  m_pResource = rhs.m_pResource;
  rhs.m_pResource = nullptr;

  if (m_pResource)
  {
    MigrateResourceRefCount(m_pResource, &rhs, this);
  }
}

// static
void wdResourceHandleStreamOperations::WriteHandle(wdStreamWriter& Stream, const wdResource* pResource)
{
  if (pResource != nullptr)
  {
    Stream << pResource->GetDynamicRTTI()->GetTypeName();
    Stream << pResource->GetResourceID();
  }
  else
  {
    const char* szEmpty = "";
    Stream << szEmpty;
  }
}

// static
void wdResourceHandleStreamOperations::ReadHandle(wdStreamReader& Stream, wdTypelessResourceHandle& ResourceHandle)
{
  wdStringBuilder sTemp;

  Stream >> sTemp;
  if (sTemp.IsEmpty())
  {
    ResourceHandle.Invalidate();
    return;
  }

  const wdRTTI* pRtti = wdRTTI::FindTypeByName(sTemp);
  if (pRtti == nullptr)
  {
    wdLog::Error("Unknown resource type '{0}'", sTemp);
    ResourceHandle.Invalidate();
  }

  // read unique ID for restoring the resource (from file)
  Stream >> sTemp;

  if (pRtti != nullptr)
  {
    ResourceHandle = wdResourceManager::LoadResourceByType(pRtti, sTemp);
  }
}



WD_STATICLINK_FILE(Core, Core_ResourceManager_Implementation_ResourceHandle);
