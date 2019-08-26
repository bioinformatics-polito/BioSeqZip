# ===========================================================================
# apps/CMakeLists.txt
# ----------------------
#
# Module for configuring applications part of the BioSeqZip application
# suite.
# ---------------------------------------------------------------------------
#
# author : Emanuele Parisi
# ===========================================================================

include(GNUInstallDirs)

# ---------------------------------------------------------------------------
# Configure an target for every application of the BioSeqZip suite.
# ---------------------------------------------------------------------------

add_executable(bioseqzip_collapse
               collapse/collapse.cpp)
target_link_libraries(bioseqzip_collapse
                      PUBLIC
                      BioSeqZip)
target_compile_definitions(bioseqzip_collapse
                           INTERFACE
                           -DBIOSEQZIP_VERSION=${BioSeqZip_VERSION})
target_compile_options(bioseqzip_collapse
                       INTERFACE
                       -march=native)

add_executable(bioseqzip_expand
               expand/expand.cpp)
target_link_libraries(bioseqzip_expand
                      PUBLIC
                      BioSeqZip)
target_compile_definitions(bioseqzip_expand
                           INTERFACE
                           -DBIOSEQZIP_VERSION=${BioSeqZip_VERSION})
target_compile_options(bioseqzip_collapse
                       INTERFACE
                       -march=native)

# ---------------------------------------------------------------------------
# Configure application suite installation
# ---------------------------------------------------------------------------

install(TARGETS
        bioseqzip_collapse
        bioseqzip_expand
        DESTINATION
        ${CMAKE_INSTALL_BINDIR})
