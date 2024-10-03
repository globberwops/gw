include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG 6e79e682b726f524310d55dec8ddac4e9c52fb5f # v3.4.0
)
FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)

#
# inplace_string
#
add_executable(inplace_string_test)
target_sources(inplace_string_test PRIVATE inplace_string_test.cpp)
target_link_libraries(inplace_string_test PRIVATE Catch2::Catch2WithMain gw::inplace_string)
catch_discover_tests(inplace_string_test)

#
# named_type
#
add_executable(named_type_test)
target_sources(named_type_test PRIVATE named_type_test.cpp)
target_link_libraries(named_type_test PRIVATE Catch2::Catch2WithMain gw::named_type)
catch_discover_tests(named_type_test)

#
# strong_type
#
add_executable(strong_type_test)
target_sources(strong_type_test PRIVATE strong_type_test.cpp)
target_link_libraries(strong_type_test PRIVATE Catch2::Catch2WithMain gw::strong_type)
catch_discover_tests(strong_type_test)
