// Copyright (c) 2023 Martin Stump
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <concepts>
#include <functional>

namespace gw {

template <typename T>
concept hashable = requires(T value) {
  { std::hash<T>{}(value) } -> std::convertible_to<std::size_t>;
};

}  // namespace gw
