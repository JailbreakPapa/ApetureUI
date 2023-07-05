#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Containers/Deque.h>
#include <Foundation/IO/ChunkStream.h>
#include <Foundation/IO/MemoryStream.h>

WD_CREATE_SIMPLE_TEST(IO, ChunkStream)
{
  wdDefaultMemoryStreamStorage StreamStorage;

  wdMemoryStreamWriter MemoryWriter(&StreamStorage);
  wdMemoryStreamReader MemoryReader(&StreamStorage);

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Write Format")
  {
    wdChunkStreamWriter writer(MemoryWriter);

    writer.BeginStream(1);

    {
      writer.BeginChunk("Chunk1", 1);

      writer << (wdUInt32)4;
      writer << (float)5.6f;
      writer << (double)7.8;
      writer << "nine";
      writer << wdVec3(10, 11.2f, 13.4f);

      writer.EndChunk();
    }

    {
      writer.BeginChunk("Chunk2", 2);

      writer << "chunk 2 content";

      writer.EndChunk();
    }

    {
      writer.BeginChunk("Chunk3", 3);

      writer << "chunk 3 content";

      writer.EndChunk();
    }

    writer.EndStream();
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Read Format")
  {
    wdChunkStreamReader reader(MemoryReader);

    reader.BeginStream();

    // Chunk 1
    {
      WD_TEST_BOOL(reader.GetCurrentChunk().m_bValid);
      WD_TEST_STRING(reader.GetCurrentChunk().m_sChunkName.GetData(), "Chunk1");
      WD_TEST_INT(reader.GetCurrentChunk().m_uiChunkVersion, 1);
      WD_TEST_INT(reader.GetCurrentChunk().m_uiChunkBytes, reader.GetCurrentChunk().m_uiUnreadChunkBytes);
      WD_TEST_INT(reader.GetCurrentChunk().m_uiChunkBytes, 36);

      wdUInt32 i;
      float f;
      double d;
      wdString s;

      reader >> i;
      reader >> f;
      reader >> d;
      reader >> s;

      WD_TEST_INT(i, 4);
      WD_TEST_FLOAT(f, 5.6f, 0);
      WD_TEST_DOUBLE(d, 7.8, 0);
      WD_TEST_STRING(s.GetData(), "nine");

      WD_TEST_INT(reader.GetCurrentChunk().m_uiUnreadChunkBytes, 12);
      reader.NextChunk();
    }

    // Chunk 2
    {
      WD_TEST_BOOL(reader.GetCurrentChunk().m_bValid);
      WD_TEST_STRING(reader.GetCurrentChunk().m_sChunkName.GetData(), "Chunk2");
      WD_TEST_INT(reader.GetCurrentChunk().m_uiChunkVersion, 2);
      WD_TEST_INT(reader.GetCurrentChunk().m_uiChunkBytes, reader.GetCurrentChunk().m_uiUnreadChunkBytes);
      WD_TEST_INT(reader.GetCurrentChunk().m_uiChunkBytes, 19);

      wdString s;

      reader >> s;

      WD_TEST_STRING(s.GetData(), "chunk 2 content");

      WD_TEST_INT(reader.GetCurrentChunk().m_uiUnreadChunkBytes, 0);
      reader.NextChunk();
    }

    // Chunk 3
    {
      WD_TEST_BOOL(reader.GetCurrentChunk().m_bValid);
      WD_TEST_STRING(reader.GetCurrentChunk().m_sChunkName.GetData(), "Chunk3");
      WD_TEST_INT(reader.GetCurrentChunk().m_uiChunkVersion, 3);
      WD_TEST_INT(reader.GetCurrentChunk().m_uiChunkBytes, reader.GetCurrentChunk().m_uiUnreadChunkBytes);
      WD_TEST_INT(reader.GetCurrentChunk().m_uiChunkBytes, 19);

      wdString s;

      reader >> s;

      WD_TEST_STRING(s.GetData(), "chunk 3 content");

      WD_TEST_INT(reader.GetCurrentChunk().m_uiUnreadChunkBytes, 0);
      reader.NextChunk();
    }

    WD_TEST_BOOL(!reader.GetCurrentChunk().m_bValid);

    reader.SetEndChunkFileMode(wdChunkStreamReader::EndChunkFileMode::SkipToEnd);
    reader.EndStream();

    wdUInt8 Temp[1024];
    WD_TEST_INT(MemoryReader.ReadBytes(Temp, 1024), 0); // nothing left to read
  }
}
