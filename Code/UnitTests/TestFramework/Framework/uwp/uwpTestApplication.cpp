#include <TestFramework/TestFrameworkPCH.h>

#if WD_ENABLED(WD_PLATFORM_WINDOWS_UWP)
#  include <Foundation/Basics/Platform/uwp/UWPUtils.h>
#  include <Foundation/Strings/StringConversion.h>
#  include <TestFramework/Framework/uwp/uwpTestApplication.h>
#  include <TestFramework/Framework/uwp/uwpTestFramework.h>
#  include <windows.ui.core.h>
#  include <wrl/event.h>

using namespace ABI::Windows::Foundation;

wdUwpTestApplication::wdUwpTestApplication(wdTestFramework& testFramework)
  : m_testFramework(testFramework)
{
}

wdUwpTestApplication::~wdUwpTestApplication() {}

HRESULT wdUwpTestApplication::CreateView(IFrameworkView** viewProvider)
{
  *viewProvider = this;
  return S_OK;
}

HRESULT wdUwpTestApplication::Initialize(ICoreApplicationView* applicationView)
{
  using OnActivatedHandler =
    __FITypedEventHandler_2_Windows__CApplicationModel__CCore__CCoreApplicationView_Windows__CApplicationModel__CActivation__CIActivatedEventArgs;
  WD_SUCCEED_OR_RETURN(
    applicationView->add_Activated(Callback<OnActivatedHandler>(this, &wdUwpTestApplication::OnActivated).Get(), &m_eventRegistrationOnActivate));



  wdStartup::StartupBaseSystems();

  return S_OK;
}

HRESULT wdUwpTestApplication::SetWindow(ABI::Windows::UI::Core::ICoreWindow* window)
{
  return S_OK;
}

HRESULT wdUwpTestApplication::Load(HSTRING entryPoint)
{
  return S_OK;
}

HRESULT wdUwpTestApplication::Run()
{
  ComPtr<ABI::Windows::UI::Core::ICoreWindowStatic> coreWindowStatics;
  WD_SUCCEED_OR_RETURN(
    ABI::Windows::Foundation::GetActivationFactory(HStringReference(RuntimeClass_Windows_UI_Core_CoreWindow).Get(), &coreWindowStatics));
  ComPtr<ABI::Windows::UI::Core::ICoreWindow> coreWindow;
  WD_SUCCEED_OR_RETURN(coreWindowStatics->GetForCurrentThread(&coreWindow));
  ComPtr<ABI::Windows::UI::Core::ICoreDispatcher> dispatcher;
  WD_SUCCEED_OR_RETURN(coreWindow->get_Dispatcher(&dispatcher));

  while (m_testFramework.RunTestExecutionLoop() == wdTestAppRun::Continue)
  {
    dispatcher->ProcessEvents(ABI::Windows::UI::Core::CoreProcessEventsOption_ProcessAllIfPresent);
  }

  return S_OK;
}

HRESULT wdUwpTestApplication::Uninitialize()
{
  m_testFramework.AbortTests();
  return S_OK;
}

HRESULT wdUwpTestApplication::OnActivated(ICoreApplicationView* applicationView, IActivatedEventArgs* args)
{
  applicationView->remove_Activated(m_eventRegistrationOnActivate);

  ActivationKind activationKind;
  WD_SUCCEED_OR_RETURN(args->get_Kind(&activationKind));

  if (activationKind == ActivationKind_Launch)
  {
    ComPtr<ILaunchActivatedEventArgs> launchArgs;
    WD_SUCCEED_OR_RETURN(args->QueryInterface(launchArgs.GetAddressOf()));

    HString argHString;
    WD_SUCCEED_OR_RETURN(launchArgs->get_Arguments(argHString.GetAddressOf()));

    wdDynamicArray<wdString> commandLineArgs;
    wdDynamicArray<const char*> argv;
    wdCommandLineUtils::SplitCommandLineString(wdStringUtf8(argHString).GetData(), true, commandLineArgs, argv);

    wdCommandLineUtils cmd;
    cmd.SetCommandLine(argv.GetCount(), argv.GetData(), wdCommandLineUtils::PreferOsArgs);

    m_testFramework.GetTestSettingsFromCommandLine(cmd);

    // Setup an extended execution session to prevent app from going to sleep during testing.
    wdUwpUtils::CreateInstance<IExtendedExecutionSession>(
      RuntimeClass_Windows_ApplicationModel_ExtendedExecution_ExtendedExecutionSession, m_extendedExecutionSession);
    WD_ASSERT_DEV(m_extendedExecutionSession, "Failed to create extended session. Can't prevent app from backgrounding during testing.");
    m_extendedExecutionSession->put_Reason(ExtendedExecutionReason::ExtendedExecutionReason_Unspecified);
    wdStringHString desc("Keep Unit Tests Running");
    m_extendedExecutionSession->put_Description(desc.GetData().Get());

    using OnRevokedHandler = __FITypedEventHandler_2_IInspectable_Windows__CApplicationModel__CExtendedExecution__CExtendedExecutionRevokedEventArgs;
    WD_SUCCEED_OR_RETURN(m_extendedExecutionSession->add_Revoked(
      Callback<OnRevokedHandler>(this, &wdUwpTestApplication::OnSessionRevoked).Get(), &m_eventRegistrationOnRevokedSession));

    ComPtr<__FIAsyncOperation_1_Windows__CApplicationModel__CExtendedExecution__CExtendedExecutionResult> pAsyncOp;
    if (SUCCEEDED(m_extendedExecutionSession->RequestExtensionAsync(&pAsyncOp)))
    {
      wdUwpUtils::wdWinRtPutCompleted<ExtendedExecutionResult, ExtendedExecutionResult>(pAsyncOp, [this](const ExtendedExecutionResult& pResult) {
        switch (pResult)
        {
          case ExtendedExecutionResult::ExtendedExecutionResult_Allowed:
            wdLog::Info("Extended session is active.");
            break;
          case ExtendedExecutionResult::ExtendedExecutionResult_Denied:
            wdLog::Error("Extended session is denied.");
            break;
        }
      });
    }
  }

  return S_OK;
}

HRESULT wdUwpTestApplication::OnSessionRevoked(IInspectable* sender, IExtendedExecutionRevokedEventArgs* args)
{
  wdLog::Error("Extended session revoked.");
  return S_OK;
}

#endif

WD_STATICLINK_FILE(TestFramework, TestFramework_Framework_uwp_uwpTestApplication);
