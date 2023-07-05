#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/IO/FileSystem/DataDirTypeFolder.h>
#include <Foundation/IO/FileSystem/DeferredFileWriter.h>
#include <Foundation/IO/FileSystem/FileReader.h>

WD_CREATE_SIMPLE_TEST(IO, DeferredFileWriter)
{
  WD_TEST_BOOL(wdFileSystem::AddDataDirectory("", "", ":", wdFileSystem::AllowWrites) == WD_SUCCESS);

  const wdStringBuilder szOutputFolder = wdTestFramework::GetInstance()->GetAbsOutputPath();
  wdStringBuilder sOutputFolderResolved;
  wdFileSystem::ResolveSpecialDirectory(szOutputFolder, sOutputFolderResolved).IgnoreResult();

  wdStringBuilder sTempFile = sOutputFolderResolved;
  sTempFile.AppendPath("Temp.tmp");

  // make sure the file does not exist
  wdFileSystem::DeleteFile(sTempFile);
  WD_TEST_BOOL(!wdFileSystem::ExistsFile(sTempFile));

  WD_TEST_BLOCK(wdTestBlock::Enabled, "DeferredFileWriter")
  {
    wdDeferredFileWriter writer;
    writer.SetOutput(sTempFile);

    for (wdUInt64 i = 0; i < 1'000'000; ++i)
    {
      writer << i;
    }

    // does not exist yet
    WD_TEST_BOOL(!wdFileSystem::ExistsFile(sTempFile));
  }

  // now it exists
  WD_TEST_BOOL(wdFileSystem::ExistsFile(sTempFile));

  // check content is correct
  {
    wdFileReader reader;
    WD_TEST_BOOL(reader.Open(sTempFile).Succeeded());

    for (wdUInt64 i = 0; i < 1'000'000; ++i)
    {
      wdUInt64 v;
      reader >> v;
      WD_TEST_BOOL(v == i);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "DeferredFileWriter2")
  {
    wdDeferredFileWriter writer;
    writer.SetOutput(sTempFile);

    for (wdUInt64 i = 1; i < 100'000; ++i)
    {
      writer << i;
    }

    // does exist from earlier
    WD_TEST_BOOL(wdFileSystem::ExistsFile(sTempFile));

    // check content is as previous correct
    {
      wdFileReader reader;
      WD_TEST_BOOL(reader.Open(sTempFile).Succeeded());

      for (wdUInt64 i = 0; i < 1'000'000; ++i)
      {
        wdUInt64 v;
        reader >> v;
        WD_TEST_BOOL(v == i);
      }
    }
  }

  // exist but now was overwritten
  WD_TEST_BOOL(wdFileSystem::ExistsFile(sTempFile));

  // check content is as previous correct
  {
    wdFileReader reader;
    WD_TEST_BOOL(reader.Open(sTempFile).Succeeded());

    for (wdUInt64 i = 1; i < 100'000; ++i)
    {
      wdUInt64 v;
      reader >> v;
      WD_TEST_BOOL(v == i);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Discard")
  {
    wdStringBuilder sTempFile2 = sOutputFolderResolved;
    sTempFile2.AppendPath("Temp2.tmp");
    {
      wdDeferredFileWriter writer;
      writer.SetOutput(sTempFile2);
      writer << 10;
      writer.Discard();
    }
    WD_TEST_BOOL(!wdFileSystem::ExistsFile(sTempFile2));
  }

  wdFileSystem::DeleteFile(sTempFile);
  wdFileSystem::ClearAllDataDirectories();
}
