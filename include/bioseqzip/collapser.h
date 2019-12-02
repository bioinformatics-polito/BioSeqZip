/**
 * \file   bioseqzip/collapser.h
 * \author Gianvito Urgese
 * \author Emanuele Parisi
 * \date   February, 2019
 */

#ifndef BIOSEQZIP_INCLUDE_BIOSEQZIP_COLLAPSER_H
#define BIOSEQZIP_INCLUDE_BIOSEQZIP_COLLAPSER_H

#include <array>
#include <experimental/filesystem>
#include <vector>

#include "cpputils/cpputils.h"

#include "tag_io.h"

#include "bioseqzip/core/details_reader.h"
#include "bioseqzip/core/details_record.h"
#include "bioseqzip/core/details_writer.h"
#include "bioseqzip/core/sequence_reader.h"
#include "bioseqzip/core/sequence_record.h"
#include "bioseqzip/core/sequence_writer.h"
#include "bioseqzip/buffer.h"
#include "bioseqzip/results_collapse.h"
#include "bioseqzip/stats_collapse.h"

namespace fs = std::experimental::filesystem;

namespace bioseqzip
{

/**
 * Class providing facilities for collapsing a single sample. It is able to
 * handle both single-end and paired-end, interleaved or breakpoint library
 * layouts.
 */
class Collapser
{
public:
	/**
	 * Class constructor.
	 */
	Collapser () noexcept
	{
		outputDirPath_    = fs::current_path();
		temporaryDirPath_ = fs::current_path();
		maxOutputRecords_ = std::numeric_limits<uint64_t>::max();
		maxRam_           = 1024ull * 1024ull * 1024ull;
		trimLeft_         = 0;
		trimRight_        = 0;
		nThreads_         = 1;
	}

	/**
	 * Class copy constructor.
	 *
	 * \param other is the object the current instance is initialized from.
	 */
	Collapser (const Collapser& other) = default;

	/**
	 * Class move constructor.
	 *
	 * \param other is the object moved into the current instance.
	 */
	Collapser (Collapser&& other) noexcept = default;

	inline void
	configure (const fs::path& outputDirPath,
	           const fs::path& temporaryDirPath,
	           uint64_t maxOutputRecords,
	           uint64_t maxRam,
	           uint64_t trimLeft,
	           uint64_t trimRight,
	           uint64_t nThreads) noexcept
	{
		outputDirPath_    = outputDirPath;
		temporaryDirPath_ = temporaryDirPath;
		maxOutputRecords_ = maxOutputRecords;
		maxRam_           = maxRam;
		trimLeft_         = trimLeft;
		trimRight_        = trimRight;
		nThreads_         = nThreads;
	}

	template <typename TRecord,
	          typename TOutStream,
	          typename TForceNoQual>
	inline auto
	collapseSingleEnd (const fs::path& samplePath,
	                   const fs::path& outputBasename) noexcept
	{
		StatsCollapse                stats;
		cpputils::Chronometer<>      timer;
		SequenceReader<seqan::SeqFileIn,
		               TRecord,
		               TForceNoQual> reader;
		SequenceWriter<seqan::TagFileOut,
		               TRecord>      tmpWriter;
		SequenceWriter<TOutStream,
		               TRecord>      outWriter;

		// Start collapse time measurement.
		timer.start();

		// Initialize reader and writers.
		reader.configureSingleEnd(samplePath);
		tmpWriter.configureTemporary(LibraryLayout::SINGLE_END,
		                             temporaryDirPath_);
		outWriter.configure(LibraryLayout::SINGLE_END,
		                    outputDirPath_,
		                    outputBasename,
		                    maxOutputRecords_);

		// Run the collapse procedure and fill the statistics data structure.
		collapseSS_(reader,
		            tmpWriter,
		            outWriter,
		            stats);
		stats.sampleId = outputBasename.generic_string();
		stats.rawSpace = fs::file_size(samplePath);
		for (auto&& s : outWriter.getOutputPaths())
		{
			stats.collapsedSpace += fs::file_size(s.first);
		}
		stats.runtime  = timer.stop();

		return ResultsCollapse(std::move(outWriter.getOutputPaths()),
		                       {},
		                       {stats});
	}

	template <typename TRecord,
	          typename TOutStream,
	          typename TForceNoQual>
	inline auto
	collapsePairedEnd (const fs::path& samplePath,
	                   const fs::path& pairedPath,
	                   const fs::path& outputBasename) noexcept
	{
		StatsCollapse                stats;
		cpputils::Chronometer<>      timer;
		SequenceReader<seqan::SeqFileIn,
		               TRecord,
		               TForceNoQual> reader;
		SequenceWriter<seqan::TagFileOut,
		               TRecord>      tmpWriter;
		SequenceWriter<TOutStream,
		               TRecord>      outWriter;

		// Start collapse time measurement.
		timer.start();

		// Initialize reader and writers.
		reader.configurePairedEnd(samplePath,
		                          pairedPath);
		tmpWriter.configureTemporary(LibraryLayout::PAIRED_END,
		                             temporaryDirPath_);
		outWriter.configure(LibraryLayout::PAIRED_END,
		                    outputDirPath_,
		                    outputBasename,
		                    maxOutputRecords_);

		// Run the collapse procedure and fill the statistics data structure.
		collapseSS_(reader,
		            tmpWriter,
		            outWriter,
		            stats);
		stats.sampleId = outputBasename.generic_string();
		stats.rawSpace = fs::file_size(samplePath);
		stats.rawSpace += fs::file_size(pairedPath);
		for (auto&& s : outWriter.getOutputPaths())
		{
			stats.collapsedSpace += fs::file_size(s.first);
			stats.collapsedSpace += fs::file_size(s.second);
		}
		stats.runtime  = timer.stop();

		return ResultsCollapse(std::move(outWriter.getOutputPaths()),
		                       {},
		                       {stats});
	}

	template <typename TRecord,
	          typename TOutStream,
	          typename TForceNoQual>
	inline auto
	collapseInterleaved (const fs::path& samplePath,
	                     const fs::path& outputBasename) noexcept
	{
		StatsCollapse                stats;
		cpputils::Chronometer<>      timer;
		SequenceReader<seqan::SeqFileIn,
		               TRecord,
		               TForceNoQual> reader;
		SequenceWriter<seqan::TagFileOut,
		               TRecord>      tmpWriter;
		SequenceWriter<TOutStream,
		               TRecord>      outWriter;

		// Start collapse time measurement.
		timer.start();

		// Initialize reader and writers.
		reader.configureInterleaved(samplePath);
		tmpWriter.configureTemporary(LibraryLayout::INTERLEAVED,
		                             temporaryDirPath_);
		outWriter.configure(LibraryLayout::INTERLEAVED,
		                    outputDirPath_,
		                    outputBasename,
		                    maxOutputRecords_);

		// Run the collapse procedure and fill the statistics data structure.
		collapseSS_(reader,
		            tmpWriter,
		            outWriter,
		            stats);
		stats.sampleId = outputBasename.generic_string();
		stats.rawSpace = fs::file_size(samplePath);
		for (auto&& s : outWriter.getOutputPaths())
		{
			stats.collapsedSpace += fs::file_size(s.first);
			stats.collapsedSpace += fs::file_size(s.second);
		}
		stats.runtime  = timer.stop();

		return ResultsCollapse(std::move(outWriter.getOutputPaths()),
		                       {},
		                       {stats});
	}

	template <typename TRecord,
	          typename TOutStream,
	          typename TForceNoQual>
	inline auto
	collapseBreakpoint (const fs::path& samplePath,
	                    uint64_t bpOffset,
	                    const fs::path& outputBasename) noexcept
	{
		StatsCollapse                stats;
		cpputils::Chronometer<>      timer;
		SequenceReader<seqan::SeqFileIn,
		               TRecord,
		               TForceNoQual> reader;
		SequenceWriter<seqan::TagFileOut,
		               TRecord>      tmpWriter;
		SequenceWriter<TOutStream,
		               TRecord>      outWriter;

		// Start collapse time measurement.
		timer.start();

		// Initialize reader and writers.
		reader.configureBreakpoint(samplePath,
		                           bpOffset);
		tmpWriter.configureTemporary(LibraryLayout::BREAKPOINT,
		                             temporaryDirPath_);
		outWriter.configure(LibraryLayout::BREAKPOINT,
		                    outputDirPath_,
		                    outputBasename,
		                    maxOutputRecords_);

		// Run the collapse procedure and fill the statistics data structure.
		collapseSS_(reader,
		            tmpWriter,
		            outWriter,
		            stats);
		stats.sampleId = outputBasename.generic_string();
		stats.rawSpace = fs::file_size(samplePath);
		for (auto&& s : outWriter.getOutputPaths())
		{
			stats.collapsedSpace += fs::file_size(s.first);
			stats.collapsedSpace += fs::file_size(s.second);
		}
		stats.runtime  = timer.stop();

		return ResultsCollapse(std::move(outWriter.getOutputPaths()),
		                       {},
		                       {stats});
	}

	template <typename TSeqRecord,
	          typename TTabRecord,
	          typename TOutStream,
	          typename TForceNoQual>
	inline auto
	collapseSingleEnd (const std::vector<fs::path>& samplesPaths,
	                   const fs::path& outputBasename) noexcept
	{
		std::vector<StatsCollapse>       ssStats;
		StatsCollapse                    msStats;
		cpputils::Chronometer<>          timer;
		std::vector<seqan::String<char>> samplesTags;
		SequenceWriter<TOutStream,
		               TSeqRecord>       outWriter;
		SequenceWriter<seqan::TagFileOut,
		               TSeqRecord>       tmpOutWriter;
		DetailsWriter<TTabRecord>        tabWriter;
		DetailsWriter<TTabRecord>        tmpTabWriter;
		Collapser                        ssCollapser;
		std::vector<std::pair<fs::path,
		                      fs::path>> collapsedSamples;

		// Start collapse time measurement.
		timer.start();

		// Create a tag name for every sample to be collapsed.
		for (const auto& s : samplesPaths)
		{
			samplesTags.emplace_back(s.stem().generic_string().data());
		}

		// Initialize output and tab writers.
		outWriter.configure(LibraryLayout::SINGLE_END,
		                    outputDirPath_,
		                    outputBasename,
		                    maxOutputRecords_);
		tmpOutWriter.configureTemporary(LibraryLayout::SINGLE_END,
		                                temporaryDirPath_);
		tabWriter.configure(outputDirPath_,
		                    outputBasename,
		                    samplesTags,
		                    maxOutputRecords_);
		tmpTabWriter.configureTemporary(temporaryDirPath_,
		                                samplesTags);

		// Collapse every sample found by the track procedure using a single
		// sample collapser.
		ssCollapser.configure(outputDirPath_,
		                      temporaryDirPath_,
		                      std::numeric_limits<uint64_t>::max(),
		                      maxRam_,
		                      trimLeft_,
		                      trimRight_,
		                      nThreads_);
		for (auto i = 0u; i < samplesPaths.size(); i++)
		{
			ResultsCollapse ssResults;
			auto& tag  = samplesTags[i];
			auto& path = samplesPaths[i];
			auto basename = fs::path(sa::toCString(samplesTags[i]));

			ssResults = ssCollapser.collapseSingleEnd<TSeqRecord,
			                                          sa::TagFileOut,
			                                          TForceNoQual>(samplesPaths[i],
			                                                        basename);
			collapsedSamples.emplace_back(ssResults.collapsedPaths[0]);
			ssStats.emplace_back(ssResults.statistics[0]);
		}

		// Once IO facilities are initialized, run the collapse procedure.
		collapseMS_<64,
		            TSeqRecord,
		            TTabRecord,
		            SequenceReader<sa::TagFileIn,
		                           TSeqRecord,
		                           TForceNoQual>,
		            DetailsReader<TTabRecord>>(collapsedSamples,
		                                       tmpOutWriter,
		                                       tmpTabWriter,
		                                       outWriter,
		                                       tabWriter,
		                                       msStats);
		msStats.sampleId = outputBasename.generic_string();
		for (const auto& p : samplesPaths)
		{
			msStats.rawSpace += fs::file_size(p);
		}
		for (const auto& s : ssStats)
		{
			msStats.rawSequences += s.rawSequences;
		}
		for (const auto& p : outWriter.getOutputPaths())
		{
			msStats.collapsedSpace += fs::file_size(p.first);
		}
		for (auto      & s : collapsedSamples)
		{
			remove(s.first.generic_string().data());
		}
		msStats.runtime = timer.stop();

		// Aggregate the multi-samples statistics to the single sample ones and
		// return all of them through the collapse result structure.
		ssStats.emplace_back(msStats);

		return ResultsCollapse(outWriter.getOutputPaths(),
		                       tabWriter.getOutputPaths(),
		                       {ssStats});
	}

    template <typename TSeqRecord,
            typename TTabRecord,
            typename TOutStream,
            typename TForceNoQual>
    inline auto
    collapsePairedEnd (const std::vector<fs::path>& samplesPaths,
                       const std::vector<fs::path>& pairedPaths,
                       const fs::path& outputBasename) noexcept
    {
        std::vector<StatsCollapse>       ssStats;
        StatsCollapse                    msStats;
        cpputils::Chronometer<>          timer;
        std::vector<seqan::String<char>> samplesTags;
        SequenceWriter<TOutStream,
                TSeqRecord>       outWriter;
        SequenceWriter<seqan::TagFileOut,
                TSeqRecord>       tmpOutWriter;
        DetailsWriter<TTabRecord>        tabWriter;
        DetailsWriter<TTabRecord>        tmpTabWriter;
        Collapser                        ssCollapser;
        std::vector<std::pair<fs::path,
                fs::path>> collapsedSamples;

        // Start collapse time measurement.
        timer.start();

        // Create a tag name for every sample to be collapsed.
        for (const auto& s : samplesPaths)
        {
            samplesTags.emplace_back(s.stem().generic_string().data());
        }

        // Initialize output and tab writers.
        outWriter.configure(LibraryLayout::PAIRED_END,
                            outputDirPath_,
                            outputBasename,
                            maxOutputRecords_);
        tmpOutWriter.configureTemporary(LibraryLayout::PAIRED_END,
                                        temporaryDirPath_);
        tabWriter.configure(outputDirPath_,
                            outputBasename,
                            samplesTags,
                            maxOutputRecords_);
        tmpTabWriter.configureTemporary(temporaryDirPath_,
                                        samplesTags);

        // Collapse every sample found by the track procedure using a single
        // sample collapser.
        ssCollapser.configure(outputDirPath_,
                              temporaryDirPath_,
                              std::numeric_limits<uint64_t>::max(),
                              maxRam_,
                              trimLeft_,
                              trimRight_,
                              nThreads_);
        for (auto i = 0u; i < samplesPaths.size(); i++)
        {
            ResultsCollapse ssResults;
            auto& tag  = samplesTags[i];
            auto& path = samplesPaths[i];
            auto basename = fs::path(sa::toCString(samplesTags[i]));

            ssResults = ssCollapser.collapsePairedEnd<TSeqRecord,
                    sa::TagFileOut,
                    TForceNoQual>(samplesPaths[i],
                                  pairedPaths[i],
                                  basename);
            collapsedSamples.emplace_back(ssResults.collapsedPaths[0]);
            ssStats.emplace_back(ssResults.statistics[0]);
        }

        // Once IO facilities are initialized, run the collapse procedure.
        collapseMS_<64,
                TSeqRecord,
                TTabRecord,
                SequenceReader<sa::TagFileIn,
                        TSeqRecord,
                        TForceNoQual>,
                DetailsReader<TTabRecord>>(collapsedSamples,
                                           tmpOutWriter,
                                           tmpTabWriter,
                                           outWriter,
                                           tabWriter,
                                           msStats);
        msStats.sampleId = outputBasename.generic_string();
        for (auto i = 0ul;
             i < samplesPaths.size();
             i++) {
            msStats.rawSpace += fs::file_size(samplesPaths[i]);
            msStats.rawSpace += fs::file_size(pairedPaths[i]);
        }
        for (const auto& s : ssStats)
        {
            msStats.rawSequences += s.rawSequences;
        }
        for (const auto& p : outWriter.getOutputPaths())
        {
            msStats.collapsedSpace += fs::file_size(p.first);
            msStats.collapsedSpace += fs::file_size(p.second);
        }
        for (auto      & s : collapsedSamples)
        {
            remove(s.first.generic_string().data());
            remove(s.second.generic_string().data());
        }
        msStats.runtime = timer.stop();

        // Aggregate the multi-samples statistics to the single sample ones and
        // return all of them through the collapse result structure.
        ssStats.emplace_back(msStats);

        return ResultsCollapse(outWriter.getOutputPaths(),
                               tabWriter.getOutputPaths(),
                               {ssStats});
    }

private:
	/**
	 * Path of the directory where output files are stored.
	 */
	fs::path outputDirPath_;
	/**
	 * Path of the directory where temporary batches are stored.
	 */
	fs::path temporaryDirPath_;
	/**
	 * Maximum number of records a single output file can contain.
	 */
	uint64_t maxOutputRecords_;
	/**
	 * Maximum amount of bytes the collapse procedure is allowed to use.
	 */
	uint64_t maxRam_;
	/**
	 * Number of bases trimmed from the left side of any read sequence.
	 */
	uint64_t trimLeft_;
	/**
	 * Number of bases trimmed from the right side of any read sequence.
	 */
	uint64_t trimRight_;
	/**
	 * Maximum number of working threads used by the collpse procedure.
	 */
	uint64_t nThreads_;

	template <typename TRecord,
	          typename TReader>
	inline auto
	getBytesSequenceRatio_ (TReader reader) noexcept
	{
		constexpr uint64_t testBatchSize = 100;

		uint64_t        memoryUtil    = 0;
		uint64_t        loadedRecords = 0;
		Buffer<TRecord> buffer;

		loadedRecords = buffer.load(reader,
		                            testBatchSize);
		memoryUtil    = buffer.memoryConsumption();

		return static_cast<uint64_t>(memoryUtil / loadedRecords);
	}

	template <typename TTabRecord>
	inline uint64_t
	getBytesTabRatio_ (uint64_t nSamples) noexcept
	{
		TTabRecord record;

		record.initialize(0,
		                  0,
		                  nSamples);

		return static_cast<uint64_t>(record.memoryConsumption());
	}

	template <typename TRecord,
	          typename TReader,
	          typename TTmpWriter,
	          typename TOutWriter>
	inline void
	makeTemporaryBatches_ (TReader& reader,
	                       TTmpWriter& tmpWriter,
	                       TOutWriter& outWriter,
	                       Buffer<TRecord>& buffer,
	                       uint64_t maxLoadable,
	                       StatsCollapse& stats) noexcept
	{
		auto loaded    = 0ul;
		auto collapsed = 0ul;

		// Load buffer, trim, sort and collapse the first batch of sequences.
		loaded = buffer.load(reader,
		                     maxLoadable);
		stats.rawSequences = loaded;
		if (trimLeft_ > 0 || trimRight_ > 0)
		{
			buffer.forEach([this] (auto& record)
			               {
				               record.trim(trimLeft_,
				                           trimRight_);
			               });
		}
		buffer.sort(nThreads_,
		            [] (const auto& left,
		                const auto& right)
		            {
			            return left.getSequence() < right.getSequence();
		            });
		collapsed = buffer.collapse(nThreads_,
		                            [] (const auto& left,
		                                const auto& right)
		                            {
			                            return left.getSequence() ==
			                                   right.getSequence();
		                            });

		// If the reader stream has no more data to read, forward the content
		// for the buffer directly to the output directory; otherwise, start
		// creating the temporary files.
		if (reader.atEnd())
		{
			buffer.storeIf(outWriter,
			               [] (const auto& record)
			               {
				               return record.getCount() > 0;
			               });
			outWriter.flush();
			stats.collapsedSequences = collapsed;
		}
		else
		{
			buffer.storeIf(tmpWriter,
			               [] (const auto& record)
			               {
				               return record.getCount() > 0;
			               });
			do
			{
				loaded = buffer.load(reader,
				                     maxLoadable);
				stats.rawSequences += loaded;
				if (trimLeft_ > 0 || trimRight_ > 0)
				{
					buffer.forEach([this] (auto& record)
					               {
						               record.trim(trimLeft_,
						                           trimRight_);
					               });
				}
				buffer.sort(nThreads_,
				            [] (const auto& left,
				                const auto& right)
				            {
					            return left.getSequence() < right.getSequence();
				            });
				collapsed = buffer.collapse(nThreads_,
				                            [] (const auto& left,
				                                const auto& right)
				                            {
					                            return left.getSequence() ==
					                                   right.getSequence();
				                            });
				if (loaded > 0)
				{
					tmpWriter.switchSink();
					buffer.storeIf(tmpWriter,
					               [] (const auto& record)
					               {
						               return record.getCount() > 0;
					               });
				}
			}
			while (!reader.atEnd());
			tmpWriter.flush();
		}
	}

	template <typename TReader>
	inline void
	configureBatchReaders_ (std::vector<TReader>& readers,
	                        std::vector<std::pair<fs::path,
	                                              fs::path>>& paths,
	                        uint64_t startIdx) noexcept
	{
		for (auto i = startIdx; i < startIdx + readers.size(); i++)
		{
			if (paths[startIdx].second.empty())
			{
				readers[i - startIdx].configureSingleEnd(paths[i].first);
			}
			else
			{
				readers[i - startIdx].configurePairedEnd(paths[i].first,
				                                         paths[i].second);
			}
		}
	}

	template <typename TReader>
	inline void
	configureTabReaders_ (std::vector<TReader>& readers,
	                      std::vector<fs::path>& paths,
	                      uint64_t startIdx) noexcept
	{
		for (auto i = startIdx; i < startIdx + readers.size(); i++)
		{
			readers[i - startIdx].configure(paths[i]);
		}
	}

	template <uint64_t N_WAY_MERGE,
			typename TRecord,
			typename TReader,
			typename TTmpWriter,
			typename TOutWriter>
	inline void
	mergeSSTmpBatches_ (TTmpWriter& tmpWriter,
	                    TOutWriter& outWriter,
	                    uint64_t maxLoadable,
	                    StatsCollapse& stats) noexcept
	{
		std::vector<TReader> readers;
		auto                 startIdx         = 0ul;
		auto                 writtenSequences = 0ul;

		readers.resize(N_WAY_MERGE);
		while (startIdx + N_WAY_MERGE < tmpWriter.getOutputPaths().size())
		{
			// Configure temporary readers, temporary writer and merge batches.
			configureBatchReaders_(readers,
			                       tmpWriter.getOutputPaths(),
			                       startIdx);

			// Switch sink on the temporary writer and perform the n-way merge
			// procedure.
			tmpWriter.switchSink();
			Buffer<TRecord>::merge(readers,
			                       tmpWriter,
			                       maxLoadable / N_WAY_MERGE);

			// Update start index.
			startIdx += N_WAY_MERGE;
		}

		// Perform last merge round.
		tmpWriter.flush();
		readers.resize(tmpWriter.getOutputPaths().size() - startIdx);
		configureBatchReaders_(readers,
		                       tmpWriter.getOutputPaths(),
		                       startIdx);
		writtenSequences = Buffer<TRecord>::merge(readers,
		                                          outWriter,
		                                          maxLoadable / readers.size());
		stats.collapsedSequences = writtenSequences;
	}

	template <typename TSeqRecord,
	          typename TOutStream,
	          typename TForceNoQual>
	inline void
	collapseSS_ (SequenceReader<sa::SeqFileIn,
	                            TSeqRecord,
	                            TForceNoQual>& reader,
	             SequenceWriter<sa::TagFileOut,
	                            TSeqRecord>& tmpWriter,
	             SequenceWriter<TOutStream,
	                            TSeqRecord>& outWriter,
	             StatsCollapse& stats) noexcept
	{
		constexpr auto RAMSafetyFactor = 1.65;

		auto               bytesPerSequence = 0ul;
		auto               loadable         = 0ul;
		Buffer<TSeqRecord> buffer;

		// Compute the maximum number of sequences loadable without exceeding
		// the memory limit the user imposed.
		bytesPerSequence = getBytesSequenceRatio_<TSeqRecord>(reader);
		loadable         = maxRam_ / (RAMSafetyFactor * bytesPerSequence);

		// Split input source into sorted temporary batches, then clear the
		// allocated buffer.
		makeTemporaryBatches_(reader,
		                      tmpWriter,
		                      outWriter,
		                      buffer,
		                      loadable,
		                      stats);
		buffer.reset();

		// Merge temporary batches, if at least two of them have been created.
		if (tmpWriter.getOutputPaths().size() > 1)
		{
			mergeSSTmpBatches_<16,
			                   TSeqRecord,
			                   SequenceReader<sa::TagFileIn,
			                                  TSeqRecord,
			                                  TForceNoQual >>(tmpWriter,
			                                                  outWriter,
			                                                  loadable,
			                                                  stats);

			// Remove temporary batch files.
			for (auto& p : tmpWriter.getOutputPaths())
			{
				// Remove temporary batches.
				remove(p.first.generic_string().data());
				if (!p.second.empty())
				{
					remove(p.second.generic_string().data());
				}
			}
		}
	}

	template <uint64_t N_WAY_MERGE,
			typename TSeqRecord,
			typename TTabRecord,
			typename TSeqReader,
			typename TTabReader,
			typename TTmpOutWriter,
			typename TTmpTabWriter,
			typename TOutWriter,
			typename TTabWriter>
	inline void
	collapseMS_ (std::vector<std::pair<fs::path,
	                                   fs::path>>& samples,
	             TTmpOutWriter& tmpOutWriter,
	             TTmpTabWriter& tmpTabWriter,
	             TOutWriter& outWriter,
	             TTabWriter& tabWriter,
	             StatsCollapse& stats) noexcept
	{
	    constexpr auto RAMSeqSafetyFactor = 2;
        constexpr auto RAMTabSafetyFactor = 3;

		std::vector<TSeqReader> seqReaders;
		std::vector<TTabReader> tabReaders;
		std::vector<uint64_t>   samplesIds;
		auto                    nSamples       = samples.size();
		auto                    bytesSeqRatio  = 0ul;
		auto                    bytesTabRatio  = 0ul;
		auto                    bytesDataRatio = 0ul;
		auto                    written        = 0ul;
		auto                    loadable       = 0ul;

		// Select the Bytes over records read ration, choosing the worst ratio
		// for every collapsed sample.
		for (auto i = 0u;
		     i < nSamples;
		     i++)
		{
			TSeqReader testSeqReader;
			auto       currentRatio = 0ul;

			// Configure the test reader.
			if (samples[i].second.empty())
			{
				testSeqReader.configureSingleEnd(samples[i].first);
			}
			else
			{
				testSeqReader.configurePairedEnd(samples[i].first,
				                                 samples[i].second);
			}

			// Perform the test.
			currentRatio = getBytesSequenceRatio_<TSeqRecord>(testSeqReader);
			if (currentRatio > bytesSeqRatio)
			{
				bytesSeqRatio = currentRatio;
			}
		}
		bytesSeqRatio *= RAMSeqSafetyFactor;

		// Compute the Bytes over tab record ratio, that is fixed for every
		// sample as it depends on the overall number of samples collapsed.
		bytesTabRatio = getBytesTabRatio_<TTabRecord>(nSamples);
		bytesTabRatio *= RAMTabSafetyFactor;

		// Start the merge procedure.
		if (nSamples <= N_WAY_MERGE)
		{
			bytesDataRatio = bytesSeqRatio * nSamples + bytesTabRatio;

			// Initialize readers and compute the maximum number of records
			// loadable.
			seqReaders.resize(samples.size());
			samplesIds.resize(samples.size());
			std::iota(samplesIds.begin(),
			          samplesIds.end(),
			          0);
			configureBatchReaders_(seqReaders,
			                       samples,
			                       0);
			loadable = maxRam_ / bytesDataRatio;

			// Merge the collapsed samples.
			written = Buffer<TSeqRecord>::template merge<TTabRecord>(seqReaders,
			                                                         outWriter,
			                                                         tabWriter,
			                                                         samplesIds,
			                                                         loadable,
			                                                         nSamples);
		}
		else
		{
			auto startIdx1        = 0ul;
			auto temporarySamples = 0ul;

			while (startIdx1 < nSamples)
			{
				bytesDataRatio = bytesSeqRatio * N_WAY_MERGE + bytesTabRatio;

				// Initialize readers and compute the maximum number of records
				// loadable.
				seqReaders.resize(std::min(N_WAY_MERGE,
				                           nSamples - startIdx1));
				samplesIds.resize(std::min(N_WAY_MERGE,
				                           nSamples - startIdx1));
				std::iota(samplesIds.begin(),
				          samplesIds.end(),
				          startIdx1);
				configureBatchReaders_(seqReaders,
				                       samples,
				                       startIdx1);
				loadable = maxRam_ / bytesDataRatio;

				// Merge the collapsed samples.
				Buffer<TSeqRecord>::template merge<TTabRecord>(seqReaders,
				                                               tmpOutWriter,
				                                               tmpTabWriter,
				                                               samplesIds,
				                                               loadable,
				                                               nSamples);
				tmpOutWriter.switchSink();
				tmpTabWriter.switchSink();
				startIdx1 += N_WAY_MERGE;
			}

			temporarySamples = tmpOutWriter.getOutputPaths().size();
			if (temporarySamples <= N_WAY_MERGE)
			{
				bytesDataRatio =
						(bytesSeqRatio + bytesTabRatio) * temporarySamples;

				// Initialize readers and compute the maximum number of records
				// loadable.
				seqReaders.resize(tmpOutWriter.getOutputPaths().size());
				tabReaders.resize(tmpTabWriter.getOutputPaths().size());
				configureBatchReaders_(seqReaders,
				                       tmpOutWriter.getOutputPaths(),
				                       0);
				configureTabReaders_(tabReaders,
				                     tmpTabWriter.getOutputPaths(),
				                     0);
				loadable = maxRam_ / bytesDataRatio;

				// Merge the collapsed samples.
				written = Buffer<TSeqRecord>::template merge<TTabRecord>(seqReaders,
				                                                         tabReaders,
				                                                         outWriter,
				                                                         tabWriter,
				                                                         loadable);
			}
			else
			{
				auto startIdx2 = 0ul;

				while (startIdx2 + N_WAY_MERGE <= temporarySamples)
				{
					bytesDataRatio =
							(bytesSeqRatio + bytesTabRatio) * N_WAY_MERGE;

					// Initialize readers and compute the maximum number of records
					// loadable.
					seqReaders.resize(N_WAY_MERGE);
					tabReaders.resize(N_WAY_MERGE);
					configureBatchReaders_(seqReaders,
					                       tmpOutWriter.getOutputPaths(),
					                       startIdx2);
					configureTabReaders_(tabReaders,
					                     tmpTabWriter.getOutputPaths(),
					                     startIdx2);
					loadable = maxRam_ / bytesDataRatio;

					// Merge the collapsed samples.
					Buffer<TSeqRecord>::template merge<TTabRecord>(seqReaders,
					                                               tabReaders,
					                                               tmpOutWriter,
					                                               tmpTabWriter,
					                                               loadable);
					tmpOutWriter.switchSink();
					tmpTabWriter.switchSink();
					temporarySamples++;
					startIdx2 += N_WAY_MERGE;
				}

				// Perform last merge raound operation.
				bytesDataRatio = (bytesSeqRatio + bytesTabRatio) *
				                 (temporarySamples - startIdx2);
				seqReaders.resize(temporarySamples - startIdx2);
				tabReaders.resize(temporarySamples - startIdx2);
				configureBatchReaders_(seqReaders,
				                       tmpOutWriter.getOutputPaths(),
				                       startIdx2);
				configureTabReaders_(tabReaders,
				                     tmpTabWriter.getOutputPaths(),
				                     startIdx2);
				loadable = maxRam_ / bytesDataRatio;
				written  = Buffer<TSeqRecord>::template merge<TTabRecord>(seqReaders,
				                                                          tabReaders,
				                                                          outWriter,
				                                                          tabWriter,
				                                                          loadable);
			}
		}
		stats.collapsedSequences = written;
	}
};

} // bioseqzip

#endif // BIOSEQZIP_INCLUDE_BIOSEQZIP_COLLAPSER_H
