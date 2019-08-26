/**
 * \file      include/bioseqzip/core/details_record.h
 * \author    Gianvito Urgese
 * \author    Emanuele Parisi
 * \author    Orazio Scicolone
 * \date      November, 2018
 *
 * \copyright Copyright (C) 2019 Gianvito Urgese. All Rights Reserved.
 *
 * File containing basic facilities for keeping track of tag sequences
 * occurrences over multiple samples datasets.
 */
#ifndef BIOSEQZIP_BIOSEQZIP_CORE_DETAILS_RECORD_H
#define BIOSEQZIP_BIOSEQZIP_CORE_DETAILS_RECORD_H

#include <fstream>

#include <seqan/sequence.h>

namespace sa = seqan;

namespace bioseqzip
{

/**
 * Class for keeping track of the number of times a tag sequence occur in a
 * dataset mad of multiple samples.
 *
 * \tparam TCounter is the type used for storing how many time a tag sequence
 * occurs in a sample.
 */
template <typename TCounter>
class DetailsRecord
{
public:

	/**
	 * Method for merging two records occurrences, marking the latter as not
	 * valid.
	 *
	 * \param record1 is the first occurrences array to be merged and the one
	 * where merge result is stored.
	 * \param record2 is the second occurrences array to be merged, and the one
	 * that is marked as not valid when merge operation is accomplished.
	 */
	static inline void
	merge (DetailsRecord<TCounter>& record1,
	       DetailsRecord<TCounter>& record2) noexcept
	{
		for (auto i      = 0u; i < seqan::length(record1.occurrences_); i++)
		{
			record1.occurrences_[i] += record2.occurrences_[i];
			record2.occurrences_[i] = 0;
		}
		record1.overall_ += record2.overall_;
		record2.overall_ = static_cast<TCounter>(0);
	}

	/**
	 * Method for merging a set of occurrences arrays, marking all but the first
	 * as non valid.
	 *
     * \tparam TOccurrencesIter is the iterator-type pointing to a buffer entry.
	 * \param itStart is the first occurrences array to be considered and the
	 * only one marked as valid once the merge procedure is accomplished.
	 * \param itEnd is the first occurrences array not to be considered.
	 */
	template <typename TOccurrencesIter>
	static inline void
	merge (TOccurrencesIter itStart,
	       TOccurrencesIter itEnd) noexcept
	{
		for (auto it = itStart + 1; it != itEnd; it++)
		{
			DetailsRecord::merge(*itStart,
			                     *it);
		}
	}

	/**
	 * Class constructor.
	 */
	DetailsRecord () noexcept : overall_(0)
	{}

	/**
	 * Class copy constructor.
	 *
	 * \param other is the occurrences array from which the current instance is
	 * initialized.
	 */
	DetailsRecord (const DetailsRecord& other) noexcept
	{
		seqan::assign(occurrences_,
		              other.occurrences_);
		overall_ = other.overall_;
	}

	/**
	 * Class copy assignment operator.
	 *
	 * \param other is the occurrences array assigned to the current instance.
	 * \return the reference to the current record.
	 */
	inline DetailsRecord&
	operator= (const DetailsRecord& other) noexcept
	{
		if (this != &other)
		{
			seqan::assign(occurrences_,
			              other.occurrences_);
			overall_ = other.overall_;
		}

		return *this;
	}

	/**
	 * Class move constructor.
	 *
	 * \param other is the occurrences array moved into the current instance.
	 */
	DetailsRecord (DetailsRecord&& other) noexcept
	{
		seqan::move(occurrences_,
		            other.occurrences_);
		overall_ = other.overall_;
		other.overall_ = static_cast<TCounter>(0);
	}

	/**
	 * Class move assignment operator.
	 *
	 * \param other is the record moved into the current instance.
	 * \return the reference to the current record.
	 */
	inline DetailsRecord&
	operator= (DetailsRecord&& other) noexcept
	{
		if (this != &other)
		{
			seqan::move(occurrences_,
			            other.occurrences_);
			overall_ = other.overall_;
			other.overall_ = static_cast<TCounter>(0);
		}

		return *this;
	}

	inline const auto&
	getCounters () const noexcept
	{
		return occurrences_;
	}

	inline auto
	getCount () const noexcept
	{
		return overall_;
	}

	/**
	 * Setup an occurrences array, initializing the number of occurrences for
	 * a single sample entry.
	 *
	 * \param overall is the number of occurrences a single sample is
	 * characterized from.
	 * \param sampleId is the numeric identifier of the initialized sample.
	 * \param nSamples is the overall number of samples represented by the
	 * occurrences array.
	 */
	inline void
	initialize (TCounter overall,
	            uint64_t sampleId,
	            uint64_t nSamples) noexcept
	{
		seqan::resize(occurrences_,
		              nSamples,
		              0,
		              seqan::Exact());
		occurrences_[sampleId] = overall;
		overall_ = overall;
	}

	/**
	 * Read an occurrences array from a source ASCII file.
	 *
	 * \param source is the stream data are read from.
	 */
	inline void
	read (std::ifstream& source) noexcept
	{
		std::istringstream stringStream;
		std::string        line;
		std::string        token;
		uint64_t           counter = 0;

		std::getline(source,
		             line);
		stringStream = std::istringstream(line);
		while (std::getline(stringStream,
		                    token,
		                    '\t'))
		{
			if (counter == 0)
			{
				overall_ = atoll(token.data());
			}
			else
			{
				if (seqan::length(occurrences_) >= counter)
				{
					occurrences_[counter - 1] = atoll(token.data());
				}
				else
				{
					seqan::appendValue(occurrences_,
					                   atoll(token.data()));
				}
			}
			counter++;
		}
	}

	/**
	 * Write an occurrences array to the target sink stream.
	 *
	 * \param sink is the stream to where data are written.
	 */
	inline void
	write (std::ofstream& sink) const noexcept
	{
		sink << overall_ << "\t";
		for (auto i = 0u; i < seqan::length(occurrences_) - 1; i++)
		{
			sink << occurrences_[i] << "\t";
		}
		sink << occurrences_[seqan::length(occurrences_) - 1] << "\n";
	}

	/**
	 * Compute the RAM memory required by the record.
	 *
	 * \return the amount of Bytes the record occupies in the system RAM.
	 */
	inline uint64_t
	memoryConsumption () const noexcept
	{
		return sizeof(TCounter) * seqan::capacity(occurrences_) +
		       sizeof(DetailsRecord<TCounter>);
	}

private:
	/**
	 * Array storing the statistics about how many time a given tag sequence
	 * appears in a given sample.
	 */
	seqan::String<TCounter> occurrences_;
	/**
	 * Overall tag sequence occurrences.
	 */
	TCounter                overall_;
};

} // bioseqzip

#endif // BIOSEQZIP_BIOSEQZIP_CORE_DETAILS_RECORD_H
