#ifndef BIOSEQZIP_TESTS_UNITS_TEST_BIOSEQZIP_H
#define BIOSEQZIP_TESTS_UNITS_TEST_BIOSEQZIP_H

#include <gtest/gtest.h>

#include <seqan/seq_io.h>
#include <seqan/stream.h>

#include "bioseqzip/core/library_layout.h"
#include "bioseqzip/core/details_reader.h"
#include "bioseqzip/core/details_record.h"
#include "bioseqzip/core/details_writer.h"
#include "bioseqzip/core/sequence_reader.h"
#include "bioseqzip/core/sequence_record.h"
#include "bioseqzip/core/sequence_writer.h"

#include "bioseqzip/buffer.h"
#include "../../apps/collapse/sample_tracker.h"

#if BioSeqZip_APPLE

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#else

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#endif

class TestBioSeqZip : public ::testing::Test
{
protected:

	using TTag = seqan::String<seqan::Dna5>;
	using TQual = seqan::String<char>;
	using TCounter = uint32_t;
	using TBpOffset = uint16_t;
	using TDetailsRecord = bioseqzip::DetailsRecord<TCounter>;
	using TSequenceRecord = bioseqzip::SequenceRecord<TTag,
	                                                  TQual,
	                                                  TCounter,
	                                                  TBpOffset>;

	fs::path outputTestDirPath;

	inline void
	SetUp ()
	{
		outputTestDirPath = fs::current_path();
	}

	inline ::testing::AssertionResult
	checkSameFastx (const fs::path& pathFile1,
	                const fs::path& pathFile2) noexcept
	{
		seqan::SeqFileIn                          sourceStream1;
		seqan::SeqFileIn                          sourceStream2;
		seqan::String<seqan::String<char>>        headers1;
		seqan::String<seqan::String<seqan::Dna5>> sequences1;
		seqan::String<seqan::String<char>>        headers2;
		seqan::String<seqan::String<seqan::Dna5>> sequences2;

		// Open the source files.
		seqan::open(sourceStream1,
		            pathFile1.generic_string().data());
		seqan::open(sourceStream2,
		            pathFile2.generic_string().data());

		// Read data from the source files.
		seqan::readRecords(headers1,
		                   sequences1,
		                   sourceStream1);
		seqan::readRecords(headers2,
		                   sequences2,
		                   sourceStream2);

		// Check the two files have the same number of records.
		if (seqan::length(sequences1) != seqan::length(sequences2))
		{
			return ::testing::AssertionFailure() << "The files have a "
			                                        "different number of "
			                                        "records";
		}

		// Check sequence by sequence equality.
		for (auto i = 0ull;
		     i < seqan::length(sequences1);
		     i++)
		{
			if (sequences1[i] != sequences2[i])
			{
				return ::testing::AssertionFailure() << "Sequence "
				                                     << seqan::toCString(sequences1[i])
				                                     << " != "
				                                     << seqan::toCString(sequences2[i]);
			}
		}

		// Close the source files.
		seqan::close(sourceStream1);
		seqan::close(sourceStream2);

		return ::testing::AssertionSuccess();
	}

	inline ::testing::AssertionResult
	checkSameTagx (const fs::path& pathFile1,
	               const fs::path& pathFile2) noexcept
	{
		seqan::TagFileIn                          sourceStream1;
		seqan::TagFileIn                          sourceStream2;
		seqan::String<seqan::String<seqan::Dna5>> sequences1;
		seqan::String<TCounter>                   counters1;
		seqan::String<seqan::String<seqan::Dna5>> sequences2;
		seqan::String<TCounter>                   counters2;


		// Open the source files.
		seqan::open(sourceStream1,
		            pathFile1.generic_string().data());
		seqan::open(sourceStream2,
		            pathFile2.generic_string().data());

		// Read data from the source files.
		seqan::readRecords(sequences1,
		                   counters1,
		                   sourceStream1);
		seqan::readRecords(sequences2,
		                   counters2,
		                   sourceStream2);

		// Check the two files have the same number of records.
		if (seqan::length(sequences1) != seqan::length(sequences2))
		{
			return ::testing::AssertionFailure() << "The files have a "
			                                        "different number of "
			                                        "records";
		}

		// Check sequence by sequence equality.
		for (auto i = 0ull;
		     i < seqan::length(sequences1);
		     i++)
		{
			if (sequences1[i] != sequences2[i])
			{
				return ::testing::AssertionFailure() << "Sequence "
				                                     << seqan::toCString(sequences1[i])
				                                     << " != "
				                                     << seqan::toCString(sequences2[i]);
			}
			if (counters1[i] != counters2[i])
			{
				return ::testing::AssertionFailure() << "Counter "
				                                     << counters1[i]
				                                     << " != "
				                                     << counters2[i];
			}
		}

		// Close the source files.
		seqan::close(sourceStream1);
		seqan::close(sourceStream2);

		return ::testing::AssertionSuccess();
	}

	inline ::testing::AssertionResult
	checkSameDetails (const fs::path& pathFile1,
	                  const fs::path& pathFile2) noexcept
	{
		std::ifstream sourceStream1;
		std::ifstream sourceStream2;
		std::string   line1;
		std::string   line2;

		// Open the source files.
		sourceStream1.open(pathFile1);
		sourceStream2.open(pathFile2);

		// Check the headers of the two source files are the same
		std::getline(sourceStream1,
		             line1);
		std::getline(sourceStream2,
		             line2);
		if (line1 != line2)
		{
			return ::testing::AssertionFailure() << "Different headers : "
			                                     << line1
			                                     << " vs "
			                                     << line2
			                                     << std::endl;
		}

		// Check the contents of the files are the same.
		while (!sourceStream1.eof() && !sourceStream2.eof())
		{
			std::getline(sourceStream1,
			             line1);
			std::getline(sourceStream2,
			             line2);
			if (line1 != line2)
			{
				return ::testing::AssertionFailure() << "Different lines : "
				                                     << line1
				                                     << " vs "
				                                     << line2
				                                     << std::endl;
			}
		}

		// Close the source files.
		sourceStream1.close();
		sourceStream2.close();

		return ::testing::AssertionSuccess();
	}
};

#endif // BIOSEQZIP_TESTS_UNITS_TEST_BIOSEQZIP_H
