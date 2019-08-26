import random


class DetailsEntry:
    @staticmethod
    def simulate(n_samples,
                 min_occurrences,
                 max_occurrences):
        occurrences = list()
        for i in range(n_samples):
            occurrences.append(random.randint(min_occurrences,
                                              max_occurrences))

        return DetailsEntry(occurrences)

    @staticmethod
    def simulate_n(n_samples,
                   min_occurrences,
                   max_occurrences,
                   dataset_size):
        dataset = list()
        for i in range(dataset_size):
            dataset.append(DetailsEntry.simulate(n_samples,
                                                 min_occurrences,
                                                 max_occurrences))

        return dataset

    def __init__(self,
                 occurrences):
        self._occurrences = occurrences
        self._overall = sum(occurrences)

    @property
    def occurrences(self):
        return self._occurrences

    @property
    def overall(self):
        return self._overall
