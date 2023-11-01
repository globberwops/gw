// Copyright (c) 2023 Martin Stump
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <type_traits>

namespace gw {

template <template <typename> class T, typename Derived>
concept CrtpConcept = requires { std::is_base_of_v<T<Derived>, Derived>; };

template <template <typename> class T, typename Derived>
  requires CrtpConcept<T, Derived>
struct Crtp {
  auto Self() noexcept -> Derived& { return static_cast<Derived&>(*this); }
  auto Self() const noexcept -> Derived const& { return static_cast<Derived const&>(*this); }

 private:
  Crtp() = default;
  friend T<Derived>;
};

}  // namespace gw
