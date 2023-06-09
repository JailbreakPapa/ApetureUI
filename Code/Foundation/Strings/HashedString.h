#pragma once

#include <Foundation/Algorithm/HashingUtils.h>
#include <Foundation/Containers/Map.h>
#include <Foundation/Strings/String.h>
#include <Foundation/Threading/AtomicInteger.h>

class wdTempHashedString;

/// \brief This class is optimized to take nearly no memory (sizeof(void*)) and to allow very fast checks whether two strings are identical.
///
/// Internally only a reference to the string data is stored. The data itself is stored in a central location, where no duplicates are
/// possible. Thus two identical strings will result in identical wdHashedString objects, which makes equality comparisons very easy
/// (it's a pointer comparison).\n
/// Copying wdHashedString objects around and assigning between them is very fast as well.\n
/// \n
/// Assigning from some other string type is rather slow though, as it requires thread synchronization.\n
/// You can also get access to the actual string data via GetString().\n
/// \n
/// You should use wdHashedString whenever the size of the encapsulating object is important and when changes to the string itself
/// are rare, but checks for equality might be frequent (e.g. in a system where objects are identified via their name).\n
/// At runtime when you need to compare wdHashedString objects with some temporary string object, used wdTempHashedString,
/// as it will only use the string's hash value for comparison, but will not store the actual string anywhere.
class WD_FOUNDATION_DLL wdHashedString
{
public:
  struct HashedData
  {
#if WD_ENABLED(WD_HASHED_STRING_REF_COUNTING)
    wdAtomicInteger32 m_iRefCount;
#endif
    wdString m_sString;
  };

  // Do NOT use a hash-table! The map does not relocate memory when it resizes, which is a vital aspect for the hashed strings to work.
  typedef wdMap<wdUInt64, HashedData, wdCompareHelper<wdUInt64>, wdStaticAllocatorWrapper> StringStorage;
  typedef StringStorage::Iterator HashedType;

#if WD_ENABLED(WD_HASHED_STRING_REF_COUNTING)
  /// \brief This will remove all hashed strings from the central storage, that are not referenced anymore.
  ///
  /// All hashed string values are stored in a central location and wdHashedString just references them. Those strings are then
  /// reference counted. Once some string is not referenced anymore, its ref count reaches zero, but it will not be removed from
  /// the storage, as it might be reused later again.
  /// This function will clean up all unused strings. It should typically not be necessary to call this function at all, unless lots of
  /// strings get stored in wdHashedString that are not really used throughout the applications life time.
  ///
  /// Returns the number of unused strings that were removed.
  static wdUInt32 ClearUnusedStrings();
#endif

  WD_DECLARE_MEM_RELOCATABLE_TYPE();

  /// \brief Initializes this string to the empty string.
  wdHashedString(); // [tested]

  /// \brief Copies the given wdHashedString.
  wdHashedString(const wdHashedString& rhs); // [tested]

  /// \brief Moves the given wdHashedString.
  wdHashedString(wdHashedString&& rhs); // [tested]

  /// \brief Releases the reference to the internal data. Does NOT deallocate any data, even if this held the last reference to some string.
  ~wdHashedString();

  /// \brief Copies the given wdHashedString.
  void operator=(const wdHashedString& rhs); // [tested]

  /// \brief Moves the given wdHashedString.
  void operator=(wdHashedString&& rhs); // [tested]

  /// \brief Assigning a new string from a string constant is a slow operation, but the hash computation can happen at compile time.
  ///
  /// If you need to create an object to compare wdHashedString objects against, prefer to use wdTempHashedString. It will only compute
  /// the strings hash value, but does not require any thread synchronization.
  template <size_t N>
  void Assign(const char (&string)[N]); // [tested]

  template <size_t N>
  void Assign(char (&string)[N]) = delete;

  /// \brief Assigning a new string from a non-hashed string is a very slow operation, this should be used rarely.
  ///
  /// If you need to create an object to compare wdHashedString objects against, prefer to use wdTempHashedString. It will only compute
  /// the strings hash value, but does not require any thread synchronization.
  void Assign(wdStringView sString); // [tested]

  /// \brief Comparing whether two hashed strings are identical is just a pointer comparison. This operation is what wdHashedString is
  /// optimized for.
  ///
  /// \note Comparing between wdHashedString objects is always error-free, so even if two string had the same hash value, although they are
  /// different, this comparison function will not report they are the same.
  bool operator==(const wdHashedString& rhs) const; // [tested]

  /// \brief \see operator==
  bool operator!=(const wdHashedString& rhs) const; // [tested]

  /// \brief Compares this string object to an wdTempHashedString object. This should be used whenever some object needs to be found
  /// and the string to compare against is not yet an wdHashedString object.
  bool operator==(const wdTempHashedString& rhs) const; // [tested]

  /// \brief \see operator==
  bool operator!=(const wdTempHashedString& rhs) const; // [tested]

  /// \brief This operator allows sorting objects by hash value, not by alphabetical order.
  bool operator<(const wdHashedString& rhs) const; // [tested]

  /// \brief This operator allows sorting objects by hash value, not by alphabetical order.
  bool operator<(const wdTempHashedString& rhs) const; // [tested]

  /// \brief Gives access to the actual string data, so you can do all the typical (read-only) string operations on it.
  const wdString& GetString() const; // [tested]

  /// \brief Gives access to the actual string data, so you can do all the typical (read-only) string operations on it.
  const char* GetData() const;

  /// \brief Returns the hash of the stored string.
  wdUInt64 GetHash() const; // [tested]

  /// \brief Returns whether the string is empty.
  bool IsEmpty() const;

  /// \brief Resets the string to the empty string.
  void Clear();

  /// \brief Returns a string view to this string's data.
  WD_ALWAYS_INLINE operator wdStringView() const { return GetString().GetView(); }

  /// \brief Returns a string view to this string's data.
  WD_ALWAYS_INLINE wdStringView GetView() const { return GetString().GetView(); }

  /// \brief Returns a pointer to the internal Utf8 string.
  WD_ALWAYS_INLINE operator const char*() const { return GetData(); }

private:
  static void InitHashedString();
  static HashedType AddHashedString(wdStringView sString, wdUInt64 uiHash);

  HashedType m_Data;
};

/// \brief Helper function to create an wdHashedString. This can be used to initialize static hashed string variables.
template <size_t N>
wdHashedString wdMakeHashedString(const char (&string)[N]);


/// \brief A class to use together with wdHashedString for quick comparisons with temporary strings that need not be stored further.
///
/// Whenever you have objects that use wdHashedString members and you need to compare against them with some temporary string,
/// prefer to use wdTempHashedString instead of wdHashedString, as the latter requires thread synchronization to actually set up the
/// object.
class WD_FOUNDATION_DLL wdTempHashedString
{
  friend class wdHashedString;

public:
  wdTempHashedString(); // [tested]

  /// \brief Creates an wdTempHashedString object from the given string constant. The hash can be computed at compile time.
  template <size_t N>
  wdTempHashedString(const char (&string)[N]); // [tested]

  template <size_t N>
  wdTempHashedString(char (&string)[N]) = delete;

  /// \brief Creates an wdTempHashedString object from the given string. Computes the hash of the given string during runtime, which might
  /// be slow.
  wdTempHashedString(wdStringView sString); // [tested]

  /// \brief Copies the hash from rhs.
  wdTempHashedString(const wdTempHashedString& rhs); // [tested]

  /// \brief Copies the hash from the wdHashedString.
  wdTempHashedString(const wdHashedString& rhs); // [tested]

  explicit wdTempHashedString(wdUInt32 uiHash) = delete;

  /// \brief Copies the hash from the 64 bit integer.
  explicit wdTempHashedString(wdUInt64 uiHash);

  /// \brief The hash of the given string can be computed at compile time.
  template <size_t N>
  void operator=(const char (&string)[N]); // [tested]

  /// \brief Computes and stores the hash of the given string during runtime, which might be slow.
  void operator=(wdStringView sString); // [tested]

  /// \brief Copies the hash from rhs.
  void operator=(const wdTempHashedString& rhs); // [tested]

  /// \brief Copies the hash from the wdHashedString.
  void operator=(const wdHashedString& rhs); // [tested]

  /// \brief Compares the two objects by their hash value. Might report incorrect equality, if two strings have the same hash value.
  bool operator==(const wdTempHashedString& rhs) const; // [tested]

  /// \brief \see operator==
  bool operator!=(const wdTempHashedString& rhs) const; // [tested]

  /// \brief This operator allows soring objects by hash value, not by alphabetical order.
  bool operator<(const wdTempHashedString& rhs) const; // [tested]

  /// \brief Checks whether the wdTempHashedString represents the empty string.
  bool IsEmpty() const; // [tested]

  /// \brief Resets the string to the empty string.
  void Clear(); // [tested]

  /// \brief Returns the hash of the stored string.
  wdUInt64 GetHash() const; // [tested]

private:
  wdUInt64 m_uiHash;
};

// For wdFormatString
WD_FOUNDATION_DLL wdStringView BuildString(char* szTmp, wdUInt32 uiLength, const wdHashedString& sArg);

#include <Foundation/Strings/Implementation/HashedString_inl.h>
