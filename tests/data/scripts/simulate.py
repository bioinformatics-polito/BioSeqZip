from sequence_entry import *
from details_entry import *


def simulate_single_end(dataset_name,
                        read_length,
                        read_min_occurrences,
                        read_max_occurrences,
                        dataset_size):
    # Simulate dataset content
    entries = SequenceEntry.simulate_n(read_length,
                                       read_min_occurrences,
                                       read_max_occurrences,
                                       dataset_size)

    # Write fastx formatted dataset
    fastq_records = list()
    for e in entries:
        fastq_records = fastq_records + e.unpack()
    random.shuffle(fastq_records)
    with open(dataset_name + '.fq', 'w') as fq_out:
        for i in range(len(fastq_records)):
            fq_out.writelines(['@read' + str(i) + '\n',
                               fastq_records[i][0] + '\n',
                               '+\n',
                               fastq_records[i][1] + '\n'])

    # Write tagx formatted dataset
    entries.sort()
    with open(dataset_name + '.tagq', 'w') as tagq_out:
        for e in entries:
            tagq_out.write(e.tag + '\t' +
                           e.quality_average + '\t' +
                           str(e.occurrences) + '\n')


def simulate_paired_end(dataset_name,
                        read_length,
                        read_min_occurrences,
                        read_max_occurrences,
                        dataset_size):
    # Simulate dataset content
    entries_mate_1 = list()
    entries_mate_2 = list()
    entries = SequenceEntry.simulate_n(read_length,
                                       read_min_occurrences,
                                       read_max_occurrences,
                                       dataset_size)
    for e in entries:
        pair = SequenceEntry.separate_mates(e)
        entries_mate_1.append(pair[0])
        entries_mate_2.append(pair[1])

    # Write fastx formatted dataset
    fastq_mates_1 = list()
    fastq_mates_2 = list()
    for e in zip(entries_mate_1, entries_mate_2):
        fastq_mates_1 = fastq_mates_1 + e[0].unpack()
        fastq_mates_2 = fastq_mates_2 + e[1].unpack()
    fastq_joint = zip(fastq_mates_1,
                      fastq_mates_2)
    random.shuffle(fastq_joint)
    with open(dataset_name + '.1.fq', 'w') as fq1_out:
        with open(dataset_name + '.2.fq', 'w') as fq2_out:
            for i in range(len(fastq_joint)):
                fq1_out.writelines(['@read' + str(i) + '.1\n',
                                    fastq_joint[i][0][0] + '\n',
                                    '+\n',
                                    fastq_joint[i][0][1] + '\n'])
                fq2_out.writelines(['@read' + str(i) + '.2\n',
                                    fastq_joint[i][1][0] + '\n',
                                    '+\n',
                                    fastq_joint[i][1][1] + '\n'])

    # Write tagx formatted dataset
    entries.sort()
    with open(dataset_name + '.1.tagq', 'w') as tagq1_out:
        with open(dataset_name + '.2.tagq', 'w') as tagq2_out:
            for e in entries:
                pair = SequenceEntry.separate_mates(e)
                tagq1_out.write(pair[0].tag + '\t' +
                                pair[0].quality_average + '\t' +
                                str(e.occurrences) + '\n')
                tagq2_out.write(pair[1].tag + '\t' +
                                pair[1].quality_average + '\t' +
                                str(e.occurrences) + '\n')


def simulate_details_file(dataset_name,
                          dataset_n_samples,
                          read_min_occurrences,
                          read_max_occurrences,
                          dataset_size):
    # Simulate dataset content
    entries = DetailsEntry.simulate_n(dataset_n_samples,
                                      read_min_occurrences,
                                      read_max_occurrences,
                                      dataset_size)

    # Write tab file
    entries.sort()
    with open(dataset_name + '.tab', 'w') as tab_file:
        tab_file.write('Overall\t' +
                       '\t'.join(['Sample' + str(i)
                                  for i in range(n_samples)]) + '\n')
        for e in entries:
            tab_file.write(str(e.overall) + '\t' +
                           '\t'.join([str(occ)
                                      for occ in e.occurrences]) + '\n')


if __name__ == '__main__':
    # Datasets configuration.
    dataset_uniques = 2000
    tag_length = 50
    tag_min_occurrences = 1
    tag_max_occurrences = 20
    n_samples = 8

    print('# Automatic test data generation')
    print('# ===============================================================')
    print('# Dataset unique reads  : ' + str(dataset_uniques) + ' reads')
    print('# Reads size            : ' + str(tag_length) + ' bp')
    print('# Reads min occurrences : ' + str(tag_min_occurrences) + ' times')
    print('# Reads max occurrences : ' + str(tag_max_occurrences) + ' times')
    print('# N samples             : ' + str(n_samples) + ' samples')
    print('# ===============================================================')

    # Simulate datasets.
    print('# Simulate datasets')
    print('# \tsimulate single-end')
    simulate_single_end('../se',
                        tag_length,
                        tag_min_occurrences,
                        tag_max_occurrences,
                        dataset_uniques)
    print('# \tsimulate paired-end datasets')
    simulate_paired_end('../pe',
                        tag_length,
                        tag_min_occurrences,
                        tag_max_occurrences,
                        dataset_uniques)

    # Simulate occurrences arrays
    print('# \tsimulate occurrences arrays')
    simulate_details_file('../details',
                          n_samples,
                          tag_min_occurrences,
                          tag_max_occurrences,
                          dataset_uniques)

    # Finish.
    print('#\n# Finish')
