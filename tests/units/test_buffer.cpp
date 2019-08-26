#include "test_bioseqzip.h"

class TestBuffer : public TestBioSeqZip
{
};

TEST_F (TestBuffer,
        testLoadStoreFastqSingleEnd)
{
	bioseqzip::SequenceReader<seqan::SeqFileIn,
	                          TSequenceRecord,
	                          std::false_type> reader;
	bioseqzip::SequenceWriter<seqan::SeqFileOut,
	                          TSequenceRecord> writer;
	bioseqzip::Buffer<TSequenceRecord>         buffer;

	// Initialize the source reader and output writer.
	reader.configureSingleEnd(TEST_FQ_SE);
	writer.configureTemporary(bioseqzip::LibraryLayout::SINGLE_END,
	                          outputTestDirPath);

	// Load the buffer reading the source data.
	buffer.load(reader);
	buffer.store(writer);
	writer.flush();

	// Perform test.
	EXPECT_TRUE(checkSameFastx(TEST_FQ_SE,
	                           writer.getOutputPaths()[0].first));
	remove(writer.getOutputPaths()[0].first);
}

TEST_F (TestBuffer,
        testLoadStoreFastqPairedEnd)
{
	bioseqzip::SequenceReader<seqan::SeqFileIn,
	                          TSequenceRecord,
	                          std::false_type> reader;
	bioseqzip::SequenceWriter<seqan::SeqFileOut,
	                          TSequenceRecord> writer;
	bioseqzip::Buffer<TSequenceRecord>         buffer;

	// Initialize the source reader and output writer.
	reader.configurePairedEnd(TEST_FQ_PE1,
	                          TEST_FQ_PE2);
	writer.configureTemporary(bioseqzip::LibraryLayout::PAIRED_END,
	                          outputTestDirPath);

	// Load the buffer reading the source data.
	buffer.load(reader);
	buffer.store(writer);
	writer.flush();

	// Perform test.
	EXPECT_TRUE(checkSameFastx(TEST_FQ_PE1,
	                           writer.getOutputPaths()[0].first));
	EXPECT_TRUE(checkSameFastx(TEST_FQ_PE2,
	                           writer.getOutputPaths()[0].second));
	remove(writer.getOutputPaths()[0].first);
	remove(writer.getOutputPaths()[0].second);
}

TEST_F (TestBuffer,
        testLoadStoreTagqSingleEnd)
{
	bioseqzip::SequenceReader<seqan::TagFileIn,
	                          TSequenceRecord,
	                          std::false_type> reader;
	bioseqzip::SequenceWriter<seqan::TagFileOut,
	                          TSequenceRecord> writer;
	bioseqzip::Buffer<TSequenceRecord>         buffer;

	// Initialize the source reader and output writer.
	reader.configureSingleEnd(TEST_TQ_SE);
	writer.configureTemporary(bioseqzip::LibraryLayout::SINGLE_END,
	                          outputTestDirPath);

	// Load the buffer reading the source data.
	buffer.load(reader);
	buffer.store(writer);
	writer.flush();

	// Perform test.
	EXPECT_TRUE(checkSameTagx(TEST_TQ_SE,
	                          writer.getOutputPaths()[0].first));
	remove(writer.getOutputPaths()[0].first);
}

TEST_F (TestBuffer,
        testLoadStoreTagqPairedEnd)
{
	bioseqzip::SequenceReader<seqan::TagFileIn,
	                          TSequenceRecord,
	                          std::false_type> reader;
	bioseqzip::SequenceWriter<seqan::TagFileOut,
	                          TSequenceRecord> writer;
	bioseqzip::Buffer<TSequenceRecord>         buffer;

	// Initialize the source reader and output writer.
	reader.configurePairedEnd(TEST_TQ_PE1,
	                          TEST_TQ_PE2);
	writer.configureTemporary(bioseqzip::LibraryLayout::PAIRED_END,
	                          outputTestDirPath);

	// Load the buffer reading the source data.
	buffer.load(reader);
	buffer.store(writer);
	writer.flush();

	// Perform test.
	EXPECT_TRUE(checkSameFastx(TEST_TQ_PE1,
	                           writer.getOutputPaths()[0].first));
	EXPECT_TRUE(checkSameFastx(TEST_TQ_PE2,
	                           writer.getOutputPaths()[0].second));
	remove(writer.getOutputPaths()[0].first);
	remove(writer.getOutputPaths()[0].second);
}

TEST_F (TestBuffer,
        testLoadStoreDetails)
{
	bioseqzip::DetailsReader<TDetailsRecord> reader;
	bioseqzip::DetailsWriter<TDetailsRecord> writer;
	bioseqzip::Buffer<TDetailsRecord>        buffer;

	// Initialize the source reader and output writer.
	reader.configure(TEST_DETAILS);
	writer.configureTemporary(outputTestDirPath,
	                          reader.getHeader());

	// Load the buffer reading the source data.
	buffer.load(reader);
	buffer.store(writer);
	writer.flush();

	// Perform test.
	EXPECT_TRUE(checkSameDetails(TEST_DETAILS,
	                             writer.getOutputPaths()[0]));
	remove(writer.getOutputPaths()[0]);
}

TEST_F (TestBuffer,
        testSortSequences)
{
	constexpr uint64_t minThreads = 1;
	constexpr uint64_t maxThreads = 4;

	bioseqzip::SequenceReader<seqan::SeqFileIn,
	                          TSequenceRecord,
	                          std::false_type> reader;
	bioseqzip::Buffer<TSequenceRecord>         buffer;

	for (auto i = minThreads;
	     i < maxThreads;
	     i++)
	{
		auto bufferSize = 0ul;

		// Initialize the source reader.
		reader.configureSingleEnd(TEST_FQ_SE);

		// Load the buffer reading the source data.
		bufferSize = buffer.load(reader);
		buffer.sort(i,
		            [] (const auto& left,
		                const auto& right)
		            {
			            return left.getSequence() < right.getSequence();
		            });

		// Perform test.
		for (auto j = 0ul;
		     j < bufferSize - 1;
		     j++)
		{
			if (buffer[j].getSequence() > buffer[j + 1].getSequence())
			{
				FAIL() << "Wrong records order with "
				       << i
				       << " threads";
			}
		}
	}
}

TEST_F (TestBuffer,
        testSortDetails)
{
	constexpr uint64_t minThreads = 1;
	constexpr uint64_t maxThreads = 4;

	bioseqzip::DetailsReader<TDetailsRecord> reader;
	bioseqzip::Buffer<TDetailsRecord>        buffer;

	for (auto i = minThreads;
	     i < maxThreads;
	     i++)
	{
		auto bufferSize = 0ul;

		// Initialize the source reader.
		reader.configure(TEST_DETAILS);

		// Load the buffer reading the source data.
		bufferSize = buffer.load(reader);
		buffer.sort(i,
		            [] (const auto& left,
		                const auto& right)
		            {
			            return left.getCount() < right.getCount();
		            });

		// Perform test.
		for (auto j = 0ul;
		     j < bufferSize - 1;
		     j++)
		{
			if (buffer[j].getCount() > buffer[j + 1].getCount())
			{
				FAIL() << "Wrong records order with "
				       << i
				       << " threads";
			}
		}
	}
}

TEST_F (TestBuffer,
        testCollapse)
{
	constexpr uint64_t minThreads = 1;
	constexpr uint64_t maxThreads = 4;

	bioseqzip::SequenceReader<seqan::SeqFileIn,
	                          TSequenceRecord,
	                          std::false_type> reader;
	bioseqzip::SequenceWriter<seqan::TagFileOut,
	                          TSequenceRecord> writer;
	bioseqzip::Buffer<TSequenceRecord>         buffer;

	for (auto i = minThreads;
	     i < maxThreads;
	     i++)
	{
		auto bufferSize = 0ul;

		// Initialize the source reader and output writer.
		reader.configureSingleEnd(TEST_FQ_SE);
		writer.configureTemporary(bioseqzip::LibraryLayout::SINGLE_END,
		                          outputTestDirPath);

		// Load the buffer reading the source data.
		bufferSize = buffer.load(reader);
		buffer.sort(i,
		            [] (const auto& left,
		                const auto& right)
		            {
			            return left.getSequence() < right.getSequence();
		            });
		buffer.collapse(i,
		                [] (auto& left,
		                    auto& right)
		                {
			                return left.getSequence() ==
			                       right.getSequence();
		                });
		buffer.storeIf(writer,
		               [] (const auto& record)
		               {
			               return record.getCount() > 0;
		               });
		writer.flush();

		// Perform test.
		EXPECT_TRUE(checkSameFastx(TEST_TQ_SE,
		                           writer.getOutputPaths()[0].first));
		remove(writer.getOutputPaths()[0].first);
	}
}

TEST_F(TestBuffer,
       testMerge)
{
	constexpr auto recordsReadPerRound = 7000ul;

	bioseqzip::SequenceReader<seqan::SeqFileIn,
	                          TSequenceRecord,
	                          std::false_type>              splitReader;
	std::vector<bioseqzip::SequenceReader<seqan::TagFileIn,
	                                      TSequenceRecord,
	                                      std::false_type>> readers;
	bioseqzip::SequenceWriter<seqan::TagFileOut,
	                          TSequenceRecord>              splitWriter;
	bioseqzip::SequenceWriter<seqan::TagFileOut,
	                          TSequenceRecord>              writer;
	bioseqzip::Buffer<TSequenceRecord>                      splitBuffer;

	// Create the temporary reader and writer, then simulate the creation of
	// a set of sorted and pre-collapsed batch files.
	splitReader.configureSingleEnd(TEST_FQ_SE);
	splitWriter.configureTemporary(bioseqzip::LibraryLayout::SINGLE_END,
	                               outputTestDirPath);

	while (!splitReader.atEnd())
	{
		splitBuffer.load(splitReader,
		                 recordsReadPerRound);
		splitBuffer.sort(1ul,
		                 [] (const auto& left,
		                     const auto& right)
		                 {
			                 return left.getSequence() <
			                        right.getSequence();
		                 });
		splitBuffer.collapse(1ul,
		                     [] (const auto& left,
		                         const auto& right)
		                     {
			                     return left.getSequence() ==
			                            right.getSequence();
		                     });
		splitBuffer.storeIf(splitWriter,
		                    [] (const auto& record)
		                    {
			                    return record.getCount() > 0;
		                    });
		if (!splitReader.atEnd())
		{
			splitWriter.switchSink();
		}
	}
	splitWriter.flush();

	// Init the test readers and writer. Then, launch the merge procedure.
	readers.resize(splitWriter.getOutputPaths().size());
	for (auto i = 0ul;
	     i < splitWriter.getOutputPaths().size();
	     i++)
	{
		readers[i].configureSingleEnd(splitWriter.getOutputPaths()[i].first);
	}
	writer.configureTemporary(bioseqzip::LibraryLayout::SINGLE_END,
	                          outputTestDirPath);
	bioseqzip::Buffer<TSequenceRecord>::merge(readers,
	                                          writer,
	                                          1000ul);
	writer.flush();

	// Perform test.
	EXPECT_TRUE(checkSameTagx(TEST_TQ_SE,
	                          writer.getOutputPaths()[0].first));
	remove(writer.getOutputPaths()[0].first);
	for (auto& p : splitWriter.getOutputPaths()) {
		remove(p.first);
	}
}
