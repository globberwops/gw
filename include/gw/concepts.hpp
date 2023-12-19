// Copyright (c) 2023 Martin Stump
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <concepts>
#include <type_traits>

#ifdef GW_ENABLE_HASH_CALCULATION
#include <cstddef>
#include <functional>
#endif  // GW_ENABLE_HASH_CALCULATION

#ifdef GW_ENABLE_STREAM_OPERATORS
#include <iostream>
#endif  // GW_ENABLE_STREAM_OPERATORS

#ifdef GW_ENABLE_STRING_CONVERSION
#include <string>
#endif  // GW_ENABLE_STRING_CONVERSION

namespace gw {

template <typename T>
concept arithmetic = requires { std::is_arithmetic_v<T>; };

template <typename T>
concept incrementable = requires(T value) {
  { ++value } -> std::common_with<T>;
  value++;
};

template <typename T>
concept decrementable = requires(T value) {
  { --value } -> std::common_with<T>;
  value--;
};

template <typename T>
concept complete = requires { sizeof(T); };

#ifdef GW_ENABLE_HASH_CALCULATION
template <typename T>
concept hashable = requires(T value) {
  { std::hash<T>{}(value) } -> std::same_as<std::size_t>;
};
#endif  // GW_ENABLE_HASH_CALCULATION

#ifdef GW_ENABLE_STREAM_OPERATORS
template <typename T>
concept ostreamable = requires(std::ostream& ostream, T value) {
  { ostream << value } -> std::same_as<std::ostream&>;
};

template <typename T>
concept istreamable = requires(std::istream& istream, T value) {
  { istream >> value } -> std::same_as<std::istream&>;
};
#endif  // GW_ENABLE_STREAM_OPERATORS

#ifdef GW_ENABLE_STRING_CONVERSION
template <typename T>
concept named = requires(T value) {
  { value.name() } -> std::convertible_to<std::string>;
};

template <typename T>
concept string_convertable = requires(T value) {
  { std::to_string(value) } -> std::same_as<std::string>;
};
#endif  // GW_ENABLE_STRING_CONVERSION

}  // namespace gw
