#ifndef STATICSTRING_H
#define STATICSTRING_H

#include <cassert>
#include <cstring>
#include <istream>
namespace gtr {
namespace containers {
template <int N = 32> struct static_string {
    using value_type = char;
    static constexpr std::size_t npos = (std::size_t)-1;
    static constexpr std::size_t nul_index = N - 1;

    value_type data[N];

    inline std::size_t size() { return std::strlen(data); }
    inline bool full() { return size() == nul_index; }

    // Default constructor
    inline static_string() { std::memset(data, '\0', N); }

    // Constructor from const char*
    inline constexpr static_string(const value_type *str) {
        std::size_t n = std::strlen(str);
        std::strncpy(data, str, N);
        data[n >= N ? nul_index : n] = '\0';
    }

    // Copy constructor
    inline static_string(const static_string &other) = default;

    // Move constructor
    inline static_string(static_string &&other) noexcept = default;

    template <int U> inline static_string(static_string<U> &&other) noexcept {
        std::memcpy(data, other.data, U > N ? N : U);
        data[nul_index] = '\0';
    }
    // Copy assignment operator
    inline static_string &operator=(const static_string &other) = default;

    template <int U> inline static_string &operator=(const static_string<U> &other) {
        if (this != (void *)(&other)) {
            std::strncat(data, other.data, N - size() - 1);
        }
        return *this;
    }

    // Move assignment operator
    inline static_string &operator=(static_string &&other) noexcept = default;

    template <int U> inline static_string &operator=(static_string<U> &&other) noexcept {
        if (this != (void *)(&other)) {
            std::strncat(data, other.data, N - size() - 1);
            data[N - 1] = '\0';
        }
        return *this;
    }

    // Operator+= to concatenate another string
    template <int U> inline static_string &operator+=(const static_string<U> &other) { return operator+=(other.data); }

    // Operator+= to concatenate another string
    inline static_string &operator+=(const static_string &other) { return operator+=(other.data); }

    // Operator+= to concatenate another string
    inline static_string &operator+=(const value_type *other) { return append(other); }

    inline static_string operator+(const value_type c) {
        static_string result = *this;
        result.append(c);
        return result;
    }

    inline static_string operator+(const value_type *other) {
        static_string result = *this;
        result.append(other);
        return result;
    }

    inline static_string operator+(const static_string &other) { return operator+(other); }

    // Friend declaration for << operator
    inline friend std::ostream &operator<<(std::ostream &os, const static_string &str) {
        os << str.data;
        return os;
    }

    // Friend declaration for >> operator
    inline friend std::istream &operator>>(std::istream &is, static_string &str) {
        is.getline(str.data, N + 1);
        return is;
    }

    template <int U> inline bool operator==(const static_string<U> &other) const { return std::strncmp(data, other.data, N) == 0; }
    // Append method to concatenate another string
    inline static_string &append(const value_type *other) {
        std::strncat(data, other, N - size() - 1);
        data[nul_index] = '\0';
        return *this;
    }

    inline static_string &append(const value_type *other, std::size_t count) {
        std::strncat(data, other, count);
        data[nul_index] = '\0';
        return *this;
    }

    inline static_string &append(const value_type other) {
        std::size_t s = size();
        if (s < N - 1)
            data[s] = other;
        data[s + 1] = '\0';
        return *this;
    }

    template <int U> inline static_string &append(const static_string<U> &other) { return append(other.data); }

    inline std::size_t find(const value_type *other) {
        size_t data_len = std::strlen(data);
        size_t other_len = std::strlen(other);

        for (size_t i = 0; i <= data_len - other_len; ++i) {
            if (std::strncmp(data + i, other, other_len) == 0) {
                return i;
            }
        }
        return npos;
    }

    inline value_type operator[](std::size_t index) const {
        assert(index < N);
        return data[index];
    }

    inline value_type &operator[](std::size_t index) {
        assert(index < N);
        return data[index];
    }

    inline operator const value_type *() const { return data; }

    // iterators
    inline value_type *begin() { return data; }
    
    inline value_type *end() { return data + size(); }
    
    inline const value_type *begin() const { return data; }
    
    inline const value_type *end() const { return data + size(); }

    // Destructor
    inline ~static_string() = default;
};
};     // namespace containers
};     // namespace gtr
#endif // STATICSTRING_H
