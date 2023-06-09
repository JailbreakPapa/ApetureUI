#pragma once

#include <Foundation/Basics.h>

/// \brief Collection of helper methods when working with endianess "problems"
struct WD_FOUNDATION_DLL wdEndianHelper
{

  /// \brief Returns true if called on a big endian system, false otherwise.
  ///
  /// \note Note that usually the compile time decisions with the defines WD_PLATFORM_LITTLE_ENDIAN, WD_PLATFORM_BIG_ENDIAN is preferred.
  static inline bool IsBigEndian()
  {
    const int i = 1;
    return (*(char*)&i) == 0;
  }

  /// \brief Returns true if called on a little endian system, false otherwise.
  ///
  /// \note Note that usually the compile time decisions with the defines WD_PLATFORM_LITTLE_ENDIAN, WD_PLATFORM_BIG_ENDIAN is preferred.
  static inline bool IsLittleEndian() { return !IsBigEndian(); }

  /// \brief Switches endianess of the given array of words (16 bit values).
  static inline void SwitchWords(wdUInt16* pWords, wdUInt32 uiCount) // [tested]
  {
    for (wdUInt32 i = 0; i < uiCount; i++)
      pWords[i] = Switch(pWords[i]);
  }

  /// \brief Switches endianess of the given array of double words (32 bit values).
  static inline void SwitchDWords(wdUInt32* pDWords, wdUInt32 uiCount) // [tested]
  {
    for (wdUInt32 i = 0; i < uiCount; i++)
      pDWords[i] = Switch(pDWords[i]);
  }

  /// \brief Switches endianess of the given array of quad words (64 bit values).
  static inline void SwitchQWords(wdUInt64* pQWords, wdUInt32 uiCount) // [tested]
  {
    for (wdUInt32 i = 0; i < uiCount; i++)
      pQWords[i] = Switch(pQWords[i]);
  }

  /// \brief Returns a single switched word (16 bit value).
  static WD_ALWAYS_INLINE wdUInt16 Switch(wdUInt16 uiWord) // [tested]
  {
    return (((uiWord & 0xFF) << 8) | ((uiWord >> 8) & 0xFF));
  }

  /// \brief Returns a single switched double word (32 bit value).
  static WD_ALWAYS_INLINE wdUInt32 Switch(wdUInt32 uiDWord) // [tested]
  {
    return (((uiDWord & 0xFF) << 24) | (((uiDWord >> 8) & 0xFF) << 16) | (((uiDWord >> 16) & 0xFF) << 8) | ((uiDWord >> 24) & 0xFF));
  }

  /// \brief Returns a single switched quad word (64 bit value).
  static WD_ALWAYS_INLINE wdUInt64 Switch(wdUInt64 uiQWord) // [tested]
  {
    return (((uiQWord & 0xFF) << 56) | ((uiQWord & 0xFF00) << 40) | ((uiQWord & 0xFF0000) << 24) | ((uiQWord & 0xFF000000) << 8) |
            ((uiQWord & 0xFF00000000) >> 8) | ((uiQWord & 0xFF0000000000) >> 24) | ((uiQWord & 0xFF000000000000) >> 40) |
            ((uiQWord & 0xFF00000000000000) >> 56));
  }

  /// \brief Switches a value in place (template accepts pointers for 2, 4 & 8 byte data types)
  template <typename T>
  static void SwitchInPlace(T* pValue) // [tested]
  {
    WD_CHECK_AT_COMPILETIME_MSG(
      (sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8), "Switch in place only works for type equivalents of wdUInt16, wdUInt32, wdUInt64!");

    if (sizeof(T) == 2)
    {
      struct TAnd16BitUnion
      {
        union
        {
          wdUInt16 BitValue;
          T TValue;
        };
      };

      TAnd16BitUnion Temp;
      Temp.TValue = *pValue;
      Temp.BitValue = Switch(Temp.BitValue);

      *pValue = Temp.TValue;
    }
    else if (sizeof(T) == 4)
    {
      struct TAnd32BitUnion
      {
        union
        {
          wdUInt32 BitValue;
          T TValue;
        };
      };

      TAnd32BitUnion Temp;
      Temp.TValue = *pValue;
      Temp.BitValue = Switch(Temp.BitValue);

      *pValue = Temp.TValue;
    }
    else if (sizeof(T) == 8)
    {
      struct TAnd64BitUnion
      {
        union
        {
          wdUInt64 BitValue;
          T TValue;
        };
      };

      TAnd64BitUnion Temp;
      Temp.TValue = *pValue;
      Temp.BitValue = Switch(Temp.BitValue);

      *pValue = Temp.TValue;
    }
  }

#if WD_ENABLED(WD_PLATFORM_LITTLE_ENDIAN)

  static WD_ALWAYS_INLINE void LittleEndianToNative(wdUInt16* /*pWords*/, wdUInt32 /*uiCount*/) {}

  static WD_ALWAYS_INLINE void NativeToLittleEndian(wdUInt16* /*pWords*/, wdUInt32 /*uiCount*/) {}

  static WD_ALWAYS_INLINE void LittleEndianToNative(wdUInt32* /*pDWords*/, wdUInt32 /*uiCount*/) {}

  static WD_ALWAYS_INLINE void NativeToLittleEndian(wdUInt32* /*pDWords*/, wdUInt32 /*uiCount*/) {}

  static WD_ALWAYS_INLINE void LittleEndianToNative(wdUInt64* /*pQWords*/, wdUInt32 /*uiCount*/) {}

  static WD_ALWAYS_INLINE void NativeToLittleEndian(wdUInt64* /*pQWords*/, wdUInt32 /*uiCount*/) {}

  static WD_ALWAYS_INLINE void BigEndianToNative(wdUInt16* pWords, wdUInt32 uiCount) { SwitchWords(pWords, uiCount); }

  static WD_ALWAYS_INLINE void NativeToBigEndian(wdUInt16* pWords, wdUInt32 uiCount) { SwitchWords(pWords, uiCount); }

  static WD_ALWAYS_INLINE void BigEndianToNative(wdUInt32* pDWords, wdUInt32 uiCount) { SwitchDWords(pDWords, uiCount); }

  static WD_ALWAYS_INLINE void NativeToBigEndian(wdUInt32* pDWords, wdUInt32 uiCount) { SwitchDWords(pDWords, uiCount); }

  static WD_ALWAYS_INLINE void BigEndianToNative(wdUInt64* pQWords, wdUInt32 uiCount) { SwitchQWords(pQWords, uiCount); }

  static WD_ALWAYS_INLINE void NativeToBigEndian(wdUInt64* pQWords, wdUInt32 uiCount) { SwitchQWords(pQWords, uiCount); }

#elif WD_ENABLED(WD_PLATFORM_BIG_ENDIAN)

  static WD_ALWAYS_INLINE void LittleEndianToNative(wdUInt16* pWords, wdUInt32 uiCount) { SwitchWords(pWords, uiCount); }

  static WD_ALWAYS_INLINE void NativeToLittleEndian(wdUInt16* pWords, wdUInt32 uiCount) { SwitchWords(pWords, uiCount); }

  static WD_ALWAYS_INLINE void LittleEndianToNative(wdUInt32* pDWords, wdUInt32 uiCount) { SwitchDWords(pDWords, uiCount); }

  static WD_ALWAYS_INLINE void NativeToLittleEndian(wdUInt32* pDWords, wdUInt32 uiCount) { SwitchDWords(pDWords, uiCount); }

  static WD_ALWAYS_INLINE void LittleEndianToNative(wdUInt64* pQWords, wdUInt32 uiCount) { SwitchQWords(pQWords, uiCount); }

  static WD_ALWAYS_INLINE void NativeToLittleEndian(wdUInt64* pQWords, wdUInt32 uiCount) { SwitchQWords(pQWords, uiCount); }

  static WD_ALWAYS_INLINE void BigEndianToNative(wdUInt16* /*pWords*/, wdUInt32 /*uiCount*/) {}

  static WD_ALWAYS_INLINE void NativeToBigEndian(wdUInt16* /*pWords*/, wdUInt32 /*uiCount*/) {}

  static WD_ALWAYS_INLINE void BigEndianToNative(wdUInt32* /*pWords*/, wdUInt32 /*uiCount*/) {}

  static WD_ALWAYS_INLINE void NativeToBigEndian(wdUInt32* /*pWords*/, wdUInt32 /*uiCount*/) {}

  static WD_ALWAYS_INLINE void BigEndianToNative(wdUInt64* /*pWords*/, wdUInt32 /*uiCount*/) {}

  static WD_ALWAYS_INLINE void NativeToBigEndian(wdUInt64* /*pWords*/, wdUInt32 /*uiCount*/) {}

#endif


  /// \brief Switches a given struct according to the layout described in the szFormat parameter
  ///
  /// The format string may contain the characters:
  ///  - c, b for a member of 1 byte
  ///  - w, s for a member of 2 bytes (word, wdUInt16)
  ///  - d for a member of 4 bytes (DWORD, wdUInt32)
  ///  - q for a member of 8 bytes (DWORD, wdUInt64)
  static void SwitchStruct(void* pDataPointer, const char* szFormat);

  /// \brief Templated helper method for SwitchStruct
  template <typename T>
  static void SwitchStruct(T* pDataPointer, const char* szFormat) // [tested]
  {
    SwitchStruct(static_cast<void*>(pDataPointer), szFormat);
  }

  /// \brief Switches a given set of struct according to the layout described in the szFormat parameter
  ///
  /// The format string may contain the characters:
  ///  - c, b for a member of 1 byte
  ///  - w, s for a member of 2 bytes (word, wdUInt16)
  ///  - d for a member of 4 bytes (DWORD, wdUInt32)
  ///  - q for a member of 8 bytes (DWORD, wdUInt64)
  static void SwitchStructs(void* pDataPointer, const char* szFormat, wdUInt32 uiStride, wdUInt32 uiCount); // [tested]

  /// \brief Templated helper method for SwitchStructs
  template <typename T>
  static void SwitchStructs(T* pDataPointer, const char* szFormat, wdUInt32 uiCount) // [tested]
  {
    SwitchStructs(static_cast<void*>(pDataPointer), szFormat, sizeof(T), uiCount);
  }
};
