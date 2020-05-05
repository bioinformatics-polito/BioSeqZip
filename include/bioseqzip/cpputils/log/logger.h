/**
 * \file      log/logger.h
 * \author    Emanuele Parisi
 * \date      January, 2019
 * \copyright Copyright 2019 Emanuele Parisi. All rights reserved.
 *
 * This file contains facilities providing the user generic synchronous
 * logging capabilities.
 */

#ifndef CPPUTILS_LOG_LOGGER_H
#define CPPUTILS_LOG_LOGGER_H

#include <mutex>

#include "priority.h"

/**
 * Shortcut macro for performing logging with ERROR priority.
 */
#define ERROR(LOGGER, MSG) \
((LOGGER).log(cpputils::Priority::ERROR, (MSG)))

/**
 * Shortcut macro for performing logging with WARNING priority.
 */
#define WARNING(LOGGER, MSG) \
((LOGGER).log(cpputils::Priority::WARNING, (MSG)))

/**
 * Shortcut macro for performing logging with INFO priority.
 */
#define INFO(LOGGER, MSG) \
((LOGGER).log(cpputils::Priority::INFO, (MSG)))

/**
 * Shortcut macro for performing logging with CONFIG priority.
 */
#define CONFIG(LOGGER, MSG) \
((LOGGER).log(cpputils::Priority::CONFIG, (MSG)))

/**
 * Shortcut macro for performing logging with DEBUG priority.
 */
#define DEBUG(LOGGER, MSG) \
((LOGGER).log(cpputils::Priority::DEBUG, (MSG)))

namespace cpputils
{

/**
 * A class for performing synchronous thread-safe log operation on a given
 * sink.
 *
 * This class allows performing log operation on the sink specified. These
 * operations have to be considered synchronous, meaning that any log message
 * is forwarded to the proper sink immediately, and thread-safe, as a mutex
 * guarantees mutual exclusion between concurrent access to the logger
 * facility.
 *
 * \tparam TSink is the sink in charge of actually perform the log action.
 * \tparam TSpec is a further specialization tag, which is 'void' by default.
 */
template <typename TSink,
          typename TSpec = void>
class Logger
{
public:
	/**
	 * Class constructor.
	 *
	 * The constructor simply initializes the logger priority threshold to
	 * the lowest available priority, meaning that by default every log message
	 * is forwarded to the specified sink.
	 */
	Logger () noexcept
	{
		threshold_ = Priority::DEBUG;
	}

	/**
	 * Class constructor with embedded sink initialization.
	 *
	 * \tparam TSinkConfs are the types of the set of objects used for
	 * constructing the logger private sink.
	 * \param sinkConfs are the set of objects used for constructing the sink
	 * object.
	 */
	template <typename ... TSinkConfs>
	Logger (TSinkConfs ... sinkConfs) noexcept
	{
		threshold_ = Priority::DEBUG;
		sink_      = TSink(&sinkConfs...);
	}

	/**
	 * Deleted copy constructor.
	 *
	 * \param other is the object the current instance is initialized from.
	 */
	Logger (const Logger& other) noexcept = delete;

	/**
	 * Class move constructor.
	 *
	 * \param other is the object the current instance is initialized from.
	 */
	Logger (Logger&& other) = default;

	/**
	 * Retrieve the current log priority.
	 *
	 * \return the enum value characterizing the current logger priority
	 * threshold.
	 */
	inline Priority
	getPriority () noexcept
	{
		return threshold_;
	}

	/**
	 * Set the logger priority threshold to the given priority value.
	 *
	 * \param priority is the value the logger threshold is set to.
	 */
	inline void
	setPriority (Priority priority) noexcept
	{
		threshold_ = priority;
	}

	/**
	 * Method for triggering the log procedure.
	 *
	 * This method is the main log facility. It takes a generic message type
	 * and forward it to the logger sink only if the log priority is lower or
	 * equal than the minimum allowed threshold.
	 *
	 * \tparam TMessage is the type of the message to be logged.
	 * \param priority is the priority of the message to be logged.
	 * \param msg is the object containing the information to be sent to the
	 * active sink.
	 */
	template <typename TMessage>
	inline void
	log (Priority priority,
	     const TMessage& msg) noexcept
	{
		if (priority <= threshold_)
		{
			std::lock_guard<std::mutex> lock(logMutex_);

			sink_ << msg;
			sink_.flush();
		}
	}

	/**
	 * Deleted copy operator.
	 *
	 * \param other is the object to be copied into current instance.
	 * \return a reference to the copied object.
	 */
	inline Logger&
	operator= (const Logger& other) noexcept = delete;

	/**
	 * Class move operator.
	 *
	 * \param other is the object moved into the current instance.
	 * \return a reference to the copied object.
	 */
	inline Logger&
	operator= (Logger&& other) = default;

private:
	/**
	 * Logger priority threshold used for deciding what messages have to be
	 * forwarded to the sink.
	 */
	Priority   threshold_;
	/**
	 * Sink in charge of actually performing the log actions requested.
	 */
	TSink      sink_;
	/**
	 * Semaphore ensuring the sink is accessed synchronously by the logger.
	 */
	std::mutex logMutex_;
};

} // cpputils

#endif // CPPUTILS_LOG_LOGGER_H
