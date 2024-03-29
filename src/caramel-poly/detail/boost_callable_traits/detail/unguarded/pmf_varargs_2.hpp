/*
Copyright (c) 2016 Barrett Adair

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

HEADER GUARDS INTENTIONALLY OMITTED
DO NOT INCLUDE THIS HEADER DIRECTLY

*/

template<typename T, typename Return, typename... Args>
struct set_varargs_member_function_qualifiers_t <
    flag_map<int BOOST_CLBL_TRTS_INCLUDE_QUALIFIERS>::value,
    false, // IsTransactionSafe
    false, // IsNoexcept
    BOOST_CLBL_TRTS_CC_TAG, T, Return, Args...> {

    using type =
        Return(BOOST_CLBL_TRTS_VARARGS_CC T::*)(Args..., ...)
        BOOST_CLBL_TRTS_INCLUDE_QUALIFIERS;
};

template<typename T, typename Return, typename... Args>
struct set_varargs_member_function_qualifiers_t <
    flag_map<int BOOST_CLBL_TRTS_INCLUDE_QUALIFIERS>::value,
    false,
    true,
    BOOST_CLBL_TRTS_CC_TAG, T, Return, Args...> {

    using type =
        Return(BOOST_CLBL_TRTS_VARARGS_CC T::*)(Args..., ...)
        BOOST_CLBL_TRTS_INCLUDE_QUALIFIERS BOOST_CLBL_TRTS_NOEXCEPT_SPECIFIER;
};

template<typename T, typename Return, typename... Args>
struct set_varargs_member_function_qualifiers_t <
    flag_map<int BOOST_CLBL_TRTS_INCLUDE_QUALIFIERS>::value,
    true,
    false,
    BOOST_CLBL_TRTS_CC_TAG, T, Return, Args...> {

    using type =
        Return(BOOST_CLBL_TRTS_VARARGS_CC T::*)(Args..., ...)
            BOOST_CLBL_TRTS_INCLUDE_QUALIFIERS
            BOOST_CLBL_TRTS_TRANSACTION_SAFE_SPECIFIER;
};

template<typename T, typename Return, typename... Args>
struct set_varargs_member_function_qualifiers_t <
    flag_map<int BOOST_CLBL_TRTS_INCLUDE_QUALIFIERS>::value,
    true,
    true,
    BOOST_CLBL_TRTS_CC_TAG, T, Return, Args...> {

    using type =
        Return(BOOST_CLBL_TRTS_VARARGS_CC T::*)(Args..., ...)
            BOOST_CLBL_TRTS_INCLUDE_QUALIFIERS
            BOOST_CLBL_TRTS_TRANSACTION_SAFE_SPECIFIER
            BOOST_CLBL_TRTS_NOEXCEPT_SPECIFIER;
};

#define BOOST_CLBL_TRTS_INCLUDE_TRANSACTION_SAFE
#define BOOST_CLBL_TRTS_IS_TRANSACTION_SAFE std::false_type
#include "pmf_varargs_3.hpp"

#undef BOOST_CLBL_TRTS_INCLUDE_TRANSACTION_SAFE
#undef BOOST_CLBL_TRTS_IS_TRANSACTION_SAFE

#ifdef BOOST_CLBL_TRTS_ENABLE_TRANSACTION_SAFE

#define BOOST_CLBL_TRTS_IS_TRANSACTION_SAFE std::true_type
#define BOOST_CLBL_TRTS_INCLUDE_TRANSACTION_SAFE transaction_safe
#include "pmf_varargs_3.hpp"
#endif

#undef BOOST_CLBL_TRTS_INCLUDE_TRANSACTION_SAFE
#undef BOOST_CLBL_TRTS_IS_TRANSACTION_SAFE
