#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/IO/FileSystem/DataDirTypeFolder.h>
#include <Foundation/IO/FileSystem/FileReader.h>
#include <Foundation/IO/FileSystem/FileSystem.h>
#include <Foundation/IO/FileSystem/FileWriter.h>

#if WD_ENABLED(WD_SUPPORTS_LONG_PATHS)
#  define LongPath                                                                                                                                   \
    "AVeryLongSubFolderPathNameThatShouldExceedThePathLengthLimitOnPlatformsLikeWindowsWhereOnly260CharactersAreAllowedOhNoesIStillNeedMoreThisIsNo" \
    "tLongEnoughAaaaaaaaaaaaaaahhhhStillTooShortAaaaaaaaaaaaaaaaaaaaaahImBoredNow"
#else
#  define LongPath "AShortPathBecaueThisPlatformDoesntSupportLongOnes"
#endif

WD_CREATE_SIMPLE_TEST(IO, FileSystem)
{
  wdStringBuilder sFileContent = "Lyrics to Taste The Cake:\n\
Turret: Who's there?\n\
Turret: Is anyone there?\n\
Turret: I see you.\n\
\n\
Chell rises from a stasis inside of a glass box\n\
She isn't greeted by faces,\n\
Only concrete and clocks.\n\
...";

  wdStringBuilder szOutputFolder = wdTestFramework::GetInstance()->GetAbsOutputPath();
  szOutputFolder.MakeCleanPath();

  wdStringBuilder sOutputFolderResolved;
  wdFileSystem::ResolveSpecialDirectory(szOutputFolder, sOutputFolderResolved).IgnoreResult();

  wdStringBuilder sOutputFolder1 = szOutputFolder;
  sOutputFolder1.AppendPath("IO", "SubFolder");
  wdStringBuilder sOutputFolder1Resolved;
  wdFileSystem::ResolveSpecialDirectory(sOutputFolder1, sOutputFolder1Resolved).IgnoreResult();

  wdStringBuilder sOutputFolder2 = szOutputFolder;
  sOutputFolder2.AppendPath("IO", "SubFolder2");
  wdStringBuilder sOutputFolder2Resolved;
  wdFileSystem::ResolveSpecialDirectory(sOutputFolder2, sOutputFolder2Resolved).IgnoreResult();

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Setup Data Dirs")
  {
    // adding the same factory three times would actually not make a difference
    wdFileSystem::RegisterDataDirectoryFactory(wdDataDirectory::FolderType::Factory);
    wdFileSystem::RegisterDataDirectoryFactory(wdDataDirectory::FolderType::Factory);
    wdFileSystem::RegisterDataDirectoryFactory(wdDataDirectory::FolderType::Factory);

    // wdFileSystem::ClearAllDataDirectoryFactories();

    wdFileSystem::RegisterDataDirectoryFactory(wdDataDirectory::FolderType::Factory);

    // for absolute paths
    WD_TEST_BOOL(wdFileSystem::AddDataDirectory("", "", ":", wdFileSystem::AllowWrites) == WD_SUCCESS);
    WD_TEST_BOOL(wdFileSystem::AddDataDirectory(szOutputFolder, "Clear", "output", wdFileSystem::AllowWrites) == WD_SUCCESS);

    wdStringBuilder sTempFile = sOutputFolder1Resolved;
    sTempFile.AppendPath(LongPath);
    sTempFile.AppendPath("Temp.tmp");

    wdFileWriter TempFile;
    WD_TEST_BOOL(TempFile.Open(sTempFile) == WD_SUCCESS);
    TempFile.Close();

    sTempFile = sOutputFolder2Resolved;
    sTempFile.AppendPath("Temp.tmp");

    WD_TEST_BOOL(TempFile.Open(sTempFile) == WD_SUCCESS);
    TempFile.Close();

    WD_TEST_BOOL(wdFileSystem::AddDataDirectory(sOutputFolder1, "Clear", "output1", wdFileSystem::AllowWrites) == WD_SUCCESS);
    WD_TEST_BOOL(wdFileSystem::AddDataDirectory(sOutputFolder2, "Clear") == WD_SUCCESS);

    WD_TEST_BOOL(wdFileSystem::AddDataDirectory(sOutputFolder2, "Remove", "output2", wdFileSystem::AllowWrites) == WD_SUCCESS);
    WD_TEST_BOOL(wdFileSystem::AddDataDirectory(sOutputFolder1, "Remove") == WD_SUCCESS);
    WD_TEST_BOOL(wdFileSystem::AddDataDirectory(sOutputFolder2, "Remove") == WD_SUCCESS);

    WD_TEST_INT(wdFileSystem::RemoveDataDirectoryGroup("Remove"), 3);

    WD_TEST_BOOL(wdFileSystem::AddDataDirectory(sOutputFolder2, "Remove", "output2", wdFileSystem::AllowWrites) == WD_SUCCESS);
    WD_TEST_BOOL(wdFileSystem::AddDataDirectory(sOutputFolder1, "Remove") == WD_SUCCESS);
    WD_TEST_BOOL(wdFileSystem::AddDataDirectory(sOutputFolder2, "Remove") == WD_SUCCESS);

    wdFileSystem::ClearAllDataDirectories();

    WD_TEST_INT(wdFileSystem::RemoveDataDirectoryGroup("Remove"), 0);
    WD_TEST_INT(wdFileSystem::RemoveDataDirectoryGroup("Clear"), 0);

    WD_TEST_BOOL(wdFileSystem::AddDataDirectory(sOutputFolder1, "", "output1", wdFileSystem::AllowWrites) == WD_SUCCESS);
    WD_TEST_BOOL(wdFileSystem::AddDataDirectory(sOutputFolder2) == WD_SUCCESS);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Add / Remove Data Dirs")
  {
    WD_TEST_BOOL(wdFileSystem::AddDataDirectory("", "xyz-rooted", "xyz", wdFileSystem::AllowWrites) == WD_SUCCESS);

    WD_TEST_BOOL(wdFileSystem::FindDataDirectoryWithRoot("xyz") != nullptr);

    WD_TEST_BOOL(wdFileSystem::RemoveDataDirectory("xyz") == true);

    WD_TEST_BOOL(wdFileSystem::FindDataDirectoryWithRoot("xyz") == nullptr);

    WD_TEST_BOOL(wdFileSystem::RemoveDataDirectory("xyz") == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Write File")
  {
    wdFileWriter FileOut;

    wdStringBuilder sAbs = sOutputFolder1Resolved;
    sAbs.AppendPath("FileSystemTest.txt");

    WD_TEST_BOOL(FileOut.Open(":output1/FileSystemTest.txt") == WD_SUCCESS);

    WD_TEST_STRING(FileOut.GetFilePathRelative(), "FileSystemTest.txt");
    WD_TEST_STRING(FileOut.GetFilePathAbsolute(), sAbs);

    WD_TEST_INT(FileOut.GetFileSize(), 0);

    WD_TEST_BOOL(FileOut.WriteBytes(sFileContent.GetData(), sFileContent.GetElementCount()) == WD_SUCCESS);

    FileOut.Flush().IgnoreResult();
    WD_TEST_INT(FileOut.GetFileSize(), sFileContent.GetElementCount());

    FileOut.Close();
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Read File")
  {
    wdFileReader FileIn;

    wdStringBuilder sAbs = sOutputFolder1Resolved;
    sAbs.AppendPath("FileSystemTest.txt");

    WD_TEST_BOOL(FileIn.Open("FileSystemTest.txt") == WD_SUCCESS);

    WD_TEST_STRING(FileIn.GetFilePathRelative(), "FileSystemTest.txt");
    WD_TEST_STRING(FileIn.GetFilePathAbsolute(), sAbs);

    WD_TEST_INT(FileIn.GetFileSize(), sFileContent.GetElementCount());

    char szTemp[1024 * 2];
    WD_TEST_INT(FileIn.ReadBytes(szTemp, 1024 * 2), sFileContent.GetElementCount());

    WD_TEST_BOOL(wdMemoryUtils::IsEqual(szTemp, sFileContent.GetData(), sFileContent.GetElementCount()));

    FileIn.Close();
  }

#if WD_DISABLED(WD_PLATFORM_WINDOWS_UWP)

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Read File (Absolute Path)")
  {
    wdFileReader FileIn;

    wdStringBuilder sAbs = sOutputFolder1Resolved;
    sAbs.AppendPath("FileSystemTest.txt");

    WD_TEST_BOOL(FileIn.Open(sAbs) == WD_SUCCESS);

    WD_TEST_STRING(FileIn.GetFilePathRelative(), "FileSystemTest.txt");
    WD_TEST_STRING(FileIn.GetFilePathAbsolute(), sAbs);

    WD_TEST_INT(FileIn.GetFileSize(), sFileContent.GetElementCount());

    char szTemp[1024 * 2];
    WD_TEST_INT(FileIn.ReadBytes(szTemp, 1024 * 2), sFileContent.GetElementCount());

    WD_TEST_BOOL(wdMemoryUtils::IsEqual(szTemp, sFileContent.GetData(), sFileContent.GetElementCount()));

    FileIn.Close();
  }

#endif

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Delete File / Exists File")
  {
    {
      WD_TEST_BOOL(wdFileSystem::ExistsFile(":output1/FileSystemTest.txt"));
      wdFileSystem::DeleteFile(":output1/FileSystemTest.txt");
      WD_TEST_BOOL(!wdFileSystem::ExistsFile("FileSystemTest.txt"));

      wdFileReader FileIn;
      WD_TEST_BOOL(FileIn.Open("FileSystemTest.txt") == WD_FAILURE);
    }

    // very long path names
    {
      wdStringBuilder sTempFile = ":output1";
      sTempFile.AppendPath(LongPath);
      sTempFile.AppendPath("Temp.tmp");

      wdFileWriter TempFile;
      WD_TEST_BOOL(TempFile.Open(sTempFile) == WD_SUCCESS);
      TempFile.Close();

      WD_TEST_BOOL(wdFileSystem::ExistsFile(sTempFile));
      wdFileSystem::DeleteFile(sTempFile);
      WD_TEST_BOOL(!wdFileSystem::ExistsFile(sTempFile));

      wdFileReader FileIn;
      WD_TEST_BOOL(FileIn.Open("FileSystemTest.txt") == WD_FAILURE);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetFileStats")
  {
    const char* szPath = ":output1/" LongPath "/FileSystemTest.txt";

    // Create file
    {
      wdFileWriter FileOut;
      wdStringBuilder sAbs = sOutputFolder1Resolved;
      sAbs.AppendPath("FileSystemTest.txt");
      WD_TEST_BOOL(FileOut.Open(szPath) == WD_SUCCESS);
      FileOut.WriteBytes("Test", 4).IgnoreResult();
    }

    wdFileStats stat;

    WD_TEST_BOOL(wdFileSystem::GetFileStats(szPath, stat).Succeeded());

    WD_TEST_BOOL(!stat.m_bIsDirectory);
    WD_TEST_STRING(stat.m_sName, "FileSystemTest.txt");
    WD_TEST_INT(stat.m_uiFileSize, 4);

    wdFileSystem::DeleteFile(szPath);
    WD_TEST_BOOL(wdFileSystem::GetFileStats(szPath, stat).Failed());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ResolvePath")
  {
    wdStringBuilder sRel, sAbs;

    WD_TEST_BOOL(wdFileSystem::ResolvePath(":output1/FileSystemTest2.txt", &sAbs, &sRel) == WD_SUCCESS);

    wdStringBuilder sExpectedAbs = sOutputFolder1Resolved;
    sExpectedAbs.AppendPath("FileSystemTest2.txt");

    WD_TEST_STRING(sAbs, sExpectedAbs);
    WD_TEST_STRING(sRel, "FileSystemTest2.txt");

    // create a file in the second dir
    {
      WD_TEST_BOOL(wdFileSystem::AddDataDirectory(sOutputFolder2, "Remove", "output2", wdFileSystem::AllowWrites) == WD_SUCCESS);

      {
        wdFileWriter FileOut;
        WD_TEST_BOOL(FileOut.Open(":output2/FileSystemTest2.txt") == WD_SUCCESS);
      }

      WD_TEST_INT(wdFileSystem::RemoveDataDirectoryGroup("Remove"), 1);
    }

    // find the path to an existing file
    {
      WD_TEST_BOOL(wdFileSystem::ResolvePath("FileSystemTest2.txt", &sAbs, &sRel) == WD_SUCCESS);

      sExpectedAbs = sOutputFolder2Resolved;
      sExpectedAbs.AppendPath("FileSystemTest2.txt");

      WD_TEST_STRING(sAbs, sExpectedAbs);
      WD_TEST_STRING(sRel, "FileSystemTest2.txt");
    }

    // find where we would write the file to (ignoring existing files)
    {
      WD_TEST_BOOL(wdFileSystem::ResolvePath(":output1/FileSystemTest2.txt", &sAbs, &sRel) == WD_SUCCESS);

      sExpectedAbs = sOutputFolder1Resolved;
      sExpectedAbs.AppendPath("FileSystemTest2.txt");

      WD_TEST_STRING(sAbs, sExpectedAbs);
      WD_TEST_STRING(sRel, "FileSystemTest2.txt");
    }

    // find where we would write the file to (ignoring existing files)
    {
      WD_TEST_BOOL(wdFileSystem::ResolvePath(":output1/SubSub/FileSystemTest2.txt", &sAbs, &sRel) == WD_SUCCESS);

      sExpectedAbs = sOutputFolder1Resolved;
      sExpectedAbs.AppendPath("SubSub/FileSystemTest2.txt");

      WD_TEST_STRING(sAbs, sExpectedAbs);
      WD_TEST_STRING(sRel, "SubSub/FileSystemTest2.txt");
    }

    wdFileSystem::DeleteFile(":output1/FileSystemTest2.txt");
    wdFileSystem::DeleteFile(":output2/FileSystemTest2.txt");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "FindFolderWithSubPath")
  {
    WD_TEST_BOOL(wdFileSystem::AddDataDirectory(szOutputFolder, "remove", "toplevel", wdFileSystem::AllowWrites) == WD_SUCCESS);
    WD_TEST_BOOL(wdFileSystem::AddDataDirectory(sOutputFolder2, "remove", "output2", wdFileSystem::AllowWrites) == WD_SUCCESS);

    wdStringBuilder StartPath;
    wdStringBuilder SubPath;
    wdStringBuilder result, expected;

    // make sure this exists
    {
      wdFileWriter FileOut;
      WD_TEST_BOOL(FileOut.Open(":output2/FileSystemTest2.txt") == WD_SUCCESS);
    }

    {
      StartPath.Set(sOutputFolder1Resolved, "/SubSub", "/Irrelevant");
      SubPath.Set("DoesNotExist");

      WD_TEST_BOOL(wdFileSystem::FindFolderWithSubPath(result, StartPath, SubPath).Failed());
    }

    {
      StartPath.Set(sOutputFolder1Resolved, "/SubSub", "/Irrelevant");
      SubPath.Set("SubFolder2");
      expected.Set(sOutputFolderResolved, "/IO/");

      WD_TEST_BOOL(wdFileSystem::FindFolderWithSubPath(result, StartPath, SubPath).Succeeded());
      WD_TEST_STRING(result, expected);
    }

    {
      StartPath.Set(sOutputFolder1Resolved, "/SubSub");
      SubPath.Set("IO/SubFolder2");
      expected.Set(sOutputFolderResolved, "/");

      WD_TEST_BOOL(wdFileSystem::FindFolderWithSubPath(result, StartPath, SubPath).Succeeded());
      WD_TEST_STRING(result, expected);
    }

    {
      StartPath.Set(sOutputFolder1Resolved, "/SubSub");
      SubPath.Set("IO/SubFolder2");
      expected.Set(sOutputFolderResolved, "/");

      WD_TEST_BOOL(wdFileSystem::FindFolderWithSubPath(result, StartPath, SubPath).Succeeded());
      WD_TEST_STRING(result, expected);
    }

    {
      StartPath.Set(sOutputFolder1Resolved, "/SubSub", "/Irrelevant");
      SubPath.Set("SubFolder2/FileSystemTest2.txt");
      expected.Set(sOutputFolderResolved, "/IO/");

      WD_TEST_BOOL(wdFileSystem::FindFolderWithSubPath(result, StartPath, SubPath).Succeeded());
      WD_TEST_STRING(result, expected);
    }

    {
      StartPath.Set(":toplevel/IO/SubFolder");
      SubPath.Set("IO/SubFolder2");
      expected.Set(":toplevel/");

      WD_TEST_BOOL(wdFileSystem::FindFolderWithSubPath(result, StartPath, SubPath).Succeeded());
      WD_TEST_STRING(result, expected);
    }

    wdFileSystem::DeleteFile(":output1/FileSystemTest2.txt");
    wdFileSystem::DeleteFile(":output2/FileSystemTest2.txt");

    wdFileSystem::RemoveDataDirectoryGroup("remove");
  }
}
