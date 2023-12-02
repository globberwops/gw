// Copyright (c) 2023 Martin Stump
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <concepts>
#include <functional>
#include <string>
#include <type_traits>

namespace gw {

template <typename T>
concept arithmetic = requires { std::is_arithmetic_v<T>; };

template <typename T>
concept complete = requires { sizeof(T); };

template <typename T>
concept hashable = requires(T value) {
  { std::hash<T>{}(value) } -> std::same_as<std::size_t>;
};

template <typename T>
concept named = requires(T value) {
  { value.name() } -> std::convertible_to<std::string>;
};

template <typename T>
concept string_convertable = requires(T value) {
  { std::to_string(value) } -> std::same_as<std::string>;
};

}  // namespace gw
