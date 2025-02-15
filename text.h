#ifndef TEXT_H
#define TEXT_H
#include <cstdarg>
#include <fstream>
#include <istream>
#include <ostream>
#include "allocator_base.h"
#include "allocators/allocators.h"
#include "vector.h"

#ifdef TEXT_PARANOID
#define PARANOID_ASSERT(x) assert(x && "Data is not on the heap!");
#else
#define PARANOID_ASSERT(x) 
#endif

namespace gtr {
namespace containers {

/**
 * @brief A text container with a fixed-size local buffer and dynamic allocation support.
 *
 * This struct provides a text container that can store text data in a fixed-size local buffer.
 * If the text data exceeds the capacity of the local buffer, it dynamically allocates memory
 * on the heap. The text container supports various operations such as formatting, appending,
 * inserting, and comparing text data. It also provides methods for converting text to different
 * types, splitting text, and checking if the text is a number.
 *
 * @tparam N The size of the local buffer.
 * @tparam Allocator The allocator type used for dynamic memory allocation.
 */
template <int N = 64, class Allocator = c_allocator<char>> struct text : private _allocator_ebo<char, Allocator> {
<<<<<<< HEAD
    static_assert(N > 3 * sizeof(std::size_t), "Text N must be bugger 3 * sizeof(std::size_t)");
  private:
    alignas(char *) char data[N];
  public:
    template <int, class> friend struct text;
=======
    static_assert(N >= 2 * sizeof(std::size_t), "Text N must be at least 2 * sizeof(std::size_t)");

    char *data;           /**< Pointer to the underlying data array. */
    char local_buffer[N]{}; /**< Local buffer for small strings. */

>>>>>>> 135bbbf9bff8e10850f7d009ee512de4c621074d
    using value_type = char;
    using size_type = std::size_t;
    static constexpr size_type npos = static_cast<size_type>(-1);

    Allocator &allocator() { return _allocator_ebo<char, Allocator>::get_allocator(); }
    const Allocator &allocator() const { return _allocator_ebo<char, Allocator>::get_allocator(); }


    /**
     * @brief Retrieves a reference to the pointer to the dynamically allocated data.
     *
     * This function returns a reference to the pointer that points to the dynamically
     * allocated data on the heap. It asserts that the data is not stored in the local
     * buffer when TEXT_PARANOID is defined.
     *
     * @return char*& A reference to the pointer to the dynamically allocated data.
     */
    char *&get_pointer() {
        PARANOID_ASSERT(!local_data());
        return *reinterpret_cast<char **>(data);
    }

    /**
     * @brief Retrieves a constant reference to the pointer to the dynamically allocated data.
     *
     * This function returns a constant reference to the pointer that points to the dynamically
     * allocated data on the heap. It asserts that the data is not stored in the local
     * buffer when TEXT_PARANOID is defined.
     *
     * @return const char *const& A constant reference to the pointer to the dynamically allocated data.
     */
    inline const char *const &get_pointer() const {
        PARANOID_ASSERT(!local_data());
        return *reinterpret_cast<const char *const *>(data);
    }

    /**
     * @brief Checks if the data is stored in the local buffer.
     *
     * This function compares the current data pointer with the local buffer
     * pointer to determine if the data is stored locally.
     *
     * @return true if the data is stored in the local buffer, false otherwise.
     */
    inline bool local_data() const { return !data[N - 1]; }

    /**
     * @brief Marks the data as stored in the heap
     *
     * This function sets the last byte of the local buffer to 0 to indicate
     * that the data is stored in the local buffer.
     */
    inline void set_heap() { data[N - 1] = 1; }

    /**
     * @brief Constructs an inline text object.
     *
     * Initializes the text object with a local buffer, sets the size to 0,
     * capacity to N-1, and marks the data as local. The first character of
     * the buffer is set to the null terminator.
     */
    inline constexpr text() : _allocator_ebo<char, Allocator>() { std::memset(data, 0, N); }

    /**
     * @brief Constructs an inline text object.
     *
     * Initializes the text object with a local buffer, sets the size to 0,
     * capacity to N-1, and marks the data as local. The first character of
     * the buffer is set to the null terminator.
     */
    inline constexpr text() : _allocator_ebo<char, Allocator>(), data(local_buffer) { }

    /**
     * @brief Returns the size of the text container.
     *
     * This function determines the size of the text container by checking if the
     * data is stored in the local buffer or not. If the data is in the local buffer,
     * it calculates the size using `std::strlen`. Otherwise, it retrieves the size
     * from the first element of the local buffer, interpreted as a `size_type`.
     *
     * @return The size of the text container.
     */
    inline std::size_t size() const {
        if (local_data()) {
            return std::strlen(data);
        }
        return reinterpret_cast<const size_type *>(data)[1];
    }

    /**
     * @brief Returns a pointer to the C-style string.
     *
     * This function checks if the data is stored in the local buffer. If so, it returns
     * the local buffer pointer. Otherwise, it retrieves the pointer from the local buffer.
     *
     * @return char* A pointer to the C-style string.
     */
    value_type *c_str() {
        if (local_data()) {
            return data;
        }
        return get_pointer();
    }

    /**
     * @brief Returns a constant pointer to the C-style string.
     *
     * This function checks if the data is stored in the local buffer. If so, it returns
     * the local buffer pointer. Otherwise, it retrieves the pointer from the local buffer.
     *
     * @return const char* A constant pointer to the C-style string.
     */
    const value_type *c_str() const {
        if (local_data()) {
            return data;
        }
        return get_pointer();
    }

    /**
     * @brief Returns the capacity of the container.
     *
     * This function checks if the data is stored in the local buffer. If so, it returns
     * the fixed capacity (N - 1). Otherwise, it retrieves the capacity from the local buffer.
     *
     * @return std::size_t The capacity of the container.
     */
    inline std::size_t capacity() const {
        if (local_data()) {
            return N - 1;
        }
        return reinterpret_cast<const size_type *>(data)[2];
    }

    /**
     * @brief Sets the size of the container to the specified new size.
     *
     * This function updates the size of the container. If the data is not
     * stored in the local buffer, it sets the size in the local buffer.
     *
     * @param new_size The new size to set for the container.
     */
    inline void set_size(size_type new_size) {
        if (!local_data()) {
            reinterpret_cast<size_type *>(data)[1] = new_size;
        }
    }

    /**
     * @brief Sets the capacity of the container.
     *
     * This function updates the capacity of the container to the specified
     * new capacity. If the data is not stored in the local buffer, it
     * modifies the capacity stored in the local buffer.
     *
     * @param new_capacity The new capacity to set for the container.
     */
    inline void set_capacity(size_type new_capacity) {
        if (!local_data()) {
            reinterpret_cast<size_type *>(data)[2] = new_capacity;
        }
    }

<<<<<<< HEAD
    /**
     * @brief Reserves memory for the container.
     *
     * This function ensures that the container has at least the specified capacity.
     * If the requested capacity is greater than the current capacity, it allocates
     * new memory and copies the existing data to the new memory location.
     *
     * @param _Reserve The desired capacity to reserve.
     */
    inline void reserve(const std::size_t _Reserve) {
        if (_Reserve > capacity()) {
            if (local_data()) {
                size_type old_size = size();
                char temp[N];
                std::memcpy(temp, data, N);
                set_heap();
                get_pointer() = allocator().malloc(_Reserve + 1);
                std::memcpy(get_pointer(), temp, old_size + 1);
                set_size(old_size);
            } else {
                get_pointer() = allocator().realloc(get_pointer(), _Reserve + 1, capacity() + 1);
            }
            set_capacity(_Reserve);
        }
    }

    inline void resize(size_type new_size) {
        if (new_size > size()) {
            reserve(new_size);
            char *data_ptr = local_data() ? data : get_pointer();
            std::memset(data_ptr + size(), 0, new_size - size() + 1);
        }
        set_size(new_size);
    }
=======
>>>>>>> 135bbbf9bff8e10850f7d009ee512de4c621074d

    /**
     * @brief Formats a string using a printf-style format string and variable arguments.
     *
     * This function takes a format string and a variable number of arguments,
     * formats the string accordingly, and stores the result in the text object.
     *
     * @param format The printf-style format string.
     * @param ... The variable arguments to format the string.
     * @return text& A reference to the formatted text object.
     */
    inline text format(const char *format, ...) {
        va_list args;
        va_start(args, format);
        int len = std::vsnprintf(nullptr, 0, format, args);
        va_end(args);
        reserve(len);
        va_start(args, format);
        if (local_data()) {
            std::vsnprintf(data, len + 1, format, args);
        } else {
            std::vsnprintf(get_pointer(), len + 1, format, args);
        }
        va_end(args);
        set_size(len);
        return *this;
    }

    /**
     * @brief Constructs a text object from a C-style string.
     *
     * This constructor initializes the text object using the provided C-style string.
     * It sets the internal buffer to the local buffer, calculates the length of the
     * input string, reserves the necessary space, copies the string into the buffer,
     * and updates the size of the text object.
     *
     * @param str The C-style string to initialize the text object with.
     */
    inline constexpr text(const char *str) : _allocator_ebo<char, Allocator>() {
        std::memset(data, 0, N);
        if (str) {
            std::size_t len = std::strlen(str);
            reserve(len);
            if (local_data()) {
                std::memcpy(data, str, len + 1);
            } else {
                std::memcpy(get_pointer(), str, len + 1);
            }
            set_size(len);
        }
    }

    /**
     * @brief Destructor for the text class.
     *
     * This inline destructor releases the allocated memory for the text object.
     * If the text object does not use local data, it frees the allocated memory
     * using the allocator's free method.
     */
    inline ~text() {
        if (!local_data()) {
            allocator().free(get_pointer(), capacity() + 1);
        }
        std::memset(data, 0, N);
    }

    /**
     * @brief Constructs a text object from another text object.
     *
     * This constructor initializes the text object using another text object.
     * It sets the internal buffer to the local buffer, reserves the necessary
     * space, copies the string into the buffer, and updates the size of the text object.
     *
     * @param str The text object to initialize the text object with.
     */
    explicit constexpr inline text(const text &str) : _allocator_ebo<char, Allocator>(str.allocator()) {
        std::memset(data, 0, N);
        reserve(str.size());
        char *data_ptr = local_data() ? data : get_pointer();
        const char *src_ptr = str.local_data() ? str.data : str.get_pointer();
        std::memcpy(data_ptr, src_ptr, str.size() + 1);
        set_size(str.size());
    }

    /**
     * @brief Constructs a text object by moving another text object.
     *
     * This constructor initializes the text object by moving another text object.
     * It sets the internal buffer to the local buffer, copies the string into the buffer,
     * and updates the size of the text object. It also updates the capacity and local_data
     * flags based on the input text object.
     *
     * @param str The text object to move from.
     */
    inline text(text &&str) noexcept {
        std::memcpy(data, str.data, N);
        std::memset(str.data, 0, N);
    }

    /**
     * @brief Assigns a C-style string to the text object.
     *
     * This method assigns a C-style string to the text object. It calculates the length
     * of the input string, reserves the necessary space, copies the string into the buffer,
     * and updates the size of the text object.
     *
     * @param str The C-style string to assign to the text object.
     * @return text& A reference to the modified text object.
     */
    inline text &operator=(const text &str) {
        if (!local_data()) {
            allocator().free(get_pointer(), capacity() + 1);
        }
        std::memset(data, 0, N); // unsets the heap flag
        reserve(str.size());
        char *data_ptr = local_data() ? data : get_pointer();
        const char *src_ptr = str.local_data() ? str.data : str.get_pointer();
        std::memcpy(data_ptr, src_ptr, str.size() + 1);
        return *this;
    }

    /**
     * @brief Converts a given value to a text representation with optional decimal places.
     *
     * This function template converts a value of type T to a text object. It supports
     * integral and floating-point types. For integral types, the value is formatted as
     * a long or unsigned long. For floating-point types, the value is formatted with
     * a specified number of decimal places.
     *
     * @tparam T The type of the value to be converted. Supported types include integral
     *           and floating-point types.
     * @param value The value to be converted to text.
     * @param decimal_places The number of decimal places to include in the text representation
     *                       for floating-point values. Default is 5.
     * @return text The text representation of the given value.
     */
    template <typename T> static text to_string(T value, int decimal_places = 5) {
        text result;
        if constexpr (std::is_integral<T>::value) {
            if constexpr (std::is_signed<T>::value) {
                result.format("%ld", static_cast<long>(value));
            } else {
                result.format("%lu", static_cast<unsigned long>(value));
            }
        } else if constexpr (std::is_floating_point<T>::value) {
            if constexpr (std::is_same<T, float>::value) {
                result.format("%.*f", decimal_places, value);
            } else if constexpr (std::is_same<T, double>::value) {
                result.format("%.*lf", decimal_places, value);
            } else {
                result.format("%.*Lf", decimal_places, value);
            }
        }
        return result;
    }

    /**
     * @brief Assigns a C-style string to the text object.
     *
     * This method assigns a C-style string to the text object. It calculates the length
     * of the input string, reserves the necessary space, copies the string into the buffer,
     * and updates the size of the text object.
     *
     * @param str The C-style string to assign to the text object.
     * @return text& A reference to the modified text object.
     */
    inline text &operator=(text &&str) noexcept {
        if (this != &str) {
            if (!local_data()) {
                allocator().free(get_pointer(), capacity() + 1);
            }
            // This is a move if the data is on the heap for src or a copy if it's local
            std::memcpy(data, str.data, N);
            std::memset(str.data, 0, N);
        }
        return *this;
    }

    /**
     * @brief Assignment operator for the text class template.
     *
     * This operator assigns the contents of another text object to the current
     * text object. It handles memory allocation and deallocation as needed.
     *
     * @tparam U The size of the text object being assigned.
     * @param str The text object to be assigned to the current object.
     * @return A reference to the current text object after assignment.
     */
    template <int U> inline text &operator=(const text<U> &str) {
        if (this != &str) {
            if (!local_data()) {
                allocator().free(get_pointer(), capacity() + 1);
            }
            std::memset(data, 0, N); // unsets the heap flag
            reserve(str.size());
            char *data_ptr = local_data() ? data : get_pointer();
            char *src_ptr = str.local_data() ? str.data : str.get_pointer();
            std::memcpy(data_ptr, src_ptr, str.size() + 1);
            set_size(str.size());
        }
        return *this;
    }

    /**
     * @brief Returns the size of the text object.
     *
     * This method returns the size of the text object, which is the number of characters
     * in the string without the null terminator.
     *
     * @return std::size_t The size of the text object.
     */
    inline bool empty() const { return size() == 0; }

    /**
     * @brief Clears the text object.
     *
     * This method clears the text object by setting the size to 0 and the first character
     * of the buffer to the null terminator.
     */
    inline void clear() {
        set_size(0);
        char *data_ptr = local_data() ? data : get_pointer();
        data_ptr[0] = '\0';
    }

    /**
     * @brief Reserves memory for the text object.
     *
     * This method reserves memory for the text object by reallocating the buffer with the
     * specified capacity. If the new capacity is greater than the current capacity, it allocates
     * new memory and copies the existing string into the new buffer. If the new capacity is less
     * than the current capacity, it does nothing.
     *
     * @param _Reserve The new capacity to reserve.
     */
    inline size_type append(const char *str) {
        std::size_t len = std::strlen(str);
        std::size_t old_size = size();
        resize(size() + len);
        char *data_ptr = local_data() ? data : get_pointer();
        std::memcpy(data_ptr + old_size, str, len + 1);
        return size();
    }

    /**
     * @brief Appends a character to the end of the text container.
     *
     * This function reserves additional space if necessary, appends the given
     * character to the end of the container, updates the size, and ensures the
     * text is null-terminated.
     *
     * @param c The character to append.
     * @return The new size of the text container.
     */
    inline size_type append(char c) {
        reserve(size() + 1);
        size_type old_size = size();
        char *data_ptr = local_data() ? data : get_pointer();
        data_ptr[old_size] = c;
        set_size(old_size + 1);
        data_ptr[old_size + 1] = '\0';
        return size();
    }

    /**
     * @brief Appends another text container to the end of this text container.
     *
     * This function reserves additional space if necessary, appends the given
     * text container to the end of this container, updates the size, and ensures
     * the text is null-terminated.
     *
     * @param str The text container to append.
     * @return The new size of the text container.
     */
    inline size_type append(text &str) {
        reserve(size() + str.size());
        char *data_ptr = local_data() ? data : get_pointer();
        char *src_ptr = str.local_data() ? str.data : str.get_pointer();
        std::memcpy(data_ptr + size(), src_ptr, str.size() + 1);
        set_size(size() + str.size());
        data_ptr[size()] = '\0';
        return size();
    }

    /**
     * @brief Appends another text container to the end of this text container.
     *
     * This function reserves additional space if necessary, appends the given
     * text container to the end of this container, updates the size, and ensures
     * the text is null-terminated.
     *
     * @param str The text container to append.
     * @return The new size of the text container.
     */
    template <int U> inline size_type append(const text<U> &str) {
        reserve(size() + str.size());
        char *data_ptr = local_data() ? data : get_pointer();
        const char *src_ptr = str.local_data() ? str.data : str.get_pointer();
        std::memcpy(data_ptr + size(), src_ptr, str.size() + 1);
        set_size(size() + str.size());
        data_ptr[size()] = '\0';
        return size();
    }

    /**
     * @brief Returns the character at the specified index.
     *
     * @param i The index of the character.
     * @return The character at the specified index.
     */
    inline char operator[](int i) const { return local_data() ? data[i] : get_pointer()[i]; }

    /**
     * @brief Returns a reference to the character at the specified index.
     *
     * @param i The index of the character.
     * @return A reference to the character at the specified index.
     */
    inline char &operator[](int i) { return local_data() ? data[i] : get_pointer()[i]; }

    /**
     * @brief Extracts a substring from the text.
     *
     * This function creates a new text object containing the substring
     * from the specified start index to the end index.
     *
     * @param start The starting index of the substring (inclusive).
     * @param end The ending index of the substring (exclusive).
     * @return A new text object containing the specified substring.
     */
    inline text substr(int start, int end) const {
        text result;
        result.reserve(end - start);
        const char *data_ptr = local_data() ? data : get_pointer();
        char *result_ptr = result.local_data() ? result.data : result.get_pointer();
        std::memcpy(result_ptr, data_ptr + start, end - start);
        result.set_size(end - start);
        result_ptr[end - start] = '\0';
        return result;
    }

    /**
     * @brief Extracts a substring from the text.
     *
     * This function creates a new text object containing the substring
     * from the specified start index to the end index.
     *
     * @param start The starting index of the substring (inclusive).
     * @return A new text object containing the specified substring.
     */
    inline text substr(int start) const {
        text result;
        result.reserve(size() - start);
        const char *data_ptr = local_data() ? data : get_pointer();
        char *result_ptr = result.local_data() ? result.data : result.get_pointer();
        std::memcpy(result_ptr, data_ptr + start, size() - start);
        result.set_size(size() - start);
        result_ptr[size() - start] = '\0';
        return result;
    }

    /**
     * @brief Finds the first occurrence of a substring in the text.
     *
     * This function searches for the first occurrence of the specified
     * substring in the text and returns the index of the first character
     * of the substring if found, or -1 if not found.
     *
     * @param str The substring to search for.
     * @return The index of the first occurrence of the substring, or -1 if not found.
     */
    inline size_type find(const char *str) const {
        std::size_t len = std::strlen(str);
        if (len <= size()) {
            const char *data_ptr = local_data() ? data : get_pointer();
            for (std::size_t i = 0; i <= size() - len; ++i) {
                if (std::strncmp(data_ptr + i, str, len) == 0) {
                    return i;
                }
            }
        }
        return npos;
    }

    /**
     * @brief Inserts a C-style string into the text at the specified index.
     *
     * This function inserts the specified C-style string into the text at the
     * specified index. It reserves additional space if necessary, moves the existing
     * characters to make room for the new string, copies the new string into the buffer,
     * updates the size, and ensures the text is null-terminated.
     *
     * @param index The index at which to insert the string.
     * @param str The C-style string to insert.
     */
    inline void insert(int index, const char *str) {
        std::size_t len = std::strlen(str);
        reserve(size() + len);
        char *data_ptr = local_data() ? data : get_pointer();
        std::memmove(data_ptr + index + len, data_ptr + index, size() - index);
        std::memcpy(data_ptr + index, str, len);
        set_size(size() + len);
        data_ptr[size()] = '\0';
    }

    /**
     * @brief Inserts a character into the text at the specified index.
     *
     * This function inserts the specified character into the text at the
     * specified index. It reserves additional space if necessary, moves the existing
     * characters to make room for the new character, inserts the new character into the buffer,
     * updates the size, and ensures the text is null-terminated.
     *
     * @param index The index at which to insert the character.
     * @param c The character to insert.
     */
    inline void insert(int index, char c) {
        reserve(size() + 1);
        char *data_ptr = local_data() ? data : get_pointer();
        std::memmove(data_ptr + index + 1, data_ptr + index, size() - index);
        data_ptr[index] = c;
        set_size(size() + 1);
        data_ptr[size()] = '\0';
    }

    /**
     * @brief Inserts a text object into the text at the specified index.
     *
     * This function inserts the specified text object into the text at the
     * specified index. It reserves additional space if necessary, moves the existing
     * characters to make room for the new text object, copies the new text object into the buffer,
     * updates the size, and ensures the text is null-terminated.
     *
     * @param index The index at which to insert the text object.
     * @param str The text object to insert.
     */
    inline void insert(int index, const text &str) {
        reserve(size() + str.size());
        char *data_ptr = local_data() ? data : get_pointer();
        char *src_ptr = str.local_data() ? str.data : str.get_pointer();
        std::memmove(data_ptr + index + str.size(), data_ptr + index, size() - index);
        std::memcpy(data_ptr + index, src_ptr, str.size());
        set_size(size() + str.size());
        data_ptr[size()] = '\0';
    }

    /**
     * @brief Inserts a text object into the text at the specified index.
     *
     * This function inserts the specified text object into the text at the
     * specified index. It reserves additional space if necessary, moves the existing
     * characters to make room for the new text object, copies the new text object into the buffer,
     * updates the size, and ensures the text is null-terminated.
     *
     * @param index The index at which to insert the text object.
     * @param str The text object to insert.
     */
    template <int U> inline void insert(int index, const text<U> &str) {
        reserve(size() + str.size());
        char *data_ptr = local_data() ? data : get_pointer();
        char *src_ptr = str.local_data() ? str.data : str.get_pointer();
        std::memmove(data_ptr + index + str.size(), data_ptr + index, size() - index);
        std::memcpy(data_ptr + index, src_ptr, str.size());
        set_size(size() + str.size());
        data_ptr[size()] = '\0';
    }

    /**
     * @brief Finds the last occurrence of a character in the text.
     *
     * This function searches for the last occurrence of the specified character
     * in the text and returns the index of the last occurrence if found, or -1 if not found.
     *
     * @param c The character to search for.
     * @return The index of the last occurrence of the character, or -1 if not found.
     */
    inline size_type find_first_of(char c) {
        char *data_ptr = local_data() ? data : get_pointer();
        for (size_type i = 0; i <= size(); ++i) {
            if (data_ptr[i] == c) {
                return i;
            }
        }
        return npos;
    }

    /**
     * @brief Finds the last occurrence of a character in the text.
     *
     * This function searches for the last occurrence of the specified character
     * in the text and returns the index of the last occurrence if found, or -1 if not found.
     *
     * @param c The character to search for.
     * @return The index of the last occurrence of the character, or -1 if not found.
     */
    inline size_type find_last_of(char c) {
        char *data_ptr = local_data() ? data : get_pointer();
        for (std::size_t i = size() + 1; i > 0; --i) {
            if (data_ptr[i - 1] == c) {
                return i - 1;
            }
        }
        return npos;
    }

    /**
     * @brief Erases a portion of the text.
     *
     * This function erases a portion of the text starting at the specified index
     * and removing the specified number of characters. If the start index is greater
     * than or equal to the size of the text, or the start index plus the count is greater
     * than the size of the text, the function does nothing.
     *
     * @param start The starting index of the portion to erase.
     * @param count The number of characters to erase.
     */
    inline void erase(int start, int count) {
        if (start >= size()) {
            return;
        }
        if (start + count > size()) {
            count = size() - start;
        }
        char *data_ptr = local_data() ? data : get_pointer();
        std::memmove(data_ptr + start, data_ptr + start + count, size() - count - start);
        data_ptr[size() - count] = '\0';
        set_size(size() - count);
    }

    /**
     * @brief Concatenates a C-style string to the current text object.
     *
     * This operator allows for the addition of a C-style string to the current
     * text object, resulting in a new text object that contains the original
     * text followed by the provided string.
     *
     * @param str The C-style string to be concatenated to the current text object.
     * @return A new text object containing the concatenated result.
     */
    inline text operator+(const char *str) {
        text result = *this;
        result.append(str);
        return result;
    }

    /**
     * @brief Overloads the + operator to append a character to the text object.
     *
     * This function creates a new text object by appending the given character
     * to the current text object. The original text object remains unchanged.
     *
     * @param c The character to be appended to the text object.
     * @return A new text object with the character appended.
     */
    inline text operator+(char c) {
        text result = *this;
        result.append(c);
        return result;
    }

    /**
     * @brief Concatenates the given text object with the current text object.
     *
     * This operator overloads the '+' operator to concatenate the current text
     * object with the provided text object. The result is a new text object
     * containing the combined contents of both text objects.
     *
     * @param str The text object to be concatenated with the current text object.
     * @return text A new text object containing the concatenated result.
     */
    inline text operator+(text &str) {
        text result = *this;
        result.append(str);
        return result;
    }

    /**
     * @brief Concatenates the given text object with the current text object.
     *
     * This operator overloads the '+' operator to concatenate the current text
     * object with the provided text object. The result is a new text object
     * containing the combined contents of both text objects.
     *
     * @param str The text object to be concatenated with the current text object.
     * @return text A new text object containing the concatenated result.
     */
    template <int U> inline text operator+(const text<U> &str) {
        text result = *this;
        result.append(str);
        return result;
    }

    /**
     * @brief Appends a C-string to the current text object.
     *
     * This operator overload allows for the concatenation of a C-string to the
     * current text object using the += operator.
     *
     * @param str The C-string to be appended to the current text object.
     * @return A reference to the current text object after the C-string has been appended.
     */
    inline text &operator+=(const char *str) {
        append(str);
        return *this;
    }

    /**
     * @brief Appends a character to the current text object.
     *
     * This operator overload allows for the concatenation of a character to the
     * current text object using the += operator.
     *
     * @param c The character to be appended to the current text object.
     * @return A reference to the current text object after the character has been appended.
     */
    template <int U> inline text friend operator+(const char *str, const text<U> &txt) {
        text result(str);
        result.append(txt);
        return result;
    }

    /**
     * @brief Appends a character to the text object.
     *
     * This operator overload allows for the addition of a single character
     * to the current text object. It appends the character to the end of the
     * text and returns a reference to the modified text object.
     *
     * @param c The character to be appended.
     * @return A reference to the modified text object.
     */
    template <int U> inline text friend operator+(const text<U> &txt, const char *str) {
        text result(txt);
        result.append(str);
        return result;
    }

    /**
     * @brief Appends a character to the text object.
     *
     * This operator overload allows for the addition of a single character
     * to the current text object. It appends the character to the end of the
     * text and returns a reference to the modified text object.
     *
     * @param c The character to be appended.
     * @return A reference to the modified text object.
     */
    template <int U> inline text friend operator+(char c, const text<U> &txt) {
        text result(c);
        result.append(txt);
        return result;
    }

    /**
     * @brief Appends a character to the text object.
     *
     * This operator overload allows for the addition of a single character
     * to the current text object. It appends the character to the end of the
     * text and returns a reference to the modified text object.
     *
     * @param c The character to be appended.
     * @return A reference to the modified text object.
     */
    template <int U> inline text friend operator+(const text<U> &txt, char c) {
        text result(txt);
        result.append(c);
        return result;
    }

    /**
     * @brief Appends a character to the text object.
     *
     * This operator overload allows for the addition of a single character
     * to the current text object. It appends the character to the end of the
     * text and returns a reference to the modified text object.
     *
     * @param c The character to be appended.
     * @return A reference to the modified text object.
     */
    inline text &operator+=(char c) {
        append(c);
        return *this;
    }

    /**
     * @brief Overloads the += operator to append the given text to the current text.
     *
     * This operator allows for the concatenation of another text object to the current
     * text object using the += syntax. It appends the content of the provided text
     * object to the current text object and returns a reference to the current text object.
     *
     * @param str The text object to be appended to the current text object.
     * @return text& A reference to the current text object after appending the given text.
     */
    inline text &operator+=(text &str) {
        append(str);
        return *this;
    }

    /**
     * @brief Counts the occurrences of a character in the text.
     *
     * This function counts the number of occurrences of the specified character
     * in the text and returns the count.
     *
     * @param c The character to count.
     * @return The number of occurrences of the character in the text.
     */
    inline std::size_t count(char c) {
        std::size_t count = 0;
        char *data_ptr = local_data() ? data : get_pointer();
        for (std::size_t i = 0; i < size(); ++i) {
            if (data_ptr[i] == c) {
                ++count;
            }
        }
        return count;
    }

    /**
     * @brief Implicit conversion operator to a constant pointer of value_type.
     *
     * This operator allows the object to be implicitly converted to a constant
     * pointer of value_type, providing direct access to the underlying data.
     *
     * @return A constant pointer to the underlying data.
     */
    inline operator const value_type *() const {
        if (local_data()) {
            return data;
        } else {
            return get_pointer();
        }
    }

    /**
     * @brief Returns an iterator to the beginning of the container.
     *
     * This function provides access to the first element in the container.
     * It returns a pointer to the first element, allowing iteration from the start.
     *
     * @return value_type* Pointer to the first element in the container.
     */
    inline value_type *begin() {
        if (local_data()) {
            return data;
        } else {
            return get_pointer();
        }
    }

    /**
     * @brief Returns a pointer to the end of the container.
     *
     * This function provides an iterator-like pointer to the position
     * one past the last element in the container. It can be used to
     * iterate over the container in conjunction with the `begin()` function.
     *
     * @return A pointer to the position one past the last element in the container.
     */
    inline value_type *end() {
        if (local_data()) {
            return data + size();
        } else {
            return get_pointer() + size();
        }
    }

    /**
     * @brief Returns a pointer to the beginning of the data.
     *
     * This function provides a constant pointer to the first element of the data array.
     *
     * @return A constant pointer to the first element of the data array.
     */
    inline const value_type *begin() const {
        if (local_data()) {
            return data;
        } else {
            return get_pointer();
        }
    }

    /**
     * @brief Returns a pointer to the end of the text container.
     *
     * This function provides a pointer to the position just past the last
     * character in the text container. It is useful for iterating over the
     * container using pointer arithmetic.
     *
     * @return A pointer to the position just past the last character in the container.
     */
    inline const value_type *end() const {
        if (local_data()) {
            return data + size();
        } else {
            return get_pointer() + size();
        }
    }

    friend std::ostream &operator<<(std::ostream &os, const text &str) {
        const char *data_ptr = str.local_data() ? str.data : str.get_pointer();
        os << data_ptr;
        return os;
    }

    /**
     * @brief Reverses the text container.
     *
     * This function creates a new text object with the characters of the
     * current text object in reverse order. It returns the reversed text object.
     *
     * @return A new text object with the characters in reverse order.
     */
    inline text reversed() {
        text result;
        result.resize(size());
        char *data_ptr = local_data() ? data : get_pointer();
        char *result_ptr = result.local_data() ? result.data : result.get_pointer();
        for (std::size_t i = 0; i < size(); ++i) {
            result_ptr[size() - i - 1] = data_ptr[i];
        }
        result_ptr[size()] = '\0';
        return result;
    }

    /**
     * @brief Checks if the text is a number.
     *
     * This function checks if the text contains only numeric characters.
     * It returns true if all characters in the text are numeric, and false otherwise.
     *
     * @return true if the text is a number, false otherwise.
     */
    bool is_number() {
        char *data_ptr = local_data() ? data : get_pointer();
        for (std::size_t i = 0; i < size(); ++i) {
            if (!std::isdigit(data_ptr[i])) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Converts the text to an integer.
     *
     * This function converts the text to an integer using `std::strtol`.
     * If the text is not a valid integer, it returns 0.
     *
     * @return The integer value of the text.
     */
    double to_double() {
        char *data_ptr = local_data() ? data : get_pointer();
        return std::strtod(data_ptr, nullptr);
    }

    /**
     * @brief Converts the text to an integer.
     *
     * This function converts the text to an integer using `std::strtol`.
     * If the text is not a valid integer, it returns 0.
     *
     * @return The integer value of the text.
     */
    int to_int() {
        char *data_ptr = local_data() ? data : get_pointer();
        return std::atoi(data_ptr);
    }

    /**
     * @brief Converts the text to a long integer.
     *
     * This function converts the text to a long integer using `std::strtol`.
     * If the text is not a valid long integer, it returns 0.
     *
     * @return The long integer value of the text.
     */
    long to_long() {
        char *data_ptr = local_data() ? data : get_pointer();
        return std::strtol(data_ptr, nullptr, 10);
    }

    /**
     * @brief Converts the text to a long long integer.
     *
     * This function converts the text to a long long integer using `std::strtoll`.
     * If the text is not a valid long long integer, it returns 0.
     *
     * @return The long long integer value of the text.
     */
    long long to_long_long() {
        char *data_ptr = local_data() ? data : get_pointer();
        return std::strtoll(data_ptr, nullptr, 10);
    }

    /**
     * @brief Converts the text to a float.
     *
     * This function converts the text to a float using `std::strtof`.
     * If the text is not a valid float, it returns 0.
     *
     * @return The float value of the text.
     */
    float to_float() {
        char *data_ptr = local_data() ? data : get_pointer();
        return std::strtof(data_ptr, nullptr);
    }

    /**
     * @brief Converts all characters in the text to uppercase.
     *
     * This function creates a new text object with all characters converted to their
     * uppercase equivalents. The size of the new text object will be the same as the
     * original.
     *
     * @return A new text object with all characters in uppercase.
     */
    inline text upper() {
        text result;
        result.resize(size());
        char *data_ptr = local_data() ? data : get_pointer();
        char *result_ptr = result.local_data() ? result.data : result.get_pointer();
        for (std::size_t i = 0; i < size(); ++i) {
            result_ptr[i] = std::toupper(data_ptr[i]);
        }
        result_ptr[size()] = '\0';
        return result;
    }

    /**
     * Converts all characters in the text to lowercase.
     *
     * @return A new text object with all characters converted to lowercase.
     */
    inline text lower() {
        text result;
        result.resize(size());
        char *data_ptr = local_data() ? data : get_pointer();
        char *result_ptr = result.local_data() ? result.data : result.get_pointer();
        for (std::size_t i = 0; i < size(); ++i) {
            result_ptr[i] = std::tolower(data_ptr[i]);
        }
        result_ptr[size()] = '\0';
        return result;
    }

    /**
     * @brief Splits the text into substrings using the specified delimiter.
     *
     * This function splits the text into substrings using the specified delimiter
     * character. It returns an array of text objects, each containing a substring.
     *
     * @param delimiter The character to split the text on.
     * @return An array of text objects containing the substrings.
     */
    containers::vector<text> split(char delimiter) {
        containers::vector<text> result;
        int count = 0;
        char *data_ptr = local_data() ? data : get_pointer();
        for (std::size_t i = 0; i < size(); ++i) {
            if (data_ptr[i] == delimiter) {
                count++;
            }
        }
        result.resize(count + 1);
        int index = 0;
        int start = 0;
        for (std::size_t i = 0; i < size(); ++i) {
            if (data_ptr[i] == delimiter) {
                result[index] = substr(start, i);
                start = i + 1;
                index++;
            }
        }
        result[index] = substr(start, size());
        return result;
    }

    /**
     * @brief Slices the text at the specified index.
     *
     * This function truncates the text by inserting a null terminator ('\0')
     * at the specified index, effectively reducing the size of the text to
     * the given index.
     *
     * @param index The position at which to slice the text. If the index is
     *              greater than or equal to the current size, the function
     *              does nothing.
     */
    inline void slice(size_type index) {
        if (index < size()) {
            char *data_ptr = local_data() ? data : get_pointer();
            data_ptr[index] = '\0';
            set_size(index);
        }
    }

    /**
     * @brief Equality comparison operator for the text class.
     *
     * This operator compares the current text object with another text object
     * to determine if they are equal. It returns true if the two text objects
     * have the same size and contain the same characters, and false otherwise.
     *
     * @param str The text object to compare with.
     * @return true if the text objects are equal, false otherwise.
     */
    inline bool operator==(const text &str) const {
        if (size() != str.size()) {
            return false;
        }
        const char *data_ptr = local_data() ? data : get_pointer();
        const char *str_ptr = str.local_data() ? str.data : str.get_pointer();
        return std::strncmp(data_ptr, str_ptr, size()) == 0;
    }

    /**
     * @brief Equality comparison operator for the text class.
     *
     * This operator compares the current text object with a C-style string
     * to determine if they are equal. It returns true if the text object
     * has the same size and contains the same characters as the C-style string,
     * and false otherwise.
     *
     * @param str The C-style string to compare with.
     * @return true if the text object is equal to the C-style string, false otherwise.
     */

    inline bool operator==(const char *str) const {
        const char *data_ptr = local_data() ? data : get_pointer();
        return std::strncmp(data_ptr, str, size()) == 0;
    }

    /**
     * @brief Inequality comparison operator for the text class.
     *
     * This operator compares the current text object with another text object
     * to determine if they are not equal. It returns true if the two text objects
     * have different sizes or contain different characters, and false otherwise.
     *
     * @param str The text object to compare with.
     * @return true if the text objects are not equal, false otherwise.
     */
    inline bool operator!=(const text &str) const { return !(*this == str); }

    /**
     * @brief Less-than comparison operator for the text class.
     *
     * This operator compares the current text object with another text object
     * to determine if the current text object is less than the other text object.
     * It returns true if the current text object is lexicographically less than
     * the other text object, and false otherwise.
     *
     * @param str The text object to compare with.
     * @return true if the current text object is less than the other text object, false otherwise.
     */
    inline bool operator<(const text &str) const {
        const char *data_ptr = local_data() ? data : get_pointer();
        const char *str_ptr = str.local_data() ? str.data : str.get_pointer();
        return std::strcmp(data_ptr, str_ptr) < 0;
    }

    /**
     * @brief Greater-than comparison operator for the text class.
     *
     * This operator compares the current text object with another text object
     * to determine if the current text object is greater than the other text object.
     * It returns true if the current text object is lexicographically greater than
     * the other text object, and false otherwise.
     *
     * @param str The text object to compare with.
     * @return true if the current text object is greater than the other text object, false otherwise.
     */
    inline bool operator>(const text &str) const {
        const char *data_ptr = local_data() ? data : get_pointer();
        const char *src_ptr = str.local_data() ? str.data : str.get_pointer();
        return std::strcmp(data_ptr, src_ptr) > 0;
    }

    /**
     * @brief Less-than-or-equal comparison operator for the text class.
     *
     * This operator compares the current text object with another text object
     * to determine if the current text object is less than or equal to the other
     * text object. It returns true if the current text object is lexicographically
     * less than or equal to the other text object, and false otherwise.
     *
     * @param str The text object to compare with.
     * @return true if the current text object is less than or equal to the other text object, false otherwise.
     */
    inline bool operator<=(const text &str) const {
        const char *data_ptr = local_data() ? data : get_pointer();
        const char *str_ptr = str.local_data() ? str.data : str.get_pointer();
        return std::strcmp(data_ptr, str_ptr) <= 0;
    }

    /**
     * @brief Greater-than-or-equal comparison operator for the text class.
     *
     * This operator compares the current text object with another text object
     * to determine if the current text object is greater than or equal to the other
     * text object. It returns true if the current text object is lexicographically
     * greater than or equal to the other text object, and false otherwise.
     *
     * @param str The text object to compare with.
     * @return true if the current text object is greater than or equal to the other text object, false otherwise.
     */
    inline bool operator>=(const text &str) const {
        const char *data_ptr = local_data() ? data : get_pointer();
        const char *str_ptr = str.local_data() ? str.data : str.get_pointer();
        return std::strcmp(data_ptr, str_ptr) >= 0;
    }

    /**
     * @brief Compares this text object with another text object using the three-way comparison operator.
     *
     * @param str The text object to compare with.
     * @return A std::strong_ordering value indicating the result of the comparison:
     *         - std::strong_ordering::less if this text is less than the other text.
     *         - std::strong_ordering::equal if this text is equal to the other text.
     *         - std::strong_ordering::greater if this text is greater than the other text.
     */
    inline auto operator<=>(const text &str) const {
        const char *data_ptr = local_data() ? data : get_pointer();
        const char *str_ptr = str.local_data() ? str.data : str.get_pointer();
        return std::strcmp(data_ptr, str_ptr) <=> 0;
    }

    /**
     * @brief Creates a text object from the contents of a file.
     *
     * This function reads the entire content of the specified file and stores it
     * in a text object. The file is read in binary mode to ensure that all bytes
     * are read correctly, and the resulting text object is null-terminated.
     *
     * @param filename The path to the file to be read.
     * @return A text object containing the contents of the file. If the file
     *         cannot be opened, an empty text object is returned.
     */
    static text from_file(const char *filename) {
        std::ifstream file(filename, std::ios::in | std::ios::binary | std::ios::ate);
        if (file.is_open()) {
            std::size_t size = file.tellg();
            text result;
            result.reserve(size);
            char *result_ptr = result.local_data() ? result.data : result.get_pointer();
            file.seekg(0, std::ios::beg);
            file.read(result_ptr, size);
            result_ptr[size] = '\0';
            result.set_size(size);
            file.close();
            return result;
        }
        return text();
    }

    /**
     * @brief Writes the contents of the data buffer to a file.
     *
     * This function opens a file with the specified filename in binary write mode,
     * writes the contents of the data buffer to the file, and then closes the file.
     *
     * @param filename The name of the file to write to.
     */
    inline void to_file(const char *filename) {
        std::ofstream file(filename, std::ios::out | std::ios::binary);
        char *data_ptr = local_data() ? data : get_pointer();
        file.write(data_ptr, size());
        file.close();
    }
};
} // namespace containers
<<<<<<< HEAD
=======
using text16 = containers::text<16>;
>>>>>>> 135bbbf9bff8e10850f7d009ee512de4c621074d
using text32 = containers::text<32>;
using text64 = containers::text<64>;
using text128 = containers::text<128>;
using text256 = containers::text<256>;
using text512 = containers::text<512>;
using text1024 = containers::text<1024>;
using text2048 = containers::text<2048>;
using text4096 = containers::text<4096>;
<<<<<<< HEAD
// GTR deafult string class
using string = text64;

template <typename T> inline string to_string(T value);
/**
 * @brief Converts a boolean value to a string representation.
 *
 * This specialization of the to_string function template converts a boolean value
 * to its string representation. It returns "true" if the value is true, and "false"
 * if the value is false.
 *
 * @param value The boolean value to convert.
 * @return string The string representation of the boolean value.
 */
template <> inline string to_string(bool value) {
    return value ? "true" : "false";
}

/**
 * @brief Converts an integer value to a string representation.
 *
 * This specialization of the to_string function template converts an integer value
 * to its string representation. It uses snprintf to format the integer as a string.
 *
 * @param value The integer value to convert.
 * @return string The string representation of the integer value.
 */
template <> inline string to_string(int value) {
    char buffer[32];
    std::snprintf(buffer, 32, "%d", value);
    return string(buffer);
}

/**
 * @brief Converts a long integer value to a string representation.
 *
 * This specialization of the to_string function template converts a long integer value
 * to its string representation. It uses snprintf to format the long integer as a string.
 *
 * @param value The long integer value to convert.
 * @return string The string representation of the long integer value.
 */
template <> inline string to_string(long value) {
    char buffer[32];
    std::snprintf(buffer, 32, "%ld", value);
    return string(buffer);
}

/**
 * @brief Converts a long long integer value to a string representation.
 *
 * This specialization of the to_string function template converts a long long integer value
 * to its string representation. It uses snprintf to format the long long integer as a string.
 *
 * @param value The long long integer value to convert.
 * @return string The string representation of the long long integer value.
 */
template <> inline string to_string(long long value) {
    char buffer[32];
    std::snprintf(buffer, 32, "%lld", value);
    return string(buffer);
}

/**
 * @brief Converts an unsigned integer value to a string representation.
 *
 * This specialization of the to_string function template converts an unsigned integer value
 * to its string representation. It uses snprintf to format the unsigned integer as a string.
 *
 * @param value The unsigned integer value to convert.
 * @return string The string representation of the unsigned integer value.
 */
template <> inline string to_string(unsigned value) {
    char buffer[32];
    std::snprintf(buffer, 32, "%u", value);
    return string(buffer);
}

/**
 * @brief Converts an unsigned long integer value to a string representation.
 *
 * This specialization of the to_string function template converts an unsigned long integer value
 * to its string representation. It uses snprintf to format the unsigned long integer as a string.
 *
 * @param value The unsigned long integer value to convert.
 * @return string The string representation of the unsigned long integer value.
 */
template <> inline string to_string(unsigned long value) {
    char buffer[32];
    std::snprintf(buffer, 32, "%lu", value);
    return string(buffer);
}

/**
 * @brief Converts an unsigned long long integer value to a string representation.
 *
 * This specialization of the to_string function template converts an unsigned long long integer value
 * to its string representation. It uses snprintf to format the unsigned long long integer as a string.
 *
 * @param value The unsigned long long integer value to convert.
 * @return string The string representation of the unsigned long long integer value.
 */
template <> inline string to_string(unsigned long long value) {
    char buffer[32];
    std::snprintf(buffer, 32, "%llu", value);
    return string(buffer);
}

/**
 * @brief Converts a float value to a string representation.
 *
 * This specialization of the to_string function template converts a float value
 * to its string representation. It uses snprintf to format the float as a string.
 *
 * @param value The float value to convert.
 * @return string The string representation of the float value.
 */
template <> inline string to_string(float value) {
    char buffer[32];
    std::snprintf(buffer, 32, "%f", value);
    return string(buffer);
}

/**
 * @brief Converts a double value to a string representation.
 *
 * This specialization of the to_string function template converts a double value
 * to its string representation. It uses snprintf to format the double as a string.
 *
 * @param value The double value to convert.
 * @return string The string representation of the double value.
 */
template <> inline string to_string(double value) {
    char buffer[32];
    std::snprintf(buffer, 32, "%f", value);
    return string(buffer);
}
=======
using string = text64;
>>>>>>> 135bbbf9bff8e10850f7d009ee512de4c621074d
} // namespace gtr
#endif