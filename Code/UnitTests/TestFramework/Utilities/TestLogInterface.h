#pragma once
#include <Foundation/Logging/Log.h>
#include <TestFramework/TestFrameworkDLL.h>

/// \brief An wdLogInterface that expects and handles error messages during test runs. Can be
/// used to ensure that expected error messages are produced by the tested functionality.
/// Expected error messages are not passed on and do not cause tests to fail.
class WD_TEST_DLL wdTestLogInterface : public wdLogInterface
{
public:
  wdTestLogInterface() = default;
  ~wdTestLogInterface();
  virtual void HandleLogMessage(const wdLoggingEventData& le) override;

  /// \brief Add expected message. Will fail the test when the expected message is not
  /// encountered. Can take an optional count, if messages are expected multiple times
  void ExpectMessage(const char* szMsg, wdLogMsgType::Enum type = wdLogMsgType::All, wdInt32 iCount = 1);

  /// \brief Set the log interface that unhandled messages are forwarded to.
  void SetParentLog(wdLogInterface* pInterface) { m_pParentLog = pInterface; }

private:
  wdLogInterface* m_pParentLog = nullptr;

  struct ExpectedMsg
  {
    wdInt32 m_iCount = 0;
    wdString m_sMsgSubString;
    wdLogMsgType::Enum m_Type = wdLogMsgType::All;
  };

  mutable wdMutex m_Mutex;
  wdHybridArray<ExpectedMsg, 8> m_ExpectedMessages;
};

/// \brief A class that sets a custom wdTestLogInterface as the thread local default log system,
/// and resets the previous system when it goes out of scope. The test version passes the previous
/// wdLogInterface on to the wdTestLogInterface to enable passing on unhandled messages.
///
/// If bCatchMessagesGlobally is false, the system only intercepts messages on the current thread.
/// If bCatchMessagesGlobally is true, it will also intercept messages from other threads, as long as they
/// go through wdGlobalLog. See wdGlobalLog::SetGlobalLogOverride().
class WD_TEST_DLL wdTestLogSystemScope : public wdLogSystemScope
{
public:
  explicit wdTestLogSystemScope(wdTestLogInterface* pInterface, bool bCatchMessagesGlobally = false)
    : wdLogSystemScope(pInterface)
  {
    m_bCatchMessagesGlobally = bCatchMessagesGlobally;
    pInterface->SetParentLog(m_pPrevious);

    if (m_bCatchMessagesGlobally)
    {
      wdGlobalLog::SetGlobalLogOverride(pInterface);
    }
  }

  ~wdTestLogSystemScope()
  {
    if (m_bCatchMessagesGlobally)
    {
      wdGlobalLog::SetGlobalLogOverride(nullptr);
    }
  }

private:
  bool m_bCatchMessagesGlobally = false;
};
