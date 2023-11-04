// Copyright (c) 2023 Martin Stump
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <functional>
#include <type_traits>
#include <typeindex>
#include <utility>

#include "gw/concepts.hpp"

namespace gw {

template <class T, typename Tag>
struct strong_type {
  // Types
  using value_type = T;
  using tag_type = Tag;

  // Constructors
  constexpr strong_type() noexcept(std::is_nothrow_default_constructible_v<T>)
    requires std::is_default_constructible_v<T>
  = default;

  constexpr explicit strong_type(const T& value) noexcept(std::is_nothrow_copy_constructible_v<T>)
    requires std::is_copy_constructible_v<T>
      : m_value{value} {}

  constexpr explicit strong_type(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>)
    requires std::is_move_constructible_v<T>
      : m_value{std::forward<T>(value)} {}

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
  constexpr auto transform(F&& callable) const& noexcept(noexcept(callable(m_value)))
    requires std::is_invocable_v<F, T>
  {
    return strong_type<std::invoke_result_t<F, T>, Tag>{callable(m_value)};
  }

  template <class F>
  constexpr auto transform(F&& callable) & noexcept(noexcept(callable(m_value)))
    requires std::is_invocable_v<F, T>
  {
    return strong_type<std::invoke_result_t<F, T>, Tag>{callable(m_value)};
  }

  template <class F>
  constexpr auto transform(F&& callable) const&& noexcept(noexcept(callable(m_value)))
    requires std::is_invocable_v<F, T>
  {
    return strong_type<std::invoke_result_t<F, T>, Tag>{callable(std::move(m_value))};
  }

  template <class F>
  constexpr auto transform(F&& callable) && noexcept(noexcept(callable(m_value)))
    requires std::is_invocable_v<F, T>
  {
    return strong_type<std::invoke_result_t<F, T>, Tag>{callable(std::move(m_value))};
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

 private:
  T m_value{};
};

template <class T, typename Tag>
constexpr auto make_strong_type(T&& value) noexcept(
    std::is_nothrow_constructible_v<strong_type<std::decay_t<T>, Tag>, T>) -> gw::strong_type<std::decay_t<T>, Tag>
  requires std::is_constructible_v<std::decay_t<T>, T>
{
  return strong_type<std::decay_t<T>, Tag>{std::forward<T>(value)};
}

template <class T, typename Tag, typename... Args>
constexpr auto make_strong_type(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
    -> gw::strong_type<std::decay_t<T>, Tag>
  requires std::is_constructible_v<std::decay_t<T>, Args...>
{
  return strong_type<std::decay_t<T>, Tag>{std::forward<Args>(args)...};
}

template <typename T, typename Tag, typename U, typename... Args>
constexpr auto make_strong_type(std::initializer_list<U> ilist, Args&&... args) noexcept(
    std::is_nothrow_constructible_v<T, std::initializer_list<U>&, Args...>)
  requires std::is_constructible_v<T, std::initializer_list<U>&, Args...>
{
  return strong_type<std::decay_t<T>, Tag>{ilist, std::forward<Args>(args)...};
}

template <typename Tag>
constexpr auto make_strong_type(auto&& value) noexcept(
    std::is_nothrow_constructible_v<strong_type<std::decay_t<decltype(value)>, Tag>, decltype(value)>)
    -> gw::strong_type<std::decay_t<decltype(value)>, Tag>
  requires std::is_constructible_v<std::decay_t<decltype(value)>, decltype(value)>
{
  return strong_type<std::decay_t<decltype(value)>, Tag>{std::forward<decltype(value)>(value)};
}

}  // namespace gw

namespace std {

template <gw::hashable T, typename Tag>
struct hash<gw::strong_type<T, Tag>> {  // NOLINT(cert-dcl58-cpp)
  auto operator()(const gw::strong_type<T, Tag>& strong_type) const noexcept -> size_t {
    auto value_hash = hash<T>{}(strong_type.value());
    auto tag_hash = hash<type_index>{}(type_index{typeid(Tag)});
    return value_hash ^ tag_hash;
  }
};

}  // namespace std
