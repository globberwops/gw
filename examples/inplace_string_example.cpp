#include <format>
#include <gw/inplace_string.hpp>
#include <iostream>
#include <stdexcept>
#include <type_traits>

void print(const auto& str) { std::cout << std::format("{}\n", str); }

auto main() -> int {
  try {
    constexpr auto name = gw::inplace_string{"John Doe"};
    constexpr auto address = gw::inplace_string{"123 Main St."};

    print(name);
    print(address);

    // gw::inplace_strings are trivial
    static_assert(std::is_trivial_v<decltype(name)>);
    static_assert(std::is_trivial_v<decltype(address)>);

  } catch (const std::length_error& ex) {
    std::cerr << ex.what() << '\n';
  }
}
