/**
 * \file      include/bioseqzip/core/library_layout.h
 * \author    Gianvito Urgese
 * \author    Emanuele Parisi
 * \author    Orazio Scicolone
 * \date      November, 2018
 *
 * \copyright Copyright (C) 2019 Gianvito Urgese. All Rights Reserved.
 *
 * File containing an enum class for labelling a sample layout.
 */

#ifndef BIOSEQZIP_BIOSEQZIP_CORE_LIBRARY_LAYOUT_H
#define BIOSEQZIP_BIOSEQZIP_CORE_LIBRARY_LAYOUT_H

namespace bioseqzip
{

/**
 * An enum class for labelling the layout of a biologic sample.
 */
enum class LibraryLayout
{
	/**
	 * Label for signaling single-end samples.
	 */
	 SINGLE_END,
	/**
	 * Label for signaling paired-end samples.
	 */
	 PAIRED_END,
	/**
	 * Label for interleaved paired-end samples.
	 */
	 INTERLEAVED,
	 /**
	  * Label for fixed-breakpoint paired-end samples.
	  */
	 BREAKPOINT
};
} // bioseqzip

#endif // BIOSEQZIP_BIOSEQZIP_CORE_LIBRARY_LAYOUT_H
