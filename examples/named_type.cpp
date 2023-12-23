#include <gw/named_type.hpp>
#include <iostream>
#include <string>

using name_t = gw::named_type<std::string, "name">;
using address_t = gw::named_type<std::string, "address">;

void print_name(const name_t& name) { std::cout << name << '\n'; }

void print_address(const address_t& address) { std::cout << address << '\n'; }

auto main() -> int {
  auto name = name_t{"John Doe"};
  auto address = address_t{"123 Main St."};

  print_name(name);
  print_address(address);

  // The following lines will not compile, because the types are distinct.
  // print_name(address);
  // print_address(name);
}
