/**
 * \file      apps/expand/input_format.h
 * \author    Gianvito Urgese
 * \author    Emanuele Parisi
 * \author    Orazio Scicolone
 * \date      November, 2018
 *
 * \brief
 */

#ifndef BIOSEQZIP_APPS_EXPAND_INPUT_FORMAT_H
#define BIOSEQZIP_APPS_EXPAND_INPUT_FORMAT_H

namespace bioseqzip
{
namespace expand
{

enum class InputFormat
{
	FASTA,
	FASTQ,
	TAG,
	TAGQ
};

} // expand
} // bioseqzip

#endif // BIOSEQZIP_APPS_EXPAND_INPUT_FORMAT_H
