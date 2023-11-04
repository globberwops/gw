#include "gw/strong_type.hpp"

#include <iostream>
#include <ostream>
#include <type_traits>
#include <vector>

struct strong_type_test_tag {};
using strong_type_test = gw::strong_type<strong_type_test_tag, int>;

static_assert(std::is_nothrow_copy_assignable_v<strong_type_test>);
static_assert(std::is_nothrow_move_assignable_v<strong_type_test>);
static_assert(std::is_nothrow_destructible_v<strong_type_test>);
static_assert(std::is_nothrow_swappable_v<strong_type_test>);

// Constructors
static_assert(std::is_nothrow_default_constructible_v<strong_type_test>);
static_assert(std::is_nothrow_copy_constructible_v<strong_type_test>);
static_assert(std::is_nothrow_move_constructible_v<strong_type_test>);
static_assert(std::is_nothrow_constructible_v<strong_type_test, int>);
static_assert(std::is_nothrow_constructible_v<strong_type_test, int&&>);
static_assert(std::is_nothrow_constructible_v<strong_type_test, const int&>);

// Observers
static_assert(strong_type_test{}.operator*() == 0);
static_assert(*strong_type_test{}.operator->() == 0);
static_assert(strong_type_test{}.value() == 0);
static_assert(noexcept(strong_type_test{}.operator*()));
static_assert(noexcept(*strong_type_test{}.operator->()));
static_assert(noexcept(strong_type_test{}.value()));

// Monadic operations
static_assert(*strong_type_test{}.transform([](auto value) noexcept { return value + 1; }) == 1);
static_assert(noexcept(strong_type_test{}.transform([](auto value) noexcept { return value + 1; })));
static_assert(!noexcept(strong_type_test{}.transform([](auto value) { return value + 1; })));

// Modifiers
consteval auto strong_type_swap_test() -> bool {
  auto strong_type1 = strong_type_test{1};
  auto strong_type2 = strong_type_test{2};
  strong_type1.swap(strong_type2);
  return *strong_type1 == 2 && *strong_type2 == 1;
}
static_assert(strong_type_swap_test());

consteval auto strong_type_swap_noexcept_test() -> bool {
  auto strong_type1 = strong_type_test{1};
  auto strong_type2 = strong_type_test{2};

  return noexcept(strong_type1.swap(strong_type2));
}
static_assert(strong_type_swap_noexcept_test());

consteval auto strong_type_reset_test() -> bool {
  auto strong_type = strong_type_test{1};
  strong_type.reset();
  return *strong_type == 0;
}
static_assert(strong_type_reset_test());
static_assert(noexcept(strong_type_test{}.reset()));

static_assert(strong_type_test{}.emplace(1) == 1);
static_assert(noexcept(strong_type_test{}.emplace()));

// Comparison operators
static_assert(strong_type_test{} == strong_type_test{});
static_assert(strong_type_test{} != strong_type_test{1});
static_assert(strong_type_test{} <= strong_type_test{1});
static_assert(strong_type_test{} < strong_type_test{1});
static_assert(strong_type_test{1} >= strong_type_test{});
static_assert(strong_type_test{1} > strong_type_test{});

static_assert(noexcept(strong_type_test{} == strong_type_test{}));
static_assert(noexcept(strong_type_test{} != strong_type_test{1}));
static_assert(noexcept(strong_type_test{} <= strong_type_test{1}));
static_assert(noexcept(strong_type_test{} < strong_type_test{1}));
static_assert(noexcept(strong_type_test{1} >= strong_type_test{}));
static_assert(noexcept(strong_type_test{1} > strong_type_test{}));

// Creation functions
struct make_strong_type_tag {};
static_assert(*gw::make_strong_type<make_strong_type_tag>(1) == 1);
static_assert(noexcept(gw::make_strong_type<make_strong_type_tag>(1)));
static_assert(gw::make_strong_type<make_strong_type_tag, std::vector<int>>({1, 2, 3}) ==
              gw::make_strong_type<make_strong_type_tag, std::vector<int>>({1, 2, 3}));
static_assert(gw::make_strong_type<make_strong_type_tag, int>(1) == gw::make_strong_type<make_strong_type_tag, int>(1));

// Hash
struct strong_type_hashable1_tag {};
using strong_type_hashable1 = gw::strong_type<strong_type_hashable1_tag, int>;
struct strong_type_hashable2_tag {};
using strong_type_hashable2 = gw::strong_type<strong_type_hashable2_tag, int>;

// Verbose
struct verbose_type {
  verbose_type() noexcept { std::cout << "verbose_type::verbose_type()\n"; }
  explicit verbose_type(int value) noexcept : m_value{value} { std::cout << "verbose_type::verbose_type(int)\n"; }
  verbose_type(const verbose_type&) noexcept { std::cout << "verbose_type::verbose_type(const verbose_type&)\n"; }
  verbose_type(verbose_type&&) noexcept { std::cout << "verbose_type::verbose_type(verbose_type&&)\n"; }
  auto operator=(const verbose_type& /*other*/) noexcept -> verbose_type& {
    std::cout << "verbose_type::operator=(const verbose_type&)\n";
    return *this;
  }
  auto operator=(verbose_type&& /*other*/) noexcept -> verbose_type& {
    std::cout << "verbose_type::operator=(verbose_type&&)\n";
    return *this;
  }
  ~verbose_type() noexcept { std::cout << "verbose_type::~verbose_type()\n"; }

 private:
  int m_value{};
};

struct verbose_type_tag {};
using strong_type_verbose = gw::strong_type<verbose_type_tag, verbose_type>;

void strong_type_verbose_test() {
  std::cout << "--------------------------------------------------------------------------------\n";
  { auto strong_type = strong_type_verbose{1}; }
  std::cout << "--------------------------------------------------------------------------------\n";
  {
    auto strong_type1 = strong_type_verbose{1};
    auto strong_type2 = strong_type_verbose{2};
    strong_type1 = strong_type2;
  }
  std::cout << "--------------------------------------------------------------------------------\n";
  {
    auto strong_type1 = strong_type_verbose{1};
    auto strong_type2 = strong_type_verbose{2};
    strong_type1 = std::move(strong_type2);
  }
  std::cout << "--------------------------------------------------------------------------------\n";
  { auto strong_type = gw::make_strong_type<verbose_type_tag>(verbose_type{}); }
  std::cout << "--------------------------------------------------------------------------------\n";
  {
    auto strong_type1 = strong_type_verbose{};
    auto strong_type2 = gw::make_strong_type<verbose_type_tag>(strong_type1);
  }
  std::cout << "--------------------------------------------------------------------------------\n";
  { auto strong_type = gw::make_strong_type<verbose_type_tag, verbose_type>(); }
  std::cout << "--------------------------------------------------------------------------------\n";
  { auto strong_type = gw::make_strong_type<verbose_type_tag, verbose_type>(1); }
}

auto strong_type_hash_test() -> bool {
  auto strong_type1 = strong_type_hashable1{};
  auto strong_type2 = strong_type_hashable1{};
  auto strong_type3 = strong_type_hashable1{1};
  auto strong_type4 = strong_type_hashable2{};

  auto strong_type1_hash = std::hash<strong_type_hashable1>{}(strong_type1);
  auto strong_type2_hash = std::hash<strong_type_hashable1>{}(strong_type2);
  auto strong_type3_hash = std::hash<strong_type_hashable1>{}(strong_type3);
  auto strong_type4_hash = std::hash<strong_type_hashable2>{}(strong_type4);

  auto same_value_same_tag = strong_type1_hash == strong_type2_hash;
  auto different_value_same_tag = strong_type1_hash != strong_type3_hash;
  auto same_value_different_tag = strong_type3_hash != strong_type4_hash;
  auto different_value_different_tag = strong_type3_hash != strong_type4_hash;

  return same_value_same_tag && different_value_same_tag && same_value_different_tag && different_value_different_tag &&
         noexcept(std::hash<strong_type_hashable1>{}(strong_type1));
}

auto main() -> int {
  strong_type_verbose_test();
  return strong_type_hash_test() ? 0 : 1;
}
