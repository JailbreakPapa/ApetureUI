#include <Core/CorePCH.h>

#include <Core/Collection/CollectionUtils.h>
#include <Foundation/IO/FileSystem/FileSystem.h>
#include <Foundation/IO/OSFile.h>

void wdCollectionUtils::AddFiles(wdCollectionResourceDescriptor& ref_collection, wdStringView sAssetTypeNameView, wdStringView sAbsPathToFolder, wdStringView sFileExtension, wdStringView sStripPrefix, wdStringView sPrependPrefix)
{
#if WD_ENABLED(WD_SUPPORTS_FILE_ITERATORS)

  const wdUInt32 uiStripPrefixLength = wdStringUtils::GetCharacterCount(sStripPrefix.GetStartPointer(), sStripPrefix.GetEndPointer());

  wdFileSystemIterator fsIt;
  fsIt.StartSearch(sAbsPathToFolder, wdFileSystemIteratorFlags::ReportFilesRecursive);

  if (!fsIt.IsValid())
    return;

  wdStringBuilder sFullPath;
  wdHashedString sAssetTypeName;
  sAssetTypeName.Assign(sAssetTypeNameView);

  for (; fsIt.IsValid(); fsIt.Next())
  {
    const auto& stats = fsIt.GetStats();

    if (wdPathUtils::HasExtension(stats.m_sName, sFileExtension))
    {
      stats.GetFullPath(sFullPath);

      sFullPath.Shrink(uiStripPrefixLength, 0);
      sFullPath.Prepend(sPrependPrefix);
      sFullPath.MakeCleanPath();

      auto& entry = ref_collection.m_Resources.ExpandAndGetRef();
      entry.m_sAssetTypeName = sAssetTypeName;
      entry.m_sResourceID = sFullPath;
      entry.m_uiFileSize = stats.m_uiFileSize;
    }
  }

#else
  WD_ASSERT_NOT_IMPLEMENTED;
#endif
}


WD_CORE_DLL void wdCollectionUtils::MergeCollections(wdCollectionResourceDescriptor& ref_result, wdArrayPtr<const wdCollectionResourceDescriptor*> inputCollections)
{
  wdMap<wdString, const wdCollectionEntry*> firstEntryOfID;

  for (const wdCollectionResourceDescriptor* inputDesc : inputCollections)
  {
    for (const wdCollectionEntry& inputEntry : inputDesc->m_Resources)
    {
      if (!firstEntryOfID.Contains(inputEntry.m_sResourceID))
      {
        firstEntryOfID.Insert(inputEntry.m_sResourceID, &inputEntry);
        ref_result.m_Resources.PushBack(inputEntry);
      }
    }
  }
}


WD_CORE_DLL void wdCollectionUtils::DeDuplicateEntries(wdCollectionResourceDescriptor& ref_result, const wdCollectionResourceDescriptor& input)
{
  const wdCollectionResourceDescriptor* firstInput = &input;
  MergeCollections(ref_result, wdArrayPtr<const wdCollectionResourceDescriptor*>(&firstInput, 1));
}

void wdCollectionUtils::AddResourceHandle(wdCollectionResourceDescriptor& ref_collection, wdTypelessResourceHandle hHandle, wdStringView sAssetTypeName, wdStringView sAbsFolderpath)
{
  if (!hHandle.IsValid())
    return;

  const char* resID = hHandle.GetResourceID();

  auto& entry = ref_collection.m_Resources.ExpandAndGetRef();

  entry.m_sAssetTypeName.Assign(sAssetTypeName);
  entry.m_sResourceID = resID;

  wdStringBuilder absFilename;

  // if a folder path is specified, replace the root (for testing filesize below)
  if (!sAbsFolderpath.IsEmpty())
  {
    wdStringView root, relFile;
    wdPathUtils::GetRootedPathParts(resID, root, relFile);
    absFilename = sAbsFolderpath;
    absFilename.AppendPath(relFile.GetStartPointer());
    absFilename.MakeCleanPath();

    wdFileStats stats;
    if (!absFilename.IsEmpty() && absFilename.IsAbsolutePath() && wdFileSystem::GetFileStats(absFilename, stats).Succeeded())
    {
      entry.m_uiFileSize = stats.m_uiFileSize;
    }
  }
}

WD_STATICLINK_FILE(Core, Core_Collection_Implementation_CollectionUtils);
