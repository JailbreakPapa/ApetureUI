#include <Core/CorePCH.h>

#include <Core/ResourceManager/Resource.h>
#include <Core/ResourceManager/ResourceTypeLoader.h>
#include <Foundation/Containers/Blob.h>
#include <Foundation/IO/FileSystem/FileReader.h>
#include <Foundation/IO/MemoryStream.h>
#include <Foundation/IO/OSFile.h>
#include <Foundation/Profiling/Profiling.h>

struct FileResourceLoadData
{
  wdBlob m_Storage;
  wdRawMemoryStreamReader m_Reader;
};

wdResourceLoadData wdResourceLoaderFromFile::OpenDataStream(const wdResource* pResource)
{
  WD_PROFILE_SCOPE("ReadResourceFile");

  wdResourceLoadData res;

  wdFileReader File;
  if (File.Open(pResource->GetResourceID().GetData()).Failed())
    return res;

  res.m_sResourceDescription = File.GetFilePathRelative().GetData();

#if WD_ENABLED(WD_SUPPORTS_FILE_STATS)
  wdFileStats stat;
  if (wdFileSystem::GetFileStats(pResource->GetResourceID(), stat).Succeeded())
  {
    res.m_LoadedFileModificationDate = stat.m_LastModificationTime;
  }

#endif

  FileResourceLoadData* pData = WD_DEFAULT_NEW(FileResourceLoadData);

  const wdUInt64 uiFileSize = File.GetFileSize();

  const wdUInt64 uiBlobCapacity = uiFileSize + File.GetFilePathAbsolute().GetElementCount() + 8; // +8 for the string overhead
  pData->m_Storage.SetCountUninitialized(uiBlobCapacity);

  wdUInt8* pBlobPtr = pData->m_Storage.GetBlobPtr<wdUInt8>().GetPtr();

  wdRawMemoryStreamWriter w(pBlobPtr, uiBlobCapacity);

  // write the absolute path to the read file into the memory stream
  w << File.GetFilePathAbsolute();

  const wdUInt64 uiOffset = w.GetNumWrittenBytes();

  File.ReadBytes(pBlobPtr + uiOffset, uiFileSize);

  pData->m_Reader.Reset(pBlobPtr, w.GetNumWrittenBytes() + uiFileSize);
  res.m_pDataStream = &pData->m_Reader;
  res.m_pCustomLoaderData = pData;

  return res;
}

void wdResourceLoaderFromFile::CloseDataStream(const wdResource* pResource, const wdResourceLoadData& loaderData)
{
  FileResourceLoadData* pData = static_cast<FileResourceLoadData*>(loaderData.m_pCustomLoaderData);

  WD_DEFAULT_DELETE(pData);
}

bool wdResourceLoaderFromFile::IsResourceOutdated(const wdResource* pResource) const
{
  // if we cannot find the target file, there is no point in trying to reload it -> claim it's up to date
  if (wdFileSystem::ResolvePath(pResource->GetResourceID(), nullptr, nullptr).Failed())
    return false;

#if WD_ENABLED(WD_SUPPORTS_FILE_STATS)

  if (pResource->GetLoadedFileModificationTime().IsValid())
  {
    wdFileStats stat;
    if (wdFileSystem::GetFileStats(pResource->GetResourceID(), stat).Failed())
      return false;

    return !stat.m_LastModificationTime.Compare(pResource->GetLoadedFileModificationTime(), wdTimestamp::CompareMode::FileTimeEqual);
  }

#endif

  return true;
}

//////////////////////////////////////////////////////////////////////////

wdResourceLoadData wdResourceLoaderFromMemory::OpenDataStream(const wdResource* pResource)
{
  m_Reader.SetStorage(&m_CustomData);
  m_Reader.SetReadPosition(0);

  wdResourceLoadData res;

  res.m_sResourceDescription = m_sResourceDescription;
  res.m_LoadedFileModificationDate = m_ModificationTimestamp;
  res.m_pDataStream = &m_Reader;
  res.m_pCustomLoaderData = nullptr;

  return res;
}

void wdResourceLoaderFromMemory::CloseDataStream(const wdResource* pResource, const wdResourceLoadData& loaderData)
{
  m_Reader.SetStorage(nullptr);
}

bool wdResourceLoaderFromMemory::IsResourceOutdated(const wdResource* pResource) const
{
  if (pResource->GetLoadedFileModificationTime().IsValid() && m_ModificationTimestamp.IsValid())
  {
    if (!m_ModificationTimestamp.Compare(pResource->GetLoadedFileModificationTime(), wdTimestamp::CompareMode::FileTimeEqual))
      return true;

    return false;
  }

  return true;
}



WD_STATICLINK_FILE(Core, Core_ResourceManager_Implementation_ResourceTypeLoader);
