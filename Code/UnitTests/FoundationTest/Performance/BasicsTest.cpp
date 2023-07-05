#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Communication/Message.h>
#include <Foundation/Logging/Log.h>
#include <Foundation/Reflection/Reflection.h>
#include <Foundation/Time/Time.h>

/* Performance Statistics:

  AMD E-350 Processor 1.6 GHz ('Fusion'), 32 Bit, Debug Mode
    Virtual Function Calls:   ~60 ns
    Simple Function Calls:    ~27 ns
    Fastcall Function Calls:  ~27 ns
    Integer Division:         52 ns
    Integer Multiplication:   23 ns
    Float Division:           25 ns
    Float Multiplication:     25 ns

  AMD E-350 Processor 1.6 GHz ('Fusion'), 64 Bit, Debug Mode
    Virtual Function Calls:   ~80 ns
    Simple Function Calls:    ~55 ns
    Fastcall Function Calls:  ~55 ns
    Integer Division:         ~97 ns
    Integer Multiplication:   ~52 ns
    Float Division:           ~66 ns
    Float Multiplication:     ~58 ns

  AMD E-350 Processor 1.6 GHz ('Fusion'), 32 Bit, Release Mode
    Virtual Function Calls:   ~9 ns
    Simple Function Calls:    ~5 ns
    Fastcall Function Calls:  ~5 ns
    Integer Division:         35 ns
    Integer Multiplication:   3.78 ns
    Float Division:           10.7 ns
    Float Multiplication:     9.5 ns

  AMD E-350 Processor 1.6 GHz ('Fusion'), 64 Bit, Release Mode
    Virtual Function Calls:   ~10 ns
    Simple Function Calls:    ~5 ns
    Fastcall Function Calls:  ~5 ns
    Integer Division:         35 ns
    Integer Multiplication:   3.23 ns
    Float Division:           8.13 ns
    Float Multiplication:     4.13 ns

  Intel Core i7 3770 3.4 GHz, 64 Bit, Release Mode
    Virtual Function Calls:   ~3.8 ns
    Simple Function Calls:    ~4.4 ns
    Fastcall Function Calls:  ~4.0 ns
    Integer Division:         8.25 ns
    Integer Multiplication:   1.55 ns
    Float Division:           4.40 ns
    Float Multiplication:     1.87 ns

*/

WD_CREATE_SIMPLE_TEST_GROUP(Performance);

struct wdMsgTest : public wdMessage
{
  WD_DECLARE_MESSAGE_TYPE(wdMsgTest, wdMessage);
};

WD_IMPLEMENT_MESSAGE_TYPE(wdMsgTest);
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdMsgTest, 1, wdRTTIDefaultAllocator<wdMsgTest>)
WD_END_DYNAMIC_REFLECTED_TYPE;


struct GetValueMessage : public wdMsgTest
{
  WD_DECLARE_MESSAGE_TYPE(GetValueMessage, wdMsgTest);

  wdInt32 m_iValue;
};
WD_IMPLEMENT_MESSAGE_TYPE(GetValueMessage);
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(GetValueMessage, 1, wdRTTIDefaultAllocator<GetValueMessage>)
WD_END_DYNAMIC_REFLECTED_TYPE;



class Base : public wdReflectedClass
{
  WD_ADD_DYNAMIC_REFLECTION(Base, wdReflectedClass);

public:
  virtual ~Base() {}

  virtual wdInt32 Virtual() = 0;
};

WD_BEGIN_DYNAMIC_REFLECTED_TYPE(Base, 1, wdRTTINoAllocator)
WD_END_DYNAMIC_REFLECTED_TYPE;

#if WD_ENABLED(WD_PLATFORM_WINDOWS)
#  define WD_FASTCALL __fastcall
#  define WD_NO_INLINE __declspec(noinline)
#elif WD_ENABLED(WD_PLATFORM_OSX) || WD_ENABLED(WD_PLATFORM_LINUX) || WD_ENABLED(WD_PLATFORM_ANDROID)
#  if WD_ENABLED(WD_PLATFORM_ARCH_X86) && WD_ENABLED(WD_PLATFORM_32BIT)
#    define WD_FASTCALL __attribute((fastcall)) // Fastcall only relevant on x86-32 and would otherwise generate warnings
#  else
#    define WD_FASTCALL
#  endif
#  define WD_NO_INLINE __attribute__((noinline))
#else
#  warning Unknown Platform.
#  define WD_FASTCALL
#  define WD_NO_INLINE __attribute__((noinline)) /* should work on GCC */
#endif

class Derived1 : public Base
{
  WD_ADD_DYNAMIC_REFLECTION(Derived1, Base);

public:
  WD_NO_INLINE wdInt32 WD_FASTCALL FastCall() { return 1; }
  WD_NO_INLINE wdInt32 NonVirtual() { return 1; }
  WD_NO_INLINE virtual wdInt32 Virtual() override { return 1; }
  WD_NO_INLINE void OnGetValueMessage(GetValueMessage& ref_msg) { ref_msg.m_iValue = 1; }
};

// clang-format off
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(Derived1, 1, wdRTTINoAllocator)
{
  WD_BEGIN_MESSAGEHANDLERS
  {
    WD_MESSAGE_HANDLER(GetValueMessage, OnGetValueMessage),
  }
  WD_END_MESSAGEHANDLERS;
}
WD_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

class Derived2 : public Base
{
  WD_ADD_DYNAMIC_REFLECTION(Derived2, Base);

public:
  WD_NO_INLINE wdInt32 WD_FASTCALL FastCall() { return 2; }
  WD_NO_INLINE wdInt32 NonVirtual() { return 2; }
  WD_NO_INLINE virtual wdInt32 Virtual() override { return 2; }
  WD_NO_INLINE void OnGetValueMessage(GetValueMessage& ref_msg) { ref_msg.m_iValue = 2; }
};

// clang-format off
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(Derived2, 1, wdRTTINoAllocator)
{
  WD_BEGIN_MESSAGEHANDLERS
  {
    WD_MESSAGE_HANDLER(GetValueMessage, OnGetValueMessage),
  }
  WD_END_MESSAGEHANDLERS;
}
WD_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

WD_CREATE_SIMPLE_TEST(Performance, Basics)
{
  const wdInt32 iNumObjects = 1000000;
  const float fNumObjects = (float)iNumObjects;

  wdDynamicArray<Derived1> Der1;
  Der1.SetCount(iNumObjects / 2);

  wdDynamicArray<Derived2> Der2;
  Der2.SetCount(iNumObjects / 2);

  wdDynamicArray<Base*> Objects;
  Objects.SetCount(iNumObjects);

  for (wdInt32 i = 0; i < iNumObjects; i += 2)
  {
    Objects[i] = &Der1[i / 2];
    Objects[i + 1] = &Der2[i / 2];
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Dispatch Message")
  {
    wdInt32 iResult = 0;

    // warm up
    for (wdUInt32 i = 0; i < iNumObjects; ++i)
    {
      GetValueMessage msg;
      Objects[i]->GetDynamicRTTI()->DispatchMessage(Objects[i], msg);
      iResult += msg.m_iValue;
    }

    wdTime t0 = wdTime::Now();

    for (wdUInt32 i = 0; i < iNumObjects; ++i)
    {
      GetValueMessage msg;
      Objects[i]->GetDynamicRTTI()->DispatchMessage(Objects[i], msg);
      iResult += msg.m_iValue;
    }

    wdTime t1 = wdTime::Now();

    WD_TEST_INT(iResult, iNumObjects * 1 + iNumObjects * 2);

    wdTime tdiff = t1 - t0;
    double tFC = tdiff.GetNanoseconds() / (double)iNumObjects;

    wdLog::Info("[test]Dispatch Message: {0}ns", wdArgF(tFC, 2), iResult);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Virtual")
  {
    wdInt32 iResult = 0;

    // warm up
    for (wdUInt32 i = 0; i < iNumObjects; ++i)
      iResult += Objects[i]->Virtual();

    wdTime t0 = wdTime::Now();

    for (wdUInt32 i = 0; i < iNumObjects; ++i)
      iResult += Objects[i]->Virtual();

    wdTime t1 = wdTime::Now();

    WD_TEST_INT(iResult, iNumObjects * 1 + iNumObjects * 2);

    wdTime tdiff = t1 - t0;
    double tFC = tdiff.GetNanoseconds() / (double)iNumObjects;

    wdLog::Info("[test]Virtual Function Calls: {0}ns", wdArgF(tFC, 2), iResult);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "NonVirtual")
  {
    wdInt32 iResult = 0;

    // warm up
    for (wdUInt32 i = 0; i < iNumObjects; i += 2)
    {
      iResult += ((Derived1*)Objects[i])->NonVirtual();
      iResult += ((Derived2*)Objects[i])->NonVirtual();
    }

    wdTime t0 = wdTime::Now();

    for (wdUInt32 i = 0; i < iNumObjects; i += 2)
    {
      iResult += ((Derived1*)Objects[i])->NonVirtual();
      iResult += ((Derived2*)Objects[i])->NonVirtual();
    }

    wdTime t1 = wdTime::Now();

    WD_TEST_INT(iResult, iNumObjects * 1 + iNumObjects * 2);

    wdTime tdiff = t1 - t0;
    double tFC = tdiff.GetNanoseconds() / (double)iNumObjects;

    wdLog::Info("[test]Non-Virtual Function Calls: {0}ns", wdArgF(tFC, 2), iResult);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "FastCall")
  {
    wdInt32 iResult = 0;

    // warm up
    for (wdUInt32 i = 0; i < iNumObjects; i += 2)
    {
      iResult += ((Derived1*)Objects[i])->FastCall();
      iResult += ((Derived2*)Objects[i])->FastCall();
    }

    wdTime t0 = wdTime::Now();

    for (wdUInt32 i = 0; i < iNumObjects; i += 2)
    {
      iResult += ((Derived1*)Objects[i])->FastCall();
      iResult += ((Derived2*)Objects[i])->FastCall();
    }

    wdTime t1 = wdTime::Now();

    WD_TEST_INT(iResult, iNumObjects * 1 + iNumObjects * 2);

    wdTime tdiff = t1 - t0;
    double tFC = tdiff.GetNanoseconds() / (double)iNumObjects;

    wdLog::Info("[test]FastCall Function Calls: {0}ns", wdArgF(tFC, 2), iResult);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "32 Bit Integer Division")
  {
    wdDynamicArray<wdInt32> Ints;
    Ints.SetCountUninitialized(iNumObjects);

    for (wdInt32 i = 0; i < iNumObjects; i += 1)
      Ints[i] = i * 100;

    wdTime t0 = wdTime::Now();

    wdInt32 iResult = 0;

    for (wdInt32 i = 1; i < iNumObjects; i += 1)
      iResult += Ints[i] / i;

    wdTime t1 = wdTime::Now();

    wdTime tdiff = t1 - t0;
    double t = tdiff.GetNanoseconds() / (double)(iNumObjects - 1);

    wdLog::Info("[test]32 Bit Integer Division: {0}ns", wdArgF(t, 2), iResult);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "32 Bit Integer Multiplication")
  {
    wdDynamicArray<wdInt32> Ints;
    Ints.SetCountUninitialized(iNumObjects);

    for (wdInt32 i = 0; i < iNumObjects; i += 1)
      Ints[i] = iNumObjects - i;

    wdTime t0 = wdTime::Now();

    wdInt32 iResult = 0;

    for (wdInt32 i = 0; i < iNumObjects; i += 1)
      iResult += Ints[i] * i;

    wdTime t1 = wdTime::Now();

    wdTime tdiff = t1 - t0;
    double t = tdiff.GetNanoseconds() / (double)(iNumObjects);

    wdLog::Info("[test]32 Bit Integer Multiplication: {0}ns", wdArgF(t, 2), iResult);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "64 Bit Integer Division")
  {
    wdDynamicArray<wdInt64> Ints;
    Ints.SetCountUninitialized(iNumObjects);

    for (wdInt32 i = 0; i < iNumObjects; i += 1)
      Ints[i] = (wdInt64)i * (wdInt64)100;

    wdTime t0 = wdTime::Now();

    wdInt64 iResult = 0;

    for (wdInt32 i = 1; i < iNumObjects; i += 1)
      iResult += Ints[i] / (wdInt64)i;

    wdTime t1 = wdTime::Now();

    wdTime tdiff = t1 - t0;
    double t = tdiff.GetNanoseconds() / (double)(iNumObjects - 1);

    wdLog::Info("[test]64 Bit Integer Division: {0}ns", wdArgF(t, 2), iResult);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "64 Bit Integer Multiplication")
  {
    wdDynamicArray<wdInt64> Ints;
    Ints.SetCountUninitialized(iNumObjects);

    for (wdInt32 i = 0; i < iNumObjects; i += 1)
      Ints[i] = iNumObjects - i;

    wdTime t0 = wdTime::Now();

    wdInt64 iResult = 0;

    for (wdInt32 i = 0; i < iNumObjects; i += 1)
      iResult += Ints[i] * (wdInt64)i;

    wdTime t1 = wdTime::Now();

    wdTime tdiff = t1 - t0;
    double t = tdiff.GetNanoseconds() / (double)(iNumObjects);

    wdLog::Info("[test]64 Bit Integer Multiplication: {0}ns", wdArgF(t, 2), iResult);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "32 Bit Float Division")
  {
    wdDynamicArray<float> Ints;
    Ints.SetCountUninitialized(iNumObjects);

    for (wdInt32 i = 0; i < iNumObjects; i += 1)
      Ints[i] = i * 100.0f;

    wdTime t0 = wdTime::Now();

    float fResult = 0;

    float d = 1.0f;
    for (wdInt32 i = 0; i < iNumObjects; i++, d += 1.0f)
      fResult += Ints[i] / d;

    wdTime t1 = wdTime::Now();

    wdTime tdiff = t1 - t0;
    double t = tdiff.GetNanoseconds() / (double)(iNumObjects);

    wdLog::Info("[test]32 Bit Float Division: {0}ns", wdArgF(t, 2), fResult);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "32 Bit Float Multiplication")
  {
    wdDynamicArray<float> Ints;
    Ints.SetCountUninitialized(iNumObjects);

    for (wdInt32 i = 0; i < iNumObjects; i++)
      Ints[i] = (float)(fNumObjects) - (float)(i);

    wdTime t0 = wdTime::Now();

    float iResult = 0;

    float d = 1.0f;
    for (wdInt32 i = 0; i < iNumObjects; i++, d += 1.0f)
      iResult += Ints[i] * d;

    wdTime t1 = wdTime::Now();

    wdTime tdiff = t1 - t0;
    double t = tdiff.GetNanoseconds() / (double)(iNumObjects);

    wdLog::Info("[test]32 Bit Float Multiplication: {0}ns", wdArgF(t, 2), iResult);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "64 Bit Double Division")
  {
    wdDynamicArray<double> Ints;
    Ints.SetCountUninitialized(iNumObjects);

    for (wdInt32 i = 0; i < iNumObjects; i += 1)
      Ints[i] = i * 100.0;

    wdTime t0 = wdTime::Now();

    double fResult = 0;

    double d = 1.0;
    for (wdInt32 i = 0; i < iNumObjects; i++, d += 1.0f)
      fResult += Ints[i] / d;

    wdTime t1 = wdTime::Now();

    wdTime tdiff = t1 - t0;
    double t = tdiff.GetNanoseconds() / (double)(iNumObjects);

    wdLog::Info("[test]64 Bit Double Division: {0}ns", wdArgF(t, 2), fResult);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "64 Bit Double Multiplication")
  {
    wdDynamicArray<double> Ints;
    Ints.SetCountUninitialized(iNumObjects);

    for (wdInt32 i = 0; i < iNumObjects; i++)
      Ints[i] = (double)(fNumObjects) - (double)(i);

    wdTime t0 = wdTime::Now();

    double iResult = 0;

    double d = 1.0;
    for (wdInt32 i = 0; i < iNumObjects; i++, d += 1.0)
      iResult += Ints[i] * d;

    wdTime t1 = wdTime::Now();

    wdTime tdiff = t1 - t0;
    double t = tdiff.GetNanoseconds() / (double)(iNumObjects);

    wdLog::Info("[test]64 Bit Double Multiplication: {0}ns", wdArgF(t, 2), iResult);
  }
}
