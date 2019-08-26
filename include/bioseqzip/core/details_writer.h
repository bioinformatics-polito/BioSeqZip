/**
 * \file      include/bioseqzip/core/detailswrite_r.h
 * \author    Gianvito Urgese
 * \author    Emanuele Parisi
 * \author    Orazio Scicolone
 * \date      November, 2018
 *
 * \copyright Copyright (C) 2019 Gianvito Urgese. All Rights Reserved.
 *
 * File containing facilities for storing occurrences array in ASCII files.
 */

#ifndef BIOSEQZIP_BIOSEQZIP_CORE_DETAILS_WRITER_H
#define BIOSEQZIP_BIOSEQZIP_CORE_DETAILS_WRITER_H

#include <chrono>
#include <experimental/filesystem>
#include <seqan/sequence.h>
#include <string>
#include <vector>

namespace fs = std::experimental::filesystem;
namespace sa = seqan;

namespace bioseqzip
{

/**
 * Class in charge of writing one or more occurrences array to a proper file.
 *
 * \tparam TRecord is the kind of occurrences array written.
 */
template <typename TRecord>
class DetailsWriter
{
public:
	/**
	 * Class constructor.
	 */
	DetailsWriter () noexcept
	{
		maxOutRecords_ = std::numeric_limits<uint64_t>::max();
		curOutRecords_ = 0;
	}

	/**
	 * Getter method for retrieving the file paths written by the writer.
	 *
	 * \return a list of file paths, one for each written file.
	 */
	inline std::vector<fs::path>&
	getOutputPaths () noexcept
	{
		return outputPaths_;
	}

	/**
	 * Reset the state of the writer. The writer will have to be initialized
	 * again before it can be used.
	 */
	inline void
	reset ()
	{
		destDirPath_.clear();
		destFileBase_.clear();
		sink_.close();
		outputPaths_.clear();
		maxOutRecords_ = std::numeric_limits<uint64_t>::max();
		curOutRecords_ = 0;
		header_.clear();
	}

	/**
	 * Configure the writer for writing data in the specified directory.
	 *
	 * \param destinationDirPath is the path of the directory to where files
	 * will be written.
	 * \param destinationFileBase is the basename of the files that will be
	 * written.
	 * \param header is the list of strings that will be printed as header for
	 * each written file.
	 * \param maxOutputRecords is the maximum number of records allowed to be
	 * written in a single output file.
	 */
	inline void
	configure (const fs::path& destinationDirPath,
	           const fs::path& destinationFileBase,
	           std::vector<sa::String<char>>& header,
	           uint64_t maxOutputRecords)
	{
		reset();
		destDirPath_   = destinationDirPath;
		destFileBase_  = destinationFileBase;
		maxOutRecords_ = maxOutputRecords;
		header_        = header;
	}

	/**
	 * Configure the writer for writing temporary data in the specified
	 * directory.
	 *
	 * \param destinationDirPath is the path of the directory to where files
	 * will be written.
	 * \param header is the list of strings that will be printed as header for
	 * each written file
	 */
	inline void
	configureTemporary (const fs::path& destinationDirPath,
	                    std::vector<sa::String<char>>& header)
	{
		reset();
		destDirPath_   = destinationDirPath;
		destFileBase_  = makeRandomFilename_();
		maxOutRecords_ = std::numeric_limits<uint64_t>::max();
		header_        = header;
	}

	template <typename TRecordIter>
	inline uint64_t
	write (TRecordIter itStart,
	       TRecordIter itEnd) noexcept
	{
		if (!sink_.is_open())
		{
			switchSink();
		}

		return write_(itStart,
		              itEnd);
	}

	/**
	 * Close the current sink file and switch to another one.
	 */
	inline void
	switchSink () noexcept
	{
		uint64_t newPathId = outputPaths_.size();

		curOutRecords_ = 0;
		sink_.close();
		outputPaths_.emplace_back(generateNewPath_(newPathId));
		sink_.open(outputPaths_.rbegin()->generic_string());
		writeHeader_();
	}

	/**
	 * Force the writer to flush the sink stream.
	 */
	inline void
	flush () noexcept
	{
		sink_.close();
		sink_.open(outputPaths_.rbegin()->generic_string(),
		           std::fstream::app);
	}

private:
	/**
	 * Directory path to where output files are stored.
	 */
	fs::path                      destDirPath_;
	/**
	 * Basename for the output files this writer will produce.
	 */
	fs::path                      destFileBase_;
	/**
	 * Vector of paths currently written by the current writer.
	 */
	std::vector<fs::path>         outputPaths_;
	/**
	 * Header used for every file written by the following writer.
	 */
	std::vector<sa::String<char>> header_;
	/**
	 * Maximum number of records the current writer will place in a single
	 * file.
	 */
	uint64_t                      maxOutRecords_;
	/**
	 * Current number of written records.
	 */
	uint64_t                      curOutRecords_;
	/**
	 * Stream object used for writing the records.
	 */
	std::ofstream                 sink_;

	/**
	 * Create a random an unambiguous filename.
	 *
	 * \return a string containing a random filename.
	 */
	inline fs::path
	makeRandomFilename_ ()
	{
		using SysClock = std::chrono::system_clock;
		using SysClockDur = SysClock::duration;

		SysClockDur sinceEpoch = SysClock::now().time_since_epoch();
		std::string rnd        = std::to_string(sinceEpoch.count());

		return fs::path(rnd);
	}

	/**
	 * Generate a new output file path.
	 *
	 * \param id is the identifier of the path to be generated.
	 * \return the path of the next file to be written.
	 */
	inline fs::path
	generateNewPath_ (uint64_t id) noexcept
	{
		fs::path fileExt = fs::path(".tab");
		fs::path newPath = destDirPath_ / destFileBase_;

		if (id > 0)
		{
			newPath += "_";
			newPath += std::to_string(id);
		}
		newPath += fileExt;

		return newPath;
	}

	/**
	 * Write the header at the beginning of a newly opened stream.
	 */
	inline void
	writeHeader_ () noexcept
	{
		sink_ << "Overall\t";
		for (auto i = 0u; i < sa::length(header_) - 1; i++)
		{
			sink_ << header_[i] << "\t";
		}
		sink_ << header_[sa::length(header_) - 1] << "\n";
	}

	template <typename TRecordIter>
	inline uint64_t
	write_ (TRecordIter itStart,
	        TRecordIter itEnd) noexcept
	{
		uint64_t printed = 0;

		for (auto it = itStart;
		     it != itEnd;
		     it++)
		{
			if (curOutRecords_ == maxOutRecords_)
			{
				switchSink();
			}
			it->write(sink_);
			curOutRecords_++;
			printed++;
		}

		return static_cast<uint64_t>(printed);
	}
};
} // bioseqzip

#endif // BIOSEQZIP_BIOSEQZIP_CORE_DETAILS_WRITER_H