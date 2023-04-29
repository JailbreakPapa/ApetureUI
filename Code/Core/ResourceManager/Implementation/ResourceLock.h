#pragma once

#include <Core/ResourceManager/Implementation/Declarations.h>

/// \brief Helper class to acquire and release a resource safely.
///
/// The constructor calls wdResourceManager::BeginAcquireResource, the destructor makes sure to call wdResourceManager::EndAcquireResource.
/// The instance of this class can be used like a pointer to the resource.
///
/// Whether the acquisition succeeded or returned a loading fallback, missing fallback or even no result, at all,
/// can be retrieved through GetAcquireResult().
/// \note If a resource is missing, but no missing fallback is specified for the resource type, the code will fail with an assertion,
/// unless you used wdResourceAcquireMode::BlockTillLoaded_NeverFail. Only then will the error be silently ignored and the acquire result
/// will be wdResourceAcquireResult::None.
///
/// \sa wdResourceManager::BeginAcquireResource()
/// \sa wdResourceAcquireMode
/// \sa wdResourceAcquireResult
template <class RESOURCE_TYPE>
class wdResourceLock
{
public:
  WD_ALWAYS_INLINE wdResourceLock(const wdTypedResourceHandle<RESOURCE_TYPE>& hResource, wdResourceAcquireMode mode,
    const wdTypedResourceHandle<RESOURCE_TYPE>& hFallbackResource = wdTypedResourceHandle<RESOURCE_TYPE>())
  {
    m_pResource = wdResourceManager::BeginAcquireResource(hResource, mode, hFallbackResource, &m_AcquireResult);
  }

  wdResourceLock(const wdResourceLock&) = delete;

  wdResourceLock(wdResourceLock&& other)
    : m_AcquireResult(other.m_AcquireResult)
    , m_pResource(other.m_pResource)
  {
    other.m_pResource = nullptr;
    other.m_AcquireResult = wdResourceAcquireResult::None;
  }

  WD_ALWAYS_INLINE ~wdResourceLock()
  {
    if (m_pResource)
    {
      wdResourceManager::EndAcquireResource(m_pResource);
    }
  }

  WD_ALWAYS_INLINE RESOURCE_TYPE* operator->() { return m_pResource; }
  WD_ALWAYS_INLINE const RESOURCE_TYPE* operator->() const { return m_pResource; }

  WD_ALWAYS_INLINE bool IsValid() const { return m_pResource != nullptr; }
  WD_ALWAYS_INLINE explicit operator bool() const { return m_pResource != nullptr; }

  WD_ALWAYS_INLINE wdResourceAcquireResult GetAcquireResult() const { return m_AcquireResult; }

  WD_ALWAYS_INLINE const RESOURCE_TYPE* GetPointer() const { return m_pResource; }
  WD_ALWAYS_INLINE RESOURCE_TYPE* GetPointerNonConst() const { return m_pResource; }

private:
  wdResourceAcquireResult m_AcquireResult;
  RESOURCE_TYPE* m_pResource;
};
