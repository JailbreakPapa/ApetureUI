#include <FoundationTest/FoundationTestPCH.h>

#if WD_ENABLED(WD_SUPPORTS_DIRECTORY_WATCHER)

#  include <Foundation/IO/DirectoryWatcher.h>
#  include <Foundation/IO/OSFile.h>
#  include <Foundation/Threading/ThreadUtils.h>

namespace DirectoryWatcherTestHelpers
{

  struct ExpectedEvent
  {
    ~ExpectedEvent(){}; // To make it non-pod

    const char* path;
    wdDirectoryWatcherAction action;
    wdDirectoryWatcherType type;

    bool operator==(const ExpectedEvent& other) const
    {
      return wdStringView(path) == wdStringView(other.path) && action == other.action && type == other.type;
    }
  };

  struct ExpectedEventStorage
  {
    wdString path;
    wdDirectoryWatcherAction action;
    wdDirectoryWatcherType type;
  };

  void TickWatcher(wdDirectoryWatcher& ref_watcher)
  {
    ref_watcher.EnumerateChanges([&](const char* szPath, wdDirectoryWatcherAction action, wdDirectoryWatcherType type) {
    },
      wdTime::Milliseconds(100));
  }
} // namespace DirectoryWatcherTestHelpers

WD_CREATE_SIMPLE_TEST(IO, DirectoryWatcher)
{
  using namespace DirectoryWatcherTestHelpers;

  wdStringBuilder tmp, tmp2;
  wdStringBuilder sTestRootPath = wdTestFramework::GetInstance()->GetAbsOutputPath();
  sTestRootPath.AppendPath("DirectoryWatcher/");

  auto CheckExpectedEvents = [&](wdDirectoryWatcher& ref_watcher, wdArrayPtr<ExpectedEvent> events) {
    wdDynamicArray<ExpectedEventStorage> firedEvents;
    wdUInt32 i = 0;
    ref_watcher.EnumerateChanges([&](const char* szPath, wdDirectoryWatcherAction action, wdDirectoryWatcherType type) {
      tmp = szPath;
      tmp.Shrink(sTestRootPath.GetCharacterCount(), 0);
      firedEvents.PushBack({tmp, action, type});
      if (i < events.GetCount())
      {
        WD_TEST_BOOL_MSG(tmp == events[i].path, "Expected event at index %d path mismatch: '%s' vs '%s'", i, tmp.GetData(), events[i].path);
        WD_TEST_BOOL_MSG(action == events[i].action, "Expected event at index %d action", i);
        WD_TEST_BOOL_MSG(type == events[i].type, "Expected event at index %d type mismatch", i);
      }
      i++;
    },
      wdTime::Milliseconds(100));
    WD_TEST_BOOL_MSG(firedEvents.GetCount() == events.GetCount(), "Directory watcher did not fire expected amount of events");
  };

  auto CheckExpectedEventsUnordered = [&](wdDirectoryWatcher& ref_watcher, wdArrayPtr<ExpectedEvent> events) {
    wdDynamicArray<ExpectedEventStorage> firedEvents;
    wdUInt32 i = 0;
    wdDynamicArray<bool> eventFired;
    eventFired.SetCount(events.GetCount());
    ref_watcher.EnumerateChanges([&](const char* szPath, wdDirectoryWatcherAction action, wdDirectoryWatcherType type) {
      tmp = szPath;
      tmp.Shrink(sTestRootPath.GetCharacterCount(), 0);
      firedEvents.PushBack({tmp, action, type});
      auto index = events.IndexOf({tmp, action, type});
      WD_TEST_BOOL_MSG(index != wdInvalidIndex, "Event %d (%s, %d, %d) not found in expected events list", i, tmp.GetData(), (int)action, (int)type);
      if (index != wdInvalidIndex)
      {
        eventFired[index] = true;
      }
      i++;
    },
      wdTime::Milliseconds(100));
    for (auto& fired : eventFired)
    {
      WD_TEST_BOOL(fired);
    }
    WD_TEST_BOOL_MSG(firedEvents.GetCount() == events.GetCount(), "Directory watcher did not fire expected amount of events");
  };

  auto CheckExpectedEventsMultiple = [&](wdArrayPtr<wdDirectoryWatcher*> watchers, wdArrayPtr<ExpectedEvent> events) {
    wdDynamicArray<ExpectedEventStorage> firedEvents;
    wdUInt32 i = 0;
    wdDirectoryWatcher::EnumerateChanges(
      watchers, [&](const char* szPath, wdDirectoryWatcherAction action, wdDirectoryWatcherType type) {
        tmp = szPath;
        tmp.Shrink(sTestRootPath.GetCharacterCount(), 0);
        firedEvents.PushBack({tmp, action, type});
        if (i < events.GetCount())
        {
          WD_TEST_BOOL_MSG(tmp == events[i].path, "Expected event at index %d path mismatch: '%s' vs '%s'", i, tmp.GetData(), events[i].path);
          WD_TEST_BOOL_MSG(action == events[i].action, "Expected event at index %d action", i);
          WD_TEST_BOOL_MSG(type == events[i].type, "Expected event at index %d type mismatch", i);
        }
        i++;
      },
      wdTime::Milliseconds(100));
    WD_TEST_BOOL_MSG(firedEvents.GetCount() == events.GetCount(), "Directory watcher did not fire expected amount of events");
  };

  auto CreateFile = [&](const char* szRelPath) {
    tmp = sTestRootPath;
    tmp.AppendPath(szRelPath);

    wdOSFile file;
    WD_TEST_BOOL(file.Open(tmp, wdFileOpenMode::Write).Succeeded());
    WD_TEST_BOOL(file.Write("Hello World", 11).Succeeded());
  };

  auto ModifyFile = [&](const char* szRelPath) {
    tmp = sTestRootPath;
    tmp.AppendPath(szRelPath);

    wdOSFile file;
    WD_TEST_BOOL(file.Open(tmp, wdFileOpenMode::Append).Succeeded());
    WD_TEST_BOOL(file.Write("Hello World", 11).Succeeded());
  };

  auto DeleteFile = [&](const char* szRelPath) {
    tmp = sTestRootPath;
    tmp.AppendPath(szRelPath);
    WD_TEST_BOOL(wdOSFile::DeleteFile(tmp).Succeeded());
  };

  auto CreateDirectory = [&](const char* szRelPath) {
    tmp = sTestRootPath;
    tmp.AppendPath(szRelPath);
    WD_TEST_BOOL(wdOSFile::CreateDirectoryStructure(tmp).Succeeded());
  };

  auto Rename = [&](const char* szFrom, const char* szTo) {
    tmp = sTestRootPath;
    tmp.AppendPath(szFrom);

    tmp2 = sTestRootPath;
    tmp2.AppendPath(szTo);

    WD_TEST_BOOL(wdOSFile::MoveFileOrDirectory(tmp, tmp2).Succeeded());
  };

  auto DeleteDirectory = [&](const char* szRelPath, bool bTest = true) {
    tmp = sTestRootPath;
    tmp.AppendPath(szRelPath);
    tmp.MakeCleanPath();

    if (bTest)
    {
      WD_TEST_BOOL(wdOSFile::DeleteFolder(tmp).Succeeded());
    }
    else
    {
      wdOSFile::DeleteFolder(tmp).IgnoreResult();
    }
  };

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Simple Create File")
  {
    wdOSFile::DeleteFolder(sTestRootPath).IgnoreResult();
    WD_TEST_BOOL(wdOSFile::CreateDirectoryStructure(sTestRootPath).Succeeded());

    wdDirectoryWatcher watcher;
    WD_TEST_BOOL(watcher.OpenDirectory(sTestRootPath, wdDirectoryWatcher::Watch::Creates | wdDirectoryWatcher::Watch::Writes).Succeeded());

    CreateFile("test.file");

    ExpectedEvent expectedEvents[] = {
      {"test.file", wdDirectoryWatcherAction::Added, wdDirectoryWatcherType::File},
    };
    CheckExpectedEvents(watcher, expectedEvents);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Simple delete file")
  {
    wdDirectoryWatcher watcher;
    WD_TEST_BOOL(watcher.OpenDirectory(sTestRootPath, wdDirectoryWatcher::Watch::Deletes).Succeeded());

    DeleteFile("test.file");

    ExpectedEvent expectedEvents[] = {
      {"test.file", wdDirectoryWatcherAction::Removed, wdDirectoryWatcherType::File},
    };
    CheckExpectedEvents(watcher, expectedEvents);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Simple modify file")
  {
    wdDirectoryWatcher watcher;
    WD_TEST_BOOL(watcher.OpenDirectory(sTestRootPath, wdDirectoryWatcher::Watch::Writes).Succeeded());

    CreateFile("test.file");

    TickWatcher(watcher);

    ModifyFile("test.file");

    ExpectedEvent expectedEvents[] = {
      {"test.file", wdDirectoryWatcherAction::Modified, wdDirectoryWatcherType::File},
    };
    CheckExpectedEvents(watcher, expectedEvents);

    DeleteFile("test.file");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Simple rename file")
  {
    wdDirectoryWatcher watcher;
    WD_TEST_BOOL(watcher.OpenDirectory(sTestRootPath, wdDirectoryWatcher::Watch::Renames).Succeeded());

    CreateFile("test.file");
    Rename("test.file", "supertest.file");

    ExpectedEvent expectedEvents[] = {
      {"test.file", wdDirectoryWatcherAction::RenamedOldName, wdDirectoryWatcherType::File},
      {"supertest.file", wdDirectoryWatcherAction::RenamedNewName, wdDirectoryWatcherType::File},
    };
    CheckExpectedEvents(watcher, expectedEvents);

    DeleteFile("supertest.file");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Simple create directory")
  {
    wdOSFile::DeleteFolder(sTestRootPath).IgnoreResult();
    WD_TEST_BOOL(wdOSFile::CreateDirectoryStructure(sTestRootPath).Succeeded());

    wdDirectoryWatcher watcher;
    WD_TEST_BOOL(watcher.OpenDirectory(sTestRootPath, wdDirectoryWatcher::Watch::Creates).Succeeded());

    CreateDirectory("testDir");

    ExpectedEvent expectedEvents[] = {
      {"testDir", wdDirectoryWatcherAction::Added, wdDirectoryWatcherType::Directory},
    };
    CheckExpectedEvents(watcher, expectedEvents);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Simple delete directory")
  {
    wdDirectoryWatcher watcher;
    WD_TEST_BOOL(watcher.OpenDirectory(sTestRootPath, wdDirectoryWatcher::Watch::Deletes).Succeeded());

    DeleteDirectory("testDir");

    ExpectedEvent expectedEvents[] = {
      {"testDir", wdDirectoryWatcherAction::Removed, wdDirectoryWatcherType::Directory},
    };
    CheckExpectedEvents(watcher, expectedEvents);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Simple rename directory")
  {
    wdDirectoryWatcher watcher;
    WD_TEST_BOOL(watcher.OpenDirectory(sTestRootPath, wdDirectoryWatcher::Watch::Renames).Succeeded());

    CreateDirectory("testDir");
    Rename("testDir", "supertestDir");

    ExpectedEvent expectedEvents[] = {
      {"testDir", wdDirectoryWatcherAction::RenamedOldName, wdDirectoryWatcherType::Directory},
      {"supertestDir", wdDirectoryWatcherAction::RenamedNewName, wdDirectoryWatcherType::Directory},
    };
    CheckExpectedEvents(watcher, expectedEvents);

    DeleteDirectory("supertestDir");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Subdirectory Create File")
  {
    tmp = sTestRootPath;
    tmp.AppendPath("subdir");
    WD_TEST_BOOL(wdOSFile::CreateDirectoryStructure(tmp).Succeeded());

    wdDirectoryWatcher watcher;
    WD_TEST_BOOL(watcher.OpenDirectory(sTestRootPath, wdDirectoryWatcher::Watch::Creates | wdDirectoryWatcher::Watch::Subdirectories).Succeeded());

    CreateFile("subdir/test.file");

    ExpectedEvent expectedEvents[] = {
      {"subdir/test.file", wdDirectoryWatcherAction::Added, wdDirectoryWatcherType::File},
    };
    CheckExpectedEvents(watcher, expectedEvents);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Subdirectory delete file")
  {

    wdDirectoryWatcher watcher;
    WD_TEST_BOOL(watcher.OpenDirectory(sTestRootPath, wdDirectoryWatcher::Watch::Deletes | wdDirectoryWatcher::Watch::Subdirectories).Succeeded());

    DeleteFile("subdir/test.file");

    ExpectedEvent expectedEvents[] = {
      {"subdir/test.file", wdDirectoryWatcherAction::Removed, wdDirectoryWatcherType::File},
    };
    CheckExpectedEvents(watcher, expectedEvents);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Subdirectory modify file")
  {
    wdDirectoryWatcher watcher;
    WD_TEST_BOOL(watcher.OpenDirectory(sTestRootPath, wdDirectoryWatcher::Watch::Writes | wdDirectoryWatcher::Watch::Subdirectories).Succeeded());

    CreateFile("subdir/test.file");

    TickWatcher(watcher);

    ModifyFile("subdir/test.file");

    ExpectedEvent expectedEvents[] = {
      {"subdir/test.file", wdDirectoryWatcherAction::Modified, wdDirectoryWatcherType::File},
    };
    CheckExpectedEvents(watcher, expectedEvents);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GUI Create Folder & file")
  {
    DeleteDirectory("sub", false);
    wdDirectoryWatcher watcher;
    WD_TEST_BOOL(watcher.OpenDirectory(
                          sTestRootPath,
                          wdDirectoryWatcher::Watch::Creates | wdDirectoryWatcher::Watch::Deletes |
                            wdDirectoryWatcher::Watch::Writes | wdDirectoryWatcher::Watch::Subdirectories)
                   .Succeeded());

    CreateDirectory("New Folder");

    ExpectedEvent expectedEvents1[] = {
      {"New Folder", wdDirectoryWatcherAction::Added, wdDirectoryWatcherType::Directory},
    };
    CheckExpectedEvents(watcher, expectedEvents1);

    Rename("New Folder", "sub");

    CreateFile("sub/bla");

    ExpectedEvent expectedEvents2[] = {
      {"sub/bla", wdDirectoryWatcherAction::Added, wdDirectoryWatcherType::File},
    };
    CheckExpectedEvents(watcher, expectedEvents2);

    ModifyFile("sub/bla");
    DeleteFile("sub/bla");

    ExpectedEvent expectedEvents3[] = {
      {"sub/bla", wdDirectoryWatcherAction::Modified, wdDirectoryWatcherType::File},
      {"sub/bla", wdDirectoryWatcherAction::Removed, wdDirectoryWatcherType::File},
    };
    CheckExpectedEvents(watcher, expectedEvents3);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GUI Create Folder & file fast")
  {
    DeleteDirectory("sub", false);
    wdDirectoryWatcher watcher;
    WD_TEST_BOOL(watcher.OpenDirectory(
                          sTestRootPath,
                          wdDirectoryWatcher::Watch::Creates | wdDirectoryWatcher::Watch::Deletes |
                            wdDirectoryWatcher::Watch::Writes | wdDirectoryWatcher::Watch::Subdirectories)
                   .Succeeded());

    CreateDirectory("New Folder");
    Rename("New Folder", "sub");

    ExpectedEvent expectedEvents1[] = {
      {"New Folder", wdDirectoryWatcherAction::Added, wdDirectoryWatcherType::Directory},
    };
    CheckExpectedEvents(watcher, expectedEvents1);

    CreateFile("sub/bla");

    ExpectedEvent expectedEvents2[] = {
      {"sub/bla", wdDirectoryWatcherAction::Added, wdDirectoryWatcherType::File},
    };
    CheckExpectedEvents(watcher, expectedEvents2);

    ModifyFile("sub/bla");
    DeleteFile("sub/bla");

    ExpectedEvent expectedEvents3[] = {
      {"sub/bla", wdDirectoryWatcherAction::Modified, wdDirectoryWatcherType::File},
      {"sub/bla", wdDirectoryWatcherAction::Removed, wdDirectoryWatcherType::File},
    };
    CheckExpectedEvents(watcher, expectedEvents3);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GUI Create Folder & file fast subdir")
  {
    DeleteDirectory("sub", false);

    wdDirectoryWatcher watcher;
    WD_TEST_BOOL(watcher.OpenDirectory(
                          sTestRootPath,
                          wdDirectoryWatcher::Watch::Creates | wdDirectoryWatcher::Watch::Deletes |
                            wdDirectoryWatcher::Watch::Writes | wdDirectoryWatcher::Watch::Subdirectories)
                   .Succeeded());

    CreateDirectory("New Folder/subsub");
    Rename("New Folder", "sub");

    TickWatcher(watcher);

    CreateFile("sub/subsub/bla");

    ExpectedEvent expectedEvents2[] = {
      {"sub/subsub/bla", wdDirectoryWatcherAction::Added, wdDirectoryWatcherType::File},
    };
    CheckExpectedEvents(watcher, expectedEvents2);

    ModifyFile("sub/subsub/bla");
    DeleteFile("sub/subsub/bla");

    ExpectedEvent expectedEvents3[] = {
      {"sub/subsub/bla", wdDirectoryWatcherAction::Modified, wdDirectoryWatcherType::File},
      {"sub/subsub/bla", wdDirectoryWatcherAction::Removed, wdDirectoryWatcherType::File},
    };
    CheckExpectedEvents(watcher, expectedEvents3);

    DeleteDirectory("sub");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GUI Delete Folder")
  {
    DeleteDirectory("sub2", false);
    DeleteDirectory("../sub2", false);

    wdDirectoryWatcher watcher;
    WD_TEST_BOOL(watcher.OpenDirectory(
                          sTestRootPath,
                          wdDirectoryWatcher::Watch::Creates | wdDirectoryWatcher::Watch::Deletes |
                            wdDirectoryWatcher::Watch::Writes | wdDirectoryWatcher::Watch::Subdirectories)
                   .Succeeded());

    CreateDirectory("sub2/subsub2");
    CreateFile("sub2/file1");
    CreateFile("sub2/subsub2/file2.txt");

    ExpectedEvent expectedEvents1[] = {
      {"sub2", wdDirectoryWatcherAction::Added, wdDirectoryWatcherType::Directory},
      {"sub2/file1", wdDirectoryWatcherAction::Added, wdDirectoryWatcherType::File},
      {"sub2/subsub2", wdDirectoryWatcherAction::Added, wdDirectoryWatcherType::Directory},
      {"sub2/subsub2/file2.txt", wdDirectoryWatcherAction::Added, wdDirectoryWatcherType::File},
    };
    CheckExpectedEventsUnordered(watcher, expectedEvents1);

    Rename("sub2", "../sub2");

    ExpectedEvent expectedEvents2[] = {
      {"sub2/subsub2/file2.txt", wdDirectoryWatcherAction::Removed, wdDirectoryWatcherType::File},
      {"sub2/subsub2", wdDirectoryWatcherAction::Removed, wdDirectoryWatcherType::Directory},
      {"sub2/file1", wdDirectoryWatcherAction::Removed, wdDirectoryWatcherType::File},
      {"sub2", wdDirectoryWatcherAction::Removed, wdDirectoryWatcherType::Directory},
    };
    // Issue here: After moving sub2 out of view, it remains in m_pathToWd
    CheckExpectedEvents(watcher, expectedEvents2);

    Rename("../sub2", "sub2");

    ExpectedEvent expectedEvents3[] = {
      {"sub2", wdDirectoryWatcherAction::Added, wdDirectoryWatcherType::Directory},
      {"sub2/file1", wdDirectoryWatcherAction::Added, wdDirectoryWatcherType::File},
      {"sub2/subsub2", wdDirectoryWatcherAction::Added, wdDirectoryWatcherType::Directory},
      {"sub2/subsub2/file2.txt", wdDirectoryWatcherAction::Added, wdDirectoryWatcherType::File},
    };
    CheckExpectedEventsUnordered(watcher, expectedEvents3);

    DeleteDirectory("sub2");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Create, Delete, Create")
  {
    wdDirectoryWatcher watcher;
    WD_TEST_BOOL(watcher.OpenDirectory(
                          sTestRootPath,
                          wdDirectoryWatcher::Watch::Creates | wdDirectoryWatcher::Watch::Deletes |
                            wdDirectoryWatcher::Watch::Writes | wdDirectoryWatcher::Watch::Subdirectories)
                   .Succeeded());

    CreateDirectory("sub2/subsub2");
    CreateFile("sub2/file1");
    CreateFile("sub2/subsub2/file2.txt");

    ExpectedEvent expectedEvents1[] = {
      {"sub2", wdDirectoryWatcherAction::Added, wdDirectoryWatcherType::Directory},
      {"sub2/file1", wdDirectoryWatcherAction::Added, wdDirectoryWatcherType::File},
      {"sub2/subsub2", wdDirectoryWatcherAction::Added, wdDirectoryWatcherType::Directory},
      {"sub2/subsub2/file2.txt", wdDirectoryWatcherAction::Added, wdDirectoryWatcherType::File},
    };
    CheckExpectedEventsUnordered(watcher, expectedEvents1);

    DeleteDirectory("sub2");

    ExpectedEvent expectedEvents2[] = {
      {"sub2/file1", wdDirectoryWatcherAction::Removed, wdDirectoryWatcherType::File},
      {"sub2/subsub2/file2.txt", wdDirectoryWatcherAction::Removed, wdDirectoryWatcherType::File},
      {"sub2/subsub2", wdDirectoryWatcherAction::Removed, wdDirectoryWatcherType::Directory},
      {"sub2", wdDirectoryWatcherAction::Removed, wdDirectoryWatcherType::Directory},
    };
    CheckExpectedEventsUnordered(watcher, expectedEvents2);

    CreateDirectory("sub2/subsub2");
    CreateFile("sub2/file1");
    CreateFile("sub2/subsub2/file2.txt");

    ExpectedEvent expectedEvents3[] = {
      {"sub2", wdDirectoryWatcherAction::Added, wdDirectoryWatcherType::Directory},
      {"sub2/file1", wdDirectoryWatcherAction::Added, wdDirectoryWatcherType::File},
      {"sub2/subsub2", wdDirectoryWatcherAction::Added, wdDirectoryWatcherType::Directory},
      {"sub2/subsub2/file2.txt", wdDirectoryWatcherAction::Added, wdDirectoryWatcherType::File},
    };
    CheckExpectedEventsUnordered(watcher, expectedEvents3);

    DeleteDirectory("sub2");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GUI Create file & delete")
  {
    DeleteDirectory("sub", false);
    wdDirectoryWatcher watcher;
    WD_TEST_BOOL(watcher.OpenDirectory(
                          sTestRootPath,
                          wdDirectoryWatcher::Watch::Creates | wdDirectoryWatcher::Watch::Deletes |
                            wdDirectoryWatcher::Watch::Writes | wdDirectoryWatcher::Watch::Renames)
                   .Succeeded());

    CreateFile("file2.txt");

    ExpectedEvent expectedEvents1[] = {
      {"file2.txt", wdDirectoryWatcherAction::Added, wdDirectoryWatcherType::File},
    };
    CheckExpectedEvents(watcher, expectedEvents1);

    Rename("file2.txt", "datei2.txt");

    ExpectedEvent expectedEvents2[] = {
      {"file2.txt", wdDirectoryWatcherAction::RenamedOldName, wdDirectoryWatcherType::File},
      {"datei2.txt", wdDirectoryWatcherAction::RenamedNewName, wdDirectoryWatcherType::File},
    };
    CheckExpectedEvents(watcher, expectedEvents2);

    DeleteFile("datei2.txt");

    ExpectedEvent expectedEvents3[] = {
      {"datei2.txt", wdDirectoryWatcherAction::Removed, wdDirectoryWatcherType::File},
    };
    CheckExpectedEvents(watcher, expectedEvents3);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Enumerate multiple")
  {
    DeleteDirectory("watch1", false);
    DeleteDirectory("watch2", false);
    DeleteDirectory("watch3", false);
    wdDirectoryWatcher watchers[3];

    wdDirectoryWatcher* pWatchers[] = {watchers + 0, watchers + 1, watchers + 2};

    CreateDirectory("watch1");
    CreateDirectory("watch2");
    CreateDirectory("watch3");

    wdStringBuilder watchPath;

    watchPath = sTestRootPath;
    watchPath.AppendPath("watch1");
    WD_TEST_BOOL(watchers[0].OpenDirectory(
                              watchPath,
                              wdDirectoryWatcher::Watch::Creates | wdDirectoryWatcher::Watch::Deletes |
                                wdDirectoryWatcher::Watch::Writes | wdDirectoryWatcher::Watch::Renames)
                   .Succeeded());

    watchPath = sTestRootPath;
    watchPath.AppendPath("watch2");
    WD_TEST_BOOL(watchers[1].OpenDirectory(
                              watchPath,
                              wdDirectoryWatcher::Watch::Creates | wdDirectoryWatcher::Watch::Deletes |
                                wdDirectoryWatcher::Watch::Writes | wdDirectoryWatcher::Watch::Renames)
                   .Succeeded());

    watchPath = sTestRootPath;
    watchPath.AppendPath("watch3");
    WD_TEST_BOOL(watchers[2].OpenDirectory(
                              watchPath,
                              wdDirectoryWatcher::Watch::Creates | wdDirectoryWatcher::Watch::Deletes |
                                wdDirectoryWatcher::Watch::Writes | wdDirectoryWatcher::Watch::Renames)
                   .Succeeded());

    CreateFile("watch1/file2.txt");

    ExpectedEvent expectedEvents1[] = {
      {"watch1/file2.txt", wdDirectoryWatcherAction::Added, wdDirectoryWatcherType::File},
    };
    CheckExpectedEventsMultiple(pWatchers, expectedEvents1);

    CreateFile("watch2/file2.txt");

    ExpectedEvent expectedEvents2[] = {
      {"watch2/file2.txt", wdDirectoryWatcherAction::Added, wdDirectoryWatcherType::File},
    };
    CheckExpectedEventsMultiple(pWatchers, expectedEvents2);

    CreateFile("watch3/file2.txt");

    ExpectedEvent expectedEvents3[] = {
      {"watch3/file2.txt", wdDirectoryWatcherAction::Added, wdDirectoryWatcherType::File},
    };
    CheckExpectedEventsMultiple(pWatchers, expectedEvents3);

    ModifyFile("watch1/file2.txt");
    ModifyFile("watch2/file2.txt");

    ExpectedEvent expectedEvents4[] = {
      {"watch1/file2.txt", wdDirectoryWatcherAction::Modified, wdDirectoryWatcherType::File},
      {"watch2/file2.txt", wdDirectoryWatcherAction::Modified, wdDirectoryWatcherType::File},
    };
    CheckExpectedEventsMultiple(pWatchers, expectedEvents4);

    DeleteFile("watch1/file2.txt");
    DeleteFile("watch2/file2.txt");
    DeleteFile("watch3/file2.txt");

    ExpectedEvent expectedEvents5[] = {
      {"watch1/file2.txt", wdDirectoryWatcherAction::Removed, wdDirectoryWatcherType::File},
      {"watch2/file2.txt", wdDirectoryWatcherAction::Removed, wdDirectoryWatcherType::File},
      {"watch3/file2.txt", wdDirectoryWatcherAction::Removed, wdDirectoryWatcherType::File},
    };
    CheckExpectedEventsMultiple(pWatchers, expectedEvents5);
  }

  wdOSFile::DeleteFolder(sTestRootPath).IgnoreResult();
}

#endif
