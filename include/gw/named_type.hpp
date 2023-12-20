// Copyright (c) 2023 Martin Stump
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <ranges>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include "gw/concepts.hpp"

namespace gw {

namespace detail {

template <std::size_t N>
struct fixed_string {
  // NOLINTNEXTLINE
  constexpr fixed_string(const char (&str)[N]) { std::copy_n(str, N, value); }

  // NOLINTNEXTLINE
  char value[N]{};
};

struct named_type_empty_base {
  constexpr auto operator<=>(const named_type_empty_base&) const noexcept = default;
};

}  // namespace detail

template <typename T, detail::fixed_string Name>
class named_type final
    : public std::conditional_t<std::ranges::range<T>, std::ranges::view_interface<named_type<T, Name>>,
                                detail::named_type_empty_base> {
 public:
  //
  // Public types
  //
  using value_type = T;

  //
  // Constructors
  //
  template <class... Args>
  constexpr explicit named_type(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
    requires std::constructible_from<T, Args...>
      : m_value(T{std::forward<Args>(args)...}) {}

  template <class U, class... Args>
  constexpr named_type(std::initializer_list<U> ilist,
                       Args&&... args) noexcept(std::is_nothrow_constructible_v<T, std::initializer_list<U>&, Args...>)
    requires std::constructible_from<T, std::initializer_list<U>&, Args...>
      : m_value(T{ilist, std::forward<Args>(args)...}) {}

  //
  // Destructor
  //
  ~named_type() noexcept(std::is_nothrow_destructible_v<T>) = default;

  //
  // Static functions
  //
  static constexpr auto name() noexcept -> std::string_view { return k_name; }

  //
  // Observers
  //
  constexpr auto operator->() const noexcept -> const T* { return &m_value; }
  constexpr auto operator->() noexcept -> T* { return &m_value; }

  constexpr auto operator*() const& noexcept -> const T& { return m_value; }
  constexpr auto operator*() & noexcept -> T& { return m_value; }
  constexpr auto operator*() const&& noexcept -> const T&& { return std::move(m_value); }
  constexpr auto operator*() && noexcept -> T&& { return std::move(m_value); }

  constexpr auto value() const& noexcept -> const T& { return m_value; }
  constexpr auto value() & noexcept -> T& { return m_value; }
  constexpr auto value() const&& noexcept -> const T&& { return std::move(m_value); }
  constexpr auto value() && noexcept -> T&& { return std::move(m_value); }

  //
  // Monadic operations
  //
  template <class F>
  constexpr auto transform(F&& func) const& noexcept(noexcept(func(m_value)))
    requires std::invocable<F, const T&>
  {
    return named_type<std::remove_cv_t<std::invoke_result_t<F, const T&>>, Name>{func(m_value)};
  }

  template <class F>
  constexpr auto transform(F&& func) & noexcept(noexcept(func(m_value)))
    requires std::invocable<F, T&>
  {
    return named_type<std::remove_cv_t<std::invoke_result_t<F, T&>>, Name>{func(m_value)};
  }

  template <class F>
  constexpr auto transform(F&& func) const&& noexcept(noexcept(func(m_value)))
    requires std::invocable<F, const T&&>
  {
    return named_type<std::remove_cv_t<std::invoke_result_t<F, const T&&>>, Name>{func(std::move(m_value))};
  }

  template <class F>
  constexpr auto transform(F&& func) && noexcept(noexcept(func(m_value)))
    requires std::invocable<F, T&&>
  {
    return named_type<std::remove_cv_t<std::invoke_result_t<F, T&&>>, Name>{func(std::move(m_value))};
  }

  //
  // Modifiers
  //
  constexpr void swap(named_type& rhs) noexcept(std::is_nothrow_swappable_v<T>)
    requires std::swappable<T>
  {
    using std::swap;
    swap(m_value, rhs.m_value);
  }

  constexpr void reset() noexcept(std::is_nothrow_default_constructible_v<T>)
    requires std::default_initializable<T>
  {
    m_value = T{};
  }

  template <class... Args>
  constexpr auto emplace(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>) -> T&
    requires std::constructible_from<T, Args...>
  {
    m_value = T{std::forward<Args>(args)...};
    return m_value;
  }

  //
  // Comparison operators
  //
  constexpr auto operator<=>(const named_type& rhs) const noexcept = default;

  //
  // Conversion operators
  //
  constexpr explicit operator const T&() const& noexcept { return m_value; }
  constexpr explicit operator T&() & noexcept { return m_value; }
  constexpr explicit operator const T&&() const&& noexcept { return std::move(m_value); }
  constexpr explicit operator T&&() && noexcept { return std::move(m_value); }

  //
  // Increment and decrement operators
  //
  constexpr auto operator++() & noexcept(noexcept(++m_value)) -> named_type&
    requires gw::incrementable<T>
  {
    ++m_value;
    return *this;
  }

  constexpr auto operator++() && noexcept(noexcept(++m_value)) -> named_type&&
    requires gw::incrementable<T>
  {
    ++m_value;
    return std::move(*this);
  }

  constexpr auto operator++(int) & noexcept(noexcept(m_value++)) -> named_type
    requires gw::incrementable<T>
  {
    return named_type{m_value++};
  }

  constexpr auto operator++(int) && noexcept(noexcept(m_value++)) -> named_type
    requires gw::incrementable<T>
  {
    return named_type{m_value++};
  }

  constexpr auto operator--() & noexcept(noexcept(--m_value)) -> named_type&
    requires gw::decrementable<T>
  {
    --m_value;
    return *this;
  }

  constexpr auto operator--() && noexcept(noexcept(--m_value)) -> named_type&&
    requires gw::decrementable<T>
  {
    --m_value;
    return std::move(*this);
  }

  constexpr auto operator--(int) & noexcept(noexcept(m_value--)) -> named_type
    requires gw::decrementable<T>
  {
    return named_type{m_value--};
  }

  constexpr auto operator--(int) && noexcept(noexcept(m_value--)) -> named_type
    requires gw::decrementable<T>
  {
    return named_type{m_value--};
  }

  //
  // Arithmetic operators
  //
  constexpr auto operator+(const named_type& rhs) const& noexcept(noexcept(m_value + rhs.m_value)) -> named_type
    requires gw::arithmetic<T>
  {
    return named_type{m_value + rhs.m_value};
  }

  constexpr auto operator+(named_type&& rhs) const& noexcept(noexcept(m_value + rhs.m_value)) -> named_type
    requires gw::arithmetic<T>
  {
    return named_type{m_value + rhs.m_value};
  }

  constexpr auto operator+(const named_type& rhs) && noexcept(noexcept(m_value + rhs.m_value)) -> named_type
    requires gw::arithmetic<T>
  {
    return named_type{m_value + rhs.m_value};
  }

  constexpr auto operator+(named_type&& rhs) && noexcept(noexcept(m_value + rhs.m_value)) -> named_type
    requires gw::arithmetic<T>
  {
    return named_type{m_value + rhs.m_value};
  }

  constexpr auto operator-(const named_type& rhs) const& noexcept(noexcept(m_value - rhs.m_value)) -> named_type
    requires gw::arithmetic<T>
  {
    return named_type{m_value - rhs.m_value};
  }

  constexpr auto operator-(named_type&& rhs) const& noexcept(noexcept(m_value - rhs.m_value)) -> named_type
    requires gw::arithmetic<T>
  {
    return named_type{m_value - rhs.m_value};
  }

  constexpr auto operator-(const named_type& rhs) && noexcept(noexcept(m_value - rhs.m_value)) -> named_type
    requires gw::arithmetic<T>
  {
    return named_type{m_value - rhs.m_value};
  }

  constexpr auto operator-(named_type&& rhs) && noexcept(noexcept(m_value - rhs.m_value)) -> named_type
    requires gw::arithmetic<T>
  {
    return named_type{m_value - rhs.m_value};
  }

  constexpr auto operator*(const named_type& rhs) const& noexcept(noexcept(m_value * rhs.m_value)) -> named_type
    requires gw::arithmetic<T>
  {
    return named_type{m_value * rhs.m_value};
  }

  constexpr auto operator*(named_type&& rhs) const& noexcept(noexcept(m_value * rhs.m_value)) -> named_type
    requires gw::arithmetic<T>
  {
    return named_type{m_value * rhs.m_value};
  }

  constexpr auto operator*(const named_type& rhs) && noexcept(noexcept(m_value * rhs.m_value)) -> named_type
    requires gw::arithmetic<T>
  {
    return named_type{m_value * rhs.m_value};
  }

  constexpr auto operator*(named_type&& rhs) && noexcept(noexcept(m_value * rhs.m_value)) -> named_type
    requires gw::arithmetic<T>
  {
    return named_type{m_value * rhs.m_value};
  }

  constexpr auto operator/(const named_type& rhs) const& noexcept(noexcept(m_value / rhs.m_value)) -> named_type
    requires gw::arithmetic<T>
  {
    return named_type{m_value / rhs.m_value};
  }

  constexpr auto operator/(named_type&& rhs) const& noexcept(noexcept(m_value / rhs.m_value)) -> named_type
    requires gw::arithmetic<T>
  {
    return named_type{m_value / rhs.m_value};
  }

  constexpr auto operator/(const named_type& rhs) && noexcept(noexcept(m_value / rhs.m_value)) -> named_type
    requires gw::arithmetic<T>
  {
    return named_type{m_value / rhs.m_value};
  }

  constexpr auto operator/(named_type&& rhs) && noexcept(noexcept(m_value / rhs.m_value)) -> named_type
    requires gw::arithmetic<T>
  {
    return named_type{m_value / rhs.m_value};
  }

  constexpr auto operator%(const named_type& rhs) const& noexcept(noexcept(m_value % rhs.m_value)) -> named_type
    requires gw::arithmetic<T>
  {
    return named_type{m_value % rhs.m_value};
  }

  constexpr auto operator%(named_type&& rhs) const& noexcept(noexcept(m_value % rhs.m_value)) -> named_type
    requires gw::arithmetic<T>
  {
    return named_type{m_value % rhs.m_value};
  }

  constexpr auto operator%(const named_type& rhs) && noexcept(noexcept(m_value % rhs.m_value)) -> named_type
    requires gw::arithmetic<T>
  {
    return named_type{m_value % rhs.m_value};
  }

  constexpr auto operator%(named_type&& rhs) && noexcept(noexcept(m_value % rhs.m_value)) -> named_type
    requires gw::arithmetic<T>
  {
    return named_type{m_value % rhs.m_value};
  }

  constexpr auto operator+=(const named_type& rhs) & noexcept(noexcept(m_value += rhs.m_value)) -> named_type&
    requires gw::arithmetic<T>
  {
    m_value += rhs.m_value;
    return *this;
  }

  constexpr auto operator+=(named_type&& rhs) & noexcept(noexcept(m_value += rhs.m_value)) -> named_type&
    requires gw::arithmetic<T>
  {
    m_value += rhs.m_value;
    return *this;
  }

  constexpr auto operator-=(const named_type& rhs) & noexcept(noexcept(m_value -= rhs.m_value)) -> named_type&
    requires gw::arithmetic<T>
  {
    m_value -= rhs.m_value;
    return *this;
  }

  constexpr auto operator-=(named_type&& rhs) & noexcept(noexcept(m_value -= rhs.m_value)) -> named_type&
    requires gw::arithmetic<T>
  {
    m_value -= rhs.m_value;
    return *this;
  }

  constexpr auto operator*=(const named_type& rhs) & noexcept(noexcept(m_value *= rhs.m_value)) -> named_type&
    requires gw::arithmetic<T>
  {
    m_value *= rhs.m_value;
    return *this;
  }

  constexpr auto operator*=(named_type&& rhs) & noexcept(noexcept(m_value *= rhs.m_value)) -> named_type&
    requires gw::arithmetic<T>
  {
    m_value *= rhs.m_value;
    return *this;
  }

  constexpr auto operator/=(const named_type& rhs) & noexcept(noexcept(m_value /= rhs.m_value)) -> named_type&
    requires gw::arithmetic<T>
  {
    m_value /= rhs.m_value;
    return *this;
  }

  constexpr auto operator/=(named_type&& rhs) & noexcept(noexcept(m_value /= rhs.m_value)) -> named_type&
    requires gw::arithmetic<T>
  {
    m_value /= rhs.m_value;
    return *this;
  }

  constexpr auto operator%=(const named_type& rhs) & noexcept(noexcept(m_value %= rhs.m_value)) -> named_type&
    requires gw::arithmetic<T>
  {
    m_value %= rhs.m_value;
    return *this;
  }

  constexpr auto operator%=(named_type&& rhs) & noexcept(noexcept(m_value %= rhs.m_value)) -> named_type&
    requires gw::arithmetic<T>
  {
    m_value %= rhs.m_value;
    return *this;
  }

  //
  // Bitwise operators
  //
  constexpr auto operator~() const& noexcept(noexcept(~m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{~m_value};
  }

  constexpr auto operator&(const named_type& rhs) const& noexcept(noexcept(m_value & rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value & rhs.m_value};
  }

  constexpr auto operator&(named_type&& rhs) const& noexcept(noexcept(m_value & rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value & rhs.m_value};
  }

  constexpr auto operator&(const named_type& rhs) && noexcept(noexcept(m_value & rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value & rhs.m_value};
  }

  constexpr auto operator&(named_type&& rhs) && noexcept(noexcept(m_value & rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value & rhs.m_value};
  }

  constexpr auto operator|(const named_type& rhs) const& noexcept(noexcept(m_value | rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value | rhs.m_value};
  }

  constexpr auto operator|(named_type&& rhs) const& noexcept(noexcept(m_value | rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value | rhs.m_value};
  }

  constexpr auto operator|(const named_type& rhs) && noexcept(noexcept(m_value | rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value | rhs.m_value};
  }

  constexpr auto operator|(named_type&& rhs) && noexcept(noexcept(m_value | rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value | rhs.m_value};
  }

  constexpr auto operator^(const named_type& rhs) const& noexcept(noexcept(m_value ^ rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value ^ rhs.m_value};
  }

  constexpr auto operator^(named_type&& rhs) const& noexcept(noexcept(m_value ^ rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value ^ rhs.m_value};
  }

  constexpr auto operator^(const named_type& rhs) && noexcept(noexcept(m_value ^ rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value ^ rhs.m_value};
  }

  constexpr auto operator^(named_type&& rhs) && noexcept(noexcept(m_value ^ rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value ^ rhs.m_value};
  }

  constexpr auto operator<<(const named_type& rhs) const& noexcept(noexcept(m_value << rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value << rhs.m_value};
  }

  constexpr auto operator<<(named_type&& rhs) const& noexcept(noexcept(m_value << rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value << rhs.m_value};
  }

  constexpr auto operator<<(const named_type& rhs) && noexcept(noexcept(m_value << rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value << rhs.m_value};
  }

  constexpr auto operator<<(named_type&& rhs) && noexcept(noexcept(m_value << rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value << rhs.m_value};
  }

  constexpr auto operator>>(const named_type& rhs) const& noexcept(noexcept(m_value >> rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value >> rhs.m_value};
  }

  constexpr auto operator>>(named_type&& rhs) const& noexcept(noexcept(m_value >> rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value >> rhs.m_value};
  }

  constexpr auto operator>>(const named_type& rhs) && noexcept(noexcept(m_value >> rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value >> rhs.m_value};
  }

  constexpr auto operator>>(named_type&& rhs) && noexcept(noexcept(m_value >> rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value >> rhs.m_value};
  }

  constexpr auto operator&=(const named_type& rhs) & noexcept(noexcept(m_value &= rhs.m_value)) -> named_type&
    requires std::unsigned_integral<T>
  {
    m_value &= rhs.m_value;
    return *this;
  }

  constexpr auto operator&=(named_type&& rhs) & noexcept(noexcept(m_value &= rhs.m_value)) -> named_type&
    requires std::unsigned_integral<T>
  {
    m_value &= rhs.m_value;
    return *this;
  }

  constexpr auto operator|=(const named_type& rhs) & noexcept(noexcept(m_value |= rhs.m_value)) -> named_type&
    requires std::unsigned_integral<T>
  {
    m_value |= rhs.m_value;
    return *this;
  }

  constexpr auto operator|=(named_type&& rhs) & noexcept(noexcept(m_value |= rhs.m_value)) -> named_type&
    requires std::unsigned_integral<T>
  {
    m_value |= rhs.m_value;
    return *this;
  }

  constexpr auto operator^=(const named_type& rhs) & noexcept(noexcept(m_value ^= rhs.m_value)) -> named_type&
    requires std::unsigned_integral<T>
  {
    m_value ^= rhs.m_value;
    return *this;
  }

  constexpr auto operator^=(named_type&& rhs) & noexcept(noexcept(m_value ^= rhs.m_value)) -> named_type&
    requires std::unsigned_integral<T>
  {
    m_value ^= rhs.m_value;
    return *this;
  }

  constexpr auto operator<<=(const named_type& rhs) & noexcept(noexcept(m_value <<= rhs.m_value)) -> named_type&
    requires std::unsigned_integral<T>
  {
    m_value <<= rhs.m_value;
    return *this;
  }

  constexpr auto operator<<=(named_type&& rhs) & noexcept(noexcept(m_value <<= rhs.m_value)) -> named_type&
    requires std::unsigned_integral<T>
  {
    m_value <<= rhs.m_value;
    return *this;
  }

  constexpr auto operator>>=(const named_type& rhs) & noexcept(noexcept(m_value >>= rhs.m_value)) -> named_type&
    requires std::unsigned_integral<T>
  {
    m_value >>= rhs.m_value;
    return *this;
  }

  constexpr auto operator>>=(named_type&& rhs) & noexcept(noexcept(m_value >>= rhs.m_value)) -> named_type&
    requires std::unsigned_integral<T>
  {
    m_value >>= rhs.m_value;
    return *this;
  }

  //
  // Ranges interface
  //
  constexpr auto begin() const noexcept(noexcept(std::ranges::begin(m_value)))
    requires std::ranges::range<T>
  {
    return std::ranges::begin(m_value);
  }

  constexpr auto begin() noexcept(noexcept(std::ranges::begin(m_value)))
    requires std::ranges::range<T>
  {
    return std::ranges::begin(m_value);
  }

  constexpr auto end() const noexcept(noexcept(std::ranges::end(m_value)))
    requires std::ranges::range<T>
  {
    return std::ranges::end(m_value);
  }

  constexpr auto end() noexcept(noexcept(std::ranges::end(m_value)))
    requires std::ranges::range<T>
  {
    return std::ranges::end(m_value);
  }

  //
  // Stream operators
  //
  friend inline auto operator<<(std::ostream& ostream,
                                const named_type& rhs) noexcept(noexcept(ostream << rhs.m_value)) -> std::ostream&
    requires gw::ostreamable<T>
  {
    return ostream << rhs.m_value;
  }

  friend inline auto operator>>(std::istream& istream,
                                named_type& rhs) noexcept(noexcept(istream >> rhs.m_value)) -> std::istream&
    requires gw::istreamable<T>
  {
    return istream >> rhs.m_value;
  }

 private:
  T m_value{};
  static constexpr auto k_name = Name.value;
};

//
// Creation functions
//
template <detail::fixed_string Name, class T, typename... Args>
constexpr auto make_named_type(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
  requires std::constructible_from<std::remove_cvref_t<T>, Args...>
{
  return named_type<std::remove_cvref_t<T>, Name>{std::forward<Args>(args)...};
}

template <detail::fixed_string Name, class T, typename U, typename... Args>
constexpr auto make_named_type(std::initializer_list<U> ilist, Args&&... args) noexcept(
    std::is_nothrow_constructible_v<T, std::initializer_list<U>&, Args...>)
  requires std::constructible_from<T, std::initializer_list<U>&, Args...>
{
  return named_type<std::remove_cvref_t<T>, Name>{ilist, std::forward<Args>(args)...};
}

template <detail::fixed_string Name, class T>
constexpr auto make_named_type(T&& value) noexcept(
    std::is_nothrow_constructible_v<named_type<std::remove_cvref_t<T>, Name>, T>)
  requires std::constructible_from<std::remove_cvref_t<T>, T>
{
  return named_type<std::remove_cvref_t<T>, Name>{std::forward<T>(value)};
}

}  // namespace gw

namespace std {

//
// Hash calculation
//
template <gw::hashable T, gw::detail::fixed_string Name>
// NOLINTNEXTLINE(cert-dcl58-cpp)
struct hash<gw::named_type<T, Name>> {
  [[nodiscard]] auto inline operator()(const gw::named_type<T, Name>& named_type) const noexcept -> size_t {
    auto value_hash = hash<T>{}(named_type.value());
    auto name_hash = hash<std::string_view>{}(named_type.name());
    return value_hash ^ name_hash;
  }
};

//
// String conversion
//
template <gw::string_convertable T, gw::detail::fixed_string Name>
// NOLINTNEXTLINE(cert-dcl58-cpp)
[[nodiscard]] auto inline to_string(gw::named_type<T, Name> named_type) -> std::string {
  return std::string{named_type.name()} + ": " + std::to_string(named_type.value());
}

}  // namespace std
