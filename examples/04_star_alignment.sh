# BioSeqZip TUTORIAL - Aligning a BodyMap 2.0 sample with STAR
# ============================================================

# INTRODUCTION
# ------------
# A practical use-case for sample collapsing is speeding-up data alignment
# avoiding aligning multiple time the same data. For providing a real-world
# example of a typical scenario where using BioSeqZip is beneficial we will
# try to emulate an analysis pipeline where a RNA-seq sample is downloaded, 
# collapsed and aligned with STAR, a well-known splicing-aware mapper. In the
# end the alignment output will be expanded again for making BioSeqZip 
# collapsing procedure almost transparent with respect to the analysis pipeline.

# WARNING !!! This tutorial will take some time and requires STAR to be 
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

# Build STAR index
mkdir index
STAR --runMode genomeGenerate \
     --genomeDir index \
     --genomeFastaFiles Homo_sapiens.GRCh38.ncrna.fa Homo_sapiens.GRCh38.cdna.all.fa \
     --limitGenomeGenerateRAM 153000000000

# Run STAR alignment
STAR --genomeDir index \
     --outStd SAM \
     --readFilesIn ERR030888.collapsed.fq > ERR030888.collapsed.star.sam 

# Expand alignment output for reproducing the correct number of occurrences 
# for each read aligned, making BioSeqZip transparent with respect to the 
# pipeline analysis.
bioseqzip_expand ERR030888.collapsed.star.sam ERR030888.expanded.star.sam

