#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Configuration/Singleton.h>

class TestSingleton
{
  WD_DECLARE_SINGLETON(TestSingleton);

public:
  TestSingleton()
    : m_SingletonRegistrar(this)
  {
  }

  wdInt32 m_iValue = 41;
};

WD_IMPLEMENT_SINGLETON(TestSingleton);

class SingletonInterface
{
public:
  virtual wdInt32 GetValue() = 0;
};

class TestSingletonOfInterface : public SingletonInterface
{
  WD_DECLARE_SINGLETON_OF_INTERFACE(TestSingletonOfInterface, SingletonInterface);

public:
  TestSingletonOfInterface()
    : m_SingletonRegistrar(this)
  {
  }

  virtual wdInt32 GetValue() { return 23; }
};

WD_IMPLEMENT_SINGLETON(TestSingletonOfInterface);


WD_CREATE_SIMPLE_TEST(Configuration, Singleton)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Singleton Registration")
  {
    {
      TestSingleton* pSingleton = wdSingletonRegistry::GetSingletonInstance<TestSingleton>();
      WD_TEST_BOOL(pSingleton == nullptr);
    }

    {
      TestSingleton g_Singleton;

      {
        TestSingleton* pSingleton = wdSingletonRegistry::GetSingletonInstance<TestSingleton>();
        WD_TEST_BOOL(pSingleton == &g_Singleton);
        WD_TEST_INT(pSingleton->m_iValue, 41);
      }
    }

    {
      TestSingleton* pSingleton = wdSingletonRegistry::GetSingletonInstance<TestSingleton>();
      WD_TEST_BOOL(pSingleton == nullptr);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Singleton of Interface")
  {
    {
      SingletonInterface* pSingleton = wdSingletonRegistry::GetSingletonInstance<SingletonInterface>();
      WD_TEST_BOOL(pSingleton == nullptr);
    }

    {
      TestSingletonOfInterface g_Singleton;

      {
        SingletonInterface* pSingleton = wdSingletonRegistry::GetSingletonInstance<SingletonInterface>();
        WD_TEST_BOOL(pSingleton == &g_Singleton);
        WD_TEST_INT(pSingleton->GetValue(), 23);
      }

      {
        TestSingletonOfInterface* pSingleton = wdSingletonRegistry::GetSingletonInstance<TestSingletonOfInterface>();
        WD_TEST_BOOL(pSingleton == &g_Singleton);
        WD_TEST_INT(pSingleton->GetValue(), 23);
      }

      {
        SingletonInterface* pSingleton = wdSingletonRegistry::GetRequiredSingletonInstance<SingletonInterface>();
        WD_TEST_BOOL(pSingleton == &g_Singleton);
        WD_TEST_INT(pSingleton->GetValue(), 23);
      }

      {
        TestSingletonOfInterface* pSingleton = wdSingletonRegistry::GetRequiredSingletonInstance<TestSingletonOfInterface>();
        WD_TEST_BOOL(pSingleton == &g_Singleton);
        WD_TEST_INT(pSingleton->GetValue(), 23);
      }
    }

    {
      SingletonInterface* pSingleton = wdSingletonRegistry::GetSingletonInstance<SingletonInterface>();
      WD_TEST_BOOL(pSingleton == nullptr);
    }

    {
      TestSingletonOfInterface* pSingleton = wdSingletonRegistry::GetSingletonInstance<TestSingletonOfInterface>();
      WD_TEST_BOOL(pSingleton == nullptr);
    }
  }
}
