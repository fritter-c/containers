#ifndef STATICSTRING_H
#define STATICSTRING_H

#include <cassert>
#include <cstring>
#include <istream>
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

namespace gtr {
namespace containers {

/**
 * @brief A fixed-size string container with a maximum length of N.
 * @tparam N The maximum length of the string.
 */
template <int N = 32> struct static_string {
    using value_type = char;
    using size_type = std::size_t;
    static constexpr size_type npos = static_cast<size_type>(-1);
    static constexpr size_type nul_index = N - 1;
    static constexpr auto buffer_size = N;
    value_type data[N];

    /**
     * @brief Returns the size of the string.
     *
     * @return The size of the string.
     */
    inline size_type size() { return std::strlen(data); }

    /**
     * @brief Checks if the string is full.
     *
     * @return True if the string is full, false otherwise.
     */
    inline bool full() { return size() == nul_index; }

    /**
     * @brief Default constructor. Initializes the string with null characters.
     */
    inline static_string() { std::memset(data, '\0', N); }

    /**
     * @brief Constructor that initializes the string with a null-terminated character array.
     *
     * @param str The null-terminated character array to initialize the string with.
     */
    inline constexpr static_string(const value_type *str) {
        std::memset(data, '\0', N);
        const char *pholder = str;
        int index = 0;
        while (*pholder != '\0' && index < N - 1) {
            data[index++] = *pholder++;
        }
    }

    /**
     * @brief Copy constructor.
     *
     * @param other The static_string to copy from.
     */
    inline static_string(const static_string &other) = default;

    /**
     * @brief Move constructor.
     *
     * @param other The static_string to move from.
     */
    inline static_string(static_string &&other) noexcept = default;

    /**
     * @brief Move constructor for static_string with different maximum length.
     *
     * @tparam U The maximum length of the other static_string.
     * @param other The static_string to move from.
     */
    template <int U> inline static_string(static_string<U> &&other) noexcept {
        std::memcpy(data, other.data, U > N ? N : U);
        data[nul_index] = '\0';
    }

    /**
     * @brief Copy assignment operator.
     *
     * @param other The static_string to copy from.
     * @return A reference to the assigned static_string.
     */
    inline static_string &operator=(const static_string &other) = default;

    /**
     * @brief Copy assignment operator for static_string with different maximum length.
     *
     * @tparam U The maximum length of the other static_string.
     * @param other The static_string to copy from.
     * @return A reference to the assigned static_string.
     */
    template <int U> inline static_string &operator=(const static_string<U> &other) {
        if (this != (void *)(&other)) {
            std::strncat(data, other.data, N - size() - 1);
        }
        return *this;
    }

    /**
     * @brief Move assignment operator.
     *
     * @param other The static_string to move from.
     * @return A reference to the assigned static_string.
     */
    inline static_string &operator=(static_string &&other) noexcept = default;

    /**
     * @brief Move assignment operator for static_string with different maximum length.
     *
     * @tparam U The maximum length of the other static_string.
     * @param other The static_string to move from.
     * @return A reference to the assigned static_string.
     */
    template <int U> inline static_string &operator=(static_string<U> &&other) noexcept {
        if (this != (void *)(&other)) {
            std::strncat(data, other.data, N - size() - 1);
            data[N - 1] = '\0';
        }
        return *this;
    }

    /**
     * @brief Appends a static_string to the current string.
     *
     * @tparam U The maximum length of the other static_string.
     * @param other The static_string to append.
     * @return A reference to the modified static_string.
     */
    template <int U> inline static_string &operator+=(const static_string<U> &other) { return operator+=(other.data); }

    /**
     * @brief Appends a static_string to the current string.
     *
     * @param other The static_string to append.
     * @return A reference to the modified static_string.
     */
    inline static_string &operator+=(const static_string &other) { return operator+=(other.data); }

    /**
     * @brief Appends a null-terminated character array to the current string.
     *
     * @param other The null-terminated character array to append.
     * @return A reference to the modified static_string.
     */
    inline static_string &operator+=(const value_type *other) { return append(other); }

    /**
     * @brief Concatenates a character to the current string and returns the result as a new static_string.
     *
     * @param c The character to concatenate.
     * @return The concatenated static_string.
     */
    inline static_string operator+(const value_type c) {
        static_string result = *this;
        result.append(c);
        return result;
    }

    /**
     * @brief Concatenates a null-terminated character array to the current string and returns the result as a new static_string.
     *
     * @param other The null-terminated character array to concatenate.
     * @return The concatenated static_string.
     */
    inline static_string operator+(const value_type *other) {
        static_string result = *this;
        result.append(other);
        return result;
    }

    /**
     * @brief Concatenates a static_string to the current string and returns the result as a new static_string.
     *
     * @param other The static_string to concatenate.
     * @return The concatenated static_string.
     */
    inline static_string operator+(const static_string &other) { return operator+(other); }

    /**
     * @brief Overload of the output stream operator for static_string.
     *
     * @param os The output stream.
     * @param str The static_string to output.
     * @return The modified output stream.
     */
    inline friend std::ostream &operator<<(std::ostream &os, const static_string &str) {
        os << str.data;
        return os;
    }

    /**
     * @brief Overload of the input stream operator for static_string.
     *
     * @param is The input stream.
     * @param str The static_string to input.
     * @return The modified input stream.
     */
    inline friend std::istream &operator>>(std::istream &is, static_string &str) {
        is.getline(str.data, N + 1);
        return is;
    }

    /**
     * @brief Equality comparison operator for static_string with different maximum length.
     *
     * @tparam U The maximum length of the other static_string.
     * @param other The static_string to compare with.
     * @return True if the strings are equal, false otherwise.
     */
    template <int U> inline bool operator==(const static_string<U> &other) const { return std::strncmp(data, other.data, N) == 0; }

    /**
     * @brief Appends a null-terminated character array to the current string.
     *
     * @param other The null-terminated character array to append.
     * @return A reference to the modified static_string.
     */
    inline static_string &append(const value_type *other) {
        std::strncat(data, other, N - size() - 1);
        data[nul_index] = '\0';
        return *this;
    }

    /**
     * @brief Appends a null-terminated character array to the current string.
     *
     * @param other The null-terminated character array to append.
     * @param count The number of characters to append.
     * @return A reference to the modified static_string.
     */
    inline static_string &append(const value_type *other, std::size_t count) {
        std::strncat(data, other, count);
        data[nul_index] = '\0';
        return *this;
    }

    /**
     * @brief Appends a character to the current string.
     *
     * @param other The character to append.
     * @return A reference to the modified static_string.
     */
    inline static_string &append(const value_type other) {
        size_type s = size();
        if (s < N - 1)
            data[s] = other;
        data[s + 1] = '\0';
        return *this;
    }

    /**
     * @brief Appends a static_string to the current string.
     *
     * @tparam U The maximum length of the other static_string.
     * @param other The static_string to append.
     * @return A reference to the modified static_string.
     */
    template <int U> inline static_string &append(const static_string<U> &other) { return append(other.data); }

    /**
     * @brief Finds the first occurrence of a null-terminated character array in the current string.
     *
     * @param other The null-terminated character array to search for.
     * @return The index of the first occurrence, or npos if not found.
     */
    inline size_type find(const value_type *other) {
        size_type data_len = std::strlen(data);
        size_type other_len = std::strlen(other);

        for (size_type i = 0; i <= data_len - other_len; ++i) {
            if (std::strncmp(data + i, other, other_len) == 0) {
                return i;
            }
        }
        return npos;
    }

    /**
     * @brief Returns a substring of the current string.
     *
     * @param start The starting index of the substring.
     * @param count The length of the substring.
     * @return The substring.
     */
    inline static_string substr(size_type start, size_type count) {
        static_string result;
        std::strncpy(result.data, data + start, count);
        result.data[count] = '\0';
        return result;
    }

    /**
     * @brief Finds the last occurrence of a character in the current string.
     *
     * @param c The character to search for.
     * @return The index of the last occurrence, or npos if not found.
     */
    size_type find_last_of(const value_type c) {
        for (size_type i = size(); i > 0; --i) {
            if (data[i] == c) {
                return i;
            }
        }
        return npos;
    }

    /**
     * @brief Finds the first occurrence of a character in the current string.
     *
     * @param c The character to search for.
     * @return The index of the first occurrence, or npos if not found.
     */
    size_type find_first_of(const value_type c) {
        for (size_type i = 0; i < size(); ++i) {
            if (data[i] == c) {
                return i;
            }
        }
        return npos;
    }

    /**
     * @brief Erases a portion of the string.
     *
     * @param start The starting index of the portion to erase.
     * @param count The number of characters to erase.
     */
    inline void erase(size_type start, size_type count) {
        size_type len = size();
        if (start >= len) {
            return;
        }
        if (start + count > len) {
            count = len - start;
        }
        std::memmove(data + start, data + start + count, len - count - start);
        data[len - count] = '\0';
    }

    /**
     * @brief Returns the character at the specified index.
     *
     * @param index The index of the character.
     * @return The character at the specified index.
     */
    inline value_type operator[](size_type index) const {
        assert(index < N);
        return data[index];
    }

    /**
     * @brief Returns a reference to the character at the specified index.
     *
     * @param index The index of the character.
     * @return A reference to the character at the specified index.
     */
    inline value_type &operator[](size_type index) {
        assert(index < N);
        return data[index];
    }

    /**
     * @brief Conversion operator to const char*.
     *
     * @return A pointer to the underlying character array.
     */
    inline operator const value_type *() const { return data; }

    /**
     * @brief Returns an iterator to the beginning of the string.
     *
     * @return An iterator to the beginning of the string.
     */
    inline value_type *begin() { return data; }

    /**
     * @brief Returns an iterator to the end of the string.
     *
     * @return An iterator to the end of the string.
     */
    inline value_type *end() { return data + size(); }

    /**
     * @brief Returns a const iterator to the beginning of the string.
     *
     * @return A const iterator to the beginning of the string.
     */
    inline const value_type *begin() const { return data; }

    /**
     * @brief Returns a const iterator to the end of the string.
     *
     * @return A const iterator to the end of the string.
     */
    inline const value_type *end() const { return data + size(); }

    /**
     * @brief Destructor.
     */
    inline ~static_string() = default;
};

} // namespace containers
} // namespace gtr

#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif
#endif // STATICSTRING_H
