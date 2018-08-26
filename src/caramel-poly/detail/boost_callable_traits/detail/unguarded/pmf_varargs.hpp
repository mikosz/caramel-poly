/*
Copyright (c) 2016 Barrett Adair

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

HEADER GUARDS INTENTIONALLY OMITTED
DO NOT INCLUDE THIS HEADER DIRECTLY

*/

#define BOOST_CLBL_TRTS_INCLUDE_QUALIFIERS
#define BOOST_CLBL_TRTS_INCLUDE_ABOMINABLE_QUALIFIERS
#include "unguarded/pmf_varargs_2.hpp"
#undef BOOST_CLBL_TRTS_INCLUDE_QUALIFIERS
#undef BOOST_CLBL_TRTS_INCLUDE_ABOMINABLE_QUALIFIERS

#define BOOST_CLBL_TRTS_INCLUDE_QUALIFIERS const
#define BOOST_CLBL_TRTS_INCLUDE_ABOMINABLE_QUALIFIERS \
    BOOST_CLBL_TRTS_ABOMINABLE_CONST
#include "unguarded/pmf_varargs_2.hpp"
#undef BOOST_CLBL_TRTS_INCLUDE_QUALIFIERS
#undef BOOST_CLBL_TRTS_INCLUDE_ABOMINABLE_QUALIFIERS

#define BOOST_CLBL_TRTS_INCLUDE_QUALIFIERS volatile
#define BOOST_CLBL_TRTS_INCLUDE_ABOMINABLE_QUALIFIERS \
    BOOST_CLBL_TRTS_ABOMINABLE_VOLATILE
#include "unguarded/pmf_varargs_2.hpp"
#undef BOOST_CLBL_TRTS_INCLUDE_QUALIFIERS
#undef BOOST_CLBL_TRTS_INCLUDE_ABOMINABLE_QUALIFIERS

#define BOOST_CLBL_TRTS_INCLUDE_QUALIFIERS const volatile
#define BOOST_CLBL_TRTS_INCLUDE_ABOMINABLE_QUALIFIERS \
    BOOST_CLBL_TRTS_ABOMINABLE_CONST BOOST_CLBL_TRTS_ABOMINABLE_VOLATILE
#include "unguarded/pmf_varargs_2.hpp"
#undef BOOST_CLBL_TRTS_INCLUDE_QUALIFIERS
#undef BOOST_CLBL_TRTS_INCLUDE_ABOMINABLE_QUALIFIERS

#ifndef BOOST_CLBL_TRTS_DISABLE_REFERENCE_QUALIFIERS

#define BOOST_CLBL_TRTS_INCLUDE_QUALIFIERS &
#define BOOST_CLBL_TRTS_INCLUDE_ABOMINABLE_QUALIFIERS &
#include "unguarded/pmf_varargs_2.hpp"
#undef BOOST_CLBL_TRTS_INCLUDE_QUALIFIERS
#undef BOOST_CLBL_TRTS_INCLUDE_ABOMINABLE_QUALIFIERS

#define BOOST_CLBL_TRTS_INCLUDE_QUALIFIERS &&
#define BOOST_CLBL_TRTS_INCLUDE_ABOMINABLE_QUALIFIERS &&
#include "unguarded/pmf_varargs_2.hpp"
#undef BOOST_CLBL_TRTS_INCLUDE_QUALIFIERS
#undef BOOST_CLBL_TRTS_INCLUDE_ABOMINABLE_QUALIFIERS

#define BOOST_CLBL_TRTS_INCLUDE_QUALIFIERS const &
#define BOOST_CLBL_TRTS_INCLUDE_ABOMINABLE_QUALIFIERS const &
#include "unguarded/pmf_varargs_2.hpp"
#undef BOOST_CLBL_TRTS_INCLUDE_QUALIFIERS
#undef BOOST_CLBL_TRTS_INCLUDE_ABOMINABLE_QUALIFIERS

#define BOOST_CLBL_TRTS_INCLUDE_QUALIFIERS volatile &
#define BOOST_CLBL_TRTS_INCLUDE_ABOMINABLE_QUALIFIERS volatile &
#include "unguarded/pmf_varargs_2.hpp"
#undef BOOST_CLBL_TRTS_INCLUDE_QUALIFIERS
#undef BOOST_CLBL_TRTS_INCLUDE_ABOMINABLE_QUALIFIERS

#define BOOST_CLBL_TRTS_INCLUDE_QUALIFIERS const volatile &
#define BOOST_CLBL_TRTS_INCLUDE_ABOMINABLE_QUALIFIERS const volatile &
#include "unguarded/pmf_varargs_2.hpp"
#undef BOOST_CLBL_TRTS_INCLUDE_QUALIFIERS
#undef BOOST_CLBL_TRTS_INCLUDE_ABOMINABLE_QUALIFIERS

#define BOOST_CLBL_TRTS_INCLUDE_QUALIFIERS const &&
#define BOOST_CLBL_TRTS_INCLUDE_ABOMINABLE_QUALIFIERS const &&
#include "unguarded/pmf_varargs_2.hpp"
#undef BOOST_CLBL_TRTS_INCLUDE_QUALIFIERS
#undef BOOST_CLBL_TRTS_INCLUDE_ABOMINABLE_QUALIFIERS

#define BOOST_CLBL_TRTS_INCLUDE_QUALIFIERS volatile &&
#define BOOST_CLBL_TRTS_INCLUDE_ABOMINABLE_QUALIFIERS volatile &&
#include "unguarded/pmf_varargs_2.hpp"
#undef BOOST_CLBL_TRTS_INCLUDE_QUALIFIERS
#undef BOOST_CLBL_TRTS_INCLUDE_ABOMINABLE_QUALIFIERS

#define BOOST_CLBL_TRTS_INCLUDE_QUALIFIERS const volatile &&
#define BOOST_CLBL_TRTS_INCLUDE_ABOMINABLE_QUALIFIERS const volatile &&
#include "unguarded/pmf_varargs_2.hpp"
#undef BOOST_CLBL_TRTS_INCLUDE_QUALIFIERS
#undef BOOST_CLBL_TRTS_INCLUDE_ABOMINABLE_QUALIFIERS

#endif // #ifndef BOOST_CLBL_TRTS_DISABLE_REFERENCE_QUALIFIERS