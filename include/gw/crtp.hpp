// Copyright (c) 2023 Martin Stump
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <type_traits>

namespace gw {

template <template <typename> class T, typename Derived>
concept crtp_concept = requires { std::is_base_of_v<T<Derived>, Derived>; };

template <template <typename> class T, typename Derived>
  requires crtp_concept<T, Derived>
struct crtp {
  auto self() noexcept -> Derived& { return static_cast<Derived&>(*this); }
  auto self() const noexcept -> Derived const& { return static_cast<Derived const&>(*this); }

 private:
  crtp() = default;
  friend T<Derived>;
};

}  // namespace gw
