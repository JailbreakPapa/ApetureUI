#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Tracks/EventTrack.h>

WD_CREATE_SIMPLE_TEST_GROUP(Tracks);

WD_CREATE_SIMPLE_TEST(Tracks, EventTrack)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Empty")
  {
    wdEventTrack et;
    wdHybridArray<wdHashedString, 8> result;

    WD_TEST_BOOL(et.IsEmpty());
    et.Sample(wdTime::Zero(), wdTime::Seconds(1.0), result);

    WD_TEST_BOOL(result.IsEmpty());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Sample")
  {
    wdEventTrack et;
    wdHybridArray<wdHashedString, 8> result;

    et.AddControlPoint(wdTime::Seconds(3.0), "Event3");
    et.AddControlPoint(wdTime::Seconds(0.0), "Event0");
    et.AddControlPoint(wdTime::Seconds(4.0), "Event4");
    et.AddControlPoint(wdTime::Seconds(1.0), "Event1");
    et.AddControlPoint(wdTime::Seconds(2.0), "Event2");

    WD_TEST_BOOL(!et.IsEmpty());

    // sampling an empty range should yield no results, even if sampling an exact time where an event is
    result.Clear();
    {{et.Sample(wdTime::Seconds(0.0), wdTime::Seconds(0.0), result);
    WD_TEST_INT(result.GetCount(), 0);
  }

  {
    result.Clear();
    et.Sample(wdTime::Seconds(1.0), wdTime::Seconds(1.0), result);
    WD_TEST_INT(result.GetCount(), 0);
  }

  {
    result.Clear();
    et.Sample(wdTime::Seconds(4.0), wdTime::Seconds(4.0), result);
    WD_TEST_INT(result.GetCount(), 0);
  }
}

{
  result.Clear();
  et.Sample(wdTime::Seconds(0.0), wdTime::Seconds(1.0), result);
  WD_TEST_INT(result.GetCount(), 1);
  WD_TEST_STRING(result[0].GetString(), "Event0");
}

{
  result.Clear();
  et.Sample(wdTime::Seconds(0.0), wdTime::Seconds(2.0), result);
  WD_TEST_INT(result.GetCount(), 2);
  WD_TEST_STRING(result[0].GetString(), "Event0");
  WD_TEST_STRING(result[1].GetString(), "Event1");
}

{
  result.Clear();
  et.Sample(wdTime::Seconds(0.0), wdTime::Seconds(4.0), result);
  WD_TEST_INT(result.GetCount(), 4);
  WD_TEST_STRING(result[0].GetString(), "Event0");
  WD_TEST_STRING(result[1].GetString(), "Event1");
  WD_TEST_STRING(result[2].GetString(), "Event2");
  WD_TEST_STRING(result[3].GetString(), "Event3");
}

{
  result.Clear();
  et.Sample(wdTime::Seconds(0.0), wdTime::Seconds(10.0), result);
  WD_TEST_INT(result.GetCount(), 5);
  WD_TEST_STRING(result[0].GetString(), "Event0");
  WD_TEST_STRING(result[1].GetString(), "Event1");
  WD_TEST_STRING(result[2].GetString(), "Event2");
  WD_TEST_STRING(result[3].GetString(), "Event3");
  WD_TEST_STRING(result[4].GetString(), "Event4");
}

{
  result.Clear();
  et.Sample(wdTime::Seconds(-0.1), wdTime::Seconds(10.0), result);
  WD_TEST_INT(result.GetCount(), 5);
  WD_TEST_STRING(result[0].GetString(), "Event0");
  WD_TEST_STRING(result[1].GetString(), "Event1");
  WD_TEST_STRING(result[2].GetString(), "Event2");
  WD_TEST_STRING(result[3].GetString(), "Event3");
  WD_TEST_STRING(result[4].GetString(), "Event4");
}

et.Clear();
WD_TEST_BOOL(et.IsEmpty());
}


WD_TEST_BLOCK(wdTestBlock::Enabled, "Reverse Sample")
{
  wdEventTrack et;
  wdHybridArray<wdHashedString, 8> result;

  et.AddControlPoint(wdTime::Seconds(3.0), "Event3");
  et.AddControlPoint(wdTime::Seconds(0.0), "Event0");
  et.AddControlPoint(wdTime::Seconds(4.0), "Event4");
  et.AddControlPoint(wdTime::Seconds(1.0), "Event1");
  et.AddControlPoint(wdTime::Seconds(2.0), "Event2");

  {
    result.Clear();
    et.Sample(wdTime::Seconds(2.0), wdTime::Seconds(0.0), result);
    WD_TEST_INT(result.GetCount(), 2);
    WD_TEST_STRING(result[0].GetString(), "Event2");
    WD_TEST_STRING(result[1].GetString(), "Event1");
  }

  {
    result.Clear();
    et.Sample(wdTime::Seconds(4.0), wdTime::Seconds(0.0), result);
    WD_TEST_INT(result.GetCount(), 4);
    WD_TEST_STRING(result[0].GetString(), "Event4");
    WD_TEST_STRING(result[1].GetString(), "Event3");
    WD_TEST_STRING(result[2].GetString(), "Event2");
    WD_TEST_STRING(result[3].GetString(), "Event1");
  }

  {
    result.Clear();
    et.Sample(wdTime::Seconds(10.0), wdTime::Seconds(0.0), result);
    WD_TEST_INT(result.GetCount(), 4);
    WD_TEST_STRING(result[0].GetString(), "Event4");
    WD_TEST_STRING(result[1].GetString(), "Event3");
    WD_TEST_STRING(result[2].GetString(), "Event2");
    WD_TEST_STRING(result[3].GetString(), "Event1");
  }

  {
    result.Clear();
    et.Sample(wdTime::Seconds(10.0), wdTime::Seconds(-0.1), result);
    WD_TEST_INT(result.GetCount(), 5);
    WD_TEST_STRING(result[0].GetString(), "Event4");
    WD_TEST_STRING(result[1].GetString(), "Event3");
    WD_TEST_STRING(result[2].GetString(), "Event2");
    WD_TEST_STRING(result[3].GetString(), "Event1");
    WD_TEST_STRING(result[4].GetString(), "Event0");
  }
}
}
