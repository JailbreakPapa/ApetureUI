#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Memory/CommonAllocators.h>
#include <Foundation/Memory/LargeBlockAllocator.h>
#include <Foundation/Memory/StackAllocator.h>

struct alignas(WD_ALIGNMENT_MINIMUM) NonAlignedVector
{
  WD_DECLARE_POD_TYPE();

  NonAlignedVector()
  {
    x = 5.0f;
    y = 6.0f;
    z = 8.0f;
  }

  float x;
  float y;
  float z;
};

struct alignas(16) AlignedVector
{
  WD_DECLARE_POD_TYPE();

  AlignedVector()
  {
    x = 5.0f;
    y = 6.0f;
    z = 8.0f;
  }

  float x;
  float y;
  float z;
  float w;
};

template <typename T>
void TestAlignmentHelper(size_t uiExpectedAlignment)
{
  wdAllocatorBase* pAllocator = wdFoundation::GetAlignedAllocator();
  WD_TEST_BOOL(pAllocator != nullptr);

  size_t uiAlignment = WD_ALIGNMENT_OF(T);
  WD_TEST_INT(uiAlignment, uiExpectedAlignment);

  T testOnStack = T();
  WD_TEST_BOOL(wdMemoryUtils::IsAligned(&testOnStack, uiExpectedAlignment));

  T* pTestBuffer = WD_NEW_RAW_BUFFER(pAllocator, T, 32);
  wdArrayPtr<T> TestArray = WD_NEW_ARRAY(pAllocator, T, 32);

  // default constructor should be called even if we declare as a pod type
  WD_TEST_FLOAT(TestArray[0].x, 5.0f, 0.0f);
  WD_TEST_FLOAT(TestArray[0].y, 6.0f, 0.0f);
  WD_TEST_FLOAT(TestArray[0].z, 8.0f, 0.0f);

  WD_TEST_BOOL(wdMemoryUtils::IsAligned(pTestBuffer, uiExpectedAlignment));
  WD_TEST_BOOL(wdMemoryUtils::IsAligned(TestArray.GetPtr(), uiExpectedAlignment));

  size_t uiExpectedSize = sizeof(T) * 32;

#if WD_ENABLED(WD_USE_ALLOCATION_TRACKING)
  WD_TEST_INT(pAllocator->AllocatedSize(pTestBuffer), uiExpectedSize);

  wdAllocatorBase::Stats stats = pAllocator->GetStats();
  WD_TEST_INT(stats.m_uiAllocationSize, uiExpectedSize * 2);
  WD_TEST_INT(stats.m_uiNumAllocations - stats.m_uiNumDeallocations, 2);
#endif

  WD_DELETE_ARRAY(pAllocator, TestArray);
  WD_DELETE_RAW_BUFFER(pAllocator, pTestBuffer);

#if WD_ENABLED(WD_USE_ALLOCATION_TRACKING)
  stats = pAllocator->GetStats();
  WD_TEST_INT(stats.m_uiAllocationSize, 0);
  WD_TEST_INT(stats.m_uiNumAllocations - stats.m_uiNumDeallocations, 0);
#endif
}

WD_CREATE_SIMPLE_TEST_GROUP(Memory);

WD_CREATE_SIMPLE_TEST(Memory, Allocator)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Alignment")
  {
    TestAlignmentHelper<NonAlignedVector>(WD_ALIGNMENT_MINIMUM);
    TestAlignmentHelper<AlignedVector>(16);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "LargeBlockAllocator")
  {
    enum
    {
      BLOCK_SIZE_IN_BYTES = 4096 * 4
    };
    const wdUInt32 uiPageSize = wdSystemInformation::Get().GetMemoryPageSize();

    wdLargeBlockAllocator<BLOCK_SIZE_IN_BYTES> allocator("Test", wdFoundation::GetDefaultAllocator(), wdMemoryTrackingFlags::EnableAllocationTracking);

    wdDynamicArray<wdDataBlock<int, BLOCK_SIZE_IN_BYTES>> blocks;
    blocks.Reserve(1000);

    for (wdUInt32 i = 0; i < 17; ++i)
    {
      auto block = allocator.AllocateBlock<int>();
      WD_TEST_BOOL(wdMemoryUtils::IsAligned(block.m_pData, uiPageSize)); // test page alignment
      WD_TEST_INT(block.m_uiCount, 0);

      blocks.PushBack(block);
    }

    wdAllocatorBase::Stats stats = allocator.GetStats();

    WD_TEST_BOOL(stats.m_uiNumAllocations == 17);
    WD_TEST_BOOL(stats.m_uiNumDeallocations == 0);
    WD_TEST_BOOL(stats.m_uiAllocationSize == 17 * BLOCK_SIZE_IN_BYTES);

    for (wdUInt32 i = 0; i < 200; ++i)
    {
      auto block = allocator.AllocateBlock<int>();
      blocks.PushBack(block);
    }

    for (wdUInt32 i = 0; i < 200; ++i)
    {
      allocator.DeallocateBlock(blocks.PeekBack());
      blocks.PopBack();
    }

    stats = allocator.GetStats();

    WD_TEST_BOOL(stats.m_uiNumAllocations == 217);
    WD_TEST_BOOL(stats.m_uiNumDeallocations == 200);
    WD_TEST_BOOL(stats.m_uiAllocationSize == 17 * BLOCK_SIZE_IN_BYTES);

    for (wdUInt32 i = 0; i < 2000; ++i)
    {
      wdUInt32 uiAction = rand() % 2;
      if (uiAction == 0)
      {
        blocks.PushBack(allocator.AllocateBlock<int>());
      }
      else if (blocks.GetCount() > 0)
      {
        wdUInt32 uiIndex = rand() % blocks.GetCount();
        auto block = blocks[uiIndex];

        allocator.DeallocateBlock(block);

        blocks.RemoveAtAndSwap(uiIndex);
      }
    }

    for (wdUInt32 i = 0; i < blocks.GetCount(); ++i)
    {
      allocator.DeallocateBlock(blocks[i]);
    }

    stats = allocator.GetStats();

    WD_TEST_BOOL(stats.m_uiNumAllocations - stats.m_uiNumDeallocations == 0);
    WD_TEST_BOOL(stats.m_uiAllocationSize == 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "StackAllocator")
  {
    wdStackAllocator<> allocator("TestStackAllocator", wdFoundation::GetAlignedAllocator());

    void* blocks[8];
    for (size_t i = 0; i < WD_ARRAY_SIZE(blocks); i++)
    {
      size_t size = i + 1;
      blocks[i] = allocator.Allocate(size, sizeof(void*), nullptr);
      WD_TEST_BOOL(blocks[i] != nullptr);
      if (i > 0)
      {
        WD_TEST_BOOL((wdUInt8*)blocks[i - 1] + (size - 1) <= blocks[i]);
      }
    }

    for (size_t i = WD_ARRAY_SIZE(blocks); i--;)
    {
      allocator.Deallocate(blocks[i]);
    }

    size_t sizes[] = {128, 128, 4096, 1024, 1024, 16000, 512, 512, 768, 768, 16000, 16000, 16000, 16000};
    void* allocs[WD_ARRAY_SIZE(sizes)];
    for (size_t i = 0; i < WD_ARRAY_SIZE(sizes); i++)
    {
      allocs[i] = allocator.Allocate(sizes[i], sizeof(void*), nullptr);
      WD_TEST_BOOL(allocs[i] != nullptr);
    }
    for (size_t i = WD_ARRAY_SIZE(sizes); i--;)
    {
      allocator.Deallocate(allocs[i]);
    }
    allocator.Reset();

    for (size_t i = 0; i < WD_ARRAY_SIZE(sizes); i++)
    {
      allocs[i] = allocator.Allocate(sizes[i], sizeof(void*), nullptr);
      WD_TEST_BOOL(allocs[i] != nullptr);
    }
    allocator.Reset();
    allocs[0] = allocator.Allocate(8, sizeof(void*), nullptr);
    WD_TEST_BOOL(allocs[0] < allocs[1]);
    allocator.Deallocate(allocs[0]);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "StackAllocator with non-PODs")
  {
    wdStackAllocator<> allocator("TestStackAllocator", wdFoundation::GetAlignedAllocator());

    wdDynamicArray<wdConstructionCounter*> counters;
    counters.Reserve(100);

    for (wdUInt32 i = 0; i < 100; ++i)
    {
      counters.PushBack(WD_NEW(&allocator, wdConstructionCounter));
    }

    for (wdUInt32 i = 0; i < 100; ++i)
    {
      WD_NEW(&allocator, NonAlignedVector);
    }

    WD_TEST_BOOL(wdConstructionCounter::HasConstructed(100));

    for (wdUInt32 i = 0; i < 50; ++i)
    {
      WD_DELETE(&allocator, counters[i * 2]);
    }

    WD_TEST_BOOL(wdConstructionCounter::HasDestructed(50));

    allocator.Reset();

    WD_TEST_BOOL(wdConstructionCounter::HasDestructed(50));
  }
}
