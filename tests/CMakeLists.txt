add_executable(StrongTypeTest)
target_sources(StrongTypeTest PRIVATE strong_type_test.cpp)
target_link_libraries(StrongTypeTest PUBLIC gw::StrongType)

add_test(NAME StrongTypeTest COMMAND StrongTypeTest)
