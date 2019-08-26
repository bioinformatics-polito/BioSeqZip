/**
 * \file      include/bioseqzip/core/sequence_writer.h
 * \author    Gianvito Urgese
 * \author    Emanuele Parisi
 * \author    Orazio Scicolone
 * \date      November, 2018
 *
 * \copyright Copyright (C) 2019 Gianvito Urgese. All Rights Reserved.
 *
 * File containing facilities for storing tag records in fastx and tagx
 * formatted files.
 */

#ifndef BIOSEQZIP_BIOSEQZIP_CORE_SEQUENCE_WRITER_H
#define BIOSEQZIP_BIOSEQZIP_CORE_SEQUENCE_WRITER_H

#include <chrono>
#include <experimental/filesystem>
#include <seqan/basic.h>
#include <seqan/seq_io.h>
#include <string>
#include <tuple>
#include <vector>

#include "library_layout.h"
#include "tag_io.h"

namespace fs = std::experimental::filesystem;
namespace sa = seqan;

namespace bioseqzip
{

/**
 * Class in charge of writing one or more tag records to a proper file.
 *
 * \tparam TStream is the type of SeqAn2 stream deciding the format of the file
 * data are written into.
 * \tparam TRecord is the kind of record written.
 */
template <typename TStream,
          typename TRecord>
class SequenceWriter
{
public:

	/**
	 * Class constructor.
	 */
	SequenceWriter () noexcept
	{
		layout_               = LibraryLayout::SINGLE_END;
		currentSequenceId_    = 0;
		maxOutputRecords_     = std::numeric_limits<uint64_t>::max();
		currentOutputRecords_ = 0;
		writeQualities_       = false;
	}

	/**
	 * Getter method for retrieving the file paths written by the writer.
	 *
	 * \return a list of file paths pairs for each written file.
	 */
	inline std::vector<std::pair<fs::path,
	                             fs::path>>&
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
		layout_ = LibraryLayout::SINGLE_END;
		destinationDirPath_.clear();
		destinationFileBase_.clear();
		sa::close(sinkMate1_);
		sa::close(sinkMate2_);
		outputPaths_.clear();
		currentSequenceId_    = 0;
		maxOutputRecords_     = std::numeric_limits<uint64_t>::max();
		currentOutputRecords_ = 0;
		writeQualities_       = false;
	}

	/**
	 * Configure the writer for writing data in a given layout to the specified
	 * directory.
	 *
	 * \param type is the layout of file the writer will produce.
	 * \param destinationDirPath is the path of the directory to where files
	 * will be written.
	 * \param destinationFileBase is the basename of the files that will be
	 * written.
	 * \param maxOutputRecords is the maximum number of records allowed to be
	 * written in a single output file.
	 */
	inline void
	configure (LibraryLayout type,
	           const fs::path& destinationDirPath,
	           const fs::path& destinationFileBase,
	           uint64_t maxOutputRecords)
	{
		reset();
		layout_              = type;
		destinationDirPath_  = destinationDirPath;
		destinationFileBase_ = destinationFileBase;
		maxOutputRecords_    = maxOutputRecords;
	}

	/**
	 * Configure the writer for writing temporary data in a given layout to the
	 * specified directory.
	 *
	 * \param type is the layout of file the writer will produce.
	 * \param destinationDirPath is the path of the directory to where files
	 * will be written.
	 */
	inline void
	configureTemporary (LibraryLayout type,
	                    const fs::path& destinationDirPath)
	{
		reset();
		layout_              = type;
		destinationDirPath_  = destinationDirPath;
		destinationFileBase_ = makeRandomFilename_();
		maxOutputRecords_    = std::numeric_limits<uint64_t>::max();
	}

	template <typename TRecordIter>
	inline uint64_t
	write (TRecordIter itStart,
	       TRecordIter itEnd) noexcept
	{
		uint64_t printed = 0;

		if (!sa::isOpen(sinkMate1_))
		{
			writeQualities_ = seqan::length(itStart->getQualities()) > 0;
			switchSink();
		}

		if (layout_ == LibraryLayout::SINGLE_END)
		{
			printed = writeSingleEnd_(itStart,
			                          itEnd,
			                          [] (const auto& record)
			                          {
				                          return true;
			                          });
		}
		else
		{
			printed = writePairedEnd_(itStart,
			                          itEnd,
			                          [] (const auto& record)
			                          {
				                          return true;
			                          });
		}

		return printed;
	}

	template <typename TRecordIter,
	          typename TValidator>
	inline uint64_t
	writeIf (TRecordIter itStart,
	         TRecordIter itEnd,
	         TValidator validator) noexcept
	{
		uint64_t printed = 0;

		if (!sa::isOpen(sinkMate1_))
		{
			writeQualities_ = seqan::length(itStart->getQualities()) > 0;
			switchSink();
		}

		if (layout_ == LibraryLayout::SINGLE_END)
		{
			printed = writeSingleEnd_(itStart,
			                          itEnd,
			                          validator);
		}
		else
		{
			printed = writePairedEnd_(itStart,
			                          itEnd,
			                          validator);
		}

		return printed;
	}

	/**
	 * Close the current sink file and switch to another one.
	 */
	inline void
	switchSink () noexcept
	{
		uint64_t newPathId = outputPaths_.size();

		currentOutputRecords_ = 0;
		if (layout_ == LibraryLayout::SINGLE_END)
		{
			sa::close(sinkMate1_);
			outputPaths_.emplace_back(generateNewPath_(newPathId,
			                                           ""),
			                          fs::path(""));
			sa::open(sinkMate1_,
			         outputPaths_.back().first.generic_string().data());
		}
		else
		{
			sa::close(sinkMate1_);
			sa::close(sinkMate2_);
			outputPaths_.emplace_back(generateNewPath_(newPathId,
			                                           "_1"),
			                          generateNewPath_(newPathId,
			                                           "_2"));
			sa::open(sinkMate1_,
			         outputPaths_.back().first.generic_string().data());
			sa::open(sinkMate2_,
			         outputPaths_.back().second.generic_string().data());
		}
	}

	/**
	 * Force the writer to flush the sink stream.
	 */
	inline void
	flush () noexcept
	{
		sa::close(sinkMate1_);
		sa::open(sinkMate1_,
		         outputPaths_.back().first.generic_string().data(),
		         sa::OPEN_APPEND);
		if (layout_ == LibraryLayout::PAIRED_END)
		{
			sa::close(sinkMate2_);
			sa::open(sinkMate2_,
			         outputPaths_.back().second.generic_string().data(),
			         sa::OPEN_APPEND);
		}
	}

private:

	/**
	 * Layout of the file to be written.
	 */
	LibraryLayout                    layout_;
	/**
	 * Directory path where generated files will be stored.
	 */
	fs::path                         destinationDirPath_;
	/**
	 * String used as basename for every file created by the writer.
	 */
	fs::path                         destinationFileBase_;
	/**
	 * Vector of pairs containing the paths to the samples written. For
	 * single-end samples the second field is an empty path.
	 */
	std::vector<std::pair<fs::path,
	                      fs::path>> outputPaths_;
	/**
	 * Unambiguous numeric identifier for the next sequence that will be
	 * written.
	 */
	uint64_t                         currentSequenceId_;
	/**
	 * Maximum number of records allowed in a single file. Once the threshold is
	 * exceeded the current file is closed and a new one is created.
	 */
	uint64_t                         maxOutputRecords_;
	/**
	 * Number of records written in the currently opened file.
	 */
	uint64_t                         currentOutputRecords_;
	/**
	 * Sink stream data to be written are sent to.
	 */
	TStream                          sinkMate1_;
	/**
	 * Sink stream second mates of data to be written are sent to. For
	 * single-end sample the stream remains uninitialized.
	 */
	TStream                          sinkMate2_;
	/**
	 * Flag controlling whether qualities have to be written or not.
	 */
	bool                             writeQualities_;

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
	 * Get the file output extension depending on the type of the output sink
	 * stream.
	 *
	 * \return a string representing the output files extension.
	 */
	inline fs::path
	getExtension_ () const noexcept
	{
		if (std::is_same<TStream,
		                 sa::SeqFileOut>::value)
		{
			if (writeQualities_)
			{
				return fs::path(sa::FileExtensions<sa::Fastq>::VALUE[0]);
			}
			return fs::path(sa::FileExtensions<sa::Fasta>::VALUE[0]);
		}
		else
		{
			if (writeQualities_)
			{
				return fs::path(sa::FileExtensions<sa::TAGQ>::VALUE[0]);
			}
			return fs::path(sa::FileExtensions<sa::TAG>::VALUE[0]);
		}
	}

	/**
	 * Generate a new output file path.
	 *
	 * \param id is the identifier of the path to be generated.
	 * \param pathSuffix is the string to be appended to the path identifier.
	 * \return the path of the next file to be written.
	 */
	inline fs::path
	generateNewPath_ (uint64_t id,
	                  const char* pathSuffix) noexcept
	{
		fs::path fileExt = getExtension_();
		fs::path newPath = destinationDirPath_ / destinationFileBase_;

		if (id > 0)
		{
			newPath += "_";
			newPath += std::to_string(id);
		}
		newPath += pathSuffix;
		newPath += fileExt;

		return newPath;
	}

	template <typename TRecordIter,
	          typename TValidator>
	inline uint64_t
	writeSingleEnd_ (TRecordIter itStart,
	                 TRecordIter itEnd,
	                 TValidator validator) noexcept
	{
		uint64_t printed = 0;

		for (auto it = itStart;
		     it != itEnd;
		     it++)
		{
			if (validator(*it))
			{
				if (currentOutputRecords_ == maxOutputRecords_)
				{
					switchSink();
				}
				it->write(sinkMate1_,
				          currentSequenceId_);
				currentOutputRecords_++;
				currentSequenceId_++;
				printed++;
			}
		}

		return static_cast<uint64_t>(printed);
	}

	template <typename TRecordIter,
	          typename TValidator>
	inline uint64_t
	writePairedEnd_ (TRecordIter itStart,
	                 TRecordIter itEnd,
	                 TValidator validator) noexcept
	{
		uint64_t printed = 0;

		for (auto it = itStart;
		     it != itEnd;
		     it++)
		{
			if (validator(*it))
			{
				if (currentOutputRecords_ == maxOutputRecords_)
				{
					switchSink();
				}
				it->write(sinkMate1_,
				          sinkMate2_,
				          currentSequenceId_);
				currentOutputRecords_++;
				currentSequenceId_++;
				printed++;
			}
		}

		return static_cast<uint64_t>(printed);
	}
};
} // bioseqzip

#endif // BIOSEQZIP_BIOSEQZIP_CORE_SEQUENCE_WRITER_H