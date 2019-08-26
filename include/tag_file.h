// ==========================================================================
//                 SeqAn - The Library for Sequence Analysis
// ==========================================================================
// Copyright (c) 2006-2018, Knut Reinert, FU Berlin
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Knut Reinert or the FU Berlin nor the names of
//       its contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL KNUT REINERT OR THE FU BERLIN BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
//
// ==========================================================================
// Author: Gianvito Urgese <gianvito.urgese@polito.it>
// Author: Emanuele Parisi <emanuele.parisi@polito.it>
// ==========================================================================
// Class for reading/writing files in TAG or TAGQ format.
// ==========================================================================

#ifndef SEQAN_TAG_IO_TAG_FILE_H_
#define SEQAN_TAG_IO_TAG_FILE_H_

namespace seqan {

// ============================================================================
// Typedefs
// ============================================================================

// ----------------------------------------------------------------------------
// Typedef TagFileIn
// ----------------------------------------------------------------------------

/*!
 * @class TagFileIn
 * @signature typedef FormattedFile<TAG, Input> TagFileIn;
 * @extends FormattedFileIn
 * @headerfile <seqan/tag_io.h>
 * @brief Class for reading TAG and TAGQ files.
 */

typedef FormattedFile<TAGQ, Input>   TagFileIn;

// ----------------------------------------------------------------------------
// Typedef TagFileOut
// ----------------------------------------------------------------------------

/*!
 * @class TagFileOut
 * @signature typedef FormattedFile<TAG, Output> TagFileOut;
 * @extends FormattedFileOut
 * @headerfile <seqan/tag_io.h>
 * @brief Class for writing TAG and TAGQ files.
 *
 * @see TagRecord
 */

typedef FormattedFile<TAGQ, Output>  TagFileOut;

// ============================================================================
// Metafunctions
// ============================================================================

// ----------------------------------------------------------------------------
// Metafunction FormattedFileContext
// ----------------------------------------------------------------------------

template <typename TSpec, typename TStorageSpec>
struct FormattedFileContext<FormattedFile<TAGQ, Input, TSpec>, TStorageSpec>
{
    typedef CharString Type;
};

// ----------------------------------------------------------------------------
// Metafunction FileFormats
// ----------------------------------------------------------------------------

template <typename TSpec>
struct FileFormat<FormattedFile<TAG, Input, TSpec>>
{
    typedef TagSelector<TagList<TAG>> Type;
};

template <typename TSpec>
struct FileFormat<FormattedFile<TAGQ, Input, TSpec>>
{
    typedef TagSelector<TagList<TAGQ, TagList<TAG>>> Type;
};

template <typename TSpec>
struct FileFormat<FormattedFile<TAG, Output, TSpec>>
{
    typedef TagSelector<TagList<TAG>> Type;
};

template <typename TSpec>
struct FileFormat<FormattedFile<TAGQ, Output, TSpec>>
{
    typedef TagSelector<TagList<TAGQ, TagList<TAG>>> Type;
};

// ============================================================================
// Functions
// ============================================================================

// ----------------------------------------------------------------------------
// Function readRecord(TagSelector); Without qualities
// ----------------------------------------------------------------------------
template <typename TTagString, typename TCounter, typename TSpec>
inline void
readRecord(TTagString& tag_seq,
           TCounter& counter,
           FormattedFile<TAGQ, Input, TSpec>& file,
           TagSelector<> const&)
{}

template <typename TTagString, typename TCounter, typename TSpec, typename TTagList>
inline void
readRecord(TTagString & tag_seq,
           TCounter& counter,
           FormattedFile<TAGQ, Input, TSpec> & file,
           TagSelector<TTagList> const & format)
{
    if (isEqual(format,
                typename TTagList::Type()))
    {
        readRecord(tag_seq,
                   counter,
                   file.iter,
                   typename TTagList::Type());
    }
    else
    {
        readRecord(tag_seq,
                   counter,
                   file,
                   static_cast<typename TagSelector<TTagList>::Base const & >(format));
    }
}

// ----------------------------------------------------------------------------
// Function readRecord(TagSelector); With qualities
// ----------------------------------------------------------------------------

template <typename TTagString, typename TQualString, typename TCounter, typename TSpec>
inline void
readRecord(TTagString&,
           TQualString&,
           TCounter&,
           FormattedFile<TAGQ, Input, TSpec>&,
           TagSelector<> const&)
{}

template <typename TTagString, typename TQualString, typename TCounter, typename TSpec, typename TTagList>
inline void
readRecord(TTagString & tag_seq,
           TQualString & qual,
           TCounter& counter,
           FormattedFile<TAGQ, Input, TSpec> & file,
           TagSelector<TTagList> const & format)
{
    if (isEqual(format,
                typename TTagList::Type()))
    {
        readRecord(tag_seq,
                   qual,
                   counter,
                   file.iter,
                   typename TTagList::Type());
    }
    else
    {
        readRecord(tag_seq,
                   qual,
                   counter,
                   file,
                   static_cast<typename TagSelector<TTagList>::Base const & >(format));
    }
}

/*!
 * @fn TagFileIn#readRecord
 * @brief Read one @link FormattedFileRecordConcept @endlink from a @link TagFileIn @endlink object.
 *
 * @signature void readRecord(tag_seq, counter[, qual], fileIn);
 *
 * @param[out] tag_seq      The @link StringConcept @endlink object where to read tag into.
 * @param[out] counter      The unsigned variable where to read the tag counter into.
 * @param[out] qual         The @link StringConcept @endlink object where to read the quality information into.
 * @param[in,out] fileIn    The @link TagFileIn @endlink object to read from.
 *
 * @throw IOError On low-level I/O errors.
 * @throw ParseError On high-level file format errors.
 */

// ----------------------------------------------------------------------------
// Function readRecord(); With separate qualities
// ----------------------------------------------------------------------------
template <typename TTagString, typename TQualString, typename TCounter, typename TSpec>
inline void
readRecord(TTagString & tag_seq,
           TQualString & qual,
           TCounter & counter,
           FormattedFile<TAGQ, Input, TSpec> & file)
{
    readRecord(tag_seq,
               qual,
               counter,
               file,
               file.format);
}


// ----------------------------------------------------------------------------
// Function readRecord(); Without separate qualities or no qualities
// ----------------------------------------------------------------------------
template <typename TTagString, typename TCounter, typename TSpec>
inline void
readRecord(TTagString & tag_seq,
           TCounter & cnt,
           FormattedFile<TAGQ, Input, TSpec> & file)
{
    readRecord(tag_seq,
               cnt,
               file,
               file.format);
}

// ----------------------------------------------------------------------------
// Function readRecords()
// ----------------------------------------------------------------------------

/*!
 * @fn TagFileIn#readRecords
 * @brief Read many records from a @link TagFileIn @endlink object.
 * @signature void readRecords(tag_seqs, quals, counters, fileIn, numRecord);
 * @see TagFileIn#readRecord
 */
template <typename TTagStringSet, typename TCounterString, typename TSpec, typename TSize>
inline void
readRecords(TTagStringSet & tag_seqs,
            TCounterString& counters,
            FormattedFile<TAGQ, Input, TSpec> & file,
            TSize maxRecords)
{
    typename Value<TTagStringSet>::Type tag_seq;
    typename Value<TCounterString>::Type counter;

    for (TSize i = 0; i < maxRecords && !atEnd(file); i++)
    {
        readRecord(tag_seq,
                   counter,
                   file,
                   file.format);
        appendValue(tag_seqs,
                    tag_seq);
        appendValue(counters,
                    counter);
    }
}

template <typename TTagStringSet, typename TCounterString, typename TSpec>
inline void
readRecords(TTagStringSet & tag_seqs,
            TCounterString& counters,
            FormattedFile<TAGQ, Input, TSpec> & file)
{
    readRecords(tag_seqs,
                counters,
                file,
                std::numeric_limits<typename Size<TTagStringSet>::Type>::max());
}

template <typename TTagStringSet, typename TQualStringSet, typename TCounterString, typename TSpec, typename TSize>
inline void
readRecords(TTagStringSet & tag_seqs,
            TQualStringSet & quals,
            TCounterString& counters,
            FormattedFile<TAGQ, Input, TSpec> & file,
            TSize maxRecords)
{
    typename Value<TTagStringSet>::Type tag_seq;
    typename Value<TQualStringSet>::Type qual;
    typename Value<TCounterString>::Type counter;

    for (TSize i = 0; i < maxRecords && !atEnd(file); i++)
    {
        readRecord(tag_seq,
                   qual,
                   counter,
                   file,
                   file.format);
        appendValue(tag_seqs,
                    tag_seq);
        appendValue(quals,
                    qual);
        appendValue(counters,
                    counter);

    }
}

template <typename TTagStringSet, typename TQualStringSet, typename TCounterString, typename TSpec>
inline void
readRecords(TTagStringSet & tag_seqs,
            TQualStringSet & quals,
            TCounterString& counters,
            FormattedFile<TAGQ, Input, TSpec> & file)
{
    readRecords(tag_seqs,
                quals,
                counters,
                file,
                std::numeric_limits<typename Size<TTagStringSet>::Type>::max());
}

// ----------------------------------------------------------------------------
// Function writeRecord(TagSelector); Without separate qualities
// ----------------------------------------------------------------------------
/*!
 * @fn TagFileOut#writeRecord
 * @brief Write one @link FormattedFileRecordConcept @endlink into a @link TagFileOut @endlink object.
 *
 * @signature void writeRecord(fileOut, meta, seq, qual);
 *
 * @param[in,out] fileOut   The @link TagFileOut @endlink object to write into.
 * @param[in] tag_seq       The @link StringConcept @endlink object where to read the tag sequence information from.
 * @param[in] qual          The @link StringConcept @endlink object where to read the quality information from.
 * @param[in] counter       The unsigned variable where to read the tag occurrences information from.
 *
 * @throw IOError On low-level I/O errors.
 * @throw ParseError On high-level file format errors.
 */
template <typename TSpec, typename TTagString, typename TCounter>
inline void
writeRecord(FormattedFile<TAGQ, Output, TSpec> &,
            TTagString const&,
            TCounter const&,
            TagSelector<> const&)
{}

template <typename TSpec, typename TTagString, typename TCounter, typename TTagList>
inline void
writeRecord(FormattedFile<TAGQ, Output, TSpec> & file,
            TTagString const & tag_seq,
            TCounter const & counter,
            TagSelector<TTagList> const & format)
{
    if (isEqual(format,
                typename TTagList::Type()))
    {
        writeRecord(file.iter,
                    tag_seq,
                    counter,
                    typename TTagList::Type());
    }
    else
    {
        writeRecord(file,
                    tag_seq,
                    counter,
                    static_cast<typename TagSelector<TTagList>::Base const & >(format));
    }
}

// ----------------------------------------------------------------------------
// Function writeRecord(TagSelector); With separate qualities
// ----------------------------------------------------------------------------

template <typename TSpec, typename TTagString, typename TQualString, typename TCounter>
inline void
writeRecord(FormattedFile<TAGQ, Output, TSpec> &,
            TTagString const &,
            TQualString const &,
            TCounter const &,
            TagSelector<> const &)
{}

template <typename TSpec, typename TTagString, typename TQualString, typename TCounter, typename TTagList>
inline void
writeRecord(FormattedFile<TAGQ, Output, TSpec> & file,
            TTagString const & tag_seq,
            TQualString const & qual,
            TCounter const & counter,
            TagSelector<TTagList> const & format)
{
    if (isEqual(format,
                typename TTagList::Type()))
    {
        writeRecord(file.iter,
                    tag_seq,
                    qual,
                    counter,
                    typename TTagList::Type());
    }
    else
    {
        writeRecord(file,
                    tag_seq,
                    qual,
                    counter,
                    static_cast<typename TagSelector<TTagList>::Base const & >(format));
    }
}

// ----------------------------------------------------------------------------
// Function writeRecord(); With separate qualities
// ----------------------------------------------------------------------------

template <typename TSpec, typename TTagString, typename TQualString, typename TCounter>
inline void
writeRecord(FormattedFile<TAGQ, Output, TSpec> & file,
            TTagString const & tag_seq,
            TQualString const & qual,
            TCounter const & counter)
{
    writeRecord(file,
                tag_seq,
                qual,
                counter,
                file.format);
}

// ----------------------------------------------------------------------------
// Function writeRecord(); Without separate qualities
// ----------------------------------------------------------------------------

template <typename TSpec, typename TTagString, typename TCounter>
inline void
writeRecord(FormattedFile<TAGQ, Output, TSpec> & file,
            TTagString const & tag_seq,
            TCounter const & counter)
{
    writeRecord(file,
                tag_seq,
                counter,
                file.format);
}

// ----------------------------------------------------------------------------
// Function writeRecords()
// ----------------------------------------------------------------------------

/*!
 * @fn TagFileOut#writeRecords
 * @brief Write many tag records into a @link TagFileOut @endlink object.
 * @signature void writeRecords(fileOut, tag_seqs, counters);
 * @see TagFileOut#writeRecord
 */
template <typename TSpec, typename TTagStringSet, typename TCounterStringSet>
inline void
writeRecords(FormattedFile<TAGQ, Output, TSpec> & file,
             TTagStringSet const & tag_seqs,
             TCounterStringSet const & counters)
{
    for (typename Size<TTagStringSet>::Type i = 0; i != length(tag_seqs); i++)
    {
        writeRecord(file,
                    tag_seqs[i],
                    counters[i],
                    file.format);
    }
}

template <typename TSpec, typename TTagStringSet, typename TQualStringSet, typename TCounterStringSet>
inline void
writeRecords(FormattedFile<TAGQ, Output, TSpec> & file,
             TTagStringSet const & tag_seqs,
             TQualStringSet const & quals,
             TCounterStringSet const & counters)
{
    for (typename Size<TTagStringSet>::Type i = 0; i != length(tag_seqs); i++)
    {
        writeRecord(file,
                    tag_seqs[i],
                    quals[i],
                    counters[i],
                    file.format);
    }
}

}  // namespace seqan

#endif // SEQAN_TAG_IO_TAG_FILE_H_
