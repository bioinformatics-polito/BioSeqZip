#include <bioseqzip/core/sequence_record.h>
#include <bioseqzip/core/details_record.h>

#include "cpputils/cpputils.h"

#include "functions.h"
#include "settings.h"

int
main (int argc,
      char** argv)
{
	seqan::ArgumentParser::ParseResult     parseResult;
	bioseqzip::expand::Settings            settings;
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
	INFO(logger,
	     "Start expand procedure\n");
	CONFIG(logger,
	       settings.toString());

	// Launch the correct pipeline specialization, depending on input settings.
	expandPipeline(settings,
	               logger);
	INFO(logger,
	     "Finish\n\n");

	return 0;
}
