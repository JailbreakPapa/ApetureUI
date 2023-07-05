#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/IO/MemoryMappedFile.h>
#include <Foundation/IO/OSFile.h>

#if WD_ENABLED(WD_SUPPORTS_MEMORY_MAPPED_FILE)

WD_CREATE_SIMPLE_TEST(IO, MemoryMappedFile)
{
  wdStringBuilder sOutputFile = wdTestFramework::GetInstance()->GetAbsOutputPath();
  sOutputFile.MakeCleanPath();
  sOutputFile.AppendPath("IO");
  sOutputFile.AppendPath("MemoryMappedFile.dat");

  const wdUInt32 uiFileSize = 1024 * 1024 * 16; // * 4

  // generate test data
  {
    wdOSFile file;
    if (!WD_TEST_BOOL_MSG(file.Open(sOutputFile, wdFileOpenMode::Write).Succeeded(), "File for memory mapping could not be created"))
      return;

    wdDynamicArray<wdUInt32> data;
    data.SetCountUninitialized(uiFileSize);

    for (wdUInt32 i = 0; i < uiFileSize; ++i)
    {
      data[i] = i;
    }

    file.Write(data.GetData(), data.GetCount() * sizeof(wdUInt32)).IgnoreResult();
    file.Close();
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Memory map for writing")
  {
    wdMemoryMappedFile memFile;

    if (!WD_TEST_BOOL_MSG(memFile.Open(sOutputFile, wdMemoryMappedFile::Mode::ReadWrite).Succeeded(), "Memory mapping a file failed"))
      return;

    WD_TEST_BOOL(memFile.GetWritePointer() != nullptr);
    WD_TEST_INT(memFile.GetFileSize(), uiFileSize * sizeof(wdUInt32));

    wdUInt32* ptr = static_cast<wdUInt32*>(memFile.GetWritePointer());

    for (wdUInt32 i = 0; i < uiFileSize; ++i)
    {
      WD_TEST_INT(ptr[i], i);
      ptr[i] = ptr[i] + 1;
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Memory map for reading")
  {
    wdMemoryMappedFile memFile;

    if (!WD_TEST_BOOL_MSG(memFile.Open(sOutputFile, wdMemoryMappedFile::Mode::ReadOnly).Succeeded(), "Memory mapping a file failed"))
      return;

    WD_TEST_BOOL(memFile.GetReadPointer() != nullptr);
    WD_TEST_INT(memFile.GetFileSize(), uiFileSize * sizeof(wdUInt32));

    const wdUInt32* ptr = static_cast<const wdUInt32*>(memFile.GetReadPointer());

    for (wdUInt32 i = 0; i < uiFileSize; ++i)
    {
      WD_TEST_INT(ptr[i], i + 1);
    }

    // try to map it a second time
    wdMemoryMappedFile memFile2;

    if (!WD_TEST_BOOL_MSG(memFile2.Open(sOutputFile, wdMemoryMappedFile::Mode::ReadOnly).Succeeded(), "Memory mapping a file twice failed"))
      return;
  }

  wdOSFile::DeleteFile(sOutputFile).IgnoreResult();
}
#endif
