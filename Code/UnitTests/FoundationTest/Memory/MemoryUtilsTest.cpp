#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Containers/HybridArray.h>

struct wdConstructTest
{
public:
  static wdHybridArray<void*, 10> s_dtorList;

  wdConstructTest() { m_iData = 42; }

  ~wdConstructTest() { s_dtorList.PushBack(this); }

  wdInt32 m_iData;
};
wdHybridArray<void*, 10> wdConstructTest::s_dtorList;

WD_CHECK_AT_COMPILETIME(sizeof(wdConstructTest) == 4);


struct PODTest
{
  WD_DECLARE_POD_TYPE();

  PODTest() { m_iData = -1; }

  wdInt32 m_iData;
};

static const wdUInt32 s_uiSize = sizeof(wdConstructTest);

WD_CREATE_SIMPLE_TEST(Memory, MemoryUtils)
{
  wdConstructTest::s_dtorList.Clear();

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Construct")
  {
    wdUInt8 uiRawData[s_uiSize * 5] = {0};
    wdConstructTest* pTest = (wdConstructTest*)(uiRawData);

    wdMemoryUtils::Construct<wdConstructTest>(pTest + 1, 2);

    WD_TEST_INT(pTest[0].m_iData, 0);
    WD_TEST_INT(pTest[1].m_iData, 42);
    WD_TEST_INT(pTest[2].m_iData, 42);
    WD_TEST_INT(pTest[3].m_iData, 0);
    WD_TEST_INT(pTest[4].m_iData, 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "MakeConstructorFunction")
  {
    wdMemoryUtils::ConstructorFunction func = wdMemoryUtils::MakeConstructorFunction<wdConstructTest>();
    WD_TEST_BOOL(func != nullptr);

    wdUInt8 uiRawData[s_uiSize] = {0};
    wdConstructTest* pTest = (wdConstructTest*)(uiRawData);

    (*func)(pTest);

    WD_TEST_INT(pTest->m_iData, 42);

    func = wdMemoryUtils::MakeConstructorFunction<PODTest>();
    WD_TEST_BOOL(func != nullptr);

    func = wdMemoryUtils::MakeConstructorFunction<wdInt32>();
    WD_TEST_BOOL(func == nullptr);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "DefaultConstruct")
  {
    wdUInt32 uiRawData[5]; // not initialized here

    wdMemoryUtils::DefaultConstruct(uiRawData + 1, 2);

    WD_TEST_INT(uiRawData[1], 0);
    WD_TEST_INT(uiRawData[2], 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "MakeDefaultConstructorFunction")
  {
    wdMemoryUtils::ConstructorFunction func = wdMemoryUtils::MakeDefaultConstructorFunction<wdInt32>();
    WD_TEST_BOOL(func != nullptr);

    wdInt32 iTest = 2;

    (*func)(&iTest);

    WD_TEST_INT(iTest, 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Construct Copy(Array)")
  {
    wdUInt8 uiRawData[s_uiSize * 5] = {0};
    wdConstructTest* pTest = (wdConstructTest*)(uiRawData);

    wdConstructTest copy[2];
    copy[0].m_iData = 43;
    copy[1].m_iData = 44;

    wdMemoryUtils::CopyConstructArray<wdConstructTest>(pTest + 1, copy, 2);

    WD_TEST_INT(pTest[0].m_iData, 0);
    WD_TEST_INT(pTest[1].m_iData, 43);
    WD_TEST_INT(pTest[2].m_iData, 44);
    WD_TEST_INT(pTest[3].m_iData, 0);
    WD_TEST_INT(pTest[4].m_iData, 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Construct Copy(Element)")
  {
    wdUInt8 uiRawData[s_uiSize * 5] = {0};
    wdConstructTest* pTest = (wdConstructTest*)(uiRawData);

    wdConstructTest copy;
    copy.m_iData = 43;

    wdMemoryUtils::CopyConstruct<wdConstructTest>(pTest + 1, copy, 2);

    WD_TEST_INT(pTest[0].m_iData, 0);
    WD_TEST_INT(pTest[1].m_iData, 43);
    WD_TEST_INT(pTest[2].m_iData, 43);
    WD_TEST_INT(pTest[3].m_iData, 0);
    WD_TEST_INT(pTest[4].m_iData, 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "MakeCopyConstructorFunction")
  {
    wdMemoryUtils::CopyConstructorFunction func = wdMemoryUtils::MakeCopyConstructorFunction<wdConstructTest>();
    WD_TEST_BOOL(func != nullptr);

    wdUInt8 uiRawData[s_uiSize] = {0};
    wdConstructTest* pTest = (wdConstructTest*)(uiRawData);

    wdConstructTest copy;
    copy.m_iData = 43;

    (*func)(pTest, &copy);

    WD_TEST_INT(pTest->m_iData, 43);

    func = wdMemoryUtils::MakeCopyConstructorFunction<PODTest>();
    WD_TEST_BOOL(func != nullptr);

    func = wdMemoryUtils::MakeCopyConstructorFunction<wdInt32>();
    WD_TEST_BOOL(func != nullptr);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Destruct")
  {
    wdUInt8 uiRawData[s_uiSize * 5] = {0};
    wdConstructTest* pTest = (wdConstructTest*)(uiRawData);

    wdMemoryUtils::Construct<wdConstructTest>(pTest + 1, 2);

    WD_TEST_INT(pTest[0].m_iData, 0);
    WD_TEST_INT(pTest[1].m_iData, 42);
    WD_TEST_INT(pTest[2].m_iData, 42);
    WD_TEST_INT(pTest[3].m_iData, 0);
    WD_TEST_INT(pTest[4].m_iData, 0);

    wdConstructTest::s_dtorList.Clear();
    wdMemoryUtils::Destruct<wdConstructTest>(pTest, 4);
    WD_TEST_INT(4, wdConstructTest::s_dtorList.GetCount());

    if (wdConstructTest::s_dtorList.GetCount() == 4)
    {
      WD_TEST_BOOL(wdConstructTest::s_dtorList[0] == &pTest[3]);
      WD_TEST_BOOL(wdConstructTest::s_dtorList[1] == &pTest[2]);
      WD_TEST_BOOL(wdConstructTest::s_dtorList[2] == &pTest[1]);
      WD_TEST_BOOL(wdConstructTest::s_dtorList[3] == &pTest[0]);
      WD_TEST_INT(pTest[4].m_iData, 0);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "MakeDestructorFunction")
  {
    wdMemoryUtils::DestructorFunction func = wdMemoryUtils::MakeDestructorFunction<wdConstructTest>();
    WD_TEST_BOOL(func != nullptr);

    wdUInt8 uiRawData[s_uiSize] = {0};
    wdConstructTest* pTest = (wdConstructTest*)(uiRawData);

    wdMemoryUtils::Construct(pTest, 1);
    WD_TEST_INT(pTest->m_iData, 42);

    wdConstructTest::s_dtorList.Clear();
    (*func)(pTest);
    WD_TEST_INT(1, wdConstructTest::s_dtorList.GetCount());

    if (wdConstructTest::s_dtorList.GetCount() == 1)
    {
      WD_TEST_BOOL(wdConstructTest::s_dtorList[0] == pTest);
    }

    func = wdMemoryUtils::MakeDestructorFunction<PODTest>();
    WD_TEST_BOOL(func == nullptr);

    func = wdMemoryUtils::MakeDestructorFunction<wdInt32>();
    WD_TEST_BOOL(func == nullptr);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Copy")
  {
    wdUInt8 uiRawData[5] = {1, 2, 3, 4, 5};
    wdUInt8 uiRawData2[5] = {6, 7, 8, 9, 0};

    WD_TEST_INT(uiRawData[0], 1);
    WD_TEST_INT(uiRawData[1], 2);
    WD_TEST_INT(uiRawData[2], 3);
    WD_TEST_INT(uiRawData[3], 4);
    WD_TEST_INT(uiRawData[4], 5);

    wdMemoryUtils::Copy(uiRawData + 1, uiRawData2 + 2, 3);

    WD_TEST_INT(uiRawData[0], 1);
    WD_TEST_INT(uiRawData[1], 8);
    WD_TEST_INT(uiRawData[2], 9);
    WD_TEST_INT(uiRawData[3], 0);
    WD_TEST_INT(uiRawData[4], 5);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Move")
  {
    wdUInt8 uiRawData[5] = {1, 2, 3, 4, 5};

    WD_TEST_INT(uiRawData[0], 1);
    WD_TEST_INT(uiRawData[1], 2);
    WD_TEST_INT(uiRawData[2], 3);
    WD_TEST_INT(uiRawData[3], 4);
    WD_TEST_INT(uiRawData[4], 5);

    wdMemoryUtils::CopyOverlapped(uiRawData + 1, uiRawData + 3, 2);

    WD_TEST_INT(uiRawData[0], 1);
    WD_TEST_INT(uiRawData[1], 4);
    WD_TEST_INT(uiRawData[2], 5);
    WD_TEST_INT(uiRawData[3], 4);
    WD_TEST_INT(uiRawData[4], 5);

    wdMemoryUtils::CopyOverlapped(uiRawData + 1, uiRawData, 4);

    WD_TEST_INT(uiRawData[0], 1);
    WD_TEST_INT(uiRawData[1], 1);
    WD_TEST_INT(uiRawData[2], 4);
    WD_TEST_INT(uiRawData[3], 5);
    WD_TEST_INT(uiRawData[4], 4);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsEqual")
  {
    wdUInt8 uiRawData1[5] = {1, 2, 3, 4, 5};
    wdUInt8 uiRawData2[5] = {1, 2, 3, 4, 5};
    wdUInt8 uiRawData3[5] = {1, 2, 3, 4, 6};

    WD_TEST_BOOL(wdMemoryUtils::IsEqual(uiRawData1, uiRawData2, 5));
    WD_TEST_BOOL(!wdMemoryUtils::IsEqual(uiRawData1, uiRawData3, 5));
    WD_TEST_BOOL(wdMemoryUtils::IsEqual(uiRawData1, uiRawData3, 4));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ZeroFill")
  {
    wdUInt8 uiRawData[5] = {1, 2, 3, 4, 5};

    WD_TEST_INT(uiRawData[0], 1);
    WD_TEST_INT(uiRawData[1], 2);
    WD_TEST_INT(uiRawData[2], 3);
    WD_TEST_INT(uiRawData[3], 4);
    WD_TEST_INT(uiRawData[4], 5);

    // T*, size_t N overload
    wdMemoryUtils::ZeroFill(uiRawData + 1, 3);

    WD_TEST_INT(uiRawData[0], 1);
    WD_TEST_INT(uiRawData[1], 0);
    WD_TEST_INT(uiRawData[2], 0);
    WD_TEST_INT(uiRawData[3], 0);
    WD_TEST_INT(uiRawData[4], 5);

    // T[N] overload
    wdMemoryUtils::ZeroFillArray(uiRawData);

    WD_TEST_INT(uiRawData[0], 0);
    WD_TEST_INT(uiRawData[1], 0);
    WD_TEST_INT(uiRawData[2], 0);
    WD_TEST_INT(uiRawData[3], 0);
    WD_TEST_INT(uiRawData[4], 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "PatternFill")
  {
    wdUInt8 uiRawData[5] = {1, 2, 3, 4, 5};

    WD_TEST_INT(uiRawData[0], 1);
    WD_TEST_INT(uiRawData[1], 2);
    WD_TEST_INT(uiRawData[2], 3);
    WD_TEST_INT(uiRawData[3], 4);
    WD_TEST_INT(uiRawData[4], 5);

    // T*, size_t N overload
    wdMemoryUtils::PatternFill(uiRawData + 1, 0xAB, 3);

    WD_TEST_INT(uiRawData[0], 1);
    WD_TEST_INT(uiRawData[1], 0xAB);
    WD_TEST_INT(uiRawData[2], 0xAB);
    WD_TEST_INT(uiRawData[3], 0xAB);
    WD_TEST_INT(uiRawData[4], 5);

    // T[N] overload
    wdMemoryUtils::PatternFillArray(uiRawData, 0xCD);

    WD_TEST_INT(uiRawData[0], 0xCD);
    WD_TEST_INT(uiRawData[1], 0xCD);
    WD_TEST_INT(uiRawData[2], 0xCD);
    WD_TEST_INT(uiRawData[3], 0xCD);
    WD_TEST_INT(uiRawData[4], 0xCD);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Compare")
  {
    wdUInt32 uiRawDataA[3] = {1, 2, 3};
    wdUInt32 uiRawDataB[3] = {3, 4, 5};

    WD_TEST_INT(uiRawDataA[0], 1);
    WD_TEST_INT(uiRawDataA[1], 2);
    WD_TEST_INT(uiRawDataA[2], 3);
    WD_TEST_INT(uiRawDataB[0], 3);
    WD_TEST_INT(uiRawDataB[1], 4);
    WD_TEST_INT(uiRawDataB[2], 5);

    WD_TEST_BOOL(wdMemoryUtils::Compare(uiRawDataA, uiRawDataB, 3) < 0);
    WD_TEST_BOOL(wdMemoryUtils::Compare(uiRawDataA + 2, uiRawDataB, 1) == 0);
    WD_TEST_BOOL(wdMemoryUtils::Compare(uiRawDataB, uiRawDataA, 3) > 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "AddByteOffset")
  {
    wdInt32* pData1 = nullptr;
    pData1 = wdMemoryUtils::AddByteOffset(pData1, 13);
    WD_TEST_BOOL(pData1 == reinterpret_cast<wdInt32*>(13));

    const wdInt32* pData2 = nullptr;
    const wdInt32* pData3 = wdMemoryUtils::AddByteOffset(pData2, 17);
    WD_TEST_BOOL(pData3 == reinterpret_cast<wdInt32*>(17));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Align / IsAligned")
  {
    {
      wdInt32* pData = (wdInt32*)1;
      WD_TEST_BOOL(!wdMemoryUtils::IsAligned(pData, 4));
      pData = wdMemoryUtils::AlignBackwards(pData, 4);
      WD_TEST_BOOL(pData == reinterpret_cast<wdInt32*>(0));
      WD_TEST_BOOL(wdMemoryUtils::IsAligned(pData, 4));
    }
    {
      wdInt32* pData = (wdInt32*)2;
      WD_TEST_BOOL(!wdMemoryUtils::IsAligned(pData, 4));
      pData = wdMemoryUtils::AlignBackwards(pData, 4);
      WD_TEST_BOOL(pData == reinterpret_cast<wdInt32*>(0));
      WD_TEST_BOOL(wdMemoryUtils::IsAligned(pData, 4));
    }
    {
      wdInt32* pData = (wdInt32*)3;
      WD_TEST_BOOL(!wdMemoryUtils::IsAligned(pData, 4));
      pData = wdMemoryUtils::AlignBackwards(pData, 4);
      WD_TEST_BOOL(pData == reinterpret_cast<wdInt32*>(0));
      WD_TEST_BOOL(wdMemoryUtils::IsAligned(pData, 4));
    }
    {
      wdInt32* pData = (wdInt32*)4;
      WD_TEST_BOOL(wdMemoryUtils::IsAligned(pData, 4));
      pData = wdMemoryUtils::AlignBackwards(pData, 4);
      WD_TEST_BOOL(pData == reinterpret_cast<wdInt32*>(4));
      WD_TEST_BOOL(wdMemoryUtils::IsAligned(pData, 4));
    }

    {
      wdInt32* pData = (wdInt32*)1;
      WD_TEST_BOOL(!wdMemoryUtils::IsAligned(pData, 4));
      pData = wdMemoryUtils::AlignForwards(pData, 4);
      WD_TEST_BOOL(pData == reinterpret_cast<wdInt32*>(4));
      WD_TEST_BOOL(wdMemoryUtils::IsAligned(pData, 4));
    }
    {
      wdInt32* pData = (wdInt32*)2;
      WD_TEST_BOOL(!wdMemoryUtils::IsAligned(pData, 4));
      pData = wdMemoryUtils::AlignForwards(pData, 4);
      WD_TEST_BOOL(pData == reinterpret_cast<wdInt32*>(4));
      WD_TEST_BOOL(wdMemoryUtils::IsAligned(pData, 4));
    }
    {
      wdInt32* pData = (wdInt32*)3;
      WD_TEST_BOOL(!wdMemoryUtils::IsAligned(pData, 4));
      pData = wdMemoryUtils::AlignForwards(pData, 4);
      WD_TEST_BOOL(pData == reinterpret_cast<wdInt32*>(4));
      WD_TEST_BOOL(wdMemoryUtils::IsAligned(pData, 4));
    }
    {
      wdInt32* pData = (wdInt32*)4;
      WD_TEST_BOOL(wdMemoryUtils::IsAligned(pData, 4));
      pData = wdMemoryUtils::AlignForwards(pData, 4);
      WD_TEST_BOOL(pData == reinterpret_cast<wdInt32*>(4));
      WD_TEST_BOOL(wdMemoryUtils::IsAligned(pData, 4));
    }
  }
}
