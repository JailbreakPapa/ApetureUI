#include <TestFramework/TestFrameworkPCH.h>

#if WD_ENABLED(WD_PLATFORM_WINDOWS_UWP)
#  include <TestFramework/Framework/uwp/uwpTestApplication.h>
#  include <TestFramework/Framework/uwp/uwpTestFramework.h>

#  include <Foundation/Logging/Log.h>

wdUwpTestFramework::wdUwpTestFramework(const char* szTestName, const char* szAbsTestDir, const char* szRelTestDataDir, int argc, const char** argv)
  : wdTestFramework(szTestName, szAbsTestDir, szRelTestDataDir, argc, argv)
{
}

wdUwpTestFramework::~wdUwpTestFramework()
{
  RoUninitialize();
}

void wdUwpTestFramework::Run()
{
  ComPtr<ABI::Windows::ApplicationModel::Core::ICoreApplication> coreApplication;
  HRESULT result = ABI::Windows::Foundation::GetActivationFactory(
    HStringReference(RuntimeClass_Windows_ApplicationModel_Core_CoreApplication).Get(), &coreApplication);
  if (FAILED(result))
  {
    std::cout << "Failed to create core application." << std::endl;
    return;
  }
  else
  {
    ComPtr<wdUwpTestApplication> application = Make<wdUwpTestApplication>(*this);
    coreApplication->Run(application.Get());
    application.Detach(); // Was already deleted by uwp.
  }
}

#endif

WD_STATICLINK_FILE(TestFramework, TestFramework_Framework_uwp_uwpTestFramework);
