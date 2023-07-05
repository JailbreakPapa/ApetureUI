#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Types/PointerWithFlags.h>

WD_CREATE_SIMPLE_TEST(Basics, PointerWithFlags)
{
  struct Dummy
  {
    float a = 3.0f;
    int b = 7;
  };

  WD_TEST_BLOCK(wdTestBlock::Enabled, "General")
  {
    wdPointerWithFlags<Dummy, 2> ptr;

    WD_TEST_INT(ptr.GetFlags(), 0);
    ptr.SetFlags(3);
    WD_TEST_INT(ptr.GetFlags(), 3);

    WD_TEST_BOOL(ptr == nullptr);
    WD_TEST_BOOL(!ptr);

    WD_TEST_INT(ptr.GetFlags(), 3);
    ptr.SetFlags(2);
    WD_TEST_INT(ptr.GetFlags(), 2);

    Dummy d1, d2;
    ptr = &d1;
    d2.a = 4;
    d2.b = 8;

    WD_TEST_BOOL(ptr.GetPtr() == &d1);
    WD_TEST_BOOL(ptr.GetPtr() != &d2);

    WD_TEST_INT(ptr.GetFlags(), 2);
    ptr.SetFlags(1);
    WD_TEST_INT(ptr.GetFlags(), 1);

    WD_TEST_BOOL(ptr == &d1);
    WD_TEST_BOOL(ptr != &d2);
    WD_TEST_BOOL(ptr);


    WD_TEST_FLOAT(ptr->a, 3.0f, 0.0f);
    WD_TEST_INT(ptr->b, 7);

    ptr = &d2;

    WD_TEST_INT(ptr.GetFlags(), 1);
    ptr.SetFlags(3);
    WD_TEST_INT(ptr.GetFlags(), 3);

    WD_TEST_BOOL(ptr != &d1);
    WD_TEST_BOOL(ptr == &d2);
    WD_TEST_BOOL(ptr);

    ptr = nullptr;
    WD_TEST_BOOL(!ptr);
    WD_TEST_BOOL(ptr == nullptr);

    WD_TEST_INT(ptr.GetFlags(), 3);
    ptr.SetFlags(0);
    WD_TEST_INT(ptr.GetFlags(), 0);

    wdPointerWithFlags<Dummy, 2> ptr2 = ptr;
    WD_TEST_BOOL(ptr == ptr2);

    WD_TEST_BOOL(ptr2.GetPtr() == ptr.GetPtr());
    WD_TEST_BOOL(ptr2.GetFlags() == ptr.GetFlags());

    ptr2.SetFlags(3);
    WD_TEST_BOOL(ptr2.GetPtr() == ptr.GetPtr());
    WD_TEST_BOOL(ptr2.GetFlags() != ptr.GetFlags());

    // the two Ptrs still compare equal (pointer part is equal, even if flags are different)
    WD_TEST_BOOL(ptr == ptr2);
  }
}
