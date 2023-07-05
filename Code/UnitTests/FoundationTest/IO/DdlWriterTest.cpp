#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/IO/OSFile.h>
#include <Foundation/IO/OpenDdlWriter.h>
#include <FoundationTest/IO/JSONTestHelpers.h>


WD_CREATE_SIMPLE_TEST(IO, DdlWriter)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Object")
  {
    StreamComparer sc("TestObject{}\n");

    wdOpenDdlWriter js;
    js.SetOutputStream(&sc);

    js.BeginObject("TestObject");
    js.EndObject();
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Named Object (global)")
  {
    StreamComparer sc("TestObject $ObjName{}\n");

    wdOpenDdlWriter js;
    js.SetOutputStream(&sc);

    js.BeginObject("TestObject", "ObjName", true);
    js.EndObject();
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Named Object (local)")
  {
    StreamComparer sc("TestObject %ObjName{}\n");

    wdOpenDdlWriter js;
    js.SetOutputStream(&sc);

    js.BeginObject("TestObject", "ObjName", false);
    js.EndObject();
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Object Hierarchy")
  {
    StreamComparer sc("obj1\n\
{\n\
	obj11 $a\n\
	{\n\
		obj111 %b{}\n\
		obj112{}\n\
	}\n\
	obj12 %c{}\n\
}\n\
obj2{}\n");

    wdOpenDdlWriter js;
    js.SetOutputStream(&sc);

    js.BeginObject("obj1");
    {
      js.BeginObject("obj11", "a", true);
      {
        {
          js.BeginObject("obj111", "b", false);
          js.EndObject();
        }
        {
          js.BeginObject("obj112");
          js.EndObject();
        }
      }
      js.EndObject();
      js.BeginObject("obj12", "c");
      {
      }
      js.EndObject();
    }
    js.EndObject();
    js.BeginObject("obj2");
    js.EndObject();
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Empty Primitive List")
  {
    StreamComparer sc("Data\n\
{\n\
	bool{}\n\
}\n");

    wdOpenDdlWriter js;
    js.SetOutputStream(&sc);

    js.BeginObject("Data");
    {
      js.BeginPrimitiveList(wdOpenDdlPrimitiveType::Bool);
      {
      }
      js.EndPrimitiveList();
    }
    js.EndObject();
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Named Primitive List (global)")
  {
    StreamComparer sc("Data\n\
{\n\
	bool $values{}\n\
}\n");

    wdOpenDdlWriter js;
    js.SetOutputStream(&sc);

    js.BeginObject("Data");
    {
      js.BeginPrimitiveList(wdOpenDdlPrimitiveType::Bool, "values", true);
      {
      }
      js.EndPrimitiveList();
    }
    js.EndObject();
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Primitive List (bool)")
  {
    StreamComparer sc("Data\n\
{\n\
	bool $values{true,false}\n\
}\n");

    wdOpenDdlWriter js;
    js.SetOutputStream(&sc);

    js.BeginObject("Data");
    {
      js.BeginPrimitiveList(wdOpenDdlPrimitiveType::Bool, "values", true);
      {
        bool val[] = {true, false, true};
        js.WriteBool(val, 2);
      }
      js.EndPrimitiveList();
    }
    js.EndObject();
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Primitive List (bool)")
  {
    StreamComparer sc("Data\n\
{\n\
	bool %values{true,true,false}\n\
}\n");

    wdOpenDdlWriter js;
    js.SetOutputStream(&sc);

    js.BeginObject("Data");
    {
      js.BeginPrimitiveList(wdOpenDdlPrimitiveType::Bool, "values", false);
      {
        bool val[] = {true, false, true};
        js.WriteBool(val, 1);
        js.WriteBool(val, 2);
      }
      js.EndPrimitiveList();
    }
    js.EndObject();
  }

  //////////////////////////////////////////////////////////////////////////

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Primitive List (int8)")
  {
    StreamComparer sc("Data\n\
{\n\
	int8 $values{0,127,-128}\n\
}\n");

    wdOpenDdlWriter js;
    js.SetOutputStream(&sc);

    js.BeginObject("Data");
    {
      js.BeginPrimitiveList(wdOpenDdlPrimitiveType::Int8, "values", true);
      {
        wdInt8 val[] = {0, 127, -128};
        js.WriteInt8(val, 3);
      }
      js.EndPrimitiveList();
    }
    js.EndObject();
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Primitive List (int16)")
  {
    StreamComparer sc("Data\n\
{\n\
	int16 $values{1,32767,-32768}\n\
}\n");

    wdOpenDdlWriter js;
    js.SetOutputStream(&sc);

    js.BeginObject("Data");
    {
      js.BeginPrimitiveList(wdOpenDdlPrimitiveType::Int16, "values", true);
      {
        wdInt16 val[] = {1, 32767, -32768};
        js.WriteInt16(&val[0], 1);
        js.WriteInt16(&val[1], 1);
        js.WriteInt16(&val[2], 1);
      }
      js.EndPrimitiveList();
    }
    js.EndObject();
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Primitive List (int32)")
  {
    StreamComparer sc("Data\n\
{\n\
	int32{-2147483647,2147483647}\n\
}\n");

    wdOpenDdlWriter js;
    js.SetOutputStream(&sc);

    js.BeginObject("Data");
    {
      js.BeginPrimitiveList(wdOpenDdlPrimitiveType::Int32);
      {
        wdInt32 val[] = {-2147483647, 2147483647};
        js.WriteInt32(&val[0], 2);
      }
      js.EndPrimitiveList();
    }
    js.EndObject();
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Primitive List (int64)")
  {
    StreamComparer sc("Data\n\
{\n\
	int64{-9223372036854775807,9223372036854775807}\n\
}\n");

    wdOpenDdlWriter js;
    js.SetOutputStream(&sc);

    js.BeginObject("Data");
    {
      js.BeginPrimitiveList(wdOpenDdlPrimitiveType::Int64);
      {
        wdInt64 val[] = {-9223372036854775807LL, 9223372036854775807LL};
        js.WriteInt64(&val[0], 1);
        js.WriteInt64(&val[1], 1);
      }
      js.EndPrimitiveList();
    }
    js.EndObject();
  }

  //////////////////////////////////////////////////////////////////////////

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Primitive List (uint8)")
  {
    StreamComparer sc("Data\n\
{\n\
	unsigned_int8{0,255}\n\
}\n");

    wdOpenDdlWriter js;
    js.SetOutputStream(&sc);
    js.SetPrimitiveTypeStringMode(wdOpenDdlWriter::TypeStringMode::Compliant);

    js.BeginObject("Data");
    {
      js.BeginPrimitiveList(wdOpenDdlPrimitiveType::UInt8);
      {
        wdUInt8 val[] = {0, 255, 27};
        js.WriteUInt8(val, 2);
      }
      js.EndPrimitiveList();
    }
    js.EndObject();
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Primitive List (uint16)")
  {
    StreamComparer sc("Data\n\
{\n\
	uint16 %values{0,32767,65535}\n\
}\n");

    wdOpenDdlWriter js;
    js.SetOutputStream(&sc);
    js.SetPrimitiveTypeStringMode(wdOpenDdlWriter::TypeStringMode::ShortenedUnsignedInt);

    js.BeginObject("Data");
    {
      js.BeginPrimitiveList(wdOpenDdlPrimitiveType::UInt16, "values", false);
      {
        wdUInt16 val[] = {0, 32767, 65535};
        js.WriteUInt16(&val[0], 1);
        js.WriteUInt16(&val[1], 2);
      }
      js.EndPrimitiveList();
    }
    js.EndObject();
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Primitive List (uint32)")
  {
    StreamComparer sc("Data\n\
{\n\
	u3{4294967295}\n\
}\n");

    wdOpenDdlWriter js;
    js.SetOutputStream(&sc);
    js.SetPrimitiveTypeStringMode(wdOpenDdlWriter::TypeStringMode::Shortest);

    js.BeginObject("Data");
    {
      js.BeginPrimitiveList(wdOpenDdlPrimitiveType::UInt32);
      {
        wdUInt32 val[] = {4294967295, 0};
        js.WriteUInt32(&val[0], 1);
      }
      js.EndPrimitiveList();
    }
    js.EndObject();
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Primitive List (uint64)")
  {
    StreamComparer sc("Data\n\
{\n\
	unsigned_int64{18446744073709551615,18446744073709551615}\n\
}\n");

    wdOpenDdlWriter js;
    js.SetOutputStream(&sc);
    js.SetPrimitiveTypeStringMode(wdOpenDdlWriter::TypeStringMode::Compliant);

    js.BeginObject("Data");
    {
      js.BeginPrimitiveList(wdOpenDdlPrimitiveType::UInt64);
      {
        wdUInt64 val[] = {18446744073709551615ULL};
        js.WriteUInt64(&val[0], 1);
        js.WriteUInt64(&val[0], 1);
      }
      js.EndPrimitiveList();
    }
    js.EndObject();
  }

  //////////////////////////////////////////////////////////////////////////

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Primitive List (float)")
  {
    StreamComparer sc("Data\n\
{\n\
	float{0}\n\
	float{0.1,0.00001}\n\
	float{0.00001,230000,-42.23,0,0.1}\n\
}\n");

    wdOpenDdlWriter js;
    js.SetOutputStream(&sc);
    js.SetFloatPrecisionMode(wdOpenDdlWriter::FloatPrecisionMode::Readable);

    js.BeginObject("Data");
    {
      float val[] = {0, 0.1f, 1e-5f, 23e4f, -42.23f};

      js.BeginPrimitiveList(wdOpenDdlPrimitiveType::Float);
      {
        js.WriteFloat(&val[0], 1);
      }
      js.EndPrimitiveList();

      js.BeginPrimitiveList(wdOpenDdlPrimitiveType::Float);
      {
        js.WriteFloat(&val[1], 1);
        js.WriteFloat(&val[2], 1);
      }
      js.EndPrimitiveList();

      js.BeginPrimitiveList(wdOpenDdlPrimitiveType::Float);
      {
        js.WriteFloat(&val[2], 3);
        js.WriteFloat(&val[0], 2);
      }
      js.EndPrimitiveList();
    }
    js.EndObject();
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Primitive List (double)")
  {
    StreamComparer sc("Data\n\
{\n\
	double{0}\n\
	double{0.1,0.00001}\n\
	double{0.00001,230000,-42.23,0,0.1}\n\
}\n");

    wdOpenDdlWriter js;
    js.SetOutputStream(&sc);
    js.SetFloatPrecisionMode(wdOpenDdlWriter::FloatPrecisionMode::Readable);

    js.BeginObject("Data");
    {
      double val[] = {0, 0.1, 1e-5, 23e4, -42.23};

      js.BeginPrimitiveList(wdOpenDdlPrimitiveType::Double);
      {
        js.WriteDouble(&val[0], 1);
      }
      js.EndPrimitiveList();

      js.BeginPrimitiveList(wdOpenDdlPrimitiveType::Double);
      {
        js.WriteDouble(&val[1], 1);
        js.WriteDouble(&val[2], 1);
      }
      js.EndPrimitiveList();

      js.BeginPrimitiveList(wdOpenDdlPrimitiveType::Double);
      {
        js.WriteDouble(&val[2], 3);
        js.WriteDouble(&val[0], 2);
      }
      js.EndPrimitiveList();
    }
    js.EndObject();
  }

  //////////////////////////////////////////////////////////////////////////

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Primitive List (string)")
  {
    StreamComparer sc("Data\n\
{\n\
	string{\"bla\"}\n\
	string{\"bla2\",\"blub\"}\n\
	string{\"A\\nwo\\rld\\\"\"}\n\
}\n");

    wdOpenDdlWriter js;
    js.SetOutputStream(&sc);

    js.BeginObject("Data");
    {
      js.BeginPrimitiveList(wdOpenDdlPrimitiveType::String);
      {
        js.WriteString("bla");
      }
      js.EndPrimitiveList();
      js.BeginPrimitiveList(wdOpenDdlPrimitiveType::String);
      {
        js.WriteString("bla2");
        js.WriteString("blub");
      }
      js.EndPrimitiveList();
      js.BeginPrimitiveList(wdOpenDdlPrimitiveType::String);
      {
        js.WriteString("A\nwo\rld\"");
      }
      js.EndPrimitiveList();
    }
    js.EndObject();
  }

  //////////////////////////////////////////////////////////////////////////

  WD_TEST_BLOCK(wdTestBlock::Enabled, "LessIndentation")
  {
    StreamComparer sc("bool $balue{true}\n\
obj1\n\
{\n\
	string{\"bla\"}\n\
	obj11\n\
	{\n\
		float %falue{23.42}\n\
	}\n\
}\n");

    wdOpenDdlWriter js;
    js.SetOutputStream(&sc);
    js.SetFloatPrecisionMode(wdOpenDdlWriter::FloatPrecisionMode::Readable);

    js.BeginPrimitiveList(wdOpenDdlPrimitiveType::Bool, "balue", true);
    {
      bool b = true;
      js.WriteBool(&b, 1);
    }
    js.EndPrimitiveList();

    js.BeginObject("obj1");
    {
      js.BeginPrimitiveList(wdOpenDdlPrimitiveType::String);
      {
        js.WriteString("bla");
      }
      js.EndPrimitiveList();

      js.BeginObject("obj11");
      {
        js.BeginPrimitiveList(wdOpenDdlPrimitiveType::Float, "falue");
        {
          float f = 23.42f;
          js.WriteFloat(&f);
        }
        js.EndPrimitiveList();
      }
      js.EndObject();
    }
    js.EndObject();
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "None")
  {
    StreamComparer sc("bool$balue{1}obj1{string{\"bla\"}obj11{float%falue{23.42}}}");

    wdOpenDdlWriter js;
    js.SetOutputStream(&sc);
    js.SetFloatPrecisionMode(wdOpenDdlWriter::FloatPrecisionMode::Readable);
    js.SetCompactMode(true);

    js.BeginPrimitiveList(wdOpenDdlPrimitiveType::Bool, "balue", true);
    {
      bool b = true;
      js.WriteBool(&b, 1);
    }
    js.EndPrimitiveList();

    js.BeginObject("obj1");
    {
      js.BeginPrimitiveList(wdOpenDdlPrimitiveType::String);
      {
        js.WriteString("bla");
      }
      js.EndPrimitiveList();

      js.BeginObject("obj11");
      {
        js.BeginPrimitiveList(wdOpenDdlPrimitiveType::Float, "falue");
        {
          float f = 23.42f;
          js.WriteFloat(&f);
        }
        js.EndPrimitiveList();
      }
      js.EndObject();
    }
    js.EndObject();
  }
}
