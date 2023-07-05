#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Utilities/Progress.h>

WD_CREATE_SIMPLE_TEST(Utility, Progress)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Simple progress")
  {
    wdProgress progress;
    {
      wdProgressRange progressRange = wdProgressRange("TestProgress", 4, false, &progress);

      WD_TEST_FLOAT(progress.GetCompletion(), 0.0f, wdMath::DefaultEpsilon<float>());
      WD_TEST_BOOL(progress.GetMainDisplayText() == "TestProgress");

      progressRange.BeginNextStep("Step1");
      WD_TEST_FLOAT(progress.GetCompletion(), 0.0f, wdMath::DefaultEpsilon<float>());
      WD_TEST_BOOL(progress.GetStepDisplayText() == "Step1");

      progressRange.BeginNextStep("Step2");
      WD_TEST_FLOAT(progress.GetCompletion(), 0.25f, wdMath::DefaultEpsilon<float>());
      WD_TEST_BOOL(progress.GetStepDisplayText() == "Step2");

      progressRange.BeginNextStep("Step3");
      WD_TEST_FLOAT(progress.GetCompletion(), 0.5f, wdMath::DefaultEpsilon<float>());
      WD_TEST_BOOL(progress.GetStepDisplayText() == "Step3");

      progressRange.BeginNextStep("Step4");
      WD_TEST_FLOAT(progress.GetCompletion(), 0.75f, wdMath::DefaultEpsilon<float>());
      WD_TEST_BOOL(progress.GetStepDisplayText() == "Step4");
    }

    WD_TEST_FLOAT(progress.GetCompletion(), 1.0f, wdMath::DefaultEpsilon<float>());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Weighted progress")
  {
    wdProgress progress;
    {
      wdProgressRange progressRange = wdProgressRange("TestProgress", 4, false, &progress);
      progressRange.SetStepWeighting(2, 2.0f);

      WD_TEST_FLOAT(progress.GetCompletion(), 0.0f, wdMath::DefaultEpsilon<float>());

      progressRange.BeginNextStep("Step0+1", 2);
      WD_TEST_FLOAT(progress.GetCompletion(), 0.2f, wdMath::DefaultEpsilon<float>());

      progressRange.BeginNextStep("Step2"); // this step should have twice the weight as the other steps.
      WD_TEST_FLOAT(progress.GetCompletion(), 0.4f, wdMath::DefaultEpsilon<float>());

      progressRange.BeginNextStep("Step3");
      WD_TEST_FLOAT(progress.GetCompletion(), 0.8f, wdMath::DefaultEpsilon<float>());
    }

    WD_TEST_FLOAT(progress.GetCompletion(), 1.0f, wdMath::DefaultEpsilon<float>());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Nested progress")
  {
    wdProgress progress;
    {
      wdProgressRange progressRange = wdProgressRange("TestProgress", 4, false, &progress);
      progressRange.SetStepWeighting(2, 2.0f);

      WD_TEST_FLOAT(progress.GetCompletion(), 0.0f, wdMath::DefaultEpsilon<float>());

      progressRange.BeginNextStep("Step0");
      WD_TEST_FLOAT(progress.GetCompletion(), 0.0f, wdMath::DefaultEpsilon<float>());

      progressRange.BeginNextStep("Step1");
      WD_TEST_FLOAT(progress.GetCompletion(), 0.2f, wdMath::DefaultEpsilon<float>());

      progressRange.BeginNextStep("Step2");
      WD_TEST_FLOAT(progress.GetCompletion(), 0.4f, wdMath::DefaultEpsilon<float>());

      {
        wdProgressRange nestedRange = wdProgressRange("Nested", 5, false, &progress);
        nestedRange.SetStepWeighting(1, 4.0f);

        WD_TEST_FLOAT(progress.GetCompletion(), 0.4f, wdMath::DefaultEpsilon<float>());

        nestedRange.BeginNextStep("NestedStep0");
        WD_TEST_FLOAT(progress.GetCompletion(), 0.4f, wdMath::DefaultEpsilon<float>());

        nestedRange.BeginNextStep("NestedStep1");
        WD_TEST_FLOAT(progress.GetCompletion(), 0.45f, wdMath::DefaultEpsilon<float>());

        nestedRange.BeginNextStep("NestedStep2");
        WD_TEST_FLOAT(progress.GetCompletion(), 0.65f, wdMath::DefaultEpsilon<float>());

        nestedRange.BeginNextStep("NestedStep3");
        WD_TEST_FLOAT(progress.GetCompletion(), 0.7f, wdMath::DefaultEpsilon<float>());

        nestedRange.BeginNextStep("NestedStep4");
        WD_TEST_FLOAT(progress.GetCompletion(), 0.75f, wdMath::DefaultEpsilon<float>());
      }
      WD_TEST_FLOAT(progress.GetCompletion(), 0.8f, wdMath::DefaultEpsilon<float>());

      progressRange.BeginNextStep("Step3");
      WD_TEST_FLOAT(progress.GetCompletion(), 0.8f, wdMath::DefaultEpsilon<float>());
    }

    WD_TEST_FLOAT(progress.GetCompletion(), 1.0f, wdMath::DefaultEpsilon<float>());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Nested progress with manual completion")
  {
    wdProgress progress;
    {
      wdProgressRange progressRange = wdProgressRange("TestProgress", 3, false, &progress);
      progressRange.SetStepWeighting(1, 2.0f);

      WD_TEST_FLOAT(progress.GetCompletion(), 0.0f, wdMath::DefaultEpsilon<float>());

      progressRange.BeginNextStep("Step0");
      WD_TEST_FLOAT(progress.GetCompletion(), 0.0f, wdMath::DefaultEpsilon<float>());

      progressRange.BeginNextStep("Step1");
      WD_TEST_FLOAT(progress.GetCompletion(), 0.25f, wdMath::DefaultEpsilon<float>());

      {
        wdProgressRange nestedRange = wdProgressRange("Nested", false, &progress);

        WD_TEST_FLOAT(progress.GetCompletion(), 0.25f, wdMath::DefaultEpsilon<float>());

        nestedRange.SetCompletion(0.5);
        WD_TEST_FLOAT(progress.GetCompletion(), 0.5f, wdMath::DefaultEpsilon<float>());
      }
      WD_TEST_FLOAT(progress.GetCompletion(), 0.75f, wdMath::DefaultEpsilon<float>());

      progressRange.BeginNextStep("Step2");
      WD_TEST_FLOAT(progress.GetCompletion(), 0.75f, wdMath::DefaultEpsilon<float>());
    }

    WD_TEST_FLOAT(progress.GetCompletion(), 1.0f, wdMath::DefaultEpsilon<float>());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Progress Events")
  {
    wdUInt32 uiNumProgressUpdatedEvents = 0;

    wdProgress progress;
    progress.m_Events.AddEventHandler([&](const wdProgressEvent& e) {
      if (e.m_Type == wdProgressEvent::Type::ProgressChanged)
      {
        ++uiNumProgressUpdatedEvents;
        WD_TEST_FLOAT(e.m_pProgressbar->GetCompletion(), uiNumProgressUpdatedEvents * 0.25f, wdMath::DefaultEpsilon<float>());
      } });

    {
      wdProgressRange progressRange = wdProgressRange("TestProgress", 4, false, &progress);

      progressRange.BeginNextStep("Step1");
      progressRange.BeginNextStep("Step2");
      progressRange.BeginNextStep("Step3");
      progressRange.BeginNextStep("Step4");
    }

    WD_TEST_FLOAT(progress.GetCompletion(), 1.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_INT(uiNumProgressUpdatedEvents, 4);
  }
}
