/**
 * \file      include/bioseqzip/core/sequence_record.h
 * \author    Gianvito Urgese
 * \author    Emanuele Parisi
 * \author    Orazio Scicolone
 * \date      November, 2018
 *
 * \copyright Copyright (C) 2019 Gianvito Urgese. All Rights Reserved.
 *
 * File containing basic facilities for records reading, writing and trimming,
 * other than records comparison operators.
 */

#ifndef BIOSEQZIP_BIOSEQZIP_CORE_SEQUENCE_RECORD_H
#define BIOSEQZIP_BIOSEQZIP_CORE_SEQUENCE_RECORD_H

#include <seqan/seq_io.h>

#include "tag_io.h"

namespace sa = seqan;

namespace bioseqzip
{

/**
 * Class representing a single read tag record, storing its sequence, counter
 * and qualities, if available.
 *
 * \tparam TTag is the type used for representing a record sequence.
 * \tparam TQual is the type used for representing a record qualities.
 * \tparam TCounter is the type used for storing how many time a record is
 * found in a sample.
 * \tparam TBpOffset is the type used for storing the length of the first
 * mate in case of paired-end datasets.
 */
template <typename TTag,
          typename TQual,
          typename TCounter,
          typename TBpOffset>
class SequenceRecord
{
public:

	/**
	 * Method for merging two records, marking the latter as not valid.
	 *
	 * \param record1 is the first record to be merged and the one where merge
	 * result is stored.
	 * \param record2 is the second record to be merged, and the one that is
	 * marked as not valid when merge operation is accomplished.
	 */
	static inline void
	merge (SequenceRecord<TTag,
	                      TQual,
	                      TCounter,
	                      TBpOffset>& record1,
	       SequenceRecord<TTag,
	                      TQual,
	                      TCounter,
	                      TBpOffset>& record2) noexcept
	{
		using TQAlpha = typename sa::Value<TQual>::Type;

		TCounter totOverall = record1.overall_ + record2.overall_;

		for (auto i = 0u; i < sa::length(record1.qual_); i++)
		{
			int64_t weightQ1   = record1.qual_[i] * record1.overall_;
			int64_t weightQ2   = record2.qual_[i] * record2.overall_;
			int64_t weightQSum = weightQ1 + weightQ2;

			record1.qual_[i] = static_cast<TQAlpha>(weightQSum / totOverall);
		}
		record1.overall_ = totOverall;
		record2.overall_ = static_cast<TCounter>(0);
	}

	/**
	 * Method for merging a set of records, marking all but the first as non
	 * valid.
	 *
     * \tparam TRecordIter is the iterator-type pointing to a buffer entry.
	 * \param itStart is the first record to be considered and the only one
	 * marked as valid once the merge procedure is accomplished.
	 * \param itEnd is the first record not to be considered.
	 */
	template <typename TRecordIter>
	static inline void
	merge (TRecordIter itStart,
	       TRecordIter itEnd) noexcept
	{
		using TQAlpha = typename sa::Value<TQual>::Type;

		sa::String<int64_t> tmpQuality;
		TCounter            tmpOverall = 0;

		sa::resize(tmpQuality,
		           sa::length(itStart->qual_),
		           0);
		for (auto it = itStart; it != itEnd; it++)
		{
			for (auto i = 0u; i < sa::length(it->qual_); i++)
			{
				tmpQuality[i] += it->qual_[i] * it->overall_;
			}
			tmpOverall += it->overall_;
			if (it != itStart)
			{
				it->overall_ = 0;
			}
		}
		for (auto i  = 0u; i < sa::length(itStart->qual_); i++)
		{
			TQAlpha qAlpha = static_cast<TQAlpha>(tmpQuality[i] / tmpOverall);

			itStart->qual_[i] = qAlpha;
		}
		itStart->overall_ = tmpOverall;
	}

	/**
	 * Class constructor.
	 */
	SequenceRecord () noexcept
	{
		overall_  = 0;
		bpOffset_ = 0;
	}

	/**
	 * Class copy constructor.
	 *
	 * \param other is the record from which the current instance is
	 * initialized.
	 */
	SequenceRecord (const SequenceRecord& other) noexcept
	{
		sa::assign(tag_,
		           other.tag_);
		sa::assign(qual_,
		           other.qual_);
		overall_  = other.overall_;
		bpOffset_ = other.bpOffset_;
	}

	/**
	 * Class copy assignment operator.
	 *
	 * \param other is the record assigned to the current instance.
	 * \return the reference to the current record.
	 */
	inline SequenceRecord&
	operator= (const SequenceRecord& other) noexcept
	{
		if (this != &other)
		{
			sa::assign(tag_,
			           other.tag_);
			sa::assign(qual_,
			           other.qual_);
			overall_  = other.overall_;
			bpOffset_ = other.bpOffset_;
		}

		return *this;
	}

	/**
	 * Class move constructor.
	 *
	 * \param other is the record moved into the current instance.
	 */
	SequenceRecord (SequenceRecord&& other) noexcept
	{
		sa::move(tag_,
		         other.tag_);
		sa::move(qual_,
		         other.qual_);
		overall_ = other.overall_;
		other.overall_ = static_cast<TCounter>(0);
		bpOffset_ = other.bpOffset_;
		other.bpOffset_ = static_cast<TBpOffset>(0);
	}

	/**
     * Class move assignment operator.
     *
     * \param other is the record moved into the current instance.
     * \return the reference to the current record.
     */
	inline SequenceRecord&
	operator= (SequenceRecord&& other) noexcept
	{
		if (this != &other)
		{
			sa::move(tag_,
			         other.tag_);
			sa::move(qual_,
			         other.qual_);
			overall_ = other.overall_;
			other.overall_ = static_cast<TCounter>(0);
			bpOffset_ = other.bpOffset_;
			other.bpOffset_ = static_cast<TBpOffset>(0);
		}

		return *this;
	}

	inline auto&
	getSequence () const noexcept
	{
		return tag_;
	}

	inline auto&
	getQualities () const noexcept
	{
		return qual_;
	}

	inline auto
	getCount () const noexcept
	{
		return overall_;
	}

	inline auto
	getBpOffset () const noexcept
	{
		return bpOffset_;
	}

	/**
	 * Read a record from any single SeqAn2 SeqFileIn stream.
	 *
	 * \tparam TForceNoQual bool type for forcing qualities to be ignored.
	 * \param source is the stream from which data are read.
	 * \param bpOffset is the offset set into the record read.
	 */
	template <typename TForceNoQual>
	inline void
	read (sa::SeqFileIn& source,
	      TBpOffset bpOffset) noexcept
	{
		sa::String<char> meta;

		sa::readRecord(meta,
		               tag_,
		               qual_,
		               source);
		sa::shrinkToFit(tag_);
		sa::shrinkToFit(qual_);
		if (TForceNoQual::value)
		{
			_clearQual();
		}
		overall_  = static_cast<TCounter>(1);
		bpOffset_ = bpOffset;
	}

	/**
	 * Read a record from any pair of SeqAn2 SeqFileIn streams.
	 *
	 * \tparam TForceNoQual bool type for forcing qualities to be ignored.
	 * \param sourceMate1 is the stream from which the first mates are read.
	 * \param sourceMate1 is the stream from which the second mates are read.
	 */
	template <typename TForceNoQual>
	inline void
	read (sa::SeqFileIn& sourceMate1,
	      sa::SeqFileIn& sourceMate2) noexcept
	{
		sa::String<char> mate;
		TTag             tmpMate2;
		TQual            tmpQualMate2;

		sa::readRecord(mate,
		               tag_,
		               qual_,
		               sourceMate1);
		sa::readRecord(mate,
		               tmpMate2,
		               tmpQualMate2,
		               sourceMate2);
		bpOffset_ = sa::length(tag_);
		sa::append(tag_,
		           tmpMate2);
		sa::shrinkToFit(tag_);
		if (TForceNoQual::value)
		{
			_clearQual();
		}
		else
		{
			sa::append(qual_,
			           tmpQualMate2);
			sa::shrinkToFit(qual_);
		}
		overall_ = static_cast<TCounter>(1);
	}

	/**
	 * Read a record from any single SeqAn2 TagFileIn stream.
	 *
	 * \tparam TForceNoQual bool type for forcing qualities to be ignored.
	 * \param source is the stream from which data are read.
	 * \param bpOffset is the offset set into the record read.
	 */
	template <typename TForceNoQual>
	inline void
	read (sa::TagFileIn& source,
	      TBpOffset bpOffset) noexcept
	{
		sa::readRecord(tag_,
		               qual_,
		               overall_,
		               source);
		sa::shrinkToFit(tag_);
		sa::shrinkToFit(qual_);
		if (TForceNoQual::value)
		{
			_clearQual();
		}
		bpOffset_ = bpOffset;
	}

	/**
	 * Read a record from any pair of SeqAn2 TagFileIn streams.
	 *
	 * \tparam TForceNoQual bool type for forcing qualities to be ignored.
	 * \param sourceMate1 is the stream from which the first mates are read.
	 * \param sourceMate1 is the stream from which the second mates are read.
	 */
	template <typename TForceNoQual>
	inline void
	read (sa::TagFileIn& sourceMate1,
	      sa::TagFileIn& sourceMate2) noexcept
	{
		TTag  tmpMate2;
		TQual tmpQualMate2;

		sa::readRecord(tag_,
		               qual_,
		               overall_,
		               sourceMate1);
		sa::readRecord(tmpMate2,
		               tmpQualMate2,
		               overall_,
		               sourceMate2);
		bpOffset_ = sa::length(tag_);
		sa::append(tag_,
		           tmpMate2);
		sa::shrinkToFit(tag_);
		if (TForceNoQual::value)
		{
			_clearQual();
		}
		else
		{
			sa::append(qual_,
			           tmpQualMate2);
			sa::shrinkToFit(qual_);
		}
	}

	/**
	 * Write a record in the given SeqAn2 SeqFileOut stream with the specified
	 * identifier.
	 *
	 * \param sink is the stream to where data are written.
	 * \param id is the numerical identifier given to the sequence.
	 */
	inline void
	write (sa::SeqFileOut& sink,
	       uint64_t id) const noexcept
	{
		if (sa::length(qual_) == 0)
		{
			sa::writeRecord(sink,
			                _makeFastxHeader(id),
			                tag_);
		}
		else
		{
			sa::writeRecord(sink,
			                _makeFastxHeader(id),
			                tag_,
			                qual_);
		}
	}

	/**
	 * Write a record in the given SeqAn2 SeqFileOut stream with the specified
	 * identifier.
	 *
	 * \param sinkMate1 is the stream to where first mates are written.
	 * \param sinkMate2 is the stream to where second mates are written.
	 * \param id is the numerical identifier given to the sequence.
	 */
	inline void
	write (sa::SeqFileOut& sinkMate1,
	       sa::SeqFileOut& sinkMate2,
	       uint64_t id) const noexcept
	{
		sa::String<char> fastxHeader = _makeFastxHeader(id);

		if (sa::length(qual_) == 0)
		{
			sa::writeRecord(sinkMate1,
			                fastxHeader,
			                sa::prefix(tag_,
			                           bpOffset_));
			sa::writeRecord(sinkMate2,
			                fastxHeader,
			                sa::suffix(tag_,
			                           bpOffset_));
		}
		else
		{
			sa::writeRecord(sinkMate1,
			                fastxHeader,
			                sa::prefix(tag_,
			                           bpOffset_),
			                sa::prefix(qual_,
			                           bpOffset_));
			sa::writeRecord(sinkMate2,
			                fastxHeader,
			                sa::suffix(tag_,
			                           bpOffset_),
			                sa::suffix(qual_,
			                           bpOffset_));
		}
	}

	/**
	 * Write a record in the given SeqAn2 TagFileOut stream with the specified
	 * identifier.
	 *
	 * \param sink is the stream to where data are written.
	 * \param id is the numerical identifier given to the sequence.
	 */
	inline void
	write (sa::TagFileOut& sink,
	       uint64_t id) const noexcept
	{
		if (sa::length(qual_) == 0)
		{
			sa::writeRecord(sink,
			                tag_,
			                overall_);
		}
		else
		{
			sa::writeRecord(sink,
			                tag_,
			                qual_,
			                overall_);
		}
	}

	/**
	 * Write a record in the given SeqAn2 TagFileOut stream with the specified
	 * identifier.
	 *
	 * \param sinkMate1 is the stream to where first mates are written.
	 * \param sinkMate2 is the stream to where second mates are written.
	 * \param id is the numerical identifier given to the sequence.
	 */
	inline void
	write (sa::TagFileOut& sinkMate1,
	       sa::TagFileOut& sinkMate2,
	       uint64_t id) const noexcept
	{
		if (sa::length(qual_) == 0)
		{
			sa::writeRecord(sinkMate1,
			                sa::prefix(tag_,
			                           bpOffset_),
			                overall_);
			sa::writeRecord(sinkMate2,
			                sa::suffix(tag_,
			                           bpOffset_),
			                overall_);
		}
		else
		{
			sa::writeRecord(sinkMate1,
			                sa::prefix(tag_,
			                           bpOffset_),
			                sa::prefix(qual_,
			                           bpOffset_),
			                overall_);
			sa::writeRecord(sinkMate2,
			                sa::suffix(tag_,
			                           bpOffset_),
			                sa::suffix(qual_,
			                           bpOffset_),
			                overall_);
		}
	}

	/**
	 * Compute the RAM memory required by the record.
	 *
	 * \return the amount of Bytes the record occupies in the system RAM.
	 */
	inline auto
	memoryConsumption () const noexcept
	{
		uint64_t tag_len         = sa::capacity(tag_);
		uint64_t tag_value_size  = sizeof(typename sa::Value<TTag>::Type);
		uint64_t quals_len       = sa::capacity(qual_);
		uint64_t qual_value_size = sizeof(typename sa::Value<TQual>::Type);

		return tag_len * tag_value_size +
		       quals_len * qual_value_size +
		       sizeof(SequenceRecord<TTag,
		                             TQual,
		                             TCounter,
		                             TBpOffset>);
	}

	inline auto
	trim (uint64_t trimLeft,
	      uint64_t trimRight) noexcept
	{
		auto sequenceInfix = seqan::infix(tag_,
		                                  trimLeft,
		                                  seqan::length(tag_) - trimRight);
		auto qualityInfix  = seqan::infix(qual_,
		                                  trimLeft,
		                                  seqan::length(qual_) - trimRight);

		tag_ = TTag(sequenceInfix);
		seqan::shrinkToFit(tag_);
		qual_ = TQual(qualityInfix);
		seqan::shrinkToFit(qual_);
		if (bpOffset_ < trimLeft)
		{
			bpOffset_ = 0ul;
		}
		else if (bpOffset_ >= trimLeft &&
		         bpOffset_ <= seqan::length(tag_) - trimRight)
		{
			bpOffset_ -= trimLeft;
		}
		else
		{
			bpOffset_ = seqan::length(tag_);
		}
	}

private:
	/**
	 * Tag sequence characterizing the record.
	 */
	TTag      tag_;
	/**
	 * Quality sequence of the record. This is empty if the record does not
	 * support qualities.
	 */
	TQual     qual_;
	/**
	 * The occurrences count of the current tag sequence.
	 */
	TCounter  overall_;
	/**
	 * Breakpoint offset in the current tag sequence used for delimiting the
	 * first mate from the second one. This value is zero for single-end data.
	 */
	TBpOffset bpOffset_;

	/**
	 * Build a record header, required for writing records to fastx formatted
	 * files.
	 *
	 * \param id is the unambiguous read identifier.
	 * \param mateTag is appended to the identifier; this is used for writing
	 * paired-end data.
	 * \return a string representing the current record fastx header.
	 */
	inline sa::String<char>
	_makeFastxHeader (uint64_t id) const noexcept
	{
		sa::String<char> header = "BIOSEQZIP|ID:";

		sa::append(header,
		           std::to_string(id).data());
		sa::append(header,
		           "|CN:");
		sa::append(header,
		           std::to_string(overall_).data());

		return header;
	}

	/**
	 * Clear record qualities.
	 */
	inline void
	_clearQual () noexcept
	{
		sa::clear(qual_);
		sa::shrinkToFit(qual_);
	}
};

} // bioseqzip

#endif // BIOSEQZIP_BIOSEQZIP_CORE_SEQUENCE_RECORD_H_
