/**
 * \file      log/sink_stdout.h
 * \author    Emanuele Parisi
 * \date      January, 2019
 * \copyright Copyright 2019 Emanuele Parisi. All rights reserved.
 *
 * This file contains facility for performing log operation on the process
 * standard output.
 */

#ifndef CPPUTILS_LOG_SINK_STDOUT_H
#define CPPUTILS_LOG_SINK_STDOUT_H

#include <iostream>

namespace cpputils
{

/**
 * Sink object for forwarding log messages to the current process standard
 * output.
 */
class SinkStdout
{
public:
	/**
	 * Class constructor.
	 */
	SinkStdout () noexcept = default;

	/**
	 * Deleted class copy constructor.
	 *
	 * \param other is the object the current instance is initialized from.
	 */
	SinkStdout (const SinkStdout& other) noexcept = delete;

	/**
	 * Class move constructor.
	 *
	 * \param other is the object the current instance is initialized from.
	 */
	SinkStdout (SinkStdout&& other) noexcept = default;

	/**
	 * Force the sink object to synchronize, performing all the log operations
	 * requested which are still pending.
	 */
	inline void
	flush () noexcept
	{
		std::flush(std::cout);
	}

	/**
	 * Class left shift operator, used for triggering message logging.
	 *
	 * \tparam TMessage is the type of message to be logged.
	 * \param message is the object to be logged. It is supposed that the
	 * proper left-shift operator between the target object and std::cout is
	 * defined.
	 * \return the reference to the current sink object.
	 */
	template <typename TMessage>
	inline SinkStdout&
	operator<< (const TMessage& message) noexcept
	{
		std::cout << message;

		return *this;
	}

	/**
	 * Class left shift operator, used for triggering message logging.
	 *
	 * This method overload is meant for allowing the concatenation of a set
	 * of log operation over the sink, ending with a std::endl, which is a
	 * function.
	 *
	 * \param func is the function object to be logged.
	 * \return the reference to the current sink object.
	 *
	 * \see http://www.cplusplus.com/reference/ostream/endl/
	 */
	inline SinkStdout&
	operator<< (std::ostream& (func) (std::ostream&)) noexcept
	{
		std::cout << func;

		return *this;
	}

	/**
     * Deleted copy operator.
     *
     * \param other is the object to be copied into current instance.
     * \return a reference to the copied object.
     */
	inline SinkStdout&
	operator= (const SinkStdout& other) noexcept = delete;

	/**
	 * Class move operator.
	 *
	 * \param other is the object moved into the current instance.
	 * \return a reference to the copied object.
	 */
	inline SinkStdout&
	operator= (SinkStdout&& other) noexcept = default;
};

} // cpputils

#endif // CPPUTILS_LOG_SINK_STDOUT_H
