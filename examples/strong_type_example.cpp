#include <format>
#include <gw/strong_type.hpp>
#include <iostream>
#include <string>
#include <type_traits>

using name_t = gw::strong_type<std::string, struct name_tag>;
using address_t = gw::strong_type<std::string, struct address_tag>;

void print_name(const name_t& name) { std::cout << std::format("{}\n", name); }

void print_address(const address_t& address) { std::cout << std::format("{}\n", address); }

auto main() -> int {
  auto name = name_t{"John Doe"};
  auto address = address_t{"123 Main St."};

  print_name(name);
  print_address(address);

  // gw::strong_types are distinct types
  static_assert(!std::is_same_v<name_t, address_t>);
}
