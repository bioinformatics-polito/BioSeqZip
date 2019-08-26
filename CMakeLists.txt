# ===========================================================================
# CMakeLists.txt
# --------------
#
# Top-level CMake build configuration file. It defines project meta-data
# and decides what sub directories have to be considered for the
# configuration process.
# ---------------------------------------------------------------------------
#
# author : Emanuele Parisi
# ===========================================================================

cmake_minimum_required(VERSION
                       3.7.2)

# ---------------------------------------------------------------------------
# Initialize project configuration.
# ---------------------------------------------------------------------------

project(BioSeqZip
        LANGUAGES CXX
        VERSION 0.0.0.0)
set(BioSeqZip_DESCRIPTION
    "Tools for treating Next-Generation Sequencing datasets redundancy.")
set(BioSeqZip_HOMEPAGE_URL
    "https://philae.polito.it/gitlab/biotools/bioseqzip")
set(CMAKE_CXX_STANDARD
    14)
set(CMAKE_CXX_STANDARD_REQUIRED
    YES)

# ---------------------------------------------------------------------------
# Select the proper default build type.
# ---------------------------------------------------------------------------

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE
	    "Release"
	    CACHE STRING
	    "The default project build type is 'Release'")
endif ()
message(STATUS
        "CMake build type is ${CMAKE_BUILD_TYPE}")

# ---------------------------------------------------------------------------
# Configure BioSeqZip project configuration.
# ---------------------------------------------------------------------------

option(BioSeqZip_BUILD_SUITE
       "Choose if BioSeqZip suite applications have to be configured" ON)
option(BioSeqZip_BUILD_TESTS
       "Choose if BioSeqZip suite tests have to be configured" OFF)
option(BioSeqZip_BUILD_DOCS
       "Choose if BioSeqZip suite documentation has to be configured" OFF)

# ---------------------------------------------------------------------------
# Configure BioSeqZip library targets.
# ---------------------------------------------------------------------------

add_subdirectory(include)

# ---------------------------------------------------------------------------
# Check what of the optional BioSeqZip project targets have to be
# configured.
# ---------------------------------------------------------------------------

if (${BioSeqZip_BUILD_SUITE})
	add_subdirectory(apps)
else ()
	message(STATUS
	        "User chose not to build BioSeqZip suite applications")
endif ()

if (${BioSeqZip_BUILD_TESTS})
	add_subdirectory(tests)
else ()
	message(STATUS
	        "User chose not to build BioSeqZip tests")
endif ()

if (${BioSeqZip_BUILD_DOCS})
	add_subdirectory(docs)
else ()
	message(STATUS
	        "User chose not to build BioSeqZip documentation")
endif ()
