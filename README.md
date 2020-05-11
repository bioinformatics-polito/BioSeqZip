BioSeqZip - An exact collapser for Second-Generation Sequencing datasets.
=========================================================================

*BioSeqZip* is a tool for performing exact collapsing of Second-Generation 
Sequencing datasets.

## Install from sources
*BioSeqZip* is publicly available on GitHub. For downloading and installing the most recent version of the tool, run the following instructions.

### Linux
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

### MacOS
MacOS users are asked to install Boost libraries and OpenMP support using `brew` (https://brew.sh/). Brew can be installed running the following command:
```
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install.sh)"
```

Once `brew` is installed, required dependencies can be installed running:
```
brew install boost
brew install libomp
```

At last, use the following CMake command:
```
git clone https://github.com/bioinformatics-polito/BioSeqZip.git bioseqzip

# Create a build directory
mkdir bioseqzip/build
cd bioseqzip/build

# Configure and build the tool
cmake .. -DCMAKE_BUILD_TYPE=Release                                                     \
         -DOpenMP_CXX_FLAGS="-Xpreprocessor -fopenmp -I$(brew --prefix libomp)/include" \
         -DOpenMP_CXX_LIB_NAMES="omp"                                                   \
         -DOpenMP_omp_LIBRARY=$(brew --prefix libomp)/lib/libomp.a
make
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
bioseqzip_collapse -i ERR040900.fastq -f fastq --csv-report
```

As a result, the *ERR030900* sample will be collapsed, reporting a FASTQ file
as output (`-f fastq`) along with a CSV report *collapsed.csv* (`--csv-report`) 
storing metadata about the collapsing procedure.

### Collapsing a set FASTQ files
*BioSeqZip* provides the capability of collapsing multiple samples, in the form of FASTQ 
files, at once. For exploiting this feature all samples to be collapsed have to be placed
in the same directory and the *BioSeqZip* collapser should be called with the `--input-dir`
option. In the following the typical usage for multi-sample collapsing is provided:

First of all, download a couple of FASTQ sample from the *BodyMap 2.0* dataset with the 
following command:
```
wget ftp://ftp.sra.ebi.ac.uk/vol1/fastq/ERR030/ERR030900/ERR030900.fastq.gz
wget ftp://ftp.sra.ebi.ac.uk/vol1/fastq/ERR030/ERR030901/ERR030901.fastq.gz
gzip -d ERR030900.fastq.gz
gzip -d ERR030901.fastq.gz
mkdir input
mv ERR030900.fastq input
mv ERR030901.fastq input
```

Collapsing it is as easy as typing:
```
bioseqzip_collapse --input-dir input -f fastq --csv-report
```

## Examples
The tool comes with a set of examples in the form of BASH script that the interested user can
have a look at, for better understanding how *BioSeqZip* can fit into their pipeline. Examples are in the 
`examples` directory:
- `01_collapsing_basics`: how to collapse single and paired-end samples and retrieving collapsing statistics such as runtime and reads/space gain.
- `02_crossformat_collapsing`: how to collapse data where the input and output dataset format does not match (e.g. fastq to fasta, fastq to tagq, etc...)
- `03_multisample_collapsing`: how to collapse more than one sample at once for merging sequences equal but belonging to different sequencing samples.
- `04_star_alignment`: a real-world example of a pipeline BioSeqZip is expected to work in, where a sample is downloaded from a remote database, collapsed, aligned with STAR and the output expanded again.
- `05_bwa_alignment`: a real-world example of a pipeline BioSeqZip is expected to work in, where a sample is downloaded from a remote database, collapsed, aligned with BWA-MEM and the output expanded again.
- `06_bodymap_analysis`: an example of how to download a set of samples from a remote server, aligning them with BWA-MEM and expanding BWA outputs.
