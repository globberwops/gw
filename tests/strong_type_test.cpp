#include "gw/strong_type.hpp"

#include <type_traits>

using StrongTypeTest = gw::StrongType<int, struct StrongTypeTestTag>;

static_assert(std::is_nothrow_default_constructible_v<StrongTypeTest>);
static_assert(std::is_nothrow_copy_constructible_v<StrongTypeTest>);
static_assert(std::is_nothrow_move_constructible_v<StrongTypeTest>);
static_assert(std::is_nothrow_copy_assignable_v<StrongTypeTest>);
static_assert(std::is_nothrow_move_assignable_v<StrongTypeTest>);
static_assert(std::is_nothrow_destructible_v<StrongTypeTest>);
static_assert(std::is_nothrow_swappable_v<StrongTypeTest>);

static_assert(std::is_nothrow_constructible_v<StrongTypeTest, int>);
static_assert(std::is_nothrow_constructible_v<StrongTypeTest, int&>);
static_assert(std::is_nothrow_constructible_v<StrongTypeTest, int&&>);
static_assert(std::is_nothrow_constructible_v<StrongTypeTest, const int>);
static_assert(std::is_nothrow_constructible_v<StrongTypeTest, const int&>);
static_assert(std::is_nothrow_constructible_v<StrongTypeTest, const int&&>);

static_assert(StrongTypeTest{}.operator*() == 0);
static_assert(*StrongTypeTest{}.operator->() == 0);
static_assert(StrongTypeTest{}.Value() == 0);

consteval auto StrongTypeSwapTest() {
  auto strong_type1 = StrongTypeTest{1};
  auto strong_type2 = StrongTypeTest{2};
  strong_type1.Swap(strong_type2);
  return *strong_type1 == 2 && *strong_type2 == 1;
}
static_assert(StrongTypeSwapTest());

consteval auto StrongTypeResetTest() {
  auto strong_type = StrongTypeTest{1};
  strong_type.Reset();
  return *strong_type == 0;
}
static_assert(StrongTypeResetTest());

static_assert(StrongTypeTest{}.Emplace(1) == 1);

static_assert(StrongTypeTest{} == StrongTypeTest{});
static_assert(StrongTypeTest{} != StrongTypeTest{1});
static_assert(StrongTypeTest{} <= StrongTypeTest{1});
static_assert(StrongTypeTest{} < StrongTypeTest{1});
static_assert(StrongTypeTest{1} >= StrongTypeTest{});
static_assert(StrongTypeTest{1} > StrongTypeTest{});

struct StrongTypeHashable1Tag {};
using StrongTypeHashable1 = gw::StrongType<int, StrongTypeHashable1Tag>;
struct StrongTypeHashable2Tag {};
using StrongTypeHashable2 = gw::StrongType<int, StrongTypeHashable2Tag>;

auto StrongTypeHashTest() -> bool {
  auto strong_type1 = StrongTypeHashable1{};
  auto strong_type2 = StrongTypeHashable1{};
  auto strong_type3 = StrongTypeHashable1{1};
  auto strong_type4 = StrongTypeHashable2{};

  auto strong_type1_hash = std::hash<StrongTypeHashable1>{}(strong_type1);
  auto strong_type2_hash = std::hash<StrongTypeHashable1>{}(strong_type2);
  auto strong_type3_hash = std::hash<StrongTypeHashable1>{}(strong_type3);
  auto strong_type4_hash = std::hash<StrongTypeHashable2>{}(strong_type4);

  auto same_value_same_tag = strong_type1_hash == strong_type2_hash;
  auto different_value_same_tag = strong_type1_hash != strong_type3_hash;
  auto same_value_different_tag = strong_type3_hash != strong_type4_hash;
  auto different_value_different_tag = strong_type3_hash != strong_type4_hash;

  return same_value_same_tag && different_value_same_tag && same_value_different_tag && different_value_different_tag;
}

auto main() -> int { return StrongTypeHashTest() ? 0 : 1; }
