#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/IO/Archive/Archive.h>
#include <Foundation/IO/Archive/DataDirTypeArchive.h>
#include <Foundation/IO/FileSystem/DataDirTypeFolder.h>
#include <Foundation/IO/FileSystem/FileReader.h>
#include <Foundation/IO/FileSystem/FileSystem.h>
#include <Foundation/IO/FileSystem/FileWriter.h>
#include <Foundation/System/Process.h>
#include <Foundation/Utilities/CommandLineUtils.h>

#if (WD_ENABLED(WD_SUPPORTS_FILE_ITERATORS) && WD_ENABLED(WD_SUPPORTS_FILE_STATS) && defined(BUILDSYSTEM_HAS_ARCHIVE_TOOL))

WD_CREATE_SIMPLE_TEST(IO, Archive)
{
  wdStringBuilder sOutputFolder = wdTestFramework::GetInstance()->GetAbsOutputPath();
  sOutputFolder.AppendPath("ArchiveTest");
  sOutputFolder.MakeCleanPath();

  // make sure it is empty
  wdOSFile::DeleteFolder(sOutputFolder).IgnoreResult();
  wdOSFile::CreateDirectoryStructure(sOutputFolder).IgnoreResult();

  if (!WD_TEST_BOOL(wdFileSystem::AddDataDirectory(sOutputFolder, "Clear", "output", wdFileSystem::AllowWrites).Succeeded()))
    return;

  const char* szTestData = "TestData";
  const char* szUnpackedData = "Unpacked";

  // write a couple of files for packaging
  const char* szFileList[] = {
    "File1.txt",
    "FolderA/File2.jpg", // should get stored uncompressed
    "FolderB/File3.txt",
    "FolderA/FolderC/File4.zip", // should get stored uncompressed
    "FolderA/FolderD/File5.txt",
    "File6.txt",
  };

  const wdUInt32 uiMinFileSize = 1024 * 128;


  WD_TEST_BLOCK(wdTestBlock::Enabled, "Generate Data")
  {
    wdUInt64 uiValue = 0;

    wdStringBuilder fileName;

    for (wdUInt32 uiFileIdx = 0; uiFileIdx < WD_ARRAY_SIZE(szFileList); ++uiFileIdx)
    {
      fileName.Set(":output/", szTestData, "/", szFileList[uiFileIdx]);

      wdFileWriter file;
      if (!WD_TEST_BOOL(file.Open(fileName).Succeeded()))
        return;

      for (wdUInt32 i = 0; i < uiMinFileSize * uiFileIdx; ++i)
      {
        file << uiValue;
        ++uiValue;
      }
    }
  }

  const wdStringBuilder sArchiveFolder(sOutputFolder, "/", szTestData);
  const wdStringBuilder sUnpackFolder(sOutputFolder, "/", szUnpackedData);
  const wdStringBuilder sArchiveFile(sOutputFolder, "/", szTestData, ".FResources");

  wdStringBuilder pathToArchiveTool = wdCommandLineUtils::GetGlobalInstance()->GetParameter(0);
  pathToArchiveTool.PathParentDirectory();
  pathToArchiveTool.AppendPath("ArchiveTool.exe");

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Create a Package")
  {

    wdProcessOptions opt;
    opt.m_sProcess = pathToArchiveTool;
    opt.m_Arguments.PushBack(sArchiveFolder);

    wdInt32 iReturnValue = 1;

    wdProcess ArchiveToolProc;
    if (!WD_TEST_BOOL(ArchiveToolProc.Execute(opt, &iReturnValue).Succeeded()))
      return;

    WD_TEST_INT(iReturnValue, 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Unpack the Package")
  {

    wdProcessOptions opt;
    opt.m_sProcess = pathToArchiveTool;
    opt.m_Arguments.PushBack("-unpack");
    opt.m_Arguments.PushBack(sArchiveFile);
    opt.m_Arguments.PushBack("-out");
    opt.m_Arguments.PushBack(sUnpackFolder);

    wdInt32 iReturnValue = 1;

    wdProcess ArchiveToolProc;
    if (!WD_TEST_BOOL(ArchiveToolProc.Execute(opt, &iReturnValue).Succeeded()))
      return;

    WD_TEST_INT(iReturnValue, 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Compare unpacked data")
  {
    wdUInt64 uiValue = 0;

    wdStringBuilder sFileSrc;
    wdStringBuilder sFileDst;

    for (wdUInt32 uiFileIdx = 0; uiFileIdx < WD_ARRAY_SIZE(szFileList); ++uiFileIdx)
    {
      sFileSrc.Set(sOutputFolder, "/", szTestData, "/", szFileList[uiFileIdx]);
      sFileDst.Set(sOutputFolder, "/", szUnpackedData, "/", szFileList[uiFileIdx]);

      WD_TEST_FILES(sFileSrc, sFileDst, "Unpacked file should be identical");
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Mount as Data Dir")
  {
    if (!WD_TEST_BOOL(wdFileSystem::AddDataDirectory(sArchiveFile, "Clear", "archive", wdFileSystem::ReadOnly) == WD_SUCCESS))
      return;

    wdStringBuilder sFileSrc;
    wdStringBuilder sFileDst;

    // test opening multiple files in parallel and keeping them open
    wdFileReader readers[WD_ARRAY_SIZE(szFileList)];
    for (wdUInt32 uiFileIdx = 0; uiFileIdx < WD_ARRAY_SIZE(szFileList); ++uiFileIdx)
    {
      sFileDst.Set(":archive/", szFileList[uiFileIdx]);
      WD_TEST_BOOL(readers[uiFileIdx].Open(sFileDst).Succeeded());

      // advance the reader a bit
      WD_TEST_INT(readers[uiFileIdx].SkipBytes(uiMinFileSize * uiFileIdx), uiMinFileSize * uiFileIdx);
    }

    for (wdUInt32 uiFileIdx = 0; uiFileIdx < WD_ARRAY_SIZE(szFileList); ++uiFileIdx)
    {
      sFileSrc.Set(":output/", szTestData, "/", szFileList[uiFileIdx]);
      sFileDst.Set(":archive/", szFileList[uiFileIdx]);

      WD_TEST_FILES(sFileSrc, sFileDst, "Unpacked file should be identical");
    }

    // mount a second time
    if (!WD_TEST_BOOL(wdFileSystem::AddDataDirectory(sArchiveFile, "Clear", "archive2", wdFileSystem::ReadOnly) == WD_SUCCESS))
      return;
  }

  wdFileSystem::RemoveDataDirectoryGroup("Clear");
}

#endif
