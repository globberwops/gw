// Copyright (c) 2023 Martin Stump
// SPDX-License-Identifier: BSL-1.0

#include "gw/named_type.hpp"

#include <array>
#include <catch2/catch_test_macros.hpp>
#include <concepts>
#include <format>
#include <ranges>
#include <sstream>
#include <type_traits>

#include "gw/concepts.hpp"

TEST_CASE("named_types are constructed", "[named_type]") {
  using test_t = gw::named_type<int, "TestType">;

  SECTION("public types") { STATIC_REQUIRE(std::is_same_v<test_t::value_type, int>); }

  SECTION("default constructed") { STATIC_REQUIRE(std::is_nothrow_default_constructible_v<test_t>); }

  SECTION("constructed from int") { STATIC_REQUIRE(std::is_nothrow_constructible_v<test_t, int>); }

  SECTION("constructed from int&&") { STATIC_REQUIRE(std::is_nothrow_constructible_v<test_t, int&&>); }

  SECTION("constructed from const int&") { STATIC_REQUIRE(std::is_nothrow_constructible_v<test_t, const int&>); }

  SECTION("copy constructed") { STATIC_REQUIRE(std::is_nothrow_copy_constructible_v<test_t>); }

  SECTION("move constructed") { STATIC_REQUIRE(std::is_nothrow_move_constructible_v<test_t>); }

  SECTION("make_named_type") {
    STATIC_REQUIRE(gw::make_named_type<"TestType">(1) == test_t{1});
    STATIC_REQUIRE(noexcept(gw::make_named_type<"TestType">(1)));
  }
}

TEST_CASE("named_types are assigned", "[named_type]") {
  using test_t = gw::named_type<int, "TestType">;

  SECTION("copy assigned") { STATIC_REQUIRE(std::is_nothrow_copy_assignable_v<test_t>); }

  SECTION("move assigned") { STATIC_REQUIRE(std::is_nothrow_move_assignable_v<test_t>); }
}

TEST_CASE("named_types are destroyed", "[named_type]") {
  using test_t = gw::named_type<int, "TestType">;

  STATIC_REQUIRE(std::is_nothrow_destructible_v<test_t>);
}

TEST_CASE("named_types are distinguished by their name", "[named_type]") {
  using test1_t = gw::named_type<int, "TestType1">;
  using test2_t = gw::named_type<int, "TestType2">;

  SECTION("named_type::name()") {
    STATIC_REQUIRE(test1_t::name() == "TestType1");
    STATIC_REQUIRE(test2_t::name() == "TestType2");
  }

  SECTION("named_type type") {
    STATIC_REQUIRE(std::is_same_v<test1_t, test1_t>);
    STATIC_REQUIRE(!std::is_same_v<test1_t, test2_t>);
  }
}

TEST_CASE("named_types are swapped", "[named_type]") {
  using test_t = gw::named_type<int, "TestType">;

  STATIC_REQUIRE(std::is_nothrow_swappable_v<test_t>);
}

TEST_CASE("named_types are observed", "[named_type]") {
  using test_t = gw::named_type<int, "TestType">;

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

TEST_CASE("named_types are transformed", "[named_type]") {
  using test_t = gw::named_type<int, "TestType">;

  auto test = []() constexpr {
    auto value = test_t{1};
    return value.transform([](auto value) { return value + 1; });
  };
  STATIC_REQUIRE(test() == test_t{2});

  STATIC_REQUIRE(noexcept(test_t{}.transform([](auto value) noexcept { return value + 1; })));
}

TEST_CASE("named_types are reset", "[named_type]") {
  using test_t = gw::named_type<int, "TestType">;

  auto test = []() constexpr {
    auto value = test_t{1};
    value.reset();
    return value;
  };
  STATIC_REQUIRE(test() == test_t{0});

  STATIC_REQUIRE(noexcept(test_t{}.reset()));
}

TEST_CASE("named_types are emplaced", "[named_type]") {
  using test_t = gw::named_type<int, "TestType">;

  STATIC_REQUIRE(test_t{}.emplace(1) == 1);
  STATIC_REQUIRE(noexcept(test_t{}.emplace(1)));
}

TEST_CASE("named_types are compared", "[named_type]") {
  using test_t = gw::named_type<int, "TestType">;

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

TEST_CASE("named_types are converted", "[named_type]") {
  using test_t = gw::named_type<int, "TestType">;

  SECTION("explicit conversion") {
    STATIC_REQUIRE(static_cast<int>(test_t{}) == 0);
    STATIC_REQUIRE(noexcept(static_cast<int>(test_t{})));
  }
}

TEST_CASE("named_types are incremented and decremented", "[named_type]") {
  using test_t = gw::named_type<int, "TestType">;

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

TEST_CASE("named_types are affirmed and negated", "[named_type]") {
  using test_t = gw::named_type<int, "TestType">;

  SECTION("affirmation") {
    STATIC_REQUIRE(+test_t{1} == test_t{1});
    STATIC_REQUIRE(noexcept(+test_t{}));
  }

  SECTION("negation") {
    STATIC_REQUIRE(-test_t{1} == test_t{-1});
    STATIC_REQUIRE(noexcept(-test_t{}));
  }
}

TEST_CASE("named_types are added and subtracted", "[named_type]") {
  using test_t = gw::named_type<int, "TestType">;

  SECTION("named_type + named_type") {
    STATIC_REQUIRE(test_t{1} + test_t{1} == test_t{2});
    STATIC_REQUIRE(noexcept(test_t{} + test_t{}));
  }

  SECTION("named_type - named_type") {
    STATIC_REQUIRE(test_t{1} - test_t{1} == test_t{0});
    STATIC_REQUIRE(noexcept(test_t{} - test_t{}));
  }
}

TEST_CASE("named_types are multiplied and divided", "[named_type]") {
  using test_t = gw::named_type<int, "TestType">;

  SECTION("named_type * named_type") {
    STATIC_REQUIRE(test_t{2} * test_t{2} == test_t{4});
    STATIC_REQUIRE(noexcept(test_t{} * test_t{}));
  }

  SECTION("named_type / named_type") {
    STATIC_REQUIRE(test_t{4} / test_t{2} == test_t{2});
    STATIC_REQUIRE(noexcept(test_t{} / test_t{}));
  }
}

TEST_CASE("named_types are modulo reduced", "[named_type]") {
  using test_t = gw::named_type<int, "TestType">;

  STATIC_REQUIRE(test_t{4} % test_t{3} == test_t{1});
  STATIC_REQUIRE(noexcept(test_t{} % test_t{}));
}

TEST_CASE("named_types are added and subtracted with assignment", "[named_type]") {
  using test_t = gw::named_type<int, "TestType">;

  SECTION("named_type += named_type") {
    auto test = []() constexpr {
      auto value = test_t{1};
      return value += test_t{1};
    };
    STATIC_REQUIRE(test() == test_t{2});

    auto value = test_t{1};
    STATIC_REQUIRE(noexcept(value += test_t{1}));
  }

  SECTION("named_type -= named_type") {
    auto test = []() constexpr {
      auto value = test_t{1};
      return value -= test_t{1};
    };
    STATIC_REQUIRE(test() == test_t{0});

    auto value = test_t{1};
    STATIC_REQUIRE(noexcept(value -= test_t{1}));
  }
}

TEST_CASE("named_types are multiplied and divided with assignment", "[named_type]") {
  using test_t = gw::named_type<int, "TestType">;

  SECTION("named_type *= named_type") {
    auto test = []() constexpr {
      auto value = test_t{2};
      return value *= test_t{2};
    };
    STATIC_REQUIRE(test() == test_t{4});

    auto value = test_t{2};
    STATIC_REQUIRE(noexcept(value *= test_t{2}));
  }

  SECTION("named_type /= named_type") {
    auto test = []() constexpr {
      auto value = test_t{4};
      return value /= test_t{2};
    };
    STATIC_REQUIRE(test() == test_t{2});

    auto value = test_t{4};
    STATIC_REQUIRE(noexcept(value /= test_t{2}));
  }
}

TEST_CASE("named_types are bitwise operated", "[named_type]") {
  using test_t = gw::named_type<unsigned int, "TestType">;

  SECTION("named_type ~") {
    STATIC_REQUIRE(~test_t{1U} == test_t{~1U});
    STATIC_REQUIRE(noexcept(~test_t{}));
  }

  SECTION("named_type & named_type") {
    STATIC_REQUIRE((test_t{1U} & test_t{1U}) == test_t{1U});
    STATIC_REQUIRE(noexcept(test_t{} & test_t{}));
  }

  SECTION("named_type | named_type") {
    STATIC_REQUIRE((test_t{1U} | test_t{1U}) == test_t{1U});
    STATIC_REQUIRE(noexcept(test_t{} | test_t{}));
  }

  SECTION("named_type ^ named_type") {
    STATIC_REQUIRE((test_t{1U} ^ test_t{1U}) == test_t{0U});
    STATIC_REQUIRE(noexcept(test_t{} ^ test_t{}));
  }
}

TEST_CASE("named_types are bitwise shifted", "[named_type]") {
  using test_t = gw::named_type<unsigned int, "TestType">;

  SECTION("named_type << named_type") {
    STATIC_REQUIRE((test_t{1U} << test_t{1U}) == test_t{2U});
    STATIC_REQUIRE(noexcept(test_t{} << test_t{}));
  }

  SECTION("named_type >> named_type") {
    STATIC_REQUIRE((test_t{2U} >> test_t{1U}) == test_t{1U});
    STATIC_REQUIRE(noexcept(test_t{} >> test_t{}));
  }
}

TEST_CASE("named_types are bitwise operated with assignment", "[named_type]") {
  using test_t = gw::named_type<unsigned int, "TestType">;

  SECTION("named_type &= named_type") {
    auto test = []() constexpr {
      auto value = test_t{1U};
      return value &= test_t{1U};
    };
    STATIC_REQUIRE(test() == test_t{1U});

    auto value = test_t{1U};
    STATIC_REQUIRE(noexcept(value &= test_t{1U}));
  }

  SECTION("named_type |= named_type") {
    auto test = []() constexpr {
      auto value = test_t{1U};
      return value |= test_t{1U};
    };
    STATIC_REQUIRE(test() == test_t{1U});

    auto value = test_t{1U};
    STATIC_REQUIRE(noexcept(value |= test_t{1U}));
  }

  SECTION("named_type ^= named_type") {
    auto test = []() constexpr {
      auto value = test_t{1U};
      return value ^= test_t{1U};
    };
    STATIC_REQUIRE(test() == test_t{0U});

    auto value = test_t{1U};
    STATIC_REQUIRE(noexcept(value ^= test_t{1U}));
  }
}

TEST_CASE("named_types are bitwise shifted with assignment", "[named_type]") {
  using test_t = gw::named_type<unsigned int, "TestType">;

  SECTION("named_type <<= named_type") {
    auto test = []() constexpr {
      auto value = test_t{1U};
      return value <<= test_t{1U};
    };
    STATIC_REQUIRE(test() == test_t{2U});

    auto value = test_t{1U};
    STATIC_REQUIRE(noexcept(value <<= test_t{1U}));
  }

  SECTION("named_type >>= named_type") {
    auto test = []() constexpr {
      auto value = test_t{2U};
      return value >>= test_t{1U};
    };
    STATIC_REQUIRE(test() == test_t{1U});

    auto value = test_t{2U};
    STATIC_REQUIRE(noexcept(value >>= test_t{1U}));
  }
}

TEST_CASE("named_types are viewable ranges", "[named_type]") {
  constexpr auto k_value = gw::make_named_type<"TestType">(std::array{1, 2, 3, 4, 5});

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

TEST_CASE("named_types are hashed", "[named_type]") {
  using test_t = gw::named_type<int, "TestType">;

  STATIC_REQUIRE(gw::hashable<test_t>);
}

TEST_CASE("named_types are streamed", "[named_type]") {
  using test_t = gw::named_type<int, "TestType">;

  SECTION("ostream named_type") {
    STATIC_REQUIRE(gw::ostreamable<test_t>);

    auto sstream = std::stringstream{};
    sstream << test_t{1};
    REQUIRE(sstream.str() == "1");
  }

  SECTION("istream named_type") {
    STATIC_REQUIRE(gw::istreamable<test_t>);

    auto sstream = std::stringstream{};
    sstream << "1";
    auto value = test_t{};
    sstream >> value;
    REQUIRE(value == test_t{1});
  }
}

TEST_CASE("named_types are formatted", "[named_type]") {
  using test_t = gw::named_type<int, "TestType">;

  STATIC_REQUIRE(std::formattable<test_t, char>);
  STATIC_REQUIRE(std::formattable<test_t, wchar_t>);

  SECTION("value only") {  //
    REQUIRE(std::format("{}", test_t{1}) == "1");
  }

  SECTION("value with name") {  //
    REQUIRE(std::format("{:#}", test_t{1}) == "TestType: 1");
  }
}
