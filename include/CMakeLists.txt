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

include(ExternalProject)

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

# Register Boost dependency.
if (APPLE)
    ExternalProject_Add(bioseqzip_boost
                        GIT_REPOSITORY "http://github.com/boostorg/boost"
                        GIT_TAG boost-1.73.0
                        CONFIGURE_COMMAND "./bootstrap.sh --prefix=${CMAKE_BINARY_DIR}/boost"
                        BUILD_COMMAND "./b2"
                        INSTALL_COMMAND "./b2 install")
    link_directories(${CMAKE_BINARY_DIR}/boost/lib/boost/)
    include_directories(${CMAKE_BINARY_DIR}/boost/include/)
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
if (APPLE)
    target_link_libraries(BioSeqZip
                          INTERFACE
                          BioSeqZip_SeqAn
                          pthread
                          boost_filesystem
                          -fopenmp)
    target_compile_definitions(BioSeqZip
                               INTERFACE
                               -DBioSeqZip_VERSION="${BioSeqZip_VERSION}"
                               -DBioSeqZip_APPLE=1)
else ()
    target_link_libraries(BioSeqZip
                          INTERFACE
                          BioSeqZip_SeqAn
                          pthread
                          stdc++fs
                          -fopenmp)
    target_compile_definitions(BioSeqZip
                               INTERFACE
                               -DBioSeqZip_VERSION="${BioSeqZip_VERSION}"
                               -DBioSeqZip_APPLE=0)
endif ()
