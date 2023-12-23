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
  using tag_t = struct test_tag;
  using test_t = gw::strong_type<int, tag_t>;

  SECTION("public types") {
    STATIC_REQUIRE(std::is_same_v<test_t::value_type, int>);
    STATIC_REQUIRE(std::is_same_v<test_t::tag_type, tag_t>);
  }

  SECTION("default constructed") { STATIC_REQUIRE(std::is_nothrow_default_constructible_v<test_t>); }

  SECTION("constructed from int") { STATIC_REQUIRE(std::is_nothrow_constructible_v<test_t, int>); }

  SECTION("constructed from int&&") { STATIC_REQUIRE(std::is_nothrow_constructible_v<test_t, int&&>); }

  SECTION("constructed from const int&") { STATIC_REQUIRE(std::is_nothrow_constructible_v<test_t, const int&>); }

  SECTION("copy constructed") { STATIC_REQUIRE(std::is_nothrow_copy_constructible_v<test_t>); }

  SECTION("move constructed") { STATIC_REQUIRE(std::is_nothrow_move_constructible_v<test_t>); }

  SECTION("make_strong_type") {
    STATIC_REQUIRE(gw::make_strong_type<test_tag>(1) == test_t{1});
    STATIC_REQUIRE(noexcept(gw::make_strong_type<test_tag>(1)));
  }
}

TEST_CASE("strong_types are assigned", "[strong_type]") {
  using test_t = gw::strong_type<int, struct test_tag>;

  SECTION("copy assigned") { STATIC_REQUIRE(std::is_nothrow_copy_assignable_v<test_t>); }

  SECTION("move assigned") { STATIC_REQUIRE(std::is_nothrow_move_assignable_v<test_t>); }
}

TEST_CASE("strong_types are destroyed", "[strong_type]") {
  using test_t = gw::strong_type<int, struct test_tag>;

  STATIC_REQUIRE(std::is_nothrow_destructible_v<test_t>);
}

TEST_CASE("strong_types are distinguished by their tag", "[strong_type]") {
  using test1_t = gw::strong_type<struct strong_type_test_tag1, int>;
  using test2_t = gw::strong_type<struct strong_type_test_tag2, int>;

  STATIC_REQUIRE(std::is_same_v<test1_t, test1_t>);
  STATIC_REQUIRE(!std::is_same_v<test1_t, test2_t>);
}

TEST_CASE("strong_types are swapped", "[strong_type]") {
  using test_t = gw::strong_type<int, struct test_tag>;

  STATIC_REQUIRE(std::is_nothrow_swappable_v<test_t>);
}

TEST_CASE("strong_types are observed", "[strong_type]") {
  using test_t = gw::strong_type<int, struct test_tag>;

  SECTION("operator*") {
    STATIC_REQUIRE(test_t{}.operator*() == 0);
    STATIC_REQUIRE(noexcept(test_t{}.operator*()));
  }

  SECTION("operator->") {
    STATIC_REQUIRE(*test_t{}.operator->() == 0);
    STATIC_REQUIRE(noexcept(test_t{}.operator->()));
  }

  SECTION("value") {
    STATIC_REQUIRE(test_t{}.value() == 0);
    STATIC_REQUIRE(noexcept(test_t{}.value()));
  }
}

TEST_CASE("strong_types are transformed", "[strong_type]") {
  using test_t = gw::strong_type<int, struct test_tag>;

  auto test = []() constexpr {
    auto value = test_t{1};
    return value.transform([](auto value) { return value + 1; });
  };
  STATIC_REQUIRE(test() == test_t{2});

  STATIC_REQUIRE(noexcept(test_t{}.transform([](auto value) noexcept { return value + 1; })));
}

TEST_CASE("strong_types are reset", "[strong_type]") {
  using test_t = gw::strong_type<int, struct test_tag>;

  auto test = []() constexpr {
    auto value = test_t{1};
    value.reset();
    return value;
  };
  STATIC_REQUIRE(test() == test_t{0});

  STATIC_REQUIRE(noexcept(test_t{}.reset()));
}

TEST_CASE("strong_types are emplaced", "[strong_type]") {
  using test_t = gw::strong_type<int, struct test_tag>;

  STATIC_REQUIRE(test_t{}.emplace(1) == 1);
  STATIC_REQUIRE(noexcept(test_t{}.emplace(1)));
}

TEST_CASE("strong_types are compared", "[strong_type]") {
  using test_t = gw::strong_type<int, struct test_tag>;

  SECTION("equality") {
    STATIC_REQUIRE(test_t{1} == test_t{1});
    STATIC_REQUIRE(test_t{1} != test_t{2});
    STATIC_REQUIRE(noexcept(test_t{} == test_t{}));
    STATIC_REQUIRE(noexcept(test_t{} != test_t{}));
  }

  SECTION("less than") {
    STATIC_REQUIRE(test_t{1} < test_t{2});
    STATIC_REQUIRE(test_t{1} <= test_t{1});
    STATIC_REQUIRE(test_t{1} <= test_t{2});
    STATIC_REQUIRE(test_t{2} > test_t{1});
    STATIC_REQUIRE(test_t{1} >= test_t{1});
    STATIC_REQUIRE(test_t{2} >= test_t{1});
    STATIC_REQUIRE(noexcept(test_t{} < test_t{}));
    STATIC_REQUIRE(noexcept(test_t{} <= test_t{}));
    STATIC_REQUIRE(noexcept(test_t{} > test_t{}));
    STATIC_REQUIRE(noexcept(test_t{} >= test_t{}));
  }

  SECTION("greater than") {
    STATIC_REQUIRE(test_t{2} > test_t{1});
    STATIC_REQUIRE(test_t{2} >= test_t{2});
    STATIC_REQUIRE(test_t{2} >= test_t{1});
    STATIC_REQUIRE(test_t{1} < test_t{2});
    STATIC_REQUIRE(test_t{2} <= test_t{2});
    STATIC_REQUIRE(test_t{1} <= test_t{2});
    STATIC_REQUIRE(noexcept(test_t{} > test_t{}));
    STATIC_REQUIRE(noexcept(test_t{} >= test_t{}));
    STATIC_REQUIRE(noexcept(test_t{} < test_t{}));
    STATIC_REQUIRE(noexcept(test_t{} <= test_t{}));
  }

  STATIC_REQUIRE(std::equality_comparable<test_t>);
  STATIC_REQUIRE(std::totally_ordered<test_t>);
  STATIC_REQUIRE(std::three_way_comparable<test_t>);
}

TEST_CASE("strong_types are converted", "[strong_type]") {
  using test_t = gw::strong_type<int, struct test_tag>;

  SECTION("explicit conversion") {
    STATIC_REQUIRE(static_cast<int>(test_t{}) == 0);
    STATIC_REQUIRE(noexcept(static_cast<int>(test_t{})));
  }
}

TEST_CASE("strong_types are incremented and decremented", "[strong_type]") {
  using test_t = gw::strong_type<int, struct test_tag>;

  SECTION("pre-increment") {
    STATIC_REQUIRE(++test_t{} == test_t{1});
    STATIC_REQUIRE(noexcept(++test_t{}));
  }

  SECTION("post-increment") {
    auto test = []() constexpr {
      auto value = test_t{1};
      return value++;
    };
    STATIC_REQUIRE(test() == test_t{1});

    auto value = test_t{};
    STATIC_REQUIRE(noexcept(value++));
  }

  SECTION("pre-decrement") {
    STATIC_REQUIRE(--test_t{} == test_t{-1});
    STATIC_REQUIRE(noexcept(--test_t{}));
  }

  SECTION("post-decrement") {
    auto test = []() constexpr {
      auto value = test_t{1};
      return value--;
    };
    STATIC_REQUIRE(test() == test_t{1});

    auto value = test_t{1};
    STATIC_REQUIRE(noexcept(value--));
  }
}

TEST_CASE("strong_types are added and subtracted", "[strong_type]") {
  using test_t = gw::strong_type<int, struct test_tag>;

  SECTION("strong_type + strong_type") {
    STATIC_REQUIRE(test_t{1} + test_t{1} == test_t{2});
    STATIC_REQUIRE(noexcept(test_t{} + test_t{}));
  }

  SECTION("strong_type - strong_type") {
    STATIC_REQUIRE(test_t{1} - test_t{1} == test_t{0});
    STATIC_REQUIRE(noexcept(test_t{} - test_t{}));
  }
}

TEST_CASE("strong_types are multiplied and divided", "[strong_type]") {
  using test_t = gw::strong_type<int, struct test_tag>;

  SECTION("strong_type * strong_type") {
    STATIC_REQUIRE(test_t{2} * test_t{2} == test_t{4});
    STATIC_REQUIRE(noexcept(test_t{} * test_t{}));
  }

  SECTION("strong_type / strong_type") {
    STATIC_REQUIRE(test_t{4} / test_t{2} == test_t{2});
    STATIC_REQUIRE(noexcept(test_t{} / test_t{}));
  }
}

TEST_CASE("strong_types are modulo reduced", "[strong_type]") {
  using test_t = gw::strong_type<int, struct test_tag>;

  STATIC_REQUIRE(test_t{4} % test_t{3} == test_t{1});
  STATIC_REQUIRE(noexcept(test_t{} % test_t{}));
}

TEST_CASE("strong_types are added and subtracted with assignment", "[strong_type]") {
  using test_t = gw::strong_type<int, struct test_tag>;

  SECTION("strong_type += strong_type") {
    auto test = []() constexpr {
      auto value = test_t{1};
      return value += test_t{1};
    };
    STATIC_REQUIRE(test() == test_t{2});

    auto value = test_t{1};
    STATIC_REQUIRE(noexcept(value += test_t{1}));
  }

  SECTION("strong_type -= strong_type") {
    auto test = []() constexpr {
      auto value = test_t{1};
      return value -= test_t{1};
    };
    STATIC_REQUIRE(test() == test_t{0});

    auto value = test_t{1};
    STATIC_REQUIRE(noexcept(value -= test_t{1}));
  }
}

TEST_CASE("strong_types are multiplied and divided with assignment", "[strong_type]") {
  using test_t = gw::strong_type<int, struct test_tag>;

  SECTION("strong_type *= strong_type") {
    auto test = []() constexpr {
      auto value = test_t{2};
      return value *= test_t{2};
    };
    STATIC_REQUIRE(test() == test_t{4});

    auto value = test_t{2};
    STATIC_REQUIRE(noexcept(value *= test_t{2}));
  }

  SECTION("strong_type /= strong_type") {
    auto test = []() constexpr {
      auto value = test_t{4};
      return value /= test_t{2};
    };
    STATIC_REQUIRE(test() == test_t{2});

    auto value = test_t{4};
    STATIC_REQUIRE(noexcept(value /= test_t{2}));
  }
}

TEST_CASE("strong_types are bitwise operated", "[strong_type]") {
  using test_t = gw::strong_type<unsigned int, struct test_tag>;

  SECTION("strong_type ~") {
    STATIC_REQUIRE(~test_t{1U} == test_t{~1U});
    STATIC_REQUIRE(noexcept(~test_t{}));
  }

  SECTION("strong_type & strong_type") {
    STATIC_REQUIRE((test_t{1U} & test_t{1U}) == test_t{1U});
    STATIC_REQUIRE(noexcept(test_t{} & test_t{}));
  }

  SECTION("strong_type | strong_type") {
    STATIC_REQUIRE((test_t{1U} | test_t{1U}) == test_t{1U});
    STATIC_REQUIRE(noexcept(test_t{} | test_t{}));
  }

  SECTION("strong_type ^ strong_type") {
    STATIC_REQUIRE((test_t{1U} ^ test_t{1U}) == test_t{0U});
    STATIC_REQUIRE(noexcept(test_t{} ^ test_t{}));
  }
}

TEST_CASE("strong_types are bitwise shifted", "[strong_type]") {
  using test_t = gw::strong_type<unsigned int, struct test_tag>;

  SECTION("strong_type << strong_type") {
    STATIC_REQUIRE((test_t{1U} << test_t{1U}) == test_t{2U});
    STATIC_REQUIRE(noexcept(test_t{} << test_t{}));
  }

  SECTION("strong_type >> strong_type") {
    STATIC_REQUIRE((test_t{2U} >> test_t{1U}) == test_t{1U});
    STATIC_REQUIRE(noexcept(test_t{} >> test_t{}));
  }
}

TEST_CASE("strong_types are bitwise operated with assignment", "[strong_type]") {
  using test_t = gw::strong_type<unsigned int, struct test_tag>;

  SECTION("strong_type &= strong_type") {
    auto test = []() constexpr {
      auto value = test_t{1U};
      return value &= test_t{1U};
    };
    STATIC_REQUIRE(test() == test_t{1U});

    auto value = test_t{1U};
    STATIC_REQUIRE(noexcept(value &= test_t{1U}));
  }

  SECTION("strong_type |= strong_type") {
    auto test = []() constexpr {
      auto value = test_t{1U};
      return value |= test_t{1U};
    };
    STATIC_REQUIRE(test() == test_t{1U});

    auto value = test_t{1U};
    STATIC_REQUIRE(noexcept(value |= test_t{1U}));
  }

  SECTION("strong_type ^= strong_type") {
    auto test = []() constexpr {
      auto value = test_t{1U};
      return value ^= test_t{1U};
    };
    STATIC_REQUIRE(test() == test_t{0U});

    auto value = test_t{1U};
    STATIC_REQUIRE(noexcept(value ^= test_t{1U}));
  }
}

TEST_CASE("strong_types are bitwise shifted with assignment", "[strong_type]") {
  using test_t = gw::strong_type<unsigned int, struct test_tag>;

  SECTION("strong_type <<= strong_type") {
    auto test = []() constexpr {
      auto value = test_t{1U};
      return value <<= test_t{1U};
    };
    STATIC_REQUIRE(test() == test_t{2U});

    auto value = test_t{1U};
    STATIC_REQUIRE(noexcept(value <<= test_t{1U}));
  }

  SECTION("strong_type >>= strong_type") {
    auto test = []() constexpr {
      auto value = test_t{2U};
      return value >>= test_t{1U};
    };
    STATIC_REQUIRE(test() == test_t{1U});

    auto value = test_t{2U};
    STATIC_REQUIRE(noexcept(value >>= test_t{1U}));
  }
}

TEST_CASE("strong_types are viewable ranges", "[strong_type]") {
  constexpr auto k_value = gw::make_strong_type<struct test_tag>(std::array{1, 2, 3, 4, 5});

  STATIC_REQUIRE(!k_value.empty());
  STATIC_REQUIRE(k_value.size() == 5);
  STATIC_REQUIRE(k_value[0] == 1);
  STATIC_REQUIRE(k_value[1] == 2);
  STATIC_REQUIRE(k_value[2] == 3);
  STATIC_REQUIRE(k_value[3] == 4);
  STATIC_REQUIRE(k_value[4] == 5);

  using test_t = std::remove_cv_t<decltype(k_value)>;
  STATIC_REQUIRE(std::ranges::viewable_range<test_t>);
}

TEST_CASE("strong_types are hashed", "[strong_type]") {
  struct test_tag {};
  using test_t = gw::strong_type<int, test_tag>;

  STATIC_REQUIRE(gw::hashable<test_t>);
}

TEST_CASE("strong_types are streamed", "[strong_type]") {
  using test_t = gw::strong_type<int, struct test_tag>;

  SECTION("ostream strong_type") {
    STATIC_REQUIRE(gw::ostreamable<test_t>);

    auto sstream = std::stringstream{};
    sstream << test_t{1};
    REQUIRE(sstream.str() == "1");
  }

  SECTION("istream strong_type") {
    STATIC_REQUIRE(gw::istreamable<test_t>);

    auto sstream = std::stringstream{};
    sstream << "1";
    auto value = test_t{};
    sstream >> value;
    REQUIRE(value == test_t{1});
  }
}

TEST_CASE("strong_types are converted to string", "[strong_type]") {
  SECTION("unnamed strong_type") {
    using test_t = gw::strong_type<int, struct test_tag>;

    REQUIRE(std::to_string(test_t{1}) == "1");
  }

  SECTION("named strong_type") {
    struct strong_type_named_tag {
      static constexpr auto name() noexcept { return "TestType"; }
    };
    using strong_type_named = gw::strong_type<int, strong_type_named_tag>;

    REQUIRE(std::to_string(strong_type_named{1}) == "TestType: 1");
  }
}
