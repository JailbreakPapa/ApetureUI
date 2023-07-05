#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/IO/MemoryStream.h>
#include <Foundation/Types/Uuid.h>


WD_CREATE_SIMPLE_TEST(Basics, Uuid)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Uuid Generation")
  {
    wdUuid ShouldBeInvalid;

    WD_TEST_BOOL(ShouldBeInvalid.IsValid() == false);

    wdUuid FirstGenerated;
    FirstGenerated.CreateNewUuid();
    WD_TEST_BOOL(FirstGenerated.IsValid());

    wdUuid SecondGenerated;
    SecondGenerated.CreateNewUuid();
    WD_TEST_BOOL(SecondGenerated.IsValid());

    WD_TEST_BOOL(!(FirstGenerated == SecondGenerated));
    WD_TEST_BOOL(FirstGenerated != SecondGenerated);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Uuid Serialization")
  {
    wdUuid Uuid;
    WD_TEST_BOOL(Uuid.IsValid() == false);

    Uuid.CreateNewUuid();
    WD_TEST_BOOL(Uuid.IsValid());

    wdDefaultMemoryStreamStorage StreamStorage;

    // Create reader
    wdMemoryStreamReader StreamReader(&StreamStorage);

    // Create writer
    wdMemoryStreamWriter StreamWriter(&StreamStorage);

    StreamWriter << Uuid;

    wdUuid ReadBack;
    WD_TEST_BOOL(ReadBack.IsValid() == false);

    StreamReader >> ReadBack;

    WD_TEST_BOOL(ReadBack == Uuid);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Stable Uuid From String")
  {
    wdUuid uuid1 = wdUuid::StableUuidForString("TEST 1");
    wdUuid uuid2 = wdUuid::StableUuidForString("TEST 2");
    wdUuid uuid3 = wdUuid::StableUuidForString("TEST 1");

    WD_TEST_BOOL(uuid1 == uuid3);
    WD_TEST_BOOL(uuid1 != uuid2);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Uuid Combine")
  {
    wdUuid uuid1;
    uuid1.CreateNewUuid();
    wdUuid uuid2;
    uuid2.CreateNewUuid();
    wdUuid combined = uuid1;
    combined.CombineWithSeed(uuid2);
    WD_TEST_BOOL(combined != uuid1);
    WD_TEST_BOOL(combined != uuid2);
    combined.RevertCombinationWithSeed(uuid2);
    WD_TEST_BOOL(combined == uuid1);

    wdUuid hashA = uuid1;
    hashA.HashCombine(uuid2);
    wdUuid hashB = uuid2;
    hashA.HashCombine(uuid1);
    WD_TEST_BOOL(hashA != hashB);
  }
}
