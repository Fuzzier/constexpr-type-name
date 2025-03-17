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

#include "type-name.hpp"

namespace t {

enum E {};
enum class EC {};
enum struct ES {};
class C {};
struct S {};

template<class T>
void show(void)
{
    constexpr auto raw = nsfx::type_name<T>::raw();
    std::cout << raw << std::endl;
    constexpr auto name = nsfx::type_name<T>::name();
    std::cout << "  N:  " << name << std::endl;
    constexpr auto base = nsfx::type_name<T>::base();
    std::cout << "  B:  " << base << std::endl;
}

} // namespace t


int main(void)
{
    using namespace t;
    ////////////////////
    // builtin
    ////////////////////
    show<void>();
    show<int>();
    //////////
    show<const void>();
    show<const int>();
    //////////
    show<const int&>();
    //////////
    show<const void*>();
    show<const int*>();
    ////////////////////
    // enum/class/struct
    ////////////////////
    show<E>();
    show<EC>();
    show<ES>();
    show<C>();
    show<S>();
    //////////
    show<const E>();
    show<const C>();
    show<const S>();
    //////////
    show<const E&>();
    show<const C&>();
    show<const S&>();
    //////////
    show<const E*>();
    show<const C*>();
    show<const S*>();
    //////////
    show<volatile E>();
    show<volatile C>();
    show<volatile S>();
    //////////
    show<const volatile E>();
    show<const volatile C>();
    show<const volatile S>();
    ////////////////////
    // array
    ////////////////////
    show<E[]>();
    show<C[1]>();
    show<S[2]>();
    //////////
    show<const E(&)[]>();
    show<const C(&)[1]>();
    show<const S(&)[2]>();
    ////////////////////
    // function
    ////////////////////
    show<E(C, S)>();
    show<E(C, S) &>();
    show<E(C, S) &&>();
    show<E(C, S) noexcept>();
    show<E(C, S) & noexcept>();
    show<E(C, S) && noexcept>();
    show<E(C, S) const>();
    show<E(C, S) const &>();
    show<E(C, S) const &&>();
    show<E(C, S) const noexcept>();
    show<E(C, S) const & noexcept>();
    show<E(C, S) const && noexcept>();
    //////////
    show<E(*)(C, S)>();
    show<E(*)(C, S) noexcept>();
    ////////////////////
    show<E(&)(C, S)>();
    show<E(&)(C, S) noexcept>();
    //////////
    show<E(C::*)(S)>();
    show<E(C::*)>();
    show<E(C::*&)(S)>();
    show<E(C::*&)>();

    return 0;
}
