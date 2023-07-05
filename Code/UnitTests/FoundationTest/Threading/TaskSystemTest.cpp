#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/IO/FileSystem/DataDirTypeFolder.h>
#include <Foundation/IO/FileSystem/FileWriter.h>
#include <Foundation/Threading/TaskSystem.h>
#include <Foundation/Time/Time.h>
#include <Foundation/Utilities/DGMLWriter.h>

class wdTestTask final : public wdTask
{
public:
  wdUInt32 m_uiIterations;
  wdTestTask* m_pDependency;
  bool m_bSupportCancel;
  wdInt32 m_iTaskID;

  wdTestTask()
  {
    m_uiIterations = 50;
    m_pDependency = nullptr;
    m_bStarted = false;
    m_bDone = false;
    m_bSupportCancel = false;
    m_iTaskID = -1;

    ConfigureTask("wdTestTask", wdTaskNesting::Never);
  }

  bool IsStarted() const { return m_bStarted; }
  bool IsDone() const { return m_bDone; }
  bool IsMultiplicityDone() const { return m_iMultiplicityCount == (int)GetMultiplicity(); }

private:
  bool m_bStarted;
  bool m_bDone;
  mutable wdAtomicInteger32 m_iMultiplicityCount;

  virtual void ExecuteWithMultiplicity(wdUInt32 uiInvocation) const override { m_iMultiplicityCount.Increment(); }

  virtual void Execute() override
  {
    if (m_iTaskID >= 0)
      wdLog::Printf("Starting Task %i at %.4f\n", m_iTaskID, wdTime::Now().GetSeconds());

    m_bStarted = true;

    WD_TEST_BOOL(m_pDependency == nullptr || m_pDependency->IsTaskFinished());

    for (wdUInt32 obst = 0; obst < m_uiIterations; ++obst)
    {
      wdThreadUtils::Sleep(wdTime::Milliseconds(1));
      wdTime::Now();

      if (HasBeenCanceled() && m_bSupportCancel)
      {
        if (m_iTaskID >= 0)
          wdLog::Printf("Canceling Task %i at %.4f\n", m_iTaskID, wdTime::Now().GetSeconds());
        return;
      }
    }

    m_bDone = true;

    if (m_iTaskID >= 0)
      wdLog::Printf("Finishing Task %i at %.4f\n", m_iTaskID, wdTime::Now().GetSeconds());
  }
};

class TaskCallbacks
{
public:
  void TaskFinished(const wdSharedPtr<wdTask>& pTask) { m_pInt->Increment(); }

  void TaskGroupFinished(wdTaskGroupID id) { m_pInt->Increment(); }

  wdAtomicInteger32* m_pInt;
};

WD_CREATE_SIMPLE_TEST(Threading, TaskSystem)
{
  wdInt8 iWorkersShort = 4;
  wdInt8 iWorkersLong = 4;

  wdTaskSystem::SetWorkerThreadCount(iWorkersShort, iWorkersLong);
  wdThreadUtils::Sleep(wdTime::Milliseconds(500));

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Single Tasks")
  {
    wdSharedPtr<wdTestTask> t[3];

    t[0] = WD_DEFAULT_NEW(wdTestTask);
    t[1] = WD_DEFAULT_NEW(wdTestTask);
    t[2] = WD_DEFAULT_NEW(wdTestTask);

    t[0]->ConfigureTask("Task 0", wdTaskNesting::Never);
    t[1]->ConfigureTask("Task 1", wdTaskNesting::Maybe);
    t[2]->ConfigureTask("Task 2", wdTaskNesting::Never);

    auto tg0 = wdTaskSystem::StartSingleTask(t[0], wdTaskPriority::LateThisFrame);
    auto tg1 = wdTaskSystem::StartSingleTask(t[1], wdTaskPriority::ThisFrame);
    auto tg2 = wdTaskSystem::StartSingleTask(t[2], wdTaskPriority::EarlyThisFrame);

    wdTaskSystem::WaitForGroup(tg0);
    wdTaskSystem::WaitForGroup(tg1);
    wdTaskSystem::WaitForGroup(tg2);

    WD_TEST_BOOL(t[0]->IsDone());
    WD_TEST_BOOL(t[1]->IsDone());
    WD_TEST_BOOL(t[2]->IsDone());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Single Tasks with Dependencies")
  {
    wdSharedPtr<wdTestTask> t[4];

    t[0] = WD_DEFAULT_NEW(wdTestTask);
    t[1] = WD_DEFAULT_NEW(wdTestTask);
    t[2] = WD_DEFAULT_NEW(wdTestTask);
    t[3] = WD_DEFAULT_NEW(wdTestTask);

    wdTaskGroupID g[4];

    t[0]->ConfigureTask("Task 0", wdTaskNesting::Never);
    t[1]->ConfigureTask("Task 1", wdTaskNesting::Maybe);
    t[2]->ConfigureTask("Task 2", wdTaskNesting::Never);
    t[3]->ConfigureTask("Task 3", wdTaskNesting::Maybe);

    g[0] = wdTaskSystem::StartSingleTask(t[0], wdTaskPriority::LateThisFrame);
    g[1] = wdTaskSystem::StartSingleTask(t[1], wdTaskPriority::ThisFrame, g[0]);
    g[2] = wdTaskSystem::StartSingleTask(t[2], wdTaskPriority::EarlyThisFrame, g[1]);
    g[3] = wdTaskSystem::StartSingleTask(t[3], wdTaskPriority::EarlyThisFrame, g[0]);

    wdTaskSystem::WaitForGroup(g[2]);
    wdTaskSystem::WaitForGroup(g[3]);

    WD_TEST_BOOL(t[0]->IsDone());
    WD_TEST_BOOL(t[1]->IsDone());
    WD_TEST_BOOL(t[2]->IsDone());
    WD_TEST_BOOL(t[3]->IsDone());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Grouped Tasks / TaskFinished Callback / GroupFinished Callback")
  {
    wdSharedPtr<wdTestTask> t[8];

    wdTaskGroupID g[4];
    wdAtomicInteger32 GroupsFinished;
    wdAtomicInteger32 TasksFinished;

    TaskCallbacks callbackGroup;
    callbackGroup.m_pInt = &GroupsFinished;

    TaskCallbacks callbackTask;
    callbackTask.m_pInt = &TasksFinished;

    g[0] = wdTaskSystem::CreateTaskGroup(wdTaskPriority::ThisFrame, wdMakeDelegate(&TaskCallbacks::TaskGroupFinished, &callbackGroup));
    g[1] = wdTaskSystem::CreateTaskGroup(wdTaskPriority::ThisFrame, wdMakeDelegate(&TaskCallbacks::TaskGroupFinished, &callbackGroup));
    g[2] = wdTaskSystem::CreateTaskGroup(wdTaskPriority::ThisFrame, wdMakeDelegate(&TaskCallbacks::TaskGroupFinished, &callbackGroup));
    g[3] = wdTaskSystem::CreateTaskGroup(wdTaskPriority::ThisFrame, wdMakeDelegate(&TaskCallbacks::TaskGroupFinished, &callbackGroup));

    for (int i = 0; i < 4; ++i)
      WD_TEST_BOOL(!wdTaskSystem::IsTaskGroupFinished(g[i]));

    wdTaskSystem::AddTaskGroupDependency(g[1], g[0]);
    wdTaskSystem::AddTaskGroupDependency(g[2], g[0]);
    wdTaskSystem::AddTaskGroupDependency(g[3], g[1]);

    for (int i = 0; i < 8; ++i)
    {
      t[i] = WD_DEFAULT_NEW(wdTestTask);
      t[i]->ConfigureTask("Test Task", wdTaskNesting::Maybe, wdMakeDelegate(&TaskCallbacks::TaskFinished, &callbackTask));
    }

    wdTaskSystem::AddTaskToGroup(g[0], t[0]);
    wdTaskSystem::AddTaskToGroup(g[1], t[1]);
    wdTaskSystem::AddTaskToGroup(g[1], t[2]);
    wdTaskSystem::AddTaskToGroup(g[2], t[3]);
    wdTaskSystem::AddTaskToGroup(g[2], t[4]);
    wdTaskSystem::AddTaskToGroup(g[2], t[5]);
    wdTaskSystem::AddTaskToGroup(g[3], t[6]);
    wdTaskSystem::AddTaskToGroup(g[3], t[7]);

    for (int i = 0; i < 8; ++i)
    {
      WD_TEST_BOOL(!t[i]->IsTaskFinished());
      WD_TEST_BOOL(!t[i]->IsDone());
    }

    // do a snapshot
    // we don't validate it, just make sure it doesn't crash
    wdDGMLGraph graph;
    wdTaskSystem::WriteStateSnapshotToDGML(graph);

    wdTaskSystem::StartTaskGroup(g[3]);
    wdTaskSystem::StartTaskGroup(g[2]);
    wdTaskSystem::StartTaskGroup(g[1]);
    wdTaskSystem::StartTaskGroup(g[0]);

    wdTaskSystem::WaitForGroup(g[3]);
    wdTaskSystem::WaitForGroup(g[2]);
    wdTaskSystem::WaitForGroup(g[1]);
    wdTaskSystem::WaitForGroup(g[0]);

    WD_TEST_INT(TasksFinished, 8);

    // It is not guaranteed that group finished callback is called after WaitForGroup returned so we need to wait a bit here.
    for (int i = 0; i < 10; i++)
    {
      if (GroupsFinished == 4)
      {
        break;
      }
      wdThreadUtils::Sleep(wdTime::Milliseconds(10));
    }
    WD_TEST_INT(GroupsFinished, 4);

    for (int i = 0; i < 4; ++i)
      WD_TEST_BOOL(wdTaskSystem::IsTaskGroupFinished(g[i]));

    for (int i = 0; i < 8; ++i)
    {
      WD_TEST_BOOL(t[i]->IsTaskFinished());
      WD_TEST_BOOL(t[i]->IsDone());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "This Frame Tasks / Next Frame Tasks")
  {
    const wdUInt32 uiNumTasks = 20;
    wdSharedPtr<wdTestTask> t[uiNumTasks];
    wdTaskGroupID tg[uiNumTasks];
    bool finished[uiNumTasks];

    for (wdUInt32 i = 0; i < uiNumTasks; i += 2)
    {
      finished[i] = false;
      finished[i + 1] = false;

      t[i] = WD_DEFAULT_NEW(wdTestTask);
      t[i + 1] = WD_DEFAULT_NEW(wdTestTask);

      t[i]->m_uiIterations = 10;
      t[i + 1]->m_uiIterations = 20;

      tg[i] = wdTaskSystem::StartSingleTask(t[i], wdTaskPriority::ThisFrame);
      tg[i + 1] = wdTaskSystem::StartSingleTask(t[i + 1], wdTaskPriority::NextFrame);
    }

    // 'finish' the first frame
    wdTaskSystem::FinishFrameTasks();

    {
      wdUInt32 uiNotAllThisTasksFinished = 0;
      wdUInt32 uiNotAllNextTasksFinished = 0;

      for (wdUInt32 i = 0; i < uiNumTasks; i += 2)
      {
        if (!t[i]->IsTaskFinished())
        {
          WD_TEST_BOOL(!finished[i]);
          ++uiNotAllThisTasksFinished;
        }
        else
        {
          finished[i] = true;
        }

        if (!t[i + 1]->IsTaskFinished())
        {
          WD_TEST_BOOL(!finished[i + 1]);
          ++uiNotAllNextTasksFinished;
        }
        else
        {
          finished[i + 1] = true;
        }
      }

      // up to the number of worker threads tasks can still be active
      WD_TEST_BOOL(uiNotAllThisTasksFinished <= wdTaskSystem::GetNumAllocatedWorkerThreads(wdWorkerThreadType::ShortTasks));
      WD_TEST_BOOL(uiNotAllNextTasksFinished <= uiNumTasks);
    }


    // 'finish' the second frame
    wdTaskSystem::FinishFrameTasks();

    {
      wdUInt32 uiNotAllThisTasksFinished = 0;
      wdUInt32 uiNotAllNextTasksFinished = 0;

      for (int i = 0; i < uiNumTasks; i += 2)
      {
        if (!t[i]->IsTaskFinished())
        {
          WD_TEST_BOOL(!finished[i]);
          ++uiNotAllThisTasksFinished;
        }
        else
        {
          finished[i] = true;
        }

        if (!t[i + 1]->IsTaskFinished())
        {
          WD_TEST_BOOL(!finished[i + 1]);
          ++uiNotAllNextTasksFinished;
        }
        else
        {
          finished[i + 1] = true;
        }
      }

      WD_TEST_BOOL(
        uiNotAllThisTasksFinished + uiNotAllNextTasksFinished <= wdTaskSystem::GetNumAllocatedWorkerThreads(wdWorkerThreadType::ShortTasks));
    }

    // 'finish' all frames
    wdTaskSystem::FinishFrameTasks();

    {
      wdUInt32 uiNotAllThisTasksFinished = 0;
      wdUInt32 uiNotAllNextTasksFinished = 0;

      for (wdUInt32 i = 0; i < uiNumTasks; i += 2)
      {
        if (!t[i]->IsTaskFinished())
        {
          WD_TEST_BOOL(!finished[i]);
          ++uiNotAllThisTasksFinished;
        }
        else
        {
          finished[i] = true;
        }

        if (!t[i + 1]->IsTaskFinished())
        {
          WD_TEST_BOOL(!finished[i + 1]);
          ++uiNotAllNextTasksFinished;
        }
        else
        {
          finished[i + 1] = true;
        }
      }

      // even after finishing multiple frames, the previous frame tasks may still be in execution
      // since no N+x tasks enforce their completion in this test
      WD_TEST_BOOL(
        uiNotAllThisTasksFinished + uiNotAllNextTasksFinished <= wdTaskSystem::GetNumAllocatedWorkerThreads(wdWorkerThreadType::ShortTasks));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Main Thread Tasks")
  {
    const wdUInt32 uiNumTasks = 20;
    wdSharedPtr<wdTestTask> t[uiNumTasks];

    for (wdUInt32 i = 0; i < uiNumTasks; ++i)
    {
      t[i] = WD_DEFAULT_NEW(wdTestTask);
      t[i]->m_uiIterations = 10;

      wdTaskSystem::StartSingleTask(t[i], wdTaskPriority::ThisFrameMainThread);
    }

    wdTaskSystem::FinishFrameTasks();

    for (wdUInt32 i = 0; i < uiNumTasks; ++i)
    {
      WD_TEST_BOOL(t[i]->IsTaskFinished());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Canceling Tasks")
  {
    const wdUInt32 uiNumTasks = 20;
    wdSharedPtr<wdTestTask> t[uiNumTasks];
    wdTaskGroupID tg[uiNumTasks];

    for (int i = 0; i < uiNumTasks; ++i)
    {
      t[i] = WD_DEFAULT_NEW(wdTestTask);
      t[i]->m_uiIterations = 50;

      tg[i] = wdTaskSystem::StartSingleTask(t[i], wdTaskPriority::ThisFrame);
    }

    wdThreadUtils::Sleep(wdTime::Milliseconds(1));

    wdUInt32 uiCanceled = 0;

    for (wdUInt32 i0 = uiNumTasks; i0 > 0; --i0)
    {
      const wdUInt32 i = i0 - 1;

      if (wdTaskSystem::CancelTask(t[i], wdOnTaskRunning::ReturnWithoutBlocking) == WD_SUCCESS)
        ++uiCanceled;
    }

    wdUInt32 uiDone = 0;
    wdUInt32 uiStarted = 0;

    for (int i = 0; i < uiNumTasks; ++i)
    {
      wdTaskSystem::WaitForGroup(tg[i]);
      WD_TEST_BOOL(t[i]->IsTaskFinished());

      if (t[i]->IsDone())
        ++uiDone;
      if (t[i]->IsStarted())
        ++uiStarted;
    }

    // at least one task should have run and thus be 'done'
    WD_TEST_BOOL(uiDone > 0);
    WD_TEST_BOOL(uiDone < uiNumTasks);

    WD_TEST_BOOL(uiStarted > 0);
    WD_TEST_BOOL_MSG(uiStarted <= wdTaskSystem::GetNumAllocatedWorkerThreads(wdWorkerThreadType::ShortTasks),
      "This test can fail when the PC is under heavy load."); // should not have managed to start more tasks than there are threads
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Canceling Tasks (forcefully)")
  {
    const wdUInt32 uiNumTasks = 20;
    wdSharedPtr<wdTestTask> t[uiNumTasks];
    wdTaskGroupID tg[uiNumTasks];

    for (int i = 0; i < uiNumTasks; ++i)
    {
      t[i] = WD_DEFAULT_NEW(wdTestTask);
      t[i]->m_uiIterations = 50;
      t[i]->m_bSupportCancel = true;

      tg[i] = wdTaskSystem::StartSingleTask(t[i], wdTaskPriority::ThisFrame);
    }

    wdThreadUtils::Sleep(wdTime::Milliseconds(1));

    wdUInt32 uiCanceled = 0;

    for (int i = uiNumTasks - 1; i >= 0; --i)
    {
      if (wdTaskSystem::CancelTask(t[i], wdOnTaskRunning::ReturnWithoutBlocking) == WD_SUCCESS)
        ++uiCanceled;
    }

    wdUInt32 uiDone = 0;
    wdUInt32 uiStarted = 0;

    for (int i = 0; i < uiNumTasks; ++i)
    {
      wdTaskSystem::WaitForGroup(tg[i]);
      WD_TEST_BOOL(t[i]->IsTaskFinished());

      if (t[i]->IsDone())
        ++uiDone;
      if (t[i]->IsStarted())
        ++uiStarted;
    }

    // not a single thread should have finished the execution
    if (WD_TEST_BOOL_MSG(uiDone == 0, "This test can fail when the PC is under heavy load."))
    {
      WD_TEST_BOOL(uiStarted > 0);
      WD_TEST_BOOL(uiStarted <= wdTaskSystem::GetNumAllocatedWorkerThreads(
                                  wdWorkerThreadType::ShortTasks)); // should not have managed to start more tasks than there are threads
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Canceling Group")
  {
    const wdUInt32 uiNumTasks = 4;
    wdSharedPtr<wdTestTask> t1[uiNumTasks];
    wdSharedPtr<wdTestTask> t2[uiNumTasks];

    wdTaskGroupID g1, g2;
    g1 = wdTaskSystem::CreateTaskGroup(wdTaskPriority::ThisFrame);
    g2 = wdTaskSystem::CreateTaskGroup(wdTaskPriority::ThisFrame);

    wdTaskSystem::AddTaskGroupDependency(g2, g1);

    for (wdUInt32 i = 0; i < uiNumTasks; ++i)
    {
      t1[i] = WD_DEFAULT_NEW(wdTestTask);
      t2[i] = WD_DEFAULT_NEW(wdTestTask);

      wdTaskSystem::AddTaskToGroup(g1, t1[i]);
      wdTaskSystem::AddTaskToGroup(g2, t2[i]);
    }

    wdTaskSystem::StartTaskGroup(g2);
    wdTaskSystem::StartTaskGroup(g1);

    wdThreadUtils::Sleep(wdTime::Milliseconds(10));

    WD_TEST_BOOL(wdTaskSystem::CancelGroup(g2, wdOnTaskRunning::WaitTillFinished) == WD_SUCCESS);

    for (int i = 0; i < uiNumTasks; ++i)
    {
      WD_TEST_BOOL(!t2[i]->IsDone());
      WD_TEST_BOOL(t2[i]->IsTaskFinished());
    }

    wdThreadUtils::Sleep(wdTime::Milliseconds(1));

    WD_TEST_BOOL(wdTaskSystem::CancelGroup(g1, wdOnTaskRunning::WaitTillFinished) == WD_FAILURE);

    for (int i = 0; i < uiNumTasks; ++i)
    {
      WD_TEST_BOOL(!t2[i]->IsDone());

      WD_TEST_BOOL(t1[i]->IsTaskFinished());
      WD_TEST_BOOL(t2[i]->IsTaskFinished());
    }

    wdThreadUtils::Sleep(wdTime::Milliseconds(100));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Tasks with Multiplicity")
  {
    wdSharedPtr<wdTestTask> t[3];
    wdTaskGroupID tg[3];

    t[0] = WD_DEFAULT_NEW(wdTestTask);
    t[1] = WD_DEFAULT_NEW(wdTestTask);
    t[2] = WD_DEFAULT_NEW(wdTestTask);

    t[0]->ConfigureTask("Task 0", wdTaskNesting::Maybe);
    t[1]->ConfigureTask("Task 1", wdTaskNesting::Maybe);
    t[2]->ConfigureTask("Task 2", wdTaskNesting::Never);

    t[0]->SetMultiplicity(1);
    t[1]->SetMultiplicity(100);
    t[2]->SetMultiplicity(1000);

    tg[0] = wdTaskSystem::StartSingleTask(t[0], wdTaskPriority::LateThisFrame);
    tg[1] = wdTaskSystem::StartSingleTask(t[1], wdTaskPriority::ThisFrame);
    tg[2] = wdTaskSystem::StartSingleTask(t[2], wdTaskPriority::EarlyThisFrame);

    wdTaskSystem::WaitForGroup(tg[0]);
    wdTaskSystem::WaitForGroup(tg[1]);
    wdTaskSystem::WaitForGroup(tg[2]);

    WD_TEST_BOOL(t[0]->IsMultiplicityDone());
    WD_TEST_BOOL(t[1]->IsMultiplicityDone());
    WD_TEST_BOOL(t[2]->IsMultiplicityDone());
  }

  // capture profiling info for testing
  /*wdStringBuilder sOutputPath = wdTestFramework::GetInstance()->GetAbsOutputPath();

  wdFileSystem::AddDataDirectory(sOutputPath.GetData());

  wdFileWriter fileWriter;
  if (fileWriter.Open("profiling.json") == WD_SUCCESS)
  {
  wdProfilingSystem::Capture(fileWriter);
  }*/
}
