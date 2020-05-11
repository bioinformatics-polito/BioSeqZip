/**
 * \file      include/bioseqzip/sample_tracker.h
 * \author    Gianvito Urgese
 * \author    Emanuele Parisi
 * \author    Orazio Scicolone
 * \date      February, 2019
 *
 * \copyright Copyright (C) 2019 Gianvito Urgese. All Rights Reserved.
 */


#ifndef BIOSEQZIP_BIOSEQZIP_SAMPLE_TRACKER_H
#define BIOSEQZIP_BIOSEQZIP_SAMPLE_TRACKER_H

#include <fstream>
#include <string>
#include <vector>

namespace sa = seqan;

namespace bioseqzip
{

class SampleTracker
{
public:
	SampleTracker () noexcept = default;

	SampleTracker (const SampleTracker& other) = default;

	SampleTracker&
	operator= (const SampleTracker& other) = default;

	inline void
	trackSingleSamples (const fs::path& samplesDirPath,
	                    const std::vector<std::string>& validExtensions,
	                    std::vector<fs::path>& samples) noexcept
	{
		fs::directory_iterator dirIt;
		fs::path               current;

		samples.clear();
		dirIt = fs::directory_iterator(samplesDirPath);
		for (auto& p : dirIt)
		{
			current = samplesDirPath / p.path().filename();
			if (isValidPath_(current,
			                 validExtensions))
			{
				samples.emplace_back(current);
			}
		}
	}

	inline void
	trackPairedSamples (const fs::path& sourceDirPath,
	                    const fs::path& pairedDirPath,
	                    const std::vector<std::string>& validExtensions,
	                    std::vector<fs::path>& samples,
	                    std::vector<fs::path>& paired) noexcept
	{
		fs::path currentPath;
		fs::path pairedPath;

		samples.clear();
		paired.clear();
		for (auto& p1 : fs::directory_iterator(sourceDirPath))
		{
			currentPath = sourceDirPath / p1.path().filename();
			if (isValidPath_(currentPath,
			                 validExtensions))
			{
				for (auto& p2 : fs::directory_iterator(pairedDirPath))
				{
					pairedPath = pairedDirPath / p2.path().filename();
					if (isValidPath_(pairedPath,
					                 validExtensions) &&
					    currentPath.filename() == pairedPath.filename())
					{
						samples.emplace_back(currentPath);
						paired.emplace_back(pairedPath);
					}
				}
			}
		}
	}

	inline void
	trackMatchFileSamples (const fs::path& matchFilePath,
	                       std::vector<fs::path>& samples,
	                       std::vector<fs::path>& paired) noexcept
	{
		std::ifstream matchFileStream;
		std::string   matchFileLine;
		fs::path      currentPath;
		fs::path      pairedPath;

		samples.clear();
		paired.clear();
		matchFileStream.open(matchFilePath.c_str());
		while (!matchFileStream.eof())
		{
			auto comaOffset = 0ul;

			std::getline(matchFileStream,
						 matchFileLine);
            comaOffset = matchFileLine.find_first_of(',');
			currentPath = matchFileLine.substr(0,
					                           comaOffset).data();
			pairedPath = matchFileLine.substr(comaOffset + 1,
					                          matchFileLine.size()).data();
			if (isValidPath_(currentPath) &&
			    isValidPath_(pairedPath))
			{
				samples.emplace_back(currentPath);
				paired.emplace_back(pairedPath);
			}
		}
		matchFileStream.close();
	}

private:

	inline bool
	isValidPath_ (const fs::path& targetPath) noexcept
	{
		return fs::is_regular_file(targetPath);
	}

	inline bool
	isValidPath_ (const fs::path& targetPath,
	              const std::vector<std::string>& validExtensions) noexcept
	{
		fs::path targetPathExt;
		
		if (fs::is_regular_file(targetPath))
		{
			targetPathExt = targetPath.extension();
			for (const auto& e : validExtensions)
			{
				if (targetPathExt.generic_string() == e)
				{
					return true;
				}
			}
		}

		return false;
	}
};

}

#endif // BIOSEQZIP_BIOSEQZIP_SAMPLE_TRACKER_H
