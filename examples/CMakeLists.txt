#
# inplace_string
#
add_executable(inplace_string_example)
target_sources(inplace_string_example PRIVATE inplace_string_example.cpp)
target_link_libraries(inplace_string_example PRIVATE gw::inplace_string)

#
# named_type
#
add_executable(named_type_example)
target_sources(named_type_example PRIVATE named_type_example.cpp)
target_link_libraries(named_type_example PRIVATE gw::named_type)

#
# strong_type
#
add_executable(strong_type_example)
target_sources(strong_type_example PRIVATE strong_type_example.cpp)
target_link_libraries(strong_type_example PRIVATE gw::strong_type)
