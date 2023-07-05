#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Containers/StaticArray.h>

namespace StaticArrayTestDetail
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
} // namespace StaticArrayTestDetail

#if WD_ENABLED(WD_PLATFORM_64BIT)
static_assert(sizeof(wdStaticArray<wdInt32, 1>) == 24);
#else
static_assert(sizeof(wdStaticArray<wdInt32, 1>) == 16);
#endif

static_assert(wdGetTypeClass<wdStaticArray<wdInt32, 1>>::value == wdTypeIsMemRelocatable::value);
static_assert(wdGetTypeClass<wdStaticArray<StaticArrayTestDetail::Dummy, 1>>::value == wdTypeIsClass::value);

WD_CREATE_SIMPLE_TEST(Containers, StaticArray)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor")
  {
    wdStaticArray<wdInt32, 32> a1;
    wdStaticArray<wdConstructionCounter, 32> a2;

    WD_TEST_BOOL(a1.GetCount() == 0);
    WD_TEST_BOOL(a2.GetCount() == 0);
    WD_TEST_BOOL(a1.IsEmpty());
    WD_TEST_BOOL(a2.IsEmpty());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Copy Constructor")
  {
    wdStaticArray<wdInt32, 32> a1;

    for (wdInt32 i = 0; i < 32; ++i)
      a1.PushBack(rand() % 100000);

    wdStaticArray<wdInt32, 64> a2 = a1;
    wdStaticArray<wdInt32, 32> a3(a1);

    WD_TEST_BOOL(a1 == a2);
    WD_TEST_BOOL(a1 == a3);
    WD_TEST_BOOL(a2 == a3);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Convert to ArrayPtr")
  {
    wdStaticArray<wdInt32, 128> a1;

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
    wdStaticArray<wdInt32, 128> a1, a2;

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
    wdStaticArray<wdInt32, 128> a1, a2;

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
    wdStaticArray<wdInt32, 128> a1;
    a1.SetCountUninitialized(100);

    for (wdInt32 i = 0; i < 100; ++i)
      a1[i] = i;

    for (wdInt32 i = 0; i < 100; ++i)
      WD_TEST_INT(a1[i], i);

    const wdStaticArray<wdInt32, 128> ca1 = a1;

    for (wdInt32 i = 0; i < 100; ++i)
      WD_TEST_INT(ca1[i], i);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetCount / GetCount / IsEmpty")
  {
    wdStaticArray<wdInt32, 128> a1;

    WD_TEST_BOOL(a1.IsEmpty());

    for (wdInt32 i = 0; i < 128; ++i)
    {
      a1.SetCount(i + 1);
      WD_TEST_INT(a1[i], 0);
      a1[i] = i;

      WD_TEST_INT((int)a1.GetCount(), i + 1);
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
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Clear")
  {
    wdStaticArray<wdInt32, 128> a1;
    a1.Clear();

    a1.PushBack(3);
    a1.Clear();

    WD_TEST_BOOL(a1.IsEmpty());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Contains / IndexOf / LastIndexOf")
  {
    wdStaticArray<wdInt32, 128> a1;

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
    wdStaticArray<wdInt32, 128> a1;

    // always inserts at the front
    for (wdInt32 i = 0; i < 100; ++i)
      a1.Insert(i, 0);

    for (wdInt32 i = 0; i < 100; ++i)
      WD_TEST_INT(a1[i], 99 - i);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "RemoveAndCopy")
  {
    wdStaticArray<wdInt32, 128> a1;

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
    wdStaticArray<wdInt32, 128> a1;

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
    wdStaticArray<wdInt32, 128> a1;

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
    wdStaticArray<wdInt32, 128> a1;

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
    wdStaticArray<wdInt32, 128> a1;

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

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Construction / Destruction")
  {
    {
      WD_TEST_BOOL(wdConstructionCounter::HasAllDestructed());

      wdStaticArray<wdConstructionCounter, 128> a1;
      wdStaticArray<wdConstructionCounter, 100> a2;

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

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SortingPrimitives")
  {
    wdStaticArray<wdUInt32, 128> list;

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
    wdStaticArray<StaticArrayTestDetail::Dummy, 128> list;

    for (wdUInt32 i = 0; i < 100; i++)
    {
      list.PushBack(StaticArrayTestDetail::Dummy(rand()));
    }
    list.Sort();

    StaticArrayTestDetail::Dummy last = 0;
    for (wdUInt32 i = 0; i < list.GetCount(); i++)
    {
      WD_TEST_BOOL(last <= list[i]);
      last = list[i];
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Various")
  {
    wdStaticArray<StaticArrayTestDetail::Dummy, 32> list;
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
    StaticArrayTestDetail::Dummy d = list.PeekBack();
    list.PopBack();
    WD_TEST_BOOL(d.a == 5);
    WD_TEST_BOOL(list.GetCount() == 4);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Assignment")
  {
    wdStaticArray<StaticArrayTestDetail::Dummy, 32> list;
    for (int i = 0; i < 16; i++)
    {
      list.PushBack(StaticArrayTestDetail::Dummy(rand()));
    }

    wdStaticArray<StaticArrayTestDetail::Dummy, 32> list2;
    for (int i = 0; i < 8; i++)
    {
      list2.PushBack(StaticArrayTestDetail::Dummy(rand()));
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
      list2.PushBack(StaticArrayTestDetail::Dummy(rand()));
    }

    list = list2;
    WD_TEST_BOOL(list.PeekBack() == list2.PeekBack());
    WD_TEST_BOOL(list == list2);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Count")
  {
    wdStaticArray<StaticArrayTestDetail::Dummy, 32> list;
    for (int i = 0; i < 16; i++)
    {
      list.PushBack(StaticArrayTestDetail::Dummy(rand()));
    }
    list.SetCount(32);
    list.SetCount(4);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "STL Iterator")
  {
    wdStaticArray<wdInt32, 1024> a1;

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
    const wdStaticArray<wdInt32, 1024>& a2 = a1;

    // STL lower bound
    auto lb = std::lower_bound(begin(a2), end(a2), 400);
    WD_TEST_BOOL(*lb == a2[400]);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "STL Reverse Iterator")
  {
    wdStaticArray<wdInt32, 1024> a1;

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
    const wdStaticArray<wdInt32, 1024>& a2 = a1;

    // STL lower bound
    auto lb = std::lower_bound(rbegin(a2), rend(a2), 400);
    WD_TEST_BOOL(*lb == a2[1000 - 400 - 1]);
  }
}
