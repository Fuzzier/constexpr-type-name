/**
 * @file
 *
 * @brief Type name at compile time.
 *
 * @author  Wei Tang <gauchyler@uestc.edu.cn>
 * @date    2025-03-14
 *
 * @copyright Copyright (c) 2025.
 *   National Key Laboratory of Science and Technology on Communications,
 *   University of Electronic Science and Technology of China.
 *   All rights reserved.
 */

#ifndef TYPE_NAME_HPP__9CFF9E19_0F21_4E1D_AE6F_C9A92C919C06
#define TYPE_NAME_HPP__9CFF9E19_0F21_4E1D_AE6F_C9A92C919C06

#include <string_view>
#include <type_traits>
#include <iostream>


#if !defined(NSFX_FUNCTION)
# if defined(__GNUC__) || defined(__clang__)
#  define NSFX_FUNCTION   __PRETTY_FUNCTION__
# elif defined(_MSC_VER)
#  define NSFX_FUNCTION   __FUNCSIG__
# else
#  error Unsupported compiler.
# endif
#endif // !defined(NSFX_FUNCTION)


namespace nsfx {

/**
 * @brief A fixed length string.
 *
 * @tparam N The capacity of the string.
 */
template<std::size_t N>
struct fixed_string_t
{
    char data_[N];
    std::size_t size_;

    static constexpr std::size_t npos = (std::size_t)(-1);
    static constexpr std::size_t capacity_ = N;

    constexpr fixed_string_t(void) noexcept = default;

    constexpr fixed_string_t(const char* str, std::size_t len) noexcept
        : fixed_string_t{}
    {
        for (size_ = 0; size_ < len && size_ < N - 1; ++size_)
        {
            data_[size_] = str[size_];
        }
        data_[size_] = '\0';
    }

    template<std::size_t M>
    constexpr fixed_string_t(const char (&str)[M]) noexcept
        : fixed_string_t{}
    {
        for (size_ = 0; size_ < M - 1 && size_ < N - 1; ++size_)
        {
            data_[size_] = str[size_];
        }
        data_[size_] = '\0';
    }

    constexpr std::string_view view(void) const noexcept
    {
        return std::string_view{data_, size_};
    }

    constexpr char& operator[](std::size_t i) noexcept
    {
        return data_[i];
    }

    constexpr const char& operator[](std::size_t i) const noexcept
    {
        return data_[i];
    }

    constexpr std::size_t find(const char c) const noexcept
    {
        if (size_)
        {
            std::size_t pos = 0;
            while (pos < size_)
            {
                if (data_[pos] == c)
                {
                    return pos;
                }
                ++pos;
            }
        }
        return npos;
    }

    constexpr std::size_t rfind(const char c) const noexcept
    {
        std::size_t pos = size_ - 1;
        if (size_)
        {
            do
            {
                if (data_[pos] == c)
                {
                    break;
                }
            }
            while (pos--);
        }
        return pos;
    }

};

/**
 * @brief Make a fixed string from a string literal.
 */
template<std::size_t N>
constexpr fixed_string_t<N>
to_fixed_string(const char (&src)[N]) noexcept
{
    return fixed_string_t<N>{src};
}

namespace details {
namespace type_name {

template<class T>
struct full
{
    // full<nsfx::Xxx>::get()
    //
    // g++  : static constexpr auto nsfx::details::type_name::full<T>::get() [with T = nsfx::Xxx]
    //                                                                                 ^^^^^^^^^
    // clang: static auto nsfx::details::type_name::full<nsfx:Xxx>::get() [T = nsfx::Xxx]
    //                                                   ^^^^^^^^              ^^^^^^^^^
    // msvc : auto __cdecl nsfx::details::type_name::full<struct nsfx::Xxx>::get(void)
    //                                                    ^^^^^^^^^^^^^^^^
    static constexpr auto get(void)
    {
        return std::string_view{NSFX_FUNCTION};
    }
};

// full<int>::get()
//
// g++  : static constexpr auto nsfx::details::type_name::full<T>::get() [with T = int]
//                                                                                 ^^^
// clang: static auto nsfx::details::type_name::full<int>::get() [T = int]
//                                                   ^^^              ^^^
// msvc : auto __cdecl nsfx::details::type_name::full<int>::get(void)
//                                                    ^^^
// The index of the first character of the type name.
inline constexpr std::size_t name_start_pos = full<int>::get().find("int");

// full<void>::get()
//
// g++  : static constexpr auto nsfx::details::type_name::full<T>::get() [with T = void]
//                                                                                 ^^^^
// clang: static auto nsfx::details::type_name::full<void>::get() [T = void]
//                                                   ^^^^              ^^^^
// msvc : auto __cdecl nsfx::details::type_name::full<void>::get(void)
//                                                    ^^^^
// How many times the type name appears in the full.
inline constexpr std::size_t num_appearance = full<void>::get().size()
                                            - full<int>::get().size();

// The number of characters excluding the type name.
inline constexpr std::size_t num_misc_chars = full<void>::get().size()
                                            - 4 * num_appearance;

/**
 * @brief Check whether a character is part of an identifier name.
 */
constexpr bool iskey(const char c) noexcept
{
    // 0-9a-zA-Z_
    return (('0' <= c && c <= '9') ||
            ('A' <= c && c <= 'Z') ||
            ('a' <= c && c <= 'z') ||
            ('_' == c));
}

/**
 * @brief Check whether a delimited substring is within a string.
 *
 * It is required that the substring is delimited.
 * That is the substring is surrounded by non-ID character.
 *
 * @pre `(pos == 0) || !iskey(str[pos-1])`
 *
 * @return
 *   The length of the substring to be removed from the type name.\n
 *   If the match is found, a non-zero value is returned.\n
 *   Otherwise, `0` is returned.
 */
template<std::size_t N, std::size_t K>
constexpr std::size_t match(
    const fixed_string_t<N>& str, std::size_t pos, std::size_t len,
    const char (&sub)[K]) noexcept
{
    constexpr std::size_t L = K - 1;
    // `sub` is zero-terminated, that is sub[K-1] == '\0'.
    // Check whether `str[pos : pos+L-1]` matches `sub[0 : L-1]`.
    std::size_t n = 0;
    while (pos + n < len && n < L)
    {
        if (str[pos + n] != sub[n])
        {
            break;
        }
        ++n;
    }
    // If they do not match.
    if (n != L)
    {
        n = 0;
    }
    // Otherwise, they match.
    else
    {
        if (pos + L != len)
        {
            // If `sub` is followed by an identifier character, then
            // it is part of a long identifier, and is not removed.
            // e.g. "structA" :  "struct" is not removed.
            //       ^^^^^^
            // e.g. "struct(" :  "struct" is removed.
            //       ^^^^^^
            if (iskey(str[pos + L]))
            {
                n = 0;
            }
            else
            {
                // If `str[pos+L]` is SPACE.
                if (str[pos + L] == ' ')
                {
                    // The SPACE is to be removed.
                    // e.g. "struct " :  space after "struct" is also removed.
                    //             ^
                    ++n;
                }
            }
        }
    }
    // The number of characters to remove.
    return n;
}

/**
 * @brief Get the raw type name of a type.
 *
 * @remark
 *   Since `__PRETTY_FUNCTION__` and `__FUNCSIG__` contains the class name and
 *   member function name,
 *   the name length of class `impl` **must** be the same as
 *   the name length of class `full`, and
 *   the name length of member function `impl::get()` **must** be the same as
 *   the name length of member function `full::get()`.
 *   Therefore, the `name_start_pos`, `num_appearance` and `num_misc_chars`
 *   are consistent for the return values of `full::get()` and `impl::get()`.
 */
template<class T>
struct impl
{
    /**
     * @brief Get the raw type name.
     *
     * @return The returned `fixed_string_t<>` is zero-terminated.
     */
    static constexpr auto raw(void)
    {
        // `full` is zero-terminated.
        constexpr auto full = to_fixed_string(NSFX_FUNCTION);
        // Extract type name from `full`.
        constexpr std::size_t N = full.capacity_;
        constexpr std::size_t L = (N - 1 - num_misc_chars) / num_appearance;
        // `name` is zero-terminated.
        return fixed_string_t<L+1>{full.data_ + name_start_pos, L};
    }

    /**
     * @brief Get the size of tidy type name.
     *
     * The keywords `enum`, `class`, `struct` and `__cdecl` are removed from
     * the type name.
     *
     * @return The returned `fixed_string_t<>` is zero-terminated.
     */
    static constexpr std::size_t dry(void) noexcept
    {
        auto name = raw();
#if defined(__clang__) || defined(__GNUC__)
        return name.capacity_;
#elif defined(_MSC_VER)
        constexpr std::size_t len = name.capacity_ - 1;
        std::size_t size = 0;
        std::size_t pos = 0;
        while (true)
        {
            // The number of characters to remove.
            std::size_t n = 0;
            n = match(name, pos, len, "enum");
            if (n) { pos += n; continue; }
            n = match(name, pos, len, "class");
            if (n) { pos += n; continue; }
            n = match(name, pos, len, "struct");
            if (n) { pos += n; continue; }
            n = match(name, pos, len, "__cdecl");
            if (n) { pos += n; continue; }
            // The character before `sub` is not an identifier character.
            while (pos < len && iskey(name[pos]))
            {
                ++size;
                ++pos;
            }
            // The character before `sub` is not an identifier character.
            while (pos < len && !iskey(name[pos]))
            {
                ++size;
                ++pos;
            }
            // The current character is a non-identifier character.
            if (pos == len)
            {
                break;
            }
        }
        // Strip trailing spaces.
        while (pos && name[pos - 1] == ' ')
        {
            --pos;
            --size;
        }
        return size;
#else
# error Unsupported compiler.
#endif
    }

    /**
     * @brief Get the tidy type name.
     *
     * The keywords `enum`, `class`, `struct` and `__cdecl` are removed from
     * the type name.
     *
     * @return The returned `fixed_string_t<>` is zero-terminated.
     */
    static constexpr auto tidy(void) noexcept
    {
        auto name = raw();
#if defined(__clang__) || defined(__GNUC__)
        return name;
#elif defined(_MSC_VER)
        constexpr std::size_t len = name.capacity_ - 1;
        constexpr std::size_t L = dry();
        fixed_string_t<L+1> dst {};
        std::size_t pos = 0;
        while (true)
        {
            // The number of characters to remove.
            std::size_t n = 0;
            n = match(name, pos, len, "enum");
            if (n) { pos += n; continue; }
            n = match(name, pos, len, "class");
            if (n) { pos += n; continue; }
            n = match(name, pos, len, "struct");
            if (n) { pos += n; continue; }
            n = match(name, pos, len, "__cdecl");
            if (n) { pos += n; continue; }
            // The character before `sub` is not an identifier character.
            while (dst.size_ < L && pos < len && iskey(name[pos]))
            {
                dst[dst.size_++] = name[pos++];
            }
            // The character before `sub` is not an identifier character.
            while (dst.size_ < L && pos < len && !iskey(name[pos]))
            {
                dst[dst.size_++] = name[pos++];
            }
            // The current character is a non-identifier character.
            if (dst.size_ == L || pos == len)
            {
                break;
            }
        }
        dst[L] = '\0';
        return dst;
#else
# error Unsupported compiler.
#endif
    }

    /**
     * @brief Get the unqualified type name.
     *
     * @pre The type **must** be a value type.
     *
     * @return The returned `fixed_string_t<>` is zero-terminated.
     */
    static constexpr auto base(void) noexcept
    {
        if constexpr (std::is_const_v<T>     ||
                      std::is_volatile_v<T>  ||
                      std::is_array_v<T>     ||
                      std::is_pointer_v<T>   ||
                      std::is_reference_v<T> ||
                      // T is not a function type, possibly qualified.
                      // e.g., int (float)
                      // e.g., int (float) const & noexcept
                      std::is_function_v<T>  ||
                      // T is not a member object pointer.
                      // e.g., int (C::*)
                      // T is not a member function pointer.
                      // e.g., int (C::*)(float)
                      std::is_member_pointer_v<T>)
        {
            return tidy();
        }
        else
        {
            constexpr auto name = tidy();
            // Find the last ':'.
            constexpr std::size_t pos = name.rfind(':');
            if constexpr (pos == name.npos)
            {
                return name;
            }
            else
            {
                constexpr std::size_t N = name.capacity_ - pos - 1;
                return fixed_string_t<N>{name.data_ + pos + 1, N};
            }
        }
    }
};


} // namespace type_name
} // namespace details


////////////////////////////////////////////////////////////////////////////////
/**
 * @ingroup NsfxTypeId
 *
 * @brief Type name.
 *
 * @see https://stackoverflow.com/questions/35941045/how-can-i-obtain-a-c-expressions-type-at-compile-time-i-e-constexprly
 */
template<class T>
struct type_name
{
    using type = T;

    /**
     * @brief Get the raw type name.
     *
     * @return The returned `fixed_string_t<>` is zero-terminated.
     */
    static constexpr auto raw(void) noexcept
    {
        return details::type_name::impl<T>::raw();
    }

    /**
     * @brief Get the type name.
     *
     * @return The returned `fixed_string_t<>` is zero-terminated.
     */
    static constexpr auto name(void) noexcept
    {
        return details::type_name::impl<T>::tidy();
    }

    /**
     * @brief Get the unqualified type name.
     *
     * @return The returned `fixed_string_t<>` is zero-terminated.
     */
    static constexpr auto base(void) noexcept
    {
        return details::type_name::impl<T>::base();
    }

};


template<std::size_t N>
std::ostream& operator<<(std::ostream& os, const fixed_string_t<N>& s)
{
    return os << s.view();
}

template<class T>
std::ostream& operator<<(std::ostream& os, type_name<T> v)
{
    return os << v.name();
}

} // namespace nsfx


#endif // TYPE_NAME_HPP__9CFF9E19_0F21_4E1D_AE6F_C9A92C919C06
