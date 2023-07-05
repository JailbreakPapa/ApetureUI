#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Containers/DynamicArray.h>
#include <Foundation/Logging/Log.h>
#include <Foundation/Reflection/Reflection.h>
#include <Foundation/Strings/String.h>
#include <Foundation/Time/Time.h>

#include <vector>

namespace
{
  enum constants
  {
#if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
    NUM_SAMPLES = 128,
    NUM_APPENDS = 1024 * 32,
    NUM_RECUSRIVE_APPENDS = 128
#else
    NUM_SAMPLES = 1024,
    NUM_APPENDS = 1024 * 64,
    NUM_RECUSRIVE_APPENDS = 256
#endif
  };

  struct SomeBigObject
  {
    WD_DECLARE_MEM_RELOCATABLE_TYPE();

    static wdUInt32 constructionCount;
    static wdUInt32 destructionCount;
    wdUInt64 i1, i2, i3, i4, i5, i6, i7, i8;

    SomeBigObject(wdUInt64 uiInit)
      : i1(uiInit)
      , i2(uiInit)
      , i3(uiInit)
      , i4(uiInit)
      , i5(uiInit)
      , i6(uiInit)
      , i7(uiInit)
      , i8(uiInit)
    {
      constructionCount++;
    }

    ~SomeBigObject() { destructionCount++; }

    SomeBigObject(const SomeBigObject& rh)
    {
      constructionCount++;
      this->i1 = rh.i1;
      this->i2 = rh.i2;
      this->i3 = rh.i3;
      this->i4 = rh.i4;
      this->i5 = rh.i5;
      this->i6 = rh.i6;
      this->i7 = rh.i7;
      this->i8 = rh.i8;
    }

    void operator=(const SomeBigObject& rh)
    {
      constructionCount++;
      this->i1 = rh.i1;
      this->i2 = rh.i2;
      this->i3 = rh.i3;
      this->i4 = rh.i4;
      this->i5 = rh.i5;
      this->i6 = rh.i6;
      this->i7 = rh.i7;
      this->i8 = rh.i8;
    }
  };

  wdUInt32 SomeBigObject::constructionCount = 0;
  wdUInt32 SomeBigObject::destructionCount = 0;
} // namespace

// Enable when needed
#define WD_PERFORMANCE_TESTS_STATE wdTestBlock::DisabledNoWarning

WD_CREATE_SIMPLE_TEST(Performance, Container)
{
  const char* TestString = "There are 10 types of people in the world. Those who understand binary and those who don't.";
  const wdUInt32 TestStringLength = (wdUInt32)strlen(TestString);

  WD_TEST_BLOCK(WD_PERFORMANCE_TESTS_STATE, "POD Dynamic Array Appending")
  {
    wdTime t0 = wdTime::Now();
    wdUInt32 sum = 0;
    for (wdUInt32 n = 0; n < NUM_SAMPLES; n++)
    {
      wdDynamicArray<int> a;
      for (wdUInt32 i = 0; i < NUM_APPENDS; i++)
      {
        a.PushBack(i);
      }

      for (wdUInt32 i = 0; i < NUM_APPENDS; i++)
      {
        sum += a[i];
      }
    }

    wdTime t1 = wdTime::Now();
    wdLog::Info("[test]POD Dynamic Array Appending {0}ms", wdArgF((t1 - t0).GetMilliseconds() / static_cast<double>(NUM_SAMPLES), 4), sum);
  }

  WD_TEST_BLOCK(WD_PERFORMANCE_TESTS_STATE, "POD std::vector Appending")
  {
    wdTime t0 = wdTime::Now();

    wdUInt32 sum = 0;
    for (wdUInt32 n = 0; n < NUM_SAMPLES; n++)
    {
      std::vector<int> a;
      for (wdUInt32 i = 0; i < NUM_APPENDS; i++)
      {
        a.push_back(i);
      }

      for (wdUInt32 i = 0; i < NUM_APPENDS; i++)
      {
        sum += a[i];
      }
    }

    wdTime t1 = wdTime::Now();
    wdLog::Info("[test]POD std::vector Appending {0}ms", wdArgF((t1 - t0).GetMilliseconds() / static_cast<double>(NUM_SAMPLES), 4), sum);
  }

  WD_TEST_BLOCK(WD_PERFORMANCE_TESTS_STATE, "wdDynamicArray<wdDynamicArray<char>> Appending")
  {
    wdTime t0 = wdTime::Now();

    wdUInt32 sum = 0;
    for (wdUInt32 n = 0; n < NUM_SAMPLES; n++)
    {
      wdDynamicArray<wdDynamicArray<char>> a;
      for (wdUInt32 i = 0; i < NUM_RECUSRIVE_APPENDS; i++)
      {
        wdUInt32 count = a.GetCount();
        a.SetCount(count + 1);
        wdDynamicArray<char>& cur = a[count];
        for (wdUInt32 j = 0; j < TestStringLength; j++)
        {
          cur.PushBack(TestString[j]);
        }
      }

      for (wdUInt32 i = 0; i < NUM_RECUSRIVE_APPENDS; i++)
      {
        sum += a[i].GetCount();
      }
    }

    wdTime t1 = wdTime::Now();
    wdLog::Info(
      "[test]wdDynamicArray<wdDynamicArray<char>> Appending {0}ms", wdArgF((t1 - t0).GetMilliseconds() / static_cast<double>(NUM_SAMPLES), 4), sum);
  }

  WD_TEST_BLOCK(WD_PERFORMANCE_TESTS_STATE, "wdDynamicArray<wdHybridArray<char, 64>> Appending")
  {
    wdTime t0 = wdTime::Now();

    wdUInt32 sum = 0;
    for (wdUInt32 n = 0; n < NUM_SAMPLES; n++)
    {
      wdDynamicArray<wdHybridArray<char, 64>> a;
      for (wdUInt32 i = 0; i < NUM_RECUSRIVE_APPENDS; i++)
      {
        wdUInt32 count = a.GetCount();
        a.SetCount(count + 1);
        wdHybridArray<char, 64>& cur = a[count];
        for (wdUInt32 j = 0; j < TestStringLength; j++)
        {
          cur.PushBack(TestString[j]);
        }
      }

      for (wdUInt32 i = 0; i < NUM_RECUSRIVE_APPENDS; i++)
      {
        sum += a[i].GetCount();
      }
    }

    wdTime t1 = wdTime::Now();
    wdLog::Info("[test]wdDynamicArray<wdHybridArray<char, 64>> Appending {0}ms",
      wdArgF((t1 - t0).GetMilliseconds() / static_cast<double>(NUM_SAMPLES), 4), sum);
  }

  WD_TEST_BLOCK(WD_PERFORMANCE_TESTS_STATE, "std::vector<std::vector<char>> Appending")
  {
    wdTime t0 = wdTime::Now();

    wdUInt32 sum = 0;
    for (wdUInt32 n = 0; n < NUM_SAMPLES; n++)
    {
      std::vector<std::vector<char>> a;
      for (wdUInt32 i = 0; i < NUM_RECUSRIVE_APPENDS; i++)
      {
        wdUInt32 count = (wdUInt32)a.size();
        a.resize(count + 1);
        std::vector<char>& cur = a[count];
        for (wdUInt32 j = 0; j < TestStringLength; j++)
        {
          cur.push_back(TestString[j]);
        }
      }

      for (wdUInt32 i = 0; i < NUM_RECUSRIVE_APPENDS; i++)
      {
        sum += (wdUInt32)a[i].size();
      }
    }

    wdTime t1 = wdTime::Now();
    wdLog::Info(
      "[test]std::vector<std::vector<char>> Appending {0}ms", wdArgF((t1 - t0).GetMilliseconds() / static_cast<double>(NUM_SAMPLES), 4), sum);
  }

  WD_TEST_BLOCK(WD_PERFORMANCE_TESTS_STATE, "wdDynamicArray<wdString> Appending")
  {
    wdTime t0 = wdTime::Now();

    wdUInt32 sum = 0;
    for (wdUInt32 n = 0; n < NUM_SAMPLES; n++)
    {
      wdDynamicArray<wdString> a;
      for (wdUInt32 i = 0; i < NUM_RECUSRIVE_APPENDS; i++)
      {
        wdUInt32 count = a.GetCount();
        a.SetCount(count + 1);
        wdString& cur = a[count];
        wdStringBuilder b;
        for (wdUInt32 j = 0; j < TestStringLength; j++)
        {
          b.Append(TestString[i]);
        }
        cur = std::move(b);
      }

      for (wdUInt32 i = 0; i < NUM_RECUSRIVE_APPENDS; i++)
      {
        sum += a[i].GetElementCount();
      }
    }

    wdTime t1 = wdTime::Now();
    wdLog::Info("[test]wdDynamicArray<wdString> Appending {0}ms", wdArgF((t1 - t0).GetMilliseconds() / static_cast<double>(NUM_SAMPLES), 4), sum);
  }

  WD_TEST_BLOCK(WD_PERFORMANCE_TESTS_STATE, "std::vector<std::string> Appending")
  {
    wdTime t0 = wdTime::Now();

    wdUInt32 sum = 0;
    for (wdUInt32 n = 0; n < NUM_SAMPLES; n++)
    {
      std::vector<std::string> a;
      for (wdUInt32 i = 0; i < NUM_RECUSRIVE_APPENDS; i++)
      {
        std::string cur;
        for (wdUInt32 j = 0; j < TestStringLength; j++)
        {
          cur += TestString[i];
        }
        a.push_back(std::move(cur));
      }

      for (wdUInt32 i = 0; i < NUM_RECUSRIVE_APPENDS; i++)
      {
        sum += (wdUInt32)a[i].length();
      }
    }

    wdTime t1 = wdTime::Now();
    wdLog::Info("[test]std::vector<std::string> Appending {0}ms", wdArgF((t1 - t0).GetMilliseconds() / static_cast<double>(NUM_SAMPLES), 4), sum);
  }

  WD_TEST_BLOCK(WD_PERFORMANCE_TESTS_STATE, "wdDynamicArray<SomeBigObject> Appending")
  {
    wdTime t0 = wdTime::Now();

    wdUInt32 sum = 0;
    for (wdUInt32 n = 0; n < NUM_SAMPLES; n++)
    {
      wdDynamicArray<SomeBigObject> a;
      for (wdUInt32 i = 0; i < NUM_APPENDS; i++)
      {
        a.PushBack(SomeBigObject(i));
      }

      for (wdUInt32 i = 0; i < NUM_RECUSRIVE_APPENDS; i++)
      {
        sum += (wdUInt32)a[i].i1;
      }
    }

    wdTime t1 = wdTime::Now();
    wdLog::Info(
      "[test]wdDynamicArray<SomeBigObject> Appending {0}ms", wdArgF((t1 - t0).GetMilliseconds() / static_cast<double>(NUM_SAMPLES), 4), sum);
  }

  WD_TEST_BLOCK(WD_PERFORMANCE_TESTS_STATE, "std::vector<SomeBigObject> Appending")
  {
    wdTime t0 = wdTime::Now();

    wdUInt32 sum = 0;
    for (wdUInt32 n = 0; n < NUM_SAMPLES; n++)
    {
      std::vector<SomeBigObject> a;
      for (wdUInt32 i = 0; i < NUM_APPENDS; i++)
      {
        a.push_back(SomeBigObject(i));
      }

      for (wdUInt32 i = 0; i < NUM_RECUSRIVE_APPENDS; i++)
      {
        sum += (wdUInt32)a[i].i1;
      }
    }

    wdTime t1 = wdTime::Now();
    wdLog::Info("[test]std::vector<SomeBigObject> Appending {0}ms", wdArgF((t1 - t0).GetMilliseconds() / static_cast<double>(NUM_SAMPLES), 4), sum);
  }

  WD_TEST_BLOCK(wdTestBlock::DisabledNoWarning, "wdMap<void*, wdUInt32>")
  {
    wdUInt32 sum = 0;



    for (wdUInt32 size = 1024; size < 4096 * 32; size += 1024)
    {
      wdMap<void*, wdUInt32> map;

      for (wdUInt32 i = 0; i < size; i++)
      {
        map.Insert(malloc(64), 64);
      }

      void* ptrs[1024];

      wdTime t0 = wdTime::Now();
      for (wdUInt32 n = 0; n < NUM_SAMPLES; n++)
      {
        for (wdUInt32 i = 0; i < 1024; i++)
        {
          void* mem = malloc(64);
          map.Insert(mem, 64);
          map.Remove(mem);
          ptrs[i] = mem;
        }

        for (wdUInt32 i = 0; i < 1024; i++)
          free(ptrs[i]);

        auto last = map.GetLastIterator();
        for (auto it = map.GetIterator(); it != last; ++it)
        {
          sum += it.Value();
        }
      }
      wdTime t1 = wdTime::Now();

      auto last = map.GetLastIterator();
      for (auto it = map.GetIterator(); it != last; ++it)
      {
        free(it.Key());
      }
      wdLog::Info(
        "[test]wdMap<void*, wdUInt32> size = {0} => {1}ms", size, wdArgF((t1 - t0).GetMilliseconds() / static_cast<double>(NUM_SAMPLES), 4), sum);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::DisabledNoWarning, "wdHashTable<void*, wdUInt32>")
  {
    wdUInt32 sum = 0;



    for (wdUInt32 size = 1024; size < 4096 * 32; size += 1024)
    {
      wdHashTable<void*, wdUInt32> map;

      for (wdUInt32 i = 0; i < size; i++)
      {
        map.Insert(malloc(64), 64);
      }

      void* ptrs[1024];

      wdTime t0 = wdTime::Now();
      for (wdUInt32 n = 0; n < NUM_SAMPLES; n++)
      {

        for (wdUInt32 i = 0; i < 1024; i++)
        {
          void* mem = malloc(64);
          map.Insert(mem, 64);
          map.Remove(mem);
          ptrs[i] = mem;
        }

        for (wdUInt32 i = 0; i < 1024; i++)
          free(ptrs[i]);

        for (auto it = map.GetIterator(); it.IsValid(); it.Next())
        {
          sum += it.Value();
        }
      }
      wdTime t1 = wdTime::Now();

      for (auto it = map.GetIterator(); it.IsValid(); it.Next())
      {
        free(it.Key());
      }

      wdLog::Info("[test]wdHashTable<void*, wdUInt32> size = {0} => {1}ms", size,
        wdArgF((t1 - t0).GetMilliseconds() / static_cast<double>(NUM_SAMPLES), 4), sum);
    }
  }
}
