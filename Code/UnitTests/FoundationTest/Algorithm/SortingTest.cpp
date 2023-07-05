#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Containers/DynamicArray.h>

namespace
{
  struct CustomComparer
  {
    WD_ALWAYS_INLINE bool Less(wdInt32 a, wdInt32 b) const { return a > b; }

    // Comparision via operator. Sorting algorithm should prefer Less operator
    bool operator()(wdInt32 a, wdInt32 b) const { return a < b; }
  };
} // namespace

WD_CREATE_SIMPLE_TEST(Algorithm, Sorting)
{
  wdDynamicArray<wdInt32> a1;

  for (wdUInt32 i = 0; i < 2000; ++i)
  {
    a1.PushBack(rand() % 100000);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "QuickSort")
  {
    wdDynamicArray<wdInt32> a2 = a1;

    wdSorting::QuickSort(a1, CustomComparer()); // quicksort uses insertion sort for partitions smaller than 16 elements

    for (wdUInt32 i = 1; i < a1.GetCount(); ++i)
    {
      WD_TEST_BOOL(a1[i - 1] >= a1[i]);
    }

    wdArrayPtr<wdInt32> arrayPtr = a2;
    wdSorting::QuickSort(arrayPtr, CustomComparer()); // quicksort uses insertion sort for partitions smaller than 16 elements

    for (wdUInt32 i = 1; i < arrayPtr.GetCount(); ++i)
    {
      WD_TEST_BOOL(arrayPtr[i - 1] >= arrayPtr[i]);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "QuickSort - Lambda")
  {
    wdDynamicArray<wdInt32> a2 = a1;
    wdSorting::QuickSort(a2, [](const auto& a, const auto& b) { return a > b; });

    for (wdUInt32 i = 1; i < a2.GetCount(); ++i)
    {
      WD_TEST_BOOL(a2[i - 1] >= a2[i]);
    }
  }
}
