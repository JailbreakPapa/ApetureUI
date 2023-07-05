#pragma once

#include <Foundation/Basics.h>
#include <Foundation/Utilities/EnumerableClass.h>
#include <TestFramework/Framework/Declarations.h>
#include <TestFramework/Framework/TestBaseClass.h>

class WD_TEST_DLL wdSimpleTestGroup : public wdTestBaseClass
{
public:
  typedef void (*SimpleTestFunc)();

  wdSimpleTestGroup(const char* szName)
    : m_szTestName(szName)
  {
  }

  void AddSimpleTest(const char* szName, SimpleTestFunc testFunc);

  virtual const char* GetTestName() const override { return m_szTestName; }

private:
  virtual void SetupSubTests() override;
  virtual wdTestAppRun RunSubTest(wdInt32 iIdentifier, wdUInt32 uiInvocationCount) override;
  virtual wdResult InitializeSubTest(wdInt32 iIdentifier) override;
  virtual wdResult DeInitializeSubTest(wdInt32 iIdentifier) override;

private:
  struct SimpleTestEntry
  {
    const char* m_szName;
    SimpleTestFunc m_Func;
  };

  const char* m_szTestName;
  std::deque<SimpleTestEntry> m_SimpleTests;
};

class WD_TEST_DLL wdRegisterSimpleTestHelper : public wdEnumerable<wdRegisterSimpleTestHelper>
{
  WD_DECLARE_ENUMERABLE_CLASS(wdRegisterSimpleTestHelper);

public:
  wdRegisterSimpleTestHelper(wdSimpleTestGroup* pTestGroup, const char* szTestName, wdSimpleTestGroup::SimpleTestFunc func)
  {
    m_pTestGroup = pTestGroup;
    m_szTestName = szTestName;
    m_Func = func;
  }

  void RegisterTest() { m_pTestGroup->AddSimpleTest(m_szTestName, m_Func); }

private:
  wdSimpleTestGroup* m_pTestGroup;
  const char* m_szTestName;
  wdSimpleTestGroup::SimpleTestFunc m_Func;
};

#define WD_CREATE_SIMPLE_TEST_GROUP(GroupName) wdSimpleTestGroup WD_CONCAT(g_SimpleTestGroup__, GroupName)(WD_STRINGIZE(GroupName));

#define WD_CREATE_SIMPLE_TEST(GroupName, TestName)                                                                                                   \
  extern wdSimpleTestGroup WD_CONCAT(g_SimpleTestGroup__, GroupName);                                                                                \
  static void wdSimpleTestFunction__##GroupName##_##TestName();                                                                                      \
  wdRegisterSimpleTestHelper wdRegisterSimpleTest__##GroupName##TestName(                                                                            \
    &WD_CONCAT(g_SimpleTestGroup__, GroupName), WD_STRINGIZE(TestName), wdSimpleTestFunction__##GroupName##_##TestName);                             \
  static void wdSimpleTestFunction__##GroupName##_##TestName()
