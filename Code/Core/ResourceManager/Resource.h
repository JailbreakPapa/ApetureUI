#pragma once

#include <Core/ResourceManager/Implementation/Declarations.h>
#include <Core/ResourceManager/ResourceHandle.h>
#include <Foundation/Reflection/Reflection.h>
#include <Foundation/Time/Timestamp.h>

/// \brief The base class for all resources.
class WD_CORE_DLL wdResource : public wdReflectedClass
{
  WD_ADD_DYNAMIC_REFLECTION(wdResource, wdReflectedClass);

protected:
  enum class DoUpdate
  {
    OnMainThread,
    OnAnyThread
  };

  enum class Unload
  {
    AllQualityLevels,
    OneQualityLevel
  };

  /// \brief Default constructor.
  wdResource(DoUpdate ResourceUpdateThread, wdUInt8 uiQualityLevelsLoadable);

  /// \brief virtual destructor.
  virtual ~wdResource();

public:
  struct MemoryUsage
  {
    MemoryUsage()
    {
      m_uiMemoryCPU = 0;
      m_uiMemoryGPU = 0;
    }

    wdUInt64 m_uiMemoryCPU;
    wdUInt64 m_uiMemoryGPU;
  };

  /// \brief Returns the unique ID that identifies this resource. On a file resource this might be a path. Can also be a GUID or any other
  /// scheme that uniquely identifies the resource.
  WD_ALWAYS_INLINE const wdString& GetResourceID() const { return m_sUniqueID; }

  /// \brief Returns the hash of the unique ID.
  WD_ALWAYS_INLINE wdUInt64 GetResourceIDHash() const { return m_uiUniqueIDHash; }

  /// \brief The resource description allows to store an additional string that might be more descriptive during debugging, than the unique
  /// ID.
  void SetResourceDescription(wdStringView sDescription);

  /// \brief The resource description allows to store an additional string that might be more descriptive during debugging, than the unique
  /// ID.
  const wdString& GetResourceDescription() const { return m_sResourceDescription; }

  /// \brief Returns the current state in which this resource is in.
  WD_ALWAYS_INLINE wdResourceState GetLoadingState() const { return m_LoadingState; }

  /// \brief Returns the current maximum quality level that the resource could have.
  ///
  /// This is used to scale the amount data used. Once a resource is in the 'Loaded' state, it can still have different
  /// quality levels. E.g. a texture can be fully used with n mipmap levels, but there might be more that could be loaded.
  /// On the other hand a resource could have a higher 'loaded quality level' then the 'max quality level', if the user
  /// just changed settings and reduced the maximum quality level that should be used. In this case the resource manager
  /// will instruct the resource to unload some of its data soon.
  ///
  /// The quality level is a purely logical concept that can be handled very different by different resource types.
  /// E.g. a texture resource could theoretically use one quality level per available mipmap level. However, since
  /// the resource should generally be able to load and unload each quality level separately, it might make more sense
  /// for a texture resource, to use one quality level for everything up to 64*64, and then one quality level for each
  /// mipmap above that, which would result in 5 quality levels for a 1024*1024 texture.
  ///
  /// Most resource will have zero or one quality levels (which is the same) as they are either loaded or not.
  WD_ALWAYS_INLINE wdUInt8 GetNumQualityLevelsDiscardable() const { return m_uiQualityLevelsDiscardable; }

  /// \brief Returns how many quality levels the resource may additionally load.
  WD_ALWAYS_INLINE wdUInt8 GetNumQualityLevelsLoadable() const { return m_uiQualityLevelsLoadable; }

  /// \brief Returns the priority that is used by the resource manager to determine which resource to load next.
  float GetLoadingPriority(wdTime now) const;

  /// \brief Returns the current resource priority.
  wdResourcePriority GetPriority() const { return m_Priority; }

  /// \brief Changes the current resource priority.
  void SetPriority(wdResourcePriority priority);

  /// \brief Returns the basic flags for the resource type. Mostly used the resource manager.
  WD_ALWAYS_INLINE const wdBitflags<wdResourceFlags>& GetBaseResourceFlags() const { return m_Flags; }

  /// \brief Returns the information about the current memory usage of the resource.
  WD_ALWAYS_INLINE const MemoryUsage& GetMemoryUsage() const { return m_MemoryUsage; }

  /// \brief Returns the time at which the resource was (tried to be) acquired last.
  /// If a resource is acquired using wdResourceAcquireMode::PointerOnly, this does not update the last acquired time, since the resource is
  /// not acquired for full use.
  WD_ALWAYS_INLINE wdTime GetLastAcquireTime() const { return m_LastAcquire; }

  /// \brief Returns the reference count of this resource.
  WD_ALWAYS_INLINE wdInt32 GetReferenceCount() const { return m_iReferenceCount; }

  /// \brief Returns the modification date of the file from which this resource was loaded.
  ///
  /// The date may be invalid, if it cannot be retrieved or the resource was created and not loaded.
  WD_ALWAYS_INLINE const wdTimestamp& GetLoadedFileModificationTime() const { return m_LoadedFileModificationTime; }

  /// \brief Returns the current value of the resource change counter.
  /// Can be used to detect whether the resource has changed since using it last time.
  ///
  /// The resource change counter is increased by calling IncResourceChangeCounter() or
  /// whenever the resource content is updated.
  WD_ALWAYS_INLINE wdUInt32 GetCurrentResourceChangeCounter() const { return m_uiResourceChangeCounter; }

  /// \brief Allows to manually increase the resource change counter to signal that dependent code might need to update.
  WD_ALWAYS_INLINE void IncResourceChangeCounter() { ++m_uiResourceChangeCounter; }

  /// \brief If the resource has modifications from the original state, it should reset itself to that state now (or force a reload on
  /// itself).
  virtual void ResetResource() {}

  /// \brief Prints the stack-traces for all handles that currently reference this resource.
  ///
  /// Only implemented if WD_RESOURCEHANDLE_STACK_TRACES is WD_ON.
  /// Otherwise the function does nothing.
  void PrintHandleStackTraces();


  mutable wdEvent<const wdResourceEvent&, wdMutex> m_ResourceEvents;

private:
  friend class wdResourceManager;
  friend class wdResourceManagerWorkerDataLoad;
  friend class wdResourceManagerWorkerUpdateContent;

  /// \brief Called by wdResourceManager shortly after resource creation.
  void SetUniqueID(wdStringView sUniqueID, bool bIsReloadable);

  void CallUnloadData(Unload WhatToUnload);

  /// \brief Requests the resource to unload another quality level. If bFullUnload is true, the resource should unload all data, because it
  /// is going to be deleted afterwards.
  virtual wdResourceLoadDesc UnloadData(Unload WhatToUnload) = 0;

  void CallUpdateContent(wdStreamReader* Stream);

  /// \brief Called whenever more data for the resource is available. The resource must read the stream to update it's data.
  ///
  /// pStream may be nullptr in case the resource data could not be found.
  virtual wdResourceLoadDesc UpdateContent(wdStreamReader* pStream) = 0;

  /// \brief Returns the resource type loader that should be used for this type of resource, unless it has been overridden on the
  /// wdResourceManager.
  ///
  /// By default, this redirects to wdResourceManager::GetDefaultResourceLoader. So there is one global default loader, that can be set
  /// on the resource manager. Overriding this function will then allow to use a different resource loader on a specific type.
  /// Additionally, one can override the resource loader from the outside, by setting it via wdResourceManager::SetResourceTypeLoader.
  /// That last method always takes precedence and allows to modify the behavior without modifying the code for the resource.
  /// But in the default case, the resource defines which loader is used.
  virtual wdResourceTypeLoader* GetDefaultResourceTypeLoader() const;

private:
  volatile wdResourceState m_LoadingState = wdResourceState::Unloaded;

  wdUInt8 m_uiQualityLevelsDiscardable = 0;
  wdUInt8 m_uiQualityLevelsLoadable = 0;


protected:
  /// \brief Non-const version for resources that want to write this variable directly.
  MemoryUsage& ModifyMemoryUsage() { return m_MemoryUsage; }

  /// \brief Call this to specify whether a resource is reloadable.
  ///
  /// By default all created resources are flagged as not reloadable.
  /// All resources loaded from file are automatically flagged as reloadable.
  void SetIsReloadable(bool bIsReloadable) { m_Flags.AddOrRemove(wdResourceFlags::IsReloadable, bIsReloadable); }

  /// \brief Used internally by the code injection macros
  void SetHasLoadingFallback(bool bHasLoadingFallback) { m_Flags.AddOrRemove(wdResourceFlags::ResourceHasFallback, bHasLoadingFallback); }

private:
  template <typename ResourceType>
  friend class wdTypedResourceHandle;

  friend WD_CORE_DLL_FRIEND void IncreaseResourceRefCount(wdResource* pResource, const void* pOwner);
  friend WD_CORE_DLL_FRIEND void DecreaseResourceRefCount(wdResource* pResource, const void* pOwner);

#if WD_ENABLED(WD_RESOURCEHANDLE_STACK_TRACES)
  friend WD_CORE_DLL_FRIEND void MigrateResourceRefCount(wdResource* pResource, const void* pOldOwner, const void* pNewOwner);

  struct HandleStackTrace
  {
    wdUInt32 m_uiNumPtrs = 0;
    void* m_Ptrs[64];
  };

  wdMutex m_HandleStackTraceMutex;
  wdHashTable<const void*, HandleStackTrace> m_HandleStackTraces;
#endif


  /// \brief This function must be overridden by all resource types.
  ///
  /// It has to compute the memory used by this resource.
  /// It is called by the resource manager whenever the resource's data has been loaded or unloaded.
  virtual void UpdateMemoryUsage(MemoryUsage& out_NewMemoryUsage) = 0;

  virtual void ReportResourceIsMissing();

  virtual bool HasResourceTypeLoadingFallback() const = 0;

  /// \brief Called by wdResourceMananger::CreateResource
  void VerifyAfterCreateResource(const wdResourceLoadDesc& ld);

  wdUInt64 m_uiUniqueIDHash = 0;
  wdUInt32 m_uiResourceChangeCounter = 0;
  wdAtomicInteger32 m_iReferenceCount = 0;
  //wdAtomicInteger32 m_iLockCount = 0; // currently not used
  wdString m_sUniqueID;
  wdString m_sResourceDescription;
  MemoryUsage m_MemoryUsage;
  wdBitflags<wdResourceFlags> m_Flags;

  wdTime m_LastAcquire;
  wdResourcePriority m_Priority = wdResourcePriority::Medium;
  wdTimestamp m_LoadedFileModificationTime;

private:
#if WD_ENABLED(WD_COMPILE_FOR_DEVELOPMENT)
  static const wdResource* GetCurrentlyUpdatingContent();
#endif
};


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// GLORIOUS MACROS FOR RESOURCE CLASS CODE GENERATION
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include <Core/ResourceManager/ResourceManager.h>

#define WD_RESOURCE_DECLARE_COMMON_CODE(SELF)                                                                                                \
  friend class ::wdResourceManager;                                                                                                          \
                                                                                                                                             \
public:                                                                                                                                      \
  /*                                                                                                                                     \ \ \
  /// \brief Unfortunately this has to be called manually from within dynamic plugins during core engine shutdown.                       \ \ \
  ///                                                                                                                                    \ \ \
  /// Without this, the dynamic plugin might still be referenced by the core engine during later shutdown phases and will crash, because \ \ \
  /// memory and code is still referenced, that is already unloaded.                                                                     \ \ \
  */                                                                                                                                         \
  static void CleanupDynamicPluginReferences();                                                                                              \
                                                                                                                                             \
  /*                                                                                                                                     \ \ \
  /// \brief Returns a typed resource handle to this resource                                                                            \ \ \
  */                                                                                                                                         \
  wdTypedResourceHandle<SELF> GetResourceHandle() const;                                                                                     \
                                                                                                                                             \
  /*                                                                                                                                     \ \ \
  /// \brief Sets the fallback resource that can be used while this resource is not yet loaded.                                          \ \ \
  ///                                                                                                                                    \ \ \
  /// By default there is no fallback resource, so all resource will block the application when requested for the first time.            \ \ \
  */                                                                                                                                         \
  void SetLoadingFallbackResource(const wdTypedResourceHandle<SELF>& hResource);                                                             \
                                                                                                                                             \
private:                                                                                                                                     \
  /* These functions are needed to access the static members, such that they get DLL exported, otherwise you get unresolved symbols */       \
  static void SetResourceTypeLoadingFallback(const wdTypedResourceHandle<SELF>& hResource);                                                  \
  static void SetResourceTypeMissingFallback(const wdTypedResourceHandle<SELF>& hResource);                                                  \
  static const wdTypedResourceHandle<SELF>& GetResourceTypeLoadingFallback() { return s_TypeLoadingFallback; }                               \
  static const wdTypedResourceHandle<SELF>& GetResourceTypeMissingFallback() { return s_TypeMissingFallback; }                               \
  virtual bool HasResourceTypeLoadingFallback() const override { return s_TypeLoadingFallback.IsValid(); }                                   \
                                                                                                                                             \
  static wdTypedResourceHandle<SELF> s_TypeLoadingFallback;                                                                                  \
  static wdTypedResourceHandle<SELF> s_TypeMissingFallback;                                                                                  \
                                                                                                                                             \
  wdTypedResourceHandle<SELF> m_hLoadingFallback;



#define WD_RESOURCE_IMPLEMENT_COMMON_CODE(SELF)                                             \
  wdTypedResourceHandle<SELF> SELF::s_TypeLoadingFallback;                                  \
  wdTypedResourceHandle<SELF> SELF::s_TypeMissingFallback;                                  \
                                                                                            \
  void SELF::CleanupDynamicPluginReferences()                                               \
  {                                                                                         \
    s_TypeLoadingFallback.Invalidate();                                                     \
    s_TypeMissingFallback.Invalidate();                                                     \
    wdResourceManager::ClearResourceCleanupCallback(&SELF::CleanupDynamicPluginReferences); \
  }                                                                                         \
                                                                                            \
  wdTypedResourceHandle<SELF> SELF::GetResourceHandle() const                               \
  {                                                                                         \
    wdTypedResourceHandle<SELF> handle((SELF*)this);                                        \
    return handle;                                                                          \
  }                                                                                         \
                                                                                            \
  void SELF::SetLoadingFallbackResource(const wdTypedResourceHandle<SELF>& hResource)       \
  {                                                                                         \
    m_hLoadingFallback = hResource;                                                         \
    SetHasLoadingFallback(m_hLoadingFallback.IsValid());                                    \
  }                                                                                         \
                                                                                            \
  void SELF::SetResourceTypeLoadingFallback(const wdTypedResourceHandle<SELF>& hResource)   \
  {                                                                                         \
    s_TypeLoadingFallback = hResource;                                                      \
    WD_RESOURCE_VALIDATE_FALLBACK(SELF);                                                    \
    wdResourceManager::AddResourceCleanupCallback(&SELF::CleanupDynamicPluginReferences);   \
  }                                                                                         \
  void SELF::SetResourceTypeMissingFallback(const wdTypedResourceHandle<SELF>& hResource)   \
  {                                                                                         \
    s_TypeMissingFallback = hResource;                                                      \
    WD_RESOURCE_VALIDATE_FALLBACK(SELF);                                                    \
    wdResourceManager::AddResourceCleanupCallback(&SELF::CleanupDynamicPluginReferences);   \
  }


#if WD_ENABLED(WD_COMPILE_FOR_DEVELOPMENT)
#  define WD_RESOURCE_VALIDATE_FALLBACK(SELF)                                       \
    if (hResource.IsValid())                                                        \
    {                                                                               \
      wdResourceLock<SELF> lock(hResource, wdResourceAcquireMode::BlockTillLoaded); \
      /* if this fails, the 'fallback resource' is missing itself*/                 \
    }
#else
#  define WD_RESOURCE_VALIDATE_FALLBACK(SELF)
#endif

#define WD_RESOURCE_DECLARE_CREATEABLE(SELF, SELF_DESCRIPTOR)      \
protected:                                                         \
  wdResourceLoadDesc CreateResource(SELF_DESCRIPTOR&& descriptor); \
                                                                   \
private:

#define WD_RESOURCE_IMPLEMENT_CREATEABLE(SELF, SELF_DESCRIPTOR) wdResourceLoadDesc SELF::CreateResource(SELF_DESCRIPTOR&& descriptor)
