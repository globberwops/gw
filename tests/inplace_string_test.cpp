// Copyright (c) 2023 Martin Stump
// SPDX-License-Identifier: BSL-1.0

#include "gw/inplace_string.hpp"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <cstdio>
#include <cstring>
#include <string_view>
#include <type_traits>

namespace gw {

TEST_CASE("inplace_string is default constructed", "[inplace_string]") {
  constexpr auto k_value = inplace_string<10UZ>{};

  STATIC_REQUIRE(k_value.empty());
  STATIC_REQUIRE(k_value.size() == 0UZ);
  STATIC_REQUIRE(k_value.length() == 0UZ);
  STATIC_REQUIRE(k_value.max_size() == 10UZ);
  STATIC_REQUIRE(k_value.capacity() == 10UZ);
  STATIC_REQUIRE(k_value == "");

  STATIC_REQUIRE(std::is_trivially_copyable_v<decltype(k_value)>);
}

TEST_CASE("inplace_string is constructed from a character", "[inplace_string]") {
  constexpr auto k_value = inplace_string<10UZ>{5UZ, 'A'};

  STATIC_REQUIRE(!k_value.empty());
  STATIC_REQUIRE(k_value.size() == 5UZ);
  STATIC_REQUIRE(k_value.length() == 5UZ);
  STATIC_REQUIRE(k_value.max_size() == 10UZ);
  STATIC_REQUIRE(k_value.capacity() == 10UZ);
  STATIC_REQUIRE(k_value == "AAAAA");
}

TEST_CASE("inplace_string is constructed from a string literal", "[inplace_string]") {
  constexpr auto k_value = inplace_string<13UZ>{"Hello, World!"};

  STATIC_REQUIRE(!k_value.empty());
  STATIC_REQUIRE(k_value.size() == 13UZ);
  STATIC_REQUIRE(k_value.length() == 13UZ);
  STATIC_REQUIRE(k_value.max_size() == 13UZ);
  STATIC_REQUIRE(k_value.capacity() == 13UZ);
  STATIC_REQUIRE(k_value == "Hello, World!");
}

TEST_CASE("inplace_string elements are accessed with bounds checking", "[inplace_string]") {
  constexpr auto k_value = inplace_string<13UZ>{"Hello, World!"};

  STATIC_REQUIRE(k_value.at(0UZ) == 'H');
  STATIC_REQUIRE(k_value.at(1UZ) == 'e');
  STATIC_REQUIRE(k_value.at(2UZ) == 'l');
  STATIC_REQUIRE(k_value.at(3UZ) == 'l');
  STATIC_REQUIRE(k_value.at(4UZ) == 'o');
  STATIC_REQUIRE(k_value.at(5UZ) == ',');
  STATIC_REQUIRE(k_value.at(6UZ) == ' ');
  STATIC_REQUIRE(k_value.at(7UZ) == 'W');
  STATIC_REQUIRE(k_value.at(8UZ) == 'o');
  STATIC_REQUIRE(k_value.at(9UZ) == 'r');
  STATIC_REQUIRE(k_value.at(10UZ) == 'l');
  STATIC_REQUIRE(k_value.at(11UZ) == 'd');
  STATIC_REQUIRE(k_value.at(12UZ) == '!');

  REQUIRE_THROWS_AS(k_value.at(13UZ), std::out_of_range);
}

TEST_CASE("inplace_string elements are accessed without bounds checking", "[inplace_string]") {
  constexpr auto k_value = inplace_string<13UZ>{"Hello, World!"};

  STATIC_REQUIRE(k_value[0UZ] == 'H');
  STATIC_REQUIRE(k_value[1UZ] == 'e');
  STATIC_REQUIRE(k_value[2UZ] == 'l');
  STATIC_REQUIRE(k_value[3UZ] == 'l');
  STATIC_REQUIRE(k_value[4UZ] == 'o');
  STATIC_REQUIRE(k_value[5UZ] == ',');
  STATIC_REQUIRE(k_value[6UZ] == ' ');
  STATIC_REQUIRE(k_value[7UZ] == 'W');
  STATIC_REQUIRE(k_value[8UZ] == 'o');
  STATIC_REQUIRE(k_value[9UZ] == 'r');
  STATIC_REQUIRE(k_value[10UZ] == 'l');
  STATIC_REQUIRE(k_value[11UZ] == 'd');
  STATIC_REQUIRE(k_value[12UZ] == '!');

  REQUIRE_NOTHROW(k_value[13UZ]);
}

TEST_CASE("inplace_string elements are accessed with front and back", "[inplace_string]") {
  constexpr auto k_value = inplace_string<13UZ>{"Hello, World!"};

  STATIC_REQUIRE(k_value.front() == 'H');
  STATIC_REQUIRE(k_value.back() == '!');
}

TEST_CASE("inplace_string elements are accessed with data and c_str", "[inplace_string]") {
  constexpr auto k_value = inplace_string<13UZ>{"Hello, World!"};

  REQUIRE(std::strcmp(k_value.data(), "Hello, World!") == 0);
  REQUIRE(std::strcmp(k_value.c_str(), "Hello, World!") == 0);
}

TEST_CASE("inplace_string is converted to std::basic_string_view", "[inplace_string]") {
  constexpr auto k_value = inplace_string<13UZ>{"Hello, World!"};

  STATIC_REQUIRE(static_cast<std::string_view>(k_value) == "Hello, World!");
}

TEST_CASE("inplace_string is iterated over", "[inplace_string]") {
  constexpr auto k_value = inplace_string<13UZ>{"Hello, World!"};

  STATIC_REQUIRE(std::equal(k_value.begin(), k_value.end(), "Hello, World!"));
  STATIC_REQUIRE(std::equal(k_value.cbegin(), k_value.cend(), "Hello, World!"));
}

TEST_CASE("inplace_string is iterated over in reverse", "[inplace_string]") {
  constexpr auto k_value = inplace_string<13UZ>{"Hello, World!"};

  STATIC_REQUIRE(std::equal(k_value.rbegin(), k_value.rend(), "!dlroW ,olleH"));
  STATIC_REQUIRE(std::equal(k_value.crbegin(), k_value.crend(), "!dlroW ,olleH"));
}

TEST_CASE("inplace_string is cleared", "[inplace_string]") {
  auto value = inplace_string<13UZ>{"Hello, World!"};

  value.clear();

  REQUIRE(value.empty());
  REQUIRE(value.size() == 0UZ);
  REQUIRE(value.length() == 0UZ);
  REQUIRE(value.max_size() == 13UZ);
  REQUIRE(value.capacity() == 13UZ);
  REQUIRE(value == "");
}

TEST_CASE("inplace_string is inserted into", "[inplace_string]") {
  auto value = inplace_string<18UZ>{"Hello, World!"};

  value.insert(7UZ, 5UZ, 'X');

  REQUIRE(value == "Hello, XXXXXWorld!");

  REQUIRE_THROWS_AS(value.insert(7UZ, 7UZ, 'X'), std::length_error);
}

TEST_CASE("inplace_string is erased from", "[inplace_string]") {
  auto value = inplace_string<13UZ>{"Hello, World!"};

  value.erase(7UZ, 5UZ);

  REQUIRE(value == "Hello, !");

  REQUIRE_THROWS_AS(value.erase(7UZ, 7UZ), std::out_of_range);
}

TEST_CASE("inplace_string is concatenated", "[inplace_string]") {
  constexpr auto k_value1 = inplace_string<7UZ>{"Hello, "};
  constexpr auto k_value2 = inplace_string<6UZ>{"World!"};

  constexpr auto k_value3 = k_value1 + k_value2;

  STATIC_REQUIRE(k_value3 == "Hello, World!");
}

TEST_CASE("inplace_string is pushed back into", "[inplace_string]") {
  auto value = inplace_string<13UZ>{"Hello, World"};

  value.push_back('!');

  REQUIRE(value == "Hello, World!");
}

TEST_CASE("inplace_string is popped back from", "[inplace_string]") {
  auto value = inplace_string<13UZ>{"Hello, World!"};

  value.pop_back();

  REQUIRE(value == "Hello, World");
}

TEST_CASE("inplace_string is appended to", "[inplace_string]") {
  auto value = inplace_string<13UZ>{"Hello, "};

  value.append(inplace_string<7UZ>{"World!"});

  REQUIRE(value == "Hello, World!");

  REQUIRE_THROWS_AS(value.append(inplace_string<1UZ>{"!"}), std::length_error);
}

TEST_CASE("inplace_string is concatenated with the += operator", "[inplace_string]") {
  auto value = inplace_string<13UZ>{"Hello, "};

  value += inplace_string<6UZ>{"World!"};

  REQUIRE(value == "Hello, World!");

  REQUIRE_THROWS_AS(value += inplace_string<1UZ>{"!"}, std::length_error);
}

TEST_CASE("inplace_string is resized", "[inplace_string]") {
  auto value = inplace_string<15UZ>{"Hello, World!"};

  SECTION("to a smaller size") {
    value.resize(7UZ);
    REQUIRE(value == "Hello, ");
  }

  SECTION("to the maximum size") {
    value.resize(15UZ, 'X');
    REQUIRE(value == "Hello, World!XX");
  }

  SECTION("to a size greater than the maximum size") {  //
    REQUIRE_THROWS_AS(value.resize(16UZ, 'X'), std::length_error);
  }
}

TEST_CASE("inplace_string is swapped", "[inplace_string]") {
  auto value1 = inplace_string<15UZ>{"Hello, World!"};
  auto value2 = inplace_string<15UZ>{"Goodbye, World!"};

  value1.swap(value2);

  REQUIRE(value1 == "Goodbye, World!");
  REQUIRE(value2 == "Hello, World!");
}

TEST_CASE("inplace_string is searched for a substring", "[inplace_string]") {
  constexpr auto k_value = inplace_string<13UZ>{"Hello, World!"};

  SECTION("with an inplace_string") {
    constexpr auto k_hello = inplace_string<5UZ>{"Hello"};
    constexpr auto k_world = inplace_string<5UZ>{"World"};

    STATIC_REQUIRE(k_value.find(k_hello) == 0UZ);
    STATIC_REQUIRE(k_value.find(k_world) == 7UZ);
  }

  SECTION("with a string_view") {
    using namespace std::string_view_literals;

    STATIC_REQUIRE(k_value.find("Hello"sv) == 0UZ);
    STATIC_REQUIRE(k_value.find("World"sv) == 7UZ);
    STATIC_REQUIRE(k_value.find("Goodbye"sv) == k_value.npos);
  }

  SECTION("with a character string") {
    STATIC_REQUIRE(k_value.find("Hello") == 0UZ);
    STATIC_REQUIRE(k_value.find("World") == 7UZ);
    STATIC_REQUIRE(k_value.find("Goodbye") == k_value.npos);
  }
}

TEST_CASE("inplace_string is searched for a substring in reverse", "[inplace_string]") {
  constexpr auto k_value = inplace_string<13UZ>{"Hello, World!"};

  SECTION("with an inplace_string") {
    constexpr auto k_hello = inplace_string<5UZ>{"Hello"};
    constexpr auto k_world = inplace_string<5UZ>{"World"};

    STATIC_REQUIRE(k_value.rfind(k_hello) == 0UZ);
    REQUIRE(k_value.rfind(k_world) == 7UZ);
  }

  SECTION("with a string_view") {
    using namespace std::string_view_literals;

    STATIC_REQUIRE(k_value.rfind("Hello"sv) == 0UZ);
    STATIC_REQUIRE(k_value.rfind("World"sv) == 7UZ);
    STATIC_REQUIRE(k_value.rfind("Goodbye"sv) == k_value.npos);
  }

  SECTION("with a character string") {
    STATIC_REQUIRE(k_value.rfind("Hello") == 0UZ);
    STATIC_REQUIRE(k_value.rfind("World") == 7UZ);
    STATIC_REQUIRE(k_value.rfind("Goodbye") == k_value.npos);
  }
}

}  // namespace gw
