/**
 * \file      include/bioseqzip/expander.h
 * \author    Gianvito Urgese
 * \author    Emanuele Parisi
 * \author    Orazio Scicolone
 * \date      January, 2019
 *
 * \copyright Copyright (C) 2019 Gianvito Urgese. All Rights Reserved.
 */

#ifndef BIOSEQZIP_BIOSEQZIP_EXPANDER_H_
#define BIOSEQZIP_BIOSEQZIP_EXPANDER_H_

#include <seqan/bam_io.h>

#if BioSeqZip_APPLE
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

namespace sa = seqan;

namespace bioseqzip
{

class Expander
{
public:
	inline void
	configure (const fs::path& outputDirPath,
	           const fs::path& temporaryDirPath,
	           uint64_t maxRam,
	           uint64_t nThreads) noexcept
	{
		outputDirPath_    = outputDirPath;
		temporaryDirPath_ = temporaryDirPath;
		maxRam_           = maxRam;
		nThreads_         = nThreads;
	}

	inline fs::path
	expandSingleSample (const fs::path& bamInputPath,
	                    const fs::path& outputBasename) noexcept
	{
		uint64_t           maxLoadable = 0;
		fs::path           sortedBamPath;
		sa::BamFileIn      bamInputFile;
		sa::BamIOContext<> bamInputContext;
		sa::BamHeader      bamInputHeader;
		fs::path           bamExpandedPath;
		sa::BamFileOut     bamExpandedFile;

		// Evaluate the maximum number of BAM alignments records loadable
		// without exceeding the memory constraints set by the caller.
		maxLoadable = computeMaxLoadable_<100>(bamInputPath,
		                                       1.2);

		// Open the BAM input file and retrieve its header and its context.
		sa::open(bamInputFile,
		         bamInputPath.generic_string().data());
		bamInputContext = sa::context(bamInputFile);
		sa::readHeader(bamInputHeader,
		               bamInputFile);

		// Open the expanded BAM file and initialize its context with the
		// context got from the BAM input file. Then, write the header and set
		// the context.
		bamExpandedPath = outputDirPath_ / outputBasename;
		bamExpandedPath += bamInputPath.extension();
		sa::open(bamExpandedFile,
		         bamExpandedPath.generic_string().data());
		sa::context(bamExpandedFile) = bamInputContext;
		sa::writeHeader(bamExpandedFile,
		                bamInputHeader);

		// Start the single sample expand procedure.
		expandSingleSampleCore_(bamInputFile,
		                        bamExpandedFile,
		                        maxLoadable);

		// Close all the streams.
		sa::close(bamInputFile);
		sa::close(bamExpandedFile);

		return bamExpandedPath;
	}

//	template <typename TInStream,
//	          typename TTagRecord,
//	          typename TOccRecord>
//	inline std::vector<fs::path>
//	expandMultiSamples (const fs::path& bamInputPath,
//	                    const fs::path& collapsedInputPath,
//	                    const fs::path& tabInputPath) noexcept
//	{
//		fs::path emptyPairedPath = fs::path("");
//
//		return expandMultiSamples<TInStream,
//		                          TTagRecord,
//		                          TOccRecord>(bamInputPath,
//		                                      collapsedInputPath,
//		                                      emptyPairedPath,
//		                                      tabInputPath);
//	}
//
//	template <typename TInStream,
//	          typename TTagRecord,
//	          typename TOccRecord>
//	inline std::vector<fs::path>
//	expandMultiSamples (const fs::path& bamInputPath,
//	                    const fs::path& collapsedInputPath,
//	                    const fs::path& collapsedPairedPath,
//	                    const fs::path& tabInputPath) noexcept
//	{
//		uint64_t              alignmentsMaxLoadable = 0;
//		uint64_t              expandMaxLoadable     = 0;
//		fs::path              bamSortedPath;
//		uint64_t              startIdx              = 0;
//		uint64_t              stopIdx               = 0;
//		bool                  expandLoopFinished    = false;
//		std::vector<fs::path> bamExpandedPaths;
//
//		// Evaluate the maximum number of records loadable during the BAM
//		// input file sort procedure and during the multiple samples expand
//		// procedure.
//		computeMaxLoadable_<100,
//		                    TTagRecord,
//		                    TOccRecord,
//		                    TInStream>(bamInputPath,
//		                               std::make_pair(collapsedInputPath,
//		                                              collapsedPairedPath),
//		                               tabInputPath,
//		                               5.0,
//		                               alignmentsMaxLoadable,
//		                               expandMaxLoadable);
//
//		// Sort the input file containing alignment records.
//		bamSortedPath = sortInputBAMFile_(bamInputPath,
//		                                  alignmentsMaxLoadable);
//
//		// Start the sorted input BAM expansion loop.
//		do
//		{
//			constexpr uint64_t                  N_WAY_EXPAND = 8;
//			sa::BamFileIn                       bamSortedFile;
//			sa::BamIOContext<>                  bamSortedContext;
//			sa::BamHeader                       bamSortedHeader;
//			core::RecordReader<TInStream,
//			                   TTagRecord,
//			                   std::false_type> tagReader;
//			core::OccurrencesReader<TOccRecord> occReader;
//			std::array<sa::BamFileOut,
//			           N_WAY_EXPAND>            bamExpandedFiles;
//
//			// Open the input BAM sorted file and retrieve its header and its
//			// context.
//			sa::open(bamSortedFile,
//			         bamSortedPath.generic_string().data());
//			bamSortedContext = sa::context(bamSortedFile);
//			sa::readHeader(bamSortedHeader,
//			               bamSortedFile);
//			sa::close(bamSortedFile);
//
//			// Initialize the tag and occurrences record readers used for
//			// loading data from the source files.
//			tagReader.configureSingleEnd(collapsedInputPath);
//			occReader.configure(tabInputPath);
//
//			// Update the index of the last file to be expanded.
//			stopIdx = std::min(startIdx + N_WAY_EXPAND,
//			                   occReader.getHeader().size());
//
//			// Open a number of BAM output streams corresponding to the files
//			// that will be expanded in the current iteration.
//			for (auto i = 0u; i < stopIdx - startIdx; i++)
//			{
//				sa::String<char> basename = occReader.getHeader()[startIdx + i];
//				fs::path         outPath;
//
//				// Create the path for the current expanded output BAM file.
//				outPath = outputDirPath_ / sa::toCString(basename);
//				outPath += bamInputPath.extension();
//				bamExpandedPaths.emplace_back(outPath);
//
//				// Open the output BAM stream corresponding to the currently
//				// created path.
//				sa::open(bamExpandedFiles[i],
//				         outPath.generic_string().data());
//				sa::context(bamExpandedFiles[i]) = bamSortedContext;
//				sa::writeHeader(bamExpandedFiles[i],
//				                bamSortedHeader);
//			}
//
//			// Start the N-way expand procedure.
//			expandMultiSamplesCore_(bamSortedFile,
//			                        tagReader,
//			                        occReader,
//			                        bamExpandedFiles,
//			                        startIdx,
//			                        stopIdx - startIdx,
//			                        expandMaxLoadable);
//
//			// Close the input file opened.
//			for (auto i = 0u; i < stopIdx - startIdx; i++)
//			{
//				sa::close(bamExpandedFiles[i]);
//			}
//
//			// Update the index of the first file to be expanded.
//			startIdx = stopIdx;
//			if (stopIdx == occReader.getHeader().size())
//			{
//				expandLoopFinished = true;
//			}
//		}
//		while (!expandLoopFinished);
//
//		return bamExpandedPaths;
//	}

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
	 * Maximum amount of bytes the collapse procedure is allowed to use.
	 */
	uint64_t maxRam_;
	/**
	 * Maximum number of working threads used by the collpse procedure.
	 */
	uint64_t nThreads_;

	template <uint64_t BATCH_SIZE>
	inline uint64_t
	computeMaxLoadable_ (const fs::path& bamInputPath,
	                     double memorySafeFactor) noexcept
	{
		sa::BamFileIn                       bamInputFile;
		sa::BamHeader                       bamInputHeader;
		std::vector<sa::BamAlignmentRecord> buffer;
		uint64_t                            bufferSize      = 0;
		uint64_t                            loaded          = 0;
		double                              ramPerBamRecord = 0;

		// Initialize the alignment record buffer source. Then, read the BAM
		// header.
		sa::open(bamInputFile,
		         bamInputPath.generic_string().data());
		sa::readHeader(bamInputHeader,
		               bamInputFile);

		// Load a batch of records from the input BAM file and evaluate its
		// memory consumption.
		buffer.resize(BATCH_SIZE);
		for (loaded = 0;
		     loaded < BATCH_SIZE && !sa::atEnd(bamInputFile);
		     loaded++)
		{
			sa::readRecord(buffer[loaded],
			               bamInputFile);
		}
		bufferSize += sizeof(std::vector<sa::BamAlignmentRecord>);
		for (auto i = 0u;
		     i < loaded;
		     i++)
		{
			uint64_t size = 0;

			size += sizeof(sa::BamAlignmentRecord);
			size += sizeof(sa::CigarElement<>) * sa::capacity(buffer[i].cigar);
			size += sizeof(buffer[i].qName);
			size += sizeof(char) * sa::capacity(buffer[i].qName);
			size += sizeof(buffer[i].seq);
			size += sizeof(char) * sa::capacity(buffer[i].seq);
			size += sizeof(buffer[i].qual);
			size += sizeof(char) * sa::capacity(buffer[i].qual);
			size += sizeof(buffer[i].tags);
			size += sizeof(char) * sa::capacity(buffer[i].tags);
			size += sizeof(buffer[i]._buffer);
			size += sizeof(char) * sa::capacity(buffer[i]._buffer);
			bufferSize += size;
		}

		// Estimate the maximum loadable sequences.
		ramPerBamRecord = memorySafeFactor * bufferSize / loaded;
		return static_cast<uint64_t>(maxRam_ / ramPerBamRecord);
	}

//	template <uint64_t BATCH_SIZE,
//			typename TTagRecord,
//			typename TOccRecord,
//			typename TInStream>
//	inline void
//	computeMaxLoadable_ (const fs::path& bamInputPath,
//	                     const std::pair<fs::path,
//	                                     fs::path>& collapsedPaths,
//	                     const fs::path& tabInputPath,
//	                     double memorySafeFactor,
//	                     uint64_t& sortMaxLoadable,
//	                     uint64_t& expandMaxLoadable) noexcept
//	{
//		sa::BamFileIn                       bamInputFile;
//		core::RecordReader<TInStream,
//		                   TTagRecord,
//		                   std::false_type> tagReader;
//		core::OccurrencesReader<TOccRecord> occReader;
//		sa::BamHeader                       bamInputHeader;
//		sa::String<sa::BamAlignmentRecord>  bamBuffer;
//		std::vector<TTagRecord>             tagBuffer;
//		std::vector<TOccRecord>             occBuffer;
//		uint64_t                            alignmentsLoaded = 0;
//		uint64_t                            tagsLoaded       = 0;
//		uint64_t                            bamBufferSize    = 0;
//		uint64_t                            totalBufferSize  = 0;
//		uint64_t                            loaded           = 0;
//		double                              ramPerBamRecord  = 0;
//		double                              ramPerRecord     = 0;
//
//		// Initialize the data sources. Then, resize the tags and
//		// occurrences buffers.
//		sa::open(bamInputFile,
//		         bamInputPath.generic_string().data());
//		sa::readHeader(bamInputHeader,
//		               bamInputFile);
//		if (collapsedPaths.second.empty())
//		{
//			tagReader.configureSingleEnd(collapsedPaths.first);
//		}
//		else
//		{
//			tagReader.configurePairedEnd(collapsedPaths.first,
//			                             collapsedPaths.second);
//		}
//		occReader.configure(tabInputPath);
//		tagBuffer.resize(BATCH_SIZE);
//		occBuffer.resize(BATCH_SIZE);
//
//		// Load a batch alignments, collapsed and occurrences records from the
//		// data sources. Then, consider the number of loaded sequences as the
//		// maximum between the three buffers sizes. Notices, that the number of
//		// collapsed sequences loaded must be equal to the number of
//		// occurrences records loaded.
//		alignmentsLoaded = sa::readRecords(bamBuffer,
//		                                   bamInputFile,
//		                                   BATCH_SIZE);
//		tagsLoaded       = tagReader.read(tagBuffer.begin(),
//		                                  tagBuffer.end());
//		tagsLoaded       = occReader.read(occBuffer.begin(),
//		                                  occBuffer.end());
//		loaded           = std::max(alignmentsLoaded,
//		                            tagsLoaded);
//
//		// Compute the amount of memory required for storing data from the two
//		// buffers.
//		bamBufferSize += sizeof(sa::String<sa::BamAlignmentRecord>);
//		totalBufferSize += sizeof(sa::String<sa::BamAlignmentRecord>);
//		for (auto i = 0u; i < loaded; i++)
//		{
//			uint64_t size = 0;
//
//			size += sizeof(sa::BamAlignmentRecord);
//			size += sizeof(sa::CigarElement<>) *
//			        sa::capacity(bamBuffer[i].cigar);
//			size += sizeof(bamBuffer[i].qName);
//			size += sizeof(char) * sa::capacity(bamBuffer[i].qName);
//			size += sizeof(bamBuffer[i].seq);
//			size += sizeof(char) * sa::capacity(bamBuffer[i].seq);
//			size += sizeof(bamBuffer[i].qual);
//			size += sizeof(char) * sa::capacity(bamBuffer[i].qual);
//			size += sizeof(bamBuffer[i].tags);
//			size += sizeof(char) * sa::capacity(bamBuffer[i].tags);
//			size += sizeof(bamBuffer[i]._buffer);
//			size += sizeof(char) * sa::capacity(bamBuffer[i]._buffer);
//			bamBufferSize += size;
//			totalBufferSize += size;
//		}
//		totalBufferSize += sizeof(std::vector<TTagRecord>);
//		totalBufferSize += sizeof(std::vector<TOccRecord>);
//		for (auto i = 0u; i < loaded; i++)
//		{
//			totalBufferSize += tagBuffer[i].memoryConsumption();
//			totalBufferSize += occBuffer[i].memoryConsumption();
//		}
//
//		// Estimate the maximum loadable sequences.
//		ramPerBamRecord   = memorySafeFactor * bamBufferSize / loaded;
//		sortMaxLoadable   = static_cast<uint64_t>(maxRam_ / ramPerBamRecord);
//		ramPerRecord      = memorySafeFactor * totalBufferSize / loaded;
//		expandMaxLoadable = static_cast<uint64_t>(maxRam_ / ramPerRecord);
//	}
//
//	inline fs::path
//	makeRandomTemporaryPath_ (const fs::path& outputPathExt) noexcept
//	{
//		using SysClock = std::chrono::system_clock;
//		using SysClockDur = SysClock::duration;
//
//		SysClockDur sinceEpoch = SysClock::now().time_since_epoch();
//		std::string rnd        = std::to_string(sinceEpoch.count());
//		fs::path    randomPath = temporaryDirPath_ / fs::path(rnd);
//
//		randomPath += outputPathExt;
//
//		return randomPath;
//	}
//
//	inline void
//	splitInputBAMFile_ (const fs::path& bamInputPath,
//	                    std::vector<fs::path>& sortedBatchPaths,
//	                    uint64_t maxLoadable) noexcept
//	{
//		sa::BamFileIn                      bamInputFile;
//		sa::BamHeader                      bamInputHeader;
//		sa::BamIOContext<>                 bamInputContext;
//		sa::String<sa::BamAlignmentRecord> buffer;
//
//		// Open the input BAM file and store the header, ready to be copied in
//		// every temporary BAM file created and copy its IOContext.
//		sa::open(bamInputFile,
//		         bamInputPath.generic_string().data());
//		sa::readHeader(bamInputHeader,
//		               bamInputFile);
//		bamInputContext = sa::context(bamInputFile);
//
//		// Split the input BAM file into a set of sorted BAM batch files,
//		// sorted by aligned sequence.
//		while (!sa::atEnd(bamInputFile))
//		{
//			fs::path       tmpBatchPath;
//			sa::BamFileOut tmpBatchFile;
//			uint64_t       loaded = 0;
//
//			// Load the buffer from the input BAM file and sort it.
//			loaded = sa::readRecords(buffer,
//			                         bamInputFile,
//			                         maxLoadable);
//			sa::resize(buffer,
//			           loaded);
//			core::sortBuffer(sa::begin(buffer),
//			                 sa::begin(buffer) + loaded,
//			                 [] (const sa::BamAlignmentRecord& left,
//			                     const sa::BamAlignmentRecord& right)
//			                 {
//				                 return left.seq < right.seq;
//			                 },
//			                 nThreads_);
//
//			// Create a new random file and write both the input BAM header and
//			// the content of the sorted buffer.
//			tmpBatchPath = makeRandomTemporaryPath_(bamInputPath.extension());
//			sa::open(tmpBatchFile,
//			         tmpBatchPath.generic_string().data());
//			sa::context(tmpBatchFile) = bamInputContext;
//			sa::writeHeader(tmpBatchFile,
//			                bamInputHeader);
//			sa::writeRecords(tmpBatchFile,
//			                 buffer);
//
//			// Close the temporary file and store its path.
//			sa::close(tmpBatchFile);
//			sortedBatchPaths.emplace_back(tmpBatchPath);
//		}
//
//		// Close the input BAM file.
//		sa::close(bamInputFile);
//	}
//
//	template <uint64_t N_WAY_MERGE>
//	inline uint64_t
//	findMinimumTag_ (const std::array<sa::String<sa::BamAlignmentRecord>,
//	                                  N_WAY_MERGE>& buffers,
//	                 const std::array<uint64_t,
//	                                  N_WAY_MERGE>& indices,
//	                 const std::array<bool,
//	                                  N_WAY_MERGE>& validityFlags) noexcept
//	{
//		uint64_t minIndex = 0;
//
//		// Assume that at least one valid buffer is always present.
//		while (!validityFlags[minIndex])
//		{
//			minIndex++;
//		}
//
//		// Look for the index of the buffer whose top alignment record has the
//		// the minimum tag sequence.
//		for (auto i = minIndex + 1; i < N_WAY_MERGE; i++)
//		{
//			if (validityFlags[i] &&
//			    buffers[i][indices[i]].seq <
//			    buffers[minIndex][indices[minIndex]].seq)
//			{
//				minIndex = i;
//			}
//		}
//
//		return minIndex;
//	}
//
//	template <uint64_t N_WAY_MERGE>
//	inline uint64_t
//	reloadBuffer_ (std::array<sa::String<sa::BamAlignmentRecord>,
//	                          N_WAY_MERGE>& buffers,
//	               std::array<uint64_t,
//	                          N_WAY_MERGE>& indices,
//	               std::array<uint64_t,
//	                          N_WAY_MERGE>& limits,
//	               std::array<sa::BamFileIn,
//	                          N_WAY_MERGE>& inBatchFiles,
//	               std::array<bool,
//	                          N_WAY_MERGE>& validityFlags,
//	               uint64_t minIndex,
//	               uint64_t nValidBatches) noexcept
//	{
//		indices[minIndex] = 0;
//		if (sa::atEnd(inBatchFiles[minIndex]))
//		{
//			limits[minIndex]        = 0;
//			validityFlags[minIndex] = false;
//			nValidBatches--;
//		}
//		else
//		{
//			limits[minIndex] = sa::readRecords(buffers[minIndex],
//			                                   inBatchFiles[minIndex],
//			                                   limits[minIndex]);
//		}
//
//		return nValidBatches;
//	}
//
//	template <uint64_t N_WAY_MERGE>
//	inline void
//	nWayMergeProcedure_ (std::array<sa::BamFileIn,
//	                                N_WAY_MERGE>& inBatchFiles,
//	                     sa::BamFileOut& outFile,
//	                     uint64_t maxLoadable,
//	                     uint64_t nBatches) noexcept
//	{
//		std::array<sa::String<sa::BamAlignmentRecord>,
//		           N_WAY_MERGE>            inBuffers;
//		std::array<bool,
//		           N_WAY_MERGE>            validityFlags;
//		std::array<uint64_t,
//		           N_WAY_MERGE>            inIndices;
//		std::array<uint64_t,
//		           N_WAY_MERGE>            inLimits;
//		sa::String<sa::BamAlignmentRecord> outBuffer;
//		uint64_t                           outIndex = 0;
//		uint64_t                           loadable = maxLoadable / nBatches;
//		uint64_t                           minIndex = 0;
//
//		// Initialize data structures and pre-load buffers associated with a
//		// valid BAM input stream reader.
//		outIndex = 0;
//		sa::resize(outBuffer,
//		           loadable);
//		for (auto i = 0u; i < N_WAY_MERGE; i++)
//		{
//			inIndices[i] = 0;
//			if (i < nBatches)
//			{
//				validityFlags[i] = true;
//				inLimits[i]      = sa::readRecords(inBuffers[i],
//				                                   inBatchFiles[i],
//				                                   loadable);
//			}
//			else
//			{
//				validityFlags[i] = false;
//				inLimits[i]      = 0;
//			}
//		}
//
//		// Main merge loop.
//		while (nBatches > 0)
//		{
//			// Find index of the buffer containing the minimum tag value.
//			minIndex = findMinimumTag_<N_WAY_MERGE>(inBuffers,
//			                                        inIndices,
//			                                        validityFlags);
//
//			// Check if accumulate or write.
//			sa::move(outBuffer[outIndex],
//			         inBuffers[minIndex][inIndices[minIndex]]);
//			outIndex++;
//			if (outIndex == loadable)
//			{
//				sa::writeRecords(outFile,
//				                 outBuffer);
//				outIndex = 0;
//			}
//
//			// Check if reload buffer and update remained buffers.
//			inIndices[minIndex]++;
//			if (inIndices[minIndex] == inLimits[minIndex])
//			{
//				nBatches = reloadBuffer_<N_WAY_MERGE>(inBuffers,
//				                                      inIndices,
//				                                      inLimits,
//				                                      inBatchFiles,
//				                                      validityFlags,
//				                                      minIndex,
//				                                      nBatches);
//			}
//		}
//		sa::resize(outBuffer,
//		           outIndex);
//		sa::writeRecords(outFile,
//		                 outBuffer);
//	}
//
//	template <uint64_t N_WAY_MERGE>
//	inline fs::path
//	mergeSortedBatches_ (const fs::path& bamInputPath,
//	                     std::vector<fs::path>& sortedBatchPaths,
//	                     uint64_t maxLoadable) noexcept
//	{
//		sa::BamFileIn           bamInputFile;
//		sa::BamHeader           bamInputHeader;
//		sa::BamIOContext<>      bamInputContext;
//		fs::path                bamSortedPath;
//		sa::BamFileOut          bamSortedFile;
//		std::array<sa::BamFileIn,
//		           N_WAY_MERGE> tmpBatchFiles;
//		sa::BamHeader           tmpHeader;
//		uint64_t                startIdx = 0;
//		uint64_t                stopIdx  = std::min(N_WAY_MERGE,
//		                                            sortedBatchPaths.size());
//
//		// Load the header of the input BAM file, along with its context.
//		sa::open(bamInputFile,
//		         bamInputPath.generic_string().data());
//		sa::readHeader(bamInputHeader,
//		               bamInputFile);
//		bamInputContext = sa::context(bamInputFile);
//		sa::close(bamInputFile);
//
//		// Merge at most N_WAY_MERGE sorted batches at a time, until all
//		// batches are processed.
//		while (stopIdx == startIdx + N_WAY_MERGE)
//		{
//			fs::path       nextBatchPath;
//			sa::BamFileOut nextBatchFile;
//
//			// Initialize an array of input BAM streams ready for loading data
//			// from the batch files.
//			for (auto i = startIdx; i < stopIdx; i++)
//			{
//				sa::open(tmpBatchFiles[i - startIdx],
//				         sortedBatchPaths[i].generic_string().data());
//				sa::readHeader(tmpHeader,
//				               tmpBatchFiles[i - startIdx]);
//			}
//
//			// Open the BAM file where the partial outcome of the merge
//			// procedure will be stored.
//			nextBatchPath = makeRandomTemporaryPath_(bamInputPath.extension());
//			sa::open(nextBatchFile,
//			         nextBatchPath.generic_string().data());
//			sa::context(nextBatchFile) = bamInputContext;
//			sa::writeHeader(nextBatchFile,
//			                bamInputHeader);
//			nWayMergeProcedure_(tmpBatchFiles,
//			                    nextBatchFile,
//			                    maxLoadable,
//			                    stopIdx - startIdx);
//			sa::close(nextBatchFile);
//			sortedBatchPaths.emplace_back(nextBatchPath);
//
//			// Update the two indexes used for tracking the batch paths to be
//			// merged.
//			startIdx = stopIdx;
//			stopIdx  = std::min(startIdx + N_WAY_MERGE,
//			                    sortedBatchPaths.size());
//		}
//		for (auto i = startIdx; i < stopIdx; i++)
//		{
//			sa::open(tmpBatchFiles[i - startIdx],
//			         sortedBatchPaths[i].generic_string().data());
//			sa::readHeader(tmpHeader,
//			               tmpBatchFiles[i - startIdx]);
//		}
//		bamSortedPath = makeRandomTemporaryPath_(bamInputPath.extension());
//		sa::open(bamSortedFile,
//		         bamSortedPath.generic_string().data());
//		sa::context(bamSortedFile) = bamInputContext;
//		sa::writeHeader(bamSortedFile,
//		                bamInputHeader);
//		nWayMergeProcedure_(tmpBatchFiles,
//		                    bamSortedFile,
//		                    maxLoadable,
//		                    stopIdx - startIdx);
//
//		// Close the temporary file containing the sorted version of the input
//		// file provided by the user.
//		sa::close(bamSortedFile);
//
//		// Remove all the sorted temporary batches created during the sort
//		// procedure.
//		for (auto& p : sortedBatchPaths)
//		{
//			remove(p.generic_string().data());
//		}
//
//		return bamSortedPath;
//	}
//
//	inline fs::path
//	sortInputBAMFile_ (const fs::path& bamInputPath,
//	                   uint64_t maxLoadable) noexcept
//	{
//		fs::path              bamSortedPath;
//		std::vector<fs::path> sortedBatchPaths;
//
//		// Split the input BAM file into a set of sorted batch files, according
//		// to the memory threshold the user provided.
//		splitInputBAMFile_(bamInputPath,
//		                   sortedBatchPaths,
//		                   maxLoadable);
//
//		// If the input was small enough that a single batch was created, then
//		// that is the sorted version of the input BAM file. Otherwise, the set
//		// of batches have to be merged into a single sorted BAM file.
//		if (sortedBatchPaths.size() == 1)
//		{
//			bamSortedPath = std::move(sortedBatchPaths[0]);
//		}
//		else
//		{
//			bamSortedPath = mergeSortedBatches_<6>(bamInputPath,
//			                                       sortedBatchPaths,
//			                                       maxLoadable);
//		}
//
//		return bamSortedPath;
//	}
//
	inline uint64_t
	getTagCount_ (sa::String<char>& tagName) noexcept
	{
		std::string stdTagName = sa::toCString(tagName);
		uint64_t    startIndex = stdTagName.find_last_of(':') + 1;
		uint64_t    stopIndex  = stdTagName.find_first_of('\0');
		std::string strCount   = stdTagName.substr(startIndex,
		                                           stopIndex - startIndex);

		return static_cast<uint64_t>(std::stol(strCount));
	}

	inline void
	expandSingleSampleCore_ (sa::BamFileIn& bamInputFile,
	                         sa::BamFileOut& bamExpandedFile,
	                         uint64_t maxLoadable) noexcept
	{
		std::vector<sa::BamAlignmentRecord> bamBuffer;
		uint64_t                            bamBufferSize = 0;
		std::vector<uint64_t>               outBuffer;

		// Resize the buffers that keep track of alignments record and how many
		// time each record has to be expanded.
		bamBuffer.resize(maxLoadable);
		outBuffer.resize(maxLoadable,
		                 0);

		// Start the expand loop.
		do
		{
			// Load a batch of records from the input BAM file.
			for (bamBufferSize = 0;
			     bamBufferSize < maxLoadable && !sa::atEnd(bamInputFile);
			     bamBufferSize++)
			{
				sa::readRecord(bamBuffer[bamBufferSize],
				               bamInputFile);
			}

			// Extract the tag count from the header of the BAM alignment
			// record. Notice that here is assumed that the header reported in
			// the BAM file is identical to the one provided by
			// bioseqzip-collapser.
			SEQAN_OMP_PRAGMA(parallel for num_threads(nThreads_))
			for (auto i = 0;
			     i < bamBufferSize;
			     i++)
			{
				outBuffer[i] = getTagCount_(bamBuffer[i].qName);
			}

			// Write the expanded records to the output BAM file.
			for (auto i = 0u;
			     i < bamBufferSize;
			     i++)
			{
				for (auto j = 0u;
				     j < outBuffer[i];
				     j++)
				{
					sa::writeRecord(bamExpandedFile,
					                bamBuffer[i]);
				}
			}
		}
		while (!sa::atEnd(bamInputFile));
	}

//	template <typename TTagRecord>
//	inline bool
//	hasSameTag_ (const sa::BamAlignmentRecord& bamRecord,
//	             const TTagRecord& tagRecord) noexcept
//	{
//		auto tagBam = sa::convert<sa::String<char>>(bamRecord.seq);
//		auto tagTag = sa::convert<sa::String<char>>(tagRecord.getTag());
//
//		if (tagRecord.getBpOffset() == 0)
//		{
//			return tagBam == tagTag;
//		}
//		return tagBam == sa::prefix(tagTag,
//		                            tagRecord.getBpOffset()) ||
//		       tagBam == sa::suffix(tagTag,
//		                            tagRecord.getBpOffset());
//	}
//
//	template <uint64_t N_WAY_EXPAND,
//			typename TInStream,
//			typename TTagRecord,
//			typename TOccRecord>
//	inline void
//	expandMultiSamplesCore_ (sa::BamFileIn& bamSortedFile,
//	                         core::RecordReader<TInStream,
//	                                            TTagRecord,
//	                                            std::false_type>& tagReader,
//	                         core::OccurrencesReader<TOccRecord>& occReader,
//	                         std::array<sa::BamFileOut,
//	                                    N_WAY_EXPAND>& bamExpandedFiles,
//	                         uint64_t firstToExpand,
//	                         uint64_t nExpandedFiles,
//	                         uint64_t maxLoadable) noexcept
//	{
//		sa::String<sa::BamAlignmentRecord> bamBuffer;
//		uint64_t                           bamBufferSize  = 0;
//		uint64_t                           bamBufferIndex = 0;
//		std::vector<TTagRecord>            tagBuffer;
//		uint64_t                           tagBufferSize  = 0;
//		uint64_t                           tagBufferIndex = 0;
//		std::vector<TOccRecord>            occBuffer;
//
//		tagBufferIndex = 0;
//		tagBuffer.resize(maxLoadable);
//		tagReader.read(tagBuffer.begin(),
//		               tagBuffer.end());
//		occBuffer.resize(maxLoadable);
//		tagBufferSize = occReader.read(occBuffer.begin(),
//		                               occBuffer.end());
//		do
//		{
//			bamBufferIndex = 0;
//			bamBufferSize  = sa::readRecords(bamBuffer,
//			                                 bamSortedFile,
//			                                 maxLoadable);
//			while (bamBufferIndex != bamBufferSize)
//			{
//				sa::BamAlignmentRecord& bamRecord = bamBuffer[bamBufferIndex];
//				TTagRecord            & tagRecord = tagBuffer[tagBufferIndex];
//
//				if (hasSameTag_(bamRecord,
//				                tagRecord))
//				{
//					TOccRecord& occRecord = occBuffer[tagBufferIndex];
//
//					for (auto i = 0u; i < nExpandedFiles; i++)
//					{
//						auto& occurrences = occRecord.getOccurrences();
//						uint64_t count = occurrences[firstToExpand + i];
//
//						for (auto j = 0u; j < count; j++)
//						{
//							sa::writeRecord(bamExpandedFiles[i],
//							                bamRecord);
//						}
//					}
//					bamBufferIndex++;
//				}
//				else
//				{
//					tagBufferIndex++;
//					if (tagBufferIndex == tagBufferSize)
//					{
//						tagBufferIndex = 0;
//						tagReader.read(tagBuffer.begin(),
//						               tagBuffer.end());
//						tagBufferSize = occReader.read(occBuffer.begin(),
//						                               occBuffer.end());
//					}
//				}
//			}
//		}
//		while (!sa::atEnd(bamSortedFile));
//	}
};

} // bioseqzip

#endif // BIOSEQZIP_BIOSEQZIP_EXPANDER_H_
