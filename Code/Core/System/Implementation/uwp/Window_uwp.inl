#include <Core/CorePCH.h>

#include <Core/System/Window.h>
#include <Foundation/Basics.h>
#include <Foundation/Basics/Platform/uwp/UWPUtils.h>
#include <Foundation/Logging/Log.h>
#include <Foundation/Types/UniquePtr.h>
#include <windows.applicationmodel.core.h>
#include <windows.ui.core.h>

namespace
{
  struct wdWindowUwpData
  {
    ComPtr<ABI::Windows::UI::Core::ICoreDispatcher> m_dispatcher;
    ComPtr<ABI::Windows::UI::Core::ICoreWindow> m_coreWindow;
  };
  wdUniquePtr<wdWindowUwpData> s_uwpWindowData;
} // namespace

wdResult wdWindow::Initialize()
{
  WD_LOG_BLOCK("wdWindow::Initialize", m_CreationDescription.m_Title.GetData());

  if (m_bInitialized)
  {
    Destroy().IgnoreResult();
  }

  // Checking and adjustments to creation desc.
  {
    if (m_CreationDescription.m_WindowMode == wdWindowMode::FullscreenFixedResolution)
    {
      wdLog::Warning("wdWindowMode::FullscreenFixedResolution is not supported on UWP. Falling back to "
                     "wdWindowMode::FullscreenBorderlessNativeResolution.");
      m_CreationDescription.m_WindowMode = wdWindowMode::FullscreenBorderlessNativeResolution;
    }
    else if (m_CreationDescription.m_WindowMode == wdWindowMode::WindowFixedResolution)
    {
      wdLog::Warning("wdWindowMode::WindowFixedResolution is not supported on UWP since resizing a window can not be restricted. Falling "
                     "back to wdWindowMode::WindowResizable");
      m_CreationDescription.m_WindowMode = wdWindowMode::WindowResizable;
    }

    if (m_CreationDescription.AdjustWindowSizeAndPosition().Failed())
      wdLog::Warning("Failed to adjust window size and position settings.");

    WD_ASSERT_RELEASE(m_CreationDescription.m_Resolution.HasNonZeroArea(), "The client area size can't be zero sized!");
  }

  // The main window is handled in a special way in UWP (closing it closes the application, not created explicitely, every window has a
  // thread, ...) which is why we're supporting only a single window for for now.
  WD_ASSERT_RELEASE(s_uwpWindowData == nullptr, "Currently, there is only a single UWP window supported!");


  s_uwpWindowData = WD_DEFAULT_NEW(wdWindowUwpData);

  ComPtr<ABI::Windows::ApplicationModel::Core::ICoreImmersiveApplication> application;
  WD_HRESULT_TO_FAILURE(ABI::Windows::Foundation::GetActivationFactory(
    HStringReference(RuntimeClass_Windows_ApplicationModel_Core_CoreApplication).Get(), &application));

  ComPtr<ABI::Windows::ApplicationModel::Core::ICoreApplicationView> mainView;
  WD_HRESULT_TO_FAILURE(application->get_MainView(&mainView));

  WD_HRESULT_TO_FAILURE(mainView->get_CoreWindow(&s_uwpWindowData->m_coreWindow));
  m_hWindowHandle = s_uwpWindowData->m_coreWindow.Get();

  // Activation of main window already done in Uwp application implementation.
  //  WD_HRESULT_TO_FAILURE(s_uwpWindowData->m_coreWindow->Activate());
  WD_HRESULT_TO_FAILURE(s_uwpWindowData->m_coreWindow->get_Dispatcher(&s_uwpWindowData->m_dispatcher));

  {
    // Get current *logical* screen DPI to do a pixel correct resize.
    ComPtr<ABI::Windows::Graphics::Display::IDisplayInformationStatics> displayInfoStatics;
    WD_HRESULT_TO_FAILURE(ABI::Windows::Foundation::GetActivationFactory(
      HStringReference(RuntimeClass_Windows_Graphics_Display_DisplayInformation).Get(), &displayInfoStatics));
    ComPtr<ABI::Windows::Graphics::Display::IDisplayInformation> displayInfo;
    WD_HRESULT_TO_FAILURE(displayInfoStatics->GetForCurrentView(&displayInfo));
    FLOAT logicalDpi = 1.0f;
    WD_HRESULT_TO_FAILURE(displayInfo->get_LogicalDpi(&logicalDpi));

    // Need application view for the next steps...
    ComPtr<ABI::Windows::UI::ViewManagement::IApplicationViewStatics2> appViewStatics;
    WD_HRESULT_TO_FAILURE(ABI::Windows::Foundation::GetActivationFactory(
      HStringReference(RuntimeClass_Windows_UI_ViewManagement_ApplicationView).Get(), &appViewStatics));
    ComPtr<ABI::Windows::UI::ViewManagement::IApplicationView> appView;
    WD_HRESULT_TO_FAILURE(appViewStatics->GetForCurrentView(&appView));
    ComPtr<ABI::Windows::UI::ViewManagement::IApplicationView2> appView2;
    WD_HRESULT_TO_FAILURE(appView.As(&appView2));
    ComPtr<ABI::Windows::UI::ViewManagement::IApplicationView3> appView3;
    WD_HRESULT_TO_FAILURE(appView.As(&appView3));

    // Request/remove fullscreen from window if requested.
    boolean isFullscreen;
    WD_HRESULT_TO_FAILURE(appView3->get_IsFullScreenMode(&isFullscreen));
    if ((isFullscreen > 0) != wdWindowMode::IsFullscreen(m_CreationDescription.m_WindowMode))
    {
      if (wdWindowMode::IsFullscreen(m_CreationDescription.m_WindowMode))
      {
        WD_HRESULT_TO_FAILURE(appView3->TryEnterFullScreenMode(&isFullscreen));
        if (!isFullscreen)
          wdLog::Warning("Failed to enter full screen mode.");
      }
      else
      {
        WD_HRESULT_TO_FAILURE(appView3->ExitFullScreenMode());
      }
    }

    // Set size. Pointless though if we're fullscreen.
    if (!isFullscreen)
    {
      boolean successfulResize = false;
      ABI::Windows::Foundation::Size size;
      size.Width = m_CreationDescription.m_Resolution.width * 96.0f / logicalDpi;
      size.Height = m_CreationDescription.m_Resolution.height * 96.0f / logicalDpi;
      WD_HRESULT_TO_FAILURE(appView3->TryResizeView(size, &successfulResize));
      if (!successfulResize)
      {
        ABI::Windows::Foundation::Rect visibleBounds;
        WD_HRESULT_TO_FAILURE(appView2->get_VisibleBounds(&visibleBounds));
        wdUInt32 actualWidth = static_cast<wdUInt32>(visibleBounds.Width * (logicalDpi / 96.0f));
        wdUInt32 actualHeight = static_cast<wdUInt32>(visibleBounds.Height * (logicalDpi / 96.0f));

        wdLog::Warning("Failed to resize the window to {0}x{1}, instead (visible) size remains at {2}x{3}",
          m_CreationDescription.m_Resolution.width, m_CreationDescription.m_Resolution.height, actualWidth, actualHeight);

        // m_CreationDescription.m_Resolution.width = actualWidth;
        // m_CreationDescription.m_Resolution.height = actualHeight;
      }
    }
  }

  m_pInputDevice = WD_DEFAULT_NEW(wdStandardInputDevice, s_uwpWindowData->m_coreWindow.Get());
  m_pInputDevice->SetClipMouseCursor(m_CreationDescription.m_bClipMouseCursor ? wdMouseCursorClipMode::ClipToWindowImmediate : wdMouseCursorClipMode::NoClip);
  m_pInputDevice->SetShowMouseCursor(m_CreationDescription.m_bShowMouseCursor);

  m_bInitialized = true;

  return WD_SUCCESS;
}

wdResult wdWindow::Destroy()
{
  if (!m_bInitialized)
    return WD_SUCCESS;

  WD_LOG_BLOCK("wdWindow::Destroy");

  m_pInputDevice = nullptr;
  s_uwpWindowData = nullptr;


  wdLog::Success("Window destroyed.");

  return WD_SUCCESS;
}

wdResult wdWindow::Resize(const wdSizeU32& newWindowSize)
{
  //#TODO Resizing fails on UWP already via the init code.
  return WD_FAILURE;
}

void wdWindow::ProcessWindowMessages()
{
  WD_ASSERT_RELEASE(s_uwpWindowData != nullptr, "No uwp window data available.");

  // Apparently ProcessAllIfPresent does NOT process all events in the queue somehow
  // if this isn't executed quite often every frame (even at 60 Hz), spatial input events quickly queue up
  // and are delivered with many seconds delay
  // as far as I can tell, there is no way to figure out whether there are more queued events
  for (int i = 0; i < 64; ++i)
  {
    HRESULT result = s_uwpWindowData->m_dispatcher->ProcessEvents(ABI::Windows::UI::Core::CoreProcessEventsOption_ProcessAllIfPresent);
    if (FAILED(result))
    {
      wdLog::Error("Window event processing failed with error code: {0}", result);
    }
  }
}

void wdWindow::OnResize(const wdSizeU32& newWindowSize)
{
  wdLog::Info("Window resized to ({0}, {1})", newWindowSize.width, newWindowSize.height);
}

wdWindowHandle wdWindow::GetNativeWindowHandle() const
{
  return m_hWindowHandle;
}
