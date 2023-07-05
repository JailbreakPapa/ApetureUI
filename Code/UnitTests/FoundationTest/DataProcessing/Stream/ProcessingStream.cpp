
#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/DataProcessing/Stream/DefaultImplementations/ZeroInitializer.h>
#include <Foundation/DataProcessing/Stream/ProcessingStreamGroup.h>
#include <Foundation/DataProcessing/Stream/ProcessingStreamIterator.h>
#include <Foundation/DataProcessing/Stream/ProcessingStreamProcessor.h>
#include <Foundation/Reflection/Reflection.h>

WD_CREATE_SIMPLE_TEST_GROUP(DataProcessing);

// Add processor

class AddOneStreamProcessor : public wdProcessingStreamProcessor
{
  WD_ADD_DYNAMIC_REFLECTION(AddOneStreamProcessor, wdProcessingStreamProcessor);

public:
  AddOneStreamProcessor()
    : m_pStream(nullptr)
  {
  }

  void SetStreamName(wdHashedString sStreamName) { m_sStreamName = sStreamName; }

protected:
  virtual wdResult UpdateStreamBindings() override
  {
    m_pStream = m_pStreamGroup->GetStreamByName(m_sStreamName);

    return m_pStream ? WD_SUCCESS : WD_FAILURE;
  }

  virtual void InitializeElements(wdUInt64 uiStartIndex, wdUInt64 uiNumElements) override {}

  virtual void Process(wdUInt64 uiNumElements) override
  {
    wdProcessingStreamIterator<float> streamIterator(m_pStream, uiNumElements, 0);

    while (!streamIterator.HasReachedEnd())
    {
      streamIterator.Current() += 1.0f;

      streamIterator.Advance();
    }
  }

  wdHashedString m_sStreamName;
  wdProcessingStream* m_pStream;
};

WD_BEGIN_DYNAMIC_REFLECTED_TYPE(AddOneStreamProcessor, 1, wdRTTIDefaultAllocator<AddOneStreamProcessor>)
WD_END_DYNAMIC_REFLECTED_TYPE;

WD_CREATE_SIMPLE_TEST(DataProcessing, ProcessingStream)
{
  wdProcessingStreamGroup Group;
  wdProcessingStream* pStream1 = Group.AddStream("Stream1", wdProcessingStream::DataType::Float);
  wdProcessingStream* pStream2 = Group.AddStream("Stream2", wdProcessingStream::DataType::Float3);

  WD_TEST_BOOL(pStream1 != nullptr);
  WD_TEST_BOOL(pStream2 != nullptr);

  wdProcessingStreamSpawnerZeroInitialized* pSpawner1 = WD_DEFAULT_NEW(wdProcessingStreamSpawnerZeroInitialized);
  wdProcessingStreamSpawnerZeroInitialized* pSpawner2 = WD_DEFAULT_NEW(wdProcessingStreamSpawnerZeroInitialized);

  pSpawner1->SetStreamName(pStream1->GetName());
  pSpawner2->SetStreamName(pStream2->GetName());

  Group.AddProcessor(pSpawner1);
  Group.AddProcessor(pSpawner2);

  Group.SetSize(128);

  WD_TEST_INT(Group.GetNumElements(), 128);
  WD_TEST_INT(Group.GetNumActiveElements(), 0);

  Group.InitializeElements(3);

  Group.Process();

  WD_TEST_INT(Group.GetNumActiveElements(), 3);


  {
    wdProcessingStreamIterator<float> stream1Iterator(pStream1, 3, 0);

    int iElementsVisited = 0;
    while (!stream1Iterator.HasReachedEnd())
    {
      WD_TEST_FLOAT(stream1Iterator.Current(), 0.0f, 0.0f);

      stream1Iterator.Advance();
      iElementsVisited++;
    }

    WD_TEST_INT(iElementsVisited, 3);
  }

  Group.InitializeElements(7);

  Group.Process();

  {
    wdProcessingStreamIterator<wdVec3> stream2Iterator(pStream2, Group.GetNumActiveElements(), 0);

    int iElementsVisited = 0;
    while (!stream2Iterator.HasReachedEnd())
    {
      WD_TEST_FLOAT(stream2Iterator.Current().x, 0.0f, 0.0f);
      WD_TEST_FLOAT(stream2Iterator.Current().y, 0.0f, 0.0f);
      WD_TEST_FLOAT(stream2Iterator.Current().z, 0.0f, 0.0f);

      stream2Iterator.Advance();
      iElementsVisited++;
    }

    WD_TEST_INT(iElementsVisited, 10);
  }

  WD_TEST_INT(Group.GetHighestNumActiveElements(), 10);

  Group.RemoveElement(5);
  Group.RemoveElement(7);

  Group.Process();

  WD_TEST_INT(Group.GetHighestNumActiveElements(), 10);
  WD_TEST_INT(Group.GetNumActiveElements(), 8);

  AddOneStreamProcessor* pProcessor1 = WD_DEFAULT_NEW(AddOneStreamProcessor);
  pProcessor1->SetStreamName(pStream1->GetName());

  Group.AddProcessor(pProcessor1);

  Group.Process();

  {
    wdProcessingStreamIterator<float> stream1Iterator(pStream1, Group.GetNumActiveElements(), 0);
    while (!stream1Iterator.HasReachedEnd())
    {
      WD_TEST_FLOAT(stream1Iterator.Current(), 1.0f, 0.001f);

      stream1Iterator.Advance();
    }
  }

  Group.Process();

  {
    wdProcessingStreamIterator<float> stream1Iterator(pStream1, Group.GetNumActiveElements(), 0);
    while (!stream1Iterator.HasReachedEnd())
    {
      WD_TEST_FLOAT(stream1Iterator.Current(), 2.0f, 0.001f);

      stream1Iterator.Advance();
    }
  }
}
