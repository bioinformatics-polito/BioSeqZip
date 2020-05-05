/**
 * \file      apps/collapse/settings.h
 * \author    Gianvito Urgese
 * \author    Emanuele Parisi
 * \date      November, 2018
 *
 * \copyright Copyright (C) 2018 Gianvito Urgese. All Rights Reserved.
 */

#ifndef BIOSEQZIP_APPS_COLLAPSE_SETTINGS_H
#define BIOSEQZIP_APPS_COLLAPSE_SETTINGS_H

#include <string>
#include <vector>

#include <seqan/arg_parse.h>
#include <seqan/seq_io.h>

#if BioSeqZip_APPLE
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

namespace bioseqzip
{
namespace collapse
{

class Settings
{
public:

	static constexpr uint8_t IN_LAYOUT_SS_SINGLE_END  = 0;
	static constexpr uint8_t IN_LAYOUT_SS_PAIRED_END  = 1;
	static constexpr uint8_t IN_LAYOUT_SS_INTERLEAVED = 2;
	static constexpr uint8_t IN_LAYOUT_SS_BREAKPOINT  = 3;
	static constexpr uint8_t IN_LAYOUT_MS_SINGLE_END  = 4;
	static constexpr uint8_t IN_LAYOUT_MS_PAIRED_END  = 5;
	static constexpr uint8_t IN_LAYOUT_MS_INTERLEAVED = 6;
	static constexpr uint8_t IN_LAYOUT_MS_BREAKPOINT  = 7;
	static constexpr uint8_t IN_LAYOUT_MS_MATCH_FILE  = 8;

	static constexpr uint8_t OUT_FORMAT_FASTA = 0;
	static constexpr uint8_t OUT_FORMAT_FASTQ = 1;
	static constexpr uint8_t OUT_FORMAT_TAG   = 2;
	static constexpr uint8_t OUT_FORMAT_TAGQ  = 3;

	// Input options.
	fs::path inputPath;
	fs::path inputDirPath;
	fs::path pairedPath;
	fs::path pairedDirPath;
	fs::path matchFilePath;
	bool     interleaved;
	uint64_t bpOffset;
	uint8_t  layout;

	// Output options.
	uint8_t  outputFormat;
	fs::path temporaryDirPath;
	fs::path outputDirPath;
	fs::path outputBasename;
	uint64_t maxOutputEntries;
	fs::path csvReportPath;
	bool     runGzip;

	// Collapse options.
	uint64_t maxRam;
	uint64_t trimLeft;
	uint64_t trimRight;

	// Miscellanous options.
	uint64_t nThreads;
	uint64_t verbosity;

	Settings ()
	{
		// Set tool meta-data.
		seqan::setAppName(parser_,
		                  "bioseqzip-collapse");
		seqan::setShortDescription(parser_,
		                           "Memory safe NGS dataset exact collapser.");
		seqan::addDescription(parser_,
		                      "bioseqzip-collapse is a tool for performing "
		                      "exact read collapsing over Next-Generation "
		                      "Sequencing datasets. It allows the user to fix "
		                      "the maximum amount of RAM the collapser is "
		                      "allowed to use and to perform sequence trimming "
		                      "when reading data.");
		seqan::setCategory(parser_,
		                   "bioseqzip tools suite");
		seqan::setVersion(parser_,
		                  BioSeqZip_VERSION);
		seqan::setDate(parser_,
		               "2019");

		// Define some useful usage lines.
		seqan::addUsageLine(parser_,
		                    "--input <\\fIpath\\fP> "
		                    "[\\fIoptions...\\fP]");
		seqan::addUsageLine(parser_,
		                    "--input <\\fImate-1 path\\fP> "
		                    "--paired <\\fImate-2 path\\fP> "
		                    "[\\fIoptions...\\fP]");
		seqan::addUsageLine(parser_,
		                    "--input <\\fIpath\\fP> "
		                    "--interleaved "
		                    "[\\fIoptions...\\fP]");
		seqan::addUsageLine(parser_,
		                    "--input <\\fIpath\\fP> "
		                    "--breakpoint <\\fIposition\\fP> "
		                    "[\\fIoptions...\\fP]");
		seqan::addUsageLine(parser_,
		                    "--input-dir <\\fIpath\\fP> "
		                    "[\\fIoptions...\\fP]");
		seqan::addUsageLine(parser_,
		                    "--input-dir <\\fImate-1 path\\fP> "
		                    "--paired-dir <\\fImate-2 path\\fP> "
		                    "[\\fIoptions...\\fP]");
		seqan::addUsageLine(parser_,
		                    "--input-dir <\\fIpath\\fP> "
		                    "--interleaved "
		                    "[\\fIoptions...\\fP]");
		seqan::addUsageLine(parser_,
		                    "--input-dir <\\fIpath\\fP> "
		                    "--breakpoint <\\fIposition\\fP> "
		                    "[\\fIoptions...\\fP]");
		seqan::addUsageLine(parser_,
		                    "--match-file <\\fIpath\\fP> "
		                    "[\\fIoptions...\\fP]");

		// Input settings section.
		seqan::addSection(parser_,
		                  "Input options");
		seqan::addOption(parser_,
		                 seqan::ArgParseOption("i",
		                                       "input",
		                                       "Path of the single sample to "
		                                       "be collapsed, or containing "
		                                       "the first mates for paired-end "
		                                       "samples.",
		                                       seqan::ArgParseArgument::INPUT_FILE,
		                                       "INPUT"));
		seqan::setValidValues(parser_,
		                      "input",
		                      seqan::SeqFileIn::getFileExtensions());

		seqan::addOption(parser_,
		                 seqan::ArgParseOption("",
		                                       "input-dir",
		                                       "Path of the directory storing "
		                                       "the samples to be collapsed, "
		                                       "or containing the first mates "
		                                       "for paired-end data.",
		                                       seqan::ArgParseArgument::INPUT_DIRECTORY,
		                                       "INPUT-DIR"));

		seqan::addOption(parser_,
		                 seqan::ArgParseOption("p",
		                                       "paired",
		                                       "Path of the dataset containing "
		                                       "the second mates for "
		                                       "paired-end data.",
		                                       seqan::ArgParseArgument::INPUT_FILE,
		                                       "PAIRED"));
		seqan::setValidValues(parser_,
		                      "paired",
		                      seqan::SeqFileIn::getFileExtensions());

		seqan::addOption(parser_,
		                 seqan::ArgParseOption("",
		                                       "paired-dir",
		                                       "Path of the directory storing "
		                                       "the second mates for paired "
		                                       "end data.",
		                                       seqan::ArgParseArgument::INPUT_DIRECTORY,
		                                       "PAIRED-DIR"));

		seqan::addOption(parser_,
		                 seqan::ArgParseOption("",
		                                       "match-file",
		                                       "Path of the tabular file "
		                                       "containing a pair of path for "
		                                       "each line, used  for tracking "
		                                       "paired samples.",
		                                       seqan::ArgParseArgument::STRING,
		                                       "MATCH-FILE"));

		seqan::addOption(parser_,
		                 seqan::ArgParseOption("",
		                                       "interleaved",
		                                       "Flag signaling that the input "
		                                       "dataset contains interleaved "
		                                       "sequences."));

		seqan::addOption(parser_,
		                 seqan::ArgParseOption("",
		                                       "breakpoint",
		                                       "Position in the input "
		                                       "sequences where read "
		                                       "breakpoint occurs.",
		                                       seqan::ArgParseArgument::INTEGER,
		                                       "BREAKPOINT-POSITION"));
		seqan::setMinValue(parser_,
		                   "breakpoint",
		                   "1");

		// Output settings section.
		std::vector<std::string> valid_output_formats;

		seqan::addSection(parser_,
		                  "Output options");
		seqan::addOption(parser_,
		                 seqan::ArgParseOption("f",
		                                       "format",
		                                       "Specify the output format for "
		                                       "the collapsed dataset.",
		                                       seqan::ArgParseArgument::STRING,
		                                       "FORMAT"));
		valid_output_formats.emplace_back("fasta");
		valid_output_formats.emplace_back("fastq");
		valid_output_formats.emplace_back("tag");
		valid_output_formats.emplace_back("tagq");
		seqan::setValidValues(parser_,
		                      "format",
		                      valid_output_formats);
		seqan::setRequired(parser_,
		                   "format",
		                   true);

		seqan::addOption(parser_,
		                 seqan::ArgParseOption("",
		                                       "tmp-directory",
		                                       "Path of the directory where "
		                                       "temporary files are stored.",
		                                       seqan::ArgParseArgument::STRING,
		                                       "TMP-DIRECTORY"));
		seqan::setDefaultValue(parser_,
		                       "tmp-directory",
		                       ".");

		seqan::addOption(parser_,
		                 seqan::ArgParseOption("o",
		                                       "output-directory",
		                                       "Path of the directory where "
		                                       "collapsed files are stored.",
		                                       seqan::ArgParseArgument::STRING,
		                                       "OUTPUT-DIRECTORY"));
		seqan::setDefaultValue(parser_,
		                       "output-directory",
		                       ".");

		seqan::addOption(parser_,
		                 seqan::ArgParseOption("b",
		                                       "basename",
		                                       "Basename of the output file. "
		                                       "If it is not set, the stem of "
		                                       "the input option will be used.",
		                                       seqan::ArgParseArgument::STRING,
		                                       "BASENAME"));

		seqan::addOption(parser_,
		                 seqan::ArgParseOption("c",
		                                       "chunk-size",
		                                       "Maximum number of sequences "
		                                       "stored in each output file.",
		                                       seqan::ArgParseArgument::INTEGER,
		                                       "CHUNK-SIZE"));
		seqan::setMinValue(parser_,
		                   "chunk-size",
		                   "1");

		seqan::addOption(parser_,
		                 seqan::ArgParseOption("",
		                                       "csv-report",
		                                       "CSV formatted output file "
		                                       "containing the details of the "
		                                       "collapse operations performed."));

		seqan::addOption(parser_,
		                 seqan::ArgParseOption("g",
		                                       "run-gzip",
		                                       "Run GZip compression on output files."));

		// Collapse settings section.
		seqan::addSection(parser_,
		                  "Collapse options");
		seqan::addOption(parser_,
		                 seqan::ArgParseOption("m",
		                                       "max-ram",
		                                       "Maximum amount of RAM the "
		                                       "collapser is allowed to use "
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

		seqan::addOption(parser_,
		                 seqan::ArgParseOption("",
		                                       "trim-left",
		                                       "Number of bases trimmed from "
		                                       "the left side of a sequence, "
		                                       "whenever it is read.",
		                                       seqan::ArgParseArgument::INTEGER,
		                                       "TRIM-LEFT"));
		seqan::setMinValue(parser_,
		                   "trim-left",
		                   "0");
		seqan::setDefaultValue(parser_,
		                       "trim-left",
		                       "0");

		seqan::addOption(parser_,
		                 seqan::ArgParseOption("",
		                                       "trim-right",
		                                       "Number of bases trimmed from "
		                                       "the right side of a sequence, "
		                                       "whenever it is read.",
		                                       seqan::ArgParseArgument::INTEGER,
		                                       "TRIM-RIGHT"));
		seqan::setMinValue(parser_,
		                   "trim-right",
		                   "0");
		seqan::setDefaultValue(parser_,
		                       "trim-right",
		                       "0");

		// Miscellaneous settings section.
		seqan::addSection(parser_,
		                  "Miscellaneous options");
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

	inline void
	setInputLayout ()
	{
		std::string errorMsg;

		bool inputSet       = isSet(parser_,
		                            "input");
		bool inputDirSet    = isSet(parser_,
		                            "input-dir");
		bool pairedSet      = isSet(parser_,
		                            "paired");
		bool pairedDirSet   = isSet(parser_,
		                            "paired-dir");
		bool interleavedSet = isSet(parser_,
		                            "interleaved");
		bool breakpointSet  = isSet(parser_,
		                            "breakpoint");
		bool matchFileSet   = isSet(parser_,
		                            "match-file");

		if (inputSet &&
		    !inputDirSet &&
		    !pairedSet &&
		    !pairedDirSet &&
		    !interleavedSet &&
		    !breakpointSet &&
		    !matchFileSet)
		{
			layout = IN_LAYOUT_SS_SINGLE_END;
		}
		else if (inputSet &&
		         !inputDirSet &&
		         pairedSet &&
		         !pairedDirSet &&
		         !interleavedSet &&
		         !breakpointSet &&
		         !matchFileSet)
		{
			layout = IN_LAYOUT_SS_PAIRED_END;
		}
		else if (inputSet &&
		         !inputDirSet &&
		         !pairedSet &&
		         !pairedDirSet &&
		         interleavedSet &&
		         !breakpointSet &&
		         !matchFileSet)
		{
			layout = IN_LAYOUT_SS_INTERLEAVED;
		}
		else if (inputSet &&
		         !inputDirSet &&
		         !pairedSet &&
		         !pairedDirSet &&
		         !interleavedSet &&
		         breakpointSet &&
		         !matchFileSet)
		{
			layout = IN_LAYOUT_SS_BREAKPOINT;
		}
		else if (!inputSet &&
		         inputDirSet &&
		         !pairedSet &&
		         !pairedDirSet &&
		         !interleavedSet &&
		         !breakpointSet &&
		         !matchFileSet)
		{
			layout = IN_LAYOUT_MS_SINGLE_END;
		}
		else if (!inputSet &&
		         inputDirSet &&
		         !pairedSet &&
		         pairedDirSet &&
		         !interleavedSet &&
		         !breakpointSet &&
		         !matchFileSet)
		{
			layout = IN_LAYOUT_MS_PAIRED_END;
		}
		else if (!inputSet &&
		         inputDirSet &&
		         !pairedSet &&
		         !pairedDirSet &&
		         interleavedSet &&
		         !breakpointSet &&
		         !matchFileSet)
		{
			layout = IN_LAYOUT_MS_INTERLEAVED;
		}
		else if (!inputSet &&
		         inputDirSet &&
		         !pairedSet &&
		         !pairedDirSet &&
		         !interleavedSet &&
		         breakpointSet &&
		         !matchFileSet)
		{
			layout = IN_LAYOUT_MS_BREAKPOINT;
		}
		else if (!inputSet &&
		         !inputDirSet &&
		         !pairedSet &&
		         !pairedDirSet &&
		         !interleavedSet &&
		         !breakpointSet &&
		         matchFileSet)
		{
			layout = IN_LAYOUT_MS_MATCH_FILE;
		}
		else
		{
			errorMsg = "Input parameters combination not recognised";
			throw std::invalid_argument(errorMsg);
		}
	}

	inline void
	setOutputFormat ()
	{
		std::string errorMsg;
		std::string outFormatStr;

		seqan::getOptionValue(outFormatStr,
		                      parser_,
		                      "format");
		if (outFormatStr == "fasta")
		{
			outputFormat = OUT_FORMAT_FASTA;
		}
		else if (outFormatStr == "fastq")
		{
			outputFormat = OUT_FORMAT_FASTQ;
		}
		else if (outFormatStr == "tag")
		{
			outputFormat = OUT_FORMAT_TAG;
		}
		else if (outFormatStr == "tagq")
		{
			outputFormat = OUT_FORMAT_TAGQ;
		}
		else
		{
			errorMsg = "Output format strong not recognised";
			throw std::invalid_argument(errorMsg);
		}
	}

	inline auto
	parseCommandLine (int argc,
	                  char** argv)
	{
		seqan::CharString                  tmpString;
		std::string                        errorMsg;
		seqan::ArgumentParser::ParseResult parseResult = seqan::parse(parser_,
		                                                              argc,
		                                                              argv);

		if (parseResult == seqan::ArgumentParser::PARSE_OK)
		{
			// Retrieve and validate input file.
			if (seqan::isSet(parser_,
			                 "input"))
			{
				seqan::getOptionValue(tmpString,
				                      parser_,
				                      "input");
				inputPath = fs::path(seqan::toCString(tmpString));
				if (!fs::is_regular_file(inputPath))
				{
					errorMsg = "Input path is not a regular file";
					throw std::invalid_argument(errorMsg);
				}
				if (!inputPath.is_absolute())
				{
					inputPath = fs::current_path() / inputPath;
				}
			}

			// Retrieve and validate input directory.
			if (seqan::isSet(parser_,
			                 "input-dir"))
			{
				seqan::getOptionValue(tmpString,
				                      parser_,
				                      "input-dir");
				inputDirPath = fs::path(seqan::toCString(tmpString));
				if (!fs::is_directory(inputDirPath))
				{
					errorMsg = "Input directory path does not exist";
					throw std::invalid_argument(errorMsg);
				}
				if (!inputDirPath.is_absolute())
				{
					inputDirPath = fs::current_path() / inputDirPath;
				}
			}

			// Retrieve and validate paired file, if it exists.
			if (seqan::isSet(parser_,
			                 "paired"))
			{
				seqan::getOptionValue(tmpString,
				                      parser_,
				                      "paired");
				pairedPath = fs::path(seqan::toCString(tmpString));
				if (!fs::is_regular_file(pairedPath))
				{
					errorMsg = "Paired path is not a regular file";
					throw std::invalid_argument(errorMsg);
				}
				if (!pairedPath.is_absolute())
				{
					pairedPath = fs::current_path() / pairedPath;
				}
			}

			// Retrieve and validate paired directory path, if it exists.
			if (seqan::isSet(parser_,
			                 "paired-dir"))
			{
				seqan::getOptionValue(tmpString,
				                      parser_,
				                      "paired-dir");
				pairedDirPath = fs::path(seqan::toCString(tmpString));
				if (!fs::is_directory(pairedDirPath))
				{
					errorMsg = "Paired directory path does not exist";
					throw std::invalid_argument(errorMsg);
				}
				if (!pairedDirPath.is_absolute())
				{
					pairedDirPath = fs::current_path() / pairedDirPath;
				}
			}

			// Retrieve and validate match file path, if it exists.
			if (seqan::isSet(parser_,
			                 "match-file"))
			{
				seqan::getOptionValue(tmpString,
				                      parser_,
				                      "match-file");
				matchFilePath = fs::path(seqan::toCString(tmpString));
				if (!fs::is_regular_file(matchFilePath))
				{
					errorMsg = "Match file path does not exist";
					throw std::invalid_argument(errorMsg);
				}
				if (!matchFilePath.is_absolute())
				{
					matchFilePath = fs::current_path() / matchFilePath;
				}
			}

			// Check if the dataset is interleaved.
			interleaved = seqan::isSet(parser_,
			                           "interleaved");

			// Check breakpoint value.
			if (seqan::isSet(parser_,
			                 "breakpoint"))
			{
				seqan::getOptionValue(bpOffset,
				                      parser_,
				                      "breakpoint");
			}

			// Set input database layout.
			setInputLayout();

			// Retrieve output format.
			setOutputFormat();

			// Retrieve and validate temporary directory.
			seqan::getOptionValue(tmpString,
			                      parser_,
			                      "tmp-directory");
			temporaryDirPath = fs::path(seqan::toCString(tmpString));
			if (!fs::is_directory(temporaryDirPath))
			{
				errorMsg = "Temporary directory path does not exists";
				throw std::invalid_argument(errorMsg);
			}

			// Retrieve and validate output directory.
			seqan::getOptionValue(tmpString,
			                      parser_,
			                      "output-directory");
			outputDirPath = fs::path(seqan::toCString(tmpString));
			if (!fs::is_directory(outputDirPath))
			{
				errorMsg = "Output directory path does not exists";
				throw std::invalid_argument(errorMsg);
			}

			// Retrieve output files basename from input basename.
			if (seqan::isSet(parser_,
			                 "basename"))
			{
				seqan::getOptionValue(tmpString,
				                      parser_,
				                      "basename");
				outputBasename = fs::path(seqan::toCString(tmpString));
			}
			else
			{
				if (seqan::isSet(parser_,
				                 "input"))
				{
					outputBasename = inputPath.stem();
				}
				else if (seqan::isSet(parser_,
						      "input-dir"))	
				{
					outputBasename = inputDirPath.stem();
				}
				else 
				{
					outputBasename = matchFilePath.stem();
				}
				outputBasename += ".collapsed";
			}

			// Get chunk size value.
			if (!seqan::isSet(parser_,
			                  "chunk-size"))
			{
				maxOutputEntries = std::numeric_limits<uint64_t>::max();
			}
			else
			{
				seqan::getOptionValue(maxOutputEntries,
				                      parser_,
				                      "chunk-size");
			}

			// Get the path to the output CSV report file.
			if (!seqan::isSet(parser_,
			                  "csv-report"))
			{
				csvReportPath = "";
			}
			else
			{
				csvReportPath = outputDirPath / outputBasename;
				csvReportPath += ".csv";
			}

			// Check if GZip output is requested.
			runGzip = false;
			if (seqan::isSet(parser_,
			                 "run-gzip"))
			{
				runGzip = true;
			}

			// Get maximum memory value.
			seqan::getOptionValue(tmpString,
			                      parser_,
			                      "max-ram");
			maxRam = parseMaxRamString(seqan::toCString(tmpString));

			// Get trim offsets.
			seqan::getOptionValue(trimLeft,
			                      parser_,
			                      "trim-left");
			seqan::getOptionValue(trimRight,
			                      parser_,
			                      "trim-right");

			// Get number of threads available.
			seqan::getOptionValue(nThreads,
			                      parser_,
			                      "threads");

			// Get verbosity level.
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
		if (layout == IN_LAYOUT_SS_SINGLE_END)
		{
			str += "\tInput layout        : single-sample, single-end\n";
			str += "\tInput path          : " +
			       inputPath.generic_string() +
			       "\n";
		}
		else if (layout == IN_LAYOUT_SS_PAIRED_END)
		{
			str += "\tInput layout        : single-sample, paired-end\n";
			str += "\tMate 1 path         : " + inputPath.generic_string() +
			       "\n";
			str += "\tMate 2 path         : " + pairedPath.generic_string() +
			       "\n";
		}
		else if (layout == IN_LAYOUT_SS_INTERLEAVED)
		{
			str += "\tInput layout        : single-sample, interleaved\n";
			str += "\tInput path          : " + inputPath.generic_string() +
			       "\n";
		}
		else if (layout == IN_LAYOUT_SS_BREAKPOINT)
		{
			str += "\tInput layout        : single-sample, breakpoint\n";
			str += "\tInput path          : " + inputPath.generic_string() +
			       "\n";
			str += "\tBreakpoint          : " + std::to_string(bpOffset) +
			       "\n";
		}
		else if (layout == IN_LAYOUT_MS_SINGLE_END)
		{
			str += "\tInput layout        : multi-samples, single-end\n";
			str += "\tInput directory     : " + inputDirPath.generic_string() +
			       "\n";
		}
		else if (layout == IN_LAYOUT_MS_PAIRED_END)
		{
			str += "\tInput layout        : multi-samples, paired-end\n";
			str += "\tMate 1 directory    : " + inputPath.generic_string() +
			       "\n";
			str += "\tMate 2 directory    : " + pairedPath.generic_string() +
			       "\n";
		}
		else if (layout == IN_LAYOUT_MS_INTERLEAVED)
		{
			str += "\tInput layout        : multi-samples, interleaved\n";
			str += "\tInput directory     : " + inputDirPath.generic_string() +
			       "\n";
		}
		else if (layout == IN_LAYOUT_MS_BREAKPOINT)
		{
			str += "\tInput layout        : multi-samples, breakpoint\n";
			str += "\tInput directory     : " + inputPath.generic_string() +
			       "\n";
			str += "\tBreakpoint          : " + std::to_string(bpOffset) +
			       "\n";
		}
		else
		{
			str += "\tInput layout        : multi-samples, match-file\n";
			str += "\tMatch file          : " + matchFilePath.generic_string() +
			       "\n";
		}

		// Report output configuration.
		str += "Output configuration\n";
		if (outputFormat == OUT_FORMAT_FASTQ)
		{
			str += "\tOutput format       : fastq\n";
		}
		else if (outputFormat == OUT_FORMAT_FASTA)
		{
			str += "\tOutput format       : fasta\n";
		}
		else if (outputFormat == OUT_FORMAT_TAGQ)
		{
			str += "\tOutput format       : tagq\n";
		}
		else
		{
			str += "\tOutput format       : tag\n";
		}
		str += "\tTemporary directory : " +
		       temporaryDirPath.generic_string() +
		       "\n";
		str += "\tOutput directory    : " +
		       outputDirPath.generic_string() +
		       "\n";
		str += "\tOutput basename     : " +
		       outputBasename.generic_string() +
		       "\n";
		str += "\tMax per-file reads  : " +
		       std::to_string(maxOutputEntries) +
		       "\n";
		str += "\tCSV report file     : " +
		       csvReportPath.generic_string() +
		       "\n";

		// Collapse configuration.
		str += "Collapse configuration\n";
		str += "\tMemory limit        : " +
		       std::to_string(maxRam) +
		       "\n";
		str += "\tTrim left           : " +
		       std::to_string(trimLeft) +
		       "\n";
		str += "\tTrim right          : " +
		       std::to_string(trimRight) +
		       "\n";
		str += "\tWorking threads     : " +
		       std::to_string(nThreads) +
		       "\n";

		return str;
	}

private:
	// Parser instance.
	seqan::ArgumentParser parser_;
};

} // collapse
} // bioseqzip

#endif // BIOSEQZIP_APPS_COLLAPSE_SETTINGS_H
