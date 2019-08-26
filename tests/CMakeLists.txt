# ===========================================================================
# tests/CMakeLists.txt
# ----------------------
#
# Module for creating the main BioSeqZip test targets.
# ---------------------------------------------------------------------------
#
# author : Emanuele Parisi
# ===========================================================================

include(${CMAKE_SOURCE_DIR}/cmake/FetchContent.cmake)

# ---------------------------------------------------------------------------
# Retrieve the external projects BioSeqZip tests depend on.
# ---------------------------------------------------------------------------

# Register Google test dependency.
fetchcontent_declare(bioseqzip_googletest
                     GIT_REPOSITORY
                     https://github.com/google/googletest.git
                     GIT_TAG
                     release-1.8.1)
fetchcontent_getproperties(bioseqzip_googletest)
if (NOT bioseqzip_googletest_POPULATED)
	fetchcontent_populate(bioseqzip_googletest)
	set(BUILD_GMOCK
	    CACHE BOOL
	    OFF)
	set(INSTALL_GTEST
	    CACHE BOOL
	    OFF)
	add_subdirectory(${bioseqzip_googletest_SOURCE_DIR}
	                 ${bioseqzip_googletest_BINARY_DIR})
endif ()

# ---------------------------------------------------------------------------
# Set the path to the units test data.
# ---------------------------------------------------------------------------

set (BIOSEQZIP_TEST_DATA_DIR
     "${CMAKE_CURRENT_SOURCE_DIR}/data")

# ---------------------------------------------------------------------------
# Configure the library top test target BioSeqZip::Test.
# ---------------------------------------------------------------------------

add_library(BioSeqZip_Test
            INTERFACE)
target_include_directories(BioSeqZip_Test
                           INTERFACE
                           ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(BioSeqZip_Test
                      INTERFACE
                      BioSeqZip
                      gtest_main
                      gtest)
target_compile_definitions(BioSeqZip_Test
                           INTERFACE
                           -DTEST_DATA_DIR="${BIOSEQZIP_TEST_DATA_DIR}"
                           -DTEST_FQ_SE="${BIOSEQZIP_TEST_DATA_DIR}/se.fq"
                           -DTEST_FQ_PE1="${BIOSEQZIP_TEST_DATA_DIR}/pe.1.fq"
                           -DTEST_FQ_PE2="${BIOSEQZIP_TEST_DATA_DIR}/pe.2.fq"
                           -DTEST_TQ_SE="${BIOSEQZIP_TEST_DATA_DIR}/se.tagq"
                           -DTEST_TQ_PE1="${BIOSEQZIP_TEST_DATA_DIR}/pe.1.tagq"
                           -DTEST_TQ_PE2="${BIOSEQZIP_TEST_DATA_DIR}/pe.2.tagq"
                           -DTEST_DETAILS="${BIOSEQZIP_TEST_DATA_DIR}/details.tab")

# ---------------------------------------------------------------------------
# Configure unit test targets.
# ---------------------------------------------------------------------------

add_subdirectory(units)
