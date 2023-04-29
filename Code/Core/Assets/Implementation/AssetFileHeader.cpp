#include <Core/CorePCH.h>

#include <Core/Assets/AssetFileHeader.h>
#include <Foundation/IO/MemoryStream.h>

static const char* g_szAssetTag = "wdAsset";

wdAssetFileHeader::wdAssetFileHeader()
{
  // initialize to a 'valid' hash
  // this may get stored, unless someone sets the hash
  m_uiHash = 0;
  m_uiVersion = 0;
}

enum wdAssetFileHeaderVersion : wdUInt8
{
  Version1 = 1,
  Version2,
  Version3,

  VersionCount,
  VersionCurrent = VersionCount - 1
};

wdResult wdAssetFileHeader::Write(wdStreamWriter& inout_stream) const
{
  WD_ASSERT_DEBUG(m_uiHash != 0xFFFFFFFFFFFFFFFF, "Cannot write an invalid hash to file");

  // 8 Bytes for identification + version
  WD_SUCCEED_OR_RETURN(inout_stream.WriteBytes(g_szAssetTag, 7));

  const wdUInt8 uiVersion = wdAssetFileHeaderVersion::VersionCurrent;
  inout_stream << uiVersion;

  // 8 Bytes for the hash
  inout_stream << m_uiHash;
  // 2 for the type version
  inout_stream << m_uiVersion;

  inout_stream << m_sGenerator;
  return WD_SUCCESS;
}

wdResult wdAssetFileHeader::Read(wdStreamReader& inout_stream)
{
  // initialize to 'invalid'
  m_uiHash = 0xFFFFFFFFFFFFFFFF;
  m_uiVersion = 0;

  char szTag[8] = {0};
  if (inout_stream.ReadBytes(szTag, 7) < 7)
  {
    WD_REPORT_FAILURE("The stream does not contain a valid asset file header");
    return WD_FAILURE;
  }

  szTag[7] = '\0';

  // invalid asset file ... this is not going to end well
  WD_ASSERT_DEBUG(wdStringUtils::IsEqual(szTag, g_szAssetTag), "The stream does not contain a valid asset file header");

  if (!wdStringUtils::IsEqual(szTag, g_szAssetTag))
    return WD_FAILURE;

  wdUInt8 uiVersion = 0;
  inout_stream >> uiVersion;

  wdUInt64 uiHash = 0;
  inout_stream >> uiHash;

  // future version?
  WD_ASSERT_DEV(uiVersion <= wdAssetFileHeaderVersion::VersionCurrent, "Unknown asset header version {0}", uiVersion);

  if (uiVersion >= wdAssetFileHeaderVersion::Version2)
  {
    inout_stream >> m_uiVersion;
  }

  if (uiVersion >= wdAssetFileHeaderVersion::Version3)
  {
    inout_stream >> m_sGenerator;
  }

  // older version? set the hash to 'invalid'
  if (uiVersion != wdAssetFileHeaderVersion::VersionCurrent)
    return WD_FAILURE;

  m_uiHash = uiHash;

  return WD_SUCCESS;
}

WD_STATICLINK_FILE(Core, Core_Assets_Implementation_AssetFileHeader);
