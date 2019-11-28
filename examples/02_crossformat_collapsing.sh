# BioSeqZip TUTORIAL - Cross-format collapsing 
# ============================================

# INTRODUCTION
# ------------
# The BioSeqZip collapser allows cross-format sample collapsing, meaning that
# the output data format is not necessarily equal to the input one. This 
# feature can be used for translating output data from fastq to fasta format
# for reducing output files size in case qualities are not required for
# post-analysis. Another use case is transforming data from fasta to tag 
# which is a custom BioSeqZip ASCII format reporting only the read and the 
# number of time it is found in the input file, omitting the sequence header.

# FASTQ TO FASTA
# --------------
# Let's see how to collapse a BodyMap 2.0 sample translating it to fasta 
# format.

# Download a BodyMap 2.0 sample
wget ftp://ftp.sra.ebi.ac.uk/vol1/fastq/ERR030/ERR030888/ERR030888.fastq.gz
gzip -d ERR030888.fastq.gz

# Collapse a fastq sample translating it to fasta
bioseqzip_collapse -i ERR030888.fastq -f fasta -v 4 --csv-format

# As soon as the collapsing procedure is finished, you should see a file named
# ERR030888.collapsed.fa which is the collapsed version of the original raw
# file belonging to the BodyMap 2.0 dataset without quality information. As it
# was expected, not only the output file is smaller than the fastq equivalent
# but also the collapsing procedure is faster (check it from the CSV report) 
# because no manipulations are performed over qualities.

# FASTQ TO TAGQ
# -------------
# Let's try to collapse a fastq sample writing the output in tag format for 
# keeping quality information avoding wasting space reporting read headers.

# Collapse a fastq sample translating it to tagq
bioseqzip_collapse -i ERR030888.fastq -f tagq -v 4 --csv-format

