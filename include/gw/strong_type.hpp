// Copyright (c) 2023 Martin Stump
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <functional>
#include <type_traits>
#include <typeindex>
#include <utility>

namespace gw {

template <typename T, typename Tag>
struct StrongType {
  using ValueType = T;
  using TagType = Tag;

  constexpr StrongType() noexcept(std::is_nothrow_default_constructible_v<T>)
    requires std::is_default_constructible_v<T>
  = default;

  constexpr explicit StrongType(const T& value) noexcept(std::is_nothrow_copy_constructible_v<T>)
    requires std::is_copy_constructible_v<T>
      : value_{value} {}

  constexpr explicit StrongType(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>)
    requires std::is_move_constructible_v<T>
      : value_{std::move(value)} {}

  constexpr auto operator->() const noexcept -> const T* { return &value_; }
  constexpr auto operator->() noexcept -> T* { return &value_; }
  constexpr auto operator*() const& noexcept -> const T& { return value_; }
  constexpr auto operator*() & noexcept -> T& { return value_; }
  constexpr auto operator*() const&& noexcept -> const T&& { return std::move(value_); }
  constexpr auto operator*() && noexcept -> T&& { return std::move(value_); }

  constexpr auto Value() const& noexcept -> const T& { return value_; }
  constexpr auto Value() & noexcept -> T& { return value_; }

  constexpr void Swap(StrongType& other) noexcept(std::is_nothrow_swappable_v<T>) {
    using std::swap;
    swap(value_, other.value_);
  }

  constexpr void Reset() noexcept(std::is_nothrow_default_constructible_v<T>)
    requires std::is_default_constructible_v<T>
  {
    value_ = T{};
  }

  template <class... Args>
  constexpr auto Emplace(Args&&... args) -> T&
    requires std::is_constructible_v<T, Args...>
  {
    value_ = T{std::forward<Args>(args)...};
    return value_;
  }

  constexpr auto operator<=>(const StrongType&) const noexcept = default;

 private:
  T value_{};
};

namespace detail {

template <typename T>
concept Hashable = requires(T value) {
  { std::hash<T>{}(value) } -> std::convertible_to<std::size_t>;
};

}  // namespace detail
}  // namespace gw

namespace std {

template <gw::detail::Hashable T, typename Tag>
struct hash<gw::StrongType<T, Tag>> {  // NOLINT(cert-dcl58-cpp)
  auto operator()(const gw::StrongType<T, Tag>& strong_type) const noexcept -> size_t {
    auto value_hash = hash<T>{}(strong_type.Value());
    auto tag_hash = hash<type_index>{}(type_index{typeid(Tag)});
    return value_hash ^ tag_hash;
  }
};

}  // namespace std
