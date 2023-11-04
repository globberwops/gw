// Copyright (c) 2023 Martin Stump
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <functional>
#include <ostream>
#include <type_traits>
#include <typeindex>
#include <utility>

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
    requires std::is_constructible_v<T, Args...>
      : m_value(T{std::forward<Args>(args)...}) {}

  template <class U, class... Args>
  constexpr strong_type(std::initializer_list<U> ilist,
                        Args&&... args) noexcept(std::is_nothrow_constructible_v<T, std::initializer_list<U>&, Args...>)
    requires std::is_constructible_v<T, std::initializer_list<U>&, Args...>
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
    requires std::is_invocable_v<F, const T&>
  {
    return strong_type<Tag, std::remove_cv_t<std::invoke_result_t<F, const T&>>>{func(m_value)};
  }

  template <class F>
  constexpr auto transform(F&& func) & noexcept(noexcept(func(m_value)))
    requires std::is_invocable_v<F, T&>
  {
    return strong_type<Tag, std::remove_cv_t<std::invoke_result_t<F, T&>>>{func(m_value)};
  }

  template <class F>
  constexpr auto transform(F&& func) const&& noexcept(noexcept(func(m_value)))
    requires std::is_invocable_v<F, const T&&>
  {
    return strong_type<Tag, std::remove_cv_t<std::invoke_result_t<F, const T&&>>>{func(std::move(m_value))};
  }

  template <class F>
  constexpr auto transform(F&& func) && noexcept(noexcept(func(m_value)))
    requires std::is_invocable_v<F, T&&>
  {
    return strong_type<Tag, std::remove_cv_t<std::invoke_result_t<F, T&&>>>{func(std::move(m_value))};
  }

  // Modifiers
  constexpr void swap(strong_type& other) noexcept(std::is_nothrow_swappable_v<T>)
    requires std::is_swappable_v<T>
  {
    using std::swap;
    swap(m_value, other.m_value);
  }

  constexpr void reset() noexcept(std::is_nothrow_default_constructible_v<T>)
    requires std::is_default_constructible_v<T>
  {
    m_value = T{};
  }

  template <class... Args>
  constexpr auto emplace(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>) -> T&
    requires std::is_constructible_v<T, Args...>
  {
    m_value = T{std::forward<Args>(args)...};
    return m_value;
  }

  // Comparison operators
  constexpr auto operator<=>(const strong_type&) const noexcept = default;

  // Stream operators
  friend auto operator<<(std::ostream& os,
                         const strong_type& strong_type) noexcept(noexcept(os << strong_type.m_value)) -> std::ostream&
    requires complete<Tag> && streamable<T>
  {
    return os << strong_type.m_value;
  }

 private:
  T m_value{};
};

// Creation functions
template <typename Tag, class T, typename... Args>
constexpr auto make_strong_type(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
  requires std::is_constructible_v<std::remove_cvref_t<T>, Args...>
{
  return strong_type<Tag, std::remove_cvref_t<T>>{std::forward<Args>(args)...};
}

template <typename Tag, class T, typename U, typename... Args>
constexpr auto make_strong_type(std::initializer_list<U> ilist, Args&&... args) noexcept(
    std::is_nothrow_constructible_v<T, std::initializer_list<U>&, Args...>)
  requires std::is_constructible_v<T, std::initializer_list<U>&, Args...>
{
  return strong_type<Tag, std::remove_cvref_t<T>>{ilist, std::forward<Args>(args)...};
}

template <typename Tag, class T>
constexpr auto make_strong_type(T&& value) noexcept(
    std::is_nothrow_constructible_v<strong_type<Tag, std::remove_cvref_t<T>>, T>)
  requires std::is_constructible_v<std::remove_cvref_t<T>, T>
{
  return strong_type<Tag, std::remove_cvref_t<T>>{std::forward<T>(value)};
}

}  // namespace gw

namespace std {

// Hash
template <gw::complete Tag, gw::hashable T>
struct hash<gw::strong_type<Tag, T>> {  // NOLINT(cert-dcl58-cpp)
  auto operator()(const gw::strong_type<Tag, T>& strong_type) const noexcept -> size_t {
    auto tag_hash = hash<type_index>{}(type_index{typeid(Tag)});
    auto value_hash = hash<T>{}(strong_type.value());
    return tag_hash ^ value_hash;
  }
};

}  // namespace std
