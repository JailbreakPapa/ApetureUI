#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Containers/StaticArray.h>
#include <Foundation/Threading/TaskSystem.h>

namespace
{
  static constexpr wdUInt32 s_uiNumberOfWorkers = 4;
  static constexpr wdUInt32 s_uiTaskItemSliceSize = 25;
  static constexpr wdUInt32 s_uiTotalNumberOfTaskItems = s_uiNumberOfWorkers * s_uiTaskItemSliceSize;
} // namespace

WD_CREATE_SIMPLE_TEST(Threading, ParallelFor)
{
  // set up controlled task system environment
  wdTaskSystem::SetWorkerThreadCount(::s_uiNumberOfWorkers, ::s_uiNumberOfWorkers);

  // shared variables
  wdMutex dataAccessMutex;

  wdUInt32 uiRangesEncounteredCheck = 0;
  wdUInt32 uiNumbersSum = 0;

  wdUInt32 uiNumbersCheckSum = 0;
  wdStaticArray<wdUInt32, ::s_uiTotalNumberOfTaskItems> numbers;

  wdParallelForParams parallelForParams;
  parallelForParams.m_uiBinSize = ::s_uiTaskItemSliceSize;
  parallelForParams.m_uiMaxTasksPerThread = 1;

  auto ResetSharedVariables = [&uiRangesEncounteredCheck, &uiNumbersSum, &uiNumbersCheckSum, &numbers]() {
    uiRangesEncounteredCheck = 0;
    uiNumbersSum = 0;

    uiNumbersCheckSum = 0;

    numbers.EnsureCount(::s_uiTotalNumberOfTaskItems);
    for (wdUInt32 i = 0; i < ::s_uiTotalNumberOfTaskItems; ++i)
    {
      numbers[i] = i + 1;
      uiNumbersCheckSum += numbers[i];
    }
  };

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Parallel For (Indexed)")
  {
    // reset
    ResetSharedVariables();

    // test
    // sum up the slice of number assigned to us via index ranges and
    // check if the ranges described by them are as expected
    wdTaskSystem::ParallelForIndexed(
      0, ::s_uiTotalNumberOfTaskItems,
      [&dataAccessMutex, &uiRangesEncounteredCheck, &uiNumbersSum, &numbers](wdUInt32 uiStartIndex, wdUInt32 uiEndIndex) {
        WD_LOCK(dataAccessMutex);

        // size check
        WD_TEST_INT(uiEndIndex - uiStartIndex, ::s_uiTaskItemSliceSize);

        // note down which range this is
        uiRangesEncounteredCheck |= 1 << (uiStartIndex / ::s_uiTaskItemSliceSize);

        // sum up numbers in our slice
        for (wdUInt32 uiIndex = uiStartIndex; uiIndex < uiEndIndex; ++uiIndex)
        {
          uiNumbersSum += numbers[uiIndex];
        }
      },
      "ParallelForIndexed Test", parallelForParams);

    // check results
    WD_TEST_INT(uiRangesEncounteredCheck, 0b1111);
    WD_TEST_INT(uiNumbersSum, uiNumbersCheckSum);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Parallel For (Array)")
  {
    // reset
    ResetSharedVariables();

    // test-specific data
    wdStaticArray<wdUInt32*, ::s_uiNumberOfWorkers> startAddresses;
    for (wdUInt32 i = 0; i < ::s_uiNumberOfWorkers; ++i)
    {
      startAddresses.PushBack(numbers.GetArrayPtr().GetPtr() + (i * ::s_uiTaskItemSliceSize));
    }

    // test
    // sum up the slice of numbers assigned to us via array pointers and
    // check if the ranges described by them are as expected
    wdTaskSystem::ParallelFor<wdUInt32>(
      numbers.GetArrayPtr(),
      [&dataAccessMutex, &uiRangesEncounteredCheck, &uiNumbersSum, &startAddresses](wdArrayPtr<wdUInt32> taskItemSlice) {
        WD_LOCK(dataAccessMutex);

        // size check
        WD_TEST_INT(taskItemSlice.GetCount(), ::s_uiTaskItemSliceSize);

        // note down which range this is
        for (wdUInt32 index = 0; index < startAddresses.GetCount(); ++index)
        {
          if (startAddresses[index] == taskItemSlice.GetPtr())
          {
            uiRangesEncounteredCheck |= 1 << index;
          }
        }

        // sum up numbers in our slice
        for (const wdUInt32& number : taskItemSlice)
        {
          uiNumbersSum += number;
        }
      },
      "ParallelFor Array Test", parallelForParams);

    // check results
    WD_TEST_INT(15, 0b1111);
    WD_TEST_INT(uiNumbersSum, uiNumbersCheckSum);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Parallel For (Array, Single)")
  {
    // reset
    ResetSharedVariables();

    // test
    // sum up the slice of numbers by summing up the individual numbers that get handed to us
    wdTaskSystem::ParallelForSingle(
      numbers.GetArrayPtr(),
      [&dataAccessMutex, &uiNumbersSum](wdUInt32 uiNumber) {
        WD_LOCK(dataAccessMutex);
        uiNumbersSum += uiNumber;
      },
      "ParallelFor Array Single Test", parallelForParams);

    // check the resulting sum
    WD_TEST_INT(uiNumbersSum, uiNumbersCheckSum);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Parallel For (Array, Single, Index)")
  {
    // reset
    ResetSharedVariables();

    // test
    // sum up the slice of numbers that got assigned to us via an index range
    wdTaskSystem::ParallelForSingleIndex(
      numbers.GetArrayPtr(),
      [&dataAccessMutex, &uiNumbersSum](wdUInt32 uiIndex, wdUInt32 uiNumber) {
        WD_LOCK(dataAccessMutex);
        uiNumbersSum += uiNumber + (uiIndex + 1);
      },
      "ParallelFor Array Single Index Test", parallelForParams);

    // check the resulting sum
    WD_TEST_INT(uiNumbersSum, 2 * uiNumbersCheckSum);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Parallel For (Array, Single) Write")
  {
    // reset
    ResetSharedVariables();

    // test
    // modify the original array of numbers
    wdTaskSystem::ParallelForSingle(
      numbers.GetArrayPtr(),
      [&dataAccessMutex](wdUInt32& ref_uiNumber) {
        WD_LOCK(dataAccessMutex);
        ref_uiNumber = ref_uiNumber * 3;
      },
      "ParallelFor Array Single Write Test (Write)", parallelForParams);

    // sum up the new values to test if writing worked
    wdTaskSystem::ParallelForSingle(
      numbers.GetArrayPtr(),
      [&dataAccessMutex, &uiNumbersSum](const wdUInt32& uiNumber) {
        WD_LOCK(dataAccessMutex);
        uiNumbersSum += uiNumber;
      },
      "ParallelFor Array Single Write Test (Sum)", parallelForParams);

    // check the resulting sum
    WD_TEST_INT(uiNumbersSum, 3 * uiNumbersCheckSum);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Parallel For (Array, Single, Index) Write")
  {
    // reset
    ResetSharedVariables();

    // test
    // modify the original array of numbers
    wdTaskSystem::ParallelForSingleIndex(
      numbers.GetArrayPtr(),
      [&dataAccessMutex](wdUInt32, wdUInt32& ref_uiNumber) {
        WD_LOCK(dataAccessMutex);
        ref_uiNumber = ref_uiNumber * 4;
      },
      "ParallelFor Array Single Write Test (Write)", parallelForParams);

    // sum up the new values to test if writing worked
    wdTaskSystem::ParallelForSingle(
      numbers.GetArrayPtr(),
      [&dataAccessMutex, &uiNumbersSum](const wdUInt32& uiNumber) {
        WD_LOCK(dataAccessMutex);
        uiNumbersSum += uiNumber;
      },
      "ParallelFor Array Single Write Test (Sum)", parallelForParams);

    // check the resulting sum
    WD_TEST_INT(uiNumbersSum, 4 * uiNumbersCheckSum);
  }
}
