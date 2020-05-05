/**
 * \file      include/bioseqzip/core/sequence_reader.h
 * \author    Gianvito Urgese
 * \author    Emanuele Parisi
 * \author    Orazio Scicolone
 * \date      November, 2018
 *
 * \copyright Copyright (C) 2019 Gianvito Urgese. All Rights Reserved.
 *
 * File containing facilities for reading records from fastx and tagx formatted
 * files.
 */

#ifndef BIOSEQZIP_BIOSEQZIP_CORE_SEQUENCE_READER_H
#define BIOSEQZIP_BIOSEQZIP_CORE_SEQUENCE_READER_H

#include <seqan/seq_io.h>

#include "library_layout.h"

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

/**
 * Class in charge of loading records from the disk, handling different kind of
 * file layouts.
 *
 * \tparam TStream is the type of SeqAn2 stream deciding the format of the file
 * data are read from.
 * \tparam TRecord is the kind of record read.
 * \tparam TForceNoQual is the bool type deciding whether qualities are ignored.
 */
template <typename TStream,
          typename TRecord,
          typename TForceNoQual>
class SequenceReader
{
public:

	/**
	 * Class constructor.
	 */
	SequenceReader () noexcept
	{
		layout_   = LibraryLayout::SINGLE_END;
		bpOffset_ = 0;
	}

	/**
	 * Class copy constructor.
	 *
	 * \param other is the reader the current instance is initialized from.
	 */
	SequenceReader (const SequenceReader& other) noexcept
	{
		layout_    = other.layout_;
		pathMate1_ = other.pathMate1_;
		pathMate2_ = other.pathMate2_;
		bpOffset_  = other.bpOffset_;
		if (layout_ == LibraryLayout::SINGLE_END)
		{
			configureSingleEnd(other.pathMate1_);
		}
		else if (layout_ == LibraryLayout::PAIRED_END)
		{
			configurePairedEnd(other.pathMate1_,
			                   other.pathMate2_);
		}
		else if (layout_ == LibraryLayout::INTERLEAVED)
		{
			configureInterleaved(other.pathMate1_);
		}
		else
		{
			configureBreakpoint(other.pathMate1_,
			                    bpOffset_);
		}
	}

	/**
	 * Class copy assignment operator.
	 *
	 * \param other is the reader assigned to the current instance.
	 * \return the reference to the current reader.
	 */
	inline SequenceReader&
	operator= (const SequenceReader& other) noexcept
	{
		if (&other != this)
		{
			layout_    = other.layout_;
			pathMate1_ = other.pathMate1_;
			pathMate2_ = other.pathMate2_;
			bpOffset_  = other.bpOffset_;
			if (layout_ == LibraryLayout::SINGLE_END)
			{
				configureSingleEnd(other.pathMate1_);
			}
			else if (layout_ == LibraryLayout::PAIRED_END)
			{
				configurePairedEnd(other.pathMate1_,
				                   other.pathMate2_);
			}
			else if (layout_ == LibraryLayout::INTERLEAVED)
			{
				configureInterleaved(other.pathMate1_);
			}
			else
			{
				configureBreakpoint(other.pathMate1_,
				                    bpOffset_);
			}
		}

		return *this;
	}

	/**
	 * Reset the state of the reader. The reader will have to be initialized
	 * again before it can be used.
	 */
	inline void
	reset () noexcept
	{
		layout_ = LibraryLayout::SINGLE_END;
		pathMate1_.clear();
		pathMate2_.clear();
		sa::close(sourceMate1_);
		sa::close(sourceMate2_);
		bpOffset_ = 0;
	}

	/**
	 * Configure the reader for loading data from single-end samples.
	 *
	 * \param path is the single-end file data are loaded from.
	 */
	inline void
	configureSingleEnd (const fs::path& path) noexcept
	{
		reset();
		layout_    = LibraryLayout::SINGLE_END;
		pathMate1_ = path;
		sa::open(sourceMate1_,
		         pathMate1_.generic_string().data());
	}

	/**
	 * Configure the reader for loading data from paired-end samples.
	 *
	 * \param pathMate1 is the file path storing the first mate of the data to
	 * be loaded.
	 * \param pathMate2 is the file path storing the second mate of the data to
	 * be loaded.
	 */
	inline void
	configurePairedEnd (const fs::path& pathMate1,
	                    const fs::path& pathMate2) noexcept
	{
		reset();
		layout_    = LibraryLayout::PAIRED_END;
		pathMate1_ = pathMate1;
		pathMate2_ = pathMate2;
		sa::open(sourceMate1_,
		         pathMate1_.generic_string().data());
		sa::open(sourceMate2_,
		         pathMate2_.generic_string().data());
	}

	/**
	 * Configure the reader for loading data from interleaved samples.
	 *
	 * \param path is the interleaved file data are loaded from.
	 */
	inline void
	configureInterleaved (const fs::path& path) noexcept
	{
		reset();
		layout_    = LibraryLayout::INTERLEAVED;
		pathMate1_ = path;
		sa::open(sourceMate1_,
		         pathMate1_.generic_string().data());
	}

	/**
	 * Configure the reader for loading data from breakpointed samples.
	 *
	 * \param path is the breakpointed file data are loaded from.
	 * \param bpOffset is the breakpoint offset to set in the record read.
	 */
	inline void
	configureBreakpoint (const fs::path& path,
	                     uint64_t bpOffset) noexcept
	{
		reset();
		layout_    = LibraryLayout::BREAKPOINT;
		pathMate1_ = path;
		bpOffset_  = bpOffset;
		sa::open(sourceMate1_,
		         pathMate1_.generic_string().data());
	}

	/**
	 * Read and trim a record.
	 *
	 * \param record is the record data are loaded into.
	 * \param trimLeft is the amount of base-pairs trimmed from the left of the
	 * tag sequence.
	 * \param trimRight is the amount of base-pairs trimmed from the right of
	 * the tag sequence.
	 */
	inline void
	read (TRecord& record) noexcept
	{
		if (layout_ == LibraryLayout::SINGLE_END)
		{
			record.template read<TForceNoQual>(sourceMate1_,
			                                   0);
		}
		else if (layout_ == LibraryLayout::PAIRED_END)
		{
			record.template read<TForceNoQual>(sourceMate1_,
			                                   sourceMate2_);
		}
		else if (layout_ == LibraryLayout::INTERLEAVED)
		{
			record.template read<TForceNoQual>(sourceMate1_,
			                                   sourceMate1_);
		}
		else
		{
			record.template read<TForceNoQual>(sourceMate1_,
			                                   bpOffset_);
		}
	}

	/**
	 * Read a set of records and store them into the target buffer region.
	 *
	 * \tparam TRecordIter is the iterator-type pointing to a record buffer
	 * entry.
	 * \param itStart is the first record buffer entry to be considered.
	 * \param itEnd is the first record buffer entry not to be considered.
	 * \return the number of buffer entries actually filled.
	 */
	template <typename TRecordIter>
	inline uint64_t
	read (TRecordIter itStart,
	      TRecordIter itEnd) noexcept
	{
		TRecordIter itBase;

		itBase = itStart;
		while (itBase != itEnd && !sa::atEnd(sourceMate1_))
		{
			read(*itBase);
			itBase++;
		}

		return static_cast<uint64_t>(itBase - itStart);
	}

	/**
	 * Check if the current file content has been completely loaded.
	 *
	 * \return true if no more data have to be loaded, false otherwise.
	 */
	inline bool
	atEnd () const noexcept
	{
		return sa::atEnd(sourceMate1_);
	}

private:
	/**
	 * The layout of the sample the current reader has to read.
	 */
	LibraryLayout layout_;
	/**
	 * Path the data are loaded from. For paired-end data, this is the path of
	 * the file storing the first mates.
	 */
	fs::path      pathMate1_;
	/**
	 * Path storing the second mates of the sequences to be read. For single-end
	 * data this field is empty.
	 */
	fs::path      pathMate2_;
	/**
	 * Offset the records are initialized with, in case of breakpoint datasets.
	 */
	uint64_t      bpOffset_;
	/**
	 * Stream object data are loaded from. For paired-end data, this is the
	 * stream from where first mates are read.
	 */
	TStream       sourceMate1_;
	/**
	 * Stream object second mates are read from. For single-end data the stream
	 * is not used.
	 */
	TStream       sourceMate2_;
};
} // bioseqzip

#endif // BIOSEQZIP_BIOSEQZIP_CORE_SEQUENCE_READER_H

