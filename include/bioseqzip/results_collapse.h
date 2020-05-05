/**
 * \file   bioseqzip/results_collapse.h
 * \author Gianvito Urgese
 * \author Emanuele Parisi
 * \date   February, 2019
 */

#ifndef BIOSEQZIP_INCLUDE_BIOSEQZIP_RESULTS_COLLAPSE_H
#define BIOSEQZIP_INCLUDE_BIOSEQZIP_RESULTS_COLLAPSE_H

#include <tuple>
#include <vector>

#include "bioseqzip/stats_collapse.h"

#if BioSeqZip_APPLE
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

namespace bioseqzip
{

class ResultsCollapse
{
public:

	std::vector<std::pair<fs::path,
	                      fs::path>> collapsedPaths;
	std::vector<fs::path>            tabPaths;
	std::vector<StatsCollapse>       statistics;

	ResultsCollapse () = default;

	ResultsCollapse (const std::vector<std::pair<fs::path,
	                                             fs::path>>& collapsed,
	                 const std::vector<fs::path>& tabs,
	                 const std::vector<StatsCollapse>& stats) noexcept
	{
		collapsedPaths = collapsed;
		tabPaths       = tabs;
		statistics     = stats;
	}

	ResultsCollapse (std::vector<std::pair<fs::path,
	                                       fs::path>>&& collapsed,
	                 std::vector<fs::path>&& tabs,
	                 std::vector<StatsCollapse>&& stats) noexcept
	{
		collapsedPaths = std::move(collapsed);
		tabPaths       = std::move(tabs);
		statistics     = std::move(stats);
	}
};

}

#endif // BIOSEQZIP_INCLUDE_BIOSEQZIP_RESULTS_COLLAPSE_H
