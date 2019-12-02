# BioSeqZip TUTORIAL - Collapsing basics
# ======================================

# INTRODUCTION
# ------------
# BioSeqZip is a tool which performs exact collapsing of biological samples. It
# allows performing analysis on samples smaller than the original ones, with 
# losing precious information.
# As a first introductory example, it will be shown how to collapse a single
# biological sample retrieved from the BodyMap 2.0 database. We will perform
# the same experiment for both single-end and paired-end data as both layouts
# are frequently used in computational biology pipelines.

# COLLAPSING SINGLE-END SAMPLES
# -----------------------------
# First, it will be shown how to retrieve and collapse a single-end sample from
# the BodyMap 2.0 dataset.

# Download a single-end sample
wget ftp://ftp.sra.ebi.ac.uk/vol1/fastq/ERR030/ERR030888/ERR030888.fastq.gz
gzip -d ERR030888.fastq.gz

# Collapse a single-end sample with BioSeqZip
bioseqzip_collapse -i ERR030888.fastq -f fastq -v 4 --csv-report

# As soon as the collapsing procedure is finished, you should see a file named
# ERR030888.collapsed.fq which is the collapsed version of the original raw
# file belonging to the BodyMap 2.0 dataset. As you can see from the command
# line arguments used, we also asked BioSeqZip to print a CSV report of the 
# collapsing procedure which reports the number of raw and collapsed reads,
# the collapse gain obtained and some runtime information.

# Print collapsing meta-data
cat ERR030888.collapsed.csv

# Notice that meta-data generation can be switch-off simply by not using the 
# --csv-report flag when calling BioSeqZip.

# COLLAPSING PAIRED-END SAMPLES
# -----------------------------
# BioSeqZip is able to deal also with paired-end samples: for demostrating it, 
# let's download a paired-end sample from the BodyMap 2.0 dataset.

# Download a paired-end sample
wget ftp://ftp.sra.ebi.ac.uk/vol1/fastq/ERR030/ERR030882/ERR030882_1.fastq.gz
wget ftp://ftp.sra.ebi.ac.uk/vol1/fastq/ERR030/ERR030882/ERR030882_2.fastq.gz
gzip -d ERR030882_1.fastq.gz
gzip -d ERR030882_2.fastq.gz

# Collapse a paired-end sample with BioSeqZip
bioseqzip_collapse -i ERR030882_1.fastq -p ERR030882_2 -f fastq -v 4 --csv-report

# Once the collapsing procedure is finished, you should see two files named
# ERR030882_1.collapsed.fq and ERR030882_2.fq which are the collapsed version 
# of the original raw sample belonging to the BodyMap 2.0 dataset. Again, a CSV 
# report is written, reporting some meta-information regarding the collapsing 
# procedure.

# Print collapsing meta-data
cat ERR030888.collapsed.csv

# RUNNING GZIP AFTER COLLAPSING
# -----------------------------
# For allowing even better collapsing performances, BioSeqZip gives the user the
# capability of running Z compression on each output file the collapser produced
# running gzip. This feature is triggered specifying the proper flag on the command
# line.
bioseqzip_collapse -i ERR030888.fastq -f fastq -v 4 -g

