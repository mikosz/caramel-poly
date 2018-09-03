// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)
//
// This is Louis Dionne's Dyno library adapted to work on Visual Studio 2017 and
// without Boost, modified to my taste. All credit for the design and delivery goes
// to Louis. His original implementation may be found here:
// https://github.com/ldionne/dyno

#ifndef CARAMELPOLY_BUILTIN_HPP__
#define CARAMELPOLY_BUILTIN_HPP__

#include "detail/ConstexprString.hpp"

namespace caramel_poly {

constexpr auto STORAGE_INFO_LABEL = METHOD_NAME("storage_info");
constexpr auto DESTRUCT_LABEL = METHOD_NAME("destruct");
constexpr auto COPY_CONSTRUCT_LABEL = METHOD_NAME("copy-construct");
constexpr auto MOVE_CONSTRUCT_LABEL = METHOD_NAME("move-construct");

// Encapsulates the minimal amount of information required to allocate
// storage for an object of a given type.
//
// This should never be created explicitly; always use `caramel_poly::storageInfoFor`.
struct StorageInfo {
	std::size_t size;
	std::size_t alignment;
};

template <typename T>
constexpr auto storageInfoFor = StorageInfo{sizeof(T), alignof(T)};

} // namespace caramel_poly

#endif /* CARAMELPOLY_BUILTIN_HPP__ */
