cmake_minimum_required(VERSION 3.23)

project(
  gw
  VERSION 1.0.0
  DESCRIPTION "A bunch of small C++ utilities"
  HOMEPAGE_URL "https://github.com/globberwops/gw"
  LANGUAGES CXX)

option(GW_BUILD_TESTS "Build tests" ${PROJECT_IS_TOP_LEVEL})
option(GW_INSTALL "Generate install target" ON)

option(GW_ENABLE_HASH_CALCULATION "Enable hash calculation" ON)
option(GW_ENABLE_STREAM_OPERATORS "Enable stream operators" ON)
option(GW_ENABLE_STRING_CONVERSION "Enable string conversion" ON)

add_library(StrongType INTERFACE)
add_library(gw::StrongType ALIAS StrongType)
target_sources(StrongType INTERFACE FILE_SET HEADERS BASE_DIRS include FILES include/gw/strong_type.hpp)
target_compile_features(StrongType INTERFACE cxx_std_20)
target_include_directories(StrongType INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
if(GW_ENABLE_HASH_CALCULATION)
  target_compile_definitions(StrongType INTERFACE GW_ENABLE_HASH_CALCULATION)
endif()
if(GW_ENABLE_STREAM_OPERATORS)
  target_compile_definitions(StrongType INTERFACE GW_ENABLE_STREAM_OPERATORS)
endif()
if(GW_ENABLE_STRING_CONVERSION)
  target_compile_definitions(StrongType INTERFACE GW_ENABLE_STRING_CONVERSION)
endif()

add_library(Crtp INTERFACE)
add_library(gw::Crtp ALIAS Crtp)
target_sources(Crtp INTERFACE FILE_SET HEADERS BASE_DIRS include FILES include/gw/crtp.hpp)
target_compile_features(Crtp INTERFACE cxx_std_20)
target_include_directories(Crtp INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)

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
    TARGETS StrongType Crtp
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
