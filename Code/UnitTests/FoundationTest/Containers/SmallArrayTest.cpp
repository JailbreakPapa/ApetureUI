#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Containers/SmallArray.h>
#include <Foundation/Memory/CommonAllocators.h>
#include <Foundation/Strings/String.h>

namespace SmallArrayTestDetail
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
  static wdSmallArray<T, 16> CreateArray(wdUInt32 uiSize, wdUInt32 uiOffset, wdUInt32 uiUserData)
  {
    wdSmallArray<T, 16> a;
    a.SetCount(static_cast<wdUInt16>(uiSize));

    for (wdUInt32 i = 0; i < uiSize; ++i)
    {
      a[i] = T(uiOffset + i);
    }

    a.template GetUserData<wdUInt32>() = uiUserData;

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
} // namespace SmallArrayTestDetail

static void TakesDynamicArray(wdDynamicArray<int>& ref_ar, int iNum, int iStart);

#if WD_ENABLED(WD_PLATFORM_64BIT)
static_assert(sizeof(wdSmallArray<wdInt32, 1>) == 16);
#else
static_assert(sizeof(wdSmallArray<wdInt32, 1>) == 12);
#endif

static_assert(wdGetTypeClass<wdSmallArray<wdInt32, 1>>::value == wdTypeIsMemRelocatable::value);
static_assert(wdGetTypeClass<wdSmallArray<SmallArrayTestDetail::NonMovableClass, 1>>::value == wdTypeIsClass::value);

WD_CREATE_SIMPLE_TEST(Containers, SmallArray)
{
  wdConstructionCounter::Reset();

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor")
  {
    wdSmallArray<wdInt32, 16> a1;
    wdSmallArray<wdConstructionCounter, 16> a2;

    WD_TEST_BOOL(a1.GetCount() == 0);
    WD_TEST_BOOL(a2.GetCount() == 0);
    WD_TEST_BOOL(a1.IsEmpty());
    WD_TEST_BOOL(a2.IsEmpty());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Copy Constructor")
  {
    wdSmallArray<wdInt32, 16> a1;

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

    a1.GetUserData<wdUInt32>() = 11;

    wdSmallArray<wdInt32, 16> a2 = a1;
    wdSmallArray<wdInt32, 16> a3(a1);

    WD_TEST_BOOL(a1 == a2);
    WD_TEST_BOOL(a1 == a3);
    WD_TEST_BOOL(a2 == a3);

    WD_TEST_INT(a2.GetUserData<wdUInt32>(), 11);
    WD_TEST_INT(a3.GetUserData<wdUInt32>(), 11);

    wdInt32 test[] = {1, 2, 3, 4};
    wdArrayPtr<wdInt32> aptr(test);

    wdSmallArray<wdInt32, 16> a4(aptr);

    WD_TEST_BOOL(a4 == aptr);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Move Constructor / Operator")
  {
    WD_TEST_BOOL(wdConstructionCounter::HasAllDestructed());

    {
      // move constructor external storage
      wdSmallArray<wdConstructionCounter, 16> a1(SmallArrayTestDetail::CreateArray<wdConstructionCounter>(100, 20, 11));

      WD_TEST_INT(a1.GetCount(), 100);
      for (wdUInt32 i = 0; i < a1.GetCount(); ++i)
        WD_TEST_INT(a1[i].m_iData, 20 + i);

      WD_TEST_INT(a1.GetUserData<wdUInt32>(), 11);

      // move operator external storage
      a1 = SmallArrayTestDetail::CreateArray<wdConstructionCounter>(200, 50, 22);

      WD_TEST_INT(a1.GetCount(), 200);
      for (wdUInt32 i = 0; i < a1.GetCount(); ++i)
        WD_TEST_INT(a1[i].m_iData, 50 + i);

      WD_TEST_INT(a1.GetUserData<wdUInt32>(), 22);
    }

    WD_TEST_BOOL(wdConstructionCounter::HasAllDestructed());
    wdConstructionCounter::Reset();

    {
      // move constructor internal storage
      wdSmallArray<wdConstructionCounter, 16> a2(SmallArrayTestDetail::CreateArray<wdConstructionCounter>(10, 30, 11));

      WD_TEST_INT(a2.GetCount(), 10);
      for (wdUInt32 i = 0; i < a2.GetCount(); ++i)
        WD_TEST_INT(a2[i].m_iData, 30 + i);

      WD_TEST_INT(a2.GetUserData<wdUInt32>(), 11);

      // move operator internal storage
      a2 = SmallArrayTestDetail::CreateArray<wdConstructionCounter>(8, 70, 22);

      WD_TEST_INT(a2.GetCount(), 8);
      for (wdUInt32 i = 0; i < a2.GetCount(); ++i)
        WD_TEST_INT(a2[i].m_iData, 70 + i);

      WD_TEST_INT(a2.GetUserData<wdUInt32>(), 22);
    }

    WD_TEST_BOOL(wdConstructionCounter::HasAllDestructed());
    wdConstructionCounter::Reset();

    wdConstructionCounterRelocatable::Reset();
    {
      // move constructor external storage relocatable
      wdSmallArray<wdConstructionCounterRelocatable, 16> a1(SmallArrayTestDetail::CreateArray<wdConstructionCounterRelocatable>(100, 20, 11));

      WD_TEST_BOOL(wdConstructionCounterRelocatable::HasDone(100, 0));

      WD_TEST_INT(a1.GetCount(), 100);
      for (wdUInt32 i = 0; i < a1.GetCount(); ++i)
        WD_TEST_INT(a1[i].m_iData, 20 + i);

      WD_TEST_INT(a1.GetUserData<wdUInt32>(), 11);

      // move operator external storage
      a1 = SmallArrayTestDetail::CreateArray<wdConstructionCounterRelocatable>(200, 50, 22);
      WD_TEST_BOOL(wdConstructionCounterRelocatable::HasDone(200, 100));

      WD_TEST_INT(a1.GetCount(), 200);
      for (wdUInt32 i = 0; i < a1.GetCount(); ++i)
        WD_TEST_INT(a1[i].m_iData, 50 + i);

      WD_TEST_INT(a1.GetUserData<wdUInt32>(), 22);
    }

    WD_TEST_BOOL(wdConstructionCounterRelocatable::HasAllDestructed());
    wdConstructionCounterRelocatable::Reset();

    {
      // move constructor internal storage relocatable
      wdSmallArray<wdConstructionCounterRelocatable, 16> a2(SmallArrayTestDetail::CreateArray<wdConstructionCounterRelocatable>(10, 30, 11));
      WD_TEST_BOOL(wdConstructionCounterRelocatable::HasDone(10, 0));

      WD_TEST_INT(a2.GetCount(), 10);
      for (wdUInt32 i = 0; i < a2.GetCount(); ++i)
        WD_TEST_INT(a2[i].m_iData, 30 + i);

      WD_TEST_INT(a2.GetUserData<wdUInt32>(), 11);

      // move operator internal storage
      a2 = SmallArrayTestDetail::CreateArray<wdConstructionCounterRelocatable>(8, 70, 22);
      WD_TEST_BOOL(wdConstructionCounterRelocatable::HasDone(8, 10));

      WD_TEST_INT(a2.GetCount(), 8);
      for (wdUInt32 i = 0; i < a2.GetCount(); ++i)
        WD_TEST_INT(a2[i].m_iData, 70 + i);

      WD_TEST_INT(a2.GetUserData<wdUInt32>(), 22);
    }

    WD_TEST_BOOL(wdConstructionCounterRelocatable::HasAllDestructed());
    wdConstructionCounterRelocatable::Reset();
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Convert to ArrayPtr")
  {
    wdSmallArray<wdInt32, 16> a1;

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
    wdSmallArray<wdInt32, 16> a1, a2;

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
    wdSmallArray<wdInt32, 16> a1, a2;

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
    wdSmallArray<wdInt32, 16> a1;
    a1.SetCountUninitialized(100);

    for (wdInt32 i = 0; i < 100; ++i)
      a1[i] = i;

    for (wdInt32 i = 0; i < 100; ++i)
      WD_TEST_INT(a1[i], i);

    const wdSmallArray<wdInt32, 16> ca1 = a1;

    for (wdInt32 i = 0; i < 100; ++i)
      WD_TEST_INT(ca1[i], i);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetCount / GetCount / IsEmpty")
  {
    wdSmallArray<wdInt32, 16> a1;

    WD_TEST_BOOL(a1.IsEmpty());

    for (wdInt32 i = 0; i < 128; ++i)
    {
      a1.SetCount(static_cast<wdUInt16>(i + 1));
      WD_TEST_INT(a1[i], 0);
      a1[i] = i;

      WD_TEST_INT(a1.GetCount(), i + 1);
      WD_TEST_BOOL(!a1.IsEmpty());
    }

    for (wdInt32 i = 0; i < 128; ++i)
      WD_TEST_INT(a1[i], i);

    for (wdInt32 i = 128; i >= 0; --i)
    {
      a1.SetCount(static_cast<wdUInt16>(i));

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
      wdSmallArray<wdInt32, 2> a2;
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
    wdSmallArray<wdInt32, 2> a2;
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
    wdSmallArray<wdInt32, 16> a1;
    a1.Clear();

    a1.PushBack(3);
    a1.Clear();

    WD_TEST_BOOL(a1.IsEmpty());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Contains / IndexOf / LastIndexOf")
  {
    wdSmallArray<wdInt32, 16> a1;

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
    wdSmallArray<wdInt32, 16> a1;

    // always inserts at the front
    for (wdInt32 i = 0; i < 100; ++i)
      a1.Insert(i, 0);

    for (wdInt32 i = 0; i < 100; ++i)
      WD_TEST_INT(a1[i], 99 - i);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "RemoveAndCopy")
  {
    wdSmallArray<wdInt32, 16> a1;

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
    wdSmallArray<wdInt32, 16> a1;

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
    wdSmallArray<wdInt32, 16> a1;

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
    wdSmallArray<wdInt32, 16> a1;

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
    wdSmallArray<wdInt32, 16> a1;

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
    wdSmallArray<wdInt32, 16> a1;

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

      wdSmallArray<wdConstructionCounter, 16> a1;
      wdSmallArray<wdConstructionCounter, 16> a2;

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
    wdSmallArray<wdInt32, 16> a;

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

    // this tests whether the static array is reused properly
    a.SetCount(15);
    a.Compact();
    WD_TEST_BOOL(a.GetHeapMemoryUsage() == 0);

    for (wdInt32 i = 0; i < 15; ++i)
      WD_TEST_INT(a[i], i);

    a.Clear();
    a.Compact();
    WD_TEST_BOOL(a.GetHeapMemoryUsage() == 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SortingPrimitives")
  {
    wdSmallArray<wdUInt32, 16> list;

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
    wdSmallArray<SmallArrayTestDetail::Dummy, 16> list;
    list.Reserve(128);

    for (wdUInt32 i = 0; i < 100; i++)
    {
      list.PushBack(SmallArrayTestDetail::Dummy(rand()));
    }
    list.Sort();

    SmallArrayTestDetail::Dummy last = 0;
    for (wdUInt32 i = 0; i < list.GetCount(); i++)
    {
      WD_TEST_BOOL(last <= list[i]);
      last = list[i];
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Various")
  {
    wdSmallArray<SmallArrayTestDetail::Dummy, 16> list;
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
    SmallArrayTestDetail::Dummy d = list.PeekBack();
    list.PopBack();
    WD_TEST_BOOL(d.a == 5);
    WD_TEST_BOOL(list.GetCount() == 4);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Assignment")
  {
    wdSmallArray<SmallArrayTestDetail::Dummy, 16> list;
    for (int i = 0; i < 16; i++)
    {
      list.PushBack(SmallArrayTestDetail::Dummy(rand()));
    }
    list.GetUserData<wdUInt32>() = 11;

    wdSmallArray<SmallArrayTestDetail::Dummy, 16> list2;
    for (int i = 0; i < 8; i++)
    {
      list2.PushBack(SmallArrayTestDetail::Dummy(rand()));
    }
    list2.GetUserData<wdUInt32>() = 22;

    list = list2;
    WD_TEST_INT(list.GetCount(), list2.GetCount());
    WD_TEST_INT(list.GetUserData<wdUInt32>(), list2.GetUserData<wdUInt32>());

    list2.Clear();
    WD_TEST_BOOL(list2.GetCount() == 0);

    list2 = list;
    WD_TEST_BOOL(list.PeekBack() == list2.PeekBack());
    WD_TEST_BOOL(list == list2);

    for (int i = 0; i < 16; i++)
    {
      list2.PushBack(SmallArrayTestDetail::Dummy(rand()));
    }

    list = list2;
    WD_TEST_BOOL(list.PeekBack() == list2.PeekBack());
    WD_TEST_BOOL(list == list2);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Count")
  {
    wdSmallArray<SmallArrayTestDetail::Dummy, 16> list;
    for (int i = 0; i < 16; i++)
    {
      list.PushBack(SmallArrayTestDetail::Dummy(rand()));
    }
    list.SetCount(32);
    list.SetCount(4);

    list.Compact();
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Reserve")
  {
    WD_TEST_BOOL(wdConstructionCounter::HasAllDestructed());

    wdSmallArray<wdConstructionCounter, 16> a;

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

    wdSmallArray<wdConstructionCounter, 16> a;

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

    a.SetCount(10);
    WD_TEST_BOOL(wdConstructionCounter::HasDone(10, 0));

    // this time objects need to be relocated
    a.SetCount(200);
    WD_TEST_BOOL(wdConstructionCounter::HasDone(200, 10));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "STL Iterator")
  {
    wdSmallArray<wdInt32, 16> a1;

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
    const wdSmallArray<wdInt32, 16>& a2 = a1;

    // STL lower bound
    auto lb = std::lower_bound(begin(a2), end(a2), 400);
    WD_TEST_BOOL(*lb == a2[400]);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "STL Reverse Iterator")
  {
    wdSmallArray<wdInt32, 16> a1;

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
    const wdSmallArray<wdInt32, 16>& a2 = a1;

    // STL lower bound
    auto lb = std::lower_bound(rbegin(a2), rend(a2), 400);
    WD_TEST_BOOL(*lb == a2[1000 - 400 - 1]);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Move")
  {
    int counter = 0;
    {
      wdSmallArray<SmallArrayTestDetail::ExternalCounter, 2> a, b;
      WD_TEST_BOOL(counter == 0);

      a.PushBack(SmallArrayTestDetail::ExternalCounter(counter));
      WD_TEST_BOOL(counter == 1);

      b = std::move(a);
      WD_TEST_BOOL(counter == 1);
    }
    WD_TEST_BOOL(counter == 2);

    counter = 0;
    {
      wdSmallArray<SmallArrayTestDetail::ExternalCounter, 2> a, b;
      WD_TEST_BOOL(counter == 0);

      a.PushBack(SmallArrayTestDetail::ExternalCounter(counter));
      a.PushBack(SmallArrayTestDetail::ExternalCounter(counter));
      a.PushBack(SmallArrayTestDetail::ExternalCounter(counter));
      a.PushBack(SmallArrayTestDetail::ExternalCounter(counter));
      WD_TEST_BOOL(counter == 4);

      b = std::move(a);
      WD_TEST_BOOL(counter == 4);
    }
    WD_TEST_BOOL(counter == 8);
  }
}
