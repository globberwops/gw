#include <format>
#include <gw/inplace_string.hpp>
#include <gw/named_type.hpp>
#include <iostream>
#include <stdexcept>
#include <type_traits>

static constexpr auto k_string_size = 20U;
using name_t = gw::named_type<gw::inplace_string<k_string_size>, "name">;
using address_t = gw::named_type<gw::inplace_string<k_string_size>, "address">;

// gw::named_types are distinct types
static_assert(!std::is_same_v<name_t, address_t>);

void print_name(const name_t& name) { std::cout << std::format("{}\n", name); }

void print_address(const address_t& address) { std::cout << std::format("{}\n", address); }

auto main() -> int {
  try {
    constexpr auto name = name_t{"John Doe"};
    constexpr auto address = address_t{"123 Main St."};

    print_name(name);
    print_address(address);

  } catch (const std::length_error& ex) {
    std::cerr << ex.what() << '\n';
  }
}
