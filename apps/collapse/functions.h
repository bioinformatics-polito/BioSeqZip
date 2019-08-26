/**
 * \file      apps/collapse/functions.h
 * \author    Gianvito Urgese
 * \author    Emanuele Parisi
 * \date      November, 2018
 *
 * \copyright Copyright (C) 2018 Gianvito Urgese. All Rights Reserved.
 */

#ifndef BIOSEQZIP_APPS_COLLAPSE_FUNCTIONS_H
#define BIOSEQZIP_APPS_COLLAPSE_FUNCTIONS_H

#include "cpputils/cpputils.h"

#include "bioseqzip/collapser.h"
#include "bioseqzip/results_collapse.h"
#include "bioseqzip/stats_collapse.h"

#include "settings.h"
#include "sample_tracker.h"

namespace fs = std::experimental::filesystem;

namespace bioseqzip {
    namespace collapse {

        template<typename TOutStream,
                typename TNoQuals,
                typename TSeqRecord,
                typename TTabRecord>
        inline void
        collapsePipeline(const Settings &settings,
                         cpputils::Logger<cpputils::SinkStdout> &logger) noexcept {
            Collapser core;
            ResultsCollapse results;

            // Configure the collapser with the arguments the user specified through
            // the command line.
            core.configure(settings.outputDirPath,
                           settings.temporaryDirPath,
                           settings.maxOutputEntries,
                           settings.maxRam,
                           settings.trimLeft,
                           settings.trimRight,
                           settings.nThreads);

            if (settings.layout == Settings::IN_LAYOUT_SS_SINGLE_END) {
                results = core.collapseSingleEnd<TSeqRecord,
                        TOutStream,
                        TNoQuals>(settings.inputPath,
                                  settings.outputBasename);
            } else if (settings.layout == Settings::IN_LAYOUT_SS_PAIRED_END) {
                results = core.collapsePairedEnd<TSeqRecord,
                        TOutStream,
                        TNoQuals>(settings.inputPath,
                                  settings.pairedPath,
                                  settings.outputBasename);
            } else if (settings.layout == Settings::IN_LAYOUT_SS_INTERLEAVED) {
                results = core.collapseInterleaved<TSeqRecord,
                        TOutStream,
                        TNoQuals>(settings.inputPath,
                                  settings.outputBasename);
            } else if (settings.layout == Settings::IN_LAYOUT_SS_BREAKPOINT) {
                results = core.collapseBreakpoint<TSeqRecord,
                        TOutStream,
                        TNoQuals>(settings.inputPath,
                                  settings.bpOffset,
                                  settings.outputBasename);
            } else {
                SampleTracker tracker;
                std::vector<std::string> validExtensions;
                std::vector<fs::path> samplesPaths;

                // Decide the valid extensions used for the collapse procedure.
                if (settings.outputFormat == Settings::OUT_FORMAT_FASTQ ||
                    settings.outputFormat == Settings::OUT_FORMAT_TAGQ) {
                    for (const auto e : seqan::FileExtensions<seqan::Fastq>::VALUE) {
                        validExtensions.emplace_back(e);
                    }
                } else {
                    for (const auto e : seqan::FileExtensions<seqan::Fasta>::VALUE) {
                        validExtensions.emplace_back(e);
                    }
                }

                if (settings.layout == Settings::IN_LAYOUT_MS_SINGLE_END) {
                    tracker.trackSingleSamples(settings.inputDirPath,
                                               validExtensions,
                                               samplesPaths);
                    results = core.collapseSingleEnd<TSeqRecord,
                                                     TTabRecord,
                                                     TOutStream,
                                                     TNoQuals>(samplesPaths,
                                                               settings.outputBasename);
                } else if (settings.layout ==
                           Settings::IN_LAYOUT_MS_PAIRED_END) {
                    std::vector<fs::path> pairedPaths;

                    tracker.trackPairedSamples(settings.inputDirPath,
                                               settings.pairedDirPath,
                                               validExtensions,
                                               samplesPaths,
                                               pairedPaths);
                    results = core.collapsePairedEnd<TSeqRecord,
                                                     TTabRecord,
                                                     TOutStream,
                                                     TNoQuals>(samplesPaths,
                                                               pairedPaths,
                                                               settings.outputBasename);
                } else if (settings.layout ==
                           Settings::IN_LAYOUT_MS_INTERLEAVED) {
                    tracker.trackSingleSamples(settings.inputDirPath,
                                               validExtensions,
                                               samplesPaths);

                } else if (settings.layout ==
                           Settings::IN_LAYOUT_MS_BREAKPOINT) {
                    tracker.trackSingleSamples(settings.inputDirPath,
                                               validExtensions,
                                               samplesPaths);
                } else {
                    std::vector<fs::path> pairedPaths;

                    tracker.trackMatchFileSamples(settings.matchFilePath,
                                                  samplesPaths,
                                                  pairedPaths);
                    results = core.collapsePairedEnd<TSeqRecord,
                                                     TTabRecord,
                                                     TOutStream,
                                                     TNoQuals>(samplesPaths,
                                                               pairedPaths,
                                                               settings.outputBasename);
                }
            }

            // If the user requested a CSV report, create it.
            if (!settings.csvReportPath.empty()) {
                CONFIG(logger,
                       "Write csv report file\n");
                StatsCollapse::writeReport(settings.csvReportPath,
                                           results.statistics);
            }

            // Print the user the list of collapsed files.
            INFO(logger,
                 std::string("Collapsed procedure output ")
                         .append(std::to_string(results.collapsedPaths.size()))
                         .append(" samples\n"));
            for (const auto &p : results.collapsedPaths) {
                CONFIG(logger,
                       std::string("\t").append(p.first.generic_string())
                               .append("\n"));
                if (!p.second.empty()) {
                    CONFIG(logger,
                           std::string("\t").append(p.second.generic_string())
                                   .append("\n"));
                }
            }
        }

    } // collapse
} // bioseqzip

#endif // BIOSEQZIP_APPS_COLLAPSE_FUNCTIONS_H
