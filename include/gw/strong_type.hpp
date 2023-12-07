// Copyright (c) 2023 Martin Stump
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <concepts>
#include <initializer_list>
#include <type_traits>
#include <utility>

#ifdef GW_ENABLE_HASH_CALCULATION
#include <cstddef>
#include <functional>
#include <typeindex>
#endif  // GW_ENABLE_HASH_CALCULATION

#ifdef GW_ENABLE_STREAM_OPERATORS
#include <iostream>
#endif  // GW_ENABLE_STREAM_OPERATORS

#ifdef GW_ENABLE_STRING_CONVERSION
#include <string>
#endif  // GW_ENABLE_STRING_CONVERSION

#include "gw/concepts.hpp"

namespace gw {

template <typename Tag, class T>
struct strong_type {
  // Types
  using tag_type = Tag;
  using value_type = T;

  // Constructors
  template <class... Args>
  constexpr explicit strong_type(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
    requires std::constructible_from<T, Args...>
      : m_value(T{std::forward<Args>(args)...}) {}

  template <class U, class... Args>
  constexpr strong_type(std::initializer_list<U> ilist,
                        Args&&... args) noexcept(std::is_nothrow_constructible_v<T, std::initializer_list<U>&, Args...>)
    requires std::constructible_from<T, std::initializer_list<U>&, Args...>
      : m_value(T{ilist, std::forward<Args>(args)...}) {}

  // Observers
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

  // Monadic operations
  template <class F>
  constexpr auto transform(F&& func) const& noexcept(noexcept(func(m_value)))
    requires std::invocable<F, const T&>
  {
    return strong_type<Tag, std::remove_cv_t<std::invoke_result_t<F, const T&>>>{func(m_value)};
  }

  template <class F>
  constexpr auto transform(F&& func) & noexcept(noexcept(func(m_value)))
    requires std::invocable<F, T&>
  {
    return strong_type<Tag, std::remove_cv_t<std::invoke_result_t<F, T&>>>{func(m_value)};
  }

  template <class F>
  constexpr auto transform(F&& func) const&& noexcept(noexcept(func(m_value)))
    requires std::invocable<F, const T&&>
  {
    return strong_type<Tag, std::remove_cv_t<std::invoke_result_t<F, const T&&>>>{func(std::move(m_value))};
  }

  template <class F>
  constexpr auto transform(F&& func) && noexcept(noexcept(func(m_value)))
    requires std::invocable<F, T&&>
  {
    return strong_type<Tag, std::remove_cv_t<std::invoke_result_t<F, T&&>>>{func(std::move(m_value))};
  }

  // Modifiers
  constexpr void swap(strong_type& other) noexcept(std::is_nothrow_swappable_v<T>)
    requires std::swappable<T>
  {
    using std::swap;
    swap(m_value, other.m_value);
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

  // Comparison operators
  constexpr auto operator<=>(const strong_type&) const noexcept = default;

  // Conversion operators
  constexpr explicit operator const T&() const& noexcept { return m_value; }
  constexpr explicit operator T&() & noexcept { return m_value; }
  constexpr explicit operator const T&&() const&& noexcept { return std::move(m_value); }
  constexpr explicit operator T&&() && noexcept { return std::move(m_value); }

  // Increment and decrement operators
  constexpr auto operator++() & noexcept(noexcept(++m_value)) -> strong_type&
    requires gw::arithmetic<T>
  {
    ++m_value;
    return *this;
  }

  constexpr auto operator++() && noexcept(noexcept(++m_value)) -> strong_type&&
    requires gw::arithmetic<T>
  {
    ++m_value;
    return std::move(*this);
  }

  constexpr auto operator++(int) & noexcept(noexcept(m_value++)) -> strong_type
    requires gw::arithmetic<T>
  {
    return strong_type{m_value++};
  }

  constexpr auto operator++(int) && noexcept(noexcept(m_value++)) -> strong_type
    requires gw::arithmetic<T>
  {
    return strong_type{m_value++};
  }

  constexpr auto operator--() & noexcept(noexcept(--m_value)) -> strong_type&
    requires gw::arithmetic<T>
  {
    --m_value;
    return *this;
  }

  constexpr auto operator--() && noexcept(noexcept(--m_value)) -> strong_type&&
    requires gw::arithmetic<T>
  {
    --m_value;
    return std::move(*this);
  }

  constexpr auto operator--(int) & noexcept(noexcept(m_value--)) -> strong_type
    requires gw::arithmetic<T>
  {
    return strong_type{m_value--};
  }

  constexpr auto operator--(int) && noexcept(noexcept(m_value--)) -> strong_type
    requires gw::arithmetic<T>
  {
    return strong_type{m_value--};
  }

  // Stream operators
#ifdef GW_ENABLE_STREAM_OPERATORS
  friend inline auto operator<<(std::ostream& ostream, const strong_type& rhs) -> std::ostream&
    requires gw::ostreamable<T>
  {
    return ostream << rhs.m_value;
  }

  friend inline auto operator>>(std::istream& istream, strong_type& rhs) -> std::istream&
    requires gw::istreamable<T>
  {
    return istream >> rhs.m_value;
  }
#endif  // GW_ENABLE_STREAM_OPERATORS

 private:
  T m_value{};
};

// Creation functions
template <typename Tag, class T, typename... Args>
constexpr auto make_strong_type(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
  requires std::constructible_from<std::remove_cvref_t<T>, Args...>
{
  return strong_type<Tag, std::remove_cvref_t<T>>{std::forward<Args>(args)...};
}

template <typename Tag, class T, typename U, typename... Args>
constexpr auto make_strong_type(std::initializer_list<U> ilist, Args&&... args) noexcept(
    std::is_nothrow_constructible_v<T, std::initializer_list<U>&, Args...>)
  requires std::constructible_from<T, std::initializer_list<U>&, Args...>
{
  return strong_type<Tag, std::remove_cvref_t<T>>{ilist, std::forward<Args>(args)...};
}

template <typename Tag, class T>
constexpr auto make_strong_type(T&& value) noexcept(
    std::is_nothrow_constructible_v<strong_type<Tag, std::remove_cvref_t<T>>, T>)
  requires std::constructible_from<std::remove_cvref_t<T>, T>
{
  return strong_type<Tag, std::remove_cvref_t<T>>{std::forward<T>(value)};
}

}  // namespace gw

namespace std {

// Hash
#ifdef GW_ENABLE_HASH_CALCULATION
template <gw::complete Tag, gw::hashable T>
// NOLINTNEXTLINE(cert-dcl58-cpp)
struct hash<gw::strong_type<Tag, T>> {
  [[nodiscard]] auto operator()(const gw::strong_type<Tag, T>& strong_type) const noexcept -> size_t {
    auto tag_hash = hash<type_index>{}(type_index{typeid(Tag)});
    auto value_hash = hash<T>{}(strong_type.value());
    return tag_hash ^ value_hash;
  }
};
#endif  // GW_ENABLE_HASH_CALCULATION

// String conversion
#ifdef GW_ENABLE_STRING_CONVERSION
template <typename Tag, gw::string_convertable T>
// NOLINTNEXTLINE(cert-dcl58-cpp)
[[nodiscard]] auto inline to_string(gw::strong_type<Tag, T> strong_type) -> std::string {
  return std::to_string(strong_type.value());
}

template <gw::named Tag, gw::string_convertable T>
// NOLINTNEXTLINE(cert-dcl58-cpp)
[[nodiscard]] auto inline to_string(gw::strong_type<Tag, T> strong_type) -> std::string {
  return std::string{Tag::name()} + ": " + std::to_string(strong_type.value());
}
#endif  // GW_ENABLE_STRING_CONVERSION

}  // namespace std
