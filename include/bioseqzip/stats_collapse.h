/**
 * \file   bioseqzip/collapse_stats.h
 * \author Gianvito Urgese
 * \author Emanuele Parisi
 * \author Orazio Scicolone
 * \date   February, 2019
 */

#ifndef BIOSEQZIP_INCLUDE_BIOSEQZIP_STATS_COLLAPSE_H
#define BIOSEQZIP_INCLUDE_BIOSEQZIP_STATS_COLLAPSE_H

#include <string>
#include <tuple>
#include <vector>

namespace bioseqzip
{

/**
 * Class containing metadata about the outcome of a collapse procedure, such as
 * the collapse efficiency and the amount of units of time the procedure
 * required.
 */
class StatsCollapse
{
public:

	std::string sampleId;
	uint64_t    rawSequences;
	uint64_t    rawSpace;
	uint64_t    collapsedSequences;
	uint64_t    collapsedSpace;
	uint64_t    runtime;

	static inline void
	writeReport (const fs::path& outputPath,
	             const std::vector<StatsCollapse>& statistics) noexcept
	{
		std::ofstream outStream;

		outStream.open(outputPath);
		outStream << getHeaderString_() << std::endl;
		for (const auto& s : statistics)
		{
			outStream << s.toString(',') << std::endl;
		}
		outStream.close();
	}

	/**
     * Class constructor.
     */
	StatsCollapse () noexcept
	{
		reset();
	}

	inline void
	reset () noexcept
	{
		sampleId           = "";
		rawSequences       = 0ul;
		rawSpace           = 0ul;
		collapsedSequences = 0ul;
		collapsedSpace     = 0ul;
		runtime            = 0ul;
	}

	/**
	 * Create a string reporting the collapse statistics.
	 *
	 * \param separator is the character used as record field separator.
	 * \return a string containing the collapse statistics.
	 */
	inline std::string
	toString (char separator) const noexcept
	{
		auto        sequencesRatio = 1.0 * collapsedSequences / rawSequences;
		auto        sequencesGain  = 1.0 - sequencesRatio;
		auto        spaceRatio     = 1.0 * collapsedSpace / rawSpace;
		auto        spaceGain      = 1.0 - spaceRatio;
		std::string str            = sampleId + separator;

		str += std::to_string(rawSequences) + separator;
		str += std::to_string(collapsedSequences) + separator;
		str += std::to_string(sequencesGain) + separator;
		str += std::to_string(rawSpace) + separator;
		str += std::to_string(collapsedSpace) + separator;
		str += std::to_string(spaceGain) + separator;
		str += std::to_string(runtime);

		return str;
	}

private:

	static inline std::string
	getHeaderString_ () noexcept
	{
		static constexpr const char* header_[8] = {
				"SAMPLE ID",
				"RAW SEQUENCES",
				"COLLAPSED SEQUENCES",
				"SEQUENCES GAIN",
				"RAW SPACE",
				"COLLAPSED SPACE",
				"SPACE GAIN",
				"RUNTIME"
		};

		std::string str;

		for (auto i = 0ul; i < 7; i++)
		{
			str += header_[i];
			str += ',';
		}
		str += header_[7];

		return str;
	}
};

} // bioseqzip

#endif // BIOSEQZIP_INCLUDE_BIOSEQZIP_STATS_COLLAPSE_H
