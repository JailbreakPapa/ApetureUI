#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Strings/String.h>

WD_CREATE_SIMPLE_TEST(Strings, PathUtils)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsPathSeparator")
  {
    for (int i = 0; i < 0xFFFF; ++i)
    {
      if (i == '/')
      {
        WD_TEST_BOOL(wdPathUtils::IsPathSeparator(i));
      }
      else if (i == '\\')
      {
        WD_TEST_BOOL(wdPathUtils::IsPathSeparator(i));
      }
      else
      {
        WD_TEST_BOOL(!wdPathUtils::IsPathSeparator(i));
      }
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "FindPreviousSeparator")
  {
    const char* szPath = "This/Is\\My//Path.dot\\file.extension";

    WD_TEST_BOOL(wdPathUtils::FindPreviousSeparator(szPath, szPath + 35) == szPath + 20);
    WD_TEST_BOOL(wdPathUtils::FindPreviousSeparator(szPath, szPath + 20) == szPath + 11);
    WD_TEST_BOOL(wdPathUtils::FindPreviousSeparator(szPath, szPath + 11) == szPath + 10);
    WD_TEST_BOOL(wdPathUtils::FindPreviousSeparator(szPath, szPath + 10) == szPath + 7);
    WD_TEST_BOOL(wdPathUtils::FindPreviousSeparator(szPath, szPath + 7) == szPath + 4);
    WD_TEST_BOOL(wdPathUtils::FindPreviousSeparator(szPath, szPath + 4) == nullptr);
    WD_TEST_BOOL(wdPathUtils::FindPreviousSeparator(szPath, szPath) == nullptr);
    WD_TEST_BOOL(wdPathUtils::FindPreviousSeparator(nullptr, nullptr) == nullptr);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "HasAnyExtension")
  {
    WD_TEST_BOOL(wdPathUtils::HasAnyExtension("This/Is\\My//Path.dot\\file.extension"));
    WD_TEST_BOOL(!wdPathUtils::HasAnyExtension("This/Is\\My//Path.dot\\file_no_extension"));
    WD_TEST_BOOL(!wdPathUtils::HasAnyExtension(""));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "HasExtension")
  {
    WD_TEST_BOOL(wdPathUtils::HasExtension("This/Is\\My//Path.dot\\file.extension", ".Extension"));
    WD_TEST_BOOL(wdPathUtils::HasExtension("This/Is\\My//Path.dot\\file.ext", "EXT"));
    WD_TEST_BOOL(!wdPathUtils::HasExtension("This/Is\\My//Path.dot\\file.ext", "NEXT"));
    WD_TEST_BOOL(!wdPathUtils::HasExtension("This/Is\\My//Path.dot\\file.extension", ".Ext"));
    WD_TEST_BOOL(!wdPathUtils::HasExtension("This/Is\\My//Path.dot\\file.extension", "sion"));
    WD_TEST_BOOL(!wdPathUtils::HasExtension("", "ext"));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetFileExtension")
  {
    WD_TEST_BOOL(wdPathUtils::GetFileExtension("This/Is\\My//Path.dot\\file.extension") == "extension");
    WD_TEST_BOOL(wdPathUtils::GetFileExtension("This/Is\\My//Path.dot\\file") == "");
    WD_TEST_BOOL(wdPathUtils::GetFileExtension("") == "");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetFileNameAndExtension")
  {
    WD_TEST_BOOL(wdPathUtils::GetFileNameAndExtension("This/Is\\My//Path.dot\\file.extension") == "file.extension");
    WD_TEST_BOOL(wdPathUtils::GetFileNameAndExtension("This/Is\\My//Path.dot\\.extension") == ".extension");
    WD_TEST_BOOL(wdPathUtils::GetFileNameAndExtension("This/Is\\My//Path.dot\\file") == "file");
    WD_TEST_BOOL(wdPathUtils::GetFileNameAndExtension("\\file") == "file");
    WD_TEST_BOOL(wdPathUtils::GetFileNameAndExtension("") == "");
    WD_TEST_BOOL(wdPathUtils::GetFileNameAndExtension("/") == "");
    WD_TEST_BOOL(wdPathUtils::GetFileNameAndExtension("This/Is\\My//Path.dot\\") == "");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetFileName")
  {
    WD_TEST_BOOL(wdPathUtils::GetFileName("This/Is\\My//Path.dot\\file.extension") == "file");
    WD_TEST_BOOL(wdPathUtils::GetFileName("This/Is\\My//Path.dot\\file") == "file");
    WD_TEST_BOOL(wdPathUtils::GetFileName("\\file") == "file");
    WD_TEST_BOOL(wdPathUtils::GetFileName("") == "");
    WD_TEST_BOOL(wdPathUtils::GetFileName("/") == "");
    WD_TEST_BOOL(wdPathUtils::GetFileName("This/Is\\My//Path.dot\\") == "");

    // so far we treat file and folders whose names start with a '.' as extensions
    WD_TEST_BOOL(wdPathUtils::GetFileName("This/Is\\My//Path.dot\\.stupidfile") == "");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetFileDirectory")
  {
    WD_TEST_BOOL(wdPathUtils::GetFileDirectory("This/Is\\My//Path.dot\\file.extension") == "This/Is\\My//Path.dot\\");
    WD_TEST_BOOL(wdPathUtils::GetFileDirectory("This/Is\\My//Path.dot\\.extension") == "This/Is\\My//Path.dot\\");
    WD_TEST_BOOL(wdPathUtils::GetFileDirectory("This/Is\\My//Path.dot\\file") == "This/Is\\My//Path.dot\\");
    WD_TEST_BOOL(wdPathUtils::GetFileDirectory("\\file") == "\\");
    WD_TEST_BOOL(wdPathUtils::GetFileDirectory("") == "");
    WD_TEST_BOOL(wdPathUtils::GetFileDirectory("/") == "/");
    WD_TEST_BOOL(wdPathUtils::GetFileDirectory("This/Is\\My//Path.dot\\") == "This/Is\\My//Path.dot\\");
    WD_TEST_BOOL(wdPathUtils::GetFileDirectory("This") == "");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsAbsolutePath")
  {
#if WD_ENABLED(WD_PLATFORM_WINDOWS)
    WD_TEST_BOOL(wdPathUtils::IsAbsolutePath("C:\\temp.stuff"));
    WD_TEST_BOOL(wdPathUtils::IsAbsolutePath("C:/temp.stuff"));
    WD_TEST_BOOL(wdPathUtils::IsAbsolutePath("\\\\myserver\\temp.stuff"));
    WD_TEST_BOOL(!wdPathUtils::IsAbsolutePath("\\myserver\\temp.stuff"));
    WD_TEST_BOOL(!wdPathUtils::IsAbsolutePath("temp.stuff"));
    WD_TEST_BOOL(!wdPathUtils::IsAbsolutePath("/temp.stuff"));
    WD_TEST_BOOL(!wdPathUtils::IsAbsolutePath("\\temp.stuff"));
    WD_TEST_BOOL(!wdPathUtils::IsAbsolutePath("..\\temp.stuff"));
    WD_TEST_BOOL(!wdPathUtils::IsAbsolutePath(".\\temp.stuff"));
#elif WD_ENABLED(WD_PLATFORM_OSX) || WD_ENABLED(WD_PLATFORM_LINUX) || WD_ENABLED(WD_PLATFORM_ANDROID)
    WD_TEST_BOOL(wdPathUtils::IsAbsolutePath("/usr/local/.stuff"));
    WD_TEST_BOOL(wdPathUtils::IsAbsolutePath("/file.test"));
    WD_TEST_BOOL(!wdPathUtils::IsAbsolutePath("./file.stuff"));
    WD_TEST_BOOL(!wdPathUtils::IsAbsolutePath("file.stuff"));
#else
#  error "Unknown platform."
#endif
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetRootedPathParts")
  {
    wdStringView root, relPath;
    wdPathUtils::GetRootedPathParts(":MyRoot\\folder\\file.txt", root, relPath);
    WD_TEST_BOOL(wdPathUtils::GetRootedPathRootName(":MyRoot\\folder\\file.txt") == root);
    WD_TEST_BOOL(root == "MyRoot");
    WD_TEST_BOOL(relPath == "folder\\file.txt");
    wdPathUtils::GetRootedPathParts("folder\\file2.txt", root, relPath);
    WD_TEST_BOOL(root.IsEmpty());
    WD_TEST_BOOL(relPath == "folder\\file2.txt");
  }
}
