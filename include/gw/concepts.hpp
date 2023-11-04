// Copyright (c) 2023 Martin Stump
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <concepts>
#include <functional>
#include <ostream>

namespace gw {

template <typename T>
concept hashable = requires(T value) {
  { std::hash<T>{}(value) } -> std::convertible_to<std::size_t>;
};

template <typename T>
concept complete = requires { sizeof(T); };

template <typename T>
concept streamable = requires(std::ostream& os, T value) {
  { os << value } -> std::same_as<std::ostream&>;
};

}  // namespace gw
