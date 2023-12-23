// Copyright (c) 2023 Martin Stump
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <algorithm>
#include <compare>
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

/// \brief a named type wrapper
template <typename T, detail::fixed_string Name>
class named_type final
    : public std::conditional_t<std::ranges::range<T>, std::ranges::view_interface<named_type<T, Name>>,
                                detail::named_type_empty_base> {
 public:
  //
  // Public types
  //

  /// \brief the type of the contained value
  using value_type = T;

  //
  // Constructors
  //

  /// \brief constructs the gw::named_type object
  template <typename... Args>
  constexpr explicit named_type(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
    requires std::constructible_from<T, Args...>
      : m_value(T{std::forward<Args>(args)...}) {}

  /// \brief constructs the gw::named_type object
  template <typename U, typename... Args>
  constexpr named_type(std::initializer_list<U> ilist,
                       Args&&... args) noexcept(std::is_nothrow_constructible_v<T, std::initializer_list<U>&, Args...>)
    requires std::constructible_from<T, std::initializer_list<U>&, Args...>
      : m_value(T{ilist, std::forward<Args>(args)...}) {}

  //
  // Destructor
  //

  /// \brief destroys the contained value
  ~named_type() noexcept(std::is_nothrow_destructible_v<T>) = default;

  //
  // Static functions
  //

  /// \brief returns the name of the gw::named_type
  static constexpr auto name() noexcept -> std::string_view { return k_name; }

  //
  // Observers
  //

  /// \brief accesses the contained value
  constexpr auto operator->() const noexcept -> const T* { return &m_value; }

  /// \brief accesses the contained value
  constexpr auto operator->() noexcept -> T* { return &m_value; }

  /// \brief accesses the contained value
  constexpr auto operator*() const& noexcept -> const T& { return m_value; }

  /// \brief accesses the contained value
  constexpr auto operator*() & noexcept -> T& { return m_value; }

  /// \brief accesses the contained value
  constexpr auto operator*() const&& noexcept -> const T&& { return std::move(m_value); }

  /// \brief accesses the contained value
  constexpr auto operator*() && noexcept -> T&& { return std::move(m_value); }

  /// \brief returns the contained value
  constexpr auto value() const& noexcept -> const T& { return m_value; }

  /// \brief returns the contained value
  constexpr auto value() & noexcept -> T& { return m_value; }

  /// \brief returns the contained value
  constexpr auto value() const&& noexcept -> const T&& { return std::move(m_value); }

  /// \brief returns the contained value
  constexpr auto value() && noexcept -> T&& { return std::move(m_value); }

  //
  // Monadic operations
  //

  /// \brief returns a gw::named_type containing the transformed contained value
  template <typename F>
  constexpr auto transform(F&& func) const& noexcept(noexcept(func(m_value)))
    requires std::invocable<F, const T&>
  {
    return named_type<std::remove_cv_t<std::invoke_result_t<F, const T&>>, Name>{func(m_value)};
  }

  /// \brief returns a gw::named_type containing the transformed contained value
  template <typename F>
  constexpr auto transform(F&& func) & noexcept(noexcept(func(m_value)))
    requires std::invocable<F, T&>
  {
    return named_type<std::remove_cv_t<std::invoke_result_t<F, T&>>, Name>{func(m_value)};
  }

  /// \brief returns a gw::named_type containing the transformed contained value
  template <typename F>
  constexpr auto transform(F&& func) const&& noexcept(noexcept(func(m_value)))
    requires std::invocable<F, const T&&>
  {
    return named_type<std::remove_cv_t<std::invoke_result_t<F, const T&&>>, Name>{func(std::move(m_value))};
  }

  /// \brief returns a gw::named_type containing the transformed contained value
  template <typename F>
  constexpr auto transform(F&& func) && noexcept(noexcept(func(m_value)))
    requires std::invocable<F, T&&>
  {
    return named_type<std::remove_cv_t<std::invoke_result_t<F, T&&>>, Name>{func(std::move(m_value))};
  }

  //
  // Modifiers
  //

  /// \brief specializes the std::swap algorithm
  constexpr void swap(named_type& rhs) noexcept(std::is_nothrow_swappable_v<T>)
    requires std::swappable<T>
  {
    using std::swap;
    swap(m_value, rhs.m_value);
  }

  /// \brief destroys any contained value
  constexpr void reset() noexcept(std::is_nothrow_default_constructible_v<T>)
    requires std::default_initializable<T>
  {
    m_value = T{};
  }

  /// \brief constructs the contained value in-place
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

  /// \brief compares gw::named_type objects
  constexpr auto operator==(const named_type& rhs) const& noexcept(noexcept(m_value == rhs.m_value)) -> bool
    requires std::equality_comparable<T>
  {
    return m_value == rhs.m_value;
  }

  /// \brief compares gw::named_type objects
  constexpr auto operator!=(const named_type& rhs) const& noexcept(noexcept(m_value != rhs.m_value)) -> bool
    requires std::equality_comparable<T>
  {
    return m_value != rhs.m_value;
  }

  /// \brief compares gw::named_type objects
  constexpr auto operator<(const named_type& rhs) const& noexcept(noexcept(m_value < rhs.m_value)) -> bool
    requires std::totally_ordered<T>
  {
    return m_value < rhs.m_value;
  }

  /// \brief compares gw::named_type objects
  constexpr auto operator>(const named_type& rhs) const& noexcept(noexcept(m_value > rhs.m_value)) -> bool
    requires std::totally_ordered<T>
  {
    return m_value > rhs.m_value;
  }

  /// \brief compares gw::named_type objects
  constexpr auto operator<=(const named_type& rhs) const& noexcept(noexcept(m_value <= rhs.m_value)) -> bool
    requires std::totally_ordered<T>
  {
    return m_value <= rhs.m_value;
  }

  /// \brief compares gw::named_type objects
  constexpr auto operator>=(const named_type& rhs) const& noexcept(noexcept(m_value >= rhs.m_value)) -> bool
    requires std::totally_ordered<T>
  {
    return m_value >= rhs.m_value;
  }

  /// \brief compares gw::named_type objects
  constexpr auto operator<=>(const named_type& rhs) const& noexcept(noexcept(m_value <=>
                                                                             rhs.m_value)) -> std::strong_ordering
    requires std::three_way_comparable<T>
  {
    return m_value <=> rhs.m_value;
  }

  //
  // Conversion operators
  //

  /// \brief converts the gw::named_type to its underlying type
  constexpr explicit operator const T&() const& noexcept { return m_value; }

  /// \brief converts the gw::named_type to its underlying type
  constexpr explicit operator T&() & noexcept { return m_value; }

  /// \brief converts the gw::named_type to its underlying type
  constexpr explicit operator const T&&() const&& noexcept { return std::move(m_value); }

  /// \brief converts the gw::named_type to its underlying type
  constexpr explicit operator T&&() && noexcept { return std::move(m_value); }

  //
  // Increment and decrement operators
  //

  /// \brief increments the contained value
  constexpr auto operator++() & noexcept(noexcept(++m_value)) -> named_type&
    requires incrementable<T>
  {
    ++m_value;
    return *this;
  }

  /// \brief increments the contained value
  constexpr auto operator++() && noexcept(noexcept(++m_value)) -> named_type&&
    requires incrementable<T>
  {
    ++m_value;
    return std::move(*this);
  }

  /// \brief increments the contained value
  constexpr auto operator++(int) & noexcept(noexcept(m_value++)) -> named_type
    requires incrementable<T>
  {
    return named_type{m_value++};
  }

  /// \brief increments the contained value
  constexpr auto operator++(int) && noexcept(noexcept(m_value++)) -> named_type
    requires incrementable<T>
  {
    return named_type{m_value++};
  }

  /// \brief decrements the contained value
  constexpr auto operator--() & noexcept(noexcept(--m_value)) -> named_type&
    requires decrementable<T>
  {
    --m_value;
    return *this;
  }

  /// \brief decrements the contained value
  constexpr auto operator--() && noexcept(noexcept(--m_value)) -> named_type&&
    requires decrementable<T>
  {
    --m_value;
    return std::move(*this);
  }

  /// \brief decrements the contained value
  constexpr auto operator--(int) & noexcept(noexcept(m_value--)) -> named_type
    requires decrementable<T>
  {
    return named_type{m_value--};
  }

  /// \brief decrements the contained value
  constexpr auto operator--(int) && noexcept(noexcept(m_value--)) -> named_type
    requires decrementable<T>
  {
    return named_type{m_value--};
  }

  //
  // Arithmetic operators
  //

  /// \brief affirms the contained value
  constexpr auto operator+() const& noexcept(noexcept(+m_value)) -> named_type
    requires std::signed_integral<T>
  {
    return named_type{+m_value};
  }

  /// \brief affirms the contained value
  constexpr auto operator+() && noexcept(noexcept(+m_value)) -> named_type
    requires std::signed_integral<T>
  {
    return named_type{+m_value};
  }

  /// \brief negates the contained value
  constexpr auto operator-() const& noexcept(noexcept(-m_value)) -> named_type
    requires std::signed_integral<T>
  {
    return named_type{-m_value};
  }

  /// \brief negates the contained value
  constexpr auto operator-() && noexcept(noexcept(-m_value)) -> named_type
    requires std::signed_integral<T>
  {
    return named_type{-m_value};
  }

  /// \brief adds the contained values
  constexpr auto operator+(const named_type& rhs) const& noexcept(noexcept(m_value + rhs.m_value)) -> named_type
    requires arithmetic<T>
  {
    return named_type{m_value + rhs.m_value};
  }

  /// \brief adds the contained values
  constexpr auto operator+(named_type&& rhs) const& noexcept(noexcept(m_value + rhs.m_value)) -> named_type
    requires arithmetic<T>
  {
    return named_type{m_value + rhs.m_value};
  }

  /// \brief adds the contained values
  constexpr auto operator+(const named_type& rhs) && noexcept(noexcept(m_value + rhs.m_value)) -> named_type
    requires arithmetic<T>
  {
    return named_type{m_value + rhs.m_value};
  }

  /// \brief adds the contained values
  constexpr auto operator+(named_type&& rhs) && noexcept(noexcept(m_value + rhs.m_value)) -> named_type
    requires arithmetic<T>
  {
    return named_type{m_value + rhs.m_value};
  }

  /// \brief subtracts the contained values
  constexpr auto operator-(const named_type& rhs) const& noexcept(noexcept(m_value - rhs.m_value)) -> named_type
    requires arithmetic<T>
  {
    return named_type{m_value - rhs.m_value};
  }

  /// \brief subtracts the contained values
  constexpr auto operator-(named_type&& rhs) const& noexcept(noexcept(m_value - rhs.m_value)) -> named_type
    requires arithmetic<T>
  {
    return named_type{m_value - rhs.m_value};
  }

  /// \brief subtracts the contained values
  constexpr auto operator-(const named_type& rhs) && noexcept(noexcept(m_value - rhs.m_value)) -> named_type
    requires arithmetic<T>
  {
    return named_type{m_value - rhs.m_value};
  }

  /// \brief subtracts the contained values
  constexpr auto operator-(named_type&& rhs) && noexcept(noexcept(m_value - rhs.m_value)) -> named_type
    requires arithmetic<T>
  {
    return named_type{m_value - rhs.m_value};
  }

  /// \brief multiplies the contained values
  constexpr auto operator*(const named_type& rhs) const& noexcept(noexcept(m_value * rhs.m_value)) -> named_type
    requires arithmetic<T>
  {
    return named_type{m_value * rhs.m_value};
  }

  /// \brief multiplies the contained values
  constexpr auto operator*(named_type&& rhs) const& noexcept(noexcept(m_value * rhs.m_value)) -> named_type
    requires arithmetic<T>
  {
    return named_type{m_value * rhs.m_value};
  }

  /// \brief multiplies the contained values
  constexpr auto operator*(const named_type& rhs) && noexcept(noexcept(m_value * rhs.m_value)) -> named_type
    requires arithmetic<T>
  {
    return named_type{m_value * rhs.m_value};
  }

  /// \brief multiplies the contained values
  constexpr auto operator*(named_type&& rhs) && noexcept(noexcept(m_value * rhs.m_value)) -> named_type
    requires arithmetic<T>
  {
    return named_type{m_value * rhs.m_value};
  }

  /// \brief devides the contained values
  constexpr auto operator/(const named_type& rhs) const& noexcept(noexcept(m_value / rhs.m_value)) -> named_type
    requires arithmetic<T>
  {
    return named_type{m_value / rhs.m_value};
  }

  /// \brief devides the contained values
  constexpr auto operator/(named_type&& rhs) const& noexcept(noexcept(m_value / rhs.m_value)) -> named_type
    requires arithmetic<T>
  {
    return named_type{m_value / rhs.m_value};
  }

  /// \brief devides the contained values
  constexpr auto operator/(const named_type& rhs) && noexcept(noexcept(m_value / rhs.m_value)) -> named_type
    requires arithmetic<T>
  {
    return named_type{m_value / rhs.m_value};
  }

  /// \brief devides the contained values
  constexpr auto operator/(named_type&& rhs) && noexcept(noexcept(m_value / rhs.m_value)) -> named_type
    requires arithmetic<T>
  {
    return named_type{m_value / rhs.m_value};
  }

  /// \brief calculates the remainder of the contained values
  constexpr auto operator%(const named_type& rhs) const& noexcept(noexcept(m_value % rhs.m_value)) -> named_type
    requires arithmetic<T>
  {
    return named_type{m_value % rhs.m_value};
  }

  /// \brief calculates the remainder of the contained values
  constexpr auto operator%(named_type&& rhs) const& noexcept(noexcept(m_value % rhs.m_value)) -> named_type
    requires arithmetic<T>
  {
    return named_type{m_value % rhs.m_value};
  }

  /// \brief calculates the remainder of the contained values
  constexpr auto operator%(const named_type& rhs) && noexcept(noexcept(m_value % rhs.m_value)) -> named_type
    requires arithmetic<T>
  {
    return named_type{m_value % rhs.m_value};
  }

  /// \brief calculates the remainder of the contained values
  constexpr auto operator%(named_type&& rhs) && noexcept(noexcept(m_value % rhs.m_value)) -> named_type
    requires arithmetic<T>
  {
    return named_type{m_value % rhs.m_value};
  }

  /// \brief adds the contained values and assigns the result
  constexpr auto operator+=(const named_type& rhs) & noexcept(noexcept(m_value += rhs.m_value)) -> named_type&
    requires arithmetic<T>
  {
    m_value += rhs.m_value;
    return *this;
  }

  /// \brief adds the contained values and assigns the result
  constexpr auto operator+=(named_type&& rhs) & noexcept(noexcept(m_value += rhs.m_value)) -> named_type&
    requires arithmetic<T>
  {
    m_value += rhs.m_value;
    return *this;
  }

  /// \brief subtracts the contained values and assigns the result
  constexpr auto operator-=(const named_type& rhs) & noexcept(noexcept(m_value -= rhs.m_value)) -> named_type&
    requires arithmetic<T>
  {
    m_value -= rhs.m_value;
    return *this;
  }

  /// \brief subtracts the contained values and assigns the result
  constexpr auto operator-=(named_type&& rhs) & noexcept(noexcept(m_value -= rhs.m_value)) -> named_type&
    requires arithmetic<T>
  {
    m_value -= rhs.m_value;
    return *this;
  }

  /// \brief multiplies the contained values and assigns the result
  constexpr auto operator*=(const named_type& rhs) & noexcept(noexcept(m_value *= rhs.m_value)) -> named_type&
    requires arithmetic<T>
  {
    m_value *= rhs.m_value;
    return *this;
  }

  /// \brief multiplies the contained values and assigns the result
  constexpr auto operator*=(named_type&& rhs) & noexcept(noexcept(m_value *= rhs.m_value)) -> named_type&
    requires arithmetic<T>
  {
    m_value *= rhs.m_value;
    return *this;
  }

  /// \brief devides the contained values and assigns the result
  constexpr auto operator/=(const named_type& rhs) & noexcept(noexcept(m_value /= rhs.m_value)) -> named_type&
    requires arithmetic<T>
  {
    m_value /= rhs.m_value;
    return *this;
  }

  /// \brief devides the contained values and assigns the result
  constexpr auto operator/=(named_type&& rhs) & noexcept(noexcept(m_value /= rhs.m_value)) -> named_type&
    requires arithmetic<T>
  {
    m_value /= rhs.m_value;
    return *this;
  }

  /// \brief calculates the remainder of the contained values and assigns the result
  constexpr auto operator%=(const named_type& rhs) & noexcept(noexcept(m_value %= rhs.m_value)) -> named_type&
    requires arithmetic<T>
  {
    m_value %= rhs.m_value;
    return *this;
  }

  /// \brief calculates the remainder of the contained values and assigns the result
  constexpr auto operator%=(named_type&& rhs) & noexcept(noexcept(m_value %= rhs.m_value)) -> named_type&
    requires arithmetic<T>
  {
    m_value %= rhs.m_value;
    return *this;
  }

  //
  // Bitwise operators
  //

  /// \brief inverts the contained value
  constexpr auto operator~() const& noexcept(noexcept(~m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{~m_value};
  }

  /// \brief performs binary AND on the contained values
  constexpr auto operator&(const named_type& rhs) const& noexcept(noexcept(m_value & rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value & rhs.m_value};
  }

  /// \brief performs binary AND on the contained values
  constexpr auto operator&(named_type&& rhs) const& noexcept(noexcept(m_value & rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value & rhs.m_value};
  }

  /// \brief performs binary AND on the contained values
  constexpr auto operator&(const named_type& rhs) && noexcept(noexcept(m_value & rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value & rhs.m_value};
  }

  /// \brief performs binary AND on the contained values
  constexpr auto operator&(named_type&& rhs) && noexcept(noexcept(m_value & rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value & rhs.m_value};
  }

  /// \brief performs binary OR on the contained values
  constexpr auto operator|(const named_type& rhs) const& noexcept(noexcept(m_value | rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value | rhs.m_value};
  }

  /// \brief performs binary OR on the contained values
  constexpr auto operator|(named_type&& rhs) const& noexcept(noexcept(m_value | rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value | rhs.m_value};
  }

  /// \brief performs binary OR on the contained values
  constexpr auto operator|(const named_type& rhs) && noexcept(noexcept(m_value | rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value | rhs.m_value};
  }

  /// \brief performs binary OR on the contained values
  constexpr auto operator|(named_type&& rhs) && noexcept(noexcept(m_value | rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value | rhs.m_value};
  }

  /// \brief performs binary XOR on the contained values
  constexpr auto operator^(const named_type& rhs) const& noexcept(noexcept(m_value ^ rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value ^ rhs.m_value};
  }

  /// \brief performs binary XOR on the contained values
  constexpr auto operator^(named_type&& rhs) const& noexcept(noexcept(m_value ^ rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value ^ rhs.m_value};
  }

  /// \brief performs binary XOR on the contained values
  constexpr auto operator^(const named_type& rhs) && noexcept(noexcept(m_value ^ rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value ^ rhs.m_value};
  }

  /// \brief performs binary XOR on the contained values
  constexpr auto operator^(named_type&& rhs) && noexcept(noexcept(m_value ^ rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value ^ rhs.m_value};
  }

  /// \brief performs binary left shift on the contained values
  constexpr auto operator<<(const named_type& rhs) const& noexcept(noexcept(m_value << rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value << rhs.m_value};
  }

  /// \brief performs binary left shift on the contained values
  constexpr auto operator<<(named_type&& rhs) const& noexcept(noexcept(m_value << rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value << rhs.m_value};
  }

  /// \brief performs binary left shift on the contained values
  constexpr auto operator<<(const named_type& rhs) && noexcept(noexcept(m_value << rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value << rhs.m_value};
  }

  /// \brief performs binary left shift on the contained values
  constexpr auto operator<<(named_type&& rhs) && noexcept(noexcept(m_value << rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value << rhs.m_value};
  }

  /// \brief performs binary right shift on the contained values
  constexpr auto operator>>(const named_type& rhs) const& noexcept(noexcept(m_value >> rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value >> rhs.m_value};
  }

  /// \brief performs binary right shift on the contained values
  constexpr auto operator>>(named_type&& rhs) const& noexcept(noexcept(m_value >> rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value >> rhs.m_value};
  }

  /// \brief performs binary right shift on the contained values
  constexpr auto operator>>(const named_type& rhs) && noexcept(noexcept(m_value >> rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value >> rhs.m_value};
  }

  /// \brief performs binary right shift on the contained values
  constexpr auto operator>>(named_type&& rhs) && noexcept(noexcept(m_value >> rhs.m_value)) -> named_type
    requires std::unsigned_integral<T>
  {
    return named_type{m_value >> rhs.m_value};
  }

  /// \brief performs binary AND on the contained values and assigns the result
  constexpr auto operator&=(const named_type& rhs) & noexcept(noexcept(m_value &= rhs.m_value)) -> named_type&
    requires std::unsigned_integral<T>
  {
    m_value &= rhs.m_value;
    return *this;
  }

  /// \brief performs binary AND on the contained values and assigns the result
  constexpr auto operator&=(named_type&& rhs) & noexcept(noexcept(m_value &= rhs.m_value)) -> named_type&
    requires std::unsigned_integral<T>
  {
    m_value &= rhs.m_value;
    return *this;
  }

  /// \brief performs binary OR on the contained values and assigns the result
  constexpr auto operator|=(const named_type& rhs) & noexcept(noexcept(m_value |= rhs.m_value)) -> named_type&
    requires std::unsigned_integral<T>
  {
    m_value |= rhs.m_value;
    return *this;
  }

  /// \brief performs binary OR on the contained values and assigns the result
  constexpr auto operator|=(named_type&& rhs) & noexcept(noexcept(m_value |= rhs.m_value)) -> named_type&
    requires std::unsigned_integral<T>
  {
    m_value |= rhs.m_value;
    return *this;
  }

  /// \brief performs binary XOR on the contained values and assigns the result
  constexpr auto operator^=(const named_type& rhs) & noexcept(noexcept(m_value ^= rhs.m_value)) -> named_type&
    requires std::unsigned_integral<T>
  {
    m_value ^= rhs.m_value;
    return *this;
  }

  /// \brief performs binary XOR on the contained values and assigns the result
  constexpr auto operator^=(named_type&& rhs) & noexcept(noexcept(m_value ^= rhs.m_value)) -> named_type&
    requires std::unsigned_integral<T>
  {
    m_value ^= rhs.m_value;
    return *this;
  }

  /// \brief performs binary left shift on the contained values and assigns the result
  constexpr auto operator<<=(const named_type& rhs) & noexcept(noexcept(m_value <<= rhs.m_value)) -> named_type&
    requires std::unsigned_integral<T>
  {
    m_value <<= rhs.m_value;
    return *this;
  }

  /// \brief performs binary left shift on the contained values and assigns the result
  constexpr auto operator<<=(named_type&& rhs) & noexcept(noexcept(m_value <<= rhs.m_value)) -> named_type&
    requires std::unsigned_integral<T>
  {
    m_value <<= rhs.m_value;
    return *this;
  }

  /// \brief performs binary right shift on the contained values and assigns the result
  constexpr auto operator>>=(const named_type& rhs) & noexcept(noexcept(m_value >>= rhs.m_value)) -> named_type&
    requires std::unsigned_integral<T>
  {
    m_value >>= rhs.m_value;
    return *this;
  }

  /// \brief performs binary right shift on the contained values and assigns the result
  constexpr auto operator>>=(named_type&& rhs) & noexcept(noexcept(m_value >>= rhs.m_value)) -> named_type&
    requires std::unsigned_integral<T>
  {
    m_value >>= rhs.m_value;
    return *this;
  }

  //
  // Ranges interface
  //

  /// \brief returns an iterator to the beginning of the contained value
  constexpr auto begin() const noexcept(noexcept(std::ranges::begin(m_value)))
    requires std::ranges::range<T>
  {
    return std::ranges::begin(m_value);
  }

  /// \brief returns an iterator to the beginning of the contained value
  constexpr auto begin() noexcept(noexcept(std::ranges::begin(m_value)))
    requires std::ranges::range<T>
  {
    return std::ranges::begin(m_value);
  }

  /// \brief returns an iterator to the end of the contained value
  constexpr auto end() const noexcept(noexcept(std::ranges::end(m_value)))
    requires std::ranges::range<T>
  {
    return std::ranges::end(m_value);
  }

  /// \brief returns an iterator to the end of the contained value
  constexpr auto end() noexcept(noexcept(std::ranges::end(m_value)))
    requires std::ranges::range<T>
  {
    return std::ranges::end(m_value);
  }

  //
  // Stream operators
  //

  /// \brief inserts formatted data
  friend inline auto operator<<(std::ostream& ostream,
                                const named_type& rhs) noexcept(noexcept(ostream << rhs.m_value)) -> std::ostream&
    requires ostreamable<T>
  {
    return ostream << rhs.m_value;
  }

  /// \brief extracts formatted data
  friend inline auto operator>>(std::istream& istream,
                                named_type& rhs) noexcept(noexcept(istream >> rhs.m_value)) -> std::istream&
    requires istreamable<T>
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

/// \brief hash support for gw::named_type
template <::gw::hashable T, ::gw::detail::fixed_string Name>
// NOLINTNEXTLINE(cert-dcl58-cpp)
struct hash<::gw::named_type<T, Name>> {
  [[nodiscard]] auto inline operator()(const ::gw::named_type<T, Name>& named_type) const noexcept -> size_t {
    auto value_hash = hash<T>{}(named_type.value());
    auto name_hash = hash<string_view>{}(named_type.name());
    return value_hash ^ name_hash;
  }
};

//
// String conversion
//

/// \brief string conversion support for gw::named_type
template <::gw::string_convertable T, ::gw::detail::fixed_string Name>
// NOLINTNEXTLINE(cert-dcl58-cpp)
[[nodiscard]] auto inline to_string(const ::gw::named_type<T, Name>& named_type) -> string {
  return string{named_type.name()} + ": " + to_string(named_type.value());
}

}  // namespace std
