# ===========================================================================
# docs/CMakeLists.txt
# -------------------
#
# CMake module for configuring the build documentation target. This target
# is optional and the user can choose not to build it settings the
# BioSeqZip_BUILD_DOCS cache variable to NO. If the documentation has to be
# built, doxygen is required.
# ------------------------------------------------------------------------
# 
# author : Emanuele Parisi
# ===========================================================================

# ---------------------------------------------------------------------------
# Look for required packages.
# ---------------------------------------------------------------------------

message (STATUS "Searching for doxygen.")
find_package (Doxygen
              REQUIRED)

if (NOT ${DOXYGEN_FOUND})
	message (FATAL_ERROR
	         "Could not find doxygen. Not building documentation.")
endif ()

if (NOT ${DOXYGEN_DOT_FOUND})
	message (STATUS
	         "Could not find dot tool. Disabling dot support.")
	set (BioSeqZip_DOXYGEN_HAVE_DOT "NO")
else ()
	message (STATUS
	         "Found dot tool. Enabling dot support.")
	set (BioSeqZip_DOXYGEN_HAVE_DOT "YES")
endif ()

# ---------------------------------------------------------------------------
# Configure and create documentation target.
# ---------------------------------------------------------------------------

message (STATUS
         "Configuring documentation target.")
set(BioSeqZip_DOXYFILE_IN
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set (BioSeqZip_DOXYGEN_OUTPUT_DIR
     ${CMAKE_CURRENT_SOURCE_DIR})
set (BioSeqZip_DOXYGEN_INCLUDE_DIR
     ${CMAKE_SOURCE_DIR}/include)
configure_file (${BioSeqZip_DOXYFILE_IN}
                ${BioSeqZip_DOXYGEN_OUTPUT_DIR}/Doxyfile)
add_custom_target(bioseqzip_documentation
                  COMMAND
                  ${DOXYGEN_EXECUTABLE}
                  WORKING_DIRECTORY
                  ${BioSeqZip_DOXYGEN_OUTPUT_DIR}
                  COMMENT
                  "Generating API documentation with Doxygen"
                  VERBATIM)
