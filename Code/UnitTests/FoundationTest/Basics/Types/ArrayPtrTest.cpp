#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Containers/DynamicArray.h>

template <typename T>
static void testArrayPtr(wdArrayPtr<T> arrayPtr, typename wdArrayPtr<T>::PointerType extectedPtr, wdUInt32 uiExpectedCount)
{
  WD_TEST_BOOL(arrayPtr.GetPtr() == extectedPtr);
  WD_TEST_INT(arrayPtr.GetCount(), uiExpectedCount);
}

// static void TakeConstArrayPtr(wdArrayPtr<const int> cint)
//{
//}
//
// static void TakeConstArrayPtr2(wdArrayPtr<const int*> cint, wdArrayPtr<const int* const> cintc)
//{
//}

WD_CREATE_SIMPLE_TEST(Basics, ArrayPtr)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Empty Constructor")
  {
    wdArrayPtr<wdInt32> Empty;

    WD_TEST_BOOL(Empty.GetPtr() == nullptr);
    WD_TEST_BOOL(Empty.GetCount() == 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor")
  {
    wdInt32 pIntData[] = {1, 2, 3, 4, 5};

    wdArrayPtr<wdInt32> ap(pIntData, 3);
    WD_TEST_BOOL(ap.GetPtr() == pIntData);
    WD_TEST_BOOL(ap.GetCount() == 3);

    wdArrayPtr<wdInt32> ap2(pIntData, 0u);
    WD_TEST_BOOL(ap2.GetPtr() == nullptr);
    WD_TEST_BOOL(ap2.GetCount() == 0);

    wdArrayPtr<wdInt32> ap3(pIntData);
    WD_TEST_BOOL(ap3.GetPtr() == pIntData);
    WD_TEST_BOOL(ap3.GetCount() == 5);

    wdArrayPtr<wdInt32> ap4(ap);
    WD_TEST_BOOL(ap4.GetPtr() == pIntData);
    WD_TEST_BOOL(ap4.GetCount() == 3);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdMakeArrayPtr")
  {
    wdInt32 pIntData[] = {1, 2, 3, 4, 5};

    testArrayPtr(wdMakeArrayPtr(pIntData, 3), pIntData, 3);
    testArrayPtr(wdMakeArrayPtr(pIntData, 0), nullptr, 0);
    testArrayPtr(wdMakeArrayPtr(pIntData), pIntData, 5);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator=")
  {
    wdInt32 pIntData[] = {1, 2, 3, 4, 5};

    wdArrayPtr<wdInt32> ap(pIntData, 3);
    WD_TEST_BOOL(ap.GetPtr() == pIntData);
    WD_TEST_BOOL(ap.GetCount() == 3);

    wdArrayPtr<wdInt32> ap2;
    ap2 = ap;

    WD_TEST_BOOL(ap2.GetPtr() == pIntData);
    WD_TEST_BOOL(ap2.GetCount() == 3);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Clear")
  {
    wdInt32 pIntData[] = {1, 2, 3, 4, 5};

    wdArrayPtr<wdInt32> ap(pIntData, 3);
    WD_TEST_BOOL(ap.GetPtr() == pIntData);
    WD_TEST_BOOL(ap.GetCount() == 3);

    ap.Clear();

    WD_TEST_BOOL(ap.GetPtr() == nullptr);
    WD_TEST_BOOL(ap.GetCount() == 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator== / operator!= / operator<")
  {
    wdInt32 pIntData[] = {1, 2, 3, 4, 5};

    wdArrayPtr<wdInt32> ap1(pIntData, 3);
    wdArrayPtr<wdInt32> ap2(pIntData, 3);
    wdArrayPtr<wdInt32> ap3(pIntData, 4);
    wdArrayPtr<wdInt32> ap4(pIntData + 1, 3);

    WD_TEST_BOOL(ap1 == ap2);
    WD_TEST_BOOL(ap1 != ap3);
    WD_TEST_BOOL(ap1 != ap4);

    WD_TEST_BOOL(ap1 < ap3);
    wdInt32 pIntData2[] = {1, 2, 4};
    wdArrayPtr<wdInt32> ap5(pIntData2, 3);
    WD_TEST_BOOL(ap1 < ap5);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator[]")
  {
    wdInt32 pIntData[] = {1, 2, 3, 4, 5};

    wdArrayPtr<wdInt32> ap(pIntData + 1, 3);
    WD_TEST_INT(ap[0], 2);
    WD_TEST_INT(ap[1], 3);
    WD_TEST_INT(ap[2], 4);
    ap[2] = 10;
    WD_TEST_INT(ap[2], 10);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "const operator[]")
  {
    wdInt32 pIntData[] = {1, 2, 3, 4, 5};

    const wdArrayPtr<wdInt32> ap(pIntData + 1, 3);
    WD_TEST_INT(ap[0], 2);
    WD_TEST_INT(ap[1], 3);
    WD_TEST_INT(ap[2], 4);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "CopyFrom")
  {
    wdInt32 pIntData1[] = {1, 2, 3, 4, 5};
    wdInt32 pIntData2[] = {6, 7, 8, 9, 0};

    wdArrayPtr<wdInt32> ap1(pIntData1 + 1, 3);
    wdArrayPtr<wdInt32> ap2(pIntData2 + 2, 3);

    ap1.CopyFrom(ap2);

    WD_TEST_INT(pIntData1[0], 1);
    WD_TEST_INT(pIntData1[1], 8);
    WD_TEST_INT(pIntData1[2], 9);
    WD_TEST_INT(pIntData1[3], 0);
    WD_TEST_INT(pIntData1[4], 5);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetSubArray")
  {
    wdInt32 pIntData1[] = {1, 2, 3, 4, 5};

    wdArrayPtr<wdInt32> ap1(pIntData1, 5);
    wdArrayPtr<wdInt32> ap2 = ap1.GetSubArray(2, 3);

    WD_TEST_BOOL(ap2.GetPtr() == &pIntData1[2]);
    WD_TEST_BOOL(ap2.GetCount() == 3);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Const Conversions")
  {
    wdInt32 pIntData1[] = {1, 2, 3, 4, 5};
    wdArrayPtr<wdInt32> ap1(pIntData1);
    wdArrayPtr<const wdInt32> ap2(ap1);
    wdArrayPtr<const wdInt32> ap3(pIntData1);
    ap2 = ap1; // non const to const assign
    ap3 = ap2; // const to const assign
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Empty Constructor (const)")
  {
    wdArrayPtr<const wdInt32> Empty;

    WD_TEST_BOOL(Empty.GetPtr() == nullptr);
    WD_TEST_BOOL(Empty.GetCount() == 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor (const)")
  {
    const wdInt32 pIntData[] = {1, 2, 3, 4, 5};

    wdArrayPtr<const wdInt32> ap(pIntData, 3);
    WD_TEST_BOOL(ap.GetPtr() == pIntData);
    WD_TEST_BOOL(ap.GetCount() == 3);

    wdArrayPtr<const wdInt32> ap2(pIntData, 0u);
    WD_TEST_BOOL(ap2.GetPtr() == nullptr);
    WD_TEST_BOOL(ap2.GetCount() == 0);

    wdArrayPtr<const wdInt32> ap3(pIntData);
    WD_TEST_BOOL(ap3.GetPtr() == pIntData);
    WD_TEST_BOOL(ap3.GetCount() == 5);

    wdArrayPtr<const wdInt32> ap4(ap);
    WD_TEST_BOOL(ap4.GetPtr() == pIntData);
    WD_TEST_BOOL(ap4.GetCount() == 3);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator=  (const)")
  {
    const wdInt32 pIntData[] = {1, 2, 3, 4, 5};

    wdArrayPtr<const wdInt32> ap(pIntData, 3);
    WD_TEST_BOOL(ap.GetPtr() == pIntData);
    WD_TEST_BOOL(ap.GetCount() == 3);

    wdArrayPtr<const wdInt32> ap2;
    ap2 = ap;

    WD_TEST_BOOL(ap2.GetPtr() == pIntData);
    WD_TEST_BOOL(ap2.GetCount() == 3);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Clear (const)")
  {
    const wdInt32 pIntData[] = {1, 2, 3, 4, 5};

    wdArrayPtr<const wdInt32> ap(pIntData, 3);
    WD_TEST_BOOL(ap.GetPtr() == pIntData);
    WD_TEST_BOOL(ap.GetCount() == 3);

    ap.Clear();

    WD_TEST_BOOL(ap.GetPtr() == nullptr);
    WD_TEST_BOOL(ap.GetCount() == 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator== / operator!=  (const)")
  {
    wdInt32 pIntData[] = {1, 2, 3, 4, 5};

    wdArrayPtr<wdInt32> ap1(pIntData, 3);
    wdArrayPtr<const wdInt32> ap2(pIntData, 3);
    wdArrayPtr<const wdInt32> ap3(pIntData, 4);
    wdArrayPtr<const wdInt32> ap4(pIntData + 1, 3);

    WD_TEST_BOOL(ap1 == ap2);
    WD_TEST_BOOL(ap3 != ap1);
    WD_TEST_BOOL(ap1 != ap4);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator[]  (const)")
  {
    const wdInt32 pIntData[] = {1, 2, 3, 4, 5};

    wdArrayPtr<const wdInt32> ap(pIntData + 1, 3);
    WD_TEST_INT(ap[0], 2);
    WD_TEST_INT(ap[1], 3);
    WD_TEST_INT(ap[2], 4);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "const operator[] (const)")
  {
    const wdInt32 pIntData[] = {1, 2, 3, 4, 5};

    const wdArrayPtr<const wdInt32> ap(pIntData + 1, 3);
    WD_TEST_INT(ap[0], 2);
    WD_TEST_INT(ap[1], 3);
    WD_TEST_INT(ap[2], 4);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetSubArray (const)")
  {
    const wdInt32 pIntData1[] = {1, 2, 3, 4, 5};

    wdArrayPtr<const wdInt32> ap1(pIntData1, 5);
    wdArrayPtr<const wdInt32> ap2 = ap1.GetSubArray(2, 3);

    WD_TEST_BOOL(ap2.GetPtr() == &pIntData1[2]);
    WD_TEST_BOOL(ap2.GetCount() == 3);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "STL Iterator")
  {
    wdDynamicArray<wdInt32> a1;

    for (wdInt32 i = 0; i < 1000; ++i)
      a1.PushBack(1000 - i - 1);

    wdArrayPtr<wdInt32> ptr1 = a1;

    // STL sort
    std::sort(begin(ptr1), end(ptr1));

    for (wdInt32 i = 1; i < 1000; ++i)
    {
      WD_TEST_BOOL(ptr1[i - 1] <= ptr1[i]);
    }

    // foreach
    wdUInt32 prev = 0;
    for (wdUInt32 val : ptr1)
    {
      WD_TEST_BOOL(prev <= val);
      prev = val;
    }

    // const array
    const wdDynamicArray<wdInt32>& a2 = a1;

    const wdArrayPtr<const wdInt32> ptr2 = a2;

    // STL lower bound
    auto lb = std::lower_bound(begin(ptr2), end(ptr2), 400);
    WD_TEST_BOOL(*lb == ptr2[400]);
  }


  WD_TEST_BLOCK(wdTestBlock::Enabled, "STL Reverse Iterator")
  {
    wdDynamicArray<wdInt32> a1;

    for (wdInt32 i = 0; i < 1000; ++i)
      a1.PushBack(1000 - i - 1);

    wdArrayPtr<wdInt32> ptr1 = a1;

    // STL sort
    std::sort(rbegin(ptr1), rend(ptr1));

    for (wdInt32 i = 1; i < 1000; ++i)
    {
      WD_TEST_BOOL(ptr1[i - 1] >= ptr1[i]);
    }

    // foreach
    wdUInt32 prev = 1000;
    for (wdUInt32 val : ptr1)
    {
      WD_TEST_BOOL(prev >= val);
      prev = val;
    }

    // const array
    const wdDynamicArray<wdInt32>& a2 = a1;

    const wdArrayPtr<const wdInt32> ptr2 = a2;

    // STL lower bound
    auto lb = std::lower_bound(rbegin(ptr2), rend(ptr2), 400);
    WD_TEST_BOOL(*lb == ptr2[1000 - 400 - 1]);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Contains / IndexOf / LastIndexOf")
  {
    wdDynamicArray<wdInt32> a0;
    wdArrayPtr<wdInt32> a1 = a0;

    for (wdInt32 i = -100; i < 100; ++i)
      WD_TEST_BOOL(!a1.Contains(i));

    for (wdInt32 i = 0; i < 100; ++i)
      a0.PushBack(i);
    for (wdInt32 i = 0; i < 100; ++i)
      a0.PushBack(i);

    a1 = a0;

    for (wdInt32 i = 0; i < 100; ++i)
    {
      WD_TEST_BOOL(a1.Contains(i));
      WD_TEST_INT(a1.IndexOf(i), i);
      WD_TEST_INT(a1.IndexOf(i, 100), i + 100);
      WD_TEST_INT(a1.LastIndexOf(i), i + 100);
      WD_TEST_INT(a1.LastIndexOf(i, 100), i);
    }
  }

  // "Implicit Conversions"
  //{
  //  {
  //    wdHybridArray<int, 4> data;
  //    TakeConstArrayPtr(data);
  //    TakeConstArrayPtr(data.GetArrayPtr());
  //  }
  //  {
  //    wdHybridArray<int*, 4> data;
  //    //TakeConstArrayPtr2(data, data); // does not compile
  //    TakeConstArrayPtr2(data.GetArrayPtr(), data.GetArrayPtr());
  //  }
  //}
}
