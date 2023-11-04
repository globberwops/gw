#include "gw/strong_type.hpp"

#include <catch2/catch_test_macros.hpp>
#include <type_traits>

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

TEST_CASE("strong_types are compared", "[strong_type]") {
  using strong_type_test = gw::strong_type<struct strong_type_test_tag, int>;

  SECTION("operator==") {
    STATIC_REQUIRE(strong_type_test{} == strong_type_test{});
    STATIC_REQUIRE(noexcept(strong_type_test{} == strong_type_test{}));
  }

  SECTION("operator!=") {
    STATIC_REQUIRE(strong_type_test{} != strong_type_test{1});
    STATIC_REQUIRE(noexcept(strong_type_test{} != strong_type_test{}));
  }

  SECTION("operator<=") {
    STATIC_REQUIRE(strong_type_test{} <= strong_type_test{1});
    STATIC_REQUIRE(noexcept(strong_type_test{} <= strong_type_test{}));
  }

  SECTION("operator<") {
    STATIC_REQUIRE(strong_type_test{} < strong_type_test{1});
    STATIC_REQUIRE(noexcept(strong_type_test{} < strong_type_test{}));
  }

  SECTION("operator>=") {
    STATIC_REQUIRE(strong_type_test{1} >= strong_type_test{});
    STATIC_REQUIRE(noexcept(strong_type_test{} >= strong_type_test{}));
  }

  SECTION("operator>") {
    STATIC_REQUIRE(strong_type_test{1} > strong_type_test{});
    STATIC_REQUIRE(noexcept(strong_type_test{} > strong_type_test{}));
  }
}

TEST_CASE("strong_types are hashed", "[strong_type]") {
  using strong_type_test = gw::strong_type<struct strong_type_test_tag, int>;

  STATIC_REQUIRE(std::is_nothrow_invocable_r_v<std::size_t, std::hash<strong_type_test>, strong_type_test>);
}
