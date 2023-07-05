#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/IO/FileSystem/FileSystem.h>
#include <Foundation/IO/FileSystem/FileWriter.h>
#include <Foundation/Profiling/Profiling.h>
#include <Foundation/Threading/ThreadUtils.h>

namespace
{
  void WriteOutProfilingCapture(const char* szFilePath)
  {
    wdStringBuilder outputPath = wdTestFramework::GetInstance()->GetAbsOutputPath();
    WD_TEST_BOOL(wdFileSystem::AddDataDirectory(outputPath.GetData(), "test", "output", wdFileSystem::AllowWrites) == WD_SUCCESS);

    wdFileWriter fileWriter;
    if (fileWriter.Open(szFilePath) == WD_SUCCESS)
    {
      wdProfilingSystem::ProfilingData profilingData;
      wdProfilingSystem::Capture(profilingData);
      profilingData.Write(fileWriter).IgnoreResult();
      wdLog::Info("Profiling capture saved to '{0}'.", fileWriter.GetFilePathAbsolute().GetData());
    }
  }
} // namespace

WD_CREATE_SIMPLE_TEST_GROUP(Profiling);

WD_CREATE_SIMPLE_TEST(Profiling, Profiling)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Nested scopes")
  {
    wdProfilingSystem::Clear();

    {
      WD_PROFILE_SCOPE("Prewarm scope");
      wdThreadUtils::Sleep(wdTime::Milliseconds(1));
    }

    wdTime endTime = wdTime::Now() + wdTime::Milliseconds(1);

    {
      WD_PROFILE_SCOPE("Outer scope");

      {
        WD_PROFILE_SCOPE("Inner scope");

        while (wdTime::Now() < endTime)
        {
        }
      }
    }

    WriteOutProfilingCapture(":output/profilingScopes.json");
  }
}
