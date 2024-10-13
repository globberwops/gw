#include <format>
#include <gw/inplace_string.hpp>
#include <iostream>
#include <type_traits>

void print(const auto& str) { std::cout << std::format("{}\n", str); }

auto main() -> int {
  using gw::inplace_string;

  auto name = inplace_string{"John Doe"};
  auto address = inplace_string{"123 Main St."};

  print(name);
  print(address);

  // gw::inplace_strings are trivial
  static_assert(std::is_trivial_v<decltype(name)>);
  static_assert(std::is_trivial_v<decltype(address)>);
}
