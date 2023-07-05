#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/IO/DeduplicationReadContext.h>
#include <Foundation/IO/DeduplicationWriteContext.h>
#include <Foundation/IO/MemoryStream.h>
#include <Foundation/Types/UniquePtr.h>

namespace
{
  struct RefCountedVec3 : public wdRefCounted
  {
    RefCountedVec3() = default;
    RefCountedVec3(const wdVec3& v)
      : m_v(v)
    {
    }

    wdResult Serialize(wdStreamWriter& inout_stream) const
    {
      inout_stream << m_v;
      return WD_SUCCESS;
    }

    wdResult Deserialize(wdStreamReader& inout_stream)
    {
      inout_stream >> m_v;
      return WD_SUCCESS;
    }

    wdVec3 m_v;
  };

  struct ComplexComponent
  {
    wdTransform* m_pTransform = nullptr;
    wdVec3* m_pPosition = nullptr;
    wdSharedPtr<RefCountedVec3> m_pScale;
    wdUInt32 m_uiIndex = wdInvalidIndex;

    wdResult Serialize(wdStreamWriter& inout_stream) const
    {
      WD_SUCCEED_OR_RETURN(wdDeduplicationWriteContext::GetContext()->WriteObject(inout_stream, m_pTransform));
      WD_SUCCEED_OR_RETURN(wdDeduplicationWriteContext::GetContext()->WriteObject(inout_stream, m_pPosition));
      WD_SUCCEED_OR_RETURN(wdDeduplicationWriteContext::GetContext()->WriteObject(inout_stream, m_pScale));

      inout_stream << m_uiIndex;
      return WD_SUCCESS;
    }

    wdResult Deserialize(wdStreamReader& inout_stream)
    {
      WD_SUCCEED_OR_RETURN(wdDeduplicationReadContext::GetContext()->ReadObject(inout_stream, m_pTransform));
      WD_SUCCEED_OR_RETURN(wdDeduplicationReadContext::GetContext()->ReadObject(inout_stream, m_pPosition));
      WD_SUCCEED_OR_RETURN(wdDeduplicationReadContext::GetContext()->ReadObject(inout_stream, m_pScale));

      inout_stream >> m_uiIndex;
      return WD_SUCCESS;
    }
  };

  struct ComplexObject
  {
    wdDynamicArray<wdUniquePtr<wdTransform>> m_Transforms;
    wdDynamicArray<wdVec3> m_Positions;
    wdDynamicArray<wdSharedPtr<RefCountedVec3>> m_Scales;

    wdDynamicArray<ComplexComponent> m_Components;

    wdMap<wdUInt32, wdTransform*> m_TransformMap;
    wdSet<wdVec3*> m_UniquePositions;

    wdResult Serialize(wdStreamWriter& inout_stream) const
    {
      WD_SUCCEED_OR_RETURN(wdDeduplicationWriteContext::GetContext()->WriteArray(inout_stream, m_Transforms));
      WD_SUCCEED_OR_RETURN(wdDeduplicationWriteContext::GetContext()->WriteArray(inout_stream, m_Positions));
      WD_SUCCEED_OR_RETURN(wdDeduplicationWriteContext::GetContext()->WriteArray(inout_stream, m_Scales));
      WD_SUCCEED_OR_RETURN(
        wdDeduplicationWriteContext::GetContext()->WriteMap(inout_stream, m_TransformMap, wdDeduplicationWriteContext::WriteMapMode::DedupValue));
      WD_SUCCEED_OR_RETURN(wdDeduplicationWriteContext::GetContext()->WriteSet(inout_stream, m_UniquePositions));
      WD_SUCCEED_OR_RETURN(inout_stream.WriteArray(m_Components));
      return WD_SUCCESS;
    }

    wdResult Deserialize(wdStreamReader& inout_stream)
    {
      WD_SUCCEED_OR_RETURN(wdDeduplicationReadContext::GetContext()->ReadArray(inout_stream, m_Transforms));
      WD_SUCCEED_OR_RETURN(wdDeduplicationReadContext::GetContext()->ReadArray(inout_stream, m_Positions,
        nullptr)); // should not allocate anything
      WD_SUCCEED_OR_RETURN(wdDeduplicationReadContext::GetContext()->ReadArray(inout_stream, m_Scales));
      WD_SUCCEED_OR_RETURN(wdDeduplicationReadContext::GetContext()->ReadMap(
        inout_stream, m_TransformMap, wdDeduplicationReadContext::ReadMapMode::DedupValue, nullptr, nullptr));           // should not allocate anything
      WD_SUCCEED_OR_RETURN(wdDeduplicationReadContext::GetContext()->ReadSet(inout_stream, m_UniquePositions, nullptr)); // should not allocate anything
      WD_SUCCEED_OR_RETURN(inout_stream.ReadArray(m_Components));
      return WD_SUCCESS;
    }
  };
} // namespace

WD_CREATE_SIMPLE_TEST(IO, DeduplicationContext)
{
  wdDefaultMemoryStreamStorage streamStorage;

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Writer")
  {
    wdMemoryStreamWriter writer(&streamStorage);

    wdDeduplicationWriteContext dedupWriteContext;

    ComplexObject obj;
    for (wdUInt32 i = 0; i < 20; ++i)
    {
      obj.m_Transforms.ExpandAndGetRef() = WD_DEFAULT_NEW(wdTransform, wdVec3(static_cast<float>(i), 0, 0));
      obj.m_Positions.ExpandAndGetRef() = wdVec3(1, 2, static_cast<float>(i));
      obj.m_Scales.ExpandAndGetRef() = WD_DEFAULT_NEW(RefCountedVec3, wdVec3(0, static_cast<float>(i), 0));
    }

    for (wdUInt32 i = 0; i < 10; ++i)
    {
      auto& component = obj.m_Components.ExpandAndGetRef();
      component.m_uiIndex = i * 2;
      component.m_pTransform = obj.m_Transforms[component.m_uiIndex].Borrow();
      component.m_pPosition = &obj.m_Positions[component.m_uiIndex];
      component.m_pScale = obj.m_Scales[component.m_uiIndex];

      obj.m_TransformMap.Insert(i, obj.m_Transforms[i].Borrow());
      obj.m_UniquePositions.Insert(&obj.m_Positions[i]);
    }



    WD_TEST_BOOL(obj.Serialize(writer).Succeeded());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Reader")
  {
    wdMemoryStreamReader reader(&streamStorage);

    wdDeduplicationReadContext dedupReadContext;

    ComplexObject obj;
    WD_TEST_BOOL(obj.Deserialize(reader).Succeeded());

    WD_TEST_INT(obj.m_Transforms.GetCount(), 20);
    WD_TEST_INT(obj.m_Positions.GetCount(), 20);
    WD_TEST_INT(obj.m_Scales.GetCount(), 20);
    WD_TEST_INT(obj.m_TransformMap.GetCount(), 10);
    WD_TEST_INT(obj.m_UniquePositions.GetCount(), 10);
    WD_TEST_INT(obj.m_Components.GetCount(), 10);

    for (wdUInt32 i = 0; i < obj.m_Components.GetCount(); ++i)
    {
      auto& component = obj.m_Components[i];

      WD_TEST_BOOL(component.m_pTransform == obj.m_Transforms[component.m_uiIndex].Borrow());
      WD_TEST_BOOL(component.m_pPosition == &obj.m_Positions[component.m_uiIndex]);
      WD_TEST_BOOL(component.m_pScale == obj.m_Scales[component.m_uiIndex]);

      WD_TEST_BOOL(component.m_pTransform->m_vPosition == wdVec3(static_cast<float>(i) * 2, 0, 0));
      WD_TEST_BOOL(*component.m_pPosition == wdVec3(1, 2, static_cast<float>(i) * 2));
      WD_TEST_BOOL(component.m_pScale->m_v == wdVec3(0, static_cast<float>(i) * 2, 0));
    }

    for (wdUInt32 i = 0; i < 10; ++i)
    {
      if (WD_TEST_BOOL(obj.m_TransformMap.GetValue(i) != nullptr))
      {
        WD_TEST_BOOL(*obj.m_TransformMap.GetValue(i) == obj.m_Transforms[i].Borrow());
      }

      WD_TEST_BOOL(obj.m_UniquePositions.Contains(&obj.m_Positions[i]));
    }
  }
}
