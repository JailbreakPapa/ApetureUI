#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Containers/Deque.h>
#include <Foundation/IO/MemoryStream.h>
#include <Foundation/IO/OpenDdlReader.h>
#include <Foundation/IO/OpenDdlUtils.h>
#include <Foundation/IO/OpenDdlWriter.h>
#include <Foundation/Strings/StringUtils.h>
#include <FoundationTest/IO/JSONTestHelpers.h>

static wdVariant CreateVariant(wdVariant::Type::Enum t, const void* pData);

WD_CREATE_SIMPLE_TEST(IO, DdlUtils)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdOpenDdlUtils::ConvertToColor")
  {
    const char* szTestData = "\
Color $c1 { float { 1, 0, 0.5 } }\
Color $c2 { float { 2, 1, 1.5, 0.1 } }\
Color $c3 { unsigned_int8 { 128, 2, 32 } }\
Color $c4 { unsigned_int8 { 128, 0, 32, 64 } }\
float $c5 { 1, 0, 0.5 }\
float $c6 { 2, 1, 1.5, 0.1 }\
unsigned_int8 $c7 { 128, 2, 32 }\
unsigned_int8 $c8 { 128, 0, 32, 64 }\
Color $c9 { float { 1, 0 } }\
Color $c10 { float { 1, 0, 3, 4, 5 } }\
Color $c11 { float { } }\
Color $c12 { }\
Color $c13 { double { 1, 1, 1, 2 } }\
";

    StringStream stream(szTestData);
    wdOpenDdlReader doc;
    WD_TEST_BOOL(doc.ParseDocument(stream).Succeeded());

    wdColor c1, c2, c3, c4, c5, c6, c7, c8, c0;

    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToColor(doc.FindElement("t0"), c0).Failed());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToColor(doc.FindElement("c1"), c1).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToColor(doc.FindElement("c2"), c2).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToColor(doc.FindElement("c3"), c3).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToColor(doc.FindElement("c4"), c4).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToColor(doc.FindElement("c5"), c5).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToColor(doc.FindElement("c6"), c6).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToColor(doc.FindElement("c7"), c7).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToColor(doc.FindElement("c8"), c8).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToColor(doc.FindElement("c9"), c0).Failed());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToColor(doc.FindElement("c10"), c0).Failed());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToColor(doc.FindElement("c11"), c0).Failed());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToColor(doc.FindElement("c12"), c0).Failed());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToColor(doc.FindElement("c13"), c0).Failed());

    WD_TEST_BOOL(c1 == wdColor(1, 0, 0.5f, 1.0f));
    WD_TEST_BOOL(c2 == wdColor(2, 1, 1.5f, 0.1f));
    WD_TEST_BOOL(c3 == wdColorGammaUB(128, 2, 32));
    WD_TEST_BOOL(c4 == wdColorGammaUB(128, 0, 32, 64));
    WD_TEST_BOOL(c5 == wdColor(1, 0, 0.5f, 1.0f));
    WD_TEST_BOOL(c6 == wdColor(2, 1, 1.5f, 0.1f));
    WD_TEST_BOOL(c7 == wdColorGammaUB(128, 2, 32));
    WD_TEST_BOOL(c8 == wdColorGammaUB(128, 0, 32, 64));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdOpenDdlUtils::ConvertToColorGamma")
  {
    const char* szTestData = "\
Color $c1 { float { 1, 0, 0.5 } }\
Color $c2 { float { 2, 1, 1.5, 0.1 } }\
Color $c3 { unsigned_int8 { 128, 2, 32 } }\
Color $c4 { unsigned_int8 { 128, 0, 32, 64 } }\
float $c5 { 1, 0, 0.5 }\
float $c6 { 2, 1, 1.5, 0.1 }\
unsigned_int8 $c7 { 128, 2, 32 }\
unsigned_int8 $c8 { 128, 0, 32, 64 }\
Color $c9 { float { 1, 0 } }\
Color $c10 { float { 1, 0, 3, 4, 5 } }\
Color $c11 { float { } }\
Color $c12 { }\
Color $c13 { double { 1, 1, 1, 2 } }\
";

    StringStream stream(szTestData);
    wdOpenDdlReader doc;
    WD_TEST_BOOL(doc.ParseDocument(stream).Succeeded());

    wdColorGammaUB c1, c2, c3, c4, c5, c6, c7, c8, c0;

    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToColorGamma(doc.FindElement("t0"), c0).Failed());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToColorGamma(doc.FindElement("c1"), c1).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToColorGamma(doc.FindElement("c2"), c2).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToColorGamma(doc.FindElement("c3"), c3).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToColorGamma(doc.FindElement("c4"), c4).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToColorGamma(doc.FindElement("c5"), c5).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToColorGamma(doc.FindElement("c6"), c6).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToColorGamma(doc.FindElement("c7"), c7).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToColorGamma(doc.FindElement("c8"), c8).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToColorGamma(doc.FindElement("c9"), c0).Failed());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToColorGamma(doc.FindElement("c10"), c0).Failed());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToColorGamma(doc.FindElement("c11"), c0).Failed());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToColorGamma(doc.FindElement("c12"), c0).Failed());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToColorGamma(doc.FindElement("c13"), c0).Failed());

    WD_TEST_BOOL(c1 == wdColorGammaUB(wdColor(1, 0, 0.5f, 1.0f)));
    WD_TEST_BOOL(c2 == wdColorGammaUB(wdColor(2, 1, 1.5f, 0.1f)));
    WD_TEST_BOOL(c3 == wdColorGammaUB(128, 2, 32));
    WD_TEST_BOOL(c4 == wdColorGammaUB(128, 0, 32, 64));
    WD_TEST_BOOL(c5 == wdColorGammaUB(wdColor(1, 0, 0.5f, 1.0f)));
    WD_TEST_BOOL(c6 == wdColorGammaUB(wdColor(2, 1, 1.5f, 0.1f)));
    WD_TEST_BOOL(c7 == wdColorGammaUB(128, 2, 32));
    WD_TEST_BOOL(c8 == wdColorGammaUB(128, 0, 32, 64));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdOpenDdlUtils::ConvertToTime")
  {
    const char* szTestData = "\
Time $t1 { float { 0.1 } }\
Time $t2 { double { 0.2 } }\
float $t3 { 0.3 }\
double $t4 { 0.4 }\
Time $t5 { double { 0.2, 2 } }\
Time $t6 { int8 { 0, 2 } }\
";

    StringStream stream(szTestData);
    wdOpenDdlReader doc;
    WD_TEST_BOOL(doc.ParseDocument(stream).Succeeded());

    wdTime t1, t2, t3, t4, t0;

    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToTime(doc.FindElement("t0"), t0).Failed());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToTime(doc.FindElement("t1"), t1).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToTime(doc.FindElement("t2"), t2).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToTime(doc.FindElement("t3"), t3).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToTime(doc.FindElement("t4"), t4).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToTime(doc.FindElement("t5"), t0).Failed());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToTime(doc.FindElement("t6"), t0).Failed());

    WD_TEST_FLOAT(t1.GetSeconds(), 0.1, 0.0001f);
    WD_TEST_FLOAT(t2.GetSeconds(), 0.2, 0.0001f);
    WD_TEST_FLOAT(t3.GetSeconds(), 0.3, 0.0001f);
    WD_TEST_FLOAT(t4.GetSeconds(), 0.4, 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdOpenDdlUtils::ConvertToVec2")
  {
    const char* szTestData = "\
Vector $v1 { float { 0.1, 2 } }\
float $v2 { 0.3, 3 }\
Vector $v3 { float { 0.1 } }\
Vector $v4 { float { 0.1, 2.2, 3.33 } }\
";

    StringStream stream(szTestData);
    wdOpenDdlReader doc;
    WD_TEST_BOOL(doc.ParseDocument(stream).Succeeded());

    wdVec2 v0, v1, v2;

    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToVec2(doc.FindElement("v0"), v0).Failed());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToVec2(doc.FindElement("v1"), v1).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToVec2(doc.FindElement("v2"), v2).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToVec2(doc.FindElement("v3"), v0).Failed());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToVec2(doc.FindElement("v4"), v0).Failed());

    WD_TEST_VEC2(v1, wdVec2(0.1f, 2.0f), 0.0001f);
    WD_TEST_VEC2(v2, wdVec2(0.3f, 3.0f), 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdOpenDdlUtils::ConvertToVec3")
  {
    const char* szTestData = "\
Vector $v1 { float { 0.1, 2, 3.2 } }\
float $v2 { 0.3, 3,0}\
Vector $v3 { float { 0.1,2 } }\
Vector $v4 { float { 0.1, 2.2, 3.33,44 } }\
";

    StringStream stream(szTestData);
    wdOpenDdlReader doc;
    WD_TEST_BOOL(doc.ParseDocument(stream).Succeeded());

    wdVec3 v0, v1, v2;

    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToVec3(doc.FindElement("v0"), v0).Failed());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToVec3(doc.FindElement("v1"), v1).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToVec3(doc.FindElement("v2"), v2).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToVec3(doc.FindElement("v3"), v0).Failed());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToVec3(doc.FindElement("v4"), v0).Failed());

    WD_TEST_VEC3(v1, wdVec3(0.1f, 2.0f, 3.2f), 0.0001f);
    WD_TEST_VEC3(v2, wdVec3(0.3f, 3.0f, 0.0f), 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdOpenDdlUtils::ConvertToVec4")
  {
    const char* szTestData = "\
Vector $v1 { float { 0.1, 2, 3.2, 44.5 } }\
float $v2 { 0.3, 3,0, 12.}\
Vector $v3 { float { 0.1,2 } }\
Vector $v4 { float { 0.1, 2.2, 3.33, 44, 67 } }\
";

    StringStream stream(szTestData);
    wdOpenDdlReader doc;
    WD_TEST_BOOL(doc.ParseDocument(stream).Succeeded());

    wdVec4 v0, v1, v2;

    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToVec4(doc.FindElement("v0"), v0).Failed());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToVec4(doc.FindElement("v1"), v1).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToVec4(doc.FindElement("v2"), v2).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToVec4(doc.FindElement("v3"), v0).Failed());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToVec4(doc.FindElement("v4"), v0).Failed());

    WD_TEST_VEC4(v1, wdVec4(0.1f, 2.0f, 3.2f, 44.5f), 0.0001f);
    WD_TEST_VEC4(v2, wdVec4(0.3f, 3.0f, 0.0f, 12.0f), 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdOpenDdlUtils::ConvertToMat3")
  {
    const char* szTestData = "\
Group $v1 { float { 1, 2, 3, 4, 5, 6, 7, 8, 9 } }\
";

    StringStream stream(szTestData);
    wdOpenDdlReader doc;
    WD_TEST_BOOL(doc.ParseDocument(stream).Succeeded());

    wdMat3 v0, v1;

    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToMat3(doc.FindElement("v0"), v0).Failed());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToMat3(doc.FindElement("v1"), v1).Succeeded());

    WD_TEST_BOOL(v1.IsEqual(wdMat3(1, 4, 7, 2, 5, 8, 3, 6, 9), 0.0001f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdOpenDdlUtils::ConvertToMat4")
  {
    const char* szTestData = "\
Group $v1 { float { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 } }\
";

    StringStream stream(szTestData);
    wdOpenDdlReader doc;
    WD_TEST_BOOL(doc.ParseDocument(stream).Succeeded());

    wdMat4 v0, v1;

    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToMat4(doc.FindElement("v0"), v0).Failed());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToMat4(doc.FindElement("v1"), v1).Succeeded());

    WD_TEST_BOOL(v1.IsEqual(wdMat4(1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15, 4, 8, 12, 16), 0.0001f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdOpenDdlUtils::ConvertToTransform")
  {
    const char* szTestData = "\
Group $v1 { float { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 } }\
";

    StringStream stream(szTestData);
    wdOpenDdlReader doc;
    WD_TEST_BOOL(doc.ParseDocument(stream).Succeeded());

    wdTransform v0, v1;

    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToTransform(doc.FindElement("v0"), v0).Failed());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToTransform(doc.FindElement("v1"), v1).Succeeded());

    WD_TEST_VEC3(v1.m_vPosition, wdVec3(1, 2, 3), 0.0001f);
    WD_TEST_BOOL(v1.m_qRotation == wdQuat(4, 5, 6, 7));
    WD_TEST_VEC3(v1.m_vScale, wdVec3(8, 9, 10), 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdOpenDdlUtils::ConvertToQuat")
  {
    const char* szTestData = "\
Vector $v1 { float { 0.1, 2, 3.2, 44.5 } }\
float $v2 { 0.3, 3,0, 12.}\
Vector $v3 { float { 0.1,2 } }\
Vector $v4 { float { 0.1, 2.2, 3.33, 44, 67 } }\
";

    StringStream stream(szTestData);
    wdOpenDdlReader doc;
    WD_TEST_BOOL(doc.ParseDocument(stream).Succeeded());

    wdQuat v0, v1, v2;

    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToQuat(doc.FindElement("v0"), v0).Failed());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToQuat(doc.FindElement("v1"), v1).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToQuat(doc.FindElement("v2"), v2).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToQuat(doc.FindElement("v3"), v0).Failed());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToQuat(doc.FindElement("v4"), v0).Failed());

    WD_TEST_BOOL(v1 == wdQuat(0.1f, 2.0f, 3.2f, 44.5f));
    WD_TEST_BOOL(v2 == wdQuat(0.3f, 3.0f, 0.0f, 12.0f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdOpenDdlUtils::ConvertToUuid")
  {
    const char* szTestData = "\
Data $v1 { unsigned_int64 { 12345678910, 10987654321 } }\
";

    StringStream stream(szTestData);
    wdOpenDdlReader doc;
    WD_TEST_BOOL(doc.ParseDocument(stream).Succeeded());

    wdUuid v0, v1;

    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToUuid(doc.FindElement("v0"), v0).Failed());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToUuid(doc.FindElement("v1"), v1).Succeeded());

    WD_TEST_BOOL(v1 == wdUuid(12345678910, 10987654321));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdOpenDdlUtils::ConvertToAngle")
  {
    const char* szTestData = "\
Data $v1 { float { 45.23 } }\
";

    StringStream stream(szTestData);
    wdOpenDdlReader doc;
    WD_TEST_BOOL(doc.ParseDocument(stream).Succeeded());

    wdAngle v0, v1;

    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToAngle(doc.FindElement("v0"), v0).Failed());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToAngle(doc.FindElement("v1"), v1).Succeeded());

    WD_TEST_FLOAT(v1.GetRadian(), 45.23f, 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdOpenDdlUtils::ConvertToVariant")
  {
    const char* szTestData = "\
Color $v1 { float { 1, 0, 0.5 } }\
ColorGamma $v2 { unsigned_int8 { 128, 0, 32, 64 } }\
Time $v3 { float { 0.1 } }\
Vec2 $v4 { float { 0.1, 2 } }\
Vec3 $v5 { float { 0.1, 2, 3.2 } }\
Vec4 $v6 { float { 0.1, 2, 3.2, 44.5 } }\
Mat3 $v7 { float { 1, 2, 3, 4, 5, 6, 7, 8, 9 } }\
Mat4 $v8 { float { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 } }\
Transform $v9 { float { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 } }\
Quat $v10 { float { 0.1, 2, 3.2, 44.5 } }\
Uuid $v11 { unsigned_int64 { 12345678910, 10987654321 } }\
Angle $v12 { float { 45.23 } }\
";

    StringStream stream(szTestData);
    wdOpenDdlReader doc;
    WD_TEST_BOOL(doc.ParseDocument(stream).Succeeded());

    wdVariant v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12;

    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToVariant(doc.FindElement("v0"), v0).Failed());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToVariant(doc.FindElement("v1"), v1).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToVariant(doc.FindElement("v2"), v2).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToVariant(doc.FindElement("v3"), v3).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToVariant(doc.FindElement("v4"), v4).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToVariant(doc.FindElement("v5"), v5).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToVariant(doc.FindElement("v6"), v6).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToVariant(doc.FindElement("v7"), v7).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToVariant(doc.FindElement("v8"), v8).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToVariant(doc.FindElement("v9"), v9).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToVariant(doc.FindElement("v10"), v10).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToVariant(doc.FindElement("v11"), v11).Succeeded());
    WD_TEST_BOOL(wdOpenDdlUtils::ConvertToVariant(doc.FindElement("v12"), v12).Succeeded());

    WD_TEST_BOOL(v1.IsA<wdColor>());
    WD_TEST_BOOL(v2.IsA<wdColorGammaUB>());
    WD_TEST_BOOL(v3.IsA<wdTime>());
    WD_TEST_BOOL(v4.IsA<wdVec2>());
    WD_TEST_BOOL(v5.IsA<wdVec3>());
    WD_TEST_BOOL(v6.IsA<wdVec4>());
    WD_TEST_BOOL(v7.IsA<wdMat3>());
    WD_TEST_BOOL(v8.IsA<wdMat4>());
    WD_TEST_BOOL(v9.IsA<wdTransform>());
    WD_TEST_BOOL(v10.IsA<wdQuat>());
    WD_TEST_BOOL(v11.IsA<wdUuid>());
    WD_TEST_BOOL(v12.IsA<wdAngle>());

    WD_TEST_BOOL(v1.Get<wdColor>() == wdColor(1, 0, 0.5));
    WD_TEST_BOOL(v2.Get<wdColorGammaUB>() == wdColorGammaUB(128, 0, 32, 64));
    WD_TEST_FLOAT(v3.Get<wdTime>().GetSeconds(), 0.1, 0.0001f);
    WD_TEST_VEC2(v4.Get<wdVec2>(), wdVec2(0.1f, 2.0f), 0.0001f);
    WD_TEST_VEC3(v5.Get<wdVec3>(), wdVec3(0.1f, 2.0f, 3.2f), 0.0001f);
    WD_TEST_VEC4(v6.Get<wdVec4>(), wdVec4(0.1f, 2.0f, 3.2f, 44.5f), 0.0001f);
    WD_TEST_BOOL(v7.Get<wdMat3>().IsEqual(wdMat3(1, 4, 7, 2, 5, 8, 3, 6, 9), 0.0001f));
    WD_TEST_BOOL(v8.Get<wdMat4>().IsEqual(wdMat4(1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15, 4, 8, 12, 16), 0.0001f));
    WD_TEST_BOOL(v9.Get<wdTransform>().m_qRotation == wdQuat(4, 5, 6, 7));
    WD_TEST_VEC3(v9.Get<wdTransform>().m_vPosition, wdVec3(1, 2, 3), 0.0001f);
    WD_TEST_VEC3(v9.Get<wdTransform>().m_vScale, wdVec3(8, 9, 10), 0.0001f);
    WD_TEST_BOOL(v10.Get<wdQuat>() == wdQuat(0.1f, 2.0f, 3.2f, 44.5f));
    WD_TEST_BOOL(v11.Get<wdUuid>() == wdUuid(12345678910, 10987654321));
    WD_TEST_FLOAT(v12.Get<wdAngle>().GetRadian(), 45.23f, 0.0001f);


    /// \test Test primitive types in wdVariant
  }

  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////

  WD_TEST_BLOCK(wdTestBlock::Enabled, "StoreColor")
  {
    StreamComparer sc("Color $v1{float{1,2,3,4}}\n");

    wdOpenDdlWriter js;
    js.SetFloatPrecisionMode(wdOpenDdlWriter::FloatPrecisionMode::Readable);
    js.SetOutputStream(&sc);

    wdOpenDdlUtils::StoreColor(js, wdColor(1, 2, 3, 4), "v1", true);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "StoreColorGamma")
  {
    StreamComparer sc("ColorGamma $v1{uint8{1,2,3,4}}\n");

    wdOpenDdlWriter js;
    js.SetOutputStream(&sc);

    wdOpenDdlUtils::StoreColorGamma(js, wdColorGammaUB(1, 2, 3, 4), "v1", true);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "StoreTime")
  {
    StreamComparer sc("Time $v1{double{2.3}}\n");

    wdOpenDdlWriter js;
    js.SetFloatPrecisionMode(wdOpenDdlWriter::FloatPrecisionMode::Readable);
    js.SetOutputStream(&sc);

    wdOpenDdlUtils::StoreTime(js, wdTime::Seconds(2.3), "v1", true);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "StoreVec2")
  {
    StreamComparer sc("Vec2 $v1{float{1,2}}\n");

    wdOpenDdlWriter js;
    js.SetFloatPrecisionMode(wdOpenDdlWriter::FloatPrecisionMode::Readable);
    js.SetOutputStream(&sc);

    wdOpenDdlUtils::StoreVec2(js, wdVec2(1, 2), "v1", true);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "StoreVec3")
  {
    StreamComparer sc("Vec3 $v1{float{1,2,3}}\n");

    wdOpenDdlWriter js;
    js.SetFloatPrecisionMode(wdOpenDdlWriter::FloatPrecisionMode::Readable);
    js.SetOutputStream(&sc);

    wdOpenDdlUtils::StoreVec3(js, wdVec3(1, 2, 3), "v1", true);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "StoreVec4")
  {
    StreamComparer sc("Vec4 $v1{float{1,2,3,4}}\n");

    wdOpenDdlWriter js;
    js.SetFloatPrecisionMode(wdOpenDdlWriter::FloatPrecisionMode::Readable);
    js.SetOutputStream(&sc);

    wdOpenDdlUtils::StoreVec4(js, wdVec4(1, 2, 3, 4), "v1", true);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "StoreMat3")
  {
    StreamComparer sc("Mat3 $v1{float{1,4,7,2,5,8,3,6,9}}\n");

    wdOpenDdlWriter js;
    js.SetFloatPrecisionMode(wdOpenDdlWriter::FloatPrecisionMode::Readable);
    js.SetOutputStream(&sc);

    wdOpenDdlUtils::StoreMat3(js, wdMat3(1, 2, 3, 4, 5, 6, 7, 8, 9), "v1", true);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "StoreMat4")
  {
    StreamComparer sc("Mat4 $v1{float{1,5,9,13,2,6,10,14,3,7,11,15,4,8,12,16}}\n");

    wdOpenDdlWriter js;
    js.SetFloatPrecisionMode(wdOpenDdlWriter::FloatPrecisionMode::Readable);
    js.SetOutputStream(&sc);

    wdOpenDdlUtils::StoreMat4(js, wdMat4(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16), "v1", true);
  }

  // WD_TEST_BLOCK(wdTestBlock::Enabled, "StoreTransform")
  //{
  //  StreamComparer sc("Transform $v1{float{1,4,7,2,5,8,3,6,9,10}}\n");

  //  wdOpenDdlWriter js;
  //  js.SetFloatPrecisionMode(wdOpenDdlWriter::FloatPrecisionMode::Readable);
  //  js.SetOutputStream(&sc);

  //  wdOpenDdlUtils::StoreTransform(js, wdTransform(wdVec3(10, 20, 30), wdMat3(1, 2, 3, 4, 5, 6, 7, 8, 9)), "v1", true);
  //}

  WD_TEST_BLOCK(wdTestBlock::Enabled, "StoreQuat")
  {
    StreamComparer sc("Quat $v1{float{1,2,3,4}}\n");

    wdOpenDdlWriter js;
    js.SetFloatPrecisionMode(wdOpenDdlWriter::FloatPrecisionMode::Readable);
    js.SetOutputStream(&sc);

    wdOpenDdlUtils::StoreQuat(js, wdQuat(1, 2, 3, 4), "v1", true);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "StoreUuid")
  {
    StreamComparer sc("Uuid $v1{u4{12345678910,10987654321}}\n");

    wdOpenDdlWriter js;
    js.SetPrimitiveTypeStringMode(wdOpenDdlWriter::TypeStringMode::Shortest);
    js.SetOutputStream(&sc);

    wdOpenDdlUtils::StoreUuid(js, wdUuid(12345678910, 10987654321), "v1", true);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "StoreAngle")
  {
    StreamComparer sc("Angle $v1{float{2.3}}\n");

    wdOpenDdlWriter js;
    js.SetFloatPrecisionMode(wdOpenDdlWriter::FloatPrecisionMode::Readable);
    js.SetOutputStream(&sc);

    wdOpenDdlUtils::StoreAngle(js, wdAngle::Radian(2.3f), "v1", true);
  }

  // this test also covers all the types that Variant supports
  WD_TEST_BLOCK(wdTestBlock::Enabled, "StoreVariant")
  {
    alignas(WD_ALIGNMENT_OF(float)) wdUInt8 rawData[sizeof(float) * 16]; // enough for mat4

    for (wdUInt8 i = 0; i < WD_ARRAY_SIZE(rawData); ++i)
    {
      rawData[i] = i + 1;
    }

    rawData[WD_ARRAY_SIZE(rawData) - 1] = 0; // string terminator

    for (wdUInt32 t = wdVariant::Type::FirstStandardType + 1; t < wdVariant::Type::LastStandardType; ++t)
    {
      const wdVariant var = CreateVariant((wdVariant::Type::Enum)t, rawData);

      wdDefaultMemoryStreamStorage storage;
      wdMemoryStreamWriter writer(&storage);
      wdMemoryStreamReader reader(&storage);

      wdOpenDdlWriter js;
      js.SetFloatPrecisionMode(wdOpenDdlWriter::FloatPrecisionMode::Exact);
      js.SetOutputStream(&writer);

      wdOpenDdlUtils::StoreVariant(js, var, "bla");

      wdOpenDdlReader doc;
      WD_TEST_BOOL(doc.ParseDocument(reader).Succeeded());

      const auto pVarElem = doc.GetRootElement()->FindChild("bla");

      wdVariant result;
      wdOpenDdlUtils::ConvertToVariant(pVarElem, result).IgnoreResult();

      WD_TEST_BOOL(var == result);
    }
  }
}

static wdVariant CreateVariant(wdVariant::Type::Enum t, const void* pData)
{
  switch (t)
  {
    case wdVariant::Type::Bool:
      return wdVariant(*((bool*)pData));
    case wdVariant::Type::Int8:
      return wdVariant(*((wdInt8*)pData));
    case wdVariant::Type::UInt8:
      return wdVariant(*((wdUInt8*)pData));
    case wdVariant::Type::Int16:
      return wdVariant(*((wdInt16*)pData));
    case wdVariant::Type::UInt16:
      return wdVariant(*((wdUInt16*)pData));
    case wdVariant::Type::Int32:
      return wdVariant(*((wdInt32*)pData));
    case wdVariant::Type::UInt32:
      return wdVariant(*((wdUInt32*)pData));
    case wdVariant::Type::Int64:
      return wdVariant(*((wdInt64*)pData));
    case wdVariant::Type::UInt64:
      return wdVariant(*((wdUInt64*)pData));
    case wdVariant::Type::Float:
      return wdVariant(*((float*)pData));
    case wdVariant::Type::Double:
      return wdVariant(*((double*)pData));
    case wdVariant::Type::Color:
      return wdVariant(*((wdColor*)pData));
    case wdVariant::Type::Vector2:
      return wdVariant(*((wdVec2*)pData));
    case wdVariant::Type::Vector3:
      return wdVariant(*((wdVec3*)pData));
    case wdVariant::Type::Vector4:
      return wdVariant(*((wdVec4*)pData));
    case wdVariant::Type::Vector2I:
      return wdVariant(*((wdVec2I32*)pData));
    case wdVariant::Type::Vector3I:
      return wdVariant(*((wdVec3I32*)pData));
    case wdVariant::Type::Vector4I:
      return wdVariant(*((wdVec4I32*)pData));
    case wdVariant::Type::Vector2U:
      return wdVariant(*((wdVec2U32*)pData));
    case wdVariant::Type::Vector3U:
      return wdVariant(*((wdVec3U32*)pData));
    case wdVariant::Type::Vector4U:
      return wdVariant(*((wdVec4U32*)pData));
    case wdVariant::Type::Quaternion:
      return wdVariant(*((wdQuat*)pData));
    case wdVariant::Type::Matrix3:
      return wdVariant(*((wdMat3*)pData));
    case wdVariant::Type::Matrix4:
      return wdVariant(*((wdMat4*)pData));
    case wdVariant::Type::Transform:
      return wdVariant(*((wdTransform*)pData));
    case wdVariant::Type::String:
    case wdVariant::Type::StringView: // string views are stored as full strings as well
      return wdVariant((const char*)pData);
    case wdVariant::Type::DataBuffer:
    {
      wdDataBuffer db;
      db.SetCountUninitialized(sizeof(float) * 16);
      for (wdUInt32 i = 0; i < db.GetCount(); ++i)
        db[i] = ((wdUInt8*)pData)[i];

      return wdVariant(db);
    }
    case wdVariant::Type::Time:
      return wdVariant(*((wdTime*)pData));
    case wdVariant::Type::Uuid:
      return wdVariant(*((wdUuid*)pData));
    case wdVariant::Type::Angle:
      return wdVariant(*((wdAngle*)pData));
    case wdVariant::Type::ColorGamma:
      return wdVariant(*((wdColorGammaUB*)pData));

    default:
      WD_REPORT_FAILURE("Unknown type");
  }

  return wdVariant();
}
