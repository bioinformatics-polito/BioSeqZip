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

