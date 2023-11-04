include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG 6e79e682b726f524310d55dec8ddac4e9c52fb5f # v3.4.0
)

FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)

add_executable(StrongTypeTest)
target_sources(StrongTypeTest PRIVATE strong_type_test.cpp)
target_link_libraries(StrongTypeTest PRIVATE Catch2::Catch2WithMain gw::StrongType)

catch_discover_tests(StrongTypeTest)
