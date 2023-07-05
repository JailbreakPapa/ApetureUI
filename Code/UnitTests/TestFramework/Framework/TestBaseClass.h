#pragma once

#include <Foundation/Basics.h>
#include <Foundation/Types/Status.h>
#include <Foundation/Utilities/EnumerableClass.h>
#include <TestFramework/Framework/Declarations.h>

struct wdTestConfiguration;
class wdImage;

class WD_TEST_DLL wdTestBaseClass : public wdEnumerable<wdTestBaseClass>
{
  friend class wdTestFramework;

  WD_DECLARE_ENUMERABLE_CLASS(wdTestBaseClass);

public:
  // *** Override these functions to implement the required test functionality ***

  /// Override this function to give the test a proper name.
  virtual const char* GetTestName() const /*override*/ = 0;

  const char* GetSubTestName(wdInt32 iIdentifier) const;

  /// Override this function to add additional information to the test configuration
  virtual void UpdateConfiguration(wdTestConfiguration& ref_config) const /*override*/;

  /// \brief Implement this to add support for image comparisons. See WD_TEST_IMAGE_MSG.
  virtual wdResult GetImage(wdImage& ref_img) { return WD_FAILURE; }

  /// \brief Implement this to add support for depth buffer image comparisons. See WD_TEST_DEPTH_IMAGE_MSG.
  virtual wdResult GetDepthImage(wdImage& ref_img) { return WD_FAILURE; }

  /// \brief Used to map the 'number' for an image comparison, to a string used for finding the comparison image.
  ///
  /// By default image comparison screenshots are called 'TestName_SubTestName_XYZ'
  /// This can be fully overridden to use any other file name.
  /// The location of the comparison images (ie the folder) cannot be specified at the moment.
  virtual void MapImageNumberToString(const char* szTestName, const char* szSubTestName, wdUInt32 uiImageNumber, wdStringBuilder& out_sString) const;

protected:
  /// Called at startup to determine if the test can be run. Should return a detailed error message on failure.
  virtual std::string IsTestAvailable() const { return {}; };
  /// Called at startup to setup all tests. Should use 'AddSubTest' to register all the sub-tests to the test framework.
  virtual void SetupSubTests() = 0;
  /// Called to run the test that was registered with the given identifier.
  virtual wdTestAppRun RunSubTest(wdInt32 iIdentifier, wdUInt32 uiInvocationCount) = 0;

  // *** Override these functions to implement optional (de-)initialization ***

  /// Called to initialize the whole test.
  virtual wdResult InitializeTest() { return WD_SUCCESS; }
  /// Called to deinitialize the whole test.
  virtual wdResult DeInitializeTest() { return WD_SUCCESS; }
  /// Called before running a sub-test to do additional initialization specifically for that test.
  virtual wdResult InitializeSubTest(wdInt32 iIdentifier) { return WD_SUCCESS; }
  /// Called after running a sub-test to do additional deinitialization specifically for that test.
  virtual wdResult DeInitializeSubTest(wdInt32 iIdentifier) { return WD_SUCCESS; }


  /// Adds a sub-test to the test suite. The index is used to identify it when running the sub-tests.
  void AddSubTest(const char* szName, wdInt32 iIdentifier);

private:
  struct TestEntry
  {
    TestEntry()
    {
      m_szName = "";
      m_iIdentifier = -1;
    }

    const char* m_szName;
    wdInt32 m_iIdentifier;
  };

  /// Removes all sub-tests.
  void ClearSubTests();

  // Called by wdTestFramework.
  wdResult DoTestInitialization();
  void DoTestDeInitialization();
  wdResult DoSubTestInitialization(wdInt32 iIdentifier);
  void DoSubTestDeInitialization(wdInt32 iIdentifier);
  wdTestAppRun DoSubTestRun(wdInt32 iIdentifier, double& fDuration, wdUInt32 uiInvocationCount);


  std::deque<TestEntry> m_Entries;
};

#define WD_CREATE_TEST(TestClass)
