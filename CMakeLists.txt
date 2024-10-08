cmake_minimum_required(VERSION 3.25)

project(
  gw
  VERSION 1.0.0
  DESCRIPTION "A bunch of small C++ utilities"
  HOMEPAGE_URL "https://github.com/globberwops/gw"
  LANGUAGES CXX)

option(GW_BUILD_DOCS "Build documentation" ${PROJECT_IS_TOP_LEVEL})
option(GW_BUILD_EXAMPLES "Build examples" ${PROJECT_IS_TOP_LEVEL})
option(GW_BUILD_TESTS "Build tests" ${PROJECT_IS_TOP_LEVEL})
option(GW_INSTALL "Generate install target" ON)
option(GW_USE_CLANG_TIDY "Use clang-tidy for static analysis" OFF)
option(GW_USE_CPPCHECK "Use cppcheck for static analysis" OFF)
option(GW_VERIFY_INTERFACE_HEADER_SETS "Verify interface header sets" ${PROJECT_IS_TOP_LEVEL})
set(GW_CXX_STANDARD
    20
    CACHE STRING "C++ standard to use")
set_property(CACHE GW_CXX_STANDARD PROPERTY STRINGS 20 23)

if(GW_USE_CLANG_TIDY)
  find_program(CLANG_TIDY_EXE NAMES clang-tidy clang-tidy-18)
  if(CLANG_TIDY_EXE)
    set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_EXE})
  else()
    message(WARNING "clang-tidy requested but executable not found")
  endif()
endif()

if(GW_USE_CPPCHECK)
  find_program(CPPCHECK_EXE NAMES cppcheck)
  if(CPPCHECK_EXE)
    set(CMAKE_CXX_CPPCHECK ${CPPCHECK_EXE})
  else()
    message(WARNING "cppcheck requested but executable not found")
  endif()
endif()

#
# gw::inplace_string
#
add_library(inplace_string INTERFACE)
add_library(gw::inplace_string ALIAS inplace_string)
target_sources(inplace_string INTERFACE FILE_SET HEADERS BASE_DIRS include FILES include/gw/inplace_string.hpp)
target_compile_features(inplace_string INTERFACE cxx_std_${GW_CXX_STANDARD})
target_include_directories(inplace_string INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
set_target_properties(inplace_string PROPERTIES VERIFY_INTERFACE_HEADER_SETS ${GW_VERIFY_INTERFACE_HEADER_SETS})

#
# gw::named_type
#
add_library(named_type INTERFACE)
add_library(gw::named_type ALIAS named_type)
target_sources(
  named_type
  INTERFACE FILE_SET
            HEADERS
            BASE_DIRS
            include
            FILES
            include/gw/concepts.hpp
            include/gw/named_type.hpp)
target_compile_features(named_type INTERFACE cxx_std_${GW_CXX_STANDARD})
target_include_directories(named_type INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
set_target_properties(named_type PROPERTIES VERIFY_INTERFACE_HEADER_SETS ${GW_VERIFY_INTERFACE_HEADER_SETS})

#
# gw::strong_type
#
add_library(strong_type INTERFACE)
add_library(gw::strong_type ALIAS strong_type)
target_sources(
  strong_type
  INTERFACE FILE_SET
            HEADERS
            BASE_DIRS
            include
            FILES
            include/gw/concepts.hpp
            include/gw/strong_type.hpp)
target_compile_features(strong_type INTERFACE cxx_std_${GW_CXX_STANDARD})
target_include_directories(strong_type INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
set_target_properties(strong_type PROPERTIES VERIFY_INTERFACE_HEADER_SETS ${GW_VERIFY_INTERFACE_HEADER_SETS})

#
# gw::crtp
#
add_library(crtp INTERFACE)
add_library(gw::crtp ALIAS crtp)
target_sources(crtp INTERFACE FILE_SET HEADERS BASE_DIRS include FILES include/gw/crtp.hpp)
target_compile_features(crtp INTERFACE cxx_std_23)
target_include_directories(crtp INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
set_target_properties(crtp PROPERTIES VERIFY_INTERFACE_HEADER_SETS ${GW_VERIFY_INTERFACE_HEADER_SETS})

if(GW_BUILD_DOCS)
  add_subdirectory(docs)
endif()

if(GW_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if(GW_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

if(GW_INSTALL)
  include(GNUInstallDirs)
  set(GW_INSTALL_INCLUDE_DIR
      ${CMAKE_INSTALL_INCLUDEDIR}
      CACHE PATH "Installation directory for header files")
  set(GW_INSTALL_CONFIG_DIR
      ${CMAKE_INSTALL_LIBDIR}/cmake/gw
      CACHE PATH "Installation directory for CMake package config files")

  include(CMakePackageConfigHelpers)

  configure_package_config_file(${PROJECT_SOURCE_DIR}/cmake/gw-config.cmake.in gw-config.cmake
                                INSTALL_DESTINATION ${GW_INSTALL_CONFIG_DIR})

  write_basic_package_version_file(
    gw-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

  install(
    TARGETS named_type strong_type crtp
    EXPORT gw-targets
    FILE_SET HEADERS
    COMPONENT gw-devel)

  install(
    EXPORT gw-targets
    FILE gw-targets.cmake
    NAMESPACE gw::
    DESTINATION ${GW_INSTALL_CONFIG_DIR})

  export(
    EXPORT gw-targets
    NAMESPACE gw::
    FILE gw-targets.cmake)

  install(
    FILES ${PROJECT_BINARY_DIR}/gw-config.cmake ${PROJECT_BINARY_DIR}/gw-config-version.cmake
    DESTINATION ${GW_INSTALL_CONFIG_DIR}
    COMPONENT gw-devel)
endif()
