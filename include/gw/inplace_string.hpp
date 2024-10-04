// Copyright (c) 2023 Martin Stump
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <format>
#include <iterator>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

/// \brief GW namespace
///
namespace gw {

/// \example inplace_string_example.cpp
//
/// \brief A fixed-size string that stores the data in-place.
//
/// \tparam N The size of the string.
/// \tparam CharT The character type.
/// \tparam Traits The character traits type.
//
template <std::size_t N, class CharT, class Traits = std::char_traits<CharT>>
class basic_inplace_string {
 public:
  //
  // Public types
  //

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

  //
  // Constants
  //

  // NOLINTNEXTLINE(readability-identifier-naming)
  static constexpr size_type npos = -1;  ///< The maximum value for size_type.

  //
  // Constructors
  //

  /// \brief Default constructor.
  ///
  constexpr basic_inplace_string() noexcept = default;

  /// \brief Construct the string with `count` copies of character `ch`.
  ///
  /// \param count The number of characters to initialize the string with.
  /// \param ch The character to initialize the string with.
  /// \throw std::length_error If count is greater than `max_size`.
  ///
  constexpr basic_inplace_string(size_type count, CharT ch) {
    if (count > capacity()) {
      throw std::length_error{"basic_inplace_string::basic_inplace_string: count (which is " + std::to_string(count) +
                              ") > max_size (which is " + std::to_string(max_size()) + ")"};
    }

    std::fill_n(begin(), count, ch);
  }

  /// \brief Construct the string with the characters from the character string pointed to by `s`.
  ///
  /// \param s The character string to initialize the string with.
  /// \throw std::length_error If the size of the string would exceed `max_size`.
  ///
  constexpr explicit basic_inplace_string(const CharT* s) {
    const auto k_length = std::char_traits<CharT>::length(s);
    if (k_length > capacity()) {
      throw std::length_error{"basic_inplace_string::basic_inplace_string: s (which is " + std::to_string(k_length) +
                              ") > max_size (which is " + std::to_string(max_size()) + ")"};
    }

    std::copy_n(s, k_length, begin());
  }

  //
  // Destructor
  //

  ~basic_inplace_string() noexcept = default;

  //
  // Element access
  //

  /// \brief Get a reference to the character at the specified position.
  ///
  /// \param pos The position of the character to get.
  /// \return A reference to the character at the specified position.
  /// \throw std::out_of_range If `pos` is out of range.
  ///
  constexpr auto at(size_type pos) -> reference {
    return pos < size() ? m_data[pos]
                        : throw std::out_of_range{"basic_inplace_string::at: pos (which is " + std::to_string(pos) +
                                                  ") >= max_size (which is " + std::to_string(max_size()) + ")"};
  }

  /// \brief Get a const reference to the character at the specified position.
  ///
  /// \param pos The position of the character to get.
  /// \return A const reference to the character at the specified position.
  /// \throw std::out_of_range If `pos` is out of range.
  ///
  constexpr auto at(size_type pos) const -> const_reference {
    return pos < size() ? m_data[pos]
                        : throw std::out_of_range{"basic_inplace_string::at: pos (which is " + std::to_string(pos) +
                                                  ") >= max_size (which is " + std::to_string(max_size()) + ")"};
  }

  /// \brief Get a reference to the character at the specified position.
  ///
  /// \param pos The position of the character to get.
  /// \return A reference to the character at the specified position.
  ///
  constexpr auto operator[](size_type pos) noexcept -> reference { return m_data[pos]; }

  /// \brief Get a const reference to the character at the specified position.
  ///
  /// \param pos The position of the character to get.
  /// \return A const reference to the character at the specified position.
  ///
  constexpr auto operator[](size_type pos) const noexcept -> const_reference { return m_data[pos]; }

  /// \brief Get a reference to the first character in the string.
  ///
  /// \return A reference to the first character in the string.
  ///
  constexpr auto front() noexcept -> reference { return m_data[0U]; }

  /// \brief Get a const reference to the first character in the string.
  ///
  /// \return A const reference to the first character in the string.
  ///
  constexpr auto front() const noexcept -> const_reference { return m_data[0U]; }

  /// \brief Get a reference to the last character in the string.
  ///
  /// \return A reference to the last character in the string.
  ///
  constexpr auto back() noexcept -> reference { return m_data[size()]; }

  /// \brief Get a const reference to the last character in the string.
  ///
  /// \return A const reference to the last character in the string.
  ///
  constexpr auto back() const noexcept -> const_reference { return m_data[size() - 1U]; }

  /// \brief Get a pointer to the underlying character array.
  ///
  /// \return A pointer to the underlying character array.
  ///
  constexpr auto data() noexcept -> pointer { return m_data.data(); }

  /// \brief Get a const pointer to the underlying character array.
  ///
  /// \return A const pointer to the underlying character array.
  ///
  constexpr auto data() const noexcept -> const_pointer { return m_data.data(); }

  /// \brief Get a const pointer to the underlying character array.
  ///
  /// \return A const pointer to the underlying character array.
  ///
  constexpr auto c_str() const noexcept -> const_pointer { return m_data.data(); }

  /// \brief Get a string view of the string.
  ///
  /// \return A string view of the string.
  ///
  constexpr explicit operator std::basic_string_view<value_type>() const noexcept { return {data(), size()}; }

  //
  // Iterators
  //

  /// \brief Get an iterator to the beginning of the string.
  ///
  /// \return An iterator to the beginning of the string.
  ///
  constexpr auto begin() noexcept -> iterator { return data(); }

  /// \brief Get a const iterator to the beginning of the string.
  ///
  /// \return A const iterator to the beginning of the string.
  ///
  constexpr auto begin() const noexcept -> const_iterator { return data(); }

  /// \brief Get a const iterator to the beginning of the string.
  ///
  /// \return A const iterator to the beginning of the string.
  ///
  constexpr auto cbegin() const noexcept -> const_iterator { return data(); }

  /// \brief Get an iterator to the end of the string.
  ///
  /// \return An iterator to the end of the string.
  ///
  constexpr auto end() noexcept -> iterator { return std::next(data(), size()); }

  /// \brief Get a const iterator to the end of the string.
  ///
  /// \return A const iterator to the end of the string.
  ///
  constexpr auto end() const noexcept -> const_iterator { return std::next(data(), size()); }

  /// \brief Get a const iterator to the end of the string.
  ///
  /// \return A const iterator to the end of the string.
  ///
  constexpr auto cend() const noexcept -> const_iterator { return std::next(data(), size()); }

  /// \brief Get a reverse iterator to the end of the string.
  ///
  /// \return A reverse iterator to the end of the string.
  ///
  constexpr auto rbegin() noexcept -> reverse_iterator { return reverse_iterator{end()}; }

  /// \brief Get a const reverse iterator to the end of the string.
  ///
  /// \return A const reverse iterator to the end of the string.
  ///
  constexpr auto rbegin() const noexcept -> const_reverse_iterator { return const_reverse_iterator{end()}; }

  /// \brief Get a const reverse iterator to the end of the string.
  ///
  /// \return A const reverse iterator to the end of the string.
  ///
  constexpr auto crbegin() const noexcept -> const_reverse_iterator { return const_reverse_iterator{cend()}; }

  /// \brief Get a reverse iterator to the beginning of the string.
  ///
  /// \return A reverse iterator to the beginning of the string.
  ///
  constexpr auto rend() noexcept -> reverse_iterator { return reverse_iterator{begin()}; }

  /// \brief Get a const reverse iterator to the beginning of the string.
  ///
  /// \return A const reverse iterator to the beginning of the string.
  ///
  constexpr auto rend() const noexcept -> const_reverse_iterator { return const_reverse_iterator{begin()}; }

  /// \brief Get a const reverse iterator to the beginning of the string.
  ///
  /// \return A const reverse iterator to the beginning of the string.
  ///
  constexpr auto crend() const noexcept -> const_reverse_iterator { return const_reverse_iterator{cbegin()}; }

  //
  // Capacity
  //

  /// \brief Check if the string is empty.
  ///
  /// \return True if the string is empty, false otherwise.
  ///
  [[nodiscard]] constexpr auto empty() const noexcept -> bool { return size() == 0U; }

  /// \brief Get the size of the string.
  ///
  /// \return The size of the string.
  ///
  [[nodiscard]] constexpr auto size() const noexcept -> size_type { return std::char_traits<CharT>::length(data()); }

  /// \brief Get the length of the string.
  ///
  /// \return The length of the string.
  ///
  [[nodiscard]] constexpr auto length() const noexcept -> size_type { return size(); }

  /// \brief Get the maximum size of the string.
  ///
  /// \return The maximum size of the string.
  ///
  [[nodiscard]] inline constexpr auto max_size() const noexcept -> size_type { return N; }

  /// \brief Reserve storage for the string.
  ///
  /// \param new_cap The new capacity of the string.
  /// \throw std::length_error If `new_cap` is greater than `max_size`.
  ///
  constexpr void reserve(size_type new_cap) {
    if (new_cap > max_size()) {
      throw std::length_error{"basic_inplace_string::reserve: new_cap (which is " + std::to_string(new_cap) +
                              ") > max_size (which is " + std::to_string(max_size()) + ")"};
    }
  }

  /// \brief Get the capacity of the string.
  ///
  /// \return The capacity of the string.
  ///
  [[nodiscard]] constexpr auto capacity() const noexcept -> size_type { return max_size(); }

  /// \brief Shrink the capacity of the string to fit its size.
  ///
  /// \note This function does nothing.
  ///
  constexpr void shrink_to_fit() {};

  //
  // Modifiers
  //

  /// \brief Clear the contents of the string.
  ///
  constexpr void clear() noexcept { m_data.fill('\0'); }

  /// \brief Insert `count` copies of character `ch` at the position `index`.
  ///
  /// \param index The position to insert the characters at.
  /// \param count The number of characters to insert.
  /// \param ch The character to insert.
  /// \throw std::length_error If the size of the string would exceed `max_size`.
  ///
  constexpr void insert(size_type index, size_type count, CharT ch) {
    if (size() + count > max_size()) {
      throw std::length_error{"basic_inplace_string::insert: size() + count (which is " +
                              std::to_string(size() + count) + ") > max_size (which is " + std::to_string(max_size()) +
                              ")"};
    }

    std::copy_backward(begin() + index, end(), end() + count);
    std::fill_n(begin() + index, count, ch);
  }

  /// \brief Erase `count` characters from the position `index`.
  ///
  /// \param index The position to erase the characters from.
  /// \param count The number of characters to erase.
  /// \throw std::out_of_range If the `index` and `count` would exceed the size of the string.
  ///
  constexpr void erase(size_type index = 0U, size_type count = npos) {
    if (index + count > size()) {
      throw std::out_of_range{"basic_inplace_string::erase: index + count (which is " + std::to_string(index + count) +
                              ") > size() (which is " + std::to_string(size()) + ")"};
    }

    std::copy(std::next(begin(), index + count), end(), std::next(begin(), index));
    m_data[size() - count] = '\0';  // Add null terminator at the new end
  }

  /// \brief Append a character to the end of the string.
  ///
  /// \param ch The character to append.
  /// \throw std::length_error If the size of the string would exceed `max_size`.
  ///
  constexpr void push_back(CharT ch) {
    if (size() + 1U > max_size()) {
      throw std::length_error{"basic_inplace_string::push_back: size() + 1 (which is " + std::to_string(size() + 1U) +
                              ") > N (which is " + std::to_string(N) + ")"};
    }

    m_data[size()] = ch;
    m_data[size() + 1U] = '\0';  // Add null terminator at the new end
  }

  /// \brief Remove the last character from the string.
  ///
  constexpr void pop_back() noexcept { m_data[size() - 1U] = '\0'; }

  /// \brief Append a string to the end of the string.
  ///
  /// \param str The string to append.
  /// \throw std::length_error If the size of the string would exceed `max_size`.
  ///
  template <std::size_t N2>
  constexpr void append(const basic_inplace_string<N2, CharT>& str) {
    if (size() + str.size() > max_size()) {
      throw std::length_error{"basic_inplace_string::append: size() + str.size() (which is " +
                              std::to_string(size() + str.size()) + ") > max_size (which is " +
                              std::to_string(max_size()) + ")"};
    }

    std::copy(str.begin(), str.end(), end());
    m_data[size() + 1U] = '\0';  // Add null terminator at the new end
  }

  /// \brief Append a string to the end of the string.
  ///
  /// \param str The string to append.
  /// \throw std::length_error If the size of the string would exceed `max_size`.
  ///
  template <std::size_t N2>
  constexpr auto operator+=(const basic_inplace_string<N2, CharT>& str) -> basic_inplace_string& {
    if (size() + str.size() > max_size()) {
      throw std::length_error{"basic_inplace_string::operator+=: size() + str.size() (which is " +
                              std::to_string(size() + str.size()) + ") > max_size (which is " +
                              std::to_string(max_size()) + ")"};
    }

    std::copy(str.begin(), str.end(), end());
    return *this;
  }

  /// \brief Resize the string to `count` characters.
  ///
  /// \param count The new size of the string.
  /// \throw std::length_error If `count` is greater than `max_size`.
  ///
  constexpr void resize(size_type count) {
    if (count > max_size()) {
      throw std::length_error{"basic_inplace_string::resize: count (which is " + std::to_string(count) +
                              ") > max_size (which is " + std::to_string(max_size()) + ")"};
    }

    if (count > size()) {
      std::fill_n(end(), count - size(), '\0');
    }

    m_data[count] = '\0';  // Add null terminator at the new end
  }

  /// \brief Resize the string to `count` characters.
  ///
  /// \param count The new size of the string.
  /// \param ch The character to fill the string with.
  /// \throw std::length_error If `count` is greater than `max_size`.
  ///
  constexpr void resize(size_type count, CharT ch) {
    if (count > max_size()) {
      throw std::length_error{"basic_inplace_string::resize: count (which is " + std::to_string(count) +
                              ") > max_size (which is " + std::to_string(max_size()) + ")"};
    }

    if (count > size()) {
      std::fill_n(end(), count - size(), ch);
    }

    m_data[count] = '\0';  // Add null terminator at the new end
  }

  /// \brief Swap the string with another string.
  ///
  /// \param other The string to swap with.
  ///
  constexpr void swap(basic_inplace_string& other) noexcept {
    using std::swap;
    swap(m_data, other.m_data);
  }

  //
  // Search
  //

  /// \brief Find the first substring equal to `str`.
  ///
  /// \param str The string to find.
  /// \param pos The position to start searching from.
  /// \return The position of the first occurrence of the string, or `npos` if the string is not found.
  ///
  template <std::size_t N2>
  constexpr auto find(const basic_inplace_string<N2, CharT>& str, size_type pos = 0U) const noexcept -> size_type {
    return find(static_cast<std::string_view>(str), pos);
  }

  /// \brief Find the first substring equal to `str`.
  ///
  /// \param str The string view to find.
  /// \param pos The position to start searching from.
  /// \return The position of the first occurrence of the string, or `npos` if the string is not found.
  ///
  constexpr auto find(std::basic_string_view<CharT> str, size_type pos = 0U) const noexcept -> size_type {
    return static_cast<std::string_view>(*this).find(str, pos);
  }

  /// \brief Find the first substring equal to the range [`str`, `str + count`).
  ///
  /// \param str The string to find.
  /// \param pos The position to start searching from.
  /// \param count The number of characters to search for.
  /// \return The position of the first occurrence of the string, or `npos` if the string is not found.
  ///
  constexpr auto find(const char* str, size_type pos, size_type count) const noexcept -> size_type {
    return static_cast<std::string_view>(*this).find(str, pos, count);
  }

  /// \brief Find the first substring equal to the character string pointed to by `s`.
  ///
  /// \param str The string to find.
  /// \param pos The position to start searching from.
  /// \return The position of the first occurrence of the string, or `npos` if the string is not found.
  ///
  constexpr auto find(const char* str, size_type pos = 0U) const noexcept -> size_type {
    return static_cast<std::string_view>(*this).find(str, pos);
  }

  /// \brief Finds the first character `ch`.
  ///
  /// \param ch The character to find.
  /// \param pos The position to start searching from.
  /// \return The position of the first occurrence of the character, or `npos` if the character is not found.
  ///
  constexpr auto find(CharT ch, size_type pos = npos) const noexcept -> size_type {
    return static_cast<std::string_view>(*this).rfind(ch, pos);
  }

  /// \brief Find the last substring equal to `str`.
  ///
  /// \param str The string to find.
  /// \param pos The position to start searching from.
  /// \return The position of the last occurrence of the string, or `npos` if the string is not found.
  ///
  template <std::size_t N2>
  constexpr auto rfind(const basic_inplace_string<N2, CharT>& str, size_type pos = npos) const noexcept -> size_type {
    return rfind(static_cast<std::string_view>(str), pos);
  }

  /// \brief Find the last substring equal to `str`.
  ///
  /// \param str The string to find.
  /// \param pos The position to start searching from.
  /// \return The position of the last occurrence of the string, or `npos` if the string is not found.
  ///
  constexpr auto rfind(std::basic_string_view<CharT> str, size_type pos = npos) const noexcept -> size_type {
    return static_cast<std::string_view>(*this).rfind(str, pos);
  }

  /// \brief Find the last substring equal to the range [`str`, `str + count`).
  ///
  /// \param str The string to find.
  /// \param pos The position to start searching from.
  /// \param count The number of characters to search for.
  /// \return The position of the last occurrence of the string, or `npos` if the string is not found.
  ///
  constexpr auto rfind(const char* str, size_type pos, size_type count) const noexcept -> size_type {
    return static_cast<std::string_view>(*this).rfind(str, pos, count);
  }

  /// \brief Find the last substring equal to the character string pointed to by `s`.
  ///
  /// \param str The string to find.
  /// \param pos The position to start searching from.
  /// \return The position of the last occurrence of the string, or `npos` if the string is not found.
  ///
  constexpr auto rfind(const char* str, size_type pos = npos) const noexcept -> size_type {
    return static_cast<std::string_view>(*this).rfind(str, pos);
  }

  /// \brief Find the last character `ch`.
  ///
  /// \param ch The character to find.
  /// \param pos The position to start searching from.
  /// \return The position of the last occurrence of the character, or `npos` if the character is not found.
  ///
  constexpr auto rfind(CharT ch, size_type pos = npos) const noexcept -> size_type {
    return static_cast<std::string_view>(*this).rfind(ch, pos);
  }

  /// \brief Find the first character equal to one of the characters in `str`.
  ///
  /// \param str The string to search for.
  /// \param pos The position to start searching from.
  /// \return The position of the first occurrence of any character in the string, or `npos` if no character is found.
  ///
  template <std::size_t N2>
  constexpr auto find_first_of(const basic_inplace_string<N2, CharT>& str,
                               size_type pos = 0U) const noexcept -> size_type {
    return find_first_of(static_cast<std::string_view>(str), pos);
  }

  /// \brief Find the first character equal to one of the characters in `str`.
  ///
  /// \param str The string view to search for.
  /// \param pos The position to start searching from.
  /// \return The position of the first occurrence of any character in the string, or `npos` if no character is found.
  ///
  constexpr auto find_first_of(std::basic_string_view<CharT> str, size_type pos = 0U) const noexcept -> size_type {
    return static_cast<std::string_view>(*this).find_first_of(str, pos);
  }

  /// \brief Find the first character equal to one of the characters in `str`.
  ///
  /// \param str The character string to search for.
  /// \param pos The position to start searching from.
  /// \return The position of the first occurrence of any character in the string, or `npos` if no character is found.
  ///
  constexpr auto find_first_of(const char* str, size_type pos = 0U) const noexcept -> size_type {
    return static_cast<std::string_view>(*this).find_first_of(str, pos);
  }

  //
  // Non-member functions
  //

  /// \brief Concatenate two strings.
  ///
  /// \param lhs The first string to concatenate.
  /// \param rhs The second string to concatenate.
  /// \return The concatenated string.
  ///
  template <std::size_t N2>
  friend constexpr auto operator+(const basic_inplace_string& lhs,
                                  const basic_inplace_string<N2, CharT>& rhs) -> basic_inplace_string<N + N2, CharT> {
    basic_inplace_string<N + N2, CharT> result;
    std::copy(lhs.begin(), lhs.end(), result.begin());
    std::copy(rhs.begin(), rhs.end(), std::next(result.begin(), lhs.size()));
    return result;
  }

  //
  // Comparison
  //

  /// \brief Compare the string to another string.
  ///
  /// \param lhs The first string to compare.
  /// \param rhs The second string to compare.
  /// \return True if the strings are equal, false otherwise.
  ///
  friend constexpr auto operator==(const basic_inplace_string& lhs, const basic_inplace_string& rhs) noexcept -> bool {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
  }

  /// \brief Compare the string to a string view.
  ///
  /// \param lhs The first string to compare.
  /// \param rhs The second string to compare.
  /// \return True if the strings are equal, false otherwise.
  ///
  friend constexpr auto operator==(const basic_inplace_string& lhs,
                                   std::basic_string_view<value_type> rhs) noexcept -> bool {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
  }

  /// \brief Compare the string to a string view.
  ///
  /// \param lhs The first string to compare.
  /// \param rhs The second string to compare.
  /// \return True if the strings are equal, false otherwise.
  ///
  friend constexpr auto operator==(std::basic_string_view<value_type> lhs,
                                   const basic_inplace_string& rhs) noexcept -> bool {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
  }

  /// \brief Compares the string to a character string.
  ///
  /// \param lhs The first string to compare.
  /// \param rhs The second string to compare.
  /// \return True if the strings are equal, false otherwise.
  ///
  friend constexpr auto operator==(const basic_inplace_string& lhs, const value_type* rhs) noexcept -> bool {
    return static_cast<std::string_view>(lhs) == std::string_view{rhs};
  }

  /// \brief Compares the string to a character string.
  ///
  /// \param lhs The first string to compare.
  /// \param rhs The second string to compare.
  /// \return True if the strings are equal, false otherwise.
  ///
  friend constexpr auto operator==(const value_type* lhs, const basic_inplace_string& rhs) noexcept -> bool {
    return std::string_view{lhs} == static_cast<std::string_view>(rhs);
  }

 private:
  std::array<value_type, N + 1U> m_data{};
};

//
// Deduction guides
//

/// \brief Deduction guide for basic_inplace_string.
///
template <typename CharT, std::size_t N>
basic_inplace_string(const CharT (&)[N]) -> basic_inplace_string<N - 1U, CharT>;

//
// Type aliases
//

/// \brief A fixed-size string of `char` that stores the data in-place.
///
/// \tparam N The size of the string.
///
template <std::size_t N>
using inplace_string = basic_inplace_string<N, char>;

/// \brief A fixed-size string of `wchar_t` that stores the data in-place.
///
/// \tparam N The size of the string.
///
template <std::size_t N>
using inplace_wstring = basic_inplace_string<N, wchar_t>;

/// \brief A fixed-size string of `char16_t` that stores the data in-place.
///
/// \tparam N The size of the string.
///
template <std::size_t N>
using inplace_u16string = basic_inplace_string<N, char16_t>;

/// \brief A fixed-size string of `char32_t` that stores the data in-place.
///
/// \tparam N The size of the string.
///
template <std::size_t N>
using inplace_u32string = basic_inplace_string<N, char32_t>;

}  // namespace gw

namespace std {

/// \brief Format the inplace_string object.
///
template <std::size_t N, class CharT>
// NOLINTNEXTLINE(cert-dcl58-cpp)
struct formatter<::gw::basic_inplace_string<N, CharT>, CharT> {
  /// \brief Parse the format string.
  ///
  template <class ParseContext>
  constexpr auto parse(ParseContext& context) -> ParseContext::iterator {
    return context.begin();
  }

  /// \brief Format the inplace_string object.
  ///
  template <class FormatContext>
  auto format(const ::gw::basic_inplace_string<N, CharT>& str,
              FormatContext& context) const -> FormatContext::iterator {
    return format_to(context.out(), "{}", std::basic_string_view{str.data(), str.size()});
  }
};

}  // namespace std
