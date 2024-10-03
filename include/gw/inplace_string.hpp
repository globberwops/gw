// Copyright (c) 2023 Martin Stump
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <string_view>

/// \brief GW namespace
///
namespace gw {

/// \brief A fixed-size string that stores the data in-place.
///
/// \tparam N The size of the string.
/// \tparam CharT The character type.
/// \tparam Traits The character traits type.
///
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
  constexpr basic_inplace_string() noexcept { m_data.fill('\0'); }

  /// \brief Construct the string with count copies of character ch.
  ///
  /// \param count The number of characters to initialize the string with.
  /// \param ch The character to initialize the string with.
  ///
  constexpr basic_inplace_string(size_type count, CharT ch) {
    if (count > capacity()) {
      throw std::length_error{"basic_inplace_string::basic_inplace_string"};
    }

    m_data.fill('\0');
    std::fill_n(begin(), count, ch);
  }

  /// \brief Construct the string with the characters from the character string pointed to by s.
  ///
  /// \param s The character string to initialize the string with.
  ///
  constexpr explicit basic_inplace_string(const CharT* s) {
    const auto k_length = std::char_traits<CharT>::length(s);
    if (k_length > capacity()) {
      throw std::length_error{"basic_inplace_string::basic_inplace_string"};
    }

    m_data.fill('\0');
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
  /// \throw std::out_of_range If pos is out of range.
  ///
  constexpr auto at(size_type pos) -> reference {
    return pos < size() ? m_data[pos + 1UZ] : throw std::out_of_range{"basic_inplace_string::at"};
  }

  /// \brief Get a const reference to the character at the specified position.
  ///
  /// \param pos The position of the character to get.
  /// \return A const reference to the character at the specified position.
  /// \throw std::out_of_range If pos is out of range.
  ///
  constexpr auto at(size_type pos) const -> const_reference {
    return pos < size() ? m_data[pos + 1UZ] : throw std::out_of_range{"basic_inplace_string::at"};
  }

  /// \brief Get a reference to the character at the specified position.
  ///
  /// \param pos The position of the character to get.
  /// \return A reference to the character at the specified position.
  ///
  constexpr auto operator[](size_type pos) noexcept -> reference { return m_data[pos + 1UZ]; }

  /// \brief Get a const reference to the character at the specified position.
  ///
  /// \param pos The position of the character to get.
  /// \return A const reference to the character at the specified position.
  ///
  constexpr auto operator[](size_type pos) const noexcept -> const_reference { return m_data[pos + 1UZ]; }

  /// \brief Get a reference to the first character in the string.
  ///
  /// \return A reference to the first character in the string.
  ///
  constexpr auto front() noexcept -> reference { return m_data[1UZ]; }

  /// \brief Get a const reference to the first character in the string.
  ///
  /// \return A const reference to the first character in the string.
  ///
  constexpr auto front() const noexcept -> const_reference { return m_data[1UZ]; }

  /// \brief Get a reference to the last character in the string.
  ///
  /// \return A reference to the last character in the string.
  ///
  constexpr auto back() noexcept -> reference { return m_data[size()]; }

  /// \brief Get a const reference to the last character in the string.
  ///
  /// \return A const reference to the last character in the string.
  ///
  constexpr auto back() const noexcept -> const_reference { return m_data[size()]; }

  /// \brief Get a pointer to the underlying character array.
  ///
  /// \return A pointer to the underlying character array.
  ///
  constexpr auto data() noexcept -> pointer { return std::next(m_data.data()); }

  /// \brief Get a const pointer to the underlying character array.
  ///
  /// \return A const pointer to the underlying character array.
  ///
  constexpr auto data() const noexcept -> const_pointer { return std::next(m_data.data()); }

  /// \brief Get a const pointer to the underlying character array.
  ///
  /// \return A const pointer to the underlying character array.
  ///
  constexpr auto c_str() const noexcept -> const_pointer { return std::next(m_data.data()); }

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
  [[nodiscard]] constexpr auto empty() const noexcept -> bool { return m_data[1] == '\0'; }

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
  [[nodiscard]] constexpr auto max_size() const noexcept -> size_type { return N; }

  /// \brief Get the capacity of the string.
  ///
  /// \return The capacity of the string.
  ///
  [[nodiscard]] constexpr auto capacity() const noexcept -> size_type { return N; }

  //
  // Modifiers
  //

  /// \brief Clear the contents of the string.
  ///
  constexpr void clear() noexcept { m_data.fill('\0'); }

  /// \brief Insert count copies of character ch at the position index.
  ///
  /// \param index The position to insert the characters at.
  /// \param count The number of characters to insert.
  /// \param ch The character to insert.
  /// \throw std::length_error If the size of the string would exceed the capacity.
  ///
  constexpr void insert(size_type index, size_type count, CharT ch) {
    if (size() + count > N) {
      throw std::length_error{"basic_inplace_string::insert"};
    }

    std::copy_backward(begin() + index, end(), end() + count);
    std::fill_n(begin() + index, count, ch);
  }

  /// \brief Erase count characters from the position index.
  ///
  /// \param index The position to erase the characters from.
  /// \param count The number of characters to erase.
  /// \throw std::out_of_range If the index and count would exceed the size of the string.
  ///
  constexpr void erase(size_type index = 0UZ, size_type count = npos) {
    if (index + count > size()) {
      throw std::out_of_range{"basic_inplace_string::erase"};
    }

    std::copy(std::next(begin(), index + count), end(), std::next(begin(), index));
    m_data[1 + size() - count] = '\0';  // Add null terminator at the new end
  }

  /// \brief Append a character to the end of the string.
  ///
  /// \param ch The character to append.
  /// \throw std::length_error If the size of the string would exceed the capacity.
  ///
  constexpr void push_back(CharT ch) {
    if (size() + 1UZ > N) {
      throw std::length_error{"basic_inplace_string::push_back"};
    }

    m_data[size() + 1UZ] = ch;
    m_data[size() + 2UZ] = '\0';  // Add null terminator at the new end
  }

  /// \brief Remove the last character from the string.
  ///
  constexpr void pop_back() noexcept { m_data[size()] = '\0'; }

  /// \brief Append a string to the end of the string.
  ///
  /// \param str The string to append.
  /// \throw std::length_error If the size of the string would exceed the capacity.
  ///
  template <std::size_t N2>
  constexpr void append(const basic_inplace_string<N2, CharT>& str) {
    if (size() + str.size() > N) {
      throw std::length_error{"basic_inplace_string::append"};
    }

    std::copy(str.begin(), str.end(), end());
  }

  /// \brief Append a string to the end of the string.
  ///
  /// \param str The string to append.
  /// \throw std::length_error If the size of the string would exceed the capacity.
  ///
  template <std::size_t N2>
  constexpr auto operator+=(const basic_inplace_string<N2, CharT>& str) -> basic_inplace_string& {
    if (size() + str.size() > N) {
      throw std::length_error{"basic_inplace_string::operator+="};
    }

    std::copy(str.begin(), str.end(), end());
    return *this;
  }

  //
  // Non-member functions
  //

  /// \brief Concatenate two strings.
  ///
  /// \param lhs The string to concatenate.
  /// \param rhs The string to concatenate.
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
  /// \param lhs The string to compare.
  /// \param rhs The string to compare.
  /// \return True if the strings are equal, false otherwise.
  ///
  friend constexpr auto operator==(const basic_inplace_string& lhs, const basic_inplace_string& rhs) noexcept -> bool {
    return static_cast<std::string_view>(lhs) == static_cast<std::string_view>(rhs);
  }

  /// \brief Compare the string to a string view.
  ///
  /// \param lhs The string to compare.
  /// \param rhs The string view to compare.
  /// \return True if the strings are equal, false otherwise.
  ///
  friend constexpr auto operator==(const basic_inplace_string& lhs,
                                   std::basic_string_view<value_type> rhs) noexcept -> bool {
    return static_cast<std::string_view>(lhs) == rhs;
  }

  /// \brief Compare the string to a string view.
  ///
  /// \param lhs The string view to compare.
  /// \param rhs The string to compare.
  /// \return True if the strings are equal, false otherwise.
  ///
  friend constexpr auto operator==(std::basic_string_view<value_type> lhs,
                                   const basic_inplace_string& rhs) noexcept -> bool {
    return lhs == static_cast<std::string_view>(rhs);
  }

  /// \brief Compares the string to a character string.
  ///
  /// \param lhs The string to compare.
  /// \param rhs The character string to compare.
  ///
  friend constexpr auto operator==(const basic_inplace_string& lhs, const value_type* rhs) noexcept -> bool {
    return static_cast<std::string_view>(lhs) == std::string_view{rhs};
  }

  /// \brief Compares the string to a character string.
  ///
  /// \param lhs The character string to compare.
  /// \param rhs The string to compare.
  ///
  friend constexpr auto operator==(const value_type* lhs, const basic_inplace_string& rhs) noexcept -> bool {
    return std::string_view{lhs} == static_cast<std::string_view>(rhs);
  }

 private:
  std::array<value_type, N + 2> m_data{};
};

//
// Deduction guides
//

template <typename CharT, std::size_t N>
basic_inplace_string(const CharT (&)[N]) -> basic_inplace_string<N - 1UZ, CharT>;

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
