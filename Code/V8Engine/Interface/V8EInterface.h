#pragma once

#include <Foundation/Types/SharedPtr.h>
#include <V8Engine/V8EngineDLL.h>
#include <V8Engine/V8EnginePCH.h>

namespace v8
{
  class Platform;
}
namespace aperture
{
  class WD_V8ENGINE_DLL V8EInterface
  {
  public:
    V8EInterface();
    virtual ~V8EInterface();
    /// <summary>
    ///  Initializes the V8 Engine.
    /// </summary>
    /// <param name="exepath">The Path to the executable that is using the library.</param>
    /// <param name="resourcepath">This is the path that V8 will look for for resources, specificly startup data.</param>
    void InitV8(const char* exepath, const char* resourcepath);
    /// <summary>
    /// Shuts down V8.
    /// </summary>
    void ShutdownV8();

  protected:
  private:
    v8::Platform* V8EPlatform;
  };
} // namespace aperture
