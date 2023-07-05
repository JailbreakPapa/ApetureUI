#include <FoundationTest/FoundationTestPCH.h>

// This test does not actually run, it tests compile time stuff

namespace
{
  struct AggregatePod
  {
    int m_1;
    float m_2;

    WD_DETECT_TYPE_CLASS(int, float);
  };

  struct AggregatePod2
  {
    int m_1;
    float m_2;
    AggregatePod m_3;

    WD_DETECT_TYPE_CLASS(int, float, AggregatePod);
  };

  struct MemRelocateable
  {
    WD_DECLARE_MEM_RELOCATABLE_TYPE();
  };

  struct AggregateMemRelocateable
  {
    int m_1;
    float m_2;
    AggregatePod m_3;
    MemRelocateable m_4;

    WD_DETECT_TYPE_CLASS(int, float, AggregatePod, MemRelocateable);
  };

  class ClassType
  {
  };

  struct AggregateClass
  {
    int m_1;
    float m_2;
    AggregatePod m_3;
    MemRelocateable m_4;
    ClassType m_5;

    WD_DETECT_TYPE_CLASS(int, float, AggregatePod, MemRelocateable, ClassType);
  };

  WD_CHECK_AT_COMPILETIME(wdGetTypeClass<AggregatePod>::value == wdTypeIsPod::value);
  WD_CHECK_AT_COMPILETIME(wdGetTypeClass<AggregatePod2>::value == wdTypeIsPod::value);
  WD_CHECK_AT_COMPILETIME(wdGetTypeClass<MemRelocateable>::value == wdTypeIsMemRelocatable::value);
  WD_CHECK_AT_COMPILETIME(wdGetTypeClass<AggregateMemRelocateable>::value == wdTypeIsMemRelocatable::value);
  WD_CHECK_AT_COMPILETIME(wdGetTypeClass<ClassType>::value == wdTypeIsClass::value);
  WD_CHECK_AT_COMPILETIME(wdGetTypeClass<AggregateClass>::value == wdTypeIsClass::value);
} // namespace
