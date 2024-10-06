// Copyright (c) 2023 Martin Stump
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <compare>
#include <concepts>
#include <cstddef>
#include <format>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <ranges>
#include <string>
#include <type_traits>
#include <typeindex>
#include <utility>

#include "gw/concepts.hpp"

/// \brief GW namespace
namespace gw {

namespace detail {

struct strong_type_empty_base {
  constexpr auto operator<=>(const strong_type_empty_base&) const noexcept -> std::strong_ordering = default;
};

}  // namespace detail

/// \example strong_type_example.cpp
//
/// \brief Strong type wrapper.
//
/// \details The class template `gw::strong_type` is a simple wrapper around a value of some type `T`. It is intended to
/// be used to create a new type that is distinct from `T`, but which has the same representation and semantics as `T`.
/// This is useful to distinguish between values that are conceptually different, but which have the same underlying
/// type. For example, a `std::string` can be used to represent a person's name, but it can also be used to represent a
/// person's address. Using `gw::strong_type` to create a `name_t` and an `address_t` allows the compiler to catch
/// errors where a `name_t` is used where an `address_t` is expected, and vice versa.
//
template <typename T, typename Tag>
class strong_type final
    : public std::conditional_t<std::ranges::range<T>, std::ranges::view_interface<strong_type<T, Tag>>,
                                detail::strong_type_empty_base> {
 public:
  //
  // Public types
  //

  using value_type = T;  ///< The type of the contained value.
  using tag_type = Tag;  ///< The tag type.

  //
  // Constructors
  //

  /// \brief Construct the gw::strong_type object
  template <typename... Args>
  constexpr explicit strong_type(Args&&... args) noexcept(std::is_nothrow_constructible_v<value_type, Args...>)
    requires std::constructible_from<value_type, Args...>
      : m_value(value_type{std::forward<Args>(args)...}) {}

  /// \brief constructs the gw::strong_type object
  template <typename U, typename... Args>
  constexpr strong_type(std::initializer_list<U> ilist, Args&&... args) noexcept(
      std::is_nothrow_constructible_v<value_type, std::initializer_list<U>&, Args...>)
    requires std::constructible_from<value_type, std::initializer_list<U>&, Args...>
      : m_value(value_type{ilist, std::forward<Args>(args)...}) {}

  //
  // Destructor
  //

  /// \brief destroys the contained value
  ~strong_type() noexcept(std::is_nothrow_destructible_v<value_type>) = default;

  //
  // Observers
  //

  /// \brief accesses the contained value
  constexpr auto operator->() const noexcept -> const value_type* { return &m_value; }

  /// \brief accesses the contained value
  constexpr auto operator->() noexcept -> value_type* { return &m_value; }

  /// \brief accesses the contained value
  constexpr auto operator*() const& noexcept -> const value_type& { return m_value; }

  /// \brief accesses the contained value
  constexpr auto operator*() & noexcept -> value_type& { return m_value; }

  /// \brief accesses the contained value
  constexpr auto operator*() const&& noexcept -> const value_type&& { return std::move(m_value); }

  /// \brief accesses the contained value
  constexpr auto operator*() && noexcept -> value_type&& { return std::move(m_value); }

  /// \brief returns the contained value
  constexpr auto value() const& noexcept -> const value_type& { return m_value; }

  /// \brief returns the contained value
  constexpr auto value() & noexcept -> value_type& { return m_value; }

  /// \brief returns the contained value
  constexpr auto value() const&& noexcept -> const value_type&& { return std::move(m_value); }

  /// \brief returns the contained value
  constexpr auto value() && noexcept -> value_type&& { return std::move(m_value); }

  //
  // Monadic operations
  //

  /// \brief returns a gw::strong_type containing the transformed contained value
  template <typename F>
  constexpr auto transform(F&& func) const& noexcept(noexcept(func(m_value))) -> strong_type
    requires std::invocable<F, const value_type&>
  {
    return strong_type<std::remove_cv_t<std::invoke_result_t<F, const value_type&>>, tag_type>{func(m_value)};
  }

  /// \brief returns a gw::strong_type containing the transformed contained value
  template <typename F>
  constexpr auto transform(F&& func) & noexcept(noexcept(func(m_value))) -> strong_type
    requires std::invocable<F, value_type&>
  {
    return strong_type<std::remove_cv_t<std::invoke_result_t<F, value_type&>>, tag_type>{func(m_value)};
  }

  /// \brief returns a gw::strong_type containing the transformed contained value
  template <typename F>
  constexpr auto transform(F&& func) const&& noexcept(noexcept(func(m_value))) -> strong_type
    requires std::invocable<F, const value_type&&>
  {
    return strong_type<std::remove_cv_t<std::invoke_result_t<F, const value_type&&>>, tag_type>{
        func(std::move(m_value))};
  }

  /// \brief returns a gw::strong_type containing the transformed contained value
  template <typename F>
  constexpr auto transform(F&& func) && noexcept(noexcept(func(m_value))) -> strong_type
    requires std::invocable<F, value_type&&>
  {
    return strong_type<std::remove_cv_t<std::invoke_result_t<F, value_type&&>>, tag_type>{func(std::move(m_value))};
  }

  //
  // Modifiers
  //

  /// \brief specializes the std::swap algorithm
  constexpr void swap(strong_type& rhs) noexcept(std::is_nothrow_swappable_v<value_type>)
    requires std::swappable<value_type>
  {
    using std::swap;
    swap(m_value, rhs.m_value);
  }

  /// \brief destroys any contained value
  constexpr void reset() noexcept(std::is_nothrow_default_constructible_v<value_type>)
    requires std::default_initializable<value_type>
  {
    m_value = value_type{};
  }

  /// \brief constructs the contained value in-place
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

  /// \brief compares gw::strong_type objects
  constexpr auto operator==(const strong_type& rhs) const& noexcept(noexcept(m_value == rhs.m_value)) -> bool
    requires std::equality_comparable<value_type>
  {
    return m_value == rhs.m_value;
  }

  /// \brief compares gw::strong_type objects
  constexpr auto operator!=(const strong_type& rhs) const& noexcept(noexcept(m_value != rhs.m_value)) -> bool
    requires std::equality_comparable<value_type>
  {
    return m_value != rhs.m_value;
  }

  /// \brief compares gw::strong_type objects
  constexpr auto operator<(const strong_type& rhs) const& noexcept(noexcept(m_value < rhs.m_value)) -> bool
    requires std::totally_ordered<value_type>
  {
    return m_value < rhs.m_value;
  }

  /// \brief compares gw::strong_type objects
  constexpr auto operator>(const strong_type& rhs) const& noexcept(noexcept(m_value > rhs.m_value)) -> bool
    requires std::totally_ordered<value_type>
  {
    return m_value > rhs.m_value;
  }

  /// \brief compares gw::strong_type objects
  constexpr auto operator<=(const strong_type& rhs) const& noexcept(noexcept(m_value <= rhs.m_value)) -> bool
    requires std::totally_ordered<value_type>
  {
    return m_value <= rhs.m_value;
  }

  /// \brief compares gw::strong_type objects
  constexpr auto operator>=(const strong_type& rhs) const& noexcept(noexcept(m_value >= rhs.m_value)) -> bool
    requires std::totally_ordered<value_type>
  {
    return m_value >= rhs.m_value;
  }

  /// \brief compares gw::strong_type objects
  constexpr auto operator<=>(const strong_type& rhs) const& noexcept(noexcept(m_value <=>
                                                                              rhs.m_value)) -> std::strong_ordering
    requires std::three_way_comparable<value_type>
  {
    return m_value <=> rhs.m_value;
  }

  //
  // Conversion operators
  //

  /// \brief converts the gw::strong_type to its underlying type
  constexpr explicit operator const value_type&() const& noexcept { return m_value; }

  /// \brief converts the gw::strong_type to its underlying type
  constexpr explicit operator value_type&() & noexcept { return m_value; }

  /// \brief converts the gw::strong_type to its underlying type
  constexpr explicit operator const value_type&&() const&& noexcept { return std::move(m_value); }

  /// \brief converts the gw::strong_type to its underlying type
  constexpr explicit operator value_type&&() && noexcept { return std::move(m_value); }

  //
  // Increment and decrement operators
  //

  /// \brief increments the contained value
  constexpr auto operator++() & noexcept(noexcept(++m_value)) -> strong_type&
    requires incrementable<value_type>
  {
    ++m_value;
    return *this;
  }

  /// \brief increments the contained value
  constexpr auto operator++() && noexcept(noexcept(++m_value)) -> strong_type&&
    requires incrementable<value_type>
  {
    ++m_value;
    return std::move(*this);
  }

  /// \brief increments the contained value
  constexpr auto operator++(int) & noexcept(noexcept(m_value++)) -> strong_type
    requires incrementable<value_type>
  {
    return strong_type{m_value++};
  }

  /// \brief increments the contained value
  constexpr auto operator++(int) && noexcept(noexcept(m_value++)) -> strong_type
    requires incrementable<value_type>
  {
    return strong_type{m_value++};
  }

  /// \brief decrements the contained value
  constexpr auto operator--() & noexcept(noexcept(--m_value)) -> strong_type&
    requires decrementable<value_type>
  {
    --m_value;
    return *this;
  }

  /// \brief decrements the contained value
  constexpr auto operator--() && noexcept(noexcept(--m_value)) -> strong_type&&
    requires decrementable<value_type>
  {
    --m_value;
    return std::move(*this);
  }

  /// \brief decrements the contained value
  constexpr auto operator--(int) & noexcept(noexcept(m_value--)) -> strong_type
    requires decrementable<value_type>
  {
    return strong_type{m_value--};
  }

  /// \brief decrements the contained value
  constexpr auto operator--(int) && noexcept(noexcept(m_value--)) -> strong_type
    requires decrementable<value_type>
  {
    return strong_type{m_value--};
  }

  //
  // Arithmetic operators
  //

  /// \brief affirms the contained value
  constexpr auto operator+() const& noexcept(noexcept(+m_value)) -> strong_type
    requires std::signed_integral<value_type>
  {
    return strong_type{+m_value};
  }

  /// \brief affirms the contained value
  constexpr auto operator+() && noexcept(noexcept(+m_value)) -> strong_type
    requires std::signed_integral<value_type>
  {
    return strong_type{+m_value};
  }

  /// \brief negates the contained value
  constexpr auto operator-() const& noexcept(noexcept(-m_value)) -> strong_type
    requires std::signed_integral<value_type>
  {
    return strong_type{-m_value};
  }

  /// \brief negates the contained value
  constexpr auto operator-() && noexcept(noexcept(-m_value)) -> strong_type
    requires std::signed_integral<value_type>
  {
    return strong_type{-m_value};
  }

  /// \brief adds the contained values
  constexpr auto operator+(const strong_type& rhs) const& noexcept(noexcept(m_value + rhs.m_value)) -> strong_type
    requires arithmetic<value_type>
  {
    return strong_type{m_value + rhs.m_value};
  }

  /// \brief adds the contained values
  constexpr auto operator+(strong_type&& rhs) const& noexcept(noexcept(m_value + rhs.m_value)) -> strong_type
    requires arithmetic<value_type>
  {
    return strong_type{m_value + rhs.m_value};
  }

  /// \brief adds the contained values
  constexpr auto operator+(const strong_type& rhs) && noexcept(noexcept(m_value + rhs.m_value)) -> strong_type
    requires arithmetic<value_type>
  {
    return strong_type{m_value + rhs.m_value};
  }

  /// \brief adds the contained values
  constexpr auto operator+(strong_type&& rhs) && noexcept(noexcept(m_value + rhs.m_value)) -> strong_type
    requires arithmetic<value_type>
  {
    return strong_type{m_value + rhs.m_value};
  }

  /// \brief subtracts the contained values
  constexpr auto operator-(const strong_type& rhs) const& noexcept(noexcept(m_value - rhs.m_value)) -> strong_type
    requires arithmetic<value_type>
  {
    return strong_type{m_value - rhs.m_value};
  }

  /// \brief subtracts the contained values
  constexpr auto operator-(strong_type&& rhs) const& noexcept(noexcept(m_value - rhs.m_value)) -> strong_type
    requires arithmetic<value_type>
  {
    return strong_type{m_value - rhs.m_value};
  }

  /// \brief subtracts the contained values
  constexpr auto operator-(const strong_type& rhs) && noexcept(noexcept(m_value - rhs.m_value)) -> strong_type
    requires arithmetic<value_type>
  {
    return strong_type{m_value - rhs.m_value};
  }

  /// \brief subtracts the contained values
  constexpr auto operator-(strong_type&& rhs) && noexcept(noexcept(m_value - rhs.m_value)) -> strong_type
    requires arithmetic<value_type>
  {
    return strong_type{m_value - rhs.m_value};
  }

  /// \brief multiplies the contained values
  constexpr auto operator*(const strong_type& rhs) const& noexcept(noexcept(m_value * rhs.m_value)) -> strong_type
    requires arithmetic<value_type>
  {
    return strong_type{m_value * rhs.m_value};
  }

  /// \brief multiplies the contained values
  constexpr auto operator*(strong_type&& rhs) const& noexcept(noexcept(m_value * rhs.m_value)) -> strong_type
    requires arithmetic<value_type>
  {
    return strong_type{m_value * rhs.m_value};
  }

  /// \brief multiplies the contained values
  constexpr auto operator*(const strong_type& rhs) && noexcept(noexcept(m_value * rhs.m_value)) -> strong_type
    requires arithmetic<value_type>
  {
    return strong_type{m_value * rhs.m_value};
  }

  /// \brief multiplies the contained values
  constexpr auto operator*(strong_type&& rhs) && noexcept(noexcept(m_value * rhs.m_value)) -> strong_type
    requires arithmetic<value_type>
  {
    return strong_type{m_value * rhs.m_value};
  }

  /// \brief devides the contained values
  constexpr auto operator/(const strong_type& rhs) const& noexcept(noexcept(m_value / rhs.m_value)) -> strong_type
    requires arithmetic<value_type>
  {
    return strong_type{m_value / rhs.m_value};
  }

  /// \brief devides the contained values
  constexpr auto operator/(strong_type&& rhs) const& noexcept(noexcept(m_value / rhs.m_value)) -> strong_type
    requires arithmetic<value_type>
  {
    return strong_type{m_value / rhs.m_value};
  }

  /// \brief devides the contained values
  constexpr auto operator/(const strong_type& rhs) && noexcept(noexcept(m_value / rhs.m_value)) -> strong_type
    requires arithmetic<value_type>
  {
    return strong_type{m_value / rhs.m_value};
  }

  /// \brief devides the contained values
  constexpr auto operator/(strong_type&& rhs) && noexcept(noexcept(m_value / rhs.m_value)) -> strong_type
    requires arithmetic<value_type>
  {
    return strong_type{m_value / rhs.m_value};
  }

  /// \brief calculates the remainder of the contained values
  constexpr auto operator%(const strong_type& rhs) const& noexcept(noexcept(m_value % rhs.m_value)) -> strong_type
    requires arithmetic<value_type>
  {
    return strong_type{m_value % rhs.m_value};
  }

  /// \brief calculates the remainder of the contained values
  constexpr auto operator%(strong_type&& rhs) const& noexcept(noexcept(m_value % rhs.m_value)) -> strong_type
    requires arithmetic<value_type>
  {
    return strong_type{m_value % rhs.m_value};
  }

  /// \brief calculates the remainder of the contained values
  constexpr auto operator%(const strong_type& rhs) && noexcept(noexcept(m_value % rhs.m_value)) -> strong_type
    requires arithmetic<value_type>
  {
    return strong_type{m_value % rhs.m_value};
  }

  /// \brief calculates the remainder of the contained values
  constexpr auto operator%(strong_type&& rhs) && noexcept(noexcept(m_value % rhs.m_value)) -> strong_type
    requires arithmetic<value_type>
  {
    return strong_type{m_value % rhs.m_value};
  }

  /// \brief adds the contained values and assigns the result
  constexpr auto operator+=(const strong_type& rhs) & noexcept(noexcept(m_value += rhs.m_value)) -> strong_type&
    requires arithmetic<value_type>
  {
    m_value += rhs.m_value;
    return *this;
  }

  /// \brief adds the contained values and assigns the result
  constexpr auto operator+=(strong_type&& rhs) & noexcept(noexcept(m_value += rhs.m_value)) -> strong_type&
    requires arithmetic<value_type>
  {
    m_value += rhs.m_value;
    return *this;
  }

  /// \brief subtracts the contained values and assigns the result
  constexpr auto operator-=(const strong_type& rhs) & noexcept(noexcept(m_value -= rhs.m_value)) -> strong_type&
    requires arithmetic<value_type>
  {
    m_value -= rhs.m_value;
    return *this;
  }

  /// \brief subtracts the contained values and assigns the result
  constexpr auto operator-=(strong_type&& rhs) & noexcept(noexcept(m_value -= rhs.m_value)) -> strong_type&
    requires arithmetic<value_type>
  {
    m_value -= rhs.m_value;
    return *this;
  }

  /// \brief multiplies the contained values and assigns the result
  constexpr auto operator*=(const strong_type& rhs) & noexcept(noexcept(m_value *= rhs.m_value)) -> strong_type&
    requires arithmetic<value_type>
  {
    m_value *= rhs.m_value;
    return *this;
  }

  /// \brief multiplies the contained values and assigns the result
  constexpr auto operator*=(strong_type&& rhs) & noexcept(noexcept(m_value *= rhs.m_value)) -> strong_type&
    requires arithmetic<value_type>
  {
    m_value *= rhs.m_value;
    return *this;
  }

  /// \brief devides the contained values and assigns the result
  constexpr auto operator/=(const strong_type& rhs) & noexcept(noexcept(m_value /= rhs.m_value)) -> strong_type&
    requires arithmetic<value_type>
  {
    m_value /= rhs.m_value;
    return *this;
  }

  /// \brief devides the contained values and assigns the result
  constexpr auto operator/=(strong_type&& rhs) & noexcept(noexcept(m_value /= rhs.m_value)) -> strong_type&
    requires arithmetic<value_type>
  {
    m_value /= rhs.m_value;
    return *this;
  }

  /// \brief calculates the remainder of the contained values and assigns the result
  constexpr auto operator%=(const strong_type& rhs) & noexcept(noexcept(m_value %= rhs.m_value)) -> strong_type&
    requires arithmetic<value_type>
  {
    m_value %= rhs.m_value;
    return *this;
  }

  /// \brief calculates the remainder of the contained values and assigns the result
  constexpr auto operator%=(strong_type&& rhs) & noexcept(noexcept(m_value %= rhs.m_value)) -> strong_type&
    requires arithmetic<value_type>
  {
    m_value %= rhs.m_value;
    return *this;
  }

  //
  // Bitwise operators
  //

  /// \brief inverts the contained value
  constexpr auto operator~() const& noexcept(noexcept(~m_value)) -> strong_type
    requires std::unsigned_integral<value_type>
  {
    return strong_type{~m_value};
  }

  /// \brief performs binary AND on the contained values
  constexpr auto operator&(const strong_type& rhs) const& noexcept(noexcept(m_value & rhs.m_value)) -> strong_type
    requires std::unsigned_integral<value_type>
  {
    return strong_type{m_value & rhs.m_value};
  }

  /// \brief performs binary AND on the contained values
  constexpr auto operator&(strong_type&& rhs) const& noexcept(noexcept(m_value & rhs.m_value)) -> strong_type
    requires std::unsigned_integral<value_type>
  {
    return strong_type{m_value & rhs.m_value};
  }

  /// \brief performs binary AND on the contained values
  constexpr auto operator&(const strong_type& rhs) && noexcept(noexcept(m_value & rhs.m_value)) -> strong_type
    requires std::unsigned_integral<value_type>
  {
    return strong_type{m_value & rhs.m_value};
  }

  /// \brief performs binary AND on the contained values
  constexpr auto operator&(strong_type&& rhs) && noexcept(noexcept(m_value & rhs.m_value)) -> strong_type
    requires std::unsigned_integral<value_type>
  {
    return strong_type{m_value & rhs.m_value};
  }

  /// \brief performs binary OR on the contained values
  constexpr auto operator|(const strong_type& rhs) const& noexcept(noexcept(m_value | rhs.m_value)) -> strong_type
    requires std::unsigned_integral<value_type>
  {
    return strong_type{m_value | rhs.m_value};
  }

  /// \brief performs binary OR on the contained values
  constexpr auto operator|(strong_type&& rhs) const& noexcept(noexcept(m_value | rhs.m_value)) -> strong_type
    requires std::unsigned_integral<value_type>
  {
    return strong_type{m_value | rhs.m_value};
  }

  /// \brief performs binary OR on the contained values
  constexpr auto operator|(const strong_type& rhs) && noexcept(noexcept(m_value | rhs.m_value)) -> strong_type
    requires std::unsigned_integral<value_type>
  {
    return strong_type{m_value | rhs.m_value};
  }

  /// \brief performs binary OR on the contained values
  constexpr auto operator|(strong_type&& rhs) && noexcept(noexcept(m_value | rhs.m_value)) -> strong_type
    requires std::unsigned_integral<value_type>
  {
    return strong_type{m_value | rhs.m_value};
  }

  /// \brief performs binary XOR on the contained values
  constexpr auto operator^(const strong_type& rhs) const& noexcept(noexcept(m_value ^ rhs.m_value)) -> strong_type
    requires std::unsigned_integral<value_type>
  {
    return strong_type{m_value ^ rhs.m_value};
  }

  /// \brief performs binary XOR on the contained values
  constexpr auto operator^(strong_type&& rhs) const& noexcept(noexcept(m_value ^ rhs.m_value)) -> strong_type
    requires std::unsigned_integral<value_type>
  {
    return strong_type{m_value ^ rhs.m_value};
  }

  /// \brief performs binary XOR on the contained values
  constexpr auto operator^(const strong_type& rhs) && noexcept(noexcept(m_value ^ rhs.m_value)) -> strong_type
    requires std::unsigned_integral<value_type>
  {
    return strong_type{m_value ^ rhs.m_value};
  }

  /// \brief performs binary XOR on the contained values
  constexpr auto operator^(strong_type&& rhs) && noexcept(noexcept(m_value ^ rhs.m_value)) -> strong_type
    requires std::unsigned_integral<value_type>
  {
    return strong_type{m_value ^ rhs.m_value};
  }

  /// \brief performs binary left shift on the contained values
  constexpr auto operator<<(const strong_type& rhs) const& noexcept(noexcept(m_value << rhs.m_value)) -> strong_type
    requires std::unsigned_integral<value_type>
  {
    return strong_type{m_value << rhs.m_value};
  }

  /// \brief performs binary left shift on the contained values
  constexpr auto operator<<(strong_type&& rhs) const& noexcept(noexcept(m_value << rhs.m_value)) -> strong_type
    requires std::unsigned_integral<value_type>
  {
    return strong_type{m_value << rhs.m_value};
  }

  /// \brief performs binary left shift on the contained values
  constexpr auto operator<<(const strong_type& rhs) && noexcept(noexcept(m_value << rhs.m_value)) -> strong_type
    requires std::unsigned_integral<value_type>
  {
    return strong_type{m_value << rhs.m_value};
  }

  /// \brief performs binary left shift on the contained values
  constexpr auto operator<<(strong_type&& rhs) && noexcept(noexcept(m_value << rhs.m_value)) -> strong_type
    requires std::unsigned_integral<value_type>
  {
    return strong_type{m_value << rhs.m_value};
  }

  /// \brief performs binary right shift on the contained values
  constexpr auto operator>>(const strong_type& rhs) const& noexcept(noexcept(m_value >> rhs.m_value)) -> strong_type
    requires std::unsigned_integral<value_type>
  {
    return strong_type{m_value >> rhs.m_value};
  }

  /// \brief performs binary right shift on the contained values
  constexpr auto operator>>(strong_type&& rhs) const& noexcept(noexcept(m_value >> rhs.m_value)) -> strong_type
    requires std::unsigned_integral<value_type>
  {
    return strong_type{m_value >> rhs.m_value};
  }

  /// \brief performs binary right shift on the contained values
  constexpr auto operator>>(const strong_type& rhs) && noexcept(noexcept(m_value >> rhs.m_value)) -> strong_type
    requires std::unsigned_integral<value_type>
  {
    return strong_type{m_value >> rhs.m_value};
  }

  /// \brief performs binary right shift on the contained values
  constexpr auto operator>>(strong_type&& rhs) && noexcept(noexcept(m_value >> rhs.m_value)) -> strong_type
    requires std::unsigned_integral<value_type>
  {
    return strong_type{m_value >> rhs.m_value};
  }

  /// \brief performs binary AND on the contained values and assigns the result
  constexpr auto operator&=(const strong_type& rhs) & noexcept(noexcept(m_value &= rhs.m_value)) -> strong_type&
    requires std::unsigned_integral<value_type>
  {
    m_value &= rhs.m_value;
    return *this;
  }

  /// \brief performs binary AND on the contained values and assigns the result
  constexpr auto operator&=(strong_type&& rhs) & noexcept(noexcept(m_value &= rhs.m_value)) -> strong_type&
    requires std::unsigned_integral<value_type>
  {
    m_value &= rhs.m_value;
    return *this;
  }

  /// \brief performs binary OR on the contained values and assigns the result
  constexpr auto operator|=(const strong_type& rhs) & noexcept(noexcept(m_value |= rhs.m_value)) -> strong_type&
    requires std::unsigned_integral<value_type>
  {
    m_value |= rhs.m_value;
    return *this;
  }

  /// \brief performs binary OR on the contained values and assigns the result
  constexpr auto operator|=(strong_type&& rhs) & noexcept(noexcept(m_value |= rhs.m_value)) -> strong_type&
    requires std::unsigned_integral<value_type>
  {
    m_value |= rhs.m_value;
    return *this;
  }

  /// \brief performs binary XOR on the contained values and assigns the result
  constexpr auto operator^=(const strong_type& rhs) & noexcept(noexcept(m_value ^= rhs.m_value)) -> strong_type&
    requires std::unsigned_integral<value_type>
  {
    m_value ^= rhs.m_value;
    return *this;
  }

  /// \brief performs binary XOR on the contained values and assigns the result
  constexpr auto operator^=(strong_type&& rhs) & noexcept(noexcept(m_value ^= rhs.m_value)) -> strong_type&
    requires std::unsigned_integral<value_type>
  {
    m_value ^= rhs.m_value;
    return *this;
  }

  /// \brief performs binary left shift on the contained values and assigns the result
  constexpr auto operator<<=(const strong_type& rhs) & noexcept(noexcept(m_value <<= rhs.m_value)) -> strong_type&
    requires std::unsigned_integral<value_type>
  {
    m_value <<= rhs.m_value;
    return *this;
  }

  /// \brief performs binary left shift on the contained values and assigns the result
  constexpr auto operator<<=(strong_type&& rhs) & noexcept(noexcept(m_value <<= rhs.m_value)) -> strong_type&
    requires std::unsigned_integral<value_type>
  {
    m_value <<= rhs.m_value;
    return *this;
  }

  /// \brief performs binary right shift on the contained values and assigns the result
  constexpr auto operator>>=(const strong_type& rhs) & noexcept(noexcept(m_value >>= rhs.m_value)) -> strong_type&
    requires std::unsigned_integral<value_type>
  {
    m_value >>= rhs.m_value;
    return *this;
  }

  /// \brief performs binary right shift on the contained values and assigns the result
  constexpr auto operator>>=(strong_type&& rhs) & noexcept(noexcept(m_value >>= rhs.m_value)) -> strong_type&
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
                                const strong_type& rhs) noexcept(noexcept(ostream << rhs.m_value)) -> std::ostream&
    requires ostreamable<value_type>
  {
    return ostream << rhs.m_value;
  }

  /// \brief extracts formatted data
  friend inline auto operator>>(std::istream& istream,
                                strong_type& rhs) noexcept(noexcept(istream >> rhs.m_value)) -> std::istream&
    requires istreamable<value_type>
  {
    return istream >> rhs.m_value;
  }

 private:
  value_type m_value{};
};

//
// Creation functions
//

/// \brief creates a gw::strong_type object
template <typename Tag, typename T, typename... Args>
constexpr auto make_strong_type(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
  requires std::constructible_from<std::remove_cvref_t<T>, Args...>
{
  return strong_type<std::remove_cvref_t<T>, Tag>{std::forward<Args>(args)...};
}

/// \brief creates a gw::strong_type object
template <typename Tag, typename T, typename U, typename... Args>
constexpr auto make_strong_type(std::initializer_list<U> ilist, Args&&... args) noexcept(
    std::is_nothrow_constructible_v<T, std::initializer_list<U>&, Args...>)
  requires std::constructible_from<T, std::initializer_list<U>&, Args...>
{
  return strong_type<std::remove_cvref_t<T>, Tag>{ilist, std::forward<Args>(args)...};
}

/// \brief creates a gw::strong_type object
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

/// \brief hash support for gw::strong_type
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

/// \brief Format the `gw::strong_type` object.
///
template <typename T, typename Tag, class CharT>
// NOLINTNEXTLINE(cert-dcl58-cpp)
struct formatter<::gw::strong_type<T, Tag>, CharT> {
  /// \brief Parse the format string.
  ///
  template <class ParseContext>
  constexpr auto parse(ParseContext& context) -> ParseContext::iterator {
    return context.begin();
  }

  /// \brief Format the `gw::strong_type` object.
  ///
  template <class FormatContext>
  auto format(const ::gw::strong_type<T, Tag>& strong_type, FormatContext& context) const -> FormatContext::iterator
#if __cplusplus > 202002L
    requires std::formattable<T, CharT>
#endif  // __cplusplus > 202002L
  {
    if constexpr (::gw::named<Tag>) {
      return format_to(context.out(), "{}: {}", Tag::name(), strong_type.value());
    }

    return format_to(context.out(), "{}", strong_type.value());
  }
};

}  // namespace std
