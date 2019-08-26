/**
 * \file
 * \author Emanuele Parisi
 * \date January, 2019
 *
 * \brief
 */

#ifndef BIOSEQZIP_APPS_EXPAND_FUNCTIONS_H_
#define BIOSEQZIP_APPS_EXPAND_FUNCTIONS_H_

#include "cpputils/cpputils.h"

#include "bioseqzip/expander.h"

#include "settings.h"

namespace bz = bioseqzip;
namespace lg = cpputils;
namespace sw = cpputils;
namespace fs = std::experimental::filesystem;
namespace sa = seqan;

namespace bioseqzip
{
namespace expand
{

inline void
singleSamplePipeline (const Settings& settings,
                      lg::Logger<lg::SinkStdout>& logger) noexcept
{
	bz::Expander expander;

	// Configure the expander object with the setting passed by the user
	// through the command line interface.
	expander.configure(settings.outputDirPath,
	                   settings.temporaryDirPath,
	                   settings.maxRam,
	                   settings.nThreads);

	INFO(logger,
	     "Start single sample expand procedure\n");
	expander.expandSingleSample(settings.alignSourcePath,
	                            settings.outputBasename);
	INFO(logger,
	     "Expand finished\n");
}

// template <typename TInStream,
//           typename TTagRecord,
//           typename TOccRecord>
// inline void
// multiSamplesPipeline (const Settings& settings,
//                       lg::Logger<lg::SinkStdout>& logger) noexcept
// {
// 	bz::Expander expander;
// 
// 	// Configure the expander object with the setting passed by the user
// 	// through the command line interface.
// 	expander.configure(settings.outputDirPath,
// 	                   settings.temporaryDirPath,
// 	                   settings.maxRam,
// 	                   settings.nThreads);
// 
// 	INFO(logger,
// 	     "Start multiple samples expand procedure\n");
// 	if (settings.pairedSamplePath.empty())
// 	{
// 		expander.expandMultiSamples<TInStream,
// 		                            TTagRecord,
// 		                            TOccRecord>(settings.alignSourcePath,
// 		                                        settings.inputSamplePath,
// 		                                        settings.tabSourcePath);
// 	}
// 	else
// 	{
// 		expander.expandMultiSamples<TInStream,
// 		                            TTagRecord,
// 		                            TOccRecord>(settings.alignSourcePath,
// 		                                        settings.inputSamplePath,
// 		                                        settings.pairedSamplePath,
// 		                                        settings.tabSourcePath);
// 	}
// 	INFO(logger,
// 	     "Expand finished\n");
// }

} // expand
} // bioseqzip

#endif // BIOSEQZIP_APPS_EXPAND_FUNCTIONS_H_
