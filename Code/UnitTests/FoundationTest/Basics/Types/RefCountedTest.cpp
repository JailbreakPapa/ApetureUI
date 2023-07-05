#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Types/RefCounted.h>

class RefCountedTestClass : public wdRefCounted
{
public:
  wdUInt32 m_uiDummyMember = 0x42u;
};

WD_CREATE_SIMPLE_TEST(Basics, RefCounted)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Ref Counting")
  {
    RefCountedTestClass Instance;

    WD_TEST_BOOL(Instance.GetRefCount() == 0);
    WD_TEST_BOOL(!Instance.IsReferenced());

    Instance.AddRef();

    WD_TEST_BOOL(Instance.GetRefCount() == 1);
    WD_TEST_BOOL(Instance.IsReferenced());

    /// Test scoped ref pointer
    {
      wdScopedRefPointer<RefCountedTestClass> ScopeTester(&Instance);

      WD_TEST_BOOL(Instance.GetRefCount() == 2);
      WD_TEST_BOOL(Instance.IsReferenced());
    }

    /// Test assignment of scoped ref pointer
    {
      wdScopedRefPointer<RefCountedTestClass> ScopeTester;

      ScopeTester = &Instance;

      WD_TEST_BOOL(Instance.GetRefCount() == 2);
      WD_TEST_BOOL(Instance.IsReferenced());

      wdScopedRefPointer<RefCountedTestClass> ScopeTester2;

      ScopeTester2 = ScopeTester;

      WD_TEST_BOOL(Instance.GetRefCount() == 3);
      WD_TEST_BOOL(Instance.IsReferenced());

      wdScopedRefPointer<RefCountedTestClass> ScopeTester3(ScopeTester);

      WD_TEST_BOOL(Instance.GetRefCount() == 4);
      WD_TEST_BOOL(Instance.IsReferenced());
    }

    /// Test copy constructor for wdRefCounted
    {
      RefCountedTestClass inst2(Instance);
      RefCountedTestClass inst3;
      inst3 = Instance;

      WD_TEST_BOOL(Instance.GetRefCount() == 1);
      WD_TEST_BOOL(Instance.IsReferenced());

      WD_TEST_BOOL(inst2.GetRefCount() == 0);
      WD_TEST_BOOL(!inst2.IsReferenced());

      WD_TEST_BOOL(inst3.GetRefCount() == 0);
      WD_TEST_BOOL(!inst3.IsReferenced());
    }

    WD_TEST_BOOL(Instance.GetRefCount() == 1);
    WD_TEST_BOOL(Instance.IsReferenced());

    Instance.ReleaseRef();

    WD_TEST_BOOL(Instance.GetRefCount() == 0);
    WD_TEST_BOOL(!Instance.IsReferenced());
  }
}
