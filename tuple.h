#ifndef TUPLE_H
#define TUPLE_H
#include <cstdint>
namespace gtr {
namespace containers {

template <typename... Args>
struct tuple;

// Base case: empty tuple
template <>
struct tuple<> {
    tuple() = default;
};

// Recursive case
template <typename Head, typename... Tail>
struct tuple<Head, Tail...> : tuple<Tail...> {
    // Default constructor
    tuple() : tuple<Tail...>(), value() {}

    // Constructor with parameters
    tuple(const Head& head, const Tail&... tail)
        : value(head), tuple<Tail...>(tail...) {}

    Head value;
};

// Helper struct to get the type and value at index I
template <std::size_t I, typename Tuple>
struct tuple_element;

// Specialization for I == 0
template <typename Head, typename... Tail>
struct tuple_element<0, tuple<Head, Tail...>> {
    using type = Head;

    static type& get(tuple<Head, Tail...>& t) {
        return t.value;
    }

    static const type& get(const tuple<Head, Tail...>& t) {
        return t.value;
    }
};

// Recursive case for I > 0
template <std::size_t I, typename Head, typename... Tail>
struct tuple_element<I, tuple<Head, Tail...>> {
    using type = typename tuple_element<I - 1, tuple<Tail...>>::type;

    static type& get(tuple<Head, Tail...>& t) {
        return tuple_element<I - 1, tuple<Tail...>>::get(t);
    }

    static const type& get(const tuple<Head, Tail...>& t) {
        return tuple_element<I - 1, tuple<Tail...>>::get(t);
    }
};

// Helper function to get the value at index I
template <std::size_t I, typename... Args>
typename tuple_element<I, tuple<Args...>>::type& get(tuple<Args...>& t) {
    return tuple_element<I, tuple<Args...>>::get(t);
}

template <std::size_t I, typename... Args>
const typename tuple_element<I, tuple<Args...>>::type& get(const tuple<Args...>& t) {
    return tuple_element<I, tuple<Args...>>::get(t);
}
// Tuple size metafunction
template <typename Tuple>
struct tuple_size;

template <>
struct tuple_size<tuple<>> {
    static constexpr std::size_t value = 0;
};

template <typename Head, typename... Tail>
struct tuple_size<tuple<Head, Tail...>> {
    static constexpr std::size_t value = 1 + tuple_size<tuple<Tail...>>::value;
};

// index_sequence and make_index_sequence implementations
template <std::size_t... Is>
struct index_sequence {};

template <std::size_t N, std::size_t... Is>
struct make_index_sequence_impl : make_index_sequence_impl<N - 1, N - 1, Is...> {};

template <std::size_t... Is>
struct make_index_sequence_impl<0, Is...> {
    using type = index_sequence<Is...>;
};

template <std::size_t N>
using make_index_sequence = typename make_index_sequence_impl<N>::type;

// apply_impl function to unpack the tuple and call the function
template <typename F, typename Tuple, std::size_t... Is>
auto apply_impl(F&& f, Tuple&& t, index_sequence<Is...>)
-> decltype(std::forward<F>(f)(get<Is>(std::forward<Tuple>(t))...)) {
    return std::forward<F>(f)(get<Is>(std::forward<Tuple>(t))...);
}

// apply function that generates the index sequence and calls apply_impl
template <typename F, typename... Args>
auto apply(F&& f, tuple<Args...>& t)
-> decltype(apply_impl(std::forward<F>(f), t, make_index_sequence<sizeof...(Args)>{})) {
    return apply_impl(std::forward<F>(f), t, make_index_sequence<sizeof...(Args)>{});
}

template <typename F, typename... Args>
auto apply(F&& f, const tuple<Args...>& t)
-> decltype(apply_impl(std::forward<F>(f), t, make_index_sequence<sizeof...(Args)>{})) {
    return apply_impl(std::forward<F>(f), t, make_index_sequence<sizeof...(Args)>{});
}

} // namespace containers
} // namespace gtr

#endif // !TUPLE_H
