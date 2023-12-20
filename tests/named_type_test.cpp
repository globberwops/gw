// Copyright (c) 2023 Martin Stump
// SPDX-License-Identifier: BSL-1.0

#include "gw/named_type.hpp"

#include <array>
#include <catch2/catch_test_macros.hpp>
#include <concepts>
#include <ranges>
#include <sstream>
#include <type_traits>

#include "gw/concepts.hpp"

TEST_CASE("named_types are constructed", "[named_type]") {
  using named_type_test = gw::named_type<int, "NamedTypeTest">;

  SECTION("default constructed") { STATIC_REQUIRE(std::is_nothrow_default_constructible_v<named_type_test>); }

  SECTION("constructed from int") { STATIC_REQUIRE(std::is_nothrow_constructible_v<named_type_test, int>); }

  SECTION("constructed from int&&") { STATIC_REQUIRE(std::is_nothrow_constructible_v<named_type_test, int&&>); }

  SECTION("constructed from const int&") {
    STATIC_REQUIRE(std::is_nothrow_constructible_v<named_type_test, const int&>);
  }

  SECTION("copy constructed") { STATIC_REQUIRE(std::is_nothrow_copy_constructible_v<named_type_test>); }

  SECTION("move constructed") { STATIC_REQUIRE(std::is_nothrow_move_constructible_v<named_type_test>); }

  SECTION("make_named_type") {
    STATIC_REQUIRE(gw::make_named_type<"NamedTypeTest">(1) == named_type_test{1});
    STATIC_REQUIRE(noexcept(gw::make_named_type<"NamedTypeTest">(1)));
  }
}

TEST_CASE("named_types are assigned", "[named_type]") {
  using named_type_test = gw::named_type<int, "NamedTypeTest">;

  SECTION("copy assigned") { STATIC_REQUIRE(std::is_nothrow_copy_assignable_v<named_type_test>); }

  SECTION("move assigned") { STATIC_REQUIRE(std::is_nothrow_move_assignable_v<named_type_test>); }
}

TEST_CASE("named_types are destroyed", "[named_type]") {
  using named_type_test = gw::named_type<int, "NamedTypeTest">;

  STATIC_REQUIRE(std::is_nothrow_destructible_v<named_type_test>);
}

TEST_CASE("named_types are distinguished by their name", "[named_type]") {
  using named_type_test1 = gw::named_type<int, "NamedTypeTest1">;
  using named_type_test2 = gw::named_type<int, "NamedTypeTest2">;

  STATIC_REQUIRE(std::is_same_v<named_type_test1, named_type_test1>);
  STATIC_REQUIRE(!std::is_same_v<named_type_test1, named_type_test2>);
}

TEST_CASE("named_types are swapped", "[named_type]") {
  using named_type_test = gw::named_type<int, "NamedTypeTest">;

  STATIC_REQUIRE(std::is_nothrow_swappable_v<named_type_test>);
}

TEST_CASE("named_types are observed", "[named_type]") {
  using named_type_test = gw::named_type<int, "NamedTypeTest">;

  SECTION("operator*") {
    STATIC_REQUIRE(named_type_test{}.operator*() == 0);
    STATIC_REQUIRE(noexcept(named_type_test{}.operator*()));
  }

  SECTION("operator->") {
    STATIC_REQUIRE(*named_type_test{}.operator->() == 0);
    STATIC_REQUIRE(noexcept(named_type_test{}.operator->()));
  }

  SECTION("value") {
    STATIC_REQUIRE(named_type_test{}.value() == 0);
    STATIC_REQUIRE(noexcept(named_type_test{}.value()));
  }
}

TEST_CASE("named_types are transformed", "[named_type]") {
  using named_type_test = gw::named_type<int, "NamedTypeTest">;

  auto test = []() constexpr {
    auto value = named_type_test{1};
    return value.transform([](auto value) { return value + 1; });
  };
  STATIC_REQUIRE(test() == named_type_test{2});

  STATIC_REQUIRE(noexcept(named_type_test{}.transform([](auto value) noexcept { return value + 1; })));
}

TEST_CASE("named_types are reset", "[named_type]") {
  using named_type_test = gw::named_type<int, "NamedTypeTest">;

  auto test = []() constexpr {
    auto value = named_type_test{1};
    value.reset();
    return value;
  };
  STATIC_REQUIRE(test() == named_type_test{0});

  STATIC_REQUIRE(noexcept(named_type_test{}.reset()));
}

TEST_CASE("named_types are emplaced", "[named_type]") {
  using named_type_test = gw::named_type<int, "NamedTypeTest">;

  STATIC_REQUIRE(named_type_test{}.emplace(1) == 1);
  STATIC_REQUIRE(noexcept(named_type_test{}.emplace(1)));
}

TEST_CASE("named_types are totally ordered", "[named_type]") {
  using named_type_test = gw::named_type<int, "NamedTypeTest">;

  STATIC_REQUIRE(std::totally_ordered<named_type_test>);
}

TEST_CASE("named_types are converted", "[named_type]") {
  using named_type_test = gw::named_type<int, "NamedTypeTest">;

  SECTION("explicit conversion") {
    STATIC_REQUIRE(static_cast<int>(named_type_test{}) == 0);
    STATIC_REQUIRE(noexcept(static_cast<int>(named_type_test{})));
  }
}

TEST_CASE("named_types are incremented and decremented", "[named_type]") {
  using named_type_test = gw::named_type<int, "NamedTypeTest">;

  SECTION("pre-increment") {
    STATIC_REQUIRE(++named_type_test{} == named_type_test{1});
    STATIC_REQUIRE(noexcept(++named_type_test{}));
  }

  SECTION("post-increment") {
    auto test = []() constexpr {
      auto value = named_type_test{1};
      return value++;
    };
    STATIC_REQUIRE(test() == named_type_test{1});

    auto value = named_type_test{};
    STATIC_REQUIRE(noexcept(value++));
  }

  SECTION("pre-decrement") {
    STATIC_REQUIRE(--named_type_test{} == named_type_test{-1});
    STATIC_REQUIRE(noexcept(--named_type_test{}));
  }

  SECTION("post-decrement") {
    auto test = []() constexpr {
      auto value = named_type_test{1};
      return value--;
    };
    STATIC_REQUIRE(test() == named_type_test{1});

    auto value = named_type_test{1};
    STATIC_REQUIRE(noexcept(value--));
  }
}

TEST_CASE("named_types are added and subtracted", "[named_type]") {
  using named_type_test = gw::named_type<int, "NamedTypeTest">;

  SECTION("named_type + named_type") {
    STATIC_REQUIRE(named_type_test{1} + named_type_test{1} == named_type_test{2});
    STATIC_REQUIRE(noexcept(named_type_test{} + named_type_test{}));
  }

  SECTION("named_type - named_type") {
    STATIC_REQUIRE(named_type_test{1} - named_type_test{1} == named_type_test{0});
    STATIC_REQUIRE(noexcept(named_type_test{} - named_type_test{}));
  }
}

TEST_CASE("named_types are multiplied and divided", "[named_type]") {
  using named_type_test = gw::named_type<int, "NamedTypeTest">;

  SECTION("named_type * named_type") {
    STATIC_REQUIRE(named_type_test{2} * named_type_test{2} == named_type_test{4});
    STATIC_REQUIRE(noexcept(named_type_test{} * named_type_test{}));
  }

  SECTION("named_type / named_type") {
    STATIC_REQUIRE(named_type_test{4} / named_type_test{2} == named_type_test{2});
    STATIC_REQUIRE(noexcept(named_type_test{} / named_type_test{}));
  }
}

TEST_CASE("named_types are modulo reduced", "[named_type]") {
  using named_type_test = gw::named_type<int, "NamedTypeTest">;

  STATIC_REQUIRE(named_type_test{4} % named_type_test{3} == named_type_test{1});
  STATIC_REQUIRE(noexcept(named_type_test{} % named_type_test{}));
}

TEST_CASE("named_types are added and subtracted with assignment", "[named_type]") {
  using named_type_test = gw::named_type<int, "NamedTypeTest">;

  SECTION("named_type += named_type") {
    auto test = []() constexpr {
      auto value = named_type_test{1};
      return value += named_type_test{1};
    };
    STATIC_REQUIRE(test() == named_type_test{2});

    auto value = named_type_test{1};
    STATIC_REQUIRE(noexcept(value += named_type_test{1}));
  }

  SECTION("named_type -= named_type") {
    auto test = []() constexpr {
      auto value = named_type_test{1};
      return value -= named_type_test{1};
    };
    STATIC_REQUIRE(test() == named_type_test{0});

    auto value = named_type_test{1};
    STATIC_REQUIRE(noexcept(value -= named_type_test{1}));
  }
}

TEST_CASE("named_types are multiplied and divided with assignment", "[named_type]") {
  using named_type_test = gw::named_type<int, "NamedTypeTest">;

  SECTION("named_type *= named_type") {
    auto test = []() constexpr {
      auto value = named_type_test{2};
      return value *= named_type_test{2};
    };
    STATIC_REQUIRE(test() == named_type_test{4});

    auto value = named_type_test{2};
    STATIC_REQUIRE(noexcept(value *= named_type_test{2}));
  }

  SECTION("named_type /= named_type") {
    auto test = []() constexpr {
      auto value = named_type_test{4};
      return value /= named_type_test{2};
    };
    STATIC_REQUIRE(test() == named_type_test{2});

    auto value = named_type_test{4};
    STATIC_REQUIRE(noexcept(value /= named_type_test{2}));
  }
}

TEST_CASE("named_types are bitwise operated", "[named_type]") {
  using named_type_test = gw::named_type<unsigned int, "NamedTypeTest">;

  SECTION("named_type ~") {
    STATIC_REQUIRE(~named_type_test{1U} == named_type_test{~1U});
    STATIC_REQUIRE(noexcept(~named_type_test{}));
  }

  SECTION("named_type & named_type") {
    STATIC_REQUIRE((named_type_test{1U} & named_type_test{1U}) == named_type_test{1U});
    STATIC_REQUIRE(noexcept(named_type_test{} & named_type_test{}));
  }

  SECTION("named_type | named_type") {
    STATIC_REQUIRE((named_type_test{1U} | named_type_test{1U}) == named_type_test{1U});
    STATIC_REQUIRE(noexcept(named_type_test{} | named_type_test{}));
  }

  SECTION("named_type ^ named_type") {
    STATIC_REQUIRE((named_type_test{1U} ^ named_type_test{1U}) == named_type_test{0U});
    STATIC_REQUIRE(noexcept(named_type_test{} ^ named_type_test{}));
  }
}

TEST_CASE("named_types are bitwise shifted", "[named_type]") {
  using named_type_test = gw::named_type<unsigned int, "NamedTypeTest">;

  SECTION("named_type << named_type") {
    STATIC_REQUIRE((named_type_test{1U} << named_type_test{1U}) == named_type_test{2U});
    STATIC_REQUIRE(noexcept(named_type_test{} << named_type_test{}));
  }

  SECTION("named_type >> named_type") {
    STATIC_REQUIRE((named_type_test{2U} >> named_type_test{1U}) == named_type_test{1U});
    STATIC_REQUIRE(noexcept(named_type_test{} >> named_type_test{}));
  }
}

TEST_CASE("named_types are bitwise operated with assignment", "[named_type]") {
  using named_type_test = gw::named_type<unsigned int, "NamedTypeTest">;

  SECTION("named_type &= named_type") {
    auto test = []() constexpr {
      auto value = named_type_test{1U};
      return value &= named_type_test{1U};
    };
    STATIC_REQUIRE(test() == named_type_test{1U});

    auto value = named_type_test{1U};
    STATIC_REQUIRE(noexcept(value &= named_type_test{1U}));
  }

  SECTION("named_type |= named_type") {
    auto test = []() constexpr {
      auto value = named_type_test{1U};
      return value |= named_type_test{1U};
    };
    STATIC_REQUIRE(test() == named_type_test{1U});

    auto value = named_type_test{1U};
    STATIC_REQUIRE(noexcept(value |= named_type_test{1U}));
  }

  SECTION("named_type ^= named_type") {
    auto test = []() constexpr {
      auto value = named_type_test{1U};
      return value ^= named_type_test{1U};
    };
    STATIC_REQUIRE(test() == named_type_test{0U});

    auto value = named_type_test{1U};
    STATIC_REQUIRE(noexcept(value ^= named_type_test{1U}));
  }
}

TEST_CASE("named_types are bitwise shifted with assignment", "[named_type]") {
  using named_type_test = gw::named_type<unsigned int, "NamedTypeTest">;

  SECTION("named_type <<= named_type") {
    auto test = []() constexpr {
      auto value = named_type_test{1U};
      return value <<= named_type_test{1U};
    };
    STATIC_REQUIRE(test() == named_type_test{2U});

    auto value = named_type_test{1U};
    STATIC_REQUIRE(noexcept(value <<= named_type_test{1U}));
  }

  SECTION("named_type >>= named_type") {
    auto test = []() constexpr {
      auto value = named_type_test{2U};
      return value >>= named_type_test{1U};
    };
    STATIC_REQUIRE(test() == named_type_test{1U});

    auto value = named_type_test{2U};
    STATIC_REQUIRE(noexcept(value >>= named_type_test{1U}));
  }
}

TEST_CASE("named_types are viewable ranges", "[named_type]") {
  constexpr auto k_value = gw::make_named_type<"NamedTypeTest">(std::array{1, 2, 3, 4, 5});

  STATIC_REQUIRE(!k_value.empty());
  STATIC_REQUIRE(k_value.size() == 5);
  STATIC_REQUIRE(k_value[0] == 1);
  STATIC_REQUIRE(k_value[1] == 2);
  STATIC_REQUIRE(k_value[2] == 3);
  STATIC_REQUIRE(k_value[3] == 4);
  STATIC_REQUIRE(k_value[4] == 5);

  using named_type_test = std::remove_cv_t<decltype(k_value)>;
  STATIC_REQUIRE(std::ranges::viewable_range<named_type_test>);
}

TEST_CASE("named_types are hashed", "[named_type]") {
  using named_type_test = gw::named_type<int, "NamedTypeTest">;

  STATIC_REQUIRE(gw::hashable<named_type_test>);
}

TEST_CASE("named_types are streamed", "[named_type]") {
  using named_type_test = gw::named_type<int, "NamedTypeTest">;

  SECTION("ostream named_type") {
    STATIC_REQUIRE(gw::ostreamable<named_type_test>);

    auto sstream = std::stringstream{};
    sstream << named_type_test{1};
    REQUIRE(sstream.str() == "1");
  }

  SECTION("istream named_type") {
    STATIC_REQUIRE(gw::istreamable<named_type_test>);

    auto sstream = std::stringstream{};
    sstream << "1";
    auto value = named_type_test{};
    sstream >> value;
    REQUIRE(value == named_type_test{1});
  }
}

TEST_CASE("named_types are converted to string", "[named_type]") {
  SECTION("unnamed named_type") {
    using named_type_test = gw::named_type<int, "NamedTypeTest">;

    REQUIRE(std::to_string(named_type_test{1}) == "NamedTypeTest: 1");
  }
}
