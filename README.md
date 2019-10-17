BioSeqZip - An exact collapser for Second-Generation Sequencing datasets.
=========================================================================

*BioSeqZip* is a tool for performing exact collapsing of Second-Generation 
Sequencing datasets.

## Install from sources
*BioSeqZip* is publicly available on GitHub. For downloading and installing
the most recent version of the tool, follow these instructions:
```
# Download the sources from GitHub
git clone https://github.com/bioinformatics-polito/BioSeqZip.git bioseqzip

# Create a build directory
mkdir bioseqzip/build
cd bioseqzip/build

# Configure and build the tool
cmake .. -DCMAKE_BUILD_TYPE=Release
make

# Move the binaries where you prefer
cp apps/bioseqzip* /usr/local/bin
```

## Usage
For having an idea of the capabilities of *BioSeqZip* you can print the
tool usage instructions with:
```
bioseqzip_collapse --help
```

### Collapsing FASTQ files
A typical task may be to collapse a FASTQ file, from the *BodyMap 2.0* ensemble, a 
very well-known dataset containing both single and paired-end sequencing files.

First of all, download a FASTQ sample from the *BodyMap 2.0* dataset with the 
following command:
```
wget ftp://ftp.sra.ebi.ac.uk/vol1/fastq/ERR030/ERR030900/ERR030900.fastq.gz
gzip -d ERR030900.fastq.gz
```

Collapsing it is as easy as typing:
```
bioseqzip_collapse -i ERR040900.fastq -f fastq -b collapsed --csv-report
```

As a result, the *ERR030900* sample will be collapsed, reporting a FASTQ file
as output (`-f fastq`) whose name will be *collapsed.fastq* (`-b collapsed`) 
along with a CSV report *collapsed.csv* (`--csv-report`) storing metadata about
the collapsing procedure.
