#pragma once

#include <V8Engine/Interface/V8EInterface.h>
// V8 Includes
#include <libplatform/libplatform.h>
#include <v8.h>

aperture::V8EInterface::V8EInterface()
{
}

aperture::V8EInterface::~V8EInterface()
{
}

void aperture::V8EInterface::InitV8(const char* exepath, const char* resourcepath)
{
  // Get ICU Data, startup_bin,etc..
  v8::V8::InitializeICUDefaultLocation(exepath);
  v8::V8::InitializeExternalStartupData(resourcepath);
  std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
  // Init Platform
  V8EPlatform = platform.get();

  v8::V8::InitializePlatform(V8EPlatform);
  v8::V8::Initialize();
}

void aperture::V8EInterface::ShutdownV8()
{
  v8::V8::Dispose();
  v8::V8::ShutdownPlatform();
}
