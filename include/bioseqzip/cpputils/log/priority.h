/**
 * \file      log/priority.h
 * \author    Emanuele Parisi
 * \date      January, 2019
 * \copyright Copyright 2019 Emanuele Parisi. All rights reserved.
 *
 * Definitions of the priority any Logger has to be able to handle.
 */

#ifndef CPPUTILS_LOG_PRIORITY_H
#define CPPUTILS_LOG_PRIORITY_H

namespace cpputils
{

/**
 * Enumeration of the standard priority thresholds allowed by any Logger.
 */
enum class Priority
{
	/**
	 * Serious errors messages.
	 */
	ERROR   = 0,
	/**
	 * Messages communicating that an unexpected event occurred.
	 */
	WARNING = 1,
	/**
	 * Messages giving information about the application state.
	 */
	INFO    = 2,
	/**
	 * Messages giving configuration information.
	 */
	CONFIG  = 3,
	/**
	 * Debug messages.
	 */
	DEBUG   = 4
};

} // cpputils

#endif // CPPUTILS_LOG_PRIORITY_H
