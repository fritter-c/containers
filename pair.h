#ifndef PAIR_H
#define PAIR_H
#include <type_traits>
#include <utility>

namespace gtr {
namespace containers {
template <typename First, typename Second> struct pair {
    First first;
    Second second;
    pair() : first(), second() {}
    pair(const First &first, const Second &second) : first(first), second(second) {}
    pair(First &&first, Second &&second) : first(std::move(first)), second(std::move(second)) {}
    pair(const pair &other) : first(other.first), second(other.second) {}
    pair(pair &&other) noexcept : first(std::move(other.first)), second(std::move(other.second)) {}
    pair &operator=(const pair &other) {
        first = other.first;
        second = other.second;
        return *this;
    }
    pair &operator=(pair &&other) noexcept {
        if (this != &other) {
            first = std::move(other.first);
            second = std::move(other.second);
        }
        return *this;
    }
    bool operator==(const pair &other) const { return first == other.first && second == other.second; }
    bool operator!=(const pair &other) const { return first != other.first || second != other.second; }
    bool operator<(const pair &other) const { return first < other.first || (!(other.first < first) && second < other.second); }
    bool operator>(const pair &other) const { return other < *this; }
    bool operator<=(const pair &other) const { return !(other < *this); }
    bool operator>=(const pair &other) const { return !(*this < other); }
}; // namespace gtr

template <typename First, typename Second> pair<First, Second> make_pair(const First &first, const Second &second) {
    return pair<First, Second>(first, second);
}

template <typename T>
concept Empty = std::is_empty_v<T>;
template <typename T>
concept NotFinal = !std::is_final_v<T>;
template <Empty First, typename Second>
    requires NotFinal<First>
struct compressed_pair : public First { // Pair with empty base optimization (First has no size)
    Second second;
    compressed_pair() : First(), second() {}
    compressed_pair(const First &first, const Second &second) : First(first), second(second) {}
    compressed_pair(First &&first, Second &&second) : First(std::move(first)), second(std::move(second)) {}
    compressed_pair(const compressed_pair &other) : First(other), second(other.second) {}
    compressed_pair(compressed_pair &&other) noexcept : First(std::move(other)), second(std::move(other.second)) {}

    compressed_pair &operator=(const compressed_pair &other) {
        First::operator=(other);
        second = other.second;
        return *this;
    }

    compressed_pair &operator=(compressed_pair &&other) noexcept {
        if (this != &other) {
            First::operator=(std::move(other));
            second = std::move(other.second);
        }
        return *this;
    }

    bool operator==(const compressed_pair &other) const { return First::operator==(other) && second == other.second; }
    bool operator!=(const compressed_pair &other) const { return First::operator!=(other) || second != other.second; }
    bool operator<(const compressed_pair &other) const { return First::operator<(other) || (!(other < *this) && second < other.second); }
    bool operator>(const compressed_pair &other) const { return other < *this; }
    bool operator<=(const compressed_pair &other) const { return !(other < *this); }
    bool operator>=(const compressed_pair &other) const { return !(*this < other); }
};

template <typename First, typename Second>
    requires Empty<First> && NotFinal<First>
compressed_pair<First, Second> make_compressed_pair(const First &first, const Second &second) {
    return compressed_pair<First, Second>(first, second);
}

template <typename First, typename Second>
    requires Empty<First> && NotFinal<First>
compressed_pair<First, Second> make_compressed_pair(First &&first, Second &&second) {
    return compressed_pair<First, Second>(std::move(first), std::move(second));
}
} // namespace containers
} // namespace gtr
#endif // !PAIR_H
