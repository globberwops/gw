// Copyright (c) 2023 Martin Stump
// SPDX-License-Identifier: BSL-1.0

#include "gw/inplace_string.hpp"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <cstdio>
#include <cstring>
#include <format>
#include <string_view>
#include <type_traits>

namespace gw {

TEST_CASE("inplace_string is default constructed", "[inplace_string]") {
  constexpr auto k_value = inplace_string<10U>{};

  STATIC_REQUIRE(k_value.empty());
  STATIC_REQUIRE(k_value.size() == 0U);
  STATIC_REQUIRE(k_value.length() == 0U);
  STATIC_REQUIRE(k_value.max_size() == 10U);
  STATIC_REQUIRE(k_value.capacity() == 10U);
  STATIC_REQUIRE(k_value == "");

  STATIC_REQUIRE(std::is_trivially_copyable_v<decltype(k_value)>);
}

TEST_CASE("inplace_string is constructed from a character", "[inplace_string]") {
  constexpr auto k_value = inplace_string<10U>{5U, 'A'};

  STATIC_REQUIRE(!k_value.empty());
  STATIC_REQUIRE(k_value.size() == 5U);
  STATIC_REQUIRE(k_value.length() == 5U);
  STATIC_REQUIRE(k_value.max_size() == 10U);
  STATIC_REQUIRE(k_value.capacity() == 10U);
  STATIC_REQUIRE(k_value == "AAAAA");
}

TEST_CASE("inplace_string is constructed from a string literal", "[inplace_string]") {
  constexpr auto k_value = inplace_string<13U>{"Hello, World!"};

  STATIC_REQUIRE(!k_value.empty());
  STATIC_REQUIRE(k_value.size() == 13U);
  STATIC_REQUIRE(k_value.length() == 13U);
  STATIC_REQUIRE(k_value.max_size() == 13U);
  STATIC_REQUIRE(k_value.capacity() == 13U);
  STATIC_REQUIRE(k_value == "Hello, World!");
}

TEST_CASE("inplace_string elements are accessed with bounds checking", "[inplace_string]") {
  constexpr auto k_value = inplace_string<13U>{"Hello, World!"};

  STATIC_REQUIRE(k_value.at(0U) == 'H');
  STATIC_REQUIRE(k_value.at(1U) == 'e');
  STATIC_REQUIRE(k_value.at(2U) == 'l');
  STATIC_REQUIRE(k_value.at(3U) == 'l');
  STATIC_REQUIRE(k_value.at(4U) == 'o');
  STATIC_REQUIRE(k_value.at(5U) == ',');
  STATIC_REQUIRE(k_value.at(6U) == ' ');
  STATIC_REQUIRE(k_value.at(7U) == 'W');
  STATIC_REQUIRE(k_value.at(8U) == 'o');
  STATIC_REQUIRE(k_value.at(9U) == 'r');
  STATIC_REQUIRE(k_value.at(10U) == 'l');
  STATIC_REQUIRE(k_value.at(11U) == 'd');
  STATIC_REQUIRE(k_value.at(12U) == '!');

  REQUIRE_THROWS_AS(k_value.at(13U), std::out_of_range);
}

TEST_CASE("inplace_string elements are accessed without bounds checking", "[inplace_string]") {
  constexpr auto k_value = inplace_string<13U>{"Hello, World!"};

  STATIC_REQUIRE(k_value[0U] == 'H');
  STATIC_REQUIRE(k_value[1U] == 'e');
  STATIC_REQUIRE(k_value[2U] == 'l');
  STATIC_REQUIRE(k_value[3U] == 'l');
  STATIC_REQUIRE(k_value[4U] == 'o');
  STATIC_REQUIRE(k_value[5U] == ',');
  STATIC_REQUIRE(k_value[6U] == ' ');
  STATIC_REQUIRE(k_value[7U] == 'W');
  STATIC_REQUIRE(k_value[8U] == 'o');
  STATIC_REQUIRE(k_value[9U] == 'r');
  STATIC_REQUIRE(k_value[10U] == 'l');
  STATIC_REQUIRE(k_value[11U] == 'd');
  STATIC_REQUIRE(k_value[12U] == '!');

  REQUIRE_NOTHROW(k_value[13U]);
}

TEST_CASE("inplace_string elements are accessed with front and back", "[inplace_string]") {
  constexpr auto k_value = inplace_string<13U>{"Hello, World!"};

  STATIC_REQUIRE(k_value.front() == 'H');
  STATIC_REQUIRE(k_value.back() == '!');
}

TEST_CASE("inplace_string elements are accessed with data and c_str", "[inplace_string]") {
  constexpr auto k_value = inplace_string<13U>{"Hello, World!"};

  REQUIRE(std::strcmp(k_value.data(), "Hello, World!") == 0);
  REQUIRE(std::strcmp(k_value.c_str(), "Hello, World!") == 0);
}

TEST_CASE("inplace_string is converted to std::basic_string_view", "[inplace_string]") {
  constexpr auto k_value = inplace_string<13U>{"Hello, World!"};

  STATIC_REQUIRE(static_cast<std::string_view>(k_value) == "Hello, World!");
}

TEST_CASE("inplace_string is iterated over", "[inplace_string]") {
  constexpr auto k_value = inplace_string<13U>{"Hello, World!"};

  STATIC_REQUIRE(std::equal(k_value.begin(), k_value.end(), "Hello, World!"));
  STATIC_REQUIRE(std::equal(k_value.cbegin(), k_value.cend(), "Hello, World!"));
}

TEST_CASE("inplace_string is iterated over in reverse", "[inplace_string]") {
  constexpr auto k_value = inplace_string<13U>{"Hello, World!"};

  STATIC_REQUIRE(std::equal(k_value.rbegin(), k_value.rend(), "!dlroW ,olleH"));
  STATIC_REQUIRE(std::equal(k_value.crbegin(), k_value.crend(), "!dlroW ,olleH"));
}

TEST_CASE("inplace_string is cleared", "[inplace_string]") {
  auto value = inplace_string<13U>{"Hello, World!"};

  value.clear();

  REQUIRE(value.empty());
  REQUIRE(value.size() == 0U);
  REQUIRE(value.length() == 0U);
  REQUIRE(value.max_size() == 13U);
  REQUIRE(value.capacity() == 13U);
  REQUIRE(value == "");
}

TEST_CASE("inplace_string is inserted into", "[inplace_string]") {
  auto value = inplace_string<18U>{"Hello, World!"};

  value.insert(7U, 5U, 'X');

  REQUIRE(value == "Hello, XXXXXWorld!");

  REQUIRE_THROWS_AS(value.insert(7U, 7U, 'X'), std::length_error);
}

TEST_CASE("inplace_string is erased from", "[inplace_string]") {
  auto value = inplace_string<13U>{"Hello, World!"};

  value.erase(7U, 5U);

  REQUIRE(value == "Hello, !");

  REQUIRE_THROWS_AS(value.erase(7U, 7U), std::out_of_range);
}

TEST_CASE("inplace_string is concatenated", "[inplace_string]") {
  constexpr auto k_value1 = inplace_string<7U>{"Hello, "};
  constexpr auto k_value2 = inplace_string<6U>{"World!"};

  constexpr auto k_value3 = k_value1 + k_value2;

  STATIC_REQUIRE(k_value3 == "Hello, World!");
}

TEST_CASE("inplace_string is pushed back into", "[inplace_string]") {
  auto value = inplace_string<13U>{"Hello, World"};

  value.push_back('!');

  REQUIRE(value == "Hello, World!");
}

TEST_CASE("inplace_string is popped back from", "[inplace_string]") {
  auto value = inplace_string<13U>{"Hello, World!"};

  value.pop_back();

  REQUIRE(value == "Hello, World");
}

TEST_CASE("inplace_string is appended to", "[inplace_string]") {
  auto value = inplace_string<13U>{"Hello, "};

  value.append(inplace_string<7U>{"World!"});

  REQUIRE(value == "Hello, World!");

  REQUIRE_THROWS_AS(value.append(inplace_string<1U>{"!"}), std::length_error);
}

TEST_CASE("inplace_string is concatenated with the += operator", "[inplace_string]") {
  auto value = inplace_string<13U>{"Hello, "};

  value += inplace_string<6U>{"World!"};

  REQUIRE(value == "Hello, World!");

  REQUIRE_THROWS_AS(value += inplace_string<1U>{"!"}, std::length_error);
}

TEST_CASE("inplace_string is resized", "[inplace_string]") {
  auto value = inplace_string<15U>{"Hello, World!"};

  SECTION("to a smaller size") {
    value.resize(7U);
    REQUIRE(value == "Hello, ");
  }

  SECTION("to the maximum size") {
    value.resize(15U, 'X');
    REQUIRE(value == "Hello, World!XX");
  }

  SECTION("to a size greater than the maximum size") {  //
    REQUIRE_THROWS_AS(value.resize(16U, 'X'), std::length_error);
  }
}

TEST_CASE("inplace_string is swapped", "[inplace_string]") {
  auto value1 = inplace_string<15U>{"Hello, World!"};
  auto value2 = inplace_string<15U>{"Goodbye, World!"};

  value1.swap(value2);

  REQUIRE(value1 == "Goodbye, World!");
  REQUIRE(value2 == "Hello, World!");
}

TEST_CASE("inplace_string is searched for a substring", "[inplace_string]") {
  constexpr auto k_value = inplace_string<13U>{"Hello, World!"};

  SECTION("with an inplace_string") {
    constexpr auto k_hello = inplace_string<5U>{"Hello"};
    constexpr auto k_world = inplace_string<5U>{"World"};

    STATIC_REQUIRE(k_value.find(k_hello) == 0U);
    STATIC_REQUIRE(k_value.find(k_world) == 7U);
  }

  SECTION("with a string_view") {
    using namespace std::string_view_literals;

    STATIC_REQUIRE(k_value.find("Hello"sv) == 0U);
    STATIC_REQUIRE(k_value.find("World"sv) == 7U);
    STATIC_REQUIRE(k_value.find("Goodbye"sv) == k_value.npos);
  }

  SECTION("with a character string, a position, and a count") {
    STATIC_REQUIRE(k_value.find("Hello!", 0U, 5U) == 0U);
    STATIC_REQUIRE(k_value.find("World.", 7U, 5U) == 7U);
    STATIC_REQUIRE(k_value.find("Goodbye", 0U, 7U) == k_value.npos);
  }

  SECTION("with a character string") {
    STATIC_REQUIRE(k_value.find("Hello") == 0U);
    STATIC_REQUIRE(k_value.find("World") == 7U);
    STATIC_REQUIRE(k_value.find("Goodbye") == k_value.npos);
  }

  SECTION("with a character") {
    STATIC_REQUIRE(k_value.find('H') == 0U);
    STATIC_REQUIRE(k_value.find('W') == 7U);
    STATIC_REQUIRE(k_value.find('G') == k_value.npos);
  }
}

TEST_CASE("inplace_string is searched for a substring in reverse", "[inplace_string]") {
  constexpr auto k_value = inplace_string<13U>{"Hello, World!"};

  SECTION("with an inplace_string") {
    constexpr auto k_hello = inplace_string<5U>{"Hello"};
    constexpr auto k_world = inplace_string<5U>{"World"};

    STATIC_REQUIRE(k_value.rfind(k_hello) == 0U);
    REQUIRE(k_value.rfind(k_world) == 7U);
  }

  SECTION("with a string_view") {
    using namespace std::string_view_literals;

    STATIC_REQUIRE(k_value.rfind("Hello"sv) == 0U);
    STATIC_REQUIRE(k_value.rfind("World"sv) == 7U);
    STATIC_REQUIRE(k_value.rfind("Goodbye"sv) == k_value.npos);
  }

  SECTION("with a character string, a position, and a count") {
    STATIC_REQUIRE(k_value.rfind("Hello!", 5U, 5U) == 0U);
    STATIC_REQUIRE(k_value.rfind("World.", 13U, 5U) == 7U);
    STATIC_REQUIRE(k_value.rfind("Goodbye", 0U, 7U) == k_value.npos);
  }

  SECTION("with a character string") {
    STATIC_REQUIRE(k_value.rfind("Hello") == 0U);
    STATIC_REQUIRE(k_value.rfind("World") == 7U);
    STATIC_REQUIRE(k_value.rfind("Goodbye") == k_value.npos);
  }

  SECTION("with a character") {
    STATIC_REQUIRE(k_value.rfind('H') == 0U);
    STATIC_REQUIRE(k_value.rfind('W') == 7U);
    STATIC_REQUIRE(k_value.rfind('G') == k_value.npos);
  }
}

TEST_CASE("inplace_string is searched for any character", "[inplace_string]") {
  constexpr auto k_value = inplace_string<13U>{"Hello, World!"};

  SECTION("with an inplace_string") {
    constexpr auto k_hello = inplace_string<5U>{"Hello"};
    constexpr auto k_world = inplace_string<5U>{"World"};

    STATIC_REQUIRE(k_value.find_first_of(k_hello) == 0U);
    STATIC_REQUIRE(k_value.find_first_of(k_world) == 2U);
  }

  SECTION("with a string_view") {
    using namespace std::string_view_literals;

    STATIC_REQUIRE(k_value.find_first_of("Hello"sv) == 0U);
    STATIC_REQUIRE(k_value.find_first_of("World"sv) == 2U);
    STATIC_REQUIRE(k_value.find_first_of("Goodbye"sv) == 1U);
  }

  SECTION("with a character string") {
    STATIC_REQUIRE(k_value.find_first_of("Hello") == 0U);
    STATIC_REQUIRE(k_value.find_first_of("World") == 2U);
    STATIC_REQUIRE(k_value.find_first_of("Goodbye") == 1U);
  }
}

TEST_CASE("inplace_string is hashed", "[inplace_string]") {
  constexpr auto k_value = inplace_string<13U>{"Hello, World!"};

  REQUIRE(std::hash<inplace_string<13U>>{}(k_value) == std::hash<std::string_view>{}("Hello, World!"));
}

TEST_CASE("inplace_string is formatted", "[inplace_string]") {
  SECTION("with char") {
    constexpr auto k_value = inplace_string<13U>{"Hello, World!"};

    STATIC_REQUIRE(std::formattable<inplace_string<13U>, char>);

    REQUIRE(std::format("{}", k_value) == "Hello, World!");
  }

  SECTION("with wchar_t") {
    constexpr auto k_value = inplace_wstring<13U>{L"Hello, World!"};

    STATIC_REQUIRE(std::formattable<inplace_wstring<13U>, wchar_t>);

    REQUIRE(std::format(L"{}", k_value) == L"Hello, World!");
  }
}

}  // namespace gw
