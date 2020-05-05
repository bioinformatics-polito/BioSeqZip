/**
 * \file      apps/expand/settings.h
 * \author    Gianvito Urgese
 * \author    Emanuele Parisi
 * \date      November, 2018
 *
 * \copyright Copyright (C) 2018 Gianvito Urgese. All Rights Reserved.
 *
 * \brief
 */

#ifndef BIOSEQZIP_APPS_EXPAND_SETTINGS_H
#define BIOSEQZIP_APPS_EXPAND_SETTINGS_H

#include <iostream>
#include <string>

#include <seqan/arg_parse.h>
#include <seqan/seq_io.h>

#include "tag_io.h"

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
namespace expand
{

/**
 * Class wrapping the parameters controlling the expand process and the SeqAn2
 * engine for command line argument parsing.
 */
class Settings
{
public:

	/**
	 * Path to the SAM or BAM file containing the alignment records to be
	 * expanded.
	 */
	fs::path alignSourcePath;
	/**
	 * Path to the tabular file containing the details of the distribution of
	 * collapsed sequences among the samples collapsed.
	 */
	fs::path tabSourcePath;
	/**
	 * Path to the directory used for storing intermediate files used by the
	 * expand procedure.
	 */
	fs::path temporaryDirPath;
	/**
	 * Path to the directory where expanded alignment files are stored.
	 */
	fs::path outputDirPath;
	/**
	 * Basename of the output file containing the expanded alignment records.
	 */
	fs::path outputBasename;
	/**
	 * Maximum amount of RAM the expand process is allowed to use.
	 */
	uint64_t maxRam;
	/**
	 * Maximum number of threads the expand process is allowed to use.
	 */
	uint64_t nThreads;
	/**
	 * Verbosity variable modulating the amount of information the expand
	 * process provides the used.
	 */
	uint64_t verbosity;

	/**
	 * Class constructor.
	 */
	Settings ()
	{
		std::vector<std::string> inputValidValues;

		/*
		 * Initialize the argument parser with tool metadata.
		 */
		seqan::setAppName(parser_,
		                  "bioseqzip-expand");
		seqan::setShortDescription(parser_,
		                           "Bioseqzip expander tool.");
		seqan::addDescription(parser_,
		                      "bioseqzip-expand is a tool for expanding "
		                      "standard .sam and .bam files obtained from the "
		                      "alignment of samples previously collapsed by "
		                      "bioseqzip-collapse.");
		seqan::setCategory(parser_,
		                   "bioseqzip tools suite");
		seqan::setVersion(parser_,
		                  BioSeqZip_VERSION);
		seqan::setDate(parser_,
		               "2019");
		seqan::addUsageLine(parser_,
		                    "<\\fIalignments file path\\fP> "
		                    "[\\fIoptions...\\fP]");

		/*
		 * Output settings section.
		 */
		seqan::addSection(parser_,
		                  "Output options");

		// Path to the directory where temporary files will be stored.
		seqan::addOption(parser_,
		                 seqan::ArgParseOption("",
		                                       "temporary-directory",
		                                       "Path of the directory where "
		                                       "temporary files are stored.",
		                                       seqan::ArgParseArgument::OUTPUT_DIRECTORY,
		                                       "TMP-DIRECTORY"));
		seqan::setDefaultValue(parser_,
		                       "temporary-directory",
		                       ".");

		// Path to the directory where the expanded alignment file will be
		// stored.
		seqan::addOption(parser_,
		                 seqan::ArgParseOption("o",
		                                       "output-directory",
		                                       "Path of the directory where "
		                                       "expanded files are stored.",
		                                       seqan::ArgParseArgument::OUTPUT_DIRECTORY,
		                                       "OUTPUT-DIRECTORY"));
		seqan::setDefaultValue(parser_,
		                       "output-directory",
		                       ".");

		// Basename for the expanded output file.
		seqan::addOption(parser_,
		                 seqan::ArgParseOption("b",
		                                       "basename",
		                                       "Basename of the output file. "
		                                       "If it is not set, the stem of "
		                                       "the input alignments file will "
		                                       "be used.",
		                                       seqan::ArgParseArgument::STRING,
		                                       "BASENAME"));

		/*
		 * Expand settings section.
		 */
		seqan::addSection(parser_,
		                  "Expand options");

		// Tab separated file produced by bioseqzip-collapse in case of
		// multiple sample collapsing.
		seqan::addOption(parser_,
		                 seqan::ArgParseOption("",
		                                       "multi-samples-tab",
		                                       "Path to the tabular file "
		                                       "reporting how many times each "
		                                       "unique sequence occurs in "
		                                       "every samples collapsed. This "
		                                       "is the file bioseqzip-collapse "
		                                       "generates during multi-sample "
		                                       "collapsing.",
		                                       seqan::ArgParseArgument::INPUT_FILE,
		                                       "TAB"));
		seqan::setValidValues(parser_,
		                      "multi-samples-tab",
		                      ".tab");

		// Maximum amount of RAM the tool can use for expanding the input file.
		seqan::addOption(parser_,
		                 seqan::ArgParseOption("m",
		                                       "max-ram",
		                                       "Maximum amount of RAM the "
		                                       "expander is allowed to use "
		                                       "expressed in Bytes or in a "
		                                       "human-readable format. The "
		                                       "allowed suffixes are 'M' "
		                                       "(Megabytes) and 'G' "
		                                       "(Gigabytes).",
		                                       seqan::ArgParseArgument::STRING,
		                                       "MAX-RAM"));
		seqan::setDefaultValue(parser_,
		                       "max-ram",
		                       "4G");

		/*
		 * Miscellaneous settings section.
		 */
		seqan::addSection(parser_,
		                  "Miscellaneous options");

		// Maximum number of working thread the tool can use.
		seqan::addOption(parser_,
		                 seqan::ArgParseOption("t",
		                                       "threads",
		                                       "Number of working threads.",
		                                       seqan::ArgParseArgument::INTEGER,
		                                       "N-THREADS"));
		seqan::setMinValue(parser_,
		                   "threads",
		                   "1");
		seqan::setDefaultValue(parser_,
		                       "threads",
		                       "1");

		// Verbosity level the tool will use when logging information to the
		// user.
		seqan::addOption(parser_,
		                 seqan::ArgParseOption("v",
		                                       "verbosity",
		                                       "Regulate the amount of "
		                                       "information the tool prints "
		                                       "during execution.",
		                                       seqan::ArgParseArgument::INTEGER,
		                                       "VERBOSITY"));
		seqan::setMinValue(parser_,
		                   "verbosity",
		                   "0");
		seqan::setMaxValue(parser_,
		                   "verbosity",
		                   "4");
		seqan::setDefaultValue(parser_,
		                       "verbosity",
		                       "2");
	}

	inline auto
	parseMaxRamString (std::string maxRamStr)
	{
		constexpr auto MBMultiplier = 1024ul * 1024ul;
		constexpr auto GBMultiplier = MBMultiplier * 1024ul;

		std::string value          = maxRamStr.substr(0,
		                                              maxRamStr.size() - 1);
		uint64_t    convertedValue = 0ul;

		// Check the upper string region contains a number.
		if (std::any_of(value.begin(),
		                value.end(),
		                ::isalpha))
		{
			throw std::invalid_argument("Not a convertible number");
		}

		// Perform cast.
		if (maxRamStr.back() >= '0' && maxRamStr.back() <= '9')
		{
			convertedValue = std::strtoul(maxRamStr.data(),
			                              nullptr,
			                              10);
		}
		else
		{
			switch (maxRamStr.back())
			{
				case 'M':
					convertedValue = std::strtoul(value.data(),
					                              nullptr,
					                              10) * MBMultiplier;
					break;
				case 'G':
					convertedValue = std::strtoul(value.data(),
					                              nullptr,
					                              10) * GBMultiplier;
					break;
				default:
					throw std::invalid_argument("Last char for maximum RAM not "
					                            "valid; allowed characters are "
					                            "'M' and 'G'");
			}
		}

		return convertedValue;
	}

	/**
	 * Trigger the command line arguments parsing.
	 *
	 * \param argc is the count of the arguments the user provides on the
	 * command line.
	 * \param argv is the set of arguments the user provides on the command
	 * line.
	 * \return a flag reporting the outcome of the argument parse procedure.
	 */
	inline seqan::ArgumentParser::ParseResult
	parseCommandLine (int argc,
	                  char** argv)
	{
		std::string                        tmpString;
		std::string                        errorMsg;
		seqan::ArgumentParser::ParseResult parseResult = seqan::parse(parser_,
		                                                              argc,
		                                                              argv);

		if (parseResult == seqan::ArgumentParser::PARSE_OK)
		{
			/*
			 * Retrieve input settings.
			 */
			// Retrieve the path of the input file containing alignment records
			// to be expanded.
			seqan::getArgumentValue(alignSourcePath,
			                        parser_,
			                        0);
			if (!fs::is_regular_file(alignSourcePath))
			{
				errorMsg = "Input path is not a regular file";
				throw std::invalid_argument(errorMsg);
			}
			if (!alignSourcePath.is_absolute())
			{
				alignSourcePath = fs::current_path() / alignSourcePath;
			}

			// Retrieve the path to the tabular file used for multiple sample
			// expanding.
			if (seqan::isSet(parser_,
			                 "multi-samples-tab"))
			{
				seqan::getOptionValue(tabSourcePath,
				                      parser_,
				                      "multi-samples-tab");
				if (!fs::is_regular_file(tabSourcePath))
				{
					errorMsg = "Tabular path is not a regular file";
					throw std::invalid_argument(errorMsg);
				}
				if (!tabSourcePath.is_absolute())
				{
					tabSourcePath = fs::current_path() / tabSourcePath;
				}
			}

			/*
			 * Retrieve output settings.
			 */
			// Retrieve the path to the temporary directory used for storing
			// intermediate files created during the expand process.
			seqan::getOptionValue(temporaryDirPath,
			                      parser_,
			                      "temporary-directory");
			if (!fs::is_directory(temporaryDirPath))
			{
				errorMsg = "Temporary directory path does not exist";
				throw std::invalid_argument(errorMsg);
			}
			if (!temporaryDirPath.is_absolute())
			{
				temporaryDirPath = fs::current_path() / temporaryDirPath;
			}

			// Retrieve the path to the output directory where output file will
			// be stored.
			seqan::getOptionValue(outputDirPath,
			                      parser_,
			                      "output-directory");
			if (!fs::is_directory(outputDirPath))
			{
				errorMsg = "Output directory path does not exists";
				throw std::invalid_argument(errorMsg);
			}
			if (!outputDirPath.is_absolute())
			{
				outputDirPath = fs::current_path() / outputDirPath;
			}

			// Retrieve output files basename from input basename.
			if (seqan::isSet(parser_,
			                 "basename"))
			{
				seqan::getOptionValue(outputBasename,
				                      parser_,
				                      "basename");
			}
			else
			{
				outputBasename = alignSourcePath.stem();
				outputBasename += ".collapsed";
			}

			/*
			 * Retrieve expand settings.
			 */
			// The string representing the maximum amount of RAM the expand
			// process can use, expressed in Bytes.
			seqan::getOptionValue(tmpString,
			                      parser_,
			                      "max-ram");
			maxRam = parseMaxRamString(tmpString);

			/*
			 * Retrieve Miscellaneous settings.
			 */
			// Retrieve the number of threads available for expanding.
			seqan::getOptionValue(nThreads,
			                      parser_,
			                      "threads");

			// Retrieve the verbosity level the user requests.
			seqan::getOptionValue(verbosity,
			                      parser_,
			                      "verbosity");
		}

		return parseResult;
	}

	inline std::string
	toString () noexcept
	{
		std::string str;

		// Report input configuration.
		str += "Input configuration\n";
		if (tabSourcePath.empty())
		{
			str += "\tLayout              : single-sample expand\n";
			str += "\tAlignment file      : " +
			       alignSourcePath.generic_string()
			       + "\n";
		}
		else
		{
			str += "\tLayout              : multi-samples expand\n";
			str += "\tAlignment file      : " +
			       alignSourcePath.generic_string()
			       + "\n";
			str += "\tMatch file          : " +
			       tabSourcePath.generic_string() +
			       "\n";
		}

		// Report output configuration.
		str += "Output configuration\n";
		str += "\tTemporary directory : " +
		       temporaryDirPath.generic_string() +
		       "\n";
		str += "\tOutput directory    : " +
		       outputDirPath.generic_string() +
		       "\n";
		str += "\tOutput basename     : " +
		       outputBasename.generic_string() +
		       "\n";

		// Expand configuration.
		str += "Expand configuration\n";
		str += "\tMemory limit        : " +
		       std::to_string(maxRam) +
		       "\n";

		return str;
	}

private:
	/**
	 * Instance of seqan::ArgumentParser used for parsing the command line the
	 * user provides.
	 */
	seqan::ArgumentParser parser_;
};
} // expand
} // bioseqzip

#endif // BIOSEQZIP_APPS_EXPAND_SETTINGS_H