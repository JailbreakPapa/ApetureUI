#pragma once

#include <Foundation/Basics.h>

#if WD_ENABLED(WD_SUPPORTS_DIRECTORY_WATCHER)

#  include <Foundation/Basics.h>
#  include <Foundation/Strings/String.h>
#  include <Foundation/Types/Bitflags.h>
#  include <Foundation/Types/Delegate.h>

struct wdDirectoryWatcherImpl;

/// \brief Which action has been performed on a file.
enum class wdDirectoryWatcherAction
{
  None,           ///< Nothing happend
  Added,          ///< A file or directory was added
  Removed,        ///< A file or directory was removed
  Modified,       ///< A file was modified. Both Reads and Writes can 'modify' the timestamps of a file.
  RenamedOldName, ///< A file or directory was renamed. First the old name is provided.
  RenamedNewName, ///< A file or directory was renamed. The new name is provided second.
};

enum class wdDirectoryWatcherType
{
  File,
  Directory
};

/// \brief
///   Watches file actions in a directory. Changes need to be polled.
class WD_FOUNDATION_DLL wdDirectoryWatcher
{
public:
  /// \brief What to watch out for.
  struct Watch
  {
    typedef wdUInt8 StorageType;
    constexpr static wdUInt8 Default = 0;

    /// \brief Enum values
    enum Enum
    {
      Writes = WD_BIT(0),         ///< Watch for writes.
      Creates = WD_BIT(1),        ///< Watch for newly created files.
      Deletes = WD_BIT(2),        ///< Watch for deleted files.
      Renames = WD_BIT(3),        ///< Watch for renames.
      Subdirectories = WD_BIT(4), ///< Watch files in subdirectories recursively.
    };

    struct Bits
    {
      StorageType Writes : 1;
      StorageType Creates : 1;
      StorageType Deletes : 1;
      StorageType Renames : 1;
      StorageType Subdirectories : 1;
    };
  };

  wdDirectoryWatcher();
  ~wdDirectoryWatcher();

  /// \brief
  ///   Opens the directory at \p absolutePath for watching. \p whatToWatch controls what exactly should be watched.
  ///
  /// \note A instance of wdDirectoryWatcher can only watch one directory at a time.
  wdResult OpenDirectory(wdStringView sAbsolutePath, wdBitflags<Watch> whatToWatch);

  /// \brief
  ///   Closes the currently watched directory if any.
  void CloseDirectory();

  /// \brief
  ///   Returns the opened directory, will be empty if no directory was opened.
  const char* GetDirectory() const { return m_sDirectoryPath; }

  using EnumerateChangesFunction = wdDelegate<void(const char* filename, wdDirectoryWatcherAction action, wdDirectoryWatcherType type), 48>;

  /// \brief
  ///   Calls the callback \p func for each change since the last call. For each change the filename
  ///   and the action, which was performed on the file, is passed to \p func.
  ///   If waitUpToMilliseconds is greater than 0, blocks until either a change was observed or the timelimit is reached.
  ///
  /// \note There might be multiple changes on the same file reported.
  void EnumerateChanges(EnumerateChangesFunction func, wdTime waitUpTo = wdTime::Zero());

  /// \brief
  ///   Same as the other EnumerateChanges function, but enumerates multiple watchers.
  static void EnumerateChanges(wdArrayPtr<wdDirectoryWatcher*> watchers, EnumerateChangesFunction func, wdTime waitUpTo = wdTime::Zero());

private:
  wdString m_sDirectoryPath;
  wdDirectoryWatcherImpl* m_pImpl = nullptr;
};

WD_DECLARE_FLAGS_OPERATORS(wdDirectoryWatcher::Watch);

#endif
