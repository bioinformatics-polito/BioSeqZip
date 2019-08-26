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

#ifndef SEQAN_TAG_IO_TAG_READ_WRITE_H_
#define SEQAN_TAG_IO_TAG_READ_WRITE_H_

namespace seqan {

// ============================================================================
// Tags, Classes, Enums
// ============================================================================

// ----------------------------------------------------------------------------
// Tag TAG
// ----------------------------------------------------------------------------

/*!
 * @tag FileFormats#TAG
 * @brief Tag for selecting the TAG format.
 *
 * @signature typedef Tag<TAG_> TAG;
 */
struct TAG_;
typedef Tag<TAG_> TAG;

// ----------------------------------------------------------------------------
// Tag TAGQ
// ----------------------------------------------------------------------------

/*!
 * @tag FileFormats#TAGQ
 * @brief Tag for selecting the TAGQ format.
 *
 * @signature typedef Tag<TAGQ_> TAGQ;
 */
    struct TAGQ_;
    typedef Tag<TAGQ_> TAGQ;

// ----------------------------------------------------------------------------
// Class MagicHeader
// ----------------------------------------------------------------------------

template <typename T>
struct MagicHeader<TAG, T> :
        public MagicHeader<Nothing, T> {};

template <typename T>
struct MagicHeader<TAGQ, T> :
        public MagicHeader<Nothing, T> {};

// ----------------------------------------------------------------------------
// Class FileExtensions
// ----------------------------------------------------------------------------

template <typename T>
struct FileExtensions<TAG, T>
{
    static char const * VALUE[1];
};

template <typename T>
char const * FileExtensions<TAG, T>::VALUE[1] =
{
    ".tag"
};

template <typename T>
struct FileExtensions<TAGQ, T>
{
    static char const * VALUE[1];
};

template <typename T>
char const * FileExtensions<TAGQ, T>::VALUE[1] =
{
    ".tagq"
};

// ============================================================================
// Functions
// ============================================================================

// ----------------------------------------------------------------------------
// Function readRecord
// ----------------------------------------------------------------------------

/*
 * @fn TagFileIO#readRecord
 * @brief Read one TAG record from a SinglePassRecordReader.
 *
 * @signature void readRecord(tag, counter, context, iter);
 *
 * @param[out]    tag     The TAG sequence to write the results to.
 * @param[out]    counter The counter to write tag occurrences to.
 * @param[in,out] context A CharString to use for buffers.
 * @param[in,out] iter    A @link ForwardIteratorConcept forward iterator @endlink to use for reading.
 *
 * @throws IOError if something went wrong.
 */
template <typename TTagString, typename TCounter, typename TFwdIterator>
void readRecord(TTagString& tag_seq,
                TCounter& counter,
                TFwdIterator& iter,
                TAG const& tag)
{
    String<char> buffer;
    IsNewline isNewline;

    clear(tag_seq);

    // skip commented lines as well as ## directives
    skipUntil(iter, NotFunctor<IsWhitespace>());  //skip empty lines
    while (!atEnd(iter) && value(iter) == '#')
    {
        skipLine(iter);
    }
    skipUntil(iter, NotFunctor<IsWhitespace>());  //skip empty lines

    // read column 1: tag
    readUntil(tag_seq,
              iter,
              OrFunctor<IsTab, AssertFunctor<NotFunctor<IsNewline>, ParseError, TAG> >());
    skipOne(iter);

    // read column 2: counter
    readUntil(buffer,
              iter,
              OrFunctor<IsNewline, AssertFunctor<NotFunctor<IsTab>, ParseError, TAG>>());
    counter = lexicalCast<TCounter>(buffer);
    skipOne(iter);

    // skip empty lines and whitespaces
    skipUntil(iter,
              NotFunctor<IsWhitespace>());
}

/*
 * @fn TagFileIO#readRecord
 * @brief Read one TAG record from a SinglePassRecordReader.
 *
 * @signature void readRecord(tag, qual, counter, context, iter);
 *
 * @param[out]    tag     The TAG sequence to write the results to.
 * @param[out]    qual    The quality sequence, that will be cleared, as TAG files have no qualities.
 * @param[out]    counter The counter to write tag occurrences to.
 * @param[in,out] context A CharString to use for buffers.
 * @param[in,out] iter    A @link ForwardIteratorConcept forward iterator @endlink to use for reading.
 *
 * @throws IOError if something went wrong.
 */
template <typename TTagString, typename TQualString, typename TCounter, typename TFwdIterator>
void readRecord(TTagString& tag_seq,
                TQualString& qual,
                TCounter& counter,
                TFwdIterator& iter,
                TAG const& tag)
{
    clear(qual);
    readRecord(tag_seq,
               counter,
               iter,
               tag);
}

/*
 * @fn TagFileIO#readRecord
 * @brief Read one TAGQ record from a SinglePassRecordReader.
 *
 * @signature void readRecord(tag, counter, context, iter);
 *
 * @param[out]    tag     The TAGQ sequence to write the results to.
 * @param[out]    qual    The quality sequence to write the results to.
 * @param[out]    counter The counter to write tag occurrences to.
 * @param[in,out] context A CharString to use for buffers.
 * @param[in,out] iter    A @link ForwardIteratorConcept forward iterator @endlink to use for reading.
 *
 * @throws IOError if something went wrong.
 */
template <typename TTagString, typename TQualString, typename TCounter, typename TFwdIterator>
void readRecord(TTagString& tag_seq,
                TQualString& qual,
                TCounter& counter,
                TFwdIterator& iter,
                TAGQ const& tag)
{
    String<char> buffer;
    IsNewline isNewline;

    clear(tag_seq);
    clear(qual);

    // skip commented lines as well as ## directives
    skipUntil(iter, NotFunctor<IsWhitespace>());  //skip empty lines
    while (!atEnd(iter) && value(iter) == '#')
    {
        skipLine(iter);
    }
    skipUntil(iter, NotFunctor<IsWhitespace>());  //skip empty lines

    // read column 1: tag
    readUntil(tag_seq,
              iter,
              OrFunctor<IsTab, AssertFunctor<NotFunctor<IsNewline>, ParseError, TAGQ> >());
    skipOne(iter);

    // read column 2: qualities
    readUntil(qual,
              iter,
              OrFunctor<IsTab, AssertFunctor<NotFunctor<IsNewline>, ParseError, TAGQ> >());
    skipOne(iter);

    // read column 3: counter
    readUntil(buffer,
              iter,
              OrFunctor<IsNewline, AssertFunctor<NotFunctor<IsTab>, ParseError, TAGQ>>());
    counter = lexicalCast<TCounter>(buffer);
    skipOne(iter);

    // skip empty lines and whitespaces
    skipUntil(iter,
              NotFunctor<IsWhitespace>());
}

/*
 * @fn TagFileIO#readRecord
 * @brief Read one TAGQ record from a SinglePassRecordReader, without qualities.
 *
 * @signature void readRecord(tag, qual, counter, context, iter);
 *
 * @param[out]    tag     The TAGQ sequence to write the results to.
 * @param[out]    counter The counter to write tag occurrences to.
 * @param[in,out] context A CharString to use for buffers.
 * @param[in,out] iter    A @link ForwardIteratorConcept forward iterator @endlink to use for reading.
 *
 * @throws IOError if something went wrong.
 */
template <typename TTagString, typename TCounter, typename TFwdIterator>
void readRecord(TTagString& tag_seq,
                TCounter& counter,
                TFwdIterator& iter,
                TAGQ const& tag)
{
    CharString qual;

    readRecord(tag_seq,
               qual,
               counter,
               iter,
               tag);
    clear(qual);
}

// ----------------------------------------------------------------------------
// Function writeRecord()
// ----------------------------------------------------------------------------

/*
 * @fn TagFileIO#writeRecord
 * @brief Writes a tag-counter pair to a stream as TAG.
 *
 * @signature void writeRecord(stream, tag_seq, counter, tag);
 *
 * @param[in,out] target  The @link OutputIteratorConcept output iterator @endlink to write to.
 * @param[in]     tag_seq The @link StringConcept @endlink containing the tag sequence to write out.
 * @param[in]     counter The @link StringConcept @endlink containing the counter to write out.
 * @param[in]     tag     A tag to select the TAG file format.
 *
 * @throws IOError if something went wrong.
 */
template <typename TFwdIterator, typename TTagString, typename TCounter>
inline void
writeRecord (TFwdIterator& iter,
             TTagString const & tag_seq,
             TCounter const & counter,
             TAG const & tag) {
    // write tag sequence
    write(iter,
          tag_seq);
    writeValue(iter,
               '\t');

    // write counter value
    appendNumber(iter,
                 counter);
    writeValue(iter,
               '\n');
}

template <typename TFwdIterator, typename TTagString, typename TQualString, typename TCounter>
inline void
writeRecord (TFwdIterator& iter,
             TTagString const & tag_seq,
             TQualString const & qual,
             TCounter const & counter,
             TAG const & tag) {
    writeRecord(iter,
                tag_seq,
                counter,
                tag);
}

/*
 * @fn TagFileIO#writeRecord
 * @brief Writes a tag-counter pair to a stream as TAGQ.
 *
 * @signature void writeRecord(stream, tag_seq, counter, tag);
 *
 * @param[in,out] target  The @link OutputIteratorConcept output iterator @endlink to write to.
 * @param[in]     tag_seq The @link StringConcept @endlink containing the tag sequence to write out.
 * @param[in]     qual    The @link StringConcept @endlink containing the quality sequence to write out.
 * @param[in]     counter The @link StringConcept @endlink containing the counter to write out.
 * @param[in]     tag     A tag to select the TAGQ file format.
 *
 * @throws IOError if something went wrong.
 */
template <typename TFwdIterator, typename TTagString, typename TQualString, typename TCounter>
inline void
writeRecord (TFwdIterator& iter,
             TTagString const & tag_seq,
             TQualString const & qual,
             TCounter const & counter,
             TAGQ const & tag) {
    // write tag sequence
    write(iter,
          tag_seq);
    writeValue(iter,
               '\t');

    // write qualities
    write(iter,
          qual);
    writeValue(iter,
               '\t');

    // write counter value
    appendNumber(iter,
                 counter);
    writeValue(iter,
               '\n');
}

template <typename TFwdIterator, typename TTagString, typename TCounter>
inline void
writeRecord (TFwdIterator& iter,
             TTagString const & tag_seq,
             TCounter const & counter,
             TAGQ const & tag) {
    String<char> quals;

    for (auto i = 0u; i < length(tag_seq); i++) {
        appendValue(quals,
                    '!' + static_cast<char>(getQualityValue(tag_seq[i])));
    }

    writeRecord(iter,
                tag_seq,
                quals,
                counter,
                tag);
}

}  // namespace seqan

#endif  // SEQAN_TAG_IO_TAG_READ_WRITE_H_
