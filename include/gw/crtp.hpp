// Copyright (c) 2023 Martin Stump
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <concepts>
#include <type_traits>

namespace gw {

template <template <typename> class T, typename Derived>
  requires std::is_class_v<Derived> && std::same_as<Derived, std::remove_cv_t<Derived>>
struct crtp {
  constexpr auto self() noexcept -> Derived& {
    static_assert(std::derived_from<Derived, T<Derived>>);
    return static_cast<Derived&>(*this);
  }

  constexpr auto self() const noexcept -> Derived const& {
    static_assert(std::derived_from<Derived, T<Derived>>);
    return static_cast<const Derived&>(*this);
  }

  constexpr auto operator<=>(const crtp&) const noexcept = default;

 private:
  crtp() = default;
  friend T<Derived>;
};

}  // namespace gw
