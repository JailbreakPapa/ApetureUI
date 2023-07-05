#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/IO/OSFile.h>

WD_CREATE_SIMPLE_TEST(IO, OSFile)
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

  const wdUInt32 uiTextLen = sFileContent.GetElementCount();

  wdStringBuilder sOutputFile = wdTestFramework::GetInstance()->GetAbsOutputPath();
  sOutputFile.MakeCleanPath();
  sOutputFile.AppendPath("IO", "SubFolder");
  sOutputFile.AppendPath("OSFile_TestFile.txt");

  wdStringBuilder sOutputFile2 = wdTestFramework::GetInstance()->GetAbsOutputPath();
  sOutputFile2.MakeCleanPath();
  sOutputFile2.AppendPath("IO", "SubFolder2");
  sOutputFile2.AppendPath("OSFile_TestFileCopy.txt");

  wdStringBuilder sOutputFile3 = wdTestFramework::GetInstance()->GetAbsOutputPath();
  sOutputFile3.MakeCleanPath();
  sOutputFile3.AppendPath("IO", "SubFolder2", "SubSubFolder");
  sOutputFile3.AppendPath("RandomFile.txt");

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Write File")
  {
    wdOSFile f;
    WD_TEST_BOOL(f.Open(sOutputFile.GetData(), wdFileOpenMode::Write) == WD_SUCCESS);
    WD_TEST_BOOL(f.IsOpen());
    WD_TEST_INT(f.GetFilePosition(), 0);
    WD_TEST_INT(f.GetFileSize(), 0);

    for (wdUInt32 i = 0; i < uiTextLen; ++i)
    {
      WD_TEST_BOOL(f.Write(&sFileContent.GetData()[i], 1) == WD_SUCCESS);
      WD_TEST_INT(f.GetFilePosition(), i + 1);
      WD_TEST_INT(f.GetFileSize(), i + 1);
    }

    WD_TEST_INT(f.GetFilePosition(), uiTextLen);
    f.SetFilePosition(5, wdFileSeekMode::FromStart);
    WD_TEST_INT(f.GetFileSize(), uiTextLen);

    WD_TEST_INT(f.GetFilePosition(), 5);
    // f.Close(); // The file should be closed automatically
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Append File")
  {
    wdOSFile f;
    WD_TEST_BOOL(f.Open(sOutputFile.GetData(), wdFileOpenMode::Append) == WD_SUCCESS);
    WD_TEST_BOOL(f.IsOpen());
    WD_TEST_INT(f.GetFilePosition(), uiTextLen);
    WD_TEST_BOOL(f.Write(sFileContent.GetData(), uiTextLen) == WD_SUCCESS);
    WD_TEST_INT(f.GetFilePosition(), uiTextLen * 2);
    f.Close();
    WD_TEST_BOOL(!f.IsOpen());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Read File")
  {
    const wdUInt32 FS_MAX_PATH = 1024;
    char szTemp[FS_MAX_PATH];

    wdOSFile f;
    WD_TEST_BOOL(f.Open(sOutputFile.GetData(), wdFileOpenMode::Read) == WD_SUCCESS);
    WD_TEST_BOOL(f.IsOpen());
    WD_TEST_INT(f.GetFilePosition(), 0);

    WD_TEST_INT(f.Read(szTemp, FS_MAX_PATH), uiTextLen * 2);
    WD_TEST_INT(f.GetFilePosition(), uiTextLen * 2);

    WD_TEST_BOOL(wdMemoryUtils::IsEqual(szTemp, sFileContent.GetData(), uiTextLen));
    WD_TEST_BOOL(wdMemoryUtils::IsEqual(&szTemp[uiTextLen], sFileContent.GetData(), uiTextLen));

    f.Close();
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Copy File")
  {
    wdOSFile::CopyFile(sOutputFile.GetData(), sOutputFile2.GetData()).IgnoreResult();

    wdOSFile f;
    WD_TEST_BOOL(f.Open(sOutputFile2.GetData(), wdFileOpenMode::Read) == WD_SUCCESS);

    const wdUInt32 FS_MAX_PATH = 1024;
    char szTemp[FS_MAX_PATH];

    WD_TEST_INT(f.Read(szTemp, FS_MAX_PATH), uiTextLen * 2);

    WD_TEST_BOOL(wdMemoryUtils::IsEqual(szTemp, sFileContent.GetData(), uiTextLen));
    WD_TEST_BOOL(wdMemoryUtils::IsEqual(&szTemp[uiTextLen], sFileContent.GetData(), uiTextLen));

    f.Close();
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ReadAll")
  {
    wdOSFile f;
    WD_TEST_BOOL(f.Open(sOutputFile, wdFileOpenMode::Read) == WD_SUCCESS);

    wdDynamicArray<wdUInt8> fileContent;
    const wdUInt64 bytes = f.ReadAll(fileContent);

    WD_TEST_INT(bytes, uiTextLen * 2);

    WD_TEST_BOOL(wdMemoryUtils::IsEqual(fileContent.GetData(), (const wdUInt8*)sFileContent.GetData(), uiTextLen));

    f.Close();
  }

#if WD_ENABLED(WD_SUPPORTS_FILE_STATS)
  WD_TEST_BLOCK(wdTestBlock::Enabled, "File Stats")
  {
    wdFileStats s;

    wdStringBuilder dir = sOutputFile2.GetFileDirectory();

    WD_TEST_BOOL(wdOSFile::GetFileStats(sOutputFile2.GetData(), s) == WD_SUCCESS);
    // printf("%s Name: '%s' (%lli Bytes), Modified Time: %lli\n", s.m_bIsDirectory ? "Directory" : "File", s.m_sFileName.GetData(),
    // s.m_uiFileSize, s.m_LastModificationTime.GetInt64(wdSIUnitOfTime::Microsecond));

    WD_TEST_BOOL(wdOSFile::GetFileStats(dir.GetData(), s) == WD_SUCCESS);
    // printf("%s Name: '%s' (%lli Bytes), Modified Time: %lli\n", s.m_bIsDirectory ? "Directory" : "File", s.m_sFileName.GetData(),
    // s.m_uiFileSize, s.m_LastModificationTime.GetInt64(wdSIUnitOfTime::Microsecond));
  }

#  if (WD_ENABLED(WD_SUPPORTS_CASE_INSENSITIVE_PATHS) && WD_ENABLED(WD_SUPPORTS_UNRESTRICTED_FILE_ACCESS))
  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetFileCasing")
  {
    wdStringBuilder dir = sOutputFile2;
    dir.ToLower();

#    if WD_ENABLED(WD_PLATFORM_WINDOWS)
    // On Windows the drive letter will always be turned upper case by wdOSFile::GetFileCasing()
    // ensure that our input data ('ground truth') also uses an upper case drive letter
    auto driveLetterIterator = sOutputFile2.GetIteratorFront();
    const wdUInt32 uiDriveLetter = wdStringUtils::ToUpperChar(driveLetterIterator.GetCharacter());
    sOutputFile2.ChangeCharacter(driveLetterIterator, uiDriveLetter);
#    endif

    wdStringBuilder sCorrected;
    WD_TEST_BOOL(wdOSFile::GetFileCasing(dir.GetData(), sCorrected) == WD_SUCCESS);

    // On Windows the drive letter will always be made to upper case
    WD_TEST_STRING(sCorrected.GetData(), sOutputFile2.GetData());
  }
#  endif // WD_SUPPORTS_CASE_INSENSITIVE_PATHS && WD_SUPPORTS_UNRESTRICTED_FILE_ACCESS

#endif // WD_SUPPORTS_FILE_STATS

#if WD_ENABLED(WD_SUPPORTS_FILE_ITERATORS)

  WD_TEST_BLOCK(wdTestBlock::Enabled, "File Iterator")
  {
    // It is not really possible to test this stuff (with a guaranteed result), as long as we do not have
    // a test data folder with deterministic content
    // Therefore I tested it manually, and leave the code in, such that it is at least a 'does it compile and link' test.

    wdStringBuilder sOutputFolder = wdOSFile::GetApplicationDirectory();
    sOutputFolder.AppendPath("*");

    wdStringBuilder sFullPath;

    wdUInt32 uiFolders = 0;
    wdUInt32 uiFiles = 0;

    bool bSkipFolder = true;

    wdFileSystemIterator it;
    for (it.StartSearch(sOutputFolder.GetData(), wdFileSystemIteratorFlags::ReportFilesAndFoldersRecursive); it.IsValid();)
    {
      sFullPath = it.GetCurrentPath();
      sFullPath.AppendPath(it.GetStats().m_sName.GetData());

      it.GetStats();
      it.GetCurrentPath();

      if (it.GetStats().m_bIsDirectory)
      {
        ++uiFolders;
        bSkipFolder = !bSkipFolder;

        if (bSkipFolder)
        {
          it.SkipFolder(); // replaces the 'Next' call
          continue;
        }
      }
      else
      {
        ++uiFiles;
      }

      it.Next();
    }

// The binary folder will only have subdirectories on windows desktop
#  if WD_ENABLED(WD_PLATFORM_WINDOWS_DESKTOP)
    WD_TEST_BOOL(uiFolders > 0);
#  endif
    WD_TEST_BOOL(uiFiles > 0);
  }

#endif

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Delete File")
  {
    WD_TEST_BOOL(wdOSFile::DeleteFile(sOutputFile.GetData()) == WD_SUCCESS);
    WD_TEST_BOOL(wdOSFile::DeleteFile(sOutputFile.GetData()) == WD_SUCCESS); // second time should still 'succeed'

    WD_TEST_BOOL(wdOSFile::DeleteFile(sOutputFile2.GetData()) == WD_SUCCESS);
    WD_TEST_BOOL(wdOSFile::DeleteFile(sOutputFile2.GetData()) == WD_SUCCESS); // second time should still 'succeed'

    wdOSFile f;
    WD_TEST_BOOL(f.Open(sOutputFile.GetData(), wdFileOpenMode::Read) == WD_FAILURE);  // file should not exist anymore
    WD_TEST_BOOL(f.Open(sOutputFile2.GetData(), wdFileOpenMode::Read) == WD_FAILURE); // file should not exist anymore
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetCurrentWorkingDirectory")
  {
    wdStringBuilder cwd = wdOSFile::GetCurrentWorkingDirectory();

    WD_TEST_BOOL(!cwd.IsEmpty());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "MakePathAbsoluteWithCWD")
  {
    wdStringBuilder cwd = wdOSFile::GetCurrentWorkingDirectory();
    wdStringBuilder path = wdOSFile::MakePathAbsoluteWithCWD("sub/folder");

    WD_TEST_BOOL(path.StartsWith(cwd));
    WD_TEST_BOOL(path.EndsWith("/sub/folder"));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ExistsFile")
  {
    WD_TEST_BOOL(wdOSFile::ExistsFile(sOutputFile.GetData()) == false);
    WD_TEST_BOOL(wdOSFile::ExistsFile(sOutputFile2.GetData()) == false);

    {
      wdOSFile f;
      WD_TEST_BOOL(f.Open(sOutputFile.GetData(), wdFileOpenMode::Write) == WD_SUCCESS);
    }

    WD_TEST_BOOL(wdOSFile::ExistsFile(sOutputFile.GetData()) == true);
    WD_TEST_BOOL(wdOSFile::ExistsFile(sOutputFile2.GetData()) == false);

    {
      wdOSFile f;
      WD_TEST_BOOL(f.Open(sOutputFile2.GetData(), wdFileOpenMode::Write) == WD_SUCCESS);
    }

    WD_TEST_BOOL(wdOSFile::ExistsFile(sOutputFile.GetData()) == true);
    WD_TEST_BOOL(wdOSFile::ExistsFile(sOutputFile2.GetData()) == true);

    WD_TEST_BOOL(wdOSFile::DeleteFile(sOutputFile.GetData()) == WD_SUCCESS);
    WD_TEST_BOOL(wdOSFile::DeleteFile(sOutputFile2.GetData()) == WD_SUCCESS);

    WD_TEST_BOOL(wdOSFile::ExistsFile(sOutputFile.GetData()) == false);
    WD_TEST_BOOL(wdOSFile::ExistsFile(sOutputFile2.GetData()) == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ExistsDirectory")
  {
    // files are not folders
    WD_TEST_BOOL(wdOSFile::ExistsDirectory(sOutputFile.GetData()) == false);
    WD_TEST_BOOL(wdOSFile::ExistsDirectory(sOutputFile2.GetData()) == false);

    wdStringBuilder sOutputFolder = wdTestFramework::GetInstance()->GetAbsOutputPath();
    WD_TEST_BOOL(wdOSFile::ExistsDirectory(sOutputFolder) == true);

    sOutputFile.AppendPath("IO");
    WD_TEST_BOOL(wdOSFile::ExistsDirectory(sOutputFolder) == true);

    sOutputFile.AppendPath("SubFolder");
    WD_TEST_BOOL(wdOSFile::ExistsDirectory(sOutputFolder) == true);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetApplicationDirectory")
  {
    const char* szAppDir = wdOSFile::GetApplicationDirectory();
    WD_IGNORE_UNUSED(szAppDir);
  }

#if (WD_ENABLED(WD_SUPPORTS_FILE_ITERATORS) && WD_ENABLED(WD_SUPPORTS_FILE_STATS))

  WD_TEST_BLOCK(wdTestBlock::Enabled, "DeleteFolder")
  {
    {
      wdOSFile f;
      WD_TEST_BOOL(f.Open(sOutputFile3.GetData(), wdFileOpenMode::Write) == WD_SUCCESS);
    }

    wdStringBuilder SubFolder2 = wdTestFramework::GetInstance()->GetAbsOutputPath();
    SubFolder2.MakeCleanPath();
    SubFolder2.AppendPath("IO", "SubFolder2");

    WD_TEST_BOOL(wdOSFile::DeleteFolder(SubFolder2).Succeeded());
    WD_TEST_BOOL(!wdOSFile::ExistsDirectory(SubFolder2));
  }

#endif
}
