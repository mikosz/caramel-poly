// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)
//
// This is Louis Dionne's Dyno library adapted to work on Visual Studio 2017 and
// without Boost, modified to my taste. All credit for the design and delivery goes
// to Louis. His original implementation may be found here:
// https://github.com/ldionne/dyno

#ifndef CARAMELPOLY_DETAIL_ISPLACEHOLDER_HPP__
#define CARAMELPOLY_DETAIL_ISPLACEHOLDER_HPP__

#include <type_traits>

#include "../SelfPlaceholder.hpp"

namespace caramel_poly::detail {

// True if a type is a possibly const/ref-qualified placeholder, or a pointer to one.
template <class T>
constexpr auto isPlaceholder = false;

template <>
constexpr auto isPlaceholder<caramel_poly::SelfPlaceholder&> = true;

template <>
constexpr auto isPlaceholder<caramel_poly::SelfPlaceholder&&> = true;

template <>
constexpr auto isPlaceholder<caramel_poly::SelfPlaceholder*> = true;

template <>
constexpr auto isPlaceholder<const caramel_poly::SelfPlaceholder&> = true;

template <>
constexpr auto isPlaceholder<const caramel_poly::SelfPlaceholder&&> = true;

template <>
constexpr auto isPlaceholder<const caramel_poly::SelfPlaceholder*> = true;

} // namespace caramel_poly::detail

#endif /* CARAMELPOLY_DETAIL_ISPLACEHOLDER_HPP__ */
