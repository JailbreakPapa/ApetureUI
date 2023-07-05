#pragma once

#if WD_ENABLED(WD_PLATFORM_WINDOWS_UWP)

#  include <TestFramework/Framework/TestFramework.h>
#  include <TestFramework/TestFrameworkDLL.h>

#  include <Foundation/Basics/Platform/uwp/UWPUtils.h>

/// \brief Derived wdTestFramework which signals the GUI to update whenever a new tests result comes in.
class WD_TEST_DLL wdUwpTestFramework : public wdTestFramework
{
public:
  wdUwpTestFramework(const char* szTestName, const char* szAbsTestDir, const char* szRelTestDataDir, int argc, const char** argv);
  virtual ~wdUwpTestFramework();

  wdUwpTestFramework(wdUwpTestFramework&) = delete;
  void operator=(wdUwpTestFramework&) = delete;

  void Run();
};

#endif
