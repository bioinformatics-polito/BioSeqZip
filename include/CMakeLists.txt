# ===========================================================================
# include/CMakeLists.txt
# ----------------------
#
# Module for creating the BioSeqZip library core target.
# ---------------------------------------------------------------------------
#
# author : Emanuele Parisi
# ===========================================================================

include(${CMAKE_SOURCE_DIR}/cmake/FetchContent.cmake)

# ---------------------------------------------------------------------------
# Retrieve the external projects BioSeqZip depends on.
# ---------------------------------------------------------------------------

# Register SeqAn dependency.
fetchcontent_declare(bioseqzip_seqan
                     GIT_REPOSITORY
                     https://github.com/seqan/seqan.git
                     GIT_TAG
                     seqan-v2.4.0)
fetchcontent_getproperties(bioseqzip_seqan)
if (NOT bioseqzip_seqan_POPULATED)
	fetchcontent_populate(bioseqzip_seqan)
	list(APPEND
	     CMAKE_INCLUDE_PATH
	     ${CMAKE_INCLUDE_PATH}
	     ${bioseqzip_seqan_SOURCE_DIR}/include)
	set(SEQAN_FIND_DEPENDENCIES
	    "DEFAULT")
	find_package(SEQAN
	             REQUIRED
	             HINTS ${bioseqzip_seqan_SOURCE_DIR}/util/cmake)
endif ()
add_library(BioSeqZip_SeqAn
            INTERFACE)
target_include_directories(BioSeqZip_SeqAn
                           INTERFACE
                           ${SEQAN_INCLUDE_DIRS})
target_link_libraries(BioSeqZip_SeqAn
                      INTERFACE
                      ${SEQAN_LIBRARIES})
target_compile_definitions(BioSeqZip_SeqAn
                           INTERFACE
                           ${SEQAN_DEFINITIONS}
                           -DSEQAN_DISABLE_VERSION_CHECK=ON)
separate_arguments(SEQAN_SEPARATED_CXX_FLAGS
                   UNIX_COMMAND
                   ${SEQAN_CXX_FLAGS})
target_compile_options(BioSeqZip_SeqAn
                       INTERFACE
                       ${SEQAN_SEPARATED_CXX_FLAGS})

# Register CppUtilities dependency.
fetchcontent_declare(bioseqzip_cpputils
                     GIT_REPOSITORY
                     https://philae.polito.it/gitlab/parisi/cpputils.git
                     GIT_TAG
                     v0.1.2.0)
fetchcontent_getproperties(bioseqzip_cpputils)
if (NOT bioseqzip_cpputils_POPULATED)
	fetchcontent_populate(bioseqzip_cpputils)
	add_subdirectory(${bioseqzip_cpputils_SOURCE_DIR}
	                 ${bioseqzip_cpputils_BINARY_DIR})
endif ()

# ---------------------------------------------------------------------------
# Configure the library global target BioSeqZip::BioSeqZip.
# ---------------------------------------------------------------------------

add_library(BioSeqZip
            INTERFACE)
target_include_directories(BioSeqZip
                           INTERFACE
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(BioSeqZip
                      INTERFACE
                      BioSeqZip_SeqAn
                      CppUtils
                      pthread
                      stdc++fs
                      -fopenmp)
target_compile_definitions(BioSeqZip
                           INTERFACE
                           -DBioSeqZip_VERSION="${BioSeqZip_VERSION}")
