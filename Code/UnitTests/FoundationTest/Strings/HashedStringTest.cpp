#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Strings/HashedString.h>

WD_CREATE_SIMPLE_TEST(Strings, HashedString)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor")
  {
    wdHashedString s;
    wdHashedString s2;

    s2.Assign("test"); // compile time hashing

    WD_TEST_INT(s.GetHash(), 0xef46db3751d8e999llu);
    WD_TEST_STRING(s.GetString().GetData(), "");
    WD_TEST_BOOL(s.GetString().IsEmpty());

    wdTempHashedString ts("test"); // compile time hashing
    WD_TEST_INT(ts.GetHash(), 0x4fdcca5ddb678139llu);

    wdStringBuilder sb = "test2";
    wdTempHashedString ts2(sb.GetData()); // runtime hashing
    WD_TEST_INT(ts2.GetHash(), 0x890e0a4c7111eb87llu);

    wdTempHashedString ts3(s2);
    WD_TEST_INT(ts3.GetHash(), 0x4fdcca5ddb678139llu);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Assign")
  {
    wdHashedString s;
    s.Assign("Test"); // compile time hashing

    WD_TEST_STRING(s.GetString().GetData(), "Test");
    WD_TEST_INT(s.GetHash(), 0xda83efc38a8922b4llu);

    wdStringBuilder sb = "test2";
    s.Assign(sb.GetData()); // runtime hashing
    WD_TEST_STRING(s.GetString().GetData(), "test2");
    WD_TEST_INT(s.GetHash(), 0x890e0a4c7111eb87llu);

    wdTempHashedString ts("dummy");
    ts = "test"; // compile time hashing
    WD_TEST_INT(ts.GetHash(), 0x4fdcca5ddb678139llu);

    ts = sb.GetData(); // runtime hashing
    WD_TEST_INT(ts.GetHash(), 0x890e0a4c7111eb87llu);

    s.Assign("");
    WD_TEST_INT(s.GetHash(), 0xef46db3751d8e999llu);
    WD_TEST_STRING(s.GetString().GetData(), "");
    WD_TEST_BOOL(s.GetString().IsEmpty());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "TempHashedString")
  {
    wdTempHashedString ts;
    wdHashedString hs;

    WD_TEST_INT(ts.GetHash(), hs.GetHash());

    WD_TEST_INT(ts.GetHash(), 0xef46db3751d8e999llu);

    ts = "Test";
    wdTempHashedString ts2 = ts;
    WD_TEST_INT(ts.GetHash(), 0xda83efc38a8922b4llu);

    ts = "";
    ts2.Clear();
    WD_TEST_INT(ts.GetHash(), 0xef46db3751d8e999llu);
    WD_TEST_INT(ts2.GetHash(), 0xef46db3751d8e999llu);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator== / operator!=")
  {
    wdHashedString s1, s2, s3, s4;
    s1.Assign("Test1");
    s2.Assign("Test2");
    s3.Assign("Test1");
    s4.Assign("Test2");

    wdTempHashedString t1("Test1");
    wdTempHashedString t2("Test2");

    WD_TEST_STRING(s1.GetString().GetData(), "Test1");
    WD_TEST_STRING(s2.GetString().GetData(), "Test2");
    WD_TEST_STRING(s3.GetString().GetData(), "Test1");
    WD_TEST_STRING(s4.GetString().GetData(), "Test2");

    WD_TEST_BOOL(s1 == s1);
    WD_TEST_BOOL(s2 == s2);
    WD_TEST_BOOL(s3 == s3);
    WD_TEST_BOOL(s4 == s4);
    WD_TEST_BOOL(t1 == t1);
    WD_TEST_BOOL(t2 == t2);

    WD_TEST_BOOL(s1 != s2);
    WD_TEST_BOOL(s1 == s3);
    WD_TEST_BOOL(s1 != s4);
    WD_TEST_BOOL(s1 == t1);
    WD_TEST_BOOL(s1 != t2);

    WD_TEST_BOOL(s2 != s3);
    WD_TEST_BOOL(s2 == s4);
    WD_TEST_BOOL(s2 != t1);
    WD_TEST_BOOL(s2 == t2);

    WD_TEST_BOOL(s3 != s4);
    WD_TEST_BOOL(s3 == t1);
    WD_TEST_BOOL(s3 != t2);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Copying")
  {
    wdHashedString s1;
    s1.Assign("blaa");

    wdHashedString s2(s1);
    wdHashedString s3;
    s3 = s2;

    WD_TEST_BOOL(s1 == s2);
    WD_TEST_BOOL(s1 == s3);

    wdHashedString s4(std::move(s2));
    wdHashedString s5;
    s5 = std::move(s3);

    WD_TEST_BOOL(s1 == s4);
    WD_TEST_BOOL(s1 == s5);
    WD_TEST_BOOL(s1 != s2);
    WD_TEST_BOOL(s1 != s3);

    wdTempHashedString t1("blaa");

    wdTempHashedString t2(t1);
    wdTempHashedString t3("urg");
    t3 = t2;

    WD_TEST_BOOL(t1 == t2);
    WD_TEST_BOOL(t1 == t3);

    t3 = s1;
    WD_TEST_INT(t3.GetHash(), s1.GetHash());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator<")
  {
    wdHashedString s1, s2, s3;
    s1.Assign("blaa");
    s2.Assign("blub");
    s3.Assign("tut");

    wdMap<wdHashedString, wdInt32> m; // uses operator< internally
    m[s1] = 1;
    m[s2] = 2;
    m[s3] = 3;

    WD_TEST_INT(m[s1], 1);
    WD_TEST_INT(m[s2], 2);
    WD_TEST_INT(m[s3], 3);

    wdTempHashedString t1("blaa");
    wdTempHashedString t2("blub");
    wdTempHashedString t3("tut");

    WD_TEST_BOOL((s1 < s1) == (t1 < t1));
    WD_TEST_BOOL((s1 < s2) == (t1 < t2));
    WD_TEST_BOOL((s1 < s3) == (t1 < t3));

    WD_TEST_BOOL((s1 < s1) == (s1 < t1));
    WD_TEST_BOOL((s1 < s2) == (s1 < t2));
    WD_TEST_BOOL((s1 < s3) == (s1 < t3));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetString")
  {
    wdHashedString s1, s2, s3;
    s1.Assign("blaa");
    s2.Assign("blub");
    s3.Assign("tut");

    WD_TEST_STRING(s1.GetString().GetData(), "blaa");
    WD_TEST_STRING(s2.GetString().GetData(), "blub");
    WD_TEST_STRING(s3.GetString().GetData(), "tut");
  }

#if WD_ENABLED(WD_HASHED_STRING_REF_COUNTING)
  WD_TEST_BLOCK(wdTestBlock::Enabled, "ClearUnusedStrings")
  {
    wdHashedString::ClearUnusedStrings();

    {
      wdHashedString s1, s2, s3;
      s1.Assign("blaa");
      s2.Assign("blub");
      s3.Assign("tut");
    }

    WD_TEST_INT(wdHashedString::ClearUnusedStrings(), 3);
    WD_TEST_INT(wdHashedString::ClearUnusedStrings(), 0);
  }
#endif
}
