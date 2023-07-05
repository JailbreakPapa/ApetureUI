#include <ToolsFoundationTest/ToolsFoundationTestPCH.h>

#include <Foundation/Types/Uuid.h>
#include <ToolsFoundation/Object/ObjectMetaData.h>


WD_CREATE_SIMPLE_TEST(DocumentObject, ObjectMetaData)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Pointers / int")
  {
    wdObjectMetaData<void*, wdInt32> meta;

    int a = 0, b = 1, c = 2, d = 3;

    WD_TEST_BOOL(!meta.HasMetaData(&a));
    WD_TEST_BOOL(!meta.HasMetaData(&b));
    WD_TEST_BOOL(!meta.HasMetaData(&c));
    WD_TEST_BOOL(!meta.HasMetaData(&d));

    {
      auto pData = meta.BeginModifyMetaData(&a);
      *pData = a;
      meta.EndModifyMetaData();

      pData = meta.BeginModifyMetaData(&b);
      *pData = b;
      meta.EndModifyMetaData();

      pData = meta.BeginModifyMetaData(&c);
      *pData = c;
      meta.EndModifyMetaData();
    }

    WD_TEST_BOOL(meta.HasMetaData(&a));
    WD_TEST_BOOL(meta.HasMetaData(&b));
    WD_TEST_BOOL(meta.HasMetaData(&c));
    WD_TEST_BOOL(!meta.HasMetaData(&d));

    {
      auto pDataR = meta.BeginReadMetaData(&a);
      WD_TEST_INT(*pDataR, a);
      meta.EndReadMetaData();

      pDataR = meta.BeginReadMetaData(&b);
      WD_TEST_INT(*pDataR, b);
      meta.EndReadMetaData();

      pDataR = meta.BeginReadMetaData(&c);
      WD_TEST_INT(*pDataR, c);
      meta.EndReadMetaData();

      pDataR = meta.BeginReadMetaData(&d);
      WD_TEST_INT(*pDataR, 0);
      meta.EndReadMetaData();
    }
  }

  struct md
  {
    md() { b = false; }

    wdString s;
    bool b;
  };

  WD_TEST_BLOCK(wdTestBlock::Enabled, "UUID / struct")
  {
    wdObjectMetaData<wdUuid, md> meta;

    const int num = 100;

    wdDynamicArray<wdUuid> obj;
    obj.SetCount(num);

    for (wdUInt32 i = 0; i < num; ++i)
    {
      wdUuid& uid = obj[i];
      uid.CreateNewUuid();

      if (wdMath::IsEven(i))
      {
        auto d1 = meta.BeginModifyMetaData(uid);
        d1->b = true;
        d1->s = "test";

        meta.EndModifyMetaData();
      }

      WD_TEST_BOOL(meta.HasMetaData(uid) == wdMath::IsEven(i));
    }

    for (wdUInt32 i = 0; i < num; ++i)
    {
      const wdUuid& uid = obj[i];

      auto p = meta.BeginReadMetaData(uid);

      WD_TEST_BOOL(p->b == wdMath::IsEven(i));

      if (wdMath::IsEven(i))
      {
        WD_TEST_STRING(p->s, "test");
      }
      else
      {
        WD_TEST_BOOL(p->s.IsEmpty());
      }

      meta.EndReadMetaData();
      WD_TEST_BOOL(meta.HasMetaData(uid) == wdMath::IsEven(i));
    }
  }
}
