#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/IO/MemoryStream.h>
#include <Foundation/Types/Tag.h>
#include <Foundation/Types/TagRegistry.h>
#include <Foundation/Types/TagSet.h>

static_assert(sizeof(wdTagSet) == 16);

#if WD_ENABLED(WD_PLATFORM_64BIT)
static_assert(sizeof(wdTag) == 16);
#else
static_assert(sizeof(wdTag) == 12);
#endif

WD_CREATE_SIMPLE_TEST(Basics, TagSet)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Basic Tag Tests")
  {
    wdTagRegistry TempTestRegistry;

    {
      wdTag TestTag;
      WD_TEST_BOOL(!TestTag.IsValid());
    }

    wdHashedString TagName;
    TagName.Assign("BASIC_TAG_TEST");

    const wdTag& SecondInstance = TempTestRegistry.RegisterTag(TagName);
    WD_TEST_BOOL(SecondInstance.IsValid());

    const wdTag* SecondInstance2 = TempTestRegistry.GetTagByName("BASIC_TAG_TEST");

    WD_TEST_BOOL(SecondInstance2 != nullptr);
    WD_TEST_BOOL(SecondInstance2->IsValid());

    WD_TEST_BOOL(&SecondInstance == SecondInstance2);

    WD_TEST_STRING(SecondInstance2->GetTagString(), "BASIC_TAG_TEST");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Basic Tag Registration")
  {
    wdTagRegistry TempTestRegistry;

    wdTag TestTag;

    WD_TEST_BOOL(!TestTag.IsValid());

    WD_TEST_BOOL(TempTestRegistry.GetTagByName("TEST_TAG1") == nullptr);

    TestTag = TempTestRegistry.RegisterTag("TEST_TAG1");

    WD_TEST_BOOL(TestTag.IsValid());

    WD_TEST_BOOL(TempTestRegistry.GetTagByName("TEST_TAG1") != nullptr);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Basic Tag Work")
  {
    wdTagRegistry TempTestRegistry;

    TempTestRegistry.RegisterTag("TEST_TAG1");

    const wdTag* TestTag1 = TempTestRegistry.GetTagByName("TEST_TAG1");
    WD_TEST_BOOL(TestTag1 != nullptr);

    const wdTag& TestTag2 = TempTestRegistry.RegisterTag("TEST_TAG2");

    WD_TEST_BOOL(TestTag2.IsValid());

    wdTagSet tagSet;

    WD_TEST_BOOL(tagSet.IsSet(*TestTag1) == false);
    WD_TEST_BOOL(tagSet.IsSet(TestTag2) == false);

    tagSet.Set(TestTag2);

    WD_TEST_BOOL(tagSet.IsSet(*TestTag1) == false);
    WD_TEST_BOOL(tagSet.IsSet(TestTag2) == true);
    WD_TEST_INT(tagSet.GetNumTagsSet(), 1);

    tagSet.Set(*TestTag1);

    WD_TEST_BOOL(tagSet.IsSet(*TestTag1) == true);
    WD_TEST_BOOL(tagSet.IsSet(TestTag2) == true);
    WD_TEST_INT(tagSet.GetNumTagsSet(), 2);

    tagSet.Remove(*TestTag1);

    WD_TEST_BOOL(tagSet.IsSet(*TestTag1) == false);
    WD_TEST_BOOL(tagSet.IsSet(TestTag2) == true);
    WD_TEST_INT(tagSet.GetNumTagsSet(), 1);

    wdTagSet tagSet2 = tagSet;
    WD_TEST_BOOL(tagSet2.IsSet(*TestTag1) == false);
    WD_TEST_BOOL(tagSet2.IsSet(TestTag2) == true);
    WD_TEST_INT(tagSet2.GetNumTagsSet(), 1);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Many Tags")
  {
    wdTagRegistry TempTestRegistry;

    // TagSets have local storage for 1 block (64 tags)
    // Allocate enough tags so the storage overflows (or doesn't start at block 0)
    // for these tests

    wdTag RegisteredTags[250];

    // Pre register some tags
    TempTestRegistry.RegisterTag("TEST_TAG1");
    TempTestRegistry.RegisterTag("TEST_TAG2");

    for (wdUInt32 i = 0; i < 250; ++i)
    {
      wdStringBuilder TagName;
      TagName.Format("TEST_TAG{0}", i);

      RegisteredTags[i] = TempTestRegistry.RegisterTag(TagName.GetData());

      WD_TEST_BOOL(RegisteredTags[i].IsValid());
    }

    WD_TEST_INT(TempTestRegistry.GetNumTags(), 250);

    // Set all tags
    wdTagSet BigTagSet;

    BigTagSet.Set(RegisteredTags[128]);
    BigTagSet.Set(RegisteredTags[64]);
    BigTagSet.Set(RegisteredTags[0]);

    WD_TEST_BOOL(BigTagSet.IsSet(RegisteredTags[0]));
    WD_TEST_BOOL(BigTagSet.IsSet(RegisteredTags[64]));
    WD_TEST_BOOL(BigTagSet.IsSet(RegisteredTags[128]));

    for (wdUInt32 i = 0; i < 250; ++i)
    {
      BigTagSet.Set(RegisteredTags[i]);
    }

    for (wdUInt32 i = 0; i < 250; ++i)
    {
      WD_TEST_BOOL(BigTagSet.IsSet(RegisteredTags[i]));
    }

    for (wdUInt32 i = 10; i < 60; ++i)
    {
      BigTagSet.Remove(RegisteredTags[i]);
    }

    for (wdUInt32 i = 0; i < 10; ++i)
    {
      WD_TEST_BOOL(BigTagSet.IsSet(RegisteredTags[i]));
    }

    for (wdUInt32 i = 10; i < 60; ++i)
    {
      WD_TEST_BOOL(!BigTagSet.IsSet(RegisteredTags[i]));
    }

    for (wdUInt32 i = 60; i < 250; ++i)
    {
      WD_TEST_BOOL(BigTagSet.IsSet(RegisteredTags[i]));
    }

    // Set tags, but starting outside block 0. This should do no allocation
    wdTagSet Non0BlockStartSet;
    Non0BlockStartSet.Set(RegisteredTags[100]);
    WD_TEST_BOOL(Non0BlockStartSet.IsSet(RegisteredTags[100]));
    WD_TEST_BOOL(!Non0BlockStartSet.IsSet(RegisteredTags[0]));

    wdTagSet Non0BlockStartSet2 = Non0BlockStartSet;
    WD_TEST_BOOL(Non0BlockStartSet2.IsSet(RegisteredTags[100]));
    WD_TEST_INT(Non0BlockStartSet2.GetNumTagsSet(), Non0BlockStartSet.GetNumTagsSet());

    // Also test allocating a tag in an earlier block than the first tag allocated in the set
    Non0BlockStartSet.Set(RegisteredTags[0]);
    WD_TEST_BOOL(Non0BlockStartSet.IsSet(RegisteredTags[100]));
    WD_TEST_BOOL(Non0BlockStartSet.IsSet(RegisteredTags[0]));

    // Copying a tag set should work as well
    wdTagSet SecondTagSet = BigTagSet;

    for (wdUInt32 i = 60; i < 250; ++i)
    {
      WD_TEST_BOOL(SecondTagSet.IsSet(RegisteredTags[i]));
    }

    for (wdUInt32 i = 10; i < 60; ++i)
    {
      WD_TEST_BOOL(!SecondTagSet.IsSet(RegisteredTags[i]));
    }

    WD_TEST_INT(SecondTagSet.GetNumTagsSet(), BigTagSet.GetNumTagsSet());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsAnySet")
  {
    wdTagRegistry TempTestRegistry;

    // TagSets have local storage for 1 block (64 tags)
    // Allocate enough tags so the storage overflows (or doesn't start at block 0)
    // for these tests

    wdTag RegisteredTags[250];

    for (wdUInt32 i = 0; i < 250; ++i)
    {
      wdStringBuilder TagName;
      TagName.Format("TEST_TAG{0}", i);

      RegisteredTags[i] = TempTestRegistry.RegisterTag(TagName.GetData());

      WD_TEST_BOOL(RegisteredTags[i].IsValid());
    }

    wdTagSet EmptyTagSet;
    wdTagSet SecondEmptyTagSet;

    WD_TEST_BOOL(!EmptyTagSet.IsAnySet(SecondEmptyTagSet));
    WD_TEST_BOOL(!SecondEmptyTagSet.IsAnySet(EmptyTagSet));


    wdTagSet SimpleSingleTagBlock0;
    SimpleSingleTagBlock0.Set(RegisteredTags[0]);

    wdTagSet SimpleSingleTagBlock1;
    SimpleSingleTagBlock1.Set(RegisteredTags[0]);

    WD_TEST_BOOL(!SecondEmptyTagSet.IsAnySet(SimpleSingleTagBlock0));

    WD_TEST_BOOL(SimpleSingleTagBlock0.IsAnySet(SimpleSingleTagBlock0));
    WD_TEST_BOOL(SimpleSingleTagBlock0.IsAnySet(SimpleSingleTagBlock1));

    SimpleSingleTagBlock1.Remove(RegisteredTags[0]);
    WD_TEST_BOOL(!SimpleSingleTagBlock1.IsAnySet(SimpleSingleTagBlock0));

    // Try with different block sizes/offsets (but same bit index)
    SimpleSingleTagBlock1.Set(RegisteredTags[64]);

    WD_TEST_BOOL(!SimpleSingleTagBlock1.IsAnySet(SimpleSingleTagBlock0));
    WD_TEST_BOOL(!SimpleSingleTagBlock0.IsAnySet(SimpleSingleTagBlock1));

    SimpleSingleTagBlock0.Set(RegisteredTags[65]);
    WD_TEST_BOOL(!SimpleSingleTagBlock1.IsAnySet(SimpleSingleTagBlock0));
    WD_TEST_BOOL(!SimpleSingleTagBlock0.IsAnySet(SimpleSingleTagBlock1));

    SimpleSingleTagBlock0.Set(RegisteredTags[64]);
    WD_TEST_BOOL(SimpleSingleTagBlock1.IsAnySet(SimpleSingleTagBlock0));
    WD_TEST_BOOL(SimpleSingleTagBlock0.IsAnySet(SimpleSingleTagBlock1));

    wdTagSet OffsetBlock;
    OffsetBlock.Set(RegisteredTags[65]);
    WD_TEST_BOOL(OffsetBlock.IsAnySet(SimpleSingleTagBlock0));
    WD_TEST_BOOL(SimpleSingleTagBlock0.IsAnySet(OffsetBlock));

    wdTagSet OffsetBlock2;
    OffsetBlock2.Set(RegisteredTags[66]);
    WD_TEST_BOOL(!OffsetBlock.IsAnySet(OffsetBlock2));
    WD_TEST_BOOL(!OffsetBlock2.IsAnySet(OffsetBlock));

    OffsetBlock2.Set(RegisteredTags[65]);
    WD_TEST_BOOL(OffsetBlock.IsAnySet(OffsetBlock2));
    WD_TEST_BOOL(OffsetBlock2.IsAnySet(OffsetBlock));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Add / Remove / IsEmpty / Clear")
  {
    wdTagRegistry TempTestRegistry;

    TempTestRegistry.RegisterTag("TEST_TAG1");

    const wdTag* TestTag1 = TempTestRegistry.GetTagByName("TEST_TAG1");
    WD_TEST_BOOL(TestTag1 != nullptr);

    const wdTag& TestTag2 = TempTestRegistry.RegisterTag("TEST_TAG2");

    WD_TEST_BOOL(TestTag2.IsValid());

    wdTagSet tagSet;

    WD_TEST_BOOL(tagSet.IsEmpty());
    WD_TEST_BOOL(tagSet.IsSet(*TestTag1) == false);
    WD_TEST_BOOL(tagSet.IsSet(TestTag2) == false);

    tagSet.Clear();

    WD_TEST_BOOL(tagSet.IsEmpty());
    WD_TEST_BOOL(tagSet.IsSet(*TestTag1) == false);
    WD_TEST_BOOL(tagSet.IsSet(TestTag2) == false);

    tagSet.Set(TestTag2);

    WD_TEST_BOOL(!tagSet.IsEmpty());
    WD_TEST_BOOL(tagSet.IsSet(*TestTag1) == false);
    WD_TEST_BOOL(tagSet.IsSet(TestTag2) == true);

    tagSet.Remove(TestTag2);

    WD_TEST_BOOL(tagSet.IsEmpty());
    WD_TEST_BOOL(tagSet.IsSet(*TestTag1) == false);
    WD_TEST_BOOL(tagSet.IsSet(TestTag2) == false);

    tagSet.Set(*TestTag1);
    tagSet.Set(TestTag2);

    WD_TEST_BOOL(!tagSet.IsEmpty());
    WD_TEST_BOOL(tagSet.IsSet(*TestTag1) == true);
    WD_TEST_BOOL(tagSet.IsSet(TestTag2) == true);

    tagSet.Remove(*TestTag1);
    tagSet.Remove(TestTag2);

    WD_TEST_BOOL(tagSet.IsEmpty());
    WD_TEST_BOOL(tagSet.IsSet(*TestTag1) == false);
    WD_TEST_BOOL(tagSet.IsSet(TestTag2) == false);

    tagSet.Set(*TestTag1);
    tagSet.Set(TestTag2);

    WD_TEST_BOOL(!tagSet.IsEmpty());
    WD_TEST_BOOL(tagSet.IsSet(*TestTag1) == true);
    WD_TEST_BOOL(tagSet.IsSet(TestTag2) == true);

    tagSet.Clear();

    WD_TEST_BOOL(tagSet.IsEmpty());
    WD_TEST_BOOL(tagSet.IsSet(*TestTag1) == false);
    WD_TEST_BOOL(tagSet.IsSet(TestTag2) == false);
  }
}
