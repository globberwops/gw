// Copyright (c) 2023 Martin Stump
// SPDX-License-Identifier: BSL-1.0

#include "gw/strong_type.hpp"

#include <array>
#include <catch2/catch_test_macros.hpp>
#include <concepts>
#include <ranges>
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

  auto test = []() constexpr {
    auto value = strong_type_test{1};
    return value.transform([](auto value) { return value + 1; });
  };
  STATIC_REQUIRE(test() == strong_type_test{2});

  STATIC_REQUIRE(noexcept(strong_type_test{}.transform([](auto value) noexcept { return value + 1; })));
}

TEST_CASE("strong_types are reset", "[strong_type]") {
  using strong_type_test = gw::strong_type<struct strong_type_test_tag, int>;

  auto test = []() constexpr {
    auto value = strong_type_test{1};
    value.reset();
    return value;
  };
  STATIC_REQUIRE(test() == strong_type_test{0});

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
    auto test = []() constexpr {
      auto value = strong_type_test{1};
      return value++;
    };
    STATIC_REQUIRE(test() == strong_type_test{1});

    auto value = strong_type_test{};
    STATIC_REQUIRE(noexcept(value++));
  }

  SECTION("pre-decrement") {
    STATIC_REQUIRE(--strong_type_test{} == strong_type_test{-1});
    STATIC_REQUIRE(noexcept(--strong_type_test{}));
  }

  SECTION("post-decrement") {
    auto test = []() constexpr {
      auto value = strong_type_test{1};
      return value--;
    };
    STATIC_REQUIRE(test() == strong_type_test{1});

    auto value = strong_type_test{1};
    STATIC_REQUIRE(noexcept(value--));
  }
}

TEST_CASE("strong_types are added and subtracted", "[strong_type]") {
  using strong_type_test = gw::strong_type<struct strong_type_test_tag, int>;

  SECTION("strong_type + strong_type") {
    STATIC_REQUIRE(strong_type_test{1} + strong_type_test{1} == strong_type_test{2});
    STATIC_REQUIRE(noexcept(strong_type_test{} + strong_type_test{}));
  }

  SECTION("strong_type - strong_type") {
    STATIC_REQUIRE(strong_type_test{1} - strong_type_test{1} == strong_type_test{0});
    STATIC_REQUIRE(noexcept(strong_type_test{} - strong_type_test{}));
  }
}

TEST_CASE("strong_types are multiplied and divided", "[strong_type]") {
  using strong_type_test = gw::strong_type<struct strong_type_test_tag, int>;

  SECTION("strong_type * strong_type") {
    STATIC_REQUIRE(strong_type_test{2} * strong_type_test{2} == strong_type_test{4});
    STATIC_REQUIRE(noexcept(strong_type_test{} * strong_type_test{}));
  }

  SECTION("strong_type / strong_type") {
    STATIC_REQUIRE(strong_type_test{4} / strong_type_test{2} == strong_type_test{2});
    STATIC_REQUIRE(noexcept(strong_type_test{} / strong_type_test{}));
  }
}

TEST_CASE("strong_types are modulo reduced", "[strong_type]") {
  using strong_type_test = gw::strong_type<struct strong_type_test_tag, int>;

  STATIC_REQUIRE(strong_type_test{4} % strong_type_test{3} == strong_type_test{1});
  STATIC_REQUIRE(noexcept(strong_type_test{} % strong_type_test{}));
}

TEST_CASE("strong_types are added and subtracted with assignment", "[strong_type]") {
  using strong_type_test = gw::strong_type<struct strong_type_test_tag, int>;

  SECTION("strong_type += strong_type") {
    auto test = []() constexpr {
      auto value = strong_type_test{1};
      return value += strong_type_test{1};
    };
    STATIC_REQUIRE(test() == strong_type_test{2});

    auto value = strong_type_test{1};
    STATIC_REQUIRE(noexcept(value += strong_type_test{1}));
  }

  SECTION("strong_type -= strong_type") {
    auto test = []() constexpr {
      auto value = strong_type_test{1};
      return value -= strong_type_test{1};
    };
    STATIC_REQUIRE(test() == strong_type_test{0});

    auto value = strong_type_test{1};
    STATIC_REQUIRE(noexcept(value -= strong_type_test{1}));
  }
}

TEST_CASE("strong_types are multiplied and divided with assignment", "[strong_type]") {
  using strong_type_test = gw::strong_type<struct strong_type_test_tag, int>;

  SECTION("strong_type *= strong_type") {
    auto test = []() constexpr {
      auto value = strong_type_test{2};
      return value *= strong_type_test{2};
    };
    STATIC_REQUIRE(test() == strong_type_test{4});

    auto value = strong_type_test{2};
    STATIC_REQUIRE(noexcept(value *= strong_type_test{2}));
  }

  SECTION("strong_type /= strong_type") {
    auto test = []() constexpr {
      auto value = strong_type_test{4};
      return value /= strong_type_test{2};
    };
    STATIC_REQUIRE(test() == strong_type_test{2});

    auto value = strong_type_test{4};
    STATIC_REQUIRE(noexcept(value /= strong_type_test{2}));
  }
}

TEST_CASE("strong_types are bitwise operated", "[strong_type]") {
  using strong_type_test = gw::strong_type<struct strong_type_test_tag, unsigned int>;

  SECTION("strong_type & strong_type") {
    STATIC_REQUIRE((strong_type_test{1U} & strong_type_test{1U}) == strong_type_test{1U});
    STATIC_REQUIRE(noexcept(strong_type_test{} & strong_type_test{}));
  }

  SECTION("strong_type | strong_type") {
    STATIC_REQUIRE((strong_type_test{1U} | strong_type_test{1U}) == strong_type_test{1U});
    STATIC_REQUIRE(noexcept(strong_type_test{} | strong_type_test{}));
  }

  SECTION("strong_type ^ strong_type") {
    STATIC_REQUIRE((strong_type_test{1U} ^ strong_type_test{1U}) == strong_type_test{0U});
    STATIC_REQUIRE(noexcept(strong_type_test{} ^ strong_type_test{}));
  }
}

TEST_CASE("strong_types are bitwise shifted", "[strong_type]") {
  using strong_type_test = gw::strong_type<struct strong_type_test_tag, unsigned int>;

  SECTION("strong_type << strong_type") {
    STATIC_REQUIRE((strong_type_test{1U} << strong_type_test{1U}) == strong_type_test{2U});
    STATIC_REQUIRE(noexcept(strong_type_test{} << strong_type_test{}));
  }

  SECTION("strong_type >> strong_type") {
    STATIC_REQUIRE((strong_type_test{2U} >> strong_type_test{1U}) == strong_type_test{1U});
    STATIC_REQUIRE(noexcept(strong_type_test{} >> strong_type_test{}));
  }
}

TEST_CASE("strong_types are bitwise operated with assignment", "[strong_type]") {
  using strong_type_test = gw::strong_type<struct strong_type_test_tag, unsigned int>;

  SECTION("strong_type &= strong_type") {
    auto test = []() constexpr {
      auto value = strong_type_test{1U};
      return value &= strong_type_test{1U};
    };
    STATIC_REQUIRE(test() == strong_type_test{1U});

    auto value = strong_type_test{1U};
    STATIC_REQUIRE(noexcept(value &= strong_type_test{1U}));
  }

  SECTION("strong_type |= strong_type") {
    auto test = []() constexpr {
      auto value = strong_type_test{1U};
      return value |= strong_type_test{1U};
    };
    STATIC_REQUIRE(test() == strong_type_test{1U});

    auto value = strong_type_test{1U};
    STATIC_REQUIRE(noexcept(value |= strong_type_test{1U}));
  }

  SECTION("strong_type ^= strong_type") {
    auto test = []() constexpr {
      auto value = strong_type_test{1U};
      return value ^= strong_type_test{1U};
    };
    STATIC_REQUIRE(test() == strong_type_test{0U});

    auto value = strong_type_test{1U};
    STATIC_REQUIRE(noexcept(value ^= strong_type_test{1U}));
  }
}

TEST_CASE("strong_types are bitwise shifted with assignment", "[strong_type]") {
  using strong_type_test = gw::strong_type<struct strong_type_test_tag, unsigned int>;

  SECTION("strong_type <<= strong_type") {
    auto test = []() constexpr {
      auto value = strong_type_test{1U};
      return value <<= strong_type_test{1U};
    };
    STATIC_REQUIRE(test() == strong_type_test{2U});

    auto value = strong_type_test{1U};
    STATIC_REQUIRE(noexcept(value <<= strong_type_test{1U}));
  }

  SECTION("strong_type >>= strong_type") {
    auto test = []() constexpr {
      auto value = strong_type_test{2U};
      return value >>= strong_type_test{1U};
    };
    STATIC_REQUIRE(test() == strong_type_test{1U});

    auto value = strong_type_test{2U};
    STATIC_REQUIRE(noexcept(value >>= strong_type_test{1U}));
  }
}

#ifdef GW_ENABLE_RANGES_INTERFACE
TEST_CASE("strong_types are viewable ranges", "[strong_type]") {
  constexpr auto k_value = gw::make_strong_type<struct strong_type_test_tag>(std::array{1, 2, 3, 4, 5});

  STATIC_REQUIRE(!k_value.empty());
  STATIC_REQUIRE(k_value.size() == 5);
  STATIC_REQUIRE(k_value[0] == 1);
  STATIC_REQUIRE(k_value[1] == 2);
  STATIC_REQUIRE(k_value[2] == 3);
  STATIC_REQUIRE(k_value[3] == 4);
  STATIC_REQUIRE(k_value[4] == 5);

  using strong_type_test = std::remove_cv_t<decltype(k_value)>;
  STATIC_REQUIRE(std::ranges::viewable_range<strong_type_test>);
}
#endif  // GW_ENABLE_RANGES_INTERFACE

#ifdef GW_ENABLE_HASH_CALCULATION
TEST_CASE("strong_types are hashed", "[strong_type]") {
  struct strong_type_test_tag {};
  using strong_type_test = gw::strong_type<strong_type_test_tag, int>;

  STATIC_REQUIRE(gw::hashable<strong_type_test>);
}
#endif  // GW_ENABLE_HASH_CALCULATION

#ifdef GW_ENABLE_STREAM_OPERATORS
TEST_CASE("strong_types are streamed", "[strong_type]") {
  using strong_type_test = gw::strong_type<struct strong_type_test_tag, int>;

  SECTION("ostream strong_type") {
    STATIC_REQUIRE(gw::ostreamable<strong_type_test>);

    auto sstream = std::stringstream{};
    sstream << strong_type_test{1};
    REQUIRE(sstream.str() == "1");
  }

  SECTION("istream strong_type") {
    STATIC_REQUIRE(gw::istreamable<strong_type_test>);

    auto sstream = std::stringstream{};
    sstream << "1";
    auto value = strong_type_test{};
    sstream >> value;
    REQUIRE(value == strong_type_test{1});
  }
}
#endif  // GW_ENABLE_STREAM_OPERATORS

#ifdef GW_ENABLE_STRING_CONVERSION
TEST_CASE("strong_types are converted to string", "[strong_type]") {
  SECTION("unnamed strong_type") {
    using strong_type_test = gw::strong_type<struct strong_type_test_tag, int>;

    REQUIRE(std::to_string(strong_type_test{1}) == "1");
  }

  SECTION("named strong_type") {
    struct strong_type_named_tag {
      static constexpr auto name() noexcept { return "StrongTypeNamedTag"; }
    };
    using strong_type_named = gw::strong_type<strong_type_named_tag, int>;

    REQUIRE(std::to_string(strong_type_named{1}) == "StrongTypeNamedTag: 1");
  }
}
#endif  // GW_ENABLE_STRING_CONVERSION
