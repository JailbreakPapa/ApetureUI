#pragma once

#include <Core/ResourceManager/Implementation/Declarations.h>
#include <Foundation/IO/MemoryStream.h>
#include <Foundation/IO/Stream.h>
#include <Foundation/Time/Timestamp.h>

/// \brief Data returned by wdResourceTypeLoader implementations.
struct WD_CORE_DLL wdResourceLoadData
{
  /// Additional (optional) description that can help during debugging (e.g. the final file path).
  wdString m_sResourceDescription;

  /// Used to keep track when the loaded file was modified last and thus when reloading of the resource might be necessary.
  wdTimestamp m_LoadedFileModificationDate;

  /// All loaded data should be stored in a memory stream. This stream reader allows the resource to read the memory stream.
  wdStreamReader* m_pDataStream = nullptr;

  /// Custom loader data, e.g. a pointer to a custom memory block, that needs to be freed when the resource is done updating.
  void* m_pCustomLoaderData = nullptr;
};

/// \brief Base class for all resource loaders.
///
/// A resource loader handles preparing the data before the resource is updated with the data.
/// Resource loaders are always executed on a separate thread.
class WD_CORE_DLL wdResourceTypeLoader
{
public:
  wdResourceTypeLoader() {}
  virtual ~wdResourceTypeLoader() {}

  /// \brief Override this function to implement the resource loading.
  ///
  /// This function should take the information from \a pResource, e.g. which file to load, and do the loading work.
  /// It should allocate temporary storage for the loaded data and encode it in a memory stream, such that the
  /// resource can read all necessary information from the stream.
  ///
  /// \sa wdResourceLoadData
  virtual wdResourceLoadData OpenDataStream(const wdResource* pResource) = 0;

  /// \brief This function is called when the resource has been updated with the data from the resource loader and the loader can deallocate
  /// any temporary memory.
  virtual void CloseDataStream(const wdResource* pResource, const wdResourceLoadData& loaderData) = 0;

  /// \brief If this function returns true, a resource is unloaded and loaded again to update its content.
  ///
  /// Call wdResource::GetLoadedFileModificationTime() to query the file modification time that was returned
  /// through wdResourceLoadData::m_LoadedFileModificationDate.
  virtual bool IsResourceOutdated(const wdResource* pResource) const { return false; }
};

/// \brief A default implementation of wdResourceTypeLoader for standard file loading.
///
/// The loader will interpret the wdResource 'resource ID' as a path, read that full file into a memory stream.
/// The file modification data is stored as well.
/// Resources that use this loader can update their data as if they were reading the file directly.
class WD_CORE_DLL wdResourceLoaderFromFile : public wdResourceTypeLoader
{
public:
  virtual wdResourceLoadData OpenDataStream(const wdResource* pResource) override;
  virtual void CloseDataStream(const wdResource* pResource, const wdResourceLoadData& loaderData) override;
  virtual bool IsResourceOutdated(const wdResource* pResource) const override;
};


/// \brief A resource loader that is mainly used to update a resource on the fly with custom data, e.g. in an editor
///
/// Use like this:
/// Allocate a wdResourceLoaderFromMemory instance on the heap, using WD_DEFAULT_NEW and store the result in a
/// wdUniquePtr<wdResourceTypeLoader>. Then set the description, the modification time (simply use wdTimestamp::CurrentTimestamp()), and the
/// custom data. Use a wdMemoryStreamWriter to write your custom data. Make sure to write EXACTLY the same format that the targeted resource
/// type would read, including all data that would typically be written by outside code, e.g. the default wdResourceLoaderFromFile
/// additionally writes the path to the resource at the start of the stream. If such data is usually present in the stream, you must write
/// this yourself. Then call wdResourceManager::UpdateResourceWithCustomLoader(), specify the target resource and std::move your created
/// loader in there.
class WD_CORE_DLL wdResourceLoaderFromMemory : public wdResourceTypeLoader
{
public:
  virtual wdResourceLoadData OpenDataStream(const wdResource* pResource) override;
  virtual void CloseDataStream(const wdResource* pResource, const wdResourceLoadData& loaderData) override;
  virtual bool IsResourceOutdated(const wdResource* pResource) const override;

  wdString m_sResourceDescription;
  wdTimestamp m_ModificationTimestamp;
  wdDefaultMemoryStreamStorage m_CustomData;

private:
  wdMemoryStreamReader m_Reader;
};
