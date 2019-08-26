# ===========================================================================
# tests/units/CMakeLists.txt
# --------------------------
#
# CMakeLists.txt in charge of creating the actual unit test targets. A
# single target for each library component is configured.
# --------------------------------------------------------------------
#
# author : Emanuele Parisi
# ===========================================================================

# ---------------------------------------------------------------------------
# Enable testing.
# ---------------------------------------------------------------------------

enable_testing()

# ---------------------------------------------------------------------------
# Configure unit test targets.
# ---------------------------------------------------------------------------

add_executable(bioseqzip_units_bioseqzip
               test_buffer.cpp
               main.cpp)
target_link_libraries(bioseqzip_units_bioseqzip
                      PUBLIC
                      BioSeqZip_Test)
