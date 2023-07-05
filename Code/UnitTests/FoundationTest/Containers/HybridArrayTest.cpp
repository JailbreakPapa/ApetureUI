#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Containers/HybridArray.h>
#include <Foundation/Memory/CommonAllocators.h>
#include <Foundation/Strings/String.h>

namespace HybridArrayTestDetail
{

  class Dummy
  {
  public:
    int a;
    std::string s;

    Dummy()
      : a(0)
      , s("Test")
    {
    }
    Dummy(int a)
      : a(a)
      , s("Test")
    {
    }
    Dummy(const Dummy& other)
      : a(other.a)
      , s(other.s)
    {
    }
    ~Dummy() {}

    Dummy& operator=(const Dummy& other)
    {
      a = other.a;
      s = other.s;
      return *this;
    }

    bool operator<=(const Dummy& dummy) const { return a <= dummy.a; }
    bool operator>=(const Dummy& dummy) const { return a >= dummy.a; }
    bool operator>(const Dummy& dummy) const { return a > dummy.a; }
    bool operator<(const Dummy& dummy) const { return a < dummy.a; }
    bool operator==(const Dummy& dummy) const { return a == dummy.a; }
  };

  class NonMovableClass
  {
  public:
    NonMovableClass(int iVal)
    {
      m_val = iVal;
      m_pVal = &m_val;
    }

    NonMovableClass(const NonMovableClass& other)
    {
      m_val = other.m_val;
      m_pVal = &m_val;
    }

    void operator=(const NonMovableClass& other) { m_val = other.m_val; }

    int m_val = 0;
    int* m_pVal = nullptr;
  };

  template <typename T>
  static wdHybridArray<T, 16> CreateArray(wdUInt32 uiSize, wdUInt32 uiOffset)
  {
    wdHybridArray<T, 16> a;
    a.SetCount(uiSize);

    for (wdUInt32 i = 0; i < uiSize; ++i)
      a[i] = T(uiOffset + i);

    return a;
  }

  struct ExternalCounter
  {
    WD_DECLARE_MEM_RELOCATABLE_TYPE();

    ExternalCounter() = default;

    ExternalCounter(int& ref_iCounter)
      : m_counter{&ref_iCounter}
    {
    }

    ~ExternalCounter()
    {
      if (m_counter)
        (*m_counter)++;
    }

    int* m_counter{};
  };
} // namespace HybridArrayTestDetail

static void TakesDynamicArray(wdDynamicArray<int>& ref_ar, int iNum, int iStart);

#if WD_ENABLED(WD_PLATFORM_64BIT)
static_assert(sizeof(wdHybridArray<wdInt32, 1>) == 32);
#else
static_assert(sizeof(wdHybridArray<wdInt32, 1>) == 20);
#endif

static_assert(wdGetTypeClass<wdHybridArray<wdInt32, 1>>::value == wdTypeIsMemRelocatable::value);
static_assert(wdGetTypeClass<wdHybridArray<HybridArrayTestDetail::NonMovableClass, 1>>::value == wdTypeIsClass::value);

WD_CREATE_SIMPLE_TEST(Containers, HybridArray)
{
  wdConstructionCounter::Reset();

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor")
  {
    wdHybridArray<wdInt32, 16> a1;
    wdHybridArray<wdConstructionCounter, 16> a2;

    WD_TEST_BOOL(a1.GetCount() == 0);
    WD_TEST_BOOL(a2.GetCount() == 0);
    WD_TEST_BOOL(a1.IsEmpty());
    WD_TEST_BOOL(a2.IsEmpty());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Copy Constructor")
  {
    wdHybridArray<wdInt32, 16> a1;

    WD_TEST_BOOL(a1.GetHeapMemoryUsage() == 0);

    for (wdInt32 i = 0; i < 32; ++i)
    {
      a1.PushBack(rand() % 100000);

      if (i < 16)
      {
        WD_TEST_BOOL(a1.GetHeapMemoryUsage() == 0);
      }
      else
      {
        WD_TEST_BOOL(a1.GetHeapMemoryUsage() >= i * sizeof(wdInt32));
      }
    }

    wdHybridArray<wdInt32, 16> a2 = a1;
    wdHybridArray<wdInt32, 16> a3(a1);

    WD_TEST_BOOL(a1 == a2);
    WD_TEST_BOOL(a1 == a3);
    WD_TEST_BOOL(a2 == a3);

    wdInt32 test[] = {1, 2, 3, 4};
    wdArrayPtr<wdInt32> aptr(test);

    wdHybridArray<wdInt32, 16> a4(aptr);

    WD_TEST_BOOL(a4 == aptr);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Move Constructor / Operator")
  {
    WD_TEST_BOOL(wdConstructionCounter::HasAllDestructed());

    {
      // move constructor external storage
      wdHybridArray<wdConstructionCounter, 16> a1(HybridArrayTestDetail::CreateArray<wdConstructionCounter>(100, 20));

      WD_TEST_INT(a1.GetCount(), 100);
      for (wdUInt32 i = 0; i < a1.GetCount(); ++i)
        WD_TEST_INT(a1[i].m_iData, 20 + i);

      // move operator external storage
      a1 = HybridArrayTestDetail::CreateArray<wdConstructionCounter>(200, 50);

      WD_TEST_INT(a1.GetCount(), 200);
      for (wdUInt32 i = 0; i < a1.GetCount(); ++i)
        WD_TEST_INT(a1[i].m_iData, 50 + i);
    }

    WD_TEST_BOOL(wdConstructionCounter::HasAllDestructed());
    wdConstructionCounter::Reset();

    {
      // move constructor internal storage
      wdHybridArray<wdConstructionCounter, 16> a2(HybridArrayTestDetail::CreateArray<wdConstructionCounter>(10, 30));

      WD_TEST_INT(a2.GetCount(), 10);
      for (wdUInt32 i = 0; i < a2.GetCount(); ++i)
        WD_TEST_INT(a2[i].m_iData, 30 + i);

      // move operator internal storage
      a2 = HybridArrayTestDetail::CreateArray<wdConstructionCounter>(8, 70);

      WD_TEST_INT(a2.GetCount(), 8);
      for (wdUInt32 i = 0; i < a2.GetCount(); ++i)
        WD_TEST_INT(a2[i].m_iData, 70 + i);
    }

    WD_TEST_BOOL(wdConstructionCounter::HasAllDestructed());
    wdConstructionCounter::Reset();

    wdConstructionCounterRelocatable::Reset();
    {
      // move constructor external storage relocatable
      wdHybridArray<wdConstructionCounterRelocatable, 16> a1(HybridArrayTestDetail::CreateArray<wdConstructionCounterRelocatable>(100, 20));

      WD_TEST_BOOL(wdConstructionCounterRelocatable::HasDone(100, 0));

      WD_TEST_INT(a1.GetCount(), 100);
      for (wdUInt32 i = 0; i < a1.GetCount(); ++i)
        WD_TEST_INT(a1[i].m_iData, 20 + i);

      // move operator external storage
      a1 = HybridArrayTestDetail::CreateArray<wdConstructionCounterRelocatable>(200, 50);
      WD_TEST_BOOL(wdConstructionCounterRelocatable::HasDone(200, 100));

      WD_TEST_INT(a1.GetCount(), 200);
      for (wdUInt32 i = 0; i < a1.GetCount(); ++i)
        WD_TEST_INT(a1[i].m_iData, 50 + i);
    }

    WD_TEST_BOOL(wdConstructionCounterRelocatable::HasAllDestructed());
    wdConstructionCounterRelocatable::Reset();

    {
      // move constructor internal storage relocatable
      wdHybridArray<wdConstructionCounterRelocatable, 16> a2(HybridArrayTestDetail::CreateArray<wdConstructionCounterRelocatable>(10, 30));
      WD_TEST_BOOL(wdConstructionCounterRelocatable::HasDone(10, 0));

      WD_TEST_INT(a2.GetCount(), 10);
      for (wdUInt32 i = 0; i < a2.GetCount(); ++i)
        WD_TEST_INT(a2[i].m_iData, 30 + i);

      // move operator internal storage
      a2 = HybridArrayTestDetail::CreateArray<wdConstructionCounterRelocatable>(8, 70);
      WD_TEST_BOOL(wdConstructionCounterRelocatable::HasDone(8, 10));

      WD_TEST_INT(a2.GetCount(), 8);
      for (wdUInt32 i = 0; i < a2.GetCount(); ++i)
        WD_TEST_INT(a2[i].m_iData, 70 + i);
    }

    WD_TEST_BOOL(wdConstructionCounterRelocatable::HasAllDestructed());
    wdConstructionCounterRelocatable::Reset();

    {
      // move constructor with different allocators
      wdProxyAllocator proxyAllocator("test allocator", wdFoundation::GetDefaultAllocator());
      {
        wdHybridArray<wdConstructionCounterRelocatable, 16> a1(&proxyAllocator);

        a1 = HybridArrayTestDetail::CreateArray<wdConstructionCounterRelocatable>(8, 70);
        WD_TEST_BOOL(wdConstructionCounterRelocatable::HasDone(8, 0));
        WD_TEST_BOOL(a1.GetAllocator() == &proxyAllocator); // allocator must not change

        WD_TEST_INT(a1.GetCount(), 8);
        for (wdUInt32 i = 0; i < a1.GetCount(); ++i)
          WD_TEST_INT(a1[i].m_iData, 70 + i);

        a1 = HybridArrayTestDetail::CreateArray<wdConstructionCounterRelocatable>(32, 100);
        WD_TEST_BOOL(wdConstructionCounterRelocatable::HasDone(32, 8));
        WD_TEST_BOOL(a1.GetAllocator() == &proxyAllocator); // allocator must not change

        WD_TEST_INT(a1.GetCount(), 32);
        for (wdUInt32 i = 0; i < a1.GetCount(); ++i)
          WD_TEST_INT(a1[i].m_iData, 100 + i);
      }

      WD_TEST_BOOL(wdConstructionCounterRelocatable::HasAllDestructed());
      wdConstructionCounterRelocatable::Reset();

      auto allocatorStats = proxyAllocator.GetStats();
      WD_TEST_BOOL(allocatorStats.m_uiNumAllocations == allocatorStats.m_uiNumDeallocations); // check for memory leak?
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Convert to ArrayPtr")
  {
    wdHybridArray<wdInt32, 16> a1;

    for (wdInt32 i = 0; i < 100; ++i)
    {
      wdInt32 r = rand() % 100000;
      a1.PushBack(r);
    }

    wdArrayPtr<wdInt32> ap = a1;

    WD_TEST_BOOL(ap.GetCount() == a1.GetCount());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator =")
  {
    wdHybridArray<wdInt32, 16> a1, a2;

    for (wdInt32 i = 0; i < 100; ++i)
      a1.PushBack(i);

    a2 = a1;

    WD_TEST_BOOL(a1 == a2);

    wdArrayPtr<wdInt32> arrayPtr(a1);

    a2 = arrayPtr;

    WD_TEST_BOOL(a2 == arrayPtr);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator == / !=")
  {
    wdHybridArray<wdInt32, 16> a1, a2;

    WD_TEST_BOOL(a1 == a1);
    WD_TEST_BOOL(a2 == a2);
    WD_TEST_BOOL(a1 == a2);

    WD_TEST_BOOL((a1 != a1) == false);
    WD_TEST_BOOL((a2 != a2) == false);
    WD_TEST_BOOL((a1 != a2) == false);

    for (wdInt32 i = 0; i < 100; ++i)
    {
      wdInt32 r = rand() % 100000;
      a1.PushBack(r);
      a2.PushBack(r);
    }

    WD_TEST_BOOL(a1 == a1);
    WD_TEST_BOOL(a2 == a2);
    WD_TEST_BOOL(a1 == a2);

    WD_TEST_BOOL((a1 != a2) == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Index operator")
  {
    wdHybridArray<wdInt32, 16> a1;
    a1.SetCountUninitialized(100);

    for (wdInt32 i = 0; i < 100; ++i)
      a1[i] = i;

    for (wdInt32 i = 0; i < 100; ++i)
      WD_TEST_INT(a1[i], i);

    const wdHybridArray<wdInt32, 16> ca1 = a1;

    for (wdInt32 i = 0; i < 100; ++i)
      WD_TEST_INT(ca1[i], i);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetCount / GetCount / IsEmpty")
  {
    wdHybridArray<wdInt32, 16> a1;

    WD_TEST_BOOL(a1.IsEmpty());

    for (wdInt32 i = 0; i < 128; ++i)
    {
      a1.SetCount(i + 1);
      WD_TEST_INT(a1[i], 0);
      a1[i] = i;

      WD_TEST_INT(a1.GetCount(), i + 1);
      WD_TEST_BOOL(!a1.IsEmpty());
    }

    for (wdInt32 i = 0; i < 128; ++i)
      WD_TEST_INT(a1[i], i);

    for (wdInt32 i = 128; i >= 0; --i)
    {
      a1.SetCount(i);

      WD_TEST_INT(a1.GetCount(), i);

      for (wdInt32 i2 = 0; i2 < i; ++i2)
        WD_TEST_INT(a1[i2], i2);
    }

    WD_TEST_BOOL(a1.IsEmpty());

    a1.SetCountUninitialized(32);
    WD_TEST_INT(a1.GetCount(), 32);
    a1[31] = 45;
    WD_TEST_INT(a1[31], 45);

    // Test SetCount with fill value
    {
      wdHybridArray<wdInt32, 2> a2;
      a2.PushBack(5);
      a2.PushBack(3);
      a2.SetCount(10, 42);

      if (WD_TEST_INT(a2.GetCount(), 10))
      {
        WD_TEST_INT(a2[0], 5);
        WD_TEST_INT(a2[1], 3);
        WD_TEST_INT(a2[4], 42);
        WD_TEST_INT(a2[9], 42);
      }

      a2.Clear();
      a2.PushBack(1);
      a2.PushBack(2);
      a2.PushBack(3);

      a2.SetCount(2, 10);
      if (WD_TEST_INT(a2.GetCount(), 2))
      {
        WD_TEST_INT(a2[0], 1);
        WD_TEST_INT(a2[1], 2);
      }
    }
  }

  // Test SetCount with fill value
  {
    wdHybridArray<wdInt32, 2> a2;
    a2.PushBack(5);
    a2.PushBack(3);
    a2.SetCount(10, 42);

    if (WD_TEST_INT(a2.GetCount(), 10))
    {
      WD_TEST_INT(a2[0], 5);
      WD_TEST_INT(a2[1], 3);
      WD_TEST_INT(a2[4], 42);
      WD_TEST_INT(a2[9], 42);
    }

    a2.Clear();
    a2.PushBack(1);
    a2.PushBack(2);
    a2.PushBack(3);

    a2.SetCount(2, 10);
    if (WD_TEST_INT(a2.GetCount(), 2))
    {
      WD_TEST_INT(a2[0], 1);
      WD_TEST_INT(a2[1], 2);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Clear")
  {
    wdHybridArray<wdInt32, 16> a1;
    a1.Clear();

    a1.PushBack(3);
    a1.Clear();

    WD_TEST_BOOL(a1.IsEmpty());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Contains / IndexOf / LastIndexOf")
  {
    wdHybridArray<wdInt32, 16> a1;

    for (wdInt32 i = -100; i < 100; ++i)
      WD_TEST_BOOL(!a1.Contains(i));

    for (wdInt32 i = 0; i < 100; ++i)
      a1.PushBack(i);

    for (wdInt32 i = 0; i < 100; ++i)
    {
      WD_TEST_BOOL(a1.Contains(i));
      WD_TEST_INT(a1.IndexOf(i), i);
      WD_TEST_INT(a1.LastIndexOf(i), i);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Insert")
  {
    wdHybridArray<wdInt32, 16> a1;

    // always inserts at the front
    for (wdInt32 i = 0; i < 100; ++i)
      a1.Insert(i, 0);

    for (wdInt32 i = 0; i < 100; ++i)
      WD_TEST_INT(a1[i], 99 - i);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "RemoveAndCopy")
  {
    wdHybridArray<wdInt32, 16> a1;

    for (wdInt32 i = 0; i < 100; ++i)
      a1.PushBack(i % 2);

    while (a1.RemoveAndCopy(1))
    {
    }

    WD_TEST_BOOL(a1.GetCount() == 50);

    for (wdUInt32 i = 0; i < a1.GetCount(); ++i)
      WD_TEST_INT(a1[i], 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "RemoveAndSwap")
  {
    wdHybridArray<wdInt32, 16> a1;

    for (wdInt32 i = 0; i < 10; ++i)
      a1.Insert(i, i); // inserts at the end

    a1.RemoveAndSwap(9);
    a1.RemoveAndSwap(7);
    a1.RemoveAndSwap(5);
    a1.RemoveAndSwap(3);
    a1.RemoveAndSwap(1);

    WD_TEST_INT(a1.GetCount(), 5);

    for (wdInt32 i = 0; i < 5; ++i)
      WD_TEST_BOOL(wdMath::IsEven(a1[i]));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "RemoveAtAndCopy")
  {
    wdHybridArray<wdInt32, 16> a1;

    for (wdInt32 i = 0; i < 10; ++i)
      a1.Insert(i, i); // inserts at the end

    a1.RemoveAtAndCopy(9);
    a1.RemoveAtAndCopy(7);
    a1.RemoveAtAndCopy(5);
    a1.RemoveAtAndCopy(3);
    a1.RemoveAtAndCopy(1);

    WD_TEST_INT(a1.GetCount(), 5);

    for (wdInt32 i = 0; i < 5; ++i)
      WD_TEST_INT(a1[i], i * 2);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "RemoveAtAndSwap")
  {
    wdHybridArray<wdInt32, 16> a1;

    for (wdInt32 i = 0; i < 10; ++i)
      a1.Insert(i, i); // inserts at the end

    a1.RemoveAtAndSwap(9);
    a1.RemoveAtAndSwap(7);
    a1.RemoveAtAndSwap(5);
    a1.RemoveAtAndSwap(3);
    a1.RemoveAtAndSwap(1);

    WD_TEST_INT(a1.GetCount(), 5);

    for (wdInt32 i = 0; i < 5; ++i)
      WD_TEST_BOOL(wdMath::IsEven(a1[i]));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "PushBack / PopBack / PeekBack")
  {
    wdHybridArray<wdInt32, 16> a1;

    for (wdInt32 i = 0; i < 10; ++i)
    {
      a1.PushBack(i);
      WD_TEST_INT(a1.PeekBack(), i);
    }

    for (wdInt32 i = 9; i >= 0; --i)
    {
      WD_TEST_INT(a1.PeekBack(), i);
      a1.PopBack();
    }

    a1.PushBack(23);
    a1.PushBack(2);
    a1.PushBack(3);

    a1.PopBack(2);
    WD_TEST_INT(a1.PeekBack(), 23);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ExpandAndGetRef")
  {
    wdHybridArray<wdInt32, 16> a1;

    for (wdInt32 i = 0; i < 20; ++i)
    {
      wdInt32& intRef = a1.ExpandAndGetRef();
      intRef = i * 5;
    }


    WD_TEST_BOOL(a1.GetCount() == 20);

    for (wdInt32 i = 0; i < 20; ++i)
    {
      WD_TEST_INT(a1[i], i * 5);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Construction / Destruction")
  {
    {
      WD_TEST_BOOL(wdConstructionCounter::HasAllDestructed());

      wdHybridArray<wdConstructionCounter, 16> a1;
      wdHybridArray<wdConstructionCounter, 16> a2;

      WD_TEST_BOOL(wdConstructionCounter::HasDone(0, 0)); // nothing has been constructed / destructed in between
      WD_TEST_BOOL(wdConstructionCounter::HasAllDestructed());

      a1.PushBack(wdConstructionCounter(1));
      WD_TEST_BOOL(wdConstructionCounter::HasDone(2, 1)); // one temporary, one final (copy constructed)

      a1.Insert(wdConstructionCounter(2), 0);
      WD_TEST_BOOL(wdConstructionCounter::HasDone(2, 1)); // one temporary, one final (copy constructed)

      a2 = a1;
      WD_TEST_BOOL(wdConstructionCounter::HasDone(2, 0)); // two copies

      a1.Clear();
      WD_TEST_BOOL(wdConstructionCounter::HasDone(0, 2));

      a1.PushBack(wdConstructionCounter(3));
      a1.PushBack(wdConstructionCounter(4));
      a1.PushBack(wdConstructionCounter(5));
      a1.PushBack(wdConstructionCounter(6));

      WD_TEST_BOOL(wdConstructionCounter::HasDone(8, 4)); // four temporaries

      a1.RemoveAndCopy(wdConstructionCounter(3));
      WD_TEST_BOOL(wdConstructionCounter::HasDone(1, 2)); // one temporary, one destroyed

      a1.RemoveAndCopy(wdConstructionCounter(3));
      WD_TEST_BOOL(wdConstructionCounter::HasDone(1, 1)); // one temporary, none destroyed

      a1.RemoveAtAndCopy(0);
      WD_TEST_BOOL(wdConstructionCounter::HasDone(0, 1)); // one destroyed

      a1.RemoveAtAndSwap(0);
      WD_TEST_BOOL(wdConstructionCounter::HasDone(0, 1)); // one destroyed
    }

    // tests the destructor of a2 and a1
    WD_TEST_BOOL(wdConstructionCounter::HasAllDestructed());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Compact")
  {
    wdHybridArray<wdInt32, 16> a;

    for (wdInt32 i = 0; i < 1008; ++i)
    {
      a.PushBack(i);
      WD_TEST_INT(a.GetCount(), i + 1);
    }

    WD_TEST_BOOL(a.GetHeapMemoryUsage() > 0);
    a.Compact();
    WD_TEST_BOOL(a.GetHeapMemoryUsage() > 0);

    for (wdInt32 i = 0; i < 1008; ++i)
      WD_TEST_INT(a[i], i);

    // this tests whether the static array is reused properly (not the case anymore with new implementation that derives from wdDynamicArray)
    a.SetCount(15);
    a.Compact();
    // WD_TEST_BOOL(a.GetHeapMemoryUsage() == 0);
    WD_TEST_BOOL(a.GetHeapMemoryUsage() > 0);

    for (wdInt32 i = 0; i < 15; ++i)
      WD_TEST_INT(a[i], i);

    a.Clear();
    a.Compact();
    WD_TEST_BOOL(a.GetHeapMemoryUsage() == 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SortingPrimitives")
  {
    wdHybridArray<wdUInt32, 16> list;

    list.Sort();

    for (wdUInt32 i = 0; i < 45; i++)
    {
      list.PushBack(std::rand());
    }
    list.Sort();

    wdUInt32 last = 0;
    for (wdUInt32 i = 0; i < list.GetCount(); i++)
    {
      WD_TEST_BOOL(last <= list[i]);
      last = list[i];
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SortingObjects")
  {
    wdHybridArray<HybridArrayTestDetail::Dummy, 16> list;
    list.Reserve(128);

    for (wdUInt32 i = 0; i < 100; i++)
    {
      list.PushBack(HybridArrayTestDetail::Dummy(rand()));
    }
    list.Sort();

    HybridArrayTestDetail::Dummy last = 0;
    for (wdUInt32 i = 0; i < list.GetCount(); i++)
    {
      WD_TEST_BOOL(last <= list[i]);
      last = list[i];
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Various")
  {
    wdHybridArray<HybridArrayTestDetail::Dummy, 16> list;
    list.PushBack(1);
    list.PushBack(2);
    list.PushBack(3);
    list.Insert(4, 3);
    list.Insert(0, 1);
    list.Insert(0, 5);

    WD_TEST_BOOL(list[0].a == 1);
    WD_TEST_BOOL(list[1].a == 0);
    WD_TEST_BOOL(list[2].a == 2);
    WD_TEST_BOOL(list[3].a == 3);
    WD_TEST_BOOL(list[4].a == 4);
    WD_TEST_BOOL(list[5].a == 0);
    WD_TEST_BOOL(list.GetCount() == 6);

    list.RemoveAtAndCopy(3);
    list.RemoveAtAndSwap(2);

    WD_TEST_BOOL(list[0].a == 1);
    WD_TEST_BOOL(list[1].a == 0);
    WD_TEST_BOOL(list[2].a == 0);
    WD_TEST_BOOL(list[3].a == 4);
    WD_TEST_BOOL(list.GetCount() == 4);
    WD_TEST_BOOL(list.IndexOf(0) == 1);
    WD_TEST_BOOL(list.LastIndexOf(0) == 2);

    list.PushBack(5);
    WD_TEST_BOOL(list[4].a == 5);
    HybridArrayTestDetail::Dummy d = list.PeekBack();
    list.PopBack();
    WD_TEST_BOOL(d.a == 5);
    WD_TEST_BOOL(list.GetCount() == 4);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Assignment")
  {
    wdHybridArray<HybridArrayTestDetail::Dummy, 16> list;
    for (int i = 0; i < 16; i++)
    {
      list.PushBack(HybridArrayTestDetail::Dummy(rand()));
    }

    wdHybridArray<HybridArrayTestDetail::Dummy, 16> list2;
    for (int i = 0; i < 8; i++)
    {
      list2.PushBack(HybridArrayTestDetail::Dummy(rand()));
    }

    list = list2;
    WD_TEST_BOOL(list.GetCount() == list2.GetCount());

    list2.Clear();
    WD_TEST_BOOL(list2.GetCount() == 0);

    list2 = list;
    WD_TEST_BOOL(list.PeekBack() == list2.PeekBack());
    WD_TEST_BOOL(list == list2);

    for (int i = 0; i < 16; i++)
    {
      list2.PushBack(HybridArrayTestDetail::Dummy(rand()));
    }

    list = list2;
    WD_TEST_BOOL(list.PeekBack() == list2.PeekBack());
    WD_TEST_BOOL(list == list2);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Count")
  {
    wdHybridArray<HybridArrayTestDetail::Dummy, 16> list;
    for (int i = 0; i < 16; i++)
    {
      list.PushBack(HybridArrayTestDetail::Dummy(rand()));
    }
    list.SetCount(32);
    list.SetCount(4);

    list.Compact();
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Reserve")
  {
    WD_TEST_BOOL(wdConstructionCounter::HasAllDestructed());

    wdHybridArray<wdConstructionCounter, 16> a;

    WD_TEST_BOOL(wdConstructionCounter::HasDone(0, 0)); // nothing has been constructed / destructed in between
    WD_TEST_BOOL(wdConstructionCounter::HasAllDestructed());

    a.Reserve(100);

    WD_TEST_BOOL(wdConstructionCounter::HasDone(0, 0)); // nothing has been constructed / destructed in between
    WD_TEST_BOOL(wdConstructionCounter::HasAllDestructed());

    a.SetCount(10);
    WD_TEST_BOOL(wdConstructionCounter::HasDone(10, 0));

    a.Reserve(100);
    WD_TEST_BOOL(wdConstructionCounter::HasDone(0, 0));

    a.SetCount(100);
    WD_TEST_BOOL(wdConstructionCounter::HasDone(90, 0));

    a.Reserve(200);
    WD_TEST_BOOL(wdConstructionCounter::HasDone(100, 100)); // had to copy some elements over

    a.SetCount(200);
    WD_TEST_BOOL(wdConstructionCounter::HasDone(100, 0));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Compact")
  {
    WD_TEST_BOOL(wdConstructionCounter::HasAllDestructed());

    wdHybridArray<wdConstructionCounter, 16> a;

    WD_TEST_BOOL(wdConstructionCounter::HasDone(0, 0)); // nothing has been constructed / destructed in between
    WD_TEST_BOOL(wdConstructionCounter::HasAllDestructed());

    a.SetCount(100);
    WD_TEST_BOOL(wdConstructionCounter::HasDone(100, 0));

    a.SetCount(200);
    WD_TEST_BOOL(wdConstructionCounter::HasDone(200, 100));

    a.SetCount(10);
    WD_TEST_BOOL(wdConstructionCounter::HasDone(0, 190));

    // no reallocations and copying, if the memory is already available
    a.SetCount(200);
    WD_TEST_BOOL(wdConstructionCounter::HasDone(190, 0));

    a.SetCount(10);
    WD_TEST_BOOL(wdConstructionCounter::HasDone(0, 190));

    // now we remove the spare memory
    a.Compact();
    WD_TEST_BOOL(wdConstructionCounter::HasDone(10, 10));

    // this time the array needs to be relocated, and thus the already present elements need to be copied
    a.SetCount(200);
    WD_TEST_BOOL(wdConstructionCounter::HasDone(200, 10));

    // this does not deallocate memory
    a.Clear();
    WD_TEST_BOOL(wdConstructionCounter::HasDone(0, 200));

    a.SetCount(100);
    WD_TEST_BOOL(wdConstructionCounter::HasDone(100, 0));

    // therefore no object relocation
    a.SetCount(200);
    WD_TEST_BOOL(wdConstructionCounter::HasDone(100, 0));

    a.Clear();
    WD_TEST_BOOL(wdConstructionCounter::HasDone(0, 200));

    // this will deallocate ALL memory
    a.Compact();

    a.SetCount(100);
    WD_TEST_BOOL(wdConstructionCounter::HasDone(100, 0));

    // this time objects need to be relocated
    a.SetCount(200);
    WD_TEST_BOOL(wdConstructionCounter::HasDone(200, 100));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "STL Iterator")
  {
    wdHybridArray<wdInt32, 16> a1;

    for (wdInt32 i = 0; i < 1000; ++i)
      a1.PushBack(1000 - i - 1);

    // STL sort
    std::sort(begin(a1), end(a1));

    for (wdInt32 i = 1; i < 1000; ++i)
    {
      WD_TEST_BOOL(a1[i - 1] <= a1[i]);
    }

    // foreach
    wdUInt32 prev = 0;
    for (wdUInt32 val : a1)
    {
      WD_TEST_BOOL(prev <= val);
      prev = val;
    }

    // const array
    const wdHybridArray<wdInt32, 16>& a2 = a1;

    // STL lower bound
    auto lb = std::lower_bound(begin(a2), end(a2), 400);
    WD_TEST_BOOL(*lb == a2[400]);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "STL Reverse Iterator")
  {
    wdHybridArray<wdInt32, 16> a1;

    for (wdInt32 i = 0; i < 1000; ++i)
      a1.PushBack(1000 - i - 1);

    // STL sort
    std::sort(rbegin(a1), rend(a1));

    for (wdInt32 i = 1; i < 1000; ++i)
    {
      WD_TEST_BOOL(a1[i - 1] >= a1[i]);
    }

    // foreach
    wdUInt32 prev = 1000;
    for (wdUInt32 val : a1)
    {
      WD_TEST_BOOL(prev >= val);
      prev = val;
    }

    // const array
    const wdHybridArray<wdInt32, 16>& a2 = a1;

    // STL lower bound
    auto lb = std::lower_bound(rbegin(a2), rend(a2), 400);
    WD_TEST_BOOL(*lb == a2[1000 - 400 - 1]);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Swap")
  {

    wdInt32 content1[] = {1, 2, 3, 4};
    wdInt32 content2[] = {5, 6, 7, 8, 9};
    wdInt32 contentHeap1[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
    wdInt32 contentHeap2[] = {11, 12, 13, 14, 15, 16, 17, 18, 19, 110, 111, 112, 113};

    {
      // local <-> local
      wdHybridArray<wdInt32, 8> a1;
      wdHybridArray<wdInt32, 16> a2;
      a1 = wdMakeArrayPtr(content1);
      a2 = wdMakeArrayPtr(content2);

      wdInt32* a1Ptr = a1.GetData();
      wdInt32* a2Ptr = a2.GetData();

      a1.Swap(a2);

      // Because the data points to the internal storage the pointers shouldn't change when swapping
      WD_TEST_BOOL(a1Ptr == a1.GetData());
      WD_TEST_BOOL(a2Ptr == a2.GetData());

      // The data however should be swapped
      WD_TEST_BOOL(a1.GetArrayPtr() == wdMakeArrayPtr(content2));
      WD_TEST_BOOL(a2.GetArrayPtr() == wdMakeArrayPtr(content1));

      WD_TEST_INT(a1.GetCapacity(), 8);
      WD_TEST_INT(a2.GetCapacity(), 16);
    }

    {
      // local <-> heap
      wdHybridArray<wdInt32, 8> a1;
      wdDynamicArray<wdInt32> a2;
      a1 = wdMakeArrayPtr(content1);
      a2 = wdMakeArrayPtr(contentHeap1);
      wdInt32* a1Ptr = a1.GetData();
      wdInt32* a2Ptr = a2.GetData();
      a1.Swap(a2);
      WD_TEST_BOOL(a1Ptr != a1.GetData());
      WD_TEST_BOOL(a2Ptr != a2.GetData());
      WD_TEST_BOOL(a1.GetArrayPtr() == wdMakeArrayPtr(contentHeap1));
      WD_TEST_BOOL(a2.GetArrayPtr() == wdMakeArrayPtr(content1));

      WD_TEST_INT(a1.GetCapacity(), 16);
      WD_TEST_INT(a2.GetCapacity(), 16);
    }

    {
      // heap <-> local
      wdHybridArray<wdInt32, 8> a1;
      wdHybridArray<wdInt32, 7> a2;
      a1 = wdMakeArrayPtr(content1);
      a2 = wdMakeArrayPtr(contentHeap1);
      wdInt32* a1Ptr = a1.GetData();
      wdInt32* a2Ptr = a2.GetData();
      a2.Swap(a1); // Swap is opposite direction as before
      WD_TEST_BOOL(a1Ptr != a1.GetData());
      WD_TEST_BOOL(a2Ptr != a2.GetData());
      WD_TEST_BOOL(a1.GetArrayPtr() == wdMakeArrayPtr(contentHeap1));
      WD_TEST_BOOL(a2.GetArrayPtr() == wdMakeArrayPtr(content1));

      WD_TEST_INT(a1.GetCapacity(), 16);
      WD_TEST_INT(a2.GetCapacity(), 16);
    }

    {
      // heap <-> heap
      wdDynamicArray<wdInt32> a1;
      wdHybridArray<wdInt32, 8> a2;
      a1 = wdMakeArrayPtr(contentHeap1);
      a2 = wdMakeArrayPtr(contentHeap2);
      wdInt32* a1Ptr = a1.GetData();
      wdInt32* a2Ptr = a2.GetData();
      a2.Swap(a1);
      WD_TEST_BOOL(a1Ptr != a1.GetData());
      WD_TEST_BOOL(a2Ptr != a2.GetData());
      WD_TEST_BOOL(a1.GetArrayPtr() == wdMakeArrayPtr(contentHeap2));
      WD_TEST_BOOL(a2.GetArrayPtr() == wdMakeArrayPtr(contentHeap1));

      WD_TEST_INT(a1.GetCapacity(), 16);
      WD_TEST_INT(a2.GetCapacity(), 16);
    }

    {
      // empty <-> local
      wdHybridArray<wdInt32, 8> a1, a2;
      a2 = wdMakeArrayPtr(content2);
      a1.Swap(a2);
      WD_TEST_BOOL(a1.GetArrayPtr() == wdMakeArrayPtr(content2));
      WD_TEST_BOOL(a2.IsEmpty());

      WD_TEST_INT(a1.GetCapacity(), 8);
      WD_TEST_INT(a2.GetCapacity(), 8);
    }

    {
      // empty <-> empty
      wdHybridArray<wdInt32, 8> a1, a2;
      a1.Swap(a2);
      WD_TEST_BOOL(a1.IsEmpty());
      WD_TEST_BOOL(a2.IsEmpty());

      WD_TEST_INT(a1.GetCapacity(), 8);
      WD_TEST_INT(a2.GetCapacity(), 8);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Move")
  {
    int counter = 0;
    {
      wdHybridArray<HybridArrayTestDetail::ExternalCounter, 2> a, b;
      WD_TEST_BOOL(counter == 0);

      a.PushBack(HybridArrayTestDetail::ExternalCounter(counter));
      WD_TEST_BOOL(counter == 1);

      b = std::move(a);
      WD_TEST_BOOL(counter == 1);
    }
    WD_TEST_BOOL(counter == 2);

    counter = 0;
    {
      wdHybridArray<HybridArrayTestDetail::ExternalCounter, 2> a, b;
      WD_TEST_BOOL(counter == 0);

      a.PushBack(HybridArrayTestDetail::ExternalCounter(counter));
      a.PushBack(HybridArrayTestDetail::ExternalCounter(counter));
      a.PushBack(HybridArrayTestDetail::ExternalCounter(counter));
      a.PushBack(HybridArrayTestDetail::ExternalCounter(counter));
      WD_TEST_BOOL(counter == 4);

      b = std::move(a);
      WD_TEST_BOOL(counter == 4);
    }
    WD_TEST_BOOL(counter == 8);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Use wdHybridArray with wdDynamicArray")
  {
    wdHybridArray<int, 16> a;

    TakesDynamicArray(a, 4, a.GetCount());
    WD_TEST_INT(a.GetCount(), 4);
    WD_TEST_INT(a.GetCapacity(), 16);

    for (int i = 0; i < (int)a.GetCount(); ++i)
    {
      WD_TEST_INT(a[i], i);
    }

    TakesDynamicArray(a, 12, a.GetCount());
    WD_TEST_INT(a.GetCount(), 16);
    WD_TEST_INT(a.GetCapacity(), 16);

    for (int i = 0; i < (int)a.GetCount(); ++i)
    {
      WD_TEST_INT(a[i], i);
    }

    TakesDynamicArray(a, 8, a.GetCount());
    WD_TEST_INT(a.GetCount(), 24);
    WD_TEST_INT(a.GetCapacity(), 32);

    for (int i = 0; i < (int)a.GetCount(); ++i)
    {
      WD_TEST_INT(a[i], i);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Nested arrays")
  {
    wdDynamicArray<wdHybridArray<HybridArrayTestDetail::NonMovableClass, 4>> a;

    for (int i = 0; i < 100; ++i)
    {
      wdHybridArray<HybridArrayTestDetail::NonMovableClass, 4> b;
      b.PushBack(HybridArrayTestDetail::NonMovableClass(i));

      a.PushBack(std::move(b));
    }

    for (int i = 0; i < 100; ++i)
    {
      auto& nonMoveable = a[i][0];

      WD_TEST_INT(nonMoveable.m_val, i);
      WD_TEST_BOOL(nonMoveable.m_pVal == &nonMoveable.m_val);
    }
  }
}

void TakesDynamicArray(wdDynamicArray<int>& ref_ar, int iNum, int iStart)
{
  for (int i = 0; i < iNum; ++i)
  {
    ref_ar.PushBack(iStart + i);
  }
}
