#pragma once

#include <Core/Collection/CollectionResource.h>

class wdHashedString;

namespace wdCollectionUtils
{
  /// \brief Adds all files from \a szAbsPathToFolder and \a szFileExtension to \a collection
  ///
  /// The files are added as new entries using szAssetTypeName as the resource type identifier (see wdResourceManager::RegisterResourceForAssetType).
  /// \a szStripPrefix is stripped from the file system paths and \a szPrependPrefix is prepended.
  WD_CORE_DLL void AddFiles(wdCollectionResourceDescriptor& ref_collection, wdStringView sAssetTypeName, wdStringView sAbsPathToFolder,
    wdStringView sFileExtension, wdStringView sStripPrefix, wdStringView sPrependPrefix);

  /// \brief Merges all collections from the input array into the target result collection. Resource entries will be de-duplicated by resource ID
  /// string.
  WD_CORE_DLL void MergeCollections(wdCollectionResourceDescriptor& ref_result, wdArrayPtr<const wdCollectionResourceDescriptor*> inputCollections);

  /// \brief Special case of wdCollectionUtils::MergeCollections which outputs unique entries from input collection into the result collection
  WD_CORE_DLL void DeDuplicateEntries(wdCollectionResourceDescriptor& ref_result, const wdCollectionResourceDescriptor& input);

  /// \brief Extracts info (i.e. resource ID as file path) from the passed handle and adds it as a new resource entry. Does not add an entry if the
  /// resource handle is not valid.
  ///
  /// The resource type identifier must be passed explicity as szAssetTypeName (see wdResourceManager::RegisterResourceForAssetType). To determine the
  /// file size, the resource ID is used as a filename passed to wdFileSystem::GetFileStats. In case the resource's path root is not mounted, the path
  /// root can be replaced by passing non-NULL string to szAbsFolderpath, which will replace the root, e.g. with an absolute file path. This is just
  /// for the file size check within the scope of the function, it will not modify the resource Id.
  WD_CORE_DLL void AddResourceHandle(wdCollectionResourceDescriptor& ref_collection, wdTypelessResourceHandle hHandle, wdStringView sAssetTypeName, wdStringView sAbsFolderpath);

}; // namespace wdCollectionUtils
