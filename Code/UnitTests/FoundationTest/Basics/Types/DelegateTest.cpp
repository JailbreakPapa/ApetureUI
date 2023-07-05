#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Types/Delegate.h>
#include <Foundation/Types/RefCounted.h>
#include <Foundation/Types/SharedPtr.h>

namespace
{
  struct TestType
  {
    TestType() {}

    wdInt32 MethodWithManyParams(wdInt32 a, wdInt32 b, wdInt32 c, wdInt32 d, wdInt32 e, wdInt32 f) { return m_iA + a + b + c + d + e + f; }

    wdInt32 Method(wdInt32 b) { return b + m_iA; }

    wdInt32 ConstMethod(wdInt32 b) const { return b + m_iA + 4; }

    virtual wdInt32 VirtualMethod(wdInt32 b) { return b; }

    mutable wdInt32 m_iA;
  };

  struct TestTypeDerived : public TestType
  {
    wdInt32 Method(wdInt32 b) { return b + 4; }

    virtual wdInt32 VirtualMethod(wdInt32 b) override { return b + 43; }
  };

  struct BaseA
  {
    virtual ~BaseA() {}
    virtual void bar() {}

    int m_i1;
  };

  struct BaseB
  {
    virtual ~BaseB() {}
    virtual void foo() {}
    int m_i2;
  };

  struct ComplexClass : public BaseA, public BaseB
  {
    ComplexClass() { m_ctorDel = wdMakeDelegate(&ComplexClass::nonVirtualFunc, this); }

    virtual ~ComplexClass()
    {
      m_dtorDel = wdMakeDelegate(&ComplexClass::nonVirtualFunc, this);
      WD_TEST_BOOL(m_ctorDel.IsEqualIfComparable(m_dtorDel));
    }
    virtual void bar() override {}
    virtual void foo() override {}



    void nonVirtualFunc()
    {
      m_i1 = 1;
      m_i2 = 2;
      m_i3 = 3;
    }

    int m_i3;

    wdDelegate<void()> m_ctorDel;
    wdDelegate<void()> m_dtorDel;
  };

  static wdInt32 Function(wdInt32 b) { return b + 2; }
} // namespace

WD_CREATE_SIMPLE_TEST(Basics, Delegate)
{
  typedef wdDelegate<wdInt32(wdInt32)> TestDelegate;
  TestDelegate d;

#if WD_ENABLED(WD_PLATFORM_64BIT)
  WD_TEST_BOOL(sizeof(d) == 32);
#endif

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Method")
  {
    TestTypeDerived test;
    test.m_iA = 42;

    d = TestDelegate(&TestType::Method, &test);
    WD_TEST_BOOL(d.IsEqualIfComparable(TestDelegate(&TestType::Method, &test)));
    WD_TEST_BOOL(d.IsComparable());
    WD_TEST_INT(d(4), 46);

    d = TestDelegate(&TestTypeDerived::Method, &test);
    WD_TEST_BOOL(d.IsEqualIfComparable(TestDelegate(&TestTypeDerived::Method, &test)));
    WD_TEST_BOOL(d.IsComparable());
    WD_TEST_INT(d(4), 8);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Method With Many Params")
  {
    typedef wdDelegate<wdInt32(wdInt32, wdInt32, wdInt32, wdInt32, wdInt32, wdInt32)> TestDelegateMany;
    TestDelegateMany many;

    TestType test;
    test.m_iA = 1000000;

    many = TestDelegateMany(&TestType::MethodWithManyParams, &test);
    WD_TEST_BOOL(many.IsEqualIfComparable(TestDelegateMany(&TestType::MethodWithManyParams, &test)));
    WD_TEST_BOOL(d.IsComparable());
    WD_TEST_INT(many(1, 10, 100, 1000, 10000, 100000), 1111111);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Complex Class")
  {
    ComplexClass* c = new ComplexClass();
    delete c;
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Const Method")
  {
    const TestType constTest;
    constTest.m_iA = 35;

    d = TestDelegate(&TestType::ConstMethod, &constTest);
    WD_TEST_BOOL(d.IsEqualIfComparable(TestDelegate(&TestType::ConstMethod, &constTest)));
    WD_TEST_BOOL(d.IsComparable());
    WD_TEST_INT(d(4), 43);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Virtual Method")
  {
    TestTypeDerived test;

    d = TestDelegate(&TestType::VirtualMethod, &test);
    WD_TEST_BOOL(d.IsEqualIfComparable(TestDelegate(&TestType::VirtualMethod, &test)));
    WD_TEST_BOOL(d.IsComparable());
    WD_TEST_INT(d(4), 47);

    d = TestDelegate(&TestTypeDerived::VirtualMethod, &test);
    WD_TEST_BOOL(d.IsEqualIfComparable(TestDelegate(&TestTypeDerived::VirtualMethod, &test)));
    WD_TEST_BOOL(d.IsComparable());
    WD_TEST_INT(d(4), 47);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Function")
  {
    d = &Function;
    WD_TEST_BOOL(d.IsEqualIfComparable(&Function));
    WD_TEST_BOOL(d.IsComparable());
    WD_TEST_INT(d(4), 6);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Lambda - no capture")
  {
    d = [](wdInt32 i) { return i * 4; };
    WD_TEST_BOOL(d.IsComparable());
    WD_TEST_INT(d(2), 8);

    TestDelegate d2 = d;
    WD_TEST_BOOL(d2.IsEqualIfComparable(d));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Lambda - capture by value")
  {
    wdInt32 c = 20;
    d = [c](wdInt32) { return c; };
    WD_TEST_BOOL(!d.IsComparable());
    WD_TEST_INT(d(3), 20);
    c = 10;
    WD_TEST_INT(d(3), 20);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Lambda - capture by value, mutable")
  {
    wdInt32 c = 20;
    d = [c](wdInt32) mutable { return c; };
    WD_TEST_BOOL(!d.IsComparable());
    WD_TEST_INT(d(3), 20);
    c = 10;
    WD_TEST_INT(d(3), 20);

    d = [c](wdInt32 b) mutable -> decltype(b + c) {
      auto result = b + c;
      c = 1;
      return result;
    };
    WD_TEST_BOOL(!d.IsComparable());
    WD_TEST_INT(d(3), 13);
    WD_TEST_INT(d(3), 4);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Lambda - capture by reference")
  {
    wdInt32 c = 20;
    d = [&c](wdInt32 i) -> decltype(i) {
      c = 5;
      return i;
    };
    WD_TEST_BOOL(!d.IsComparable());
    WD_TEST_INT(d(3), 3);
    WD_TEST_INT(c, 5);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Lambda - capture by value of non-pod")
  {
    struct RefCountedInt : public wdRefCounted
    {
      RefCountedInt() = default;
      RefCountedInt(int i)
        : m_value(i)
      {
      }
      int m_value;
    };

    wdSharedPtr<RefCountedInt> shared = WD_DEFAULT_NEW(RefCountedInt, 1);
    WD_TEST_INT(shared->GetRefCount(), 1);
    {
      TestDelegate deleteMe = [shared](wdInt32 i) -> decltype(i) { return 0; };
      WD_TEST_BOOL(!deleteMe.IsComparable());
      WD_TEST_INT(shared->GetRefCount(), 2);
    }
    WD_TEST_INT(shared->GetRefCount(), 1);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Lambda - capture lots of things")
  {
    wdInt64 a = 10;
    wdInt64 b = 20;
    wdInt64 c = 30;
    d = [a, b, c](wdInt32 i) -> wdInt32 { return static_cast<wdInt32>(a + b + c + i); };
    WD_TEST_INT(d(6), 66);
    WD_TEST_BOOL(!d.IsComparable());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Lambda - capture lots of things - custom allocator")
  {
    wdInt64 a = 10;
    wdInt64 b = 20;
    wdInt64 c = 30;
    d = TestDelegate([a, b, c](wdInt32 i) -> wdInt32 { return static_cast<wdInt32>(a + b + c + i); }, wdFoundation::GetAlignedAllocator());
    WD_TEST_INT(d(6), 66);
    WD_TEST_BOOL(!d.IsComparable());

    d.Invalidate();
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Move semantics")
  {
    // Move pure function
    {
      d.Invalidate();
      TestDelegate d2 = &Function;
      d = std::move(d2);
      WD_TEST_BOOL(d.IsValid());
      WD_TEST_BOOL(!d2.IsValid());
      WD_TEST_BOOL(d.IsComparable());
      WD_TEST_INT(d(4), 6);
    }

    // Move delegate
    wdConstructionCounter::Reset();
    d.Invalidate();
    {
      wdConstructionCounter value;
      value.m_iData = 666;
      WD_TEST_INT(wdConstructionCounter::s_iConstructions, 1);
      WD_TEST_INT(wdConstructionCounter::s_iDestructions, 0);
      TestDelegate d2 = [value](wdInt32 i) -> wdInt32 { return value.m_iData; };
      WD_TEST_INT(wdConstructionCounter::s_iConstructions, 3); // Capture plus moving the lambda.
      WD_TEST_INT(wdConstructionCounter::s_iDestructions, 1);  // Move of lambda
      d = std::move(d2);
      // Moving a construction counter also counts as construction
      WD_TEST_INT(wdConstructionCounter::s_iConstructions, 4);
      WD_TEST_INT(wdConstructionCounter::s_iDestructions, 1);
      WD_TEST_BOOL(d.IsValid());
      WD_TEST_BOOL(!d2.IsValid());
      WD_TEST_BOOL(!d.IsComparable());
      WD_TEST_INT(d(0), 666);
    }
    WD_TEST_INT(wdConstructionCounter::s_iDestructions, 2); // value out of scope
    WD_TEST_INT(wdConstructionCounter::s_iConstructions, 4);
    d.Invalidate();
    WD_TEST_INT(wdConstructionCounter::s_iDestructions, 3); // lambda destroyed.
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Lambda - Copy")
  {
    d.Invalidate();
    wdConstructionCounter::Reset();
    {
      wdConstructionCounter value;
      value.m_iData = 666;
      WD_TEST_INT(wdConstructionCounter::s_iConstructions, 1);
      WD_TEST_INT(wdConstructionCounter::s_iDestructions, 0);
      TestDelegate d2 = TestDelegate([value](wdInt32 i) -> wdInt32 { return value.m_iData; }, wdFoundation::GetAlignedAllocator());
      WD_TEST_INT(wdConstructionCounter::s_iConstructions, 3); // Capture plus moving the lambda.
      WD_TEST_INT(wdConstructionCounter::s_iDestructions, 1);  // Move of lambda
      d = d2;
      WD_TEST_INT(wdConstructionCounter::s_iConstructions, 4); // Lambda Copy
      WD_TEST_INT(wdConstructionCounter::s_iDestructions, 1);
      WD_TEST_BOOL(d.IsValid());
      WD_TEST_BOOL(d2.IsValid());
      WD_TEST_BOOL(!d.IsComparable());
      WD_TEST_BOOL(!d2.IsComparable());
      WD_TEST_INT(d(0), 666);
      WD_TEST_INT(d2(0), 666);
    }
    WD_TEST_INT(wdConstructionCounter::s_iDestructions, 3); // value and lambda out of scope
    WD_TEST_INT(wdConstructionCounter::s_iConstructions, 4);
    d.Invalidate();
    WD_TEST_INT(wdConstructionCounter::s_iDestructions, 4); // lambda destroyed.
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Lambda - capture non-copyable type")
  {
    wdUniquePtr<wdConstructionCounter> data(WD_DEFAULT_NEW(wdConstructionCounter));
    data->m_iData = 666;
    TestDelegate d2 = [data = std::move(data)](wdInt32 i) -> wdInt32 { return data->m_iData; };
    WD_TEST_INT(d2(0), 666);
    d = std::move(d2);
    WD_TEST_BOOL(d.IsValid());
    WD_TEST_BOOL(!d2.IsValid());
    WD_TEST_INT(d(0), 666);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdMakeDelegate")
  {
    auto d1 = wdMakeDelegate(&Function);
    WD_TEST_BOOL(d1.IsEqualIfComparable(wdMakeDelegate(&Function)));

    TestType instance;
    auto d2 = wdMakeDelegate(&TestType::Method, &instance);
    WD_TEST_BOOL(d2.IsEqualIfComparable(wdMakeDelegate(&TestType::Method, &instance)));
    auto d3 = wdMakeDelegate(&TestType::ConstMethod, &instance);
    WD_TEST_BOOL(d3.IsEqualIfComparable(wdMakeDelegate(&TestType::ConstMethod, &instance)));
    auto d4 = wdMakeDelegate(&TestType::VirtualMethod, &instance);
    WD_TEST_BOOL(d4.IsEqualIfComparable(wdMakeDelegate(&TestType::VirtualMethod, &instance)));

    TestType instance2;
    auto d2_2 = wdMakeDelegate(&TestType::Method, &instance2);
    WD_TEST_BOOL(!d2_2.IsEqualIfComparable(d2));

    WD_IGNORE_UNUSED(d1);
    WD_IGNORE_UNUSED(d2);
    WD_IGNORE_UNUSED(d2_2);
    WD_IGNORE_UNUSED(d3);
    WD_IGNORE_UNUSED(d4);
  }
}
