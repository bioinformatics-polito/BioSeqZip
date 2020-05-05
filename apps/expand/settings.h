/**
 * \file      apps/expand/settings.h
 * \author    Gianvito Urgese
 * \author    Emanuele Parisi
 * \author    Orazio Scicolone
 * \date      November, 2018
 *
 * \copyright Copyright (C) 2018 Gianvito Urgese. All Rights Reserved.
 *
 * \brief
 */

#ifndef BIOSEQZIP_APPS_EXPAND_SETTINGS_H_
#define BIOSEQZIP_APPS_EXPAND_SETTINGS_H_

#include <iostream>
#include <string>

#include <seqan/arg_parse.h>
#include <seqan/seq_io.h>

#include "tag_io.h"

#include "input_format.h"

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
	fs::path    alignSourcePath;
	/**
	 * Path to the FASTA, FASTQ, TAG or TAGQ sample resulting from the
	 * bioseqzip-collapse procedure
	 */
	// fs::path    inputSamplePath;
	/**
	 * Path to the FASTA, FASTQ, TAG or TAGQ file containing the collapsed
	 * paired mates resulting from the bioseqzip-collapse procedure.
	 */
	// fs::path    pairedSamplePath;
	/**
	 * Flag for keeping track of the input format of the previously collapsed
	 * data.
	 */
	// InputFormat inputFormat;
	/**
	 * Path to the tabular file containing the details of the distribution of
	 * collapsed sequences among the samples collapsed.
	 */
	// fs::path    tabSourcePath;
	/**
	 * Path to the directory used for storing intermediate files used by the
	 * expand procedure.
	 */
	fs::path    temporaryDirPath;
	/**
	 * Path to the directory where expanded alignment files are stored.
	 */
	fs::path    outputDirPath;
	/**
	 * Basename of the output file containing the expanded alignment records.
	 */
	fs::path    outputBasename;
	/**
	 * Maximum amount of RAM the expand process is allowed to use.
	 */
	uint64_t    maxRam;
	/**
	 * Maximum number of threads the expand process is allowed to use.
	 */
	uint64_t    nThreads;
	/**
	 * Verbosity variable modulating the amount of information the expand
	 * process provides the used.
	 */
	uint64_t    verbosity;

	/**
	 * Class constructor.
	 */
	Settings ()
	{
		setupCommandLineParser_();
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
	inline sa::ArgumentParser::ParseResult
	parseCommandLine (int argc,
	                  char** argv)
	{
		sa::CharString                  tmpString;
		std::string                     errorMsg;
		sa::ArgumentParser::ParseResult parseResult = sa::parse(parser_,
		                                                        argc,
		                                                        argv);

		if (parseResult == sa::ArgumentParser::PARSE_OK)
		{
			/*
			 * Retrieve input settings.
			 */
			// Retrieve the path of the input file containing alignment records
			// to be expanded.
			sa::getArgumentValue(alignSourcePath,
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

	//		// Retrieve the path to the previously collapsed sample used for
	//		// alignment data expansion.
	//		if (sa::isSet(parser_,
	//		              "input"))
	//		{
	//			sa::getOptionValue(inputSamplePath,
	//			                   parser_,
	//			                   "input");
	//			if (!fs::is_regular_file(inputSamplePath))
	//			{
	//				errorMsg = "Input path is not a regular file";
	//				throw std::invalid_argument(errorMsg);
	//			}
	//			if (!inputSamplePath.is_absolute())
	//			{
	//				inputSamplePath = fs::current_path() / inputSamplePath;
	//			}
	//			setInputFormat_();
	//		}

	//		// Retrieve the path to the file containing the paired mates of the 
	//		// previously collapsed sample.
	//		if (sa::isSet(parser_,
	//		              "paired"))
	//		{
	//			sa::getOptionValue(pairedSamplePath,
	//			                   parser_,
	//			                   "paired");
	//			if (!sa::isSet(parser_,
	//					       "input")) {
	//				errorMsg = "Missing input path";
	//				throw std::invalid_argument(errorMsg);
	//			}
	//			if (!fs::is_regular_file(pairedSamplePath))
	//			{
	//				errorMsg = "Paired path is not a regular file";
	//				throw std::invalid_argument(errorMsg);
	//			}
	//			if (!pairedSamplePath.is_absolute())
	//			{
	//				pairedSamplePath = fs::current_path() / pairedSamplePath;
	//			}
	//		}
	//		else
	//		{
	//			pairedSamplePath = fs::path("");
	//		}

	//		// Retrieve the path to the tabular file used for multiple sample
	//		// expanding.
	//		if (sa::isSet(parser_,
	//		              "multi-samples-tab"))
	//		{
	//			sa::getOptionValue(tabSourcePath,
	//			                   parser_,
	//			                   "multi-samples-tab");
	//			if (!fs::is_regular_file(tabSourcePath))
	//			{
	//				errorMsg = "Tabular path is not a regular file";
	//				throw std::invalid_argument(errorMsg);
	//			}
	//			if (!tabSourcePath.is_absolute())
	//			{
	//				tabSourcePath = fs::current_path() / tabSourcePath;
	//			}
	//		}

			/*
			 * Retrieve output settings.
			 */
			// Retrieve the path to the temporary directory used for storing
			// intermediate files created during the expand process.
			sa::getOptionValue(temporaryDirPath,
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
			sa::getOptionValue(outputDirPath,
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
			if (sa::isSet(parser_,
			              "basename"))
			{
				sa::getOptionValue(outputBasename,
				                   parser_,
				                   "basename");
			}
			else
			{
				outputBasename = alignSourcePath.stem();
				outputBasename += ".expanded";
			}

			/*
			 * Retrieve expand settings.
			 */
			// The string representing the maximum amount of RAM the expand
			// process can use, expressed in Bytes.
			sa::getOptionValue(tmpString,
			                   parser_,
			                   "max-ram");
			maxRam = parseMaxRamString_(tmpString);

			/*
			 * Retrieve Miscellaneous settings.
			 */
			// Retrieve the number of threads available for expanding.
			sa::getOptionValue(nThreads,
			                   parser_,
			                   "threads");

			// Retrieve the verbosity level the user requests.
			sa::getOptionValue(verbosity,
			                   parser_,
			                   "verbosity");
		}

		return parseResult;
	}

private:
	/**
	 * Instance of seqan::ArgumentParser used for parsing the command line the
	 * user provides.
	 */
	sa::ArgumentParser parser_;

	/**
	 * Setup the argument parser engine that is used for command line parsing.
	 */
	inline void
	setupCommandLineParser_ () noexcept
	{
		std::vector<std::string> inputValidValues;

		/*
		 * Initialize the argument parser with tool metadata.
		 */
		sa::setAppName(parser_,
		               "bioseqzip-expand");
		sa::setShortDescription(parser_,
		                        "Bioseqzip expander tool.");
		sa::addDescription(parser_,
		                   "bioseqzip-expand is a tool for expanding standard "
		                   ".sam and .bam files obtained from the alignment of "
		                   "samples previously collapsed by "
		                   "bioseqzip-collapse.");
		sa::setCategory(parser_,
		                "bioseqzip tools suite");
		sa::setVersion(parser_,
		               BioSeqZip_VERSION);
		sa::setDate(parser_,
		            "2019");
		sa::addUsageLine(parser_,
		                 "<\\fIalignments file path\\fP> "
		                 "[\\fIoptions...\\fP]");

		/*
		 * Input settings section.
		 */
		sa::addSection(parser_,
		               "Input options");

		// Input SAM or BAM file containing the alignment records to be
		// expanded.
		sa::addArgument(parser_,
		                sa::ArgParseArgument(sa::ArgParseArgument::INPUT_FILE,
		                                     "ALIGNMENTS-FILE"));
		sa::setHelpText(parser_,
		                0,
		                "Path of the SAM or BAM file containing the alignments "
		                "to be expanded.");
		sa::setValidValues(parser_,
		                   0,
		                   sa::BamFileIn::getFileExtensions());

//		// Collapsed input sample, obtained as output of the bioseqzip-collapse
//		// tool.
//		sa::addOption(parser_,
//		              sa::ArgParseOption("i",
//		                                 "input",
//		                                 "Path to the collapsed sample "
//		                                 "obtained by the bioseqzip-collapse "
//		                                 "multiple samples collapsing "
//		                                 "procedure.",
//		                                 sa::ArgParseArgument::INPUT_FILE,
//		                                 "INPUT"));
//		for (const auto& ext : sa::SeqFileIn::getFileExtensions())
//		{
//			inputValidValues.push_back(ext);
//		}
//		for (const auto& ext : sa::TagFileIn::getFileExtensions())
//		{
//			inputValidValues.push_back(ext);
//		}
//		sa::setValidValues(parser_,
//		                   "input",
//		                   inputValidValues);
//
//		// Collapsed input sample, obtained as output of the bioseqzip-collapse
//		// tool.
//		sa::addOption(parser_,
//		              sa::ArgParseOption("p",
//		                                 "paired",
//		                                 "Path to the sample containing the"
//		                                 "collapsed paired mates obtained by "
//		                                 "the bioseqzip-collapse multiple "
//		                                 "samples collapsing procedure.",
//		                                 sa::ArgParseArgument::INPUT_FILE,
//		                                 "PAIRED"));
//		for (const auto& ext : sa::SeqFileIn::getFileExtensions())
//		{
//			inputValidValues.push_back(ext);
//		}
//		for (const auto& ext : sa::TagFileIn::getFileExtensions())
//		{
//			inputValidValues.push_back(ext);
//		}
//		sa::setValidValues(parser_,
//		                   "paired",
//		                   inputValidValues);
//
//		// Tab separated file produced by bioseqzip-collapse in case of
//		// multiple sample collapsing.
//		sa::addOption(parser_,
//		              sa::ArgParseOption("",
//		                                 "multi-samples-tab",
//		                                 "Path to the tabular file reporting "
//		                                 "how many times each unique sequence "
//		                                 "occurs in every samples collapsed. "
//		                                 "This is the file bioseqzip-collapse "
//		                                 "generates during multi-sample "
//		                                 "collapsing.",
//		                                 sa::ArgParseArgument::INPUT_FILE,
//		                                 "TAB"));
//		sa::setValidValues(parser_,
//		                   "multi-samples-tab",
//		                   ".tab");

		/*
		 * Output settings section.
		 */
		sa::addSection(parser_,
		               "Output options");

		// Path to the directory where temporary files will be stored.
		sa::addOption(parser_,
		              sa::ArgParseOption("",
		                                 "temporary-directory",
		                                 "Path of the directory where "
		                                 "temporary files are stored.",
		                                 sa::ArgParseArgument::OUTPUT_DIRECTORY,
		                                 "TMP-DIRECTORY"));
		sa::setDefaultValue(parser_,
		                    "temporary-directory",
		                    ".");

		// Path to the directory where the expanded alignment file will be
		// stored.
		sa::addOption(parser_,
		              sa::ArgParseOption("o",
		                                 "output-directory",
		                                 "Path of the directory where "
		                                 "expanded files are stored.",
		                                 sa::ArgParseArgument::OUTPUT_DIRECTORY,
		                                 "OUTPUT-DIRECTORY"));
		sa::setDefaultValue(parser_,
		                    "output-directory",
		                    ".");

		// Basename for the expanded output file.
		sa::addOption(parser_,
		              sa::ArgParseOption("b",
		                                 "basename",
		                                 "Basename of the output file. If it "
		                                 "is not set, the stem of the input "
		                                 "alignments file will be used.",
		                                 sa::ArgParseArgument::STRING,
		                                 "BASENAME"));

		/*
		 * Expand settings section.
		 */
		sa::addSection(parser_,
		               "Expand options");

		// Maximum amount of RAM the tool can use for expanding the input file.
		sa::addOption(parser_,
		              sa::ArgParseOption("m",
		                                 "max-ram",
		                                 "Maximum amount of RAM the expander "
		                                 "is allowed to use expressed in Bytes "
		                                 "or in a human-readable format. The "
		                                 "allowed suffixes are 'M' (Megabytes) "
		                                 "and 'G' (Gigabytes).",
		                                 sa::ArgParseArgument::STRING,
		                                 "MAX-RAM"));
		sa::setDefaultValue(parser_,
		                    "max-ram",
		                    "4G");

		/*
		 * Miscellaneous settings section.
		 */
		sa::addSection(parser_,
		               "Miscellaneous options");

		// Maximum number of working thread the tool can use.
		sa::addOption(parser_,
		              sa::ArgParseOption("t",
		                                 "threads",
		                                 "Number of working threads.",
		                                 sa::ArgParseArgument::INTEGER,
		                                 "N-THREADS"));
		sa::setMinValue(parser_,
		                "threads",
		                "1");
		sa::setDefaultValue(parser_,
		                    "threads",
		                    "1");

		// Verbosity level the tool will use when logging information to the
		// user.
		sa::addOption(parser_,
		              sa::ArgParseOption("v",
		                                 "verbosity",
		                                 "Regulate the amount of information "
		                                 "the tool prints during execution.",
		                                 sa::ArgParseArgument::INTEGER,
		                                 "VERBOSITY"));
		sa::setMinValue(parser_,
		                "verbosity",
		                "0");
		sa::setMaxValue(parser_,
		                "verbosity",
		                "4");
		sa::setDefaultValue(parser_,
		                    "verbosity",
		                    "2");
	}

	/**
	 * Set the collapsed sample input format looking at the extension of its
	 * path.
	 */
//	inline void
//	setInputFormat_ () noexcept
//	{
//		uint64_t extensionArraySize = 0;
//		bool     inputFormatFound   = false;
//
//		// Check whether the collapsed sample is a FASTA file.
//		extensionArraySize = sizeof(sa::FileExtensions<sa::Fasta>::VALUE);
//		for (auto i = 0;
//		     i < extensionArraySize / sizeof(char*) && !inputFormatFound;
//		     ++i)
//		{
//			std::string tmpExtension = sa::FileExtensions<sa::Fasta>::VALUE[i];
//
//			if (inputSamplePath.extension() == tmpExtension)
//			{
//				inputFormat      = InputFormat::FASTA;
//				inputFormatFound = true;
//			}
//		}
//
//		// Check whether the collapsed sample is a FASTQ file.
//		extensionArraySize = sizeof(sa::FileExtensions<sa::Fastq>::VALUE);
//		for (auto i = 0;
//		     i < extensionArraySize / sizeof(char*) && !inputFormatFound;
//		     ++i)
//		{
//			std::string tmpExtension = sa::FileExtensions<sa::Fastq>::VALUE[i];
//
//			if (inputSamplePath.extension() == tmpExtension)
//			{
//				inputFormat      = InputFormat::FASTQ;
//				inputFormatFound = true;
//			}
//		}
//
//		// Check whether the collapsed sample is a TAG file.
//		extensionArraySize = sizeof(sa::FileExtensions<sa::TAG>::VALUE);
//		for (auto i = 0;
//		     i < extensionArraySize / sizeof(char*) && !inputFormatFound;
//		     ++i)
//		{
//			std::string tmpExtension = sa::FileExtensions<sa::TAG>::VALUE[i];
//
//			if (inputSamplePath.extension() == tmpExtension)
//			{
//				inputFormat      = InputFormat::TAG;
//				inputFormatFound = true;
//			}
//		}
//
//		// Check whether the collapsed sample is a TAGQ file.
//		extensionArraySize = sizeof(sa::FileExtensions<sa::TAGQ>::VALUE);
//		for (auto i = 0;
//		     i < extensionArraySize / sizeof(char*) && !inputFormatFound;
//		     ++i)
//		{
//			std::string tmpExtension = sa::FileExtensions<sa::TAGQ>::VALUE[i];
//
//			if (inputSamplePath.extension() == tmpExtension)
//			{
//				inputFormat      = InputFormat::TAGQ;
//				inputFormatFound = true;
//			}
//		}
//	}

	/**
	 * Transform the human-readable string containing the maximum amount of RAM
	 * the expander can use into a number expressing the same amount in Bytes.
	 *
	 * \param maxRamString is the string containing the memory threshold to be
	 * interpreted.
	 * \return the maximum amount of RAM the expander can use expressed in
	 * Bytes.
	 */
	inline uint64_t
	parseMaxRamString_ (sa::CharString& maxRamString)
	{
		std::string stdString      = sa::toCString(maxRamString);
		std::string stdStringHigh  = std::string(stdString.begin(),
		                                         stdString.end() - 1);
		char        lastChar       = stdString.back();
		uint64_t    convertedValue = 0;

		// Check the upper string region contains a number.
		if (std::any_of(stdStringHigh.begin(),
		                stdStringHigh.end(),
		                ::isalpha))
		{
			throw std::invalid_argument("Not a convertible number");
		}

		// Perform cast.
		if (lastChar >= '0' && lastChar <= '9')
		{
			convertedValue = static_cast<uint64_t>(std::stoll(stdString));
		}
		else if (lastChar == 'M')
		{
			uint64_t MBMultiplyFactor = 1024ull * 1024ull;
			convertedValue = std::stoll(stdStringHigh) * MBMultiplyFactor;
		}
		else if (lastChar == 'G')
		{
			uint64_t GBMultiplyFactor = 1024ull * 1024ull * 1024ull;
			convertedValue = std::stoll(stdStringHigh) * GBMultiplyFactor;
		}
		else
		{
			throw std::invalid_argument("Last char for maximum RAM not valid. "
			                            "Allowed characters are 'M' and 'G'");
		}

		return convertedValue;
	}
};
} // expand
} // bioseqzip

#endif // BIOSEQZIP_APPS_EXPAND_SETTINGS_H_
