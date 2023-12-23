// Copyright (c) 2023 Martin Stump
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <concepts>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <ranges>
#include <string>
#include <type_traits>
#include <typeindex>
#include <utility>

#include "gw/concepts.hpp"

namespace gw {

namespace detail {

struct strong_type_empty_base {
  constexpr auto operator<=>(const strong_type_empty_base&) const noexcept = default;
};

}  // namespace detail

template <typename T, typename Tag>
struct strong_type final
    : public std::conditional_t<std::ranges::range<T>, std::ranges::view_interface<strong_type<T, Tag>>,
                                detail::strong_type_empty_base> {
  //
  // Public types
  //
  using value_type = T;
  using tag_type = Tag;

  //
  // Constructors
  //
  template <typename... Args>
  constexpr explicit strong_type(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
    requires std::constructible_from<T, Args...>
      : m_value(T{std::forward<Args>(args)...}) {}

  template <typename U, typename... Args>
  constexpr strong_type(std::initializer_list<U> ilist,
                        Args&&... args) noexcept(std::is_nothrow_constructible_v<T, std::initializer_list<U>&, Args...>)
    requires std::constructible_from<T, std::initializer_list<U>&, Args...>
      : m_value(T{ilist, std::forward<Args>(args)...}) {}

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
  template <typename F>
  constexpr auto transform(F&& func) const& noexcept(noexcept(func(m_value)))
    requires std::invocable<F, const T&>
  {
    return strong_type<std::remove_cv_t<std::invoke_result_t<F, const T&>>, Tag>{func(m_value)};
  }

  template <typename F>
  constexpr auto transform(F&& func) & noexcept(noexcept(func(m_value)))
    requires std::invocable<F, T&>
  {
    return strong_type<std::remove_cv_t<std::invoke_result_t<F, T&>>, Tag>{func(m_value)};
  }

  template <typename F>
  constexpr auto transform(F&& func) const&& noexcept(noexcept(func(m_value)))
    requires std::invocable<F, const T&&>
  {
    return strong_type<std::remove_cv_t<std::invoke_result_t<F, const T&&>>, Tag>{func(std::move(m_value))};
  }

  template <typename F>
  constexpr auto transform(F&& func) && noexcept(noexcept(func(m_value)))
    requires std::invocable<F, T&&>
  {
    return strong_type<std::remove_cv_t<std::invoke_result_t<F, T&&>>, Tag>{func(std::move(m_value))};
  }

  //
  // Modifiers
  //
  constexpr void swap(strong_type& rhs) noexcept(std::is_nothrow_swappable_v<T>)
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

  template <typename... Args>
  constexpr auto emplace(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>) -> T&
    requires std::constructible_from<T, Args...>
  {
    m_value = T{std::forward<Args>(args)...};
    return m_value;
  }

  //
  // Comparison operators
  //
  constexpr auto operator<=>(const strong_type& rhs) const noexcept = default;

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
  constexpr auto operator++() & noexcept(noexcept(++m_value)) -> strong_type&
    requires incrementable<T>
  {
    ++m_value;
    return *this;
  }

  constexpr auto operator++() && noexcept(noexcept(++m_value)) -> strong_type&&
    requires incrementable<T>
  {
    ++m_value;
    return std::move(*this);
  }

  constexpr auto operator++(int) & noexcept(noexcept(m_value++)) -> strong_type
    requires incrementable<T>
  {
    return strong_type{m_value++};
  }

  constexpr auto operator++(int) && noexcept(noexcept(m_value++)) -> strong_type
    requires incrementable<T>
  {
    return strong_type{m_value++};
  }

  constexpr auto operator--() & noexcept(noexcept(--m_value)) -> strong_type&
    requires decrementable<T>
  {
    --m_value;
    return *this;
  }

  constexpr auto operator--() && noexcept(noexcept(--m_value)) -> strong_type&&
    requires decrementable<T>
  {
    --m_value;
    return std::move(*this);
  }

  constexpr auto operator--(int) & noexcept(noexcept(m_value--)) -> strong_type
    requires decrementable<T>
  {
    return strong_type{m_value--};
  }

  constexpr auto operator--(int) && noexcept(noexcept(m_value--)) -> strong_type
    requires decrementable<T>
  {
    return strong_type{m_value--};
  }

  //
  // Arithmetic operators
  //
  constexpr auto operator+(const strong_type& rhs) const& noexcept(noexcept(m_value + rhs.m_value)) -> strong_type
    requires arithmetic<T>
  {
    return strong_type{m_value + rhs.m_value};
  }

  constexpr auto operator+(strong_type&& rhs) const& noexcept(noexcept(m_value + rhs.m_value)) -> strong_type
    requires arithmetic<T>
  {
    return strong_type{m_value + rhs.m_value};
  }

  constexpr auto operator+(const strong_type& rhs) && noexcept(noexcept(m_value + rhs.m_value)) -> strong_type
    requires arithmetic<T>
  {
    return strong_type{m_value + rhs.m_value};
  }

  constexpr auto operator+(strong_type&& rhs) && noexcept(noexcept(m_value + rhs.m_value)) -> strong_type
    requires arithmetic<T>
  {
    return strong_type{m_value + rhs.m_value};
  }

  constexpr auto operator-(const strong_type& rhs) const& noexcept(noexcept(m_value - rhs.m_value)) -> strong_type
    requires arithmetic<T>
  {
    return strong_type{m_value - rhs.m_value};
  }

  constexpr auto operator-(strong_type&& rhs) const& noexcept(noexcept(m_value - rhs.m_value)) -> strong_type
    requires arithmetic<T>
  {
    return strong_type{m_value - rhs.m_value};
  }

  constexpr auto operator-(const strong_type& rhs) && noexcept(noexcept(m_value - rhs.m_value)) -> strong_type
    requires arithmetic<T>
  {
    return strong_type{m_value - rhs.m_value};
  }

  constexpr auto operator-(strong_type&& rhs) && noexcept(noexcept(m_value - rhs.m_value)) -> strong_type
    requires arithmetic<T>
  {
    return strong_type{m_value - rhs.m_value};
  }

  constexpr auto operator*(const strong_type& rhs) const& noexcept(noexcept(m_value * rhs.m_value)) -> strong_type
    requires arithmetic<T>
  {
    return strong_type{m_value * rhs.m_value};
  }

  constexpr auto operator*(strong_type&& rhs) const& noexcept(noexcept(m_value * rhs.m_value)) -> strong_type
    requires arithmetic<T>
  {
    return strong_type{m_value * rhs.m_value};
  }

  constexpr auto operator*(const strong_type& rhs) && noexcept(noexcept(m_value * rhs.m_value)) -> strong_type
    requires arithmetic<T>
  {
    return strong_type{m_value * rhs.m_value};
  }

  constexpr auto operator*(strong_type&& rhs) && noexcept(noexcept(m_value * rhs.m_value)) -> strong_type
    requires arithmetic<T>
  {
    return strong_type{m_value * rhs.m_value};
  }

  constexpr auto operator/(const strong_type& rhs) const& noexcept(noexcept(m_value / rhs.m_value)) -> strong_type
    requires arithmetic<T>
  {
    return strong_type{m_value / rhs.m_value};
  }

  constexpr auto operator/(strong_type&& rhs) const& noexcept(noexcept(m_value / rhs.m_value)) -> strong_type
    requires arithmetic<T>
  {
    return strong_type{m_value / rhs.m_value};
  }

  constexpr auto operator/(const strong_type& rhs) && noexcept(noexcept(m_value / rhs.m_value)) -> strong_type
    requires arithmetic<T>
  {
    return strong_type{m_value / rhs.m_value};
  }

  constexpr auto operator/(strong_type&& rhs) && noexcept(noexcept(m_value / rhs.m_value)) -> strong_type
    requires arithmetic<T>
  {
    return strong_type{m_value / rhs.m_value};
  }

  constexpr auto operator%(const strong_type& rhs) const& noexcept(noexcept(m_value % rhs.m_value)) -> strong_type
    requires arithmetic<T>
  {
    return strong_type{m_value % rhs.m_value};
  }

  constexpr auto operator%(strong_type&& rhs) const& noexcept(noexcept(m_value % rhs.m_value)) -> strong_type
    requires arithmetic<T>
  {
    return strong_type{m_value % rhs.m_value};
  }

  constexpr auto operator%(const strong_type& rhs) && noexcept(noexcept(m_value % rhs.m_value)) -> strong_type
    requires arithmetic<T>
  {
    return strong_type{m_value % rhs.m_value};
  }

  constexpr auto operator%(strong_type&& rhs) && noexcept(noexcept(m_value % rhs.m_value)) -> strong_type
    requires arithmetic<T>
  {
    return strong_type{m_value % rhs.m_value};
  }

  constexpr auto operator+=(const strong_type& rhs) & noexcept(noexcept(m_value += rhs.m_value)) -> strong_type&
    requires arithmetic<T>
  {
    m_value += rhs.m_value;
    return *this;
  }

  constexpr auto operator+=(strong_type&& rhs) & noexcept(noexcept(m_value += rhs.m_value)) -> strong_type&
    requires arithmetic<T>
  {
    m_value += rhs.m_value;
    return *this;
  }

  constexpr auto operator-=(const strong_type& rhs) & noexcept(noexcept(m_value -= rhs.m_value)) -> strong_type&
    requires arithmetic<T>
  {
    m_value -= rhs.m_value;
    return *this;
  }

  constexpr auto operator-=(strong_type&& rhs) & noexcept(noexcept(m_value -= rhs.m_value)) -> strong_type&
    requires arithmetic<T>
  {
    m_value -= rhs.m_value;
    return *this;
  }

  constexpr auto operator*=(const strong_type& rhs) & noexcept(noexcept(m_value *= rhs.m_value)) -> strong_type&
    requires arithmetic<T>
  {
    m_value *= rhs.m_value;
    return *this;
  }

  constexpr auto operator*=(strong_type&& rhs) & noexcept(noexcept(m_value *= rhs.m_value)) -> strong_type&
    requires arithmetic<T>
  {
    m_value *= rhs.m_value;
    return *this;
  }

  constexpr auto operator/=(const strong_type& rhs) & noexcept(noexcept(m_value /= rhs.m_value)) -> strong_type&
    requires arithmetic<T>
  {
    m_value /= rhs.m_value;
    return *this;
  }

  constexpr auto operator/=(strong_type&& rhs) & noexcept(noexcept(m_value /= rhs.m_value)) -> strong_type&
    requires arithmetic<T>
  {
    m_value /= rhs.m_value;
    return *this;
  }

  constexpr auto operator%=(const strong_type& rhs) & noexcept(noexcept(m_value %= rhs.m_value)) -> strong_type&
    requires arithmetic<T>
  {
    m_value %= rhs.m_value;
    return *this;
  }

  constexpr auto operator%=(strong_type&& rhs) & noexcept(noexcept(m_value %= rhs.m_value)) -> strong_type&
    requires arithmetic<T>
  {
    m_value %= rhs.m_value;
    return *this;
  }

  //
  // Bitwise operators
  //
  constexpr auto operator~() const& noexcept(noexcept(~m_value)) -> strong_type
    requires std::unsigned_integral<T>
  {
    return strong_type{~m_value};
  }

  constexpr auto operator&(const strong_type& rhs) const& noexcept(noexcept(m_value & rhs.m_value)) -> strong_type
    requires std::unsigned_integral<T>
  {
    return strong_type{m_value & rhs.m_value};
  }

  constexpr auto operator&(strong_type&& rhs) const& noexcept(noexcept(m_value & rhs.m_value)) -> strong_type
    requires std::unsigned_integral<T>
  {
    return strong_type{m_value & rhs.m_value};
  }

  constexpr auto operator&(const strong_type& rhs) && noexcept(noexcept(m_value & rhs.m_value)) -> strong_type
    requires std::unsigned_integral<T>
  {
    return strong_type{m_value & rhs.m_value};
  }

  constexpr auto operator&(strong_type&& rhs) && noexcept(noexcept(m_value & rhs.m_value)) -> strong_type
    requires std::unsigned_integral<T>
  {
    return strong_type{m_value & rhs.m_value};
  }

  constexpr auto operator|(const strong_type& rhs) const& noexcept(noexcept(m_value | rhs.m_value)) -> strong_type
    requires std::unsigned_integral<T>
  {
    return strong_type{m_value | rhs.m_value};
  }

  constexpr auto operator|(strong_type&& rhs) const& noexcept(noexcept(m_value | rhs.m_value)) -> strong_type
    requires std::unsigned_integral<T>
  {
    return strong_type{m_value | rhs.m_value};
  }

  constexpr auto operator|(const strong_type& rhs) && noexcept(noexcept(m_value | rhs.m_value)) -> strong_type
    requires std::unsigned_integral<T>
  {
    return strong_type{m_value | rhs.m_value};
  }

  constexpr auto operator|(strong_type&& rhs) && noexcept(noexcept(m_value | rhs.m_value)) -> strong_type
    requires std::unsigned_integral<T>
  {
    return strong_type{m_value | rhs.m_value};
  }

  constexpr auto operator^(const strong_type& rhs) const& noexcept(noexcept(m_value ^ rhs.m_value)) -> strong_type
    requires std::unsigned_integral<T>
  {
    return strong_type{m_value ^ rhs.m_value};
  }

  constexpr auto operator^(strong_type&& rhs) const& noexcept(noexcept(m_value ^ rhs.m_value)) -> strong_type
    requires std::unsigned_integral<T>
  {
    return strong_type{m_value ^ rhs.m_value};
  }

  constexpr auto operator^(const strong_type& rhs) && noexcept(noexcept(m_value ^ rhs.m_value)) -> strong_type
    requires std::unsigned_integral<T>
  {
    return strong_type{m_value ^ rhs.m_value};
  }

  constexpr auto operator^(strong_type&& rhs) && noexcept(noexcept(m_value ^ rhs.m_value)) -> strong_type
    requires std::unsigned_integral<T>
  {
    return strong_type{m_value ^ rhs.m_value};
  }

  constexpr auto operator<<(const strong_type& rhs) const& noexcept(noexcept(m_value << rhs.m_value)) -> strong_type
    requires std::unsigned_integral<T>
  {
    return strong_type{m_value << rhs.m_value};
  }

  constexpr auto operator<<(strong_type&& rhs) const& noexcept(noexcept(m_value << rhs.m_value)) -> strong_type
    requires std::unsigned_integral<T>
  {
    return strong_type{m_value << rhs.m_value};
  }

  constexpr auto operator<<(const strong_type& rhs) && noexcept(noexcept(m_value << rhs.m_value)) -> strong_type
    requires std::unsigned_integral<T>
  {
    return strong_type{m_value << rhs.m_value};
  }

  constexpr auto operator<<(strong_type&& rhs) && noexcept(noexcept(m_value << rhs.m_value)) -> strong_type
    requires std::unsigned_integral<T>
  {
    return strong_type{m_value << rhs.m_value};
  }

  constexpr auto operator>>(const strong_type& rhs) const& noexcept(noexcept(m_value >> rhs.m_value)) -> strong_type
    requires std::unsigned_integral<T>
  {
    return strong_type{m_value >> rhs.m_value};
  }

  constexpr auto operator>>(strong_type&& rhs) const& noexcept(noexcept(m_value >> rhs.m_value)) -> strong_type
    requires std::unsigned_integral<T>
  {
    return strong_type{m_value >> rhs.m_value};
  }

  constexpr auto operator>>(const strong_type& rhs) && noexcept(noexcept(m_value >> rhs.m_value)) -> strong_type
    requires std::unsigned_integral<T>
  {
    return strong_type{m_value >> rhs.m_value};
  }

  constexpr auto operator>>(strong_type&& rhs) && noexcept(noexcept(m_value >> rhs.m_value)) -> strong_type
    requires std::unsigned_integral<T>
  {
    return strong_type{m_value >> rhs.m_value};
  }

  constexpr auto operator&=(const strong_type& rhs) & noexcept(noexcept(m_value &= rhs.m_value)) -> strong_type&
    requires std::unsigned_integral<T>
  {
    m_value &= rhs.m_value;
    return *this;
  }

  constexpr auto operator&=(strong_type&& rhs) & noexcept(noexcept(m_value &= rhs.m_value)) -> strong_type&
    requires std::unsigned_integral<T>
  {
    m_value &= rhs.m_value;
    return *this;
  }

  constexpr auto operator|=(const strong_type& rhs) & noexcept(noexcept(m_value |= rhs.m_value)) -> strong_type&
    requires std::unsigned_integral<T>
  {
    m_value |= rhs.m_value;
    return *this;
  }

  constexpr auto operator|=(strong_type&& rhs) & noexcept(noexcept(m_value |= rhs.m_value)) -> strong_type&
    requires std::unsigned_integral<T>
  {
    m_value |= rhs.m_value;
    return *this;
  }

  constexpr auto operator^=(const strong_type& rhs) & noexcept(noexcept(m_value ^= rhs.m_value)) -> strong_type&
    requires std::unsigned_integral<T>
  {
    m_value ^= rhs.m_value;
    return *this;
  }

  constexpr auto operator^=(strong_type&& rhs) & noexcept(noexcept(m_value ^= rhs.m_value)) -> strong_type&
    requires std::unsigned_integral<T>
  {
    m_value ^= rhs.m_value;
    return *this;
  }

  constexpr auto operator<<=(const strong_type& rhs) & noexcept(noexcept(m_value <<= rhs.m_value)) -> strong_type&
    requires std::unsigned_integral<T>
  {
    m_value <<= rhs.m_value;
    return *this;
  }

  constexpr auto operator<<=(strong_type&& rhs) & noexcept(noexcept(m_value <<= rhs.m_value)) -> strong_type&
    requires std::unsigned_integral<T>
  {
    m_value <<= rhs.m_value;
    return *this;
  }

  constexpr auto operator>>=(const strong_type& rhs) & noexcept(noexcept(m_value >>= rhs.m_value)) -> strong_type&
    requires std::unsigned_integral<T>
  {
    m_value >>= rhs.m_value;
    return *this;
  }

  constexpr auto operator>>=(strong_type&& rhs) & noexcept(noexcept(m_value >>= rhs.m_value)) -> strong_type&
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
                                const strong_type& rhs) noexcept(noexcept(ostream << rhs.m_value)) -> std::ostream&
    requires ostreamable<T>
  {
    return ostream << rhs.m_value;
  }

  friend inline auto operator>>(std::istream& istream,
                                strong_type& rhs) noexcept(noexcept(istream >> rhs.m_value)) -> std::istream&
    requires istreamable<T>
  {
    return istream >> rhs.m_value;
  }

 private:
  T m_value{};
};

//
// Creation functions
//
template <typename Tag, typename T, typename... Args>
constexpr auto make_strong_type(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
  requires std::constructible_from<std::remove_cvref_t<T>, Args...>
{
  return strong_type<std::remove_cvref_t<T>, Tag>{std::forward<Args>(args)...};
}

template <typename Tag, typename T, typename U, typename... Args>
constexpr auto make_strong_type(std::initializer_list<U> ilist, Args&&... args) noexcept(
    std::is_nothrow_constructible_v<T, std::initializer_list<U>&, Args...>)
  requires std::constructible_from<T, std::initializer_list<U>&, Args...>
{
  return strong_type<std::remove_cvref_t<T>, Tag>{ilist, std::forward<Args>(args)...};
}

template <typename Tag, typename T>
constexpr auto make_strong_type(T&& value) noexcept(
    std::is_nothrow_constructible_v<strong_type<std::remove_cvref_t<T>, Tag>, T>)
  requires std::constructible_from<std::remove_cvref_t<T>, T>
{
  return strong_type<std::remove_cvref_t<T>, Tag>{std::forward<T>(value)};
}

}  // namespace gw

namespace std {

//
// Hash calculation
//
template <::gw::hashable T, ::gw::complete Tag>
// NOLINTNEXTLINE(cert-dcl58-cpp)
struct hash<::gw::strong_type<T, Tag>> {
  [[nodiscard]] auto inline operator()(const ::gw::strong_type<T, Tag>& strong_type) const noexcept -> size_t {
    auto tag_hash = hash<type_index>{}(type_index{typeid(Tag)});
    auto value_hash = hash<T>{}(strong_type.value());
    return tag_hash ^ value_hash;
  }
};

//
// String conversion
//
template <::gw::string_convertable T, typename Tag>
// NOLINTNEXTLINE(cert-dcl58-cpp)
[[nodiscard]] auto inline to_string(const ::gw::strong_type<T, Tag>& strong_type) -> string {
  return to_string(strong_type.value());
}

template <::gw::string_convertable T, ::gw::named Tag>
// NOLINTNEXTLINE(cert-dcl58-cpp)
[[nodiscard]] auto inline to_string(const ::gw::strong_type<T, Tag>& strong_type) -> string {
  return string{Tag::name()} + ": " + to_string(strong_type.value());
}

}  // namespace std
