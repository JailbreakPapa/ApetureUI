#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Containers/DynamicArray.h>
#include <Foundation/Containers/IdTable.h>
#include <Foundation/Strings/String.h>

namespace
{
  typedef wdGenericId<32, 16> Id;
  typedef wdConstructionCounter st;

  struct TestObject
  {
    int x;
    wdString s;
  };
} // namespace

WD_CREATE_SIMPLE_TEST(Containers, IdTable)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor")
  {
    wdIdTable<Id, wdInt32> table;

    WD_TEST_BOOL(table.GetCount() == 0);
    WD_TEST_BOOL(table.IsEmpty());

    wdUInt32 counter = 0;
    for (wdIdTable<Id, wdInt32>::ConstIterator it = table.GetIterator(); it.IsValid(); ++it)
    {
      ++counter;
    }
    WD_TEST_INT(counter, 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Copy Constructor/Assignment/Iterator")
  {
    WD_TEST_BOOL(st::HasAllDestructed());
    {
      wdIdTable<Id, st> table1;

      for (wdInt32 i = 0; i < 200; ++i)
      {
        table1.Insert(st(i));
      }

      WD_TEST_BOOL(table1.Remove(Id(0, 1)));

      for (wdInt32 i = 0; i < 99; ++i)
      {
        Id id;
        id.m_Generation = 1;

        do
        {
          id.m_InstanceIndex = rand() % 200;
        } while (!table1.Contains(id));

        WD_TEST_BOOL(table1.Remove(id));
      }

      wdIdTable<Id, st> table2;
      table2 = table1;
      wdIdTable<Id, st> table3(table1);

      WD_TEST_BOOL(table2.IsFreelistValid());
      WD_TEST_BOOL(table3.IsFreelistValid());

      WD_TEST_INT(table1.GetCount(), 100);
      WD_TEST_INT(table2.GetCount(), 100);
      WD_TEST_INT(table3.GetCount(), 100);

      wdUInt32 uiCounter = 0;
      for (wdIdTable<Id, st>::ConstIterator it = table1.GetIterator(); it.IsValid(); ++it)
      {
        st value;

        WD_TEST_BOOL(table2.TryGetValue(it.Id(), value));
        WD_TEST_BOOL(it.Value() == value);

        WD_TEST_BOOL(table3.TryGetValue(it.Id(), value));
        WD_TEST_BOOL(it.Value() == value);

        ++uiCounter;
      }
      WD_TEST_INT(uiCounter, table1.GetCount());

      for (wdIdTable<Id, st>::Iterator it = table1.GetIterator(); it.IsValid(); ++it)
      {
        it.Value() = st(42);
      }

      for (wdIdTable<Id, st>::ConstIterator it = table1.GetIterator(); it.IsValid(); ++it)
      {
        st value;

        WD_TEST_BOOL(table1.TryGetValue(it.Id(), value));
        WD_TEST_BOOL(it.Value() == value);
        WD_TEST_BOOL(value.m_iData == 42);
      }
    }
    WD_TEST_BOOL(st::HasAllDestructed());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Verify 0 is never valid")
  {
    wdIdTable<Id, TestObject> table;

    wdUInt32 count1 = 0, count2 = 0;

    TestObject x = {11, "Test"};

    while (true)
    {
      Id id = table.Insert(x);
      WD_TEST_BOOL(id.m_Generation != 0);

      WD_TEST_BOOL(table.Remove(id));

      if (id.m_Generation > 1) // until all elements in generation 1 have been used up
        break;

      ++count1;
    }

    WD_TEST_BOOL(!table.Contains(Id(0, 0)));

    while (true)
    {
      Id id = table.Insert(x);
      WD_TEST_BOOL(id.m_Generation != 0);

      WD_TEST_BOOL(table.Remove(id));

      if (id.m_Generation == 1) // wrap around
        break;

      ++count2;
    }

    WD_TEST_BOOL(!table.Contains(Id(0, 0)));

    WD_TEST_INT(count1, 32);
    WD_TEST_INT(count2, 2097087);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Insert/Remove")
  {
    wdIdTable<Id, TestObject> table;

    for (int i = 0; i < 100; i++)
    {
      TestObject x = {rand(), "Test"};
      Id id = table.Insert(x);
      WD_TEST_INT(id.m_InstanceIndex, i);
      WD_TEST_INT(id.m_Generation, 1);

      WD_TEST_BOOL(table.Contains(id));

      TestObject y = table[id];
      WD_TEST_INT(x.x, y.x);
      WD_TEST_BOOL(x.s == y.s);
    }
    WD_TEST_INT(table.GetCount(), 100);

    Id ids[10] = {Id(13, 1), Id(0, 1), Id(16, 1), Id(34, 1), Id(56, 1), Id(57, 1), Id(79, 1), Id(85, 1), Id(91, 1), Id(97, 1)};


    for (int i = 0; i < 10; i++)
    {
      bool res = table.Remove(ids[i]);
      WD_TEST_BOOL(res);
      WD_TEST_BOOL(!table.Contains(ids[i]));
    }
    WD_TEST_INT(table.GetCount(), 90);

    for (int i = 0; i < 40; i++)
    {
      TestObject x = {1000, "Bla. This is a very long string which does not fit into 32 byte and will cause memory allocations."};
      Id newId = table.Insert(x);

      WD_TEST_BOOL(table.Contains(newId));

      TestObject y = table[newId];
      WD_TEST_INT(x.x, y.x);
      WD_TEST_BOOL(x.s == y.s);

      TestObject* pObj;
      WD_TEST_BOOL(table.TryGetValue(newId, pObj));
      WD_TEST_BOOL(pObj->s == x.s);
    }
    WD_TEST_INT(table.GetCount(), 130);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Crash test")
  {
    wdIdTable<Id, TestObject> table;
    wdDynamicArray<Id> ids;

    for (wdUInt32 i = 0; i < 100000; ++i)
    {
      int action = rand() % 2;
      if (action == 0)
      {
        TestObject x = {rand(), "Test"};
        ids.PushBack(table.Insert(x));
      }
      else
      {
        if (ids.GetCount() > 0)
        {
          wdUInt32 index = rand() % ids.GetCount();
          WD_TEST_BOOL(table.Remove(ids[index]));
          ids.RemoveAtAndSwap(index);
        }
      }

      WD_TEST_BOOL(table.IsFreelistValid());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Clear")
  {
    WD_TEST_BOOL(st::HasAllDestructed());

    wdIdTable<Id, st> m1;
    Id id0 = m1.Insert(st(1));
    WD_TEST_BOOL(st::HasDone(2, 1)); // for inserting new elements 1 temporary is created (and destroyed)

    Id id1 = m1.Insert(st(3));
    WD_TEST_BOOL(st::HasDone(2, 1)); // for inserting new elements 1 temporary is created (and destroyed)

    m1[id0] = st(2);
    WD_TEST_BOOL(st::HasDone(1, 1)); // nothing new to create, so only the one temporary is used

    m1.Clear();
    WD_TEST_BOOL(st::HasDone(0, 2));
    WD_TEST_BOOL(st::HasAllDestructed());

    WD_TEST_BOOL(!m1.Contains(id0));
    WD_TEST_BOOL(!m1.Contains(id1));
    WD_TEST_BOOL(m1.IsFreelistValid());
  }

  /*WD_TEST_BLOCK(wdTestBlock::Enabled, "Remove/Compact")
  {
    wdIdTable<Id, st> a;

    for (wdInt32 i = 0; i < 1000; ++i)
    {
      a.Insert(i);
      WD_TEST_INT(a.GetCount(), i + 1);
    }

    a.Compact();
    WD_TEST_BOOL(a.IsFreelistValid());

    {
      wdUInt32 i = 0;
      for (wdIdTable<Id, st>::Iterator it = a.GetIterator(); it.IsValid(); ++it)
      {
        WD_TEST_INT(a[it.Id()].m_iData, i);
        ++i;
      }
    }

    for (wdInt32 i = 500; i < 1000; ++i)
    {
      st oldValue;
      WD_TEST_BOOL(a.Remove(Id(i, 0), &oldValue));
      WD_TEST_INT(oldValue.m_iData, i);
    }

    a.Compact();
    WD_TEST_BOOL(a.IsFreelistValid());

    {
      wdUInt32 i = 0;
      for (wdIdTable<Id, st>::Iterator it = a.GetIterator(); it.IsValid(); ++it)
      {
        WD_TEST_INT(a[it.Id()].m_iData, i);
        ++i;
      }
    }
  }*/
}
