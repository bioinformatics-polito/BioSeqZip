/**
 * \file      stats/chronometer.h
 * \author    Emanuele Parisi
 * \date      February, 2019
 * \copyright Copyright 2019 Emanuele Parisi. All rights reserved.
 *
 * Contains a class for basic in-source time measurement.
 *
 * In this file are contained facilities for performing basic in-source time
 * measurements. This class in an interface for the facilities offered by the
 * std::chrono library; further details about types available for template
 * specialization can be found in the STL documentation.
 *
 * \see http://www.cplusplus.com/reference/chrono/
 */

#ifndef CPPUTILS_STATS_CHRONOMETER_H_
#define CPPUTILS_STATS_CHRONOMETER_H_

#include <chrono>

namespace cpputils
{

/**
 * Class for in-source time measurement.
 *
 * \tparam TDuration is the type used for expressing the measured values.
 * \tparam TTime is the time granularity used when measuring time.
 * \tparam TClock is the type of clock used for measurements.
 */
template <typename TTime = std::chrono::milliseconds,
          typename TDuration = uint64_t,
          typename TClock = std::chrono::system_clock>
class Chronometer
{
private:
	/**
	 * First measured time point.
	 */
	std::chrono::time_point<TClock> timeStart_;
	/**
	 * Last measured time point.
	 */
	std::chrono::time_point<TClock> timeStop_;

public:
	/**
	 * Class constructor.
	 */
	Chronometer () noexcept
	{
		timeStart_ = TClock::now();
		timeStop_  = TClock::now();
	}

	/**
	 * Class copy constructor.
	 *
	 * \param other is the object the current instance is initialized from.
	 */
	Chronometer (const Chronometer& other) noexcept = default;

	/**
	 * Class move constructor.
	 *
	 * \param other is the object the current instance is initialized from.
	 */
	Chronometer (Chronometer&& other) noexcept = default;

	/**
	 * Start the chronometer, getting the current time point.
	 */
	inline void
	start () noexcept
	{
		timeStart_ = TClock::now();
	}

	/**
	 * Stop the chronometer and return the time elapsed from the start point.
	 *
	 * \return the time elapsed since when the chronometer was started.
	 */
	inline TDuration
	stop () noexcept
	{
		timeStop_ = TClock::now();

		return elapsed();
	}

	/**
	 * Get the difference between the start and stop time points.
	 *
	 * This function compute the difference between the start and stop time
	 * points, without explicitly stopping the chronometer; this means that
	 * this function returns a meaningful value only if the chronometer is
	 * explicitly stopped first.
	 *
	 * \return the elapsed time considering the start and stop times.
	 */
	inline TDuration
	elapsed () const noexcept
	{
		TTime t_dur = std::chrono::duration_cast<TTime>(timeStop_ - timeStart_);

		return t_dur.count();
	}
};
} // cpputils

#endif // CPPUTILS_STATS_CHRONOMETER_H_
