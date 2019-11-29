# BioSeqZip TUTORIAL - Analysis of all BodyMap 2.0 with BioSeqZip and BWA
# =======================================================================

# INTRODUCTION
# ------------
# This tutorial provide an example of how to create a real-world pipeline
# for analyzing all single-end 75bp samples in the BodyMap 2.0 dataset.

# WARNING !!! This tutorial will take some time and requires BWA to be 
#             installed on the machine the experiment is run on.

# Download BodyMap 2.0 single-end 75-bp samples
for i in $(seq 888 903)
do
	wget ftp://ftp.sra.ebi.ac.uk/vol1/fastq/ERR030/ERR030$i/ERR030$i.fastq.gz
	gzip -d ERR030$i.fastq.gz
done

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

# Collapse, align and expand downloaded samples
for i in $(seq 888 903)
do
	bioseqzip_collapse -i ERR030$i.fastq -f fastq -v 4
	bwa mem index/reference ERR030$i.collapsed.fq > ERR030$i.collapsed.bwa.sam
	bioseqzip_expand ERR030$i.collapsed.bwa.sam ERR030$i.expanded.bwa.sam
done

