# BioSeqZip TUTORIAL - Multi-samples collapsing
# =============================================

# INTRODUCTION
# ------------
# BioSeqZip enables the user to perform multi-samples collapsing, meaning that
# multiple samples are analyzed at once and collapsed to a single output file,
# providing further data dimensionality reduction. This feature is especially
# useful for RNA-seq analysis pipeline, where often more than one sample is
# analyzed for each experiment.

# SINGLE-END MULTI-SAMPLES COLLAPSING
# -----------------------------------
# For providing an example of multi-samples collapsing, let's try to download
# 3 distinct BioSeqZip single-end samples and to collapse all three in a single
# step.

# Download three BodyMap 2.0 single-end samples
wget ftp://ftp.sra.ebi.ac.uk/vol1/fastq/ERR030/ERR030888/ERR030888.fastq.gz
wget ftp://ftp.sra.ebi.ac.uk/vol1/fastq/ERR030/ERR030889/ERR030889.fastq.gz
wget ftp://ftp.sra.ebi.ac.uk/vol1/fastq/ERR030/ERR030890/ERR030890.fastq.gz
gzip -d ERR030888.fastq.gz
gzip -d ERR030889.fastq.gz
gzip -d ERR030890.fastq.gz

# For the sake of multi-samples collapsing all samples to be considered 
# should be in a separate directory.
mkdir SE
mv ERR030888.fastq SE 
mv ERR030889.fastq SE
mv ERR030890.fastq SE

# Collapse all single-end samples in SE directory at once
bioseqzip_collapse --input-dir SE -f fastq -v 4 --csv-report

# As soon as the collapsing procedure is finished, you should see a file named
# SE.collapsed.fq which is the collapsed version of all original raw samples 
# provided as input to BioSeqZip. As usual, the CSV report provided by 
# BioSeqZip stores valuable information regarding the collapse procedure.

# Print collapsing meta-data
cat SE.collapsed.csv

# When performing multi-samples collapsing a further TAB file is provided by 
# BioSeqZip reporting the detail of how many time each read was found in which 
# sample. This is useful when performing some kind of differential analysis or
# for checking what sequence is more relevant for each sample.
cat SE.collapsed.tab

# PAIRED-END MULTI-SAMPLES COLLAPSING
# -----------------------------------
# The same functionality is provided for paired-end samples. Let's perform the 
# same analysis for three paired-end samples from the BodyMap 2.0 dataset.

# Download three BodyMap 2.0 paired-end samples
wget ftp://ftp.sra.ebi.ac.uk/vol1/fastq/ERR030/ERR030872/ERR030872_1.fastq.gz
wget ftp://ftp.sra.ebi.ac.uk/vol1/fastq/ERR030/ERR030872/ERR030872_2.fastq.gz
wget ftp://ftp.sra.ebi.ac.uk/vol1/fastq/ERR030/ERR030872/ERR030873_1.fastq.gz
wget ftp://ftp.sra.ebi.ac.uk/vol1/fastq/ERR030/ERR030872/ERR030873_2.fastq.gz
wget ftp://ftp.sra.ebi.ac.uk/vol1/fastq/ERR030/ERR030872/ERR030874_1.fastq.gz
wget ftp://ftp.sra.ebi.ac.uk/vol1/fastq/ERR030/ERR030872/ERR030874_2.fastq.gz
gzip -d ERR030872_1.fastq.gz
gzip -d ERR030872_2.fastq.gz
gzip -d ERR030873_1.fastq.gz
gzip -d ERR030873_2.fastq.gz
gzip -d ERR030874_1.fastq.gz
gzip -d ERR030874_2.fastq.gz

# For the sake of paired-end multi-samples collapsing a match file has to be 
# provided reporting paths to the target files.
touch PE.txt
echo "ERR030872_1.fastq,ERR030872_2.fastq" >> PE.txt
echo "ERR030873_1.fastq,ERR030873_2.fastq" >> PE.txt
echo "ERR030874_1.fastq,ERR030874_2.fastq" >> PE.txt

# Collapse all single-end samples in SE directory at once
bioseqzip_collapse --match PE.txt -f fastq -v 4 --csv-report

