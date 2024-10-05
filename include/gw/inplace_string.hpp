// Copyright (c) 2023 Martin Stump
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <algorithm>
#include <array>
#include <concepts>
#include <cstddef>
#include <format>
#include <iostream>
#include <iterator>
#include <ostream>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

/// \brief GW namespace
namespace gw {

/// \example inplace_string_example.cpp
//
/// \brief A fixed-size string that stores the data in-place.
/// \tparam N The size of the string.
/// \tparam CharT The character type.
/// \tparam Traits The character traits type.
template <std::size_t N, class CharT, class Traits = std::char_traits<CharT>>
class basic_inplace_string {
 public:
  using traits_type = Traits;                                            ///< The character traits type.
  using value_type = CharT;                                              ///< The character type.
  using size_type = std::size_t;                                         ///< The size type.
  using difference_type = std::ptrdiff_t;                                ///< The difference type.
  using reference = value_type&;                                         ///< The reference type.
  using const_reference = const value_type&;                             ///< The const reference type.
  using pointer = value_type*;                                           ///< The pointer type.
  using const_pointer = const value_type*;                               ///< The const pointer type.
  using iterator = value_type*;                                          ///< The iterator type.
  using const_iterator = const value_type*;                              ///< The const iterator type.
  using reverse_iterator = std::reverse_iterator<iterator>;              ///< The reverse iterator type.
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;  ///< The const reverse iterator type.

  /// \brief The character array.
  /// \note The array must be public to allow use of `basic_inplace_string` as a non-type template parameter.
  std::array<value_type, N + 1U> m_data;

  // NOLINTNEXTLINE(readability-identifier-naming)
  static constexpr size_type npos = -1;  ///< The maximum value for size_type.

  /// \brief Default constructor.
  constexpr basic_inplace_string() noexcept = default;

  /// \brief Construct the string with the characters from the character string pointed to by `str`.
  template <std::size_t N2>
  // NOLINTNEXTLINE
  consteval explicit(false) basic_inplace_string(const value_type (&str)[N2]) noexcept
    requires(N2 <= N + 1)
      : m_data{} {
    traits_type::copy(data(), str, N2 - 1);
  }

  /// \brief Construct the string with `count` copies of character `ch`.
  /// \param count The number of characters to initialize the string with.
  /// \param ch The character to initialize the string with.
  /// \throw std::length_error If count is greater than `max_size`.
  constexpr basic_inplace_string(size_type count, value_type ch) : m_data{} {
    if (count > max_size()) {
      throw std::length_error{
          std::format("basic_inplace_string::basic_inplace_string: count (which is {}) > max_size (which is {})", count,
                      max_size())};
    }
    std::fill_n(begin(), count, ch);
  }

  /// \brief Construct the string with the characters from the character string pointed to by `str`.
  /// \param str The character string to initialize the string with.
  /// \throw std::length_error If the size of `str` would exceed `max_size`.
  constexpr explicit basic_inplace_string(const value_type* str) : m_data{} {
    const auto str_size = traits_type::length(str);

    if (str_size > max_size()) {
      throw std::length_error{
          std::format("basic_inplace_string::basic_inplace_string: str_size (which is {}) > max_size (which is {})",
                      str_size, max_size())};
    }

    traits_type::copy(begin(), str, str_size);
  }

  /// \brief Construct the string with the contents of the range [str, str + count).
  /// \param str The beginning of the range.
  /// \param count The number of characters to initialize the string with.
  /// \throw std::length_error If `count` is greater than `max_size`.
  constexpr explicit basic_inplace_string(const value_type* str, size_type count) : m_data{} {
    if (count > max_size()) {
      throw std::length_error{
          std::format("basic_inplace_string::basic_inplace_string: count (which is {}) > max_size (which is {})", count,
                      max_size())};
    }
    traits_type::copy(begin(), str, count);
  }

  /// \brief Construct the string with the contents of the range [first, last).
  /// \tparam InputIt The type of the iterators.
  /// \param first The beginning of the range.
  /// \param last The end of the range.
  /// \throw std::length_error If the size of the range would exceed `max_size`.
  template <std::input_iterator InputIt>
  constexpr explicit basic_inplace_string(InputIt first, InputIt last) : m_data{} {
    const auto str_size = std::distance(first, last);

    if (str_size > max_size()) {
      throw std::length_error{
          std::format("basic_inplace_string::basic_inplace_string: str_size (which is {}) > max_size (which is {})",
                      str_size, max_size())};
    }

    std::copy(first, last, begin());
  }

  /// \brief Construct the string with the contents of the string view.
  /// \param str The string view to initialize the string with.
  /// \throw std::length_error If the size of the string view would exceed `max_size`.
  constexpr explicit basic_inplace_string(const std::basic_string_view<value_type, traits_type>& str)
      : basic_inplace_string(str.data(), str.size()) {}

  /// \brief Construct the string with the contents of the range.
  /// \tparam R The type of the range.
  /// \param range The range to initialize the string with.
  template <std::ranges::range R>
    requires(!std::is_convertible_v<const R&, std::basic_string_view<value_type, traits_type>> &&
             std::convertible_to<std::ranges::range_value_t<R>, value_type>)
  constexpr explicit basic_inplace_string(const R& range) : basic_inplace_string(std::begin(range), std::end(range)) {}

  /// \brief Copy constructor.
  constexpr basic_inplace_string(const basic_inplace_string& other) noexcept = default;

  /// \brief Move constructor.
  constexpr basic_inplace_string(basic_inplace_string&& other) noexcept = default;

  /// \brief Destructor.
  ~basic_inplace_string() noexcept = default;

  /// \brief Copy assignment operator.
  constexpr auto operator=(const basic_inplace_string& other) noexcept -> basic_inplace_string& = default;

  /// \brief Move assignment operator.
  constexpr auto operator=(basic_inplace_string&& other) noexcept -> basic_inplace_string& = default;

  /// \brief Get a reference to the character at the specified position.
  /// \param pos The position of the character to get.
  /// \return A reference to the character at the specified position.
  /// \throw std::out_of_range If `pos` is out of range.
  constexpr auto at(size_type pos) -> reference {
    return pos < size()
               ? m_data[pos]
               : throw std::out_of_range{std::format(
                     "basic_inplace_string::at: pos (which is {}) >= max_size (which is {})", pos, max_size())};
  }

  /// \brief Get a const reference to the character at the specified position.
  /// \param pos The position of the character to get.
  /// \return A const reference to the character at the specified position.
  /// \throw std::out_of_range If `pos` is out of range.
  constexpr auto at(size_type pos) const -> const_reference {
    return pos < size()
               ? m_data[pos]
               : throw std::out_of_range{std::format(
                     "basic_inplace_string::at: pos (which is {}) >= max_size (which is {})", pos, max_size())};
  }

  /// \brief Get a reference to the character at the specified position.
  /// \param pos The position of the character to get.
  /// \return A reference to the character at the specified position.
  constexpr auto operator[](size_type pos) noexcept -> reference { return m_data[pos]; }

  /// \brief Get a const reference to the character at the specified position.
  /// \param pos The position of the character to get.
  /// \return A const reference to the character at the specified position.
  constexpr auto operator[](size_type pos) const noexcept -> const_reference { return m_data[pos]; }

  /// \brief Get a reference to the first character in the string.
  /// \return A reference to the first character in the string.
  constexpr auto front() noexcept -> reference { return m_data[0]; }

  /// \brief Get a const reference to the first character in the string.
  /// \return A const reference to the first character in the string.
  constexpr auto front() const noexcept -> const_reference { return m_data[0]; }

  /// \brief Get a reference to the last character in the string.
  /// \return A reference to the last character in the string.
  constexpr auto back() noexcept -> reference { return m_data[size() - 1]; }

  /// \brief Get a const reference to the last character in the string.
  /// \return A const reference to the last character in the string.
  constexpr auto back() const noexcept -> const_reference { return m_data[size() - 1]; }

  /// \brief Get a pointer to the underlying character array.
  /// \return A pointer to the underlying character array.
  constexpr auto data() noexcept -> pointer { return m_data.data(); }

  /// \brief Get a const pointer to the underlying character array.
  /// \return A const pointer to the underlying character array.
  constexpr auto data() const noexcept -> const_pointer { return m_data.data(); }

  /// \brief Get a const pointer to the underlying character array.
  /// \return A const pointer to the underlying character array.
  constexpr auto c_str() const noexcept -> const_pointer { return m_data.data(); }

  /// \brief Get a string view of the string.
  /// \return A string view of the string.
  constexpr explicit operator std::basic_string_view<value_type>() const noexcept { return {data(), size()}; }

  /// \brief Get an iterator to the beginning of the string.
  /// \return An iterator to the beginning of the string.
  constexpr auto begin() noexcept -> iterator { return data(); }

  /// \brief Get a const iterator to the beginning of the string.
  /// \return A const iterator to the beginning of the string.
  constexpr auto begin() const noexcept -> const_iterator { return data(); }

  /// \brief Get a const iterator to the beginning of the string.
  /// \return A const iterator to the beginning of the string.
  constexpr auto cbegin() const noexcept -> const_iterator { return data(); }

  /// \brief Get an iterator to the end of the string.
  /// \return An iterator to the end of the string.
  constexpr auto end() noexcept -> iterator { return std::next(data(), size()); }

  /// \brief Get a const iterator to the end of the string.
  /// \return A const iterator to the end of the string.
  constexpr auto end() const noexcept -> const_iterator { return std::next(data(), size()); }

  /// \brief Get a const iterator to the end of the string.
  /// \return A const iterator to the end of the string.
  constexpr auto cend() const noexcept -> const_iterator { return std::next(data(), size()); }

  /// \brief Get a reverse iterator to the end of the string.
  /// \return A reverse iterator to the end of the string.
  constexpr auto rbegin() noexcept -> reverse_iterator { return reverse_iterator{end()}; }

  /// \brief Get a const reverse iterator to the end of the string.
  /// \return A const reverse iterator to the end of the string.
  constexpr auto rbegin() const noexcept -> const_reverse_iterator { return const_reverse_iterator{end()}; }

  /// \brief Get a const reverse iterator to the end of the string.
  /// \return A const reverse iterator to the end of the string.
  constexpr auto crbegin() const noexcept -> const_reverse_iterator { return const_reverse_iterator{cend()}; }

  /// \brief Get a reverse iterator to the beginning of the string.
  /// \return A reverse iterator to the beginning of the string.
  constexpr auto rend() noexcept -> reverse_iterator { return reverse_iterator{begin()}; }

  /// \brief Get a const reverse iterator to the beginning of the string.
  /// \return A const reverse iterator to the beginning of the string.
  constexpr auto rend() const noexcept -> const_reverse_iterator { return const_reverse_iterator{begin()}; }

  /// \brief Get a const reverse iterator to the beginning of the string.
  /// \return A const reverse iterator to the beginning of the string.
  constexpr auto crend() const noexcept -> const_reverse_iterator { return const_reverse_iterator{cbegin()}; }

  /// \brief Check if the string is empty.
  /// \return True if the string is empty, false otherwise.
  [[nodiscard]] constexpr auto empty() const noexcept -> bool { return size() == 0U; }

  /// \brief Get the size of the string.
  /// \return The size of the string.
  [[nodiscard]] constexpr auto size() const noexcept -> size_type { return traits_type::length(data()); }

  /// \brief Get the length of the string.
  /// \return The length of the string.
  [[nodiscard]] constexpr auto length() const noexcept -> size_type { return size(); }

  /// \brief Get the maximum size of the string.
  /// \return The maximum size of the string.
  [[nodiscard]] inline constexpr auto max_size() const noexcept -> size_type { return N; }

  /// \brief Reserve storage for the string.
  /// \param new_cap The new capacity of the string.
  /// \throw std::length_error If `new_cap` is greater than `max_size`.
  constexpr void reserve(size_type new_cap) {
    if (new_cap > max_size()) {
      throw std::length_error{std::format(
          "basic_inplace_string::reserve: new_cap (which is {}) > max_size (which is {})", new_cap, max_size())};
    }
  }

  /// \brief Get the capacity of the string.
  /// \return The capacity of the string.
  [[nodiscard]] constexpr auto capacity() const noexcept -> size_type { return max_size(); }

  /// \brief Shrink the capacity of the string to fit its size.
  /// \note This function does nothing.
  constexpr void shrink_to_fit() {}

  /// \brief Clear the contents of the string.
  constexpr void clear() noexcept { m_data[0] = value_type{}; }

  /// \brief Insert `count` copies of character `ch` at the position `index`.
  /// \param index The position to insert the characters at.
  /// \param count The number of characters to insert.
  /// \param ch The character to insert.
  /// \throw std::length_error If the size of the string would exceed `max_size`.
  constexpr void insert(size_type index, size_type count, CharT ch) {
    const auto projected_size = size() + count;
    if (projected_size > max_size()) {
      throw std::length_error{
          std::format("basic_inplace_string::insert: projected_size (which is {}) > max_size (which is {})",
                      projected_size, max_size())};
    }
    std::copy_backward(std::next(begin(), index), end(), std::next(end(), count));
    std::fill_n(std::next(begin(), index), count, ch);
    m_data[projected_size] = value_type{};  // Ensure null termination
  }

  /// \brief Erase `count` characters from the position `index`.
  /// \param index The position to erase the characters from.
  /// \param count The number of characters to erase.
  /// \throw std::out_of_range If `index` is greater than or equal to the size of the string.
  constexpr void erase(size_type index = 0U, size_type count = npos) {
    if (index >= size()) {
      throw std::out_of_range{
          std::format("basic_inplace_string::erase: index (which is {}) >= size (which is {})", index, size())};
    }
    const auto no_of_chars_to_erase = std::min(count, size() - index);
    const auto projected_size = size() - no_of_chars_to_erase;
    std::copy(std::next(begin(), index + count), end(), std::next(begin(), index));
    m_data[projected_size] = value_type{};  // Ensure null termination
  }

  /// \brief Append a character to the end of the string.
  /// \param ch The character to append.
  /// \throw std::length_error If the size of the string would exceed `max_size`.
  constexpr void push_back(value_type ch) {
    const auto projected_size = size() + 1;
    if (projected_size > max_size()) {
      throw std::length_error{
          std::format("basic_inplace_string::push_back: projected_size (which is {}) > max_size (which is {})",
                      projected_size, max_size())};
    }
    m_data[size()] = ch;
    m_data[projected_size] = value_type{};  // Ensure null termination
  }

  /// \brief Remove the last character from the string.
  constexpr void pop_back() noexcept { m_data[size() - 1] = value_type{}; }

  /// \brief Append a string to the end of the string.
  /// \tparam N2 The size of the string.
  /// \param str The string to append.
  /// \throw std::length_error If the size of the string would exceed `max_size`.
  template <std::size_t N2>
  constexpr void append(const basic_inplace_string<N2, value_type, traits_type>& str) {
    const auto projected_size = size() + str.size();
    if (projected_size > max_size()) {
      throw std::length_error{
          std::format("basic_inplace_string::append: projected_size (which is {}) > max_size (which is {})",
                      projected_size, max_size())};
    }
    traits_type::copy(end(), str.data(), str.size());
    m_data[projected_size] = value_type{};  // Ensure null termination
  }

  /// \brief Append a string to the end of the string.
  /// \tparam N2 The size of the string.
  /// \param str The string to append.
  /// \throw std::length_error If the size of the string would exceed `max_size`.
  template <std::size_t N2>
  constexpr auto operator+=(const basic_inplace_string<N2, value_type, traits_type>& str) -> basic_inplace_string& {
    const auto projected_size = size() + str.size();
    if (projected_size > max_size()) {
      throw std::length_error{
          std::format("basic_inplace_string::operator+=: projected_size (which is {}) > max_size (which is {})",
                      projected_size, max_size())};
    }
    traits_type::copy(end(), str.data(), str.size());
    m_data[projected_size] = value_type{};  // Ensure null termination
    return *this;
  }

  /// \brief Resize the string to `count` characters.
  /// \param count The new size of the string.
  /// \throw std::length_error If `count` is greater than `max_size`.
  constexpr void resize(size_type count) {
    if (count > max_size()) {
      throw std::length_error{
          std::format("basic_inplace_string::resize: count (which is {}) > max_size (which is {})", count, max_size())};
    }
    m_data[count] = value_type{};  // Ensure null termination
  }

  /// \brief Resize the string to `count` characters.
  /// \param count The new size of the string.
  /// \param ch The character to fill the string with.
  /// \throw std::length_error If `count` is greater than `max_size`.
  constexpr void resize(size_type count, value_type ch) {
    if (count > max_size()) {
      throw std::length_error{
          std::format("basic_inplace_string::resize: count (which is {}) > max_size (which is {})", count, max_size())};
    }
    if (count > size()) {
      std::fill_n(end(), count - size(), ch);
    }
    m_data[count] = value_type{};  // Ensure null termination
  }

  /// \brief Swap the string with another string.
  /// \param other The string to swap with.
  constexpr void swap(basic_inplace_string& other) noexcept {
    using std::swap;
    swap(m_data, other.m_data);
  }

  /// \brief Find the first substring equal to `str`.
  /// \tparam N2 The size of the string.
  /// \param str The string to find.
  /// \param pos The position to start searching from.
  /// \return The position of the first occurrence of the string, or `npos` if the string is not found.
  template <std::size_t N2>
  constexpr auto find(const basic_inplace_string<N2, value_type, traits_type>& str,
                      size_type pos = 0) const noexcept -> size_type {
    return find(static_cast<std::basic_string_view<value_type, traits_type>>(str), pos);
  }

  /// \brief Find the first substring equal to `str`.
  /// \param str The string view to find.
  /// \param pos The position to start searching from.
  /// \return The position of the first occurrence of the string, or `npos` if the string is not found.
  constexpr auto find(std::basic_string_view<value_type, traits_type> str,
                      size_type pos = 0) const noexcept -> size_type {
    return static_cast<std::basic_string_view<value_type, traits_type>>(*this).find(str, pos);
  }

  /// \brief Find the first substring equal to the range [`str`, `str + count`).
  /// \param str The string to find.
  /// \param pos The position to start searching from.
  /// \param count The number of characters to search for.
  /// \return The position of the first occurrence of the string, or `npos` if the string is not found.
  constexpr auto find(const value_type* str, size_type pos, size_type count) const noexcept -> size_type {
    return static_cast<std::basic_string_view<value_type, traits_type>>(*this).find(str, pos, count);
  }

  /// \brief Find the first substring equal to the character string pointed to by `str`.
  /// \param str The string to find.
  /// \param pos The position to start searching from.
  /// \return The position of the first occurrence of the string, or `npos` if the string is not found.
  constexpr auto find(const value_type* str, size_type pos = 0) const noexcept -> size_type {
    return static_cast<std::basic_string_view<value_type, traits_type>>(*this).find(str, pos);
  }

  /// \brief Find the first character `ch`.
  /// \param ch The character to find.
  /// \param pos The position to start searching from.
  /// \return The position of the first occurrence of the character, or `npos` if the character is not found.
  constexpr auto find(value_type ch, size_type pos = npos) const noexcept -> size_type {
    return static_cast<std::basic_string_view<value_type, traits_type>>(*this).rfind(ch, pos);
  }

  /// \brief Find the last substring equal to `str`.
  /// \tparam N2 The size of the string.
  /// \param str The string to find.
  /// \param pos The position to start searching from.
  /// \return The position of the last occurrence of the string, or `npos` if the string is not found.
  template <std::size_t N2>
  constexpr auto rfind(const basic_inplace_string<N2, value_type, traits_type>& str,
                       size_type pos = npos) const noexcept -> size_type {
    return rfind(static_cast<std::basic_string_view<value_type, traits_type>>(str), pos);
  }

  /// \brief Find the last substring equal to `str`.
  /// \param str The string to find.
  /// \param pos The position to start searching from.
  /// \return The position of the last occurrence of the string, or `npos` if the string is not found.
  constexpr auto rfind(std::basic_string_view<value_type, traits_type> str,
                       size_type pos = npos) const noexcept -> size_type {
    return static_cast<std::basic_string_view<value_type, traits_type>>(*this).rfind(str, pos);
  }

  /// \brief Find the last substring equal to the range [`str`, `str + count`).
  /// \param str The string to find.
  /// \param pos The position to start searching from.
  /// \param count The number of characters to search for.
  /// \return The position of the last occurrence of the string, or `npos` if the string is not found.
  constexpr auto rfind(const value_type* str, size_type pos, size_type count) const noexcept -> size_type {
    return static_cast<std::basic_string_view<value_type, traits_type>>(*this).rfind(str, pos, count);
  }

  /// \brief Find the last substring equal to the character string pointed to by `str`.
  /// \param str The string to find.
  /// \param pos The position to start searching from.
  /// \return The position of the last occurrence of the string, or `npos` if the string is not found.
  constexpr auto rfind(const value_type* str, size_type pos = npos) const noexcept -> size_type {
    return static_cast<std::basic_string_view<value_type, traits_type>>(*this).rfind(str, pos);
  }

  /// \brief Find the last character `ch`.
  /// \param ch The character to find.
  /// \param pos The position to start searching from.
  /// \return The position of the last occurrence of the character, or `npos` if the character is not found.
  constexpr auto rfind(value_type ch, size_type pos = npos) const noexcept -> size_type {
    return static_cast<std::basic_string_view<value_type, traits_type>>(*this).rfind(ch, pos);
  }

  /// \brief Find the first character equal to one of the characters in `str`.
  /// \tparam N2 The size of the string.
  /// \param str The string to search for.
  /// \param pos The position to start searching from.
  /// \return The position of the first occurrence of any character in the string, or `npos` if no character is found.
  template <std::size_t N2>
  constexpr auto find_first_of(const basic_inplace_string<N2, value_type, traits_type>& str,
                               size_type pos = 0) const noexcept -> size_type {
    return find_first_of(static_cast<std::basic_string_view<value_type, traits_type>>(str), pos);
  }

  /// \brief Find the first character equal to one of the characters in `str`.
  /// \param str The string view to search for.
  /// \param pos The position to start searching from.
  /// \return The position of the first occurrence of any character in the string, or `npos` if no character is found.
  constexpr auto find_first_of(std::basic_string_view<value_type, traits_type> str,
                               size_type pos = 0) const noexcept -> size_type {
    return static_cast<std::basic_string_view<value_type, traits_type>>(*this).find_first_of(str, pos);
  }

  /// \brief Find the first character equal to one of the characters in `str`.
  /// \param str The character string to search for.
  /// \param pos The position to start searching from.
  /// \return The position of the first occurrence of any character in the string, or `npos` if no character is found.
  constexpr auto find_first_of(const value_type* str, size_type pos = 0) const noexcept -> size_type {
    return static_cast<std::basic_string_view<value_type, traits_type>>(*this).find_first_of(str, pos);
  }

  /// \brief Concatenate two strings.
  /// \tparam N2 The size of the second string.
  /// \param lhs The first string to concatenate.
  /// \param rhs The second string to concatenate.
  /// \return The concatenated string.
  template <std::size_t N2>
  friend constexpr auto operator+(const basic_inplace_string& lhs,
                                  const basic_inplace_string<N2, value_type, traits_type>& rhs)
      -> basic_inplace_string<N + N2, value_type, traits_type> {
    basic_inplace_string<N + N2, value_type, traits_type> result;
    traits_type::copy(result.data(), lhs.data(), lhs.size());
    traits_type::copy(std::next(result.data(), lhs.size()), rhs.data(), rhs.size());
    result[lhs.size() + rhs.size()] = value_type{};  // Ensure null termination
    return result;
  }

  /// \brief Compare the string to another string.
  /// \param lhs The first string to compare.
  /// \param rhs The second string to compare.
  /// \return True if the strings are equal, false otherwise.
  friend constexpr auto operator==(const basic_inplace_string& lhs, const basic_inplace_string& rhs) noexcept -> bool {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
  }

  /// \brief Compare the string to a string view.
  /// \param lhs The first string to compare.
  /// \param rhs The second string to compare.
  /// \return True if the strings are equal, false otherwise.
  friend constexpr auto operator==(const basic_inplace_string& lhs,
                                   std::basic_string_view<value_type, traits_type> rhs) noexcept -> bool {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
  }

  /// \brief Compare the string to a string view.
  /// \param lhs The first string to compare.
  /// \param rhs The second string to compare.
  /// \return True if the strings are equal, false otherwise.
  friend constexpr auto operator==(std::basic_string_view<value_type, traits_type> lhs,
                                   const basic_inplace_string& rhs) noexcept -> bool {
    return rhs == lhs;
  }

  /// \brief Compares the string to a character string.
  /// \param lhs The first string to compare.
  /// \param rhs The second string to compare.
  /// \return True if the strings are equal, false otherwise.
  friend constexpr auto operator==(const basic_inplace_string& lhs, const value_type* rhs) noexcept -> bool {
    return traits_type::compare(lhs.data(), rhs, traits_type::length(rhs)) == 0;
  }

  /// \brief Compares the string to a character string.
  /// \param lhs The first string to compare.
  /// \param rhs The second string to compare.
  /// \return True if the strings are equal, false otherwise.
  friend constexpr auto operator==(const value_type* lhs, const basic_inplace_string& rhs) noexcept -> bool {
    return rhs == lhs;
  }

  /// \brief Compare the string to another string.
  /// \param lhs The first string to compare.
  /// \param rhs The second string to compare.
  /// \return True if the strings are not equal, false otherwise.
  friend constexpr auto operator!=(const basic_inplace_string& lhs, const basic_inplace_string& rhs) noexcept -> bool {
    return !(lhs == rhs);
  }

  /// \brief Compare the string to a string view.
  /// \param lhs The first string to compare.
  /// \param rhs The second string to compare.
  /// \return True if the strings are not equal, false otherwise.
  friend constexpr auto operator!=(const basic_inplace_string& lhs,
                                   std::basic_string_view<value_type, traits_type> rhs) noexcept -> bool {
    return !(lhs == rhs);
  }

  /// \brief Compare the string to a string view.
  /// \param lhs The first string to compare.
  /// \param rhs The second string to compare.
  /// \return True if the strings are not equal, false otherwise.
  friend constexpr auto operator!=(std::basic_string_view<value_type, traits_type> lhs,
                                   const basic_inplace_string& rhs) noexcept -> bool {
    return !(lhs == rhs);
  }

  /// \brief Compares the string to a character string.
  /// \param lhs The first string to compare.
  /// \param rhs The second string to compare.
  /// \return True if the strings are not equal, false otherwise.
  friend constexpr auto operator!=(const basic_inplace_string& lhs, const value_type* rhs) noexcept -> bool {
    return !(lhs == rhs);
  }

  /// \brief Compares the string to a character string.
  /// \param lhs The first string to compare.
  /// \param rhs The second string to compare.
  /// \return True if the strings are not equal, false otherwise.
  friend constexpr auto operator!=(const value_type* lhs, const basic_inplace_string& rhs) noexcept -> bool {
    return !(lhs == rhs);
  }

  /// \brief Write the string to an output stream.
  /// \param ostream The output stream to write to.
  /// \param rhs The string to write.
  /// \return The output stream.
  friend inline auto operator<<(std::basic_ostream<value_type, traits_type>& ostream,
                                const basic_inplace_string& rhs) -> std::basic_ostream<value_type, traits_type>& {
    return ostream << static_cast<std::basic_string_view<value_type, traits_type>>(rhs);
  }

  /// \brief Read the string from an input stream.
  /// \param istream The input stream to read from.
  /// \param rhs The string to read into.
  /// \return The input stream.
  friend inline auto operator>>(std::basic_istream<value_type, traits_type>& istream,
                                basic_inplace_string& rhs) -> std::basic_istream<value_type, traits_type>& {
    const auto projected_size = rhs.size() + istream.rdbuf()->in_avail();
    if (projected_size > rhs.max_size()) {
      throw std::length_error{
          std::format("basic_inplace_string::operator>>: projected_size (which is {}) > max_size (which is {})",
                      projected_size, rhs.max_size())};
    }
    const auto it = std::istreambuf_iterator<value_type, traits_type>{istream};
    const auto end = std::istreambuf_iterator<value_type, traits_type>{};
    std::copy(it, end, rhs.end());
    rhs[projected_size] = value_type{};  // Ensure null termination
    return istream;
  }
};

/// \brief Deduction guide for basic_inplace_string.
template <typename CharT, std::size_t N>
// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
basic_inplace_string(const CharT (&)[N]) -> basic_inplace_string<N - 1U, CharT>;

/// \brief A fixed-size string of `char` that stores the data in-place.
template <std::size_t N>
using inplace_string = basic_inplace_string<N, char>;

/// \brief A fixed-size string of `wchar_t` that stores the data in-place.
template <std::size_t N>
using inplace_wstring = basic_inplace_string<N, wchar_t>;

/// \brief A fixed-size string of `char16_t` that stores the data in-place.
template <std::size_t N>
using inplace_u16string = basic_inplace_string<N, char16_t>;

/// \brief A fixed-size string of `char32_t` that stores the data in-place.
template <std::size_t N>
using inplace_u32string = basic_inplace_string<N, char32_t>;

}  // namespace gw

namespace std {

/// \brief Hash support for `inplace_string`.
template <std::size_t N, class CharT>
// NOLINTNEXTLINE(cert-dcl58-cpp)
struct hash<::gw::basic_inplace_string<N, CharT>> {
  /// \brief Calculate the hash of the `inplace_string` object.
  [[nodiscard]] auto inline operator()(const ::gw::basic_inplace_string<N, CharT>& str) const noexcept -> size_t {
    return hash<string_view>{}(static_cast<std::basic_string_view<CharT>>(str));
  }
};

/// \brief Format the `inplace_string` object.
template <std::size_t N, class CharT>
// NOLINTNEXTLINE(cert-dcl58-cpp)
struct formatter<::gw::basic_inplace_string<N, CharT>, CharT> {
  /// \brief Parse the format string.
  template <class ParseContext>
  constexpr auto parse(ParseContext& context) const -> ParseContext::iterator {
    return context.begin();
  }

  /// \brief Format the `inplace_string` object.
  template <class FormatContext>
  constexpr auto format(const ::gw::basic_inplace_string<N, CharT>& str,
                        FormatContext& context) const -> FormatContext::iterator {
    return std::ranges::copy(static_cast<std::basic_string_view<CharT>>(str), context.out()).out;
  }
};

}  // namespace std
