cmake_minimum_required(VERSION 3.28)

option(ETHERBLOCKS_BUILD_TESTS "Build tests" ON)
option(ETHERBLOCKS_BUILD_DOCS "Build API documentation with Doxygen" OFF)
option(ETHERBLOCKS_ENABLE_INSTALL "Generate install rules" ON)

if(ETHERBLOCKS_BUILD_TESTS)
   list(APPEND VCPKG_MANIFEST_FEATURES tests)
   list(REMOVE_DUPLICATES VCPKG_MANIFEST_FEATURES)
endif()

project(
   etherblocks
   VERSION 0.1.0
   DESCRIPTION "A first-person construction game"
   LANGUAGES CXX)

configure_file("${PROJECT_SOURCE_DIR}/cmake/version.hpp.in" "${PROJECT_BINARY_DIR}/include/rude/version.hpp" @ONLY)

add_subdirectory(src)

add_subdirectory(app)

if(ETHERBLOCKS_BUILD_TESTS)
   enable_testing()
   add_subdirectory(tests)
endif()

if(ETHERBLOCKS_BUILD_DOCS)
   find_package(Doxygen QUIET)
   if(DOXYGEN_FOUND)
      set(DOXYGEN_INPUT_DIR "${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/docs")
      set(DOXYGEN_OUTPUT_DIR "${PROJECT_BINARY_DIR}/docs")
      configure_file("${CMAKE_SOURCE_DIR}/cmake/Doxyfile.in" "${PROJECT_BINARY_DIR}/Doxyfile" @ONLY)

      add_custom_target(
         docs
         COMMAND ${DOXYGEN_EXECUTABLE} "${PROJECT_BINARY_DIR}/Doxyfile"
         WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
         COMMENT "Generating API documentation with Doxygen"
         VERBATIM)
   else()
      message(STATUS "Doxygen not found; docs target disabled")
   endif()
endif()
