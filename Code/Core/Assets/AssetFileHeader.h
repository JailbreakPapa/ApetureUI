#pragma once

#include <Core/CoreDLL.h>
#include <Foundation/IO/Stream.h>
#include <Foundation/Strings/HashedString.h>

/// \brief Simple class to handle asset file headers (the very first bytes in all transformed asset files)
class WD_CORE_DLL wdAssetFileHeader
{
public:
  wdAssetFileHeader();

  /// \brief Reads the hash from file. If the file is outdated, the hash is set to 0xFFFFFFFFFFFFFFFF.
  wdResult Read(wdStreamReader& inout_stream);

  /// \brief Writes the asset hash to file (plus a little version info)
  wdResult Write(wdStreamWriter& inout_stream) const;

  /// \brief Checks whether the stored file contains the same hash.
  bool IsFileUpToDate(wdUInt64 uiExpectedHash, wdUInt16 uiVersion) const { return (m_uiHash == uiExpectedHash && m_uiVersion == uiVersion); }

  /// \brief Returns the asset file hash
  wdUInt64 GetFileHash() const { return m_uiHash; }

  /// \brief Sets the asset file hash
  void SetFileHashAndVersion(wdUInt64 uiHash, wdUInt16 v)
  {
    m_uiHash = uiHash;
    m_uiVersion = v;
  }

  /// \brief Returns the asset type version
  wdUInt16 GetFileVersion() const { return m_uiVersion; }

  /// \brief Returns the generator which was used to produce the asset file
  const wdHashedString& GetGenerator() { return m_sGenerator; }

  /// \brief Allows to set the generator string
  void SetGenerator(wdStringView sGenerator) { m_sGenerator.Assign(sGenerator); }

private:
  wdUInt64 m_uiHash;
  wdUInt16 m_uiVersion;
  wdHashedString m_sGenerator;
};
