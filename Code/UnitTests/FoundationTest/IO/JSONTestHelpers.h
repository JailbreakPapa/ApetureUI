#pragma once

#include <Foundation/IO/OSFile.h>

class StreamComparer : public wdStreamWriter
{
public:
  StreamComparer(const char* szExpectedData, bool bOnlyWriteResult = false)
  {
    m_bOnlyWriteResult = bOnlyWriteResult;
    m_szExpectedData = szExpectedData;
  }

  ~StreamComparer()
  {
    if (m_bOnlyWriteResult)
    {
      wdOSFile f;
      f.Open("C:\\Code\\JSON.txt", wdFileOpenMode::Write).IgnoreResult();
      f.Write(m_sResult.GetData(), m_sResult.GetElementCount()).IgnoreResult();
      f.Close();
    }
    else
      WD_TEST_BOOL(*m_szExpectedData == '\0');
  }

  wdResult WriteBytes(const void* pWriteBuffer, wdUInt64 uiBytesToWrite)
  {
    if (m_bOnlyWriteResult)
      m_sResult.Append((const char*)pWriteBuffer);
    else
    {
      const char* szWritten = (const char*)pWriteBuffer;

      WD_TEST_BOOL(wdMemoryUtils::IsEqual(szWritten, m_szExpectedData, (wdUInt32)uiBytesToWrite));
      m_szExpectedData += uiBytesToWrite;
    }

    return WD_SUCCESS;
  }

private:
  bool m_bOnlyWriteResult;
  wdStringBuilder m_sResult;
  const char* m_szExpectedData;
};


class StringStream : public wdStreamReader
{
public:
  StringStream(const void* pData)
  {
    m_pData = pData;
    m_uiLength = wdStringUtils::GetStringElementCount((const char*)pData);
  }

  virtual wdUInt64 ReadBytes(void* pReadBuffer, wdUInt64 uiBytesToRead)
  {
    uiBytesToRead = wdMath::Min(uiBytesToRead, m_uiLength);
    m_uiLength -= uiBytesToRead;

    if (uiBytesToRead > 0)
    {
      wdMemoryUtils::Copy((wdUInt8*)pReadBuffer, (wdUInt8*)m_pData, (size_t)uiBytesToRead);
      m_pData = wdMemoryUtils::AddByteOffset(m_pData, (ptrdiff_t)uiBytesToRead);
    }

    return uiBytesToRead;
  }

private:
  const void* m_pData;
  wdUInt64 m_uiLength;
};
