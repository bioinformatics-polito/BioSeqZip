/**
 * \file   bioseqzip/buffer.h
 * \author Gianvito Urgese
 * \author Emanuele Parisi
 * \date   February, 2019
 */

#ifndef BIOSEQZIP_INCLUDE_BIOSEQZIP_BUFFER_H
#define BIOSEQZIP_INCLUDE_BIOSEQZIP_BUFFER_H

namespace bioseqzip
{

template <typename TRecord>
class Buffer
{
public:

	template <typename TReader,
	          typename TWriter>
	static inline auto
	merge (std::vector<TReader>& readers,
	       TWriter& writer,
	       uint64_t maxLoadable) noexcept
	{
		std::vector<Buffer<TRecord>> inBuffers;
		std::vector<uint64_t>        inIndexes;
		std::vector<uint64_t>        inLimits;
		std::vector<TRecord>         outBuffer;
		TRecord                      minRecord;
		auto                         outIndex         = 0ul;
		auto                         minIndex         = 0ul;
		auto                         validReaders     = readers.size();
		auto                         writtenSequences = 0ul;

		// Initialize the data structures and pre-load the input buffers.
		inBuffers.resize(readers.size());
		inIndexes.resize(readers.size());
		inLimits.resize(readers.size());
		outBuffer.resize(maxLoadable);
		for (auto i = 0ul;
		     i < readers.size();
		     i++)
		{
			inIndexes[i] = 0ul;
			inLimits[i]  = inBuffers[i].load(readers[i],
			                                 maxLoadable);
		}

		// Pre-select the first minimum record to be considered for the merge
		// procedure.
		minIndex = findMinRecord_(inBuffers,
		                          inIndexes,
		                          inLimits);
		outBuffer[0] = std::move(inBuffers[minIndex][0]);
		inIndexes[minIndex]++;
		if (inIndexes[minIndex] == inLimits[minIndex])
		{
			inIndexes[minIndex] = 0ul;
			inLimits[minIndex]  = reloadEmptyBuffer_(inBuffers[minIndex],
			                                         readers[minIndex],
			                                         maxLoadable);
			if (inLimits[minIndex] == 0)
			{
				validReaders--;
			}
		}

		// Start the merge loop, where input data are read batch by batch,
		// moved to the output buffer and printed out once the output buffer is
		// full.
		while (validReaders > 0)
		{
			minIndex  = findMinRecord_(inBuffers,
			                           inIndexes,
			                           inLimits);
			minRecord = std::move(inBuffers[minIndex][inIndexes[minIndex]]);
			if (outBuffer[outIndex].getSequence() == minRecord.getSequence())
			{
				TRecord::merge(outBuffer[outIndex],
				               minRecord);
			}
			else
			{
				outIndex++;
				if (outIndex == maxLoadable)
				{
					writer.write(outBuffer.begin(),
					             outBuffer.end());
					writtenSequences += maxLoadable;
					outIndex = 0ul;
				}
				outBuffer[outIndex] = std::move(minRecord);
			}

			// Check if reload buffer and update remained buffers.
			inIndexes[minIndex]++;
			if (inIndexes[minIndex] == inLimits[minIndex])
			{
				inIndexes[minIndex] = 0ul;
				inLimits[minIndex]  = reloadEmptyBuffer_(inBuffers[minIndex],
				                                         readers[minIndex],
				                                         maxLoadable);
				if (inLimits[minIndex] == 0ul)
				{
					validReaders--;
				}
			}
		}
		outBuffer.resize(outIndex + 1);
		writer.write(outBuffer.begin(),
		             outBuffer.end());
		writtenSequences += outIndex + 1;

		return writtenSequences;
	}

	template <typename TTabRecord,
	          typename TReader,
	          typename TOutWriter,
	          typename TTabWriter>
	static inline auto
	merge (std::vector<TReader>& readers,
	       TOutWriter& outWriter,
	       TTabWriter& tabWriter,
	       std::vector<uint64_t>& samplesIds,
	       uint64_t maxLoadable,
	       uint64_t nSamples) noexcept
	{
		std::vector<Buffer<TRecord>> inBuffers;
		std::vector<uint64_t>        inIndexes;
		std::vector<uint64_t>        inLimits;
		std::vector<TRecord>         outBuffer;
		std::vector<TTabRecord>      tabBuffer;
		TRecord                      minRecord;
		TTabRecord                   minTabRecord;
		auto                         outIndex         = 0ul;
		auto                         minIndex         = 0ul;
		auto                         validReaders     = readers.size();
		auto                         writtenSequences = 0ul;

		// Initialize the data structures and pre-load the input buffers.
		inBuffers.resize(readers.size());
		inIndexes.resize(readers.size());
		inLimits.resize(readers.size());
		outBuffer.resize(maxLoadable);
		tabBuffer.resize(maxLoadable);
		for (auto i = 0ul;
		     i < readers.size();
		     i++)
		{
			inIndexes[i] = 0ul;
			inLimits[i]  = inBuffers[i].load(readers[i],
			                                 maxLoadable);
		}

		// Pre-select the first minimum record to be considered for the merge
		// procedure.
		minIndex = findMinRecord_(inBuffers,
		                          inIndexes,
		                          inLimits);
		outBuffer[0] = std::move(inBuffers[minIndex][0]);
		tabBuffer[0].initialize(outBuffer[0].getCount(),
		                        samplesIds[minIndex],
		                        nSamples);
		inIndexes[minIndex]++;
		if (inIndexes[minIndex] == inLimits[minIndex])
		{
			inIndexes[minIndex] = 0ul;
			inLimits[minIndex]  = reloadEmptyBuffer_(inBuffers[minIndex],
			                                         readers[minIndex],
			                                         maxLoadable);
			if (inLimits[minIndex] == 0ul)
			{
				validReaders--;
			}
		}

		// Start the merge loop, where input data are read batch by batch,
		// moved to the output buffer and printed out once the output buffer is
		// full.
		while (validReaders > 0)
		{
			minIndex  = findMinRecord_(inBuffers,
			                           inIndexes,
			                           inLimits);
			minRecord = std::move(inBuffers[minIndex][inIndexes[minIndex]]);
			minTabRecord.initialize(minRecord.getCount(),
			                        samplesIds[minIndex],
			                        nSamples);
			if (outBuffer[outIndex].getSequence() == minRecord.getSequence())
			{
				TRecord::merge(outBuffer[outIndex],
				               minRecord);
				TTabRecord::merge(tabBuffer[outIndex],
				                  minTabRecord);
			}
			else
			{
				outIndex++;
				if (outIndex == maxLoadable)
				{
					outWriter.write(outBuffer.begin(),
					                outBuffer.end());
					tabWriter.write(tabBuffer.begin(),
					                tabBuffer.end());
					writtenSequences += maxLoadable;
					outIndex = 0ul;
				}
				outBuffer[outIndex] = std::move(minRecord);
				tabBuffer[outIndex] = std::move(minTabRecord);
			}

			// Check if reload buffer and update remained buffers.
			inIndexes[minIndex]++;
			if (inIndexes[minIndex] == inLimits[minIndex])
			{
				inIndexes[minIndex] = 0ul;
				inLimits[minIndex]  = reloadEmptyBuffer_(inBuffers[minIndex],
				                                         readers[minIndex],
				                                         maxLoadable);
				if (inLimits[minIndex] == 0ul)
				{
					validReaders--;
				}
			}
		}
		outBuffer.resize(outIndex + 1);
		outWriter.write(outBuffer.begin(),
		                outBuffer.end());
		tabBuffer.resize(outIndex + 1);
		tabWriter.write(tabBuffer.begin(),
		                tabBuffer.end());
		writtenSequences += outIndex + 1;

		return writtenSequences;
	}

	template <typename TTabRecord,
	          typename TReader,
	          typename TTabReader,
	          typename TOutWriter,
	          typename TTabWriter>
	static inline auto
	merge (std::vector<TReader>& readers,
	       std::vector<TTabReader>& tabReaders,
	       TOutWriter& outWriter,
	       TTabWriter& tabWriter,
	       uint64_t maxLoadable) noexcept
	{
		std::vector<Buffer<TRecord>>    inBuffers;
		std::vector<Buffer<TTabRecord>> inTabBuffers;
		std::vector<uint64_t>           inIndexes;
		std::vector<uint64_t>           inLimits;
		std::vector<TRecord>            outBuffer;
		std::vector<TTabRecord>         tabBuffer;
		TRecord                         minRecord;
		TTabRecord                      minTabRecord;
		auto                            outIndex         = 0ul;
		auto                            minIndex         = 0ul;
		auto                            validReaders     = readers.size();
		auto                            writtenSequences = 0ul;

		// Initialize the data structures and pre-load the input buffers.
		inBuffers.resize(readers.size());
		inTabBuffers.resize(readers.size());
		inIndexes.resize(readers.size());
		inLimits.resize(readers.size());
		outBuffer.resize(maxLoadable);
		tabBuffer.resize(maxLoadable);
		for (auto i = 0ul;
		     i < readers.size();
		     i++)
		{
			inIndexes[i] = 0ul;
			inLimits[i]  = inBuffers[i].load(readers[i],
			                                 maxLoadable);
			inLimits[i]  = inTabBuffers[i].load(tabReaders[i],
			                                    maxLoadable);
		}

		// Pre-select the first minimum record to be considered for the merge
		// procedure.
		minIndex = findMinRecord_(inBuffers,
		                          inIndexes,
		                          inLimits);
		outBuffer[0] = std::move(inBuffers[minIndex][0]);
		tabBuffer[0] = std::move(inTabBuffers[minIndex][0]);
		inIndexes[minIndex]++;
		if (inIndexes[minIndex] == inLimits[minIndex])
		{
			inIndexes[minIndex] = 0ul;
			inLimits[minIndex]  = reloadEmptyBuffer_(inBuffers[minIndex],
			                                         readers[minIndex],
			                                         inTabBuffers[minIndex],
			                                         tabReaders[minIndex],
			                                         maxLoadable);
			if (inLimits[minIndex] == 0ul)
			{
				validReaders--;
			}
		}

		// Start the merge loop, where input data are read batch by batch,
		// moved to the output buffer and printed out once the output buffer is
		// full.
		while (validReaders > 0)
		{
			minIndex     = findMinRecord_(inBuffers,
			                              inIndexes,
			                              inLimits);
			minRecord    = std::move(inBuffers[minIndex][inIndexes[minIndex]]);
			minTabRecord = std::move(inTabBuffers[minIndex][inIndexes[minIndex]]);
			if (outBuffer[outIndex].getSequence() == minRecord.getSequence())
			{
				TRecord::merge(outBuffer[outIndex],
				               minRecord);
				TTabRecord::merge(tabBuffer[outIndex],
				                  minTabRecord);
			}
			else
			{
				outIndex++;
				if (outIndex == maxLoadable)
				{
					outWriter.write(outBuffer.begin(),
					                outBuffer.end());
					tabWriter.write(tabBuffer.begin(),
					                tabBuffer.end());
					writtenSequences += maxLoadable;
					outIndex = 0ul;
				}
				outBuffer[outIndex] = std::move(minRecord);
				tabBuffer[outIndex] = std::move(minTabRecord);
			}

			// Check if reload buffer and update remained buffers.
			inIndexes[minIndex]++;
			if (inIndexes[minIndex] == inLimits[minIndex])
			{
				inIndexes[minIndex] = 0ul;
				inLimits[minIndex]  = reloadEmptyBuffer_(inBuffers[minIndex],
				                                         readers[minIndex],
				                                         inTabBuffers[minIndex],
				                                         tabReaders[minIndex],
				                                         maxLoadable);
				if (inLimits[minIndex] == 0ul)
				{
					validReaders--;
				}
			}
		}
		outBuffer.resize(outIndex + 1);
		outWriter.write(outBuffer.begin(),
		                outBuffer.end());
		tabBuffer.resize(outIndex + 1);
		tabWriter.write(tabBuffer.begin(),
		                tabBuffer.end());
		writtenSequences += outIndex + 1;

		return writtenSequences;
	}

	Buffer () noexcept = default;

	Buffer (const Buffer& other) = default;

	Buffer&
	operator= (const Buffer& other) = default;

	Buffer (Buffer&& other) noexcept = default;

	Buffer&
	operator= (Buffer&& other) noexcept = default;

	inline void
	reset () noexcept
	{
		buffer_.resize(0);
		buffer_.shrink_to_fit();
	}

	template <typename TComparator>
	inline void
	sort (uint64_t nThreads,
	      TComparator comparator) noexcept
	{
		constexpr auto minRecordsPerThreads = 1ul << 10;

		nThreads = computeMaxThreads_(nThreads,
		                              minRecordsPerThreads);
		if (nThreads > 1)
		{
			sortBufferMT_(nThreads,
			              comparator);
		}
		else
		{
			std::sort(buffer_.begin(),
			          buffer_.end(),
			          comparator);
		}
	}

	template <typename TEqComparator>
	inline auto
	collapse (uint64_t nThreads,
	          TEqComparator eqComparator) noexcept
	{
		constexpr auto minRecordsPerThreads = 1ul << 10;

		uint64_t collapsed = 0ul;

		nThreads = computeMaxThreads_(nThreads,
		                              minRecordsPerThreads);
		if (nThreads > 1)
		{
			collapsed = collapseRangeMT_(nThreads,
			                             eqComparator);
		}
		else
		{
			auto pair = collapseBufferRange_(buffer_.begin(),
			                                 buffer_.end(),
			                                 eqComparator);

			collapsed = pair.second;
		}

		return collapsed;
	}

	template <typename TReader>
	inline auto
	load (TReader& reader) noexcept
	{
		constexpr auto resizeQuota = 1ul << 16;

		auto readRecords = 0ul;

		while (!reader.atEnd())
		{
			auto nextBufferSize = readRecords + resizeQuota;

			buffer_.resize(nextBufferSize);
			readRecords += reader.read(buffer_.begin() + readRecords,
			                           buffer_.begin() + nextBufferSize);
		}
		buffer_.resize(readRecords);

		return readRecords;
	}

	template <typename TReader>
	inline auto
	load (TReader& reader,
	      uint64_t nRecords) noexcept
	{
		auto readRecords = 0ul;

		buffer_.resize(nRecords);
		readRecords = reader.read(buffer_.begin(),
		                          buffer_.end());
		buffer_.resize(readRecords);

		return readRecords;
	}

	template <typename TWriter>
	inline auto
	store (TWriter& writer) const noexcept
	{
		return writer.write(buffer_.begin(),
		                    buffer_.end());
	}

	template <typename TWriter,
	          typename TValidator>
	inline auto
	storeIf (TWriter& writer,
	         TValidator validator) const noexcept
	{
		return writer.writeIf(buffer_.begin(),
		                      buffer_.end(),
		                      validator);
	}

	template <typename TOperation>
	inline void
	forEach (TOperation operation) noexcept
	{
		for (auto& r : buffer_)
		{
			operation(r);
		}
	}

	inline auto
	memoryConsumption () const noexcept
	{
		auto memoryUtil = 0ul;

		for (const auto& r : buffer_)
		{
			memoryUtil += r.memoryConsumption();
		}
		memoryUtil += sizeof(Buffer);

		return memoryUtil;
	}

	inline auto&
	operator[] (uint64_t index) const noexcept
	{
		return buffer_[index];
	}

private:

	std::vector<TRecord> buffer_;

	static inline auto
	findMinRecord_ (const std::vector<Buffer<TRecord>>& buffers,
	                const std::vector<uint64_t>& indices,
	                const std::vector<uint64_t>& limits) noexcept
	{
		auto minIndex = 0ul;

		// Assume that at least one valid buffer is always present.
		while (limits[minIndex] == 0)
		{
			minIndex++;
		}

		// Look for the index of the buffer with the minimum tag sequence.
		for (auto i = minIndex + 1; i < buffers.size(); i++)
		{
			if (limits[i] != 0)
			{
				const auto& recordLeft  = buffers[i][indices[i]];
				const auto& recordRight = buffers[minIndex][indices[minIndex]];

				if (recordLeft.getSequence() < recordRight.getSequence())
				{
					minIndex = i;
				}
			}
		}

		return minIndex;
	}

	template <typename TReader>
	static inline auto
	reloadEmptyBuffer_ (Buffer<TRecord>& buffer,
	                    TReader& reader,
	                    uint64_t maxLoadable) noexcept
	{
		if (reader.atEnd())
		{
			return 0ul;
		}

		return buffer.load(reader,
		                   maxLoadable);
	}

	template <typename TReader,
	          typename TTabRecord,
	          typename TTabReader>
	static inline auto
	reloadEmptyBuffer_ (Buffer<TRecord>& buffer,
	                    TReader& reader,
	                    Buffer<TTabRecord>& tabBuffer,
	                    TTabReader& tabReader,
	                    uint64_t maxLoadable) noexcept
	{
		if (reader.atEnd())
		{
			return 0ul;
		}

		buffer.load(reader,
		            maxLoadable);
		return tabBuffer.load(tabReader,
		                      maxLoadable);
	}

	inline auto
	computeMaxThreads_ (uint64_t nThreads,
	                    uint64_t recordsPerThread) noexcept
	{
		uint64_t theoricThreads = std::max(1ul,
		                                   buffer_.size() / recordsPerThread);

		return std::min(nThreads,
		                theoricThreads);
	}

	inline void
	setBufferBounds_ (std::vector<typename decltype(buffer_)::iterator>& bounds,
	                  uint64_t nThreads)
	{
		uint64_t sequencesPerRegion = buffer_.size() / nThreads;

		bounds.resize(nThreads + 1);
		for (auto i      = 0ul;
		     i < nThreads;
		     i++)
		{
			bounds[i] = buffer_.begin() + i * sequencesPerRegion;
		}
		bounds[nThreads] = buffer_.end();
	}

	template <typename TComparator>
	inline void
	sortBufferMT_ (uint64_t nThreads,
	               TComparator comparator) noexcept
	{
		std::vector<typename decltype(buffer_)::iterator> bounds;
		uint64_t                                          span = 1;

		setBufferBounds_(bounds,
		                 nThreads);

		SEQAN_OMP_PRAGMA(parallel
				                 for
				                 num_threads(nThreads))
		for (auto i = 0u; i < nThreads; i++)
		{
			std::sort(bounds[i],
			          bounds[i + 1],
			          comparator);
		}

		while (span < nThreads)
		{
			SEQAN_OMP_PRAGMA(parallel
					                 for
					                 num_threads(nThreads - span))
			for (auto i = 0u; i < nThreads - span; i += 2 * span)
			{
				std::inplace_merge(bounds[i],
				                   bounds[i + span],
				                   bounds[std::min(i + 2 * span,
				                                   nThreads)],
				                   comparator);
			}
			span *= 2;
		}
	}

	template <typename TEqComparator>
	inline auto
	collapseBufferRange_ (typename decltype(buffer_)::iterator itBegin,
	                      typename decltype(buffer_)::iterator itEnd,
	                      TEqComparator eqComparator) noexcept
	{
		auto itBase     = itBegin;
		auto inProgress = false;
		auto collapsed  = 1ul;

		for (auto it = itBegin + 1;
		     it < itEnd;
		     it++)
		{
			if (eqComparator(*itBase,
			                 *it))
			{
				inProgress = true;
			}
			else
			{
				if (inProgress)
				{
					TRecord::merge(itBase,
					               it);
					inProgress = false;
				}
				itBase = it;
				collapsed++;
			}
		}
		if (inProgress)
		{
			TRecord::merge(itBase,
			               itEnd);
		}

		return std::make_pair(itBase,
		                      collapsed);
	}

	template <typename TEqComparator>
	inline uint64_t
	collapseRangeMT_ (uint64_t nThreads,
	                  TEqComparator eqComparator) noexcept
	{
		std::vector<typename decltype(buffer_)::iterator> bounds;
		std::vector<std::pair<typename decltype(buffer_)::iterator,
		                      uint64_t>>                  STCollapseMeta;
		uint64_t                                          collapsed = 0;

		setBufferBounds_(bounds,
		                 nThreads);
		STCollapseMeta.resize(nThreads);

		SEQAN_OMP_PRAGMA(parallel
				                 for
				                 num_threads(nThreads))
		for (auto i = 0u; i < nThreads; i++)
		{
			STCollapseMeta[i] = collapseBufferRange_(bounds[i],
			                                         bounds[i + 1],
			                                         eqComparator);
		}

		collapsed = 0;
		for (auto i = 0u; i < nThreads; i++)
		{
			collapsed += STCollapseMeta[i].second;
		}

		for (auto i = 0u; i < nThreads - 1; i++)
		{
			if (eqComparator(*(STCollapseMeta[i].first),
			                 *bounds[i + 1]))
			{
				TRecord::merge(*(STCollapseMeta[i].first),
				               *bounds[i + 1]);
				collapsed--;
			}
		}

		return collapsed;
	}
};

}

#endif // BIOSEQZIP_INCLUDE_BIOSEQZIP_BUFFER_H
