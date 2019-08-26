#!/usr/bin/env python

import random


class SequenceEntry:
    @staticmethod
    def simulate(read_length,
                 min_occurrences,
                 max_occurrences):
        # Create a random nucleotide sequence.
        tag = ''
        for i in range(read_length):
            tag += random.choice(['A',
                                  'C',
                                  'G',
                                  'T',
                                  'N'])

        # Extract a random number expressing the sequence multiplicity.
        occurrences = random.randint(min_occurrences,
                                     max_occurrences)

        # Create a quality string for each sequence occurrence.
        qualities = list()
        for i in range(occurrences):
            quality = ''
            for j in range(read_length):
                quality += chr(ord('!') + random.choice(list(range(43))))
            qualities.append(quality)

        # Build an entry object and return it.
        return SequenceEntry(tag,
                             qualities)

    @staticmethod
    def simulate_n(read_length,
                   min_occurrences,
                   max_occurrences,
                   dataset_size):
        dataset = list()
        for i in range(dataset_size):
            dataset.append(SequenceEntry.simulate(read_length,
                                                  min_occurrences,
                                                  max_occurrences))

        return dataset

    @staticmethod
    def separate_mates(entry):
        # Split the sequence in two.
        tag_mate_1 = entry.tag[:len(entry.tag) / 2]
        tag_mate_2 = entry.tag[(len(entry.tag) / 2):]

        # Split the sequence quality in two.
        qualities_mate_1 = list()
        qualities_mate_2 = list()
        for q in entry.qualities:
            qualities_mate_1.append(q[:len(entry.tag) / 2])
            qualities_mate_2.append(q[(len(entry.tag) / 2):])

        # Return the split sequences.
        return (SequenceEntry(tag_mate_1,
                              qualities_mate_1),
                SequenceEntry(tag_mate_2,
                              qualities_mate_2))

    def __init__(self,
                 tag_sequence,
                 qualities):
        # Initialize the sequence tag and quality.
        self._tag = tag_sequence
        self._qualities = qualities

        # Pre-compute the sequence quality average.
        self._quality_average = ''
        for i in range(len(qualities[0])):
            count = 0
            for j in range(len(qualities)):
                count = count + ord(qualities[j][i])
            self._quality_average += chr(int(count / len(qualities)))

    def __cmp__(self,
                other):
        # Specify a nucleotide order compatible with the one used by SeqAn2.
        order = {
            'A': 0,
            'C': 1,
            'G': 2,
            'T': 3,
            'N': 4
        }

        # Implement the compare procedure.
        for i in range(len(self.tag)):
            if i >= len(other.tag):
                return 1
            if order[self.tag[i]] < order[other.tag[i]]:
                return -1
            if order[self.tag[i]] > order[other.tag[i]]:
                return 1
        if len(self.tag) == len(other.tag):
            return 0

        return -1

    @property
    def tag(self):
        return self._tag

    @property
    def qualities(self):
        return self._qualities

    @property
    def quality_average(self):
        return self._quality_average

    @property
    def occurrences(self):
        return len(self.qualities)

    def unpack(self):
        # Create a list of sequence/quality tuple.
        unpacked = list()
        for i in range(self.occurrences):
            unpacked.append((self.tag,
                             self.qualities[i]))

        return unpacked
