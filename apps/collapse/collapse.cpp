#include <bioseqzip/core/sequence_record.h>
#include <bioseqzip/core/details_record.h>

#include "cpputils/cpputils.h"

#include "functions.h"
#include "settings.h"

int
main (int argc,
      char** argv)
{
	using TSequence = seqan::String<seqan::Iupac>;
	using TQuality = seqan::String<char>;
	using TCounter = uint32_t;
	using TBpOffset = uint8_t;
	using TSequenceRecord = bioseqzip::SequenceRecord<TSequence,
	                                                  TQuality,
	                                                  TCounter,
	                                                  TBpOffset>;
	using TDetailsRecord = bioseqzip::DetailsRecord<TCounter>;

	seqan::ArgumentParser::ParseResult     parseResult;
	bioseqzip::collapse::Settings          settings;
	cpputils::Logger<cpputils::SinkStdout> logger;

	// Set preliminary logger verbosity.
	try
	{
		parseResult = settings.parseCommandLine(argc,
		                                        argv);
		if (parseResult != seqan::ArgumentParser::PARSE_OK)
		{
			return 0;
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

	// Adjust logger verbosity and report command line options.
	logger.setPriority(static_cast<cpputils::Priority>(settings.verbosity));
	INFO(logger, "Start collapse procedure\n");
	CONFIG(logger,
	       settings.toString());

	// Launch the correct pipeline specialization, depending on input settings.
	if (settings.outputFormat ==
	    bioseqzip::collapse::Settings::OUT_FORMAT_FASTQ)
	{
		bioseqzip::collapse::collapsePipeline<seqan::SeqFileOut,
		                                      std::false_type,
		                                      TSequenceRecord,
		                                      TDetailsRecord>(settings,
		                                                      logger);
	}
	else if (settings.outputFormat ==
	         bioseqzip::collapse::Settings::OUT_FORMAT_FASTA)
	{
		bioseqzip::collapse::collapsePipeline<seqan::SeqFileOut,
		                                      std::true_type,
		                                      TSequenceRecord,
		                                      TDetailsRecord>(settings,
		                                                      logger);
	}
	else if (settings.outputFormat ==
	         bioseqzip::collapse::Settings::OUT_FORMAT_TAGQ)
	{
		bioseqzip::collapse::collapsePipeline<seqan::TagFileOut,
		                                      std::false_type,
		                                      TSequenceRecord,
		                                      TDetailsRecord>(settings,
		                                                      logger);
	}
	else
	{
		bioseqzip::collapse::collapsePipeline<seqan::TagFileOut,
		                                      std::true_type,
		                                      TSequenceRecord,
		                                      TDetailsRecord>(settings,
		                                                      logger);
	}
	INFO(logger, "Finish\n\n");

	return 0;
}
