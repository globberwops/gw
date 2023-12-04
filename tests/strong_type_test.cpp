// Copyright (c) 2023 Martin Stump
// SPDX-License-Identifier: BSL-1.0

#include "gw/strong_type.hpp"

#include <catch2/catch_test_macros.hpp>
#include <concepts>
#include <sstream>
#include <type_traits>

#include "gw/concepts.hpp"

TEST_CASE("strong_types are constructed", "[strong_type]") {
  using strong_type_test = gw::strong_type<struct strong_type_test_tag, int>;

  SECTION("default constructed") { STATIC_REQUIRE(std::is_nothrow_default_constructible_v<strong_type_test>); }

  SECTION("constructed from int") { STATIC_REQUIRE(std::is_nothrow_constructible_v<strong_type_test, int>); }

  SECTION("constructed from int&&") { STATIC_REQUIRE(std::is_nothrow_constructible_v<strong_type_test, int&&>); }

  SECTION("constructed from const int&") {
    STATIC_REQUIRE(std::is_nothrow_constructible_v<strong_type_test, const int&>);
  }

  SECTION("copy constructed") { STATIC_REQUIRE(std::is_nothrow_copy_constructible_v<strong_type_test>); }

  SECTION("move constructed") { STATIC_REQUIRE(std::is_nothrow_move_constructible_v<strong_type_test>); }

  SECTION("make_strong_type") {
    STATIC_REQUIRE(gw::make_strong_type<strong_type_test_tag>(1) == strong_type_test{1});
    STATIC_REQUIRE(noexcept(gw::make_strong_type<strong_type_test_tag>(1)));
  }
}

TEST_CASE("strong_types are assigned", "[strong_type]") {
  using strong_type_test = gw::strong_type<struct strong_type_test_tag, int>;

  SECTION("copy assigned") { STATIC_REQUIRE(std::is_nothrow_copy_assignable_v<strong_type_test>); }

  SECTION("move assigned") { STATIC_REQUIRE(std::is_nothrow_move_assignable_v<strong_type_test>); }
}

TEST_CASE("strong_types are destroyed", "[strong_type]") {
  using strong_type_test = gw::strong_type<struct strong_type_test_tag, int>;

  STATIC_REQUIRE(std::is_nothrow_destructible_v<strong_type_test>);
}

TEST_CASE("strong_types are swapped", "[strong_type]") {
  using strong_type_test = gw::strong_type<struct strong_type_test_tag, int>;

  STATIC_REQUIRE(std::is_nothrow_swappable_v<strong_type_test>);
}

TEST_CASE("strong_types are observed", "[strong_type]") {
  using strong_type_test = gw::strong_type<struct strong_type_test_tag, int>;

  SECTION("operator*") {
    STATIC_REQUIRE(strong_type_test{}.operator*() == 0);
    STATIC_REQUIRE(noexcept(strong_type_test{}.operator*()));
  }

  SECTION("operator->") {
    STATIC_REQUIRE(*strong_type_test{}.operator->() == 0);
    STATIC_REQUIRE(noexcept(strong_type_test{}.operator->()));
  }

  SECTION("value") {
    STATIC_REQUIRE(strong_type_test{}.value() == 0);
    STATIC_REQUIRE(noexcept(strong_type_test{}.value()));
  }
}

TEST_CASE("strong_types are transformed", "[strong_type]") {
  using strong_type_test = gw::strong_type<struct strong_type_test_tag, int>;

  STATIC_REQUIRE(noexcept(strong_type_test{}.transform([](auto value) noexcept { return value + 1; })));
}

TEST_CASE("strong_types are reset", "[strong_type]") {
  using strong_type_test = gw::strong_type<struct strong_type_test_tag, int>;

  STATIC_REQUIRE(noexcept(strong_type_test{}.reset()));
}

TEST_CASE("strong_types are emplaced", "[strong_type]") {
  using strong_type_test = gw::strong_type<struct strong_type_test_tag, int>;

  STATIC_REQUIRE(strong_type_test{}.emplace(1) == 1);
  STATIC_REQUIRE(noexcept(strong_type_test{}.emplace(1)));
}

TEST_CASE("strong_types are totally ordered", "[strong_type]") {
  using strong_type_test = gw::strong_type<struct strong_type_test_tag, int>;

  STATIC_REQUIRE(std::totally_ordered<strong_type_test>);
}

TEST_CASE("strong_types are converted", "[strong_type]") {
  using strong_type_test = gw::strong_type<struct strong_type_test_tag, int>;

  SECTION("explicit conversion") {
    STATIC_REQUIRE(static_cast<int>(strong_type_test{}) == 0);
    STATIC_REQUIRE(noexcept(static_cast<int>(strong_type_test{})));
  }
}

TEST_CASE("strong_types are incremented and decremented", "[strong_type]") {
  using strong_type_test = gw::strong_type<struct strong_type_test_tag, int>;

  SECTION("pre-increment") {
    STATIC_REQUIRE(++strong_type_test{} == strong_type_test{1});
    STATIC_REQUIRE(noexcept(++strong_type_test{}));
  }

  SECTION("post-increment") {
    auto value = strong_type_test{};
    REQUIRE(value++ == strong_type_test{0});
    REQUIRE(value == strong_type_test{1});
    STATIC_REQUIRE(noexcept(value++));
  }

  SECTION("pre-decrement") {
    STATIC_REQUIRE(--strong_type_test{} == strong_type_test{-1});
    STATIC_REQUIRE(noexcept(--strong_type_test{}));
  }

  SECTION("post-decrement") {
    auto value = strong_type_test{1};
    REQUIRE(value-- == strong_type_test{1});
    REQUIRE(value == strong_type_test{0});
    STATIC_REQUIRE(noexcept(value--));
  }
}

#ifdef GW_ENABLE_HASH_CALCULATION
TEST_CASE("strong_types are hashed", "[strong_type]") {
  struct strong_type_test_tag {};
  using strong_type_test = gw::strong_type<strong_type_test_tag, int>;

  STATIC_REQUIRE(gw::hashable<strong_type_test>);
}
#endif  // GW_ENABLE_HASH_CALCULATION

#ifdef GW_ENABLE_STRING_CONVERSION
TEST_CASE("strong_types are converted to string", "[strong_type]") {
  SECTION("unnamed strong_type") {
    using strong_type_test = gw::strong_type<struct strong_type_test_tag, int>;

    REQUIRE(std::to_string(strong_type_test{1}) == "strong_type: 1");
  }

  SECTION("named strong_type") {
    struct strong_type_named_tag {
      static constexpr auto name() noexcept { return "strong_type_named_tag"; }
    };
    using strong_type_named = gw::strong_type<strong_type_named_tag, int>;

    REQUIRE(std::to_string(strong_type_named{1}) == "strong_type_named_tag: 1");
  }
}
#endif  // GW_ENABLE_STRING_CONVERSION

#ifdef GW_ENABLE_STREAM_OPERATORS
TEST_CASE("strong_types are streamed", "[strong_type]") {
  SECTION("unnamed strong_type") {
    using strong_type_test = gw::strong_type<struct strong_type_test_tag, int>;

    auto sstream = std::stringstream{};
    sstream << strong_type_test{1};
    REQUIRE(sstream.str() == "strong_type: 1");
  }

  SECTION("named strong_type") {
    struct strong_type_named_tag {
      static constexpr auto name() noexcept { return "strong_type_named_tag"; }
    };
    using strong_type_named = gw::strong_type<strong_type_named_tag, int>;

    auto sstream = std::stringstream{};
    sstream << strong_type_named{1};
    REQUIRE(sstream.str() == "strong_type_named_tag: 1");
  }
}
#endif  // GW_ENABLE_STREAM_OPERATORS
