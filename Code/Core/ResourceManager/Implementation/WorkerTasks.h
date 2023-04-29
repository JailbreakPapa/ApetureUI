#pragma once

#include <Core/ResourceManager/Implementation/Declarations.h>
#include <Core/ResourceManager/ResourceTypeLoader.h>
#include <Foundation/Threading/TaskSystem.h>
#include <Foundation/Types/UniquePtr.h>

/// \brief [internal] Worker task for loading resources (typically from disk).
class WD_CORE_DLL wdResourceManagerWorkerDataLoad final : public wdTask
{
public:
  ~wdResourceManagerWorkerDataLoad();

private:
  friend class wdResourceManager;
  friend class wdResourceManagerState;

  wdResourceManagerWorkerDataLoad();

  virtual void Execute() override;
};

/// \brief [internal] Worker task for uploading resource data.
/// Depending on the resource type, this may get scheduled to run on the main thread or on any thread.
class WD_CORE_DLL wdResourceManagerWorkerUpdateContent final : public wdTask
{
public:
  ~wdResourceManagerWorkerUpdateContent();

  wdResourceLoadData m_LoaderData;
  wdResource* m_pResourceToLoad = nullptr;
  wdResourceTypeLoader* m_pLoader = nullptr;
  // this is only used to clean up a custom loader at the right time, if one is used
  // m_pLoader is always set, no need to go through m_pCustomLoader
  wdUniquePtr<wdResourceTypeLoader> m_pCustomLoader;

private:
  friend class wdResourceManager;
  friend class wdResourceManagerState;
  friend class wdResourceManagerWorkerDataLoad;
  wdResourceManagerWorkerUpdateContent();

  virtual void Execute() override;
};
