#include <FoundationTest/FoundationTestPCH.h>

// NOTE: always save as Unicode UTF-8 with signature

#include <Foundation/Containers/Deque.h>
#include <Foundation/IO/JSONReader.h>

namespace JSONReaderTestDetail
{

  class StringStream : public wdStreamReader
  {
  public:
    StringStream(const void* pData)
    {
      m_pData = pData;
      m_uiLength = wdStringUtils::GetStringElementCount((const char*)pData);
    }

    virtual wdUInt64 ReadBytes(void* pReadBuffer, wdUInt64 uiBytesToRead)
    {
      uiBytesToRead = wdMath::Min(uiBytesToRead, m_uiLength);
      m_uiLength -= uiBytesToRead;

      if (uiBytesToRead > 0)
      {
        wdMemoryUtils::Copy((wdUInt8*)pReadBuffer, (wdUInt8*)m_pData, (size_t)uiBytesToRead);
        m_pData = wdMemoryUtils::AddByteOffset(m_pData, (ptrdiff_t)uiBytesToRead);
      }

      return uiBytesToRead;
    }

  private:
    const void* m_pData;
    wdUInt64 m_uiLength;
  };

  void TraverseTree(const wdVariant& var, wdDeque<wdString>& ref_compare)
  {
    if (ref_compare.IsEmpty())
      return;

    switch (var.GetType())
    {
      case wdVariant::Type::VariantDictionary:
      {
        // wdLog::Printf("Expect: %s - Is: %s\n", "<object>", Compare.PeekFront().GetData());
        WD_TEST_STRING(ref_compare.PeekFront().GetData(), "<object>");
        ref_compare.PopFront();

        const wdVariantDictionary& vd = var.Get<wdVariantDictionary>();

        for (auto it = vd.GetIterator(); it.IsValid(); ++it)
        {
          if (ref_compare.IsEmpty())
            return;

          // wdLog::Printf("Expect: %s - Is: %s\n", it.Key().GetData(), Compare.PeekFront().GetData());
          WD_TEST_STRING(ref_compare.PeekFront().GetData(), it.Key().GetData());
          ref_compare.PopFront();

          TraverseTree(it.Value(), ref_compare);
        }

        if (ref_compare.IsEmpty())
          return;

        WD_TEST_STRING(ref_compare.PeekFront().GetData(), "</object>");
        // wdLog::Printf("Expect: %s - Is: %s\n", "</object>", Compare.PeekFront().GetData());
        ref_compare.PopFront();
      }
      break;

      case wdVariant::Type::VariantArray:
      {
        // wdLog::Printf("Expect: %s - Is: %s\n", "<array>", Compare.PeekFront().GetData());
        WD_TEST_STRING(ref_compare.PeekFront().GetData(), "<array>");
        ref_compare.PopFront();

        const wdVariantArray& va = var.Get<wdVariantArray>();

        for (wdUInt32 i = 0; i < va.GetCount(); ++i)
        {
          TraverseTree(va[i], ref_compare);
        }

        if (ref_compare.IsEmpty())
          return;

        // wdLog::Printf("Expect: %s - Is: %s\n", "</array>", Compare.PeekFront().GetData());
        WD_TEST_STRING(ref_compare.PeekFront().GetData(), "</array>");
        ref_compare.PopFront();
      }
      break;

      case wdVariant::Type::Invalid:
        // wdLog::Printf("Expect: %s - Is: %s\n", "null", Compare.PeekFront().GetData());
        WD_TEST_STRING(ref_compare.PeekFront().GetData(), "null");
        ref_compare.PopFront();
        break;

      case wdVariant::Type::Bool:
        // wdLog::Printf("Expect: %s - Is: %s\n", var.Get<bool>() ? "bool true" : "bool false", Compare.PeekFront().GetData());
        WD_TEST_STRING(ref_compare.PeekFront().GetData(), var.Get<bool>() ? "bool true" : "bool false");
        ref_compare.PopFront();
        break;

      case wdVariant::Type::Int8:
      {
        wdStringBuilder sTemp;
        sTemp.Format("int8 {0}", var.Get<wdInt8>());
        // wdLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        WD_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case wdVariant::Type::UInt8:
      {
        wdStringBuilder sTemp;
        sTemp.Format("uint8 {0}", var.Get<wdUInt8>());
        // wdLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        WD_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case wdVariant::Type::Int16:
      {
        wdStringBuilder sTemp;
        sTemp.Format("int16 {0}", var.Get<wdInt16>());
        // wdLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        WD_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case wdVariant::Type::UInt16:
      {
        wdStringBuilder sTemp;
        sTemp.Format("uint16 {0}", var.Get<wdUInt16>());
        // wdLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        WD_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case wdVariant::Type::Int32:
      {
        wdStringBuilder sTemp;
        sTemp.Format("int32 {0}", var.Get<wdInt32>());
        // wdLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        WD_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case wdVariant::Type::UInt32:
      {
        wdStringBuilder sTemp;
        sTemp.Format("uint32 {0}", var.Get<wdUInt32>());
        // wdLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        WD_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case wdVariant::Type::Int64:
      {
        wdStringBuilder sTemp;
        sTemp.Format("int64 {0}", var.Get<wdInt64>());
        // wdLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        WD_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case wdVariant::Type::UInt64:
      {
        wdStringBuilder sTemp;
        sTemp.Format("uint64 {0}", var.Get<wdUInt64>());
        // wdLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        WD_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case wdVariant::Type::Float:
      {
        wdStringBuilder sTemp;
        sTemp.Format("float {0}", wdArgF(var.Get<float>(), 4));
        // wdLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        WD_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case wdVariant::Type::Double:
      {
        wdStringBuilder sTemp;
        sTemp.Format("double {0}", wdArgF(var.Get<double>(), 4));
        // wdLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        WD_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case wdVariant::Type::Time:
      {
        wdStringBuilder sTemp;
        sTemp.Format("time {0}", wdArgF(var.Get<wdTime>().GetSeconds(), 4));
        // wdLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        WD_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case wdVariant::Type::Angle:
      {
        wdStringBuilder sTemp;
        sTemp.Format("angle {0}", wdArgF(var.Get<wdAngle>().GetDegree(), 4));
        // wdLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        WD_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case wdVariant::Type::String:
        // wdLog::Printf("Expect: %s - Is: %s\n", var.Get<wdString>().GetData(), Compare.PeekFront().GetData());
        WD_TEST_STRING(ref_compare.PeekFront().GetData(), var.Get<wdString>().GetData());
        ref_compare.PopFront();
        break;

      case wdVariant::Type::Vector2:
      {
        wdStringBuilder sTemp;
        sTemp.Format("vec2 ({0}, {1})", wdArgF(var.Get<wdVec2>().x, 4), wdArgF(var.Get<wdVec2>().y, 4));
        // wdLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        WD_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case wdVariant::Type::Vector3:
      {
        wdStringBuilder sTemp;
        sTemp.Format("vec3 ({0}, {1}, {2})", wdArgF(var.Get<wdVec3>().x, 4), wdArgF(var.Get<wdVec3>().y, 4), wdArgF(var.Get<wdVec3>().z, 4));
        // wdLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        WD_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case wdVariant::Type::Vector4:
      {
        wdStringBuilder sTemp;
        sTemp.Format("vec4 ({0}, {1}, {2}, {3})", wdArgF(var.Get<wdVec4>().x, 4), wdArgF(var.Get<wdVec4>().y, 4), wdArgF(var.Get<wdVec4>().z, 4), wdArgF(var.Get<wdVec4>().w, 4));
        // wdLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        WD_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case wdVariant::Type::Vector2I:
      {
        wdStringBuilder sTemp;
        sTemp.Format("vec2i ({0}, {1})", var.Get<wdVec2I32>().x, var.Get<wdVec2I32>().y);
        // wdLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        WD_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case wdVariant::Type::Vector3I:
      {
        wdStringBuilder sTemp;
        sTemp.Format("vec3i ({0}, {1}, {2})", var.Get<wdVec3I32>().x, var.Get<wdVec3I32>().y, var.Get<wdVec3I32>().z);
        // wdLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        WD_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case wdVariant::Type::Vector4I:
      {
        wdStringBuilder sTemp;
        sTemp.Format("vec4i ({0}, {1}, {2}, {3})", var.Get<wdVec4I32>().x, var.Get<wdVec4I32>().y, var.Get<wdVec4I32>().z, var.Get<wdVec4I32>().w);
        // wdLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        WD_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case wdVariant::Type::Color:
      {
        wdStringBuilder sTemp;
        sTemp.Format("color ({0}, {1}, {2}, {3})", wdArgF(var.Get<wdColor>().r, 4), wdArgF(var.Get<wdColor>().g, 4), wdArgF(var.Get<wdColor>().b, 4), wdArgF(var.Get<wdColor>().a, 4));
        // wdLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        WD_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case wdVariant::Type::ColorGamma:
      {
        wdStringBuilder sTemp;
        const wdColorGammaUB c = var.ConvertTo<wdColorGammaUB>();

        sTemp.Format("gamma ({0}, {1}, {2}, {3})", c.r, c.g, c.b, c.a);
        // wdLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        WD_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case wdVariant::Type::Quaternion:
      {
        wdStringBuilder sTemp;
        sTemp.Format("quat ({0}, {1}, {2}, {3})", wdArgF(var.Get<wdQuat>().v.x, 4), wdArgF(var.Get<wdQuat>().v.y, 4), wdArgF(var.Get<wdQuat>().v.z, 4), wdArgF(var.Get<wdQuat>().w, 4));
        // wdLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        WD_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case wdVariant::Type::Matrix3:
      {
        wdMat3 m = var.Get<wdMat3>();

        wdStringBuilder sTemp;
        sTemp.Format("mat3 ({0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}, {8})", wdArgF(m.m_fElementsCM[0], 4), wdArgF(m.m_fElementsCM[1], 4), wdArgF(m.m_fElementsCM[2], 4), wdArgF(m.m_fElementsCM[3], 4), wdArgF(m.m_fElementsCM[4], 4), wdArgF(m.m_fElementsCM[5], 4), wdArgF(m.m_fElementsCM[6], 4), wdArgF(m.m_fElementsCM[7], 4), wdArgF(m.m_fElementsCM[8], 4));
        // wdLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        WD_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case wdVariant::Type::Matrix4:
      {
        wdMat4 m = var.Get<wdMat4>();

        wdStringBuilder sTemp;
        sTemp.Printf("mat4 (%.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f)", m.m_fElementsCM[0], m.m_fElementsCM[1], m.m_fElementsCM[2], m.m_fElementsCM[3], m.m_fElementsCM[4], m.m_fElementsCM[5], m.m_fElementsCM[6], m.m_fElementsCM[7], m.m_fElementsCM[8], m.m_fElementsCM[9], m.m_fElementsCM[10], m.m_fElementsCM[11], m.m_fElementsCM[12], m.m_fElementsCM[13], m.m_fElementsCM[14], m.m_fElementsCM[15]);
        // wdLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        WD_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case wdVariant::Type::Uuid:
      {
        wdUuid uuid = var.Get<wdUuid>();
        wdStringBuilder sTemp;
        wdConversionUtils::ToString(uuid, sTemp);
        sTemp.Prepend("uuid ");
        // wdLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        WD_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      default:
        WD_ASSERT_NOT_IMPLEMENTED;
        break;
    }
  }
} // namespace JSONReaderTestDetail

WD_CREATE_SIMPLE_TEST(IO, JSONReader)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Test")
  {
    wdStringUtf8 sTD(L"{\n\
\"myarray2\":[\"\",2.2],\n\
\"myarray\" : [1, 2.2, 3.3, false, \"ende\" ],\n\
\"String\"/**/ : \"testvälue\",\n\
\"double\"/***/ : 43.56,//comment\n\
\"float\" :/**//*a*/ 64/*comment*/.720001,\n\
\"bool\" : tr/*asdf*/ue,\n\
\"int\" : 23,\n\
\"MyNüll\" : nu/*asdf*/ll,\n\
\"object\" :\n\
/* totally \n weird \t stuff \n\n\n going on here // thats a line comment \n */ \
// more line comments \n\n\n\n\
{\n\
  \"variable in object\" : \"bla\\\\\\\"\\/\",\n\
    \"Subobject\" :\n\
  {\n\
    \"variable in subobject\" : \"blub\\r\\f\\n\\b\\t\",\n\
      \"array in sub\" : [\n\
    {\n\
      \"obj var\" : 234\n\
            /*stuff ] */ \
    },\n\
    {\n\
      \"obj var 2\" : -235\n//breakingcomment\n\
    }, true, 4, false ]\n\
  }\n\
},\n\
\"test\" : \"text\"\n\
}");
    const char* szTestData = sTD.GetData();

    // NOTE: The way this test is implemented, it might break, if the HashMap uses another insertion algorithm.
    // wdVariantDictionary is an wdHashmap and this test currently relies on one exact order in of the result.
    // If this should ever change (or be arbitrary at runtime), the test needs to be implemented in a more robust way.

    JSONReaderTestDetail::StringStream stream(szTestData);

    wdJSONReader reader;
    WD_TEST_BOOL(reader.Parse(stream).Succeeded());

    wdDeque<wdString> sCompare;
    sCompare.PushBack("<object>");
    sCompare.PushBack("int");
    sCompare.PushBack("double 23.0000");
    sCompare.PushBack("String");
    sCompare.PushBack(wdStringUtf8(L"testvälue").GetData()); // unicode literal

    sCompare.PushBack("double");
    sCompare.PushBack("double 43.5600");

    sCompare.PushBack("myarray");
    sCompare.PushBack("<array>");
    sCompare.PushBack("double 1.0000");
    sCompare.PushBack("double 2.2000");
    sCompare.PushBack("double 3.3000");
    sCompare.PushBack("bool false");
    sCompare.PushBack("ende");
    sCompare.PushBack("</array>");

    sCompare.PushBack("object");
    sCompare.PushBack("<object>");

    sCompare.PushBack("Subobject");
    sCompare.PushBack("<object>");

    sCompare.PushBack("array in sub");
    sCompare.PushBack("<array>");

    sCompare.PushBack("<object>");
    sCompare.PushBack("obj var");
    sCompare.PushBack("double 234.0000");
    sCompare.PushBack("</object>");

    sCompare.PushBack("<object>");
    sCompare.PushBack("obj var 2");
    sCompare.PushBack("double -235.0000");
    sCompare.PushBack("</object>");

    sCompare.PushBack("bool true");
    sCompare.PushBack("double 4.0000");
    sCompare.PushBack("bool false");

    sCompare.PushBack("</array>");


    sCompare.PushBack("variable in subobject");
    sCompare.PushBack("blub\r\f\n\b\t"); // escaped special characters

    sCompare.PushBack("</object>");

    sCompare.PushBack("variable in object");
    sCompare.PushBack("bla\\\"/"); // escaped backslash, quotation mark, slash

    sCompare.PushBack("</object>");

    sCompare.PushBack("float");
    sCompare.PushBack("double 64.7200");

    sCompare.PushBack("myarray2");
    sCompare.PushBack("<array>");
    sCompare.PushBack("");
    sCompare.PushBack("double 2.2000");
    sCompare.PushBack("</array>");

    sCompare.PushBack(wdStringUtf8(L"MyNüll").GetData()); // unicode literal
    sCompare.PushBack("null");

    sCompare.PushBack("test");
    sCompare.PushBack("text");

    sCompare.PushBack("bool");
    sCompare.PushBack("bool true");

    sCompare.PushBack("</object>");

    JSONReaderTestDetail::TraverseTree(reader.GetTopLevelObject(), sCompare);

    WD_TEST_BOOL(sCompare.IsEmpty());
  }
}
