#pragma once

#include <Core/CoreDLL.h>
#include <Core/ResourceManager/Implementation/Declarations.h>
#include <Foundation/Reflection/Reflection.h>
#include <Foundation/Strings/String.h>

/// \brief If this is set to WD_ON, stack traces are recorded for every resource handle.
///
/// This can be used to find the places that create resource handles but do not properly clean them up.
#define WD_RESOURCEHANDLE_STACK_TRACES WD_OFF

class wdResource;

template <typename T>
class wdResourceLock;

// These out-of-line helper functions allow to forward declare resource handles without knowledge about the resource class.
WD_CORE_DLL void IncreaseResourceRefCount(wdResource* pResource, const void* pOwner);
WD_CORE_DLL void DecreaseResourceRefCount(wdResource* pResource, const void* pOwner);

#if WD_ENABLED(WD_RESOURCEHANDLE_STACK_TRACES)
WD_CORE_DLL void MigrateResourceRefCount(wdResource* pResource, const void* pOldOwner, const void* pNewOwner);
#else
WD_ALWAYS_INLINE void MigrateResourceRefCount(wdResource* pResource, const void* pOldOwner, const void* pNewOwner)
{
}
#endif

/// \brief The typeless implementation of resource handles. A typed interface is provided by wdTypedResourceHandle.
class WD_CORE_DLL wdTypelessResourceHandle
{
public:
  WD_ALWAYS_INLINE wdTypelessResourceHandle() = default;

  /// \brief [internal] Increases the refcount of the given resource.
  wdTypelessResourceHandle(wdResource* pResource);

  /// \brief Increases the refcount of the given resource
  WD_ALWAYS_INLINE wdTypelessResourceHandle(const wdTypelessResourceHandle& rhs)
  {
    m_pResource = rhs.m_pResource;

    if (m_pResource)
    {
      IncreaseResourceRefCount(m_pResource, this);
    }
  }

  /// \brief Move constructor, no refcount change is necessary.
  WD_ALWAYS_INLINE wdTypelessResourceHandle(wdTypelessResourceHandle&& rhs)
  {
    m_pResource = rhs.m_pResource;
    rhs.m_pResource = nullptr;

    if (m_pResource)
    {
      MigrateResourceRefCount(m_pResource, &rhs, this);
    }
  }

  /// \brief Releases any referenced resource.
  WD_ALWAYS_INLINE ~wdTypelessResourceHandle() { Invalidate(); }

  /// \brief Returns whether the handle stores a valid pointer to a resource.
  WD_ALWAYS_INLINE bool IsValid() const { return m_pResource != nullptr; }

  /// \brief Clears any reference to a resource and reduces its refcount.
  void Invalidate();

  /// \brief Returns the Resource ID hash of the exact resource that this handle points to, without acquiring the resource.
  /// The handle must be valid.
  wdUInt64 GetResourceIDHash() const;

  /// \brief Returns the Resource ID of the exact resource that this handle points to, without acquiring the resource.
  /// The handle must be valid.
  const wdString& GetResourceID() const;

  /// \brief Releases the current reference and increases the refcount of the given resource.
  void operator=(const wdTypelessResourceHandle& rhs);

  /// \brief Move operator, no refcount change is necessary.
  void operator=(wdTypelessResourceHandle&& rhs);

  /// \brief Checks whether the two handles point to the same resource.
  WD_ALWAYS_INLINE bool operator==(const wdTypelessResourceHandle& rhs) const { return m_pResource == rhs.m_pResource; }

  /// \brief Checks whether the two handles point to the same resource.
  WD_ALWAYS_INLINE bool operator!=(const wdTypelessResourceHandle& rhs) const { return m_pResource != rhs.m_pResource; }

  /// \brief For storing handles as keys in maps
  WD_ALWAYS_INLINE bool operator<(const wdTypelessResourceHandle& rhs) const { return m_pResource < rhs.m_pResource; }

  /// \brief Checks whether the handle points to the given resource.
  WD_ALWAYS_INLINE bool operator==(const wdResource* rhs) const { return m_pResource == rhs; }

  /// \brief Checks whether the handle points to the given resource.
  WD_ALWAYS_INLINE bool operator!=(const wdResource* rhs) const { return m_pResource != rhs; }

protected:
  wdResource* m_pResource = nullptr;

private:
  // you must go through the resource manager to get access to the resource pointer
  friend class wdResourceManager;
  friend class wdResourceHandleWriteContext;
  friend class wdResourceHandleReadContext;
  friend class wdResourceHandleStreamOperations;
};

/// \brief The wdTypedResourceHandle controls access to an wdResource.
///
/// All resources must be referenced using wdTypedResourceHandle instances (instantiated with the proper resource type as the template
/// argument). You must not store a direct pointer to a resource anywhere. Instead always store resource handles. To actually access a
/// resource, use wdResourceManager::BeginAcquireResource and wdResourceManager::EndAcquireResource after you have finished using it.
///
/// wdTypedResourceHandle implements reference counting on resources. It also allows to redirect resources to fallback resources when they
/// are not yet loaded (if possible).
///
/// As long as there is one resource handle that references a resource, it is considered 'in use' and thus might not get unloaded.
/// So be careful where you store resource handles.
/// If necessary you can call Invalidate() to clear a resource handle and thus also remove the reference to the resource.
template <typename RESOURCE_TYPE>
class wdTypedResourceHandle
{
public:
  typedef RESOURCE_TYPE ResourceType;

  /// \brief A default constructed handle is invalid and does not reference any resource.
  wdTypedResourceHandle() {}

  /// \brief Increases the refcount of the given resource.
  explicit wdTypedResourceHandle(ResourceType* pResource)
    : m_hTypeless(pResource)
  {
  }

  /// \brief Increases the refcount of the given resource.
  wdTypedResourceHandle(const wdTypedResourceHandle<ResourceType>& rhs)
    : m_hTypeless(rhs.m_hTypeless)
  {
  }

  /// \brief Move constructor, no refcount change is necessary.
  wdTypedResourceHandle(wdTypedResourceHandle<ResourceType>&& rhs)
    : m_hTypeless(std::move(rhs.m_hTypeless))
  {
  }

  template <typename BaseOrDerivedType>
  wdTypedResourceHandle(const wdTypedResourceHandle<BaseOrDerivedType>& rhs)
    : m_hTypeless(rhs.m_hTypeless)
  {
    static_assert(std::is_base_of<ResourceType, BaseOrDerivedType>::value || std::is_base_of<BaseOrDerivedType, ResourceType>::value, "Only related types can be assigned to handles of this type");

#if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
    if (std::is_base_of<BaseOrDerivedType, ResourceType>::value)
    {
      WD_ASSERT_DEBUG(rhs.IsValid(), "Cannot cast invalid base handle to derived type!");
      wdResourceLock<BaseOrDerivedType> lock(rhs, wdResourceAcquireMode::PointerOnly);
      WD_ASSERT_DEBUG(wdDynamicCast<const ResourceType*>(lock.GetPointer()) != nullptr, "Types are not related!");
    }
#endif
  }

  /// \brief Releases the current reference and increases the refcount of the given resource.
  void operator=(const wdTypedResourceHandle<ResourceType>& rhs) { m_hTypeless = rhs.m_hTypeless; }

  /// \brief Move operator, no refcount change is necessary.
  void operator=(wdTypedResourceHandle<ResourceType>&& rhs) { m_hTypeless = std::move(rhs.m_hTypeless); }

  /// \brief Checks whether the two handles point to the same resource.
  WD_ALWAYS_INLINE bool operator==(const wdTypedResourceHandle<ResourceType>& rhs) const { return m_hTypeless == rhs.m_hTypeless; }

  /// \brief Checks whether the two handles point to the same resource.
  WD_ALWAYS_INLINE bool operator!=(const wdTypedResourceHandle<ResourceType>& rhs) const { return m_hTypeless != rhs.m_hTypeless; }

  /// \brief For storing handles as keys in maps
  WD_ALWAYS_INLINE bool operator<(const wdTypedResourceHandle<ResourceType>& rhs) const { return m_hTypeless < rhs.m_hTypeless; }

  /// \brief Checks whether the handle points to the given resource.
  WD_ALWAYS_INLINE bool operator==(const wdResource* rhs) const { return m_hTypeless == rhs; }

  /// \brief Checks whether the handle points to the given resource.
  WD_ALWAYS_INLINE bool operator!=(const wdResource* rhs) const { return m_hTypeless != rhs; }


  /// \brief Returns the corresponding typeless resource handle.
  WD_ALWAYS_INLINE operator const wdTypelessResourceHandle() const { return m_hTypeless; }

  /// \brief Returns the corresponding typeless resource handle.
  WD_ALWAYS_INLINE operator wdTypelessResourceHandle() { return m_hTypeless; }

  /// \brief Returns whether the handle stores a valid pointer to a resource.
  WD_ALWAYS_INLINE bool IsValid() const { return m_hTypeless.IsValid(); }

  /// \brief Returns whether the handle stores a valid pointer to a resource.
  WD_ALWAYS_INLINE explicit operator bool() const { return m_hTypeless.IsValid(); }

  /// \brief Clears any reference to a resource and reduces its refcount.
  WD_ALWAYS_INLINE void Invalidate() { m_hTypeless.Invalidate(); }

  /// \brief Returns the Resource ID hash of the exact resource that this handle points to, without acquiring the resource.
  /// The handle must be valid.
  WD_ALWAYS_INLINE wdUInt64 GetResourceIDHash() const { return m_hTypeless.GetResourceIDHash(); }

  /// \brief Returns the Resource ID of the exact resource that this handle points to, without acquiring the resource.
  /// The handle must be valid.
  WD_ALWAYS_INLINE const wdString& GetResourceID() const { return m_hTypeless.GetResourceID(); }

private:
  template <typename T>
  friend class wdTypedResourceHandle;

  // you must go through the resource manager to get access to the resource pointer
  friend class wdResourceManager;
  friend class wdResourceHandleWriteContext;
  friend class wdResourceHandleReadContext;
  friend class wdResourceHandleStreamOperations;

  wdTypelessResourceHandle m_hTypeless;
};

template <typename T>
struct wdHashHelper<wdTypedResourceHandle<T>>
{
  WD_ALWAYS_INLINE static wdUInt32 Hash(const wdTypedResourceHandle<T>& value) { return value.GetResourceIDHash(); }

  WD_ALWAYS_INLINE static bool Equal(const wdTypedResourceHandle<T>& a, const wdTypedResourceHandle<T>& b) { return a == b; }
};

// Stream operations
class wdResource;

class WD_CORE_DLL wdResourceHandleStreamOperations
{
public:
  template <typename ResourceType>
  static void WriteHandle(wdStreamWriter& inout_stream, const wdTypedResourceHandle<ResourceType>& hResource)
  {
    WriteHandle(inout_stream, hResource.m_hTypeless.m_pResource);
  }

  template <typename ResourceType>
  static void ReadHandle(wdStreamReader& inout_stream, wdTypedResourceHandle<ResourceType>& ref_hResourceHandle)
  {
    ReadHandle(inout_stream, ref_hResourceHandle.m_hTypeless);
  }

private:
  static void WriteHandle(wdStreamWriter& Stream, const wdResource* pResource);
  static void ReadHandle(wdStreamReader& Stream, wdTypelessResourceHandle& ResourceHandle);
};

/// \brief Operator to serialize resource handles
template <typename ResourceType>
void operator<<(wdStreamWriter& inout_stream, const wdTypedResourceHandle<ResourceType>& hValue)
{
  wdResourceHandleStreamOperations::WriteHandle(inout_stream, hValue);
}

/// \brief Operator to deserialize resource handles
template <typename ResourceType>
void operator>>(wdStreamReader& inout_stream, wdTypedResourceHandle<ResourceType>& ref_hValue)
{
  wdResourceHandleStreamOperations::ReadHandle(inout_stream, ref_hValue);
}
