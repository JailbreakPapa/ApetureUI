#include <Foundation/FoundationPCH.h>

#include <Foundation/Application/Application.h>
#include <Foundation/Configuration/Startup.h>

wdResult wdRun_Startup(wdApplication* pApplicationInstance)
{
  WD_ASSERT_ALWAYS(pApplicationInstance != nullptr, "wdRun() requires a valid non-null application instance pointer.");
  WD_ASSERT_ALWAYS(wdApplication::s_pApplicationInstance == nullptr, "There can only be one wdApplication.");

  // Set application instance pointer to the supplied instance
  wdApplication::s_pApplicationInstance = pApplicationInstance;

  WD_SUCCEED_OR_RETURN(pApplicationInstance->BeforeCoreSystemsStartup());

  // this will startup all base and core systems
  // 'StartupHighLevelSystems' must not be done before a window is available (if at all)
  // so we don't do that here
  wdStartup::StartupCoreSystems();

  pApplicationInstance->AfterCoreSystemsStartup();
  return WD_SUCCESS;
}

void wdRun_MainLoop(wdApplication* pApplicationInstance)
{
  while (pApplicationInstance->Run() == wdApplication::Execution::Continue)
  {
  }
}

void wdRun_Shutdown(wdApplication* pApplicationInstance)
{
  // high level systems shutdown
  // may do nothing, if the high level systems were never initialized
  {
    pApplicationInstance->BeforeHighLevelSystemsShutdown();
    wdStartup::ShutdownHighLevelSystems();
    pApplicationInstance->AfterHighLevelSystemsShutdown();
  }

  // core systems shutdown
  {
    pApplicationInstance->BeforeCoreSystemsShutdown();
    wdStartup::ShutdownCoreSystems();
    pApplicationInstance->AfterCoreSystemsShutdown();
  }

  // Flush standard output to make log available.
  fflush(stdout);
  fflush(stderr);

  // Reset application instance so code running after the app will trigger asserts etc. to be cleaned up
  // Destructor is called by entry point function
  wdApplication::s_pApplicationInstance = nullptr;

  // memory leak reporting cannot be done here, because the application instance is still alive and may still hold on to memory that needs
  // to be freed first
}

void wdRun(wdApplication* pApplicationInstance)
{
  if (wdRun_Startup(pApplicationInstance).Succeeded())
  {
    wdRun_MainLoop(pApplicationInstance);
  }
  wdRun_Shutdown(pApplicationInstance);
}

WD_STATICLINK_FILE(Foundation, Foundation_Application_Implementation_MainLoop);
