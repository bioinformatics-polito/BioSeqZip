/**
 * \file      include/bioseqzip/core/details_reader.h
 * \author    Gianvito Urgese
 * \author    Emanuele Parisi
 * \author    Orazio Scicolone
 * \date      November, 2018
 *
 * \copyright Copyright (C) 2019 Gianvito Urgese. All Rights Reserved.
 *
 * File containing facilities for loading occurrences array from ASCII files.
 */

#ifndef BIOSEQZIP_BIOSEQZIP_CORE_DETAILS_READER_H
#define BIOSEQZIP_BIOSEQZIP_CORE_DETAILS_READER_H

#include <experimental/filesystem>
#include <seqan/basic.h>
#include <seqan/seq_io.h>

namespace fs = std::experimental::filesystem;
namespace sa = seqan;

namespace bioseqzip
{

/**
 * Class in charge of loading occurrences arrays from the disk.
 *
 * \tparam TRecord is the kind of occurrences array read.
 */
template <typename TRecord>
class DetailsReader
{
public:

	/**
	 * Class constructor.
	 */
	DetailsReader () noexcept
	{}

	/**
	 * Class copy constructor.
	 *
	 * \param other is the reader the current instance is initialized from.
	 */
	DetailsReader (const DetailsReader& other) noexcept
	{
		path_ = other.path_;
		configure(other.path_);
	}

	/**
	 * Class copy assignment operator.
	 *
	 * \param other is the reader assigned to the current instance.
	 * \return the reference to the current reader.
	 */
	inline DetailsReader&
	operator= (const DetailsReader& other) noexcept
	{
		if (&other != this)
		{
			path_ = other.path_;
			configure(other.path_);
		}

		return *this;
	}

	/**
	 * Getter method for retrieving the header read from the currently opened
	 * file.
	 *
	 * \return a reference to the reader current header.
	 */
	inline std::vector<sa::String<char>>&
	getHeader () noexcept
	{
		return header_;
	}

	/**
	 * Reset the state of the reader. The reader will have to be initialized
	 * again before it can be used.
	 */
	inline void
	reset () noexcept
	{
		path_.clear();
		source_.close();
		header_.clear();
	}

	/**
	 * Configure the reader for loading data from the specified file.
	 *
	 * \param path is the file data are loaded from.
	 */
	inline void
	configure (const fs::path& path) noexcept
	{
		reset();
		path_ = path;
		source_.open(path_.generic_string());
		_readHeader();
	}

	/**
	 * Read a single record.
	 *
	 * \param record is the record data are loaded into.
	 */
	inline void
	read (TRecord& record) noexcept
	{
		record.read(source_);
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
		while (itBase != itEnd && !source_.eof())
		{
			read(*itBase);

			// Check required for not reading empty lines at the end or in the
			// middle of a file.
			if (itBase->getCount() > 0)
			{
				itBase++;
			}
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
		return source_.eof();
	}

private:
	/**
	 * Path records are read from.
	 */
	fs::path                      path_;
	/**
	 * Stream object the current reader use for reading data from the file.
	 */
	std::ifstream                 source_;
	/**
	 * Header of the file the current reader is reading.
	 */
	std::vector<sa::String<char>> header_;

	/**
	 * Read the first line of a newly opened file and parse the different header
	 * components.
	 */
	inline void
	_readHeader () noexcept
	{
		std::istringstream stringStream;
		std::string        line;
		std::string        token;
		uint64_t           counter = 0;

		header_.clear();
		std::getline(source_,
		             line);
		stringStream = std::istringstream(line);
		while (std::getline(stringStream,
		                    token,
		                    '\t'))
		{
			if (counter > 0)
			{
				header_.emplace_back(token);
			}
			counter++;
		}
	}
};
} // bioseqzip

#endif // BIOSEQZIP_BIOSEQZIP_CORE_DETAILS_READER_H