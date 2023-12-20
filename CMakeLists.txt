cmake_minimum_required(VERSION 3.23)

project(
  gw
  VERSION 1.0.0
  DESCRIPTION "A bunch of small C++ utilities"
  HOMEPAGE_URL "https://github.com/globberwops/gw"
  LANGUAGES CXX)

option(GW_BUILD_DOCS "Build documentation" ${PROJECT_IS_TOP_LEVEL})
option(GW_BUILD_TESTS "Build tests" ${PROJECT_IS_TOP_LEVEL})
option(GW_INSTALL "Generate install target" ON)

add_library(named_type INTERFACE)
add_library(gw::named_type ALIAS named_type)
target_sources(named_type INTERFACE FILE_SET HEADERS BASE_DIRS include FILES include/gw/named_type.hpp)
target_compile_features(named_type INTERFACE cxx_std_20)
target_include_directories(named_type INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)

add_library(strong_type INTERFACE)
add_library(gw::strong_type ALIAS strong_type)
target_sources(strong_type INTERFACE FILE_SET HEADERS BASE_DIRS include FILES include/gw/strong_type.hpp)
target_compile_features(strong_type INTERFACE cxx_std_20)
target_include_directories(strong_type INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)

add_library(crtp INTERFACE)
add_library(gw::crtp ALIAS crtp)
target_sources(crtp INTERFACE FILE_SET HEADERS BASE_DIRS include FILES include/gw/crtp.hpp)
target_compile_features(crtp INTERFACE cxx_std_20)
target_include_directories(crtp INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)

if(GW_BUILD_DOCS)
  add_subdirectory(docs)
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
    TARGETS strong_type crtp
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
