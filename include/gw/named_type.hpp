// Copyright (c) 2023 Martin Stump
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <algorithm>
#include <compare>
#include <concepts>
#include <cstddef>
#include <format>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <ranges>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include "gw/concepts.hpp"

/// \brief GW namespace
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
  constexpr auto operator<=>(const named_type_empty_base&) const noexcept -> std::strong_ordering = default;
};

}  // namespace detail

/// \example named_type_example.cpp
//
/// \brief Named type wrapper.
//
/// \details The class template `gw::named_type` is a simple wrapper around a value of some type `T`. It is intended to
/// be used to create a new type that is distinct from `T`, but which has the same representation and semantics as `T`.
/// This is useful to distinguish between values that are conceptually different, but which have the same underlying
/// type. For example, a `std::string` can be used to represent a person's name, but it can also be used to represent a
/// person's address. Using `gw::named_type` to create a `name_t` and an `address_t` allows the compiler to catch errors
/// where a `name_t` is used where an `address_t` is expected, and vice versa.
//
template <typename T, detail::fixed_string Name>
class named_type final
    : public std::conditional_t<std::ranges::range<T>, std::ranges::view_interface<named_type<T, Name>>,
                                detail::named_type_empty_base> {
 public:
  //
  // Public types
  //

  using value_type = T;  ///< The type of the contained value.

  //
  // Constructors
  //

  /// \brief Construct the gw::named_type object.
  template <typename... Args>
  constexpr explicit named_type(Args&&... args) noexcept(std::is_nothrow_constructible_v<value_type, Args...>)
    requires std::constructible_from<value_type, Args...>
      : m_value(value_type{std::forward<Args>(args)...}) {}

  /// \brief Construct the gw::named_type object.
  template <typename U, typename... Args>
  constexpr named_type(std::initializer_list<U> ilist, Args&&... args) noexcept(
      std::is_nothrow_constructible_v<value_type, std::initializer_list<U>&, Args...>)
    requires std::constructible_from<value_type, std::initializer_list<U>&, Args...>
      : m_value(value_type{ilist, std::forward<Args>(args)...}) {}

  //
  // Destructor
  //

  /// \brief Destroy the contained value.
  ~named_type() noexcept(std::is_nothrow_destructible_v<value_type>) = default;

  //
  // Static functions
  //

  /// \brief Return the name of the gw::named_type.
  static constexpr auto name() noexcept -> std::string_view { return k_name; }

  //
  // Observers
  //

  /// \brief Access the contained value.
  constexpr auto operator->() const noexcept -> const value_type* { return &m_value; }

  /// \brief Access the contained value.
  constexpr auto operator->() noexcept -> value_type* { return &m_value; }

  /// \brief Access the contained value.
  constexpr auto operator*() const& noexcept -> const value_type& { return m_value; }

  /// \brief Access the contained value.
  constexpr auto operator*() & noexcept -> value_type& { return m_value; }

  /// \brief Access the contained value.
  constexpr auto operator*() const&& noexcept -> const value_type&& { return std::move(m_value); }

  /// \brief Access the contained value.
  constexpr auto operator*() && noexcept -> value_type&& { return std::move(m_value); }

  /// \brief Return the contained value.
  constexpr auto value() const& noexcept -> const value_type& { return m_value; }

  /// \brief Return the contained value.
  constexpr auto value() & noexcept -> value_type& { return m_value; }

  /// \brief Return the contained value.
  constexpr auto value() const&& noexcept -> const value_type&& { return std::move(m_value); }

  /// \brief Return the contained value.
  constexpr auto value() && noexcept -> value_type&& { return std::move(m_value); }

  //
  // Monadic operations
  //

  /// \brief Return a gw::named_type containing the transformed contained value.
  template <typename F>
  constexpr auto transform(F&& func) const& noexcept(noexcept(func(m_value))) -> named_type
    requires std::invocable<F, const value_type&>
  {
    return named_type<std::remove_cv_t<std::invoke_result_t<F, const value_type&>>, Name>{func(m_value)};
  }

  /// \brief Return a gw::named_type containing the transformed contained value.
  template <typename F>
  constexpr auto transform(F&& func) & noexcept(noexcept(func(m_value))) -> named_type
    requires std::invocable<F, value_type&>
  {
    return named_type<std::remove_cv_t<std::invoke_result_t<F, value_type&>>, Name>{func(m_value)};
  }

  /// \brief Return a gw::named_type containing the transformed contained value.
  template <typename F>
  constexpr auto transform(F&& func) const&& noexcept(noexcept(func(m_value))) -> named_type
    requires std::invocable<F, const value_type&&>
  {
    return named_type<std::remove_cv_t<std::invoke_result_t<F, const value_type&&>>, Name>{func(std::move(m_value))};
  }

  /// \brief Return a gw::named_type containing the transformed contained value.
  template <typename F>
  constexpr auto transform(F&& func) && noexcept(noexcept(func(m_value))) -> named_type
    requires std::invocable<F, value_type&&>
  {
    return named_type<std::remove_cv_t<std::invoke_result_t<F, value_type&&>>, Name>{func(std::move(m_value))};
  }

  //
  // Modifiers
  //

  /// \brief Specialize the std::swap algorithm.
  constexpr void swap(named_type& rhs) noexcept(std::is_nothrow_swappable_v<value_type>)
    requires std::swappable<value_type>
  {
    using std::swap;
    swap(m_value, rhs.m_value);
  }

  /// \brief Destroy any contained value.
  constexpr void reset() noexcept(std::is_nothrow_default_constructible_v<value_type>)
    requires std::default_initializable<value_type>
  {
    m_value = value_type{};
  }

  /// \brief Construct the contained value in-place.
  template <typename... Args>
  constexpr auto emplace(Args&&... args) noexcept(std::is_nothrow_constructible_v<value_type, Args...>) -> value_type&
    requires std::constructible_from<value_type, Args...>
  {
    m_value = value_type{std::forward<Args>(args)...};
    return m_value;
  }

  //
  // Comparison operators
  //

  /// \brief Compare gw::named_type objects.
  constexpr auto operator==(const named_type& rhs) const& noexcept(noexcept(m_value == rhs.m_value)) -> bool
    requires std::equality_comparable<value_type>
  {
    return m_value == rhs.m_value;
  }

  /// \brief Compare gw::named_type objects.
  constexpr auto operator!=(const named_type& rhs) const& noexcept(noexcept(m_value != rhs.m_value)) -> bool
    requires std::equality_comparable<value_type>
  {
    return m_value != rhs.m_value;
  }

  /// \brief Compare gw::named_type objects.
  constexpr auto operator<(const named_type& rhs) const& noexcept(noexcept(m_value < rhs.m_value)) -> bool
    requires std::totally_ordered<value_type>
  {
    return m_value < rhs.m_value;
  }

  /// \brief Compare gw::named_type objects.
  constexpr auto operator>(const named_type& rhs) const& noexcept(noexcept(m_value > rhs.m_value)) -> bool
    requires std::totally_ordered<value_type>
  {
    return m_value > rhs.m_value;
  }

  /// \brief Compare gw::named_type objects.
  constexpr auto operator<=(const named_type& rhs) const& noexcept(noexcept(m_value <= rhs.m_value)) -> bool
    requires std::totally_ordered<value_type>
  {
    return m_value <= rhs.m_value;
  }

  /// \brief Compare gw::named_type objects.
  constexpr auto operator>=(const named_type& rhs) const& noexcept(noexcept(m_value >= rhs.m_value)) -> bool
    requires std::totally_ordered<value_type>
  {
    return m_value >= rhs.m_value;
  }

  /// \brief Compare gw::named_type objects.
  constexpr auto operator<=>(const named_type& rhs) const& noexcept(noexcept(m_value <=>
                                                                             rhs.m_value)) -> std::strong_ordering
    requires std::three_way_comparable<value_type>
  {
    return m_value <=> rhs.m_value;
  }

  //
  // Conversion operators
  //

  /// \brief Convert the gw::named_type to its underlying type.
  constexpr explicit operator const value_type&() const& noexcept { return m_value; }

  /// \brief Convert the gw::named_type to its underlying type.
  constexpr explicit operator value_type&() & noexcept { return m_value; }

  /// \brief Convert the gw::named_type to its underlying type.
  constexpr explicit operator const value_type&&() const&& noexcept { return std::move(m_value); }

  /// \brief Convert the gw::named_type to its underlying type.
  constexpr explicit operator value_type&&() && noexcept { return std::move(m_value); }

  //
  // Increment and decrement operators
  //

  /// \brief increments the contained value
  constexpr auto operator++() & noexcept(noexcept(++m_value)) -> named_type&
    requires incrementable<value_type>
  {
    ++m_value;
    return *this;
  }

  /// \brief increments the contained value
  constexpr auto operator++() && noexcept(noexcept(++m_value)) -> named_type&&
    requires incrementable<value_type>
  {
    ++m_value;
    return std::move(*this);
  }

  /// \brief increments the contained value
  constexpr auto operator++(int) & noexcept(noexcept(m_value++)) -> named_type
    requires incrementable<value_type>
  {
    return named_type{m_value++};
  }

  /// \brief increments the contained value
  constexpr auto operator++(int) && noexcept(noexcept(m_value++)) -> named_type
    requires incrementable<value_type>
  {
    return named_type{m_value++};
  }

  /// \brief decrements the contained value
  constexpr auto operator--() & noexcept(noexcept(--m_value)) -> named_type&
    requires decrementable<value_type>
  {
    --m_value;
    return *this;
  }

  /// \brief decrements the contained value
  constexpr auto operator--() && noexcept(noexcept(--m_value)) -> named_type&&
    requires decrementable<value_type>
  {
    --m_value;
    return std::move(*this);
  }

  /// \brief decrements the contained value
  constexpr auto operator--(int) & noexcept(noexcept(m_value--)) -> named_type
    requires decrementable<value_type>
  {
    return named_type{m_value--};
  }

  /// \brief decrements the contained value
  constexpr auto operator--(int) && noexcept(noexcept(m_value--)) -> named_type
    requires decrementable<value_type>
  {
    return named_type{m_value--};
  }

  //
  // Arithmetic operators
  //

  /// \brief affirms the contained value
  constexpr auto operator+() const& noexcept(noexcept(+m_value)) -> named_type
    requires std::signed_integral<value_type>
  {
    return named_type{+m_value};
  }

  /// \brief affirms the contained value
  constexpr auto operator+() && noexcept(noexcept(+m_value)) -> named_type
    requires std::signed_integral<value_type>
  {
    return named_type{+m_value};
  }

  /// \brief negates the contained value
  constexpr auto operator-() const& noexcept(noexcept(-m_value)) -> named_type
    requires std::signed_integral<value_type>
  {
    return named_type{-m_value};
  }

  /// \brief negates the contained value
  constexpr auto operator-() && noexcept(noexcept(-m_value)) -> named_type
    requires std::signed_integral<value_type>
  {
    return named_type{-m_value};
  }

  /// \brief adds the contained values
  constexpr auto operator+(const named_type& rhs) const& noexcept(noexcept(m_value + rhs.m_value)) -> named_type
    requires arithmetic<value_type>
  {
    return named_type{m_value + rhs.m_value};
  }

  /// \brief adds the contained values
  constexpr auto operator+(named_type&& rhs) const& noexcept(noexcept(m_value + rhs.m_value)) -> named_type
    requires arithmetic<value_type>
  {
    return named_type{m_value + rhs.m_value};
  }

  /// \brief adds the contained values
  constexpr auto operator+(const named_type& rhs) && noexcept(noexcept(m_value + rhs.m_value)) -> named_type
    requires arithmetic<value_type>
  {
    return named_type{m_value + rhs.m_value};
  }

  /// \brief adds the contained values
  constexpr auto operator+(named_type&& rhs) && noexcept(noexcept(m_value + rhs.m_value)) -> named_type
    requires arithmetic<value_type>
  {
    return named_type{m_value + rhs.m_value};
  }

  /// \brief subtracts the contained values
  constexpr auto operator-(const named_type& rhs) const& noexcept(noexcept(m_value - rhs.m_value)) -> named_type
    requires arithmetic<value_type>
  {
    return named_type{m_value - rhs.m_value};
  }

  /// \brief subtracts the contained values
  constexpr auto operator-(named_type&& rhs) const& noexcept(noexcept(m_value - rhs.m_value)) -> named_type
    requires arithmetic<value_type>
  {
    return named_type{m_value - rhs.m_value};
  }

  /// \brief subtracts the contained values
  constexpr auto operator-(const named_type& rhs) && noexcept(noexcept(m_value - rhs.m_value)) -> named_type
    requires arithmetic<value_type>
  {
    return named_type{m_value - rhs.m_value};
  }

  /// \brief subtracts the contained values
  constexpr auto operator-(named_type&& rhs) && noexcept(noexcept(m_value - rhs.m_value)) -> named_type
    requires arithmetic<value_type>
  {
    return named_type{m_value - rhs.m_value};
  }

  /// \brief multiplies the contained values
  constexpr auto operator*(const named_type& rhs) const& noexcept(noexcept(m_value * rhs.m_value)) -> named_type
    requires arithmetic<value_type>
  {
    return named_type{m_value * rhs.m_value};
  }

  /// \brief multiplies the contained values
  constexpr auto operator*(named_type&& rhs) const& noexcept(noexcept(m_value * rhs.m_value)) -> named_type
    requires arithmetic<value_type>
  {
    return named_type{m_value * rhs.m_value};
  }

  /// \brief multiplies the contained values
  constexpr auto operator*(const named_type& rhs) && noexcept(noexcept(m_value * rhs.m_value)) -> named_type
    requires arithmetic<value_type>
  {
    return named_type{m_value * rhs.m_value};
  }

  /// \brief multiplies the contained values
  constexpr auto operator*(named_type&& rhs) && noexcept(noexcept(m_value * rhs.m_value)) -> named_type
    requires arithmetic<value_type>
  {
    return named_type{m_value * rhs.m_value};
  }

  /// \brief devides the contained values
  constexpr auto operator/(const named_type& rhs) const& noexcept(noexcept(m_value / rhs.m_value)) -> named_type
    requires arithmetic<value_type>
  {
    return named_type{m_value / rhs.m_value};
  }

  /// \brief devides the contained values
  constexpr auto operator/(named_type&& rhs) const& noexcept(noexcept(m_value / rhs.m_value)) -> named_type
    requires arithmetic<value_type>
  {
    return named_type{m_value / rhs.m_value};
  }

  /// \brief devides the contained values
  constexpr auto operator/(const named_type& rhs) && noexcept(noexcept(m_value / rhs.m_value)) -> named_type
    requires arithmetic<value_type>
  {
    return named_type{m_value / rhs.m_value};
  }

  /// \brief devides the contained values
  constexpr auto operator/(named_type&& rhs) && noexcept(noexcept(m_value / rhs.m_value)) -> named_type
    requires arithmetic<value_type>
  {
    return named_type{m_value / rhs.m_value};
  }

  /// \brief calculates the remainder of the contained values
  constexpr auto operator%(const named_type& rhs) const& noexcept(noexcept(m_value % rhs.m_value)) -> named_type
    requires arithmetic<value_type>
  {
    return named_type{m_value % rhs.m_value};
  }

  /// \brief calculates the remainder of the contained values
  constexpr auto operator%(named_type&& rhs) const& noexcept(noexcept(m_value % rhs.m_value)) -> named_type
    requires arithmetic<value_type>
  {
    return named_type{m_value % rhs.m_value};
  }

  /// \brief calculates the remainder of the contained values
  constexpr auto operator%(const named_type& rhs) && noexcept(noexcept(m_value % rhs.m_value)) -> named_type
    requires arithmetic<value_type>
  {
    return named_type{m_value % rhs.m_value};
  }

  /// \brief calculates the remainder of the contained values
  constexpr auto operator%(named_type&& rhs) && noexcept(noexcept(m_value % rhs.m_value)) -> named_type
    requires arithmetic<value_type>
  {
    return named_type{m_value % rhs.m_value};
  }

  /// \brief adds the contained values and assigns the result
  constexpr auto operator+=(const named_type& rhs) & noexcept(noexcept(m_value += rhs.m_value)) -> named_type&
    requires arithmetic<value_type>
  {
    m_value += rhs.m_value;
    return *this;
  }

  /// \brief adds the contained values and assigns the result
  constexpr auto operator+=(named_type&& rhs) & noexcept(noexcept(m_value += rhs.m_value)) -> named_type&
    requires arithmetic<value_type>
  {
    m_value += rhs.m_value;
    return *this;
  }

  /// \brief subtracts the contained values and assigns the result
  constexpr auto operator-=(const named_type& rhs) & noexcept(noexcept(m_value -= rhs.m_value)) -> named_type&
    requires arithmetic<value_type>
  {
    m_value -= rhs.m_value;
    return *this;
  }

  /// \brief subtracts the contained values and assigns the result
  constexpr auto operator-=(named_type&& rhs) & noexcept(noexcept(m_value -= rhs.m_value)) -> named_type&
    requires arithmetic<value_type>
  {
    m_value -= rhs.m_value;
    return *this;
  }

  /// \brief multiplies the contained values and assigns the result
  constexpr auto operator*=(const named_type& rhs) & noexcept(noexcept(m_value *= rhs.m_value)) -> named_type&
    requires arithmetic<value_type>
  {
    m_value *= rhs.m_value;
    return *this;
  }

  /// \brief multiplies the contained values and assigns the result
  constexpr auto operator*=(named_type&& rhs) & noexcept(noexcept(m_value *= rhs.m_value)) -> named_type&
    requires arithmetic<value_type>
  {
    m_value *= rhs.m_value;
    return *this;
  }

  /// \brief devides the contained values and assigns the result
  constexpr auto operator/=(const named_type& rhs) & noexcept(noexcept(m_value /= rhs.m_value)) -> named_type&
    requires arithmetic<value_type>
  {
    m_value /= rhs.m_value;
    return *this;
  }

  /// \brief devides the contained values and assigns the result
  constexpr auto operator/=(named_type&& rhs) & noexcept(noexcept(m_value /= rhs.m_value)) -> named_type&
    requires arithmetic<value_type>
  {
    m_value /= rhs.m_value;
    return *this;
  }

  /// \brief calculates the remainder of the contained values and assigns the result
  constexpr auto operator%=(const named_type& rhs) & noexcept(noexcept(m_value %= rhs.m_value)) -> named_type&
    requires arithmetic<value_type>
  {
    m_value %= rhs.m_value;
    return *this;
  }

  /// \brief calculates the remainder of the contained values and assigns the result
  constexpr auto operator%=(named_type&& rhs) & noexcept(noexcept(m_value %= rhs.m_value)) -> named_type&
    requires arithmetic<value_type>
  {
    m_value %= rhs.m_value;
    return *this;
  }

  //
  // Bitwise operators
  //

  /// \brief inverts the contained value
  constexpr auto operator~() const& noexcept(noexcept(~m_value)) -> named_type
    requires std::unsigned_integral<value_type>
  {
    return named_type{~m_value};
  }

  /// \brief performs binary AND on the contained values
  constexpr auto operator&(const named_type& rhs) const& noexcept(noexcept(m_value & rhs.m_value)) -> named_type
    requires std::unsigned_integral<value_type>
  {
    return named_type{m_value & rhs.m_value};
  }

  /// \brief performs binary AND on the contained values
  constexpr auto operator&(named_type&& rhs) const& noexcept(noexcept(m_value & rhs.m_value)) -> named_type
    requires std::unsigned_integral<value_type>
  {
    return named_type{m_value & rhs.m_value};
  }

  /// \brief performs binary AND on the contained values
  constexpr auto operator&(const named_type& rhs) && noexcept(noexcept(m_value & rhs.m_value)) -> named_type
    requires std::unsigned_integral<value_type>
  {
    return named_type{m_value & rhs.m_value};
  }

  /// \brief performs binary AND on the contained values
  constexpr auto operator&(named_type&& rhs) && noexcept(noexcept(m_value & rhs.m_value)) -> named_type
    requires std::unsigned_integral<value_type>
  {
    return named_type{m_value & rhs.m_value};
  }

  /// \brief performs binary OR on the contained values
  constexpr auto operator|(const named_type& rhs) const& noexcept(noexcept(m_value | rhs.m_value)) -> named_type
    requires std::unsigned_integral<value_type>
  {
    return named_type{m_value | rhs.m_value};
  }

  /// \brief performs binary OR on the contained values
  constexpr auto operator|(named_type&& rhs) const& noexcept(noexcept(m_value | rhs.m_value)) -> named_type
    requires std::unsigned_integral<value_type>
  {
    return named_type{m_value | rhs.m_value};
  }

  /// \brief performs binary OR on the contained values
  constexpr auto operator|(const named_type& rhs) && noexcept(noexcept(m_value | rhs.m_value)) -> named_type
    requires std::unsigned_integral<value_type>
  {
    return named_type{m_value | rhs.m_value};
  }

  /// \brief performs binary OR on the contained values
  constexpr auto operator|(named_type&& rhs) && noexcept(noexcept(m_value | rhs.m_value)) -> named_type
    requires std::unsigned_integral<value_type>
  {
    return named_type{m_value | rhs.m_value};
  }

  /// \brief performs binary XOR on the contained values
  constexpr auto operator^(const named_type& rhs) const& noexcept(noexcept(m_value ^ rhs.m_value)) -> named_type
    requires std::unsigned_integral<value_type>
  {
    return named_type{m_value ^ rhs.m_value};
  }

  /// \brief performs binary XOR on the contained values
  constexpr auto operator^(named_type&& rhs) const& noexcept(noexcept(m_value ^ rhs.m_value)) -> named_type
    requires std::unsigned_integral<value_type>
  {
    return named_type{m_value ^ rhs.m_value};
  }

  /// \brief performs binary XOR on the contained values
  constexpr auto operator^(const named_type& rhs) && noexcept(noexcept(m_value ^ rhs.m_value)) -> named_type
    requires std::unsigned_integral<value_type>
  {
    return named_type{m_value ^ rhs.m_value};
  }

  /// \brief performs binary XOR on the contained values
  constexpr auto operator^(named_type&& rhs) && noexcept(noexcept(m_value ^ rhs.m_value)) -> named_type
    requires std::unsigned_integral<value_type>
  {
    return named_type{m_value ^ rhs.m_value};
  }

  /// \brief performs binary left shift on the contained values
  constexpr auto operator<<(const named_type& rhs) const& noexcept(noexcept(m_value << rhs.m_value)) -> named_type
    requires std::unsigned_integral<value_type>
  {
    return named_type{m_value << rhs.m_value};
  }

  /// \brief performs binary left shift on the contained values
  constexpr auto operator<<(named_type&& rhs) const& noexcept(noexcept(m_value << rhs.m_value)) -> named_type
    requires std::unsigned_integral<value_type>
  {
    return named_type{m_value << rhs.m_value};
  }

  /// \brief performs binary left shift on the contained values
  constexpr auto operator<<(const named_type& rhs) && noexcept(noexcept(m_value << rhs.m_value)) -> named_type
    requires std::unsigned_integral<value_type>
  {
    return named_type{m_value << rhs.m_value};
  }

  /// \brief performs binary left shift on the contained values
  constexpr auto operator<<(named_type&& rhs) && noexcept(noexcept(m_value << rhs.m_value)) -> named_type
    requires std::unsigned_integral<value_type>
  {
    return named_type{m_value << rhs.m_value};
  }

  /// \brief performs binary right shift on the contained values
  constexpr auto operator>>(const named_type& rhs) const& noexcept(noexcept(m_value >> rhs.m_value)) -> named_type
    requires std::unsigned_integral<value_type>
  {
    return named_type{m_value >> rhs.m_value};
  }

  /// \brief performs binary right shift on the contained values
  constexpr auto operator>>(named_type&& rhs) const& noexcept(noexcept(m_value >> rhs.m_value)) -> named_type
    requires std::unsigned_integral<value_type>
  {
    return named_type{m_value >> rhs.m_value};
  }

  /// \brief performs binary right shift on the contained values
  constexpr auto operator>>(const named_type& rhs) && noexcept(noexcept(m_value >> rhs.m_value)) -> named_type
    requires std::unsigned_integral<value_type>
  {
    return named_type{m_value >> rhs.m_value};
  }

  /// \brief performs binary right shift on the contained values
  constexpr auto operator>>(named_type&& rhs) && noexcept(noexcept(m_value >> rhs.m_value)) -> named_type
    requires std::unsigned_integral<value_type>
  {
    return named_type{m_value >> rhs.m_value};
  }

  /// \brief performs binary AND on the contained values and assigns the result
  constexpr auto operator&=(const named_type& rhs) & noexcept(noexcept(m_value &= rhs.m_value)) -> named_type&
    requires std::unsigned_integral<value_type>
  {
    m_value &= rhs.m_value;
    return *this;
  }

  /// \brief performs binary AND on the contained values and assigns the result
  constexpr auto operator&=(named_type&& rhs) & noexcept(noexcept(m_value &= rhs.m_value)) -> named_type&
    requires std::unsigned_integral<value_type>
  {
    m_value &= rhs.m_value;
    return *this;
  }

  /// \brief performs binary OR on the contained values and assigns the result
  constexpr auto operator|=(const named_type& rhs) & noexcept(noexcept(m_value |= rhs.m_value)) -> named_type&
    requires std::unsigned_integral<value_type>
  {
    m_value |= rhs.m_value;
    return *this;
  }

  /// \brief performs binary OR on the contained values and assigns the result
  constexpr auto operator|=(named_type&& rhs) & noexcept(noexcept(m_value |= rhs.m_value)) -> named_type&
    requires std::unsigned_integral<value_type>
  {
    m_value |= rhs.m_value;
    return *this;
  }

  /// \brief performs binary XOR on the contained values and assigns the result
  constexpr auto operator^=(const named_type& rhs) & noexcept(noexcept(m_value ^= rhs.m_value)) -> named_type&
    requires std::unsigned_integral<value_type>
  {
    m_value ^= rhs.m_value;
    return *this;
  }

  /// \brief performs binary XOR on the contained values and assigns the result
  constexpr auto operator^=(named_type&& rhs) & noexcept(noexcept(m_value ^= rhs.m_value)) -> named_type&
    requires std::unsigned_integral<value_type>
  {
    m_value ^= rhs.m_value;
    return *this;
  }

  /// \brief performs binary left shift on the contained values and assigns the result
  constexpr auto operator<<=(const named_type& rhs) & noexcept(noexcept(m_value <<= rhs.m_value)) -> named_type&
    requires std::unsigned_integral<value_type>
  {
    m_value <<= rhs.m_value;
    return *this;
  }

  /// \brief performs binary left shift on the contained values and assigns the result
  constexpr auto operator<<=(named_type&& rhs) & noexcept(noexcept(m_value <<= rhs.m_value)) -> named_type&
    requires std::unsigned_integral<value_type>
  {
    m_value <<= rhs.m_value;
    return *this;
  }

  /// \brief performs binary right shift on the contained values and assigns the result
  constexpr auto operator>>=(const named_type& rhs) & noexcept(noexcept(m_value >>= rhs.m_value)) -> named_type&
    requires std::unsigned_integral<value_type>
  {
    m_value >>= rhs.m_value;
    return *this;
  }

  /// \brief performs binary right shift on the contained values and assigns the result
  constexpr auto operator>>=(named_type&& rhs) & noexcept(noexcept(m_value >>= rhs.m_value)) -> named_type&
    requires std::unsigned_integral<value_type>
  {
    m_value >>= rhs.m_value;
    return *this;
  }

  //
  // Ranges interface
  //

  /// \brief returns an iterator to the beginning of the contained value
  constexpr auto begin() const noexcept(noexcept(std::ranges::begin(m_value)))
    requires std::ranges::range<value_type>
  {
    return std::ranges::begin(m_value);
  }

  /// \brief returns an iterator to the beginning of the contained value
  constexpr auto begin() noexcept(noexcept(std::ranges::begin(m_value)))
    requires std::ranges::range<value_type>
  {
    return std::ranges::begin(m_value);
  }

  /// \brief returns an iterator to the end of the contained value
  constexpr auto end() const noexcept(noexcept(std::ranges::end(m_value)))
    requires std::ranges::range<value_type>
  {
    return std::ranges::end(m_value);
  }

  /// \brief returns an iterator to the end of the contained value
  constexpr auto end() noexcept(noexcept(std::ranges::end(m_value)))
    requires std::ranges::range<value_type>
  {
    return std::ranges::end(m_value);
  }

  //
  // Stream operators
  //

  /// \brief inserts formatted data
  friend inline auto operator<<(std::ostream& ostream,
                                const named_type& rhs) noexcept(noexcept(ostream << rhs.m_value)) -> std::ostream&
    requires ostreamable<value_type>
  {
    return ostream << rhs.m_value;
  }

  /// \brief extracts formatted data
  friend inline auto operator>>(std::istream& istream,
                                named_type& rhs) noexcept(noexcept(istream >> rhs.m_value)) -> std::istream&
    requires istreamable<value_type>
  {
    return istream >> rhs.m_value;
  }

 private:
  value_type m_value{};
  static constexpr auto k_name = Name.value;
};

//
// Creation functions
//

/// \brief creates a gw::named_type object
template <detail::fixed_string Name, typename T, typename... Args>
constexpr auto make_named_type(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
  requires std::constructible_from<std::remove_cvref_t<T>, Args...>
{
  return named_type<std::remove_cvref_t<T>, Name>{std::forward<Args>(args)...};
}

/// \brief creates a gw::named_type object
template <detail::fixed_string Name, typename T, typename U, typename... Args>
constexpr auto make_named_type(std::initializer_list<U> ilist, Args&&... args) noexcept(
    std::is_nothrow_constructible_v<T, std::initializer_list<U>&, Args...>)
  requires std::constructible_from<T, std::initializer_list<U>&, Args...>
{
  return named_type<std::remove_cvref_t<T>, Name>{ilist, std::forward<Args>(args)...};
}

/// \brief creates a gw::named_type object
template <detail::fixed_string Name, typename T>
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

/// \brief Hash support for `gw::named_type`.
///
template <::gw::hashable T, ::gw::detail::fixed_string Name>
// NOLINTNEXTLINE(cert-dcl58-cpp)
struct hash<::gw::named_type<T, Name>> {
  /// \brief Calculate the hash of the `gw::named_type` object.
  ///
  [[nodiscard]] auto inline operator()(const ::gw::named_type<T, Name>& named_type) const noexcept -> size_t {
    auto value_hash = hash<T>{}(named_type.value());
    auto name_hash = hash<string_view>{}(named_type.name());
    return value_hash ^ name_hash;
  }
};

//
// String conversion
//

/// \brief Format the `gw::named_type` object.
///
template <typename T, ::gw::detail::fixed_string Name, class CharT>
// NOLINTNEXTLINE(cert-dcl58-cpp)
struct formatter<::gw::named_type<T, Name>, CharT> {
  bool with_name{};  ///< Include the name in the output.

  /// \brief Parse the format string.
  ///
  template <class ParseContext>
  constexpr auto parse(ParseContext& context) -> ParseContext::iterator {
    auto it = context.begin();
    if (it == context.end()) {
      return it;
    }

    if (*it == '#') {
      with_name = true;
      ++it;
    }

    if (it != context.end() && *it != '}') {
      throw std::format_error("Invalid format args for gw::named_type.");
    }

    return it;
  }

  /// \brief Format the `gw::named_type` object.
  ///
  template <class FormatContext>
  auto format(const ::gw::named_type<T, Name>& named_type, FormatContext& context) const -> FormatContext::iterator
    requires std::formattable<T, CharT>
  {
    if (with_name) {
      return format_to(context.out(), "{}: {}", named_type.name(), named_type.value());
    }

    return format_to(context.out(), "{}", named_type.value());
  }
};

}  // namespace std
