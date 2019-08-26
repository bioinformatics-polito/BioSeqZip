#include "cpputils/cpputils.h"

#include "bioseqzip/core/details_record.h"
#include "bioseqzip/core/sequence_record.h"

#include "functions.h"

using namespace bioseqzip::expand;

namespace bz = bioseqzip;
namespace lg = cpputils;
namespace sa = seqan;

// using TTagRecord = bz::core::TagRecord<sa::String<sa::Dna5>,
//                                        sa::String<char>,
//                                        uint32_t,
//                                        uint16_t>;
// using TOccurrencesRecord = bz::core::DetailsRecord<uint32_t>;

int
main (int argc,
      char** argv)
{
	sa::ArgumentParser::ParseResult parseResult;
	Settings                        settings;
	lg::Logger<lg::SinkStdout>      logger;

	// Set preliminary logger verbosity.
	try
	{
		parseResult = settings.parseCommandLine(argc,
		                                        argv);
		// Adjust logger verbosity.
		logger.setPriority(static_cast<lg::Priority>(settings.verbosity));
		if (parseResult != sa::ArgumentParser::PARSE_OK)
		{
			return 0;
		}
		else
		{
			singleSamplePipeline(settings, 
					     logger);
			// if (settings.tabSourcePath.empty())
			// {
			// 	singleSamplePipeline(settings,
			// 	                     logger);
			// }
			// else
			// {
			// 	if (settings.inputFormat == InputFormat::FASTA ||
			// 	    settings.inputFormat == InputFormat::FASTQ)
			// 	{
			// 		multiSamplesPipeline<sa::SeqFileIn,
			// 		                     TTagRecord,
			// 		                     TOccurrencesRecord>(settings,
			// 		                                         logger);
			// 	}
			// 	else
			// 	{
			// 		multiSamplesPipeline<sa::TagFileIn,
			// 		                     TTagRecord,
			// 		                     TOccurrencesRecord>(settings,
			// 		                                         logger);
			// 	}
			// }
		}
	}
	catch (std::exception& e)
	{
		ERROR(logger,
		      "ERROR: ");
		ERROR(logger,
		      e.what());
		ERROR(logger,
		      "\n");
		return -1;
	}
	INFO(logger,
	     "\nFinish.\n");

	return 0;
}
