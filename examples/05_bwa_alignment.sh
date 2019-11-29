# BioSeqZip TUTORIAL - Aligning a BodyMap 2.0 sample with BWA
# ===========================================================

# INTRODUCTION
# ------------
# A practical use-case for sample collapsing is speeding-up data alignment
# avoiding aligning multiple time the same data. For providing a real-world
# example of a typical scenario where using BioSeqZip is beneficial we will
# try to emulate an analysis pipeline where a RNA-seq sample is downloaded, 
# collapsed and aligned with BWA, one of the most well-known short-reads mapper. 
# In the end the alignment output will be expanded again for making BioSeqZip 
# collapsing procedure almost transparent with respect to the analysis pipeline.

# WARNING !!! This tutorial will take some time and requires BWA to be 
#             installed on the machine the experiment is run on.

# Download a BodyMap 2.0 single-end 75-bp sample
wget ftp://ftp.sra.ebi.ac.uk/vol1/fastq/ERR030/ERR030888/ERR030888.fastq.gz
gzip -d ERR030888.fastq.gz

# Collapse the sample to be analyzed
bioseqzip_collapse -i ERR030888.fastq -f fastq -v 4 --csv-report

# Download reference
wget ftp://ftp.ensembl.org/pub/release-98/fasta/homo_sapiens/cdna/Homo_sapiens.GRCh38.cdna.all.fa.gz
wget ftp://ftp.ensembl.org/pub/release-98/fasta/homo_sapiens/ncrna/Homo_sapiens.GRCh38.ncrna.fa.gz
gzip -d Homo_sapiens.GRCh38.cdna.all.fa.gz 
gzip -d Homo_sapiens.GRCh38.ncrna.fa.gz
cat Homo_sapiens.GRCh38.ncrna.fa > reference.fa
cat Homo_sapiens.GRCh38.cdna.all.fa >> reference.fa

# Build BWA index
mkdir index
cd index
bwa index ../reference.fa
cd ..

# Run BWA alignment
bwa mem index/reference ERR030888.collapsed.fq > ERR030888.collapsed.bwa.sam

# Expand alignment output for reproducing the correct number of occurrences 
# for each read aligned, making BioSeqZip transparent with respect to the 
# pipeline analysis.
bioseqzip_expand ERR030888.collapsed.bwa.sam ERR030888.expanded.bwa.sam

