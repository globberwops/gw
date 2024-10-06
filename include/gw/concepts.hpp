// Copyright (c) 2023 Martin Stump
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <concepts>
#include <cstddef>
#include <functional>
#include <iostream>
#include <string>
#include <type_traits>

/// \brief GW namespace
namespace gw {

/// \brief Concept for arithmetic types.
template <typename T>
concept arithmetic = requires { std::is_arithmetic_v<T>; };

/// \brief Concept for incrementable types.
template <typename T>
concept incrementable = requires(T value) {
  { ++value } -> std::common_with<T>;
  value++;
};

/// \brief Concept for decrementable types.
template <typename T>
concept decrementable = requires(T value) {
  { --value } -> std::common_with<T>;
  value--;
};

/// \brief Concept for complete types.
template <typename T>
concept complete = requires { sizeof(T); };

/// \brief Concept for hashable types.
template <typename T>
concept hashable = requires(T value) {
  { std::hash<T>{}(value) } -> std::same_as<std::size_t>;
};

/// \brief Concept for ostreamable types.
template <typename T>
concept ostreamable = requires(std::ostream& ostream, T value) {
  { ostream << value } -> std::same_as<std::ostream&>;
};

/// \brief Concept for istreamable types.
template <typename T>
concept istreamable = requires(std::istream& istream, T value) {
  { istream >> value } -> std::same_as<std::istream&>;
};

/// \brief Concept for named types.
template <typename T>
concept named = requires(T value) {
  { value.name() } -> std::convertible_to<std::string>;
};

}  // namespace gw
