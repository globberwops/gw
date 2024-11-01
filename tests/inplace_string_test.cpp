// Copyright (c) 2023 Martin Stump
// SPDX-License-Identifier: BSL-1.0

#include "gw/inplace_string.hpp"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <cstring>
#include <format>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <type_traits>

namespace gw {

using namespace std::string_view_literals;
using namespace std::string_literals;

TEST_CASE("inplace_string is a trivial type", "[inplace_string]") {
  STATIC_REQUIRE(std::is_trivial_v<inplace_string<13U>>);
  STATIC_REQUIRE(std::is_trivial_v<inplace_wstring<13U>>);
  STATIC_REQUIRE(std::is_trivial_v<inplace_u16string<13U>>);
  STATIC_REQUIRE(std::is_trivial_v<inplace_u32string<13U>>);
}

TEST_CASE("inplace_string is a standard layout type", "[inplace_string]") {
  STATIC_REQUIRE(std::is_standard_layout_v<inplace_string<13U>>);
  STATIC_REQUIRE(std::is_standard_layout_v<inplace_wstring<13U>>);
  STATIC_REQUIRE(std::is_standard_layout_v<inplace_u16string<13U>>);
  STATIC_REQUIRE(std::is_standard_layout_v<inplace_u32string<13U>>);
}

TEST_CASE("inplace_string is constructed", "[inplace_string]") {
  SECTION("with a default constructor") {  //
    STATIC_REQUIRE(std::is_nothrow_default_constructible_v<inplace_string<13U>>);
    STATIC_REQUIRE(std::is_nothrow_default_constructible_v<inplace_wstring<13U>>);
    STATIC_REQUIRE(std::is_nothrow_default_constructible_v<inplace_u16string<13U>>);
    STATIC_REQUIRE(std::is_nothrow_default_constructible_v<inplace_u32string<13U>>);
  }

  SECTION("from a string literal") {
    STATIC_REQUIRE(std::constructible_from<inplace_string<13U>, const char*>);
    constexpr auto value = basic_inplace_string{"Hello, World!"};
    STATIC_REQUIRE(value == "Hello, World!");
  }

  SECTION("from a wstring literal") {
    STATIC_REQUIRE(std::constructible_from<inplace_wstring<13U>, const wchar_t*>);
    constexpr auto value = basic_inplace_string{L"Hello, World!"};
    STATIC_REQUIRE(value == L"Hello, World!");
  }

  SECTION("from a u16string literal") {
    STATIC_REQUIRE(std::constructible_from<inplace_u16string<13U>, const char16_t*>);
    constexpr auto value = basic_inplace_string{u"Hello, World!"};
    STATIC_REQUIRE(value == u"Hello, World!");
  }

  SECTION("from a u32string literal") {
    STATIC_REQUIRE(std::constructible_from<inplace_u32string<13U>, const char32_t*>);
    constexpr auto value = basic_inplace_string{U"Hello, World!"};
    STATIC_REQUIRE(value == U"Hello, World!");
  }

  SECTION("from a count and a character") {
    STATIC_REQUIRE(std::constructible_from<inplace_string<13U>, int, char>);
    constexpr auto value = inplace_string<13U>{5, 'X'};
    STATIC_REQUIRE(value == "XXXXX");
  }

  SECTION("from a count and a wide character") {
    STATIC_REQUIRE(std::constructible_from<inplace_wstring<13U>, int, wchar_t>);
    constexpr auto value = inplace_wstring<13U>{5, L'X'};
    STATIC_REQUIRE(value == L"XXXXX");
  }

  SECTION("from a count and a u16 character") {
    STATIC_REQUIRE(std::constructible_from<inplace_u16string<13U>, int, char16_t>);
    constexpr auto value = inplace_u16string<13U>{5, u'X'};
    STATIC_REQUIRE(value == u"XXXXX");
  }

  SECTION("from a count and a u32 character") {
    STATIC_REQUIRE(std::constructible_from<inplace_u32string<13U>, int, char32_t>);
    constexpr auto value = inplace_u32string<13U>{5, U'X'};
    STATIC_REQUIRE(value == U"XXXXX");
  }

  SECTION("from iterators") {
    static constexpr auto input = "Hello, World!"sv;
    STATIC_REQUIRE(std::constructible_from<inplace_string<13U>, decltype(input)::iterator, decltype(input)::iterator>);
    constexpr auto value = inplace_string<13U>{input.begin(), input.end()};
    STATIC_REQUIRE(value == "Hello, World!");
  }

  SECTION("from a string view") {
    static constexpr auto input = "Hello, World!"sv;
    STATIC_REQUIRE(std::constructible_from<inplace_string<13U>, decltype(input)>);
    constexpr auto value = inplace_string<13U>{input};
    STATIC_REQUIRE(value == "Hello, World!");
  }

  SECTION("from a wide string view") {
    static constexpr auto input = L"Hello, World!"sv;
    STATIC_REQUIRE(std::constructible_from<inplace_wstring<13U>, decltype(input)>);
    constexpr auto value = inplace_wstring<13U>{input};
    STATIC_REQUIRE(value == L"Hello, World!");
  }

  SECTION("from a string") {
    auto input = "Hello, World!"s;
    STATIC_REQUIRE(std::constructible_from<inplace_string<13U>, decltype(input)>);
    auto value = inplace_string<13U>{input};
    REQUIRE(value == "Hello, World!");
  }

  SECTION("from a wide string") {
    auto input = L"Hello, World!"s;
    STATIC_REQUIRE(std::constructible_from<inplace_wstring<13U>, decltype(input)>);
    auto value = inplace_wstring<13U>{input};
    REQUIRE(value == L"Hello, World!");
  }

  SECTION("from a range of characters") {
    constexpr auto input = std::array{'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!'};
    STATIC_REQUIRE(std::constructible_from<inplace_string<13U>, decltype(input)>);
    constexpr auto value = inplace_string<13U>{input};
    STATIC_REQUIRE(value == "Hello, World!");
  }
}

namespace {

template <basic_inplace_string Str>
struct test_struct {
  static constexpr auto k_str = Str;
};

}  // namespace

TEST_CASE("inplace_string is used as NTTP", "[inplace_string]") {
  constexpr auto value = test_struct<"Hello, World!">{};
  STATIC_REQUIRE(value.k_str == "Hello, World!");
  STATIC_REQUIRE(value.k_str.size() == 13U);
  STATIC_REQUIRE(value.k_str.max_size() == 13U);
}

TEST_CASE("inplace_string elements are accessed ", "[inplace_string]") {
  constexpr auto value = basic_inplace_string{"Hello, World!"};

  SECTION("with bounds checking") {
    STATIC_REQUIRE(value.at(0) == 'H');
    STATIC_REQUIRE(value.at(1) == 'e');
    STATIC_REQUIRE(value.at(2) == 'l');
    STATIC_REQUIRE(value.at(3) == 'l');
    STATIC_REQUIRE(value.at(4) == 'o');
    STATIC_REQUIRE(value.at(5) == ',');
    STATIC_REQUIRE(value.at(6) == ' ');
    STATIC_REQUIRE(value.at(7) == 'W');
    STATIC_REQUIRE(value.at(8) == 'o');
    STATIC_REQUIRE(value.at(9) == 'r');
    STATIC_REQUIRE(value.at(10) == 'l');
    STATIC_REQUIRE(value.at(11) == 'd');
    STATIC_REQUIRE(value.at(12) == '!');
    REQUIRE_THROWS_AS(value.at(13), std::out_of_range);
  }

  SECTION("without bounds checking") {
    STATIC_REQUIRE(value[0] == 'H');
    STATIC_REQUIRE(value[1] == 'e');
    STATIC_REQUIRE(value[2] == 'l');
    STATIC_REQUIRE(value[3] == 'l');
    STATIC_REQUIRE(value[4] == 'o');
    STATIC_REQUIRE(value[5] == ',');
    STATIC_REQUIRE(value[6] == ' ');
    STATIC_REQUIRE(value[7] == 'W');
    STATIC_REQUIRE(value[8] == 'o');
    STATIC_REQUIRE(value[9] == 'r');
    STATIC_REQUIRE(value[10] == 'l');
    STATIC_REQUIRE(value[11] == 'd');
    STATIC_REQUIRE(value[12] == '!');
    REQUIRE_NOTHROW(value[13U]);
  }

  SECTION("with front and back") {
    STATIC_REQUIRE(value.front() == 'H');
    STATIC_REQUIRE(value.back() == '!');
  }

  SECTION("with data and c_str") {
    REQUIRE(std::strcmp(value.data(), "Hello, World!") == 0);
    REQUIRE(std::strcmp(value.c_str(), "Hello, World!") == 0);
  }
}

TEST_CASE("inplace_string is converted to std::basic_string_view", "[inplace_string]") {
  SECTION("with the conversion operator") {
    constexpr auto value = basic_inplace_string{"Hello, World!"};
    STATIC_REQUIRE(static_cast<std::basic_string_view<decltype(value)::value_type>>(value) == "Hello, World!");
  }

  SECTION("with the view method") {
    constexpr auto value = basic_inplace_string{"Hello, World!"};
    STATIC_REQUIRE(value.view() == "Hello, World!"sv);
  }
}

TEST_CASE("inplace_string is iterated over", "[inplace_string]") {
  constexpr auto value = basic_inplace_string{"Hello, World!"};

  SECTION("forward") {
    STATIC_REQUIRE(std::equal(value.begin(), value.end(), "Hello, World!"));
    STATIC_REQUIRE(std::equal(value.cbegin(), value.cend(), "Hello, World!"));
  }

  SECTION("reverse") {
    STATIC_REQUIRE(std::equal(value.rbegin(), value.rend(), "!dlroW ,olleH"));
    STATIC_REQUIRE(std::equal(value.crbegin(), value.crend(), "!dlroW ,olleH"));
  }
}

TEST_CASE("inplace_string's size is determined", "[inplace_string]") {
  constexpr auto value = basic_inplace_string{"Hello, World!"};

  SECTION("with empty") {  //
    STATIC_REQUIRE(!value.empty());
  }

  SECTION("with size") {  //
    STATIC_REQUIRE(value.size() == 13U);
  }

  SECTION("with length") {  //
    STATIC_REQUIRE(value.length() == 13U);
  }

  SECTION("with max_size") {  //
    STATIC_REQUIRE(value.max_size() == 13U);
  }

  SECTION("with capacity") {  //
    STATIC_REQUIRE(value.capacity() == 13U);
  }
}

TEST_CASE("inplace_string's size is modified", "[inplace_string]") {
  auto value = inplace_string<15>{"Hello, World!"};

  SECTION("with reserve") {
    value.reserve(10);
    REQUIRE(value.capacity() == 15U);
  }

  SECTION("with reserve and out of bounds") {  //
    REQUIRE_THROWS_AS(value.reserve(16), std::length_error);
  }

  SECTION("with shrink_to_fit") {
    value.shrink_to_fit();
    REQUIRE(value.capacity() == 15U);
  }

  SECTION("with clear") {
    value.clear();
    REQUIRE(value.empty());
  }
}

TEST_CASE("inplace_string is inserted into", "[inplace_string]") {
  auto value = inplace_string<18U>{"Hello, World!"};

  SECTION("with an index, a count, and a character - in bounds") {
    static constexpr auto index = 7U;
    static constexpr auto count = 5U;
    static constexpr auto ch = 'X';
    value.insert(index, count, ch);
    REQUIRE(value == "Hello, XXXXXWorld!");
  }

  SECTION("with an index, a count, and a character - out of bounds") {
    static constexpr auto index = 7U;
    static constexpr auto count = 7U;
    static constexpr auto ch = 'X';
    REQUIRE_THROWS_AS(value.insert(index, count, ch), std::length_error);
  }

  SECTION("with an index and a character string - in bounds") {
    static constexpr auto index = 7U;
    static constexpr auto str = "XXXXX";
    value.insert(index, str);
    REQUIRE(value == "Hello, XXXXXWorld!");
  }

  SECTION("with an index and a character string - out of bounds") {
    static constexpr auto index = 7U;
    static constexpr auto str = "XXXXXX";
    REQUIRE_THROWS_AS(value.insert(index, str), std::length_error);
  }

  SECTION("with an index, a character string, and a count - in bounds") {
    static constexpr auto index = 7U;
    static constexpr auto str = "XXXXXX";
    static constexpr auto count = 5U;
    value.insert(index, str, count);
    REQUIRE(value == "Hello, XXXXXWorld!");
  }

  SECTION("with an index, a character string, and a count - out of bounds") {
    static constexpr auto index = 7U;
    static constexpr auto str = "XXXXXX";
    static constexpr auto count = 6U;
    REQUIRE_THROWS_AS(value.insert(index, str, count), std::length_error);
  }

  SECTION("with a position and a character") {
    static constexpr auto ch = 'X';
    const auto* const pos = value.insert(value.cbegin(), ch);
    REQUIRE(pos == value.cbegin());
    REQUIRE(value == "XHello, World!");
  }

  SECTION("with a position, a count, and a character  - in bounds") {
    static constexpr auto count = 5U;
    static constexpr auto ch = 'X';
    const auto* const pos = value.insert(value.cbegin(), count, ch);
    REQUIRE(pos == value.cbegin());
    REQUIRE(value == "XXXXXHello, World!");
  }

  SECTION("with a position, a count, and a character  - out of bounds") {
    static constexpr auto count = 6U;
    static constexpr auto ch = 'X';
    REQUIRE_THROWS_AS(value.insert(value.cbegin(), count, ch), std::length_error);
  }

  SECTION("with a position and iterators - in bounds") {
    static constexpr auto range = "XXXXX"sv;
    static constexpr auto index = 7U;
    const auto* const pos_iter = std::ranges::next(value.begin(), index);
    const auto* const pos = value.insert(pos_iter, range.begin(), range.end());
    REQUIRE(pos == pos_iter);
    REQUIRE(value == "Hello, XXXXXWorld!");
  }

  SECTION("with a position and iterators - out of bounds") {
    static constexpr auto range = "XXXXXX"sv;
    static constexpr auto index = 7U;
    const auto* const pos_iter = std::ranges::next(value.begin(), index);
    REQUIRE_THROWS_AS(value.insert(pos_iter, range.begin(), range.end()), std::length_error);
  }

  SECTION("with a range - in bounds") {
    static constexpr auto range = "XXXXX"sv;
    const auto* const pos = value.insert_range(value.cbegin(), range);
    REQUIRE(pos == value.cbegin());
    REQUIRE(value == "XXXXXHello, World!");
  }

  SECTION("with a range - out of bounds") {
    static constexpr auto range = "XXXXXX"sv;
    REQUIRE_THROWS_AS(value.insert_range(value.cbegin(), range), std::length_error);
  }
}

TEST_CASE("inplace_string is erased from", "[inplace_string]") {
  auto value = basic_inplace_string{"Hello, World!"};

  SECTION("from the beginning") {
    value.erase(0U, 7U);
    REQUIRE(value == "World!");
  }

  SECTION("in the middle") {
    value.erase(5U, 7U);
    REQUIRE(value == "Hello!");
  }

  SECTION("to the end") {
    value.erase(5U);
    REQUIRE(value == "Hello");
  }

  SECTION("out of range") {  //
    REQUIRE_THROWS_AS(value.erase(13U), std::out_of_range);
  }
}

TEST_CASE("inplace_string is concatenated", "[inplace_string]") {
  constexpr auto value1 = inplace_string<7U>{"Hello, "};
  constexpr auto value2 = inplace_string<6U>{"World!"};
  constexpr auto value3 = value1 + value2;
  STATIC_REQUIRE(value3 == "Hello, World!");
}

TEST_CASE("inplace_string is pushed back into", "[inplace_string]") {
  auto value = inplace_string<13U>{"Hello, World"};

  SECTION("in bounds") {
    value.push_back('!');
    REQUIRE(value == "Hello, World!");
  }

  SECTION("out of bounds") {  //
    value.push_back('!');
    REQUIRE_THROWS_AS(value.push_back('X'), std::length_error);
  }
}

TEST_CASE("inplace_string is popped back from", "[inplace_string]") {
  auto value = inplace_string<13U>{"Hello, World!"};
  value.pop_back();
  REQUIRE(value == "Hello, World");
}

TEST_CASE("inplace_string is appended to", "[inplace_string]") {
  auto value = inplace_string<13U>{"Hello, "};

  SECTION("in bounds") {
    value.append(inplace_string<7U>{"World!"});
    REQUIRE(value == "Hello, World!");
  }

  SECTION("out of bounds") {  //
    REQUIRE_THROWS_AS(value.append(inplace_string<1U>{"World!X"}), std::length_error);
  }
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
    value.resize(7);
    REQUIRE(value == "Hello, ");
  }

  SECTION("to the maximum size") {
    value.resize(value.max_size(), 'X');
    REQUIRE(value == "Hello, World!XX");
  }

  SECTION("to a size greater than the maximum size") {  //
    REQUIRE_THROWS_AS(value.resize(16, 'X'), std::length_error);
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
  constexpr auto value = inplace_string<13U>{"Hello, World!"};

  SECTION("with an inplace_string") {
    constexpr auto k_hello = inplace_string<5U>{"Hello"};
    constexpr auto k_world = inplace_string<5U>{"World"};
    STATIC_REQUIRE(value.find(k_hello) == 0U);
    STATIC_REQUIRE(value.find(k_world) == 7U);
  }

  SECTION("with a string_view") {
    STATIC_REQUIRE(value.find("Hello"sv) == 0U);
    STATIC_REQUIRE(value.find("World"sv) == 7U);
    STATIC_REQUIRE(value.find("Goodbye"sv) == value.npos);
  }

  SECTION("with a character string, a position, and a count") {
    STATIC_REQUIRE(value.find("Hello!", 0U, 5U) == 0U);
    STATIC_REQUIRE(value.find("World.", 7U, 5U) == 7U);
    STATIC_REQUIRE(value.find("Goodbye", 0U, 7U) == value.npos);
  }

  SECTION("with a character string") {
    STATIC_REQUIRE(value.find("Hello") == 0U);
    STATIC_REQUIRE(value.find("World") == 7U);
    STATIC_REQUIRE(value.find("Goodbye") == value.npos);
  }

  SECTION("with a character") {
    STATIC_REQUIRE(value.find('H') == 0U);
    STATIC_REQUIRE(value.find('W') == 7U);
    STATIC_REQUIRE(value.find('G') == value.npos);
  }
}

TEST_CASE("inplace_string is searched for a substring in reverse", "[inplace_string]") {
  constexpr auto value = inplace_string<13U>{"Hello, World!"};

  SECTION("with an inplace_string") {
    constexpr auto k_hello = inplace_string<5U>{"Hello"};
    constexpr auto k_world = inplace_string<5U>{"World"};
    STATIC_REQUIRE(value.rfind(k_hello) == 0U);
    REQUIRE(value.rfind(k_world) == 7U);
  }

  SECTION("with a string_view") {
    STATIC_REQUIRE(value.rfind("Hello"sv) == 0U);
    STATIC_REQUIRE(value.rfind("World"sv) == 7U);
    STATIC_REQUIRE(value.rfind("Goodbye"sv) == value.npos);
  }

  SECTION("with a character string, a position, and a count") {
    STATIC_REQUIRE(value.rfind("Hello!", 5U, 5U) == 0U);
    STATIC_REQUIRE(value.rfind("World.", 13U, 5U) == 7U);
    STATIC_REQUIRE(value.rfind("Goodbye", 0U, 7U) == value.npos);
  }

  SECTION("with a character string") {
    STATIC_REQUIRE(value.rfind("Hello") == 0U);
    STATIC_REQUIRE(value.rfind("World") == 7U);
    STATIC_REQUIRE(value.rfind("Goodbye") == value.npos);
  }

  SECTION("with a character") {
    STATIC_REQUIRE(value.rfind('H') == 0U);
    STATIC_REQUIRE(value.rfind('W') == 7U);
    STATIC_REQUIRE(value.rfind('G') == value.npos);
  }
}

TEST_CASE("inplace_string is searched for any character", "[inplace_string]") {
  constexpr auto value = inplace_string<13U>{"Hello, World!"};

  SECTION("with an inplace_string") {
    constexpr auto k_hello = inplace_string<5U>{"Hello"};
    constexpr auto k_world = inplace_string<5U>{"World"};
    STATIC_REQUIRE(value.find_first_of(k_hello) == 0U);
    STATIC_REQUIRE(value.find_first_of(k_world) == 2U);
  }

  SECTION("with a string_view") {
    STATIC_REQUIRE(value.find_first_of("Hello"sv) == 0U);
    STATIC_REQUIRE(value.find_first_of("World"sv) == 2U);
    STATIC_REQUIRE(value.find_first_of("Goodbye"sv) == 1U);
  }

  SECTION("with a character string") {
    STATIC_REQUIRE(value.find_first_of("Hello") == 0U);
    STATIC_REQUIRE(value.find_first_of("World") == 2U);
    STATIC_REQUIRE(value.find_first_of("Goodbye") == 1U);
  }
}

TEST_CASE("inplace_string is streamed out", "[inplace_string]") {
  constexpr auto value = inplace_string<13U>{"Hello, World!"};
  auto stream = std::ostringstream{};
  stream << value;
  REQUIRE(stream.str() == "Hello, World!");
}

TEST_CASE("inplace_string is streamed in", "[inplace_string]") {
  auto value = inplace_string<13U>{};
  auto stream = std::istringstream{"Hello, World!"};
  stream >> value;
  REQUIRE(value == "Hello, World!");
}

TEST_CASE("inplace_string is hashed", "[inplace_string]") {
  constexpr auto value = inplace_string<13U>{"Hello, World!"};
  REQUIRE(std::hash<inplace_string<13U>>{}(value) == std::hash<std::string_view>{}("Hello, World!"));
}

TEST_CASE("inplace_string is formatted", "[inplace_string]") {
  SECTION("with char") {
    constexpr auto value = inplace_string<13U>{"Hello, World!"};
#if __cplusplus > 202002L
    STATIC_REQUIRE(std::formattable<inplace_string<13U>, char>);
#endif  // __cplusplus > 202002L
    REQUIRE(std::format("{}", value) == "Hello, World!");
  }

  SECTION("with wchar_t") {
    constexpr auto value = inplace_wstring<13U>{L"Hello, World!"};
#if __cplusplus > 202002L
    STATIC_REQUIRE(std::formattable<inplace_wstring<13U>, wchar_t>);
#endif  // __cplusplus > 202002L
    REQUIRE(std::format(L"{}", value) == L"Hello, World!");
  }
}

}  // namespace gw
