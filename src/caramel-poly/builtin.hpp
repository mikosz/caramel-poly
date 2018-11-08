// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)
//
// This is Louis Dionne's Dyno library adapted to work on Visual Studio 2017 and
// without Boost, modified to my taste. All credit for the design and delivery goes
// to Louis. His original implementation may be found here:
// https://github.com/ldionne/dyno

#ifndef CARAMELPOLY_BUILTIN_HPP__
#define CARAMELPOLY_BUILTIN_HPP__

#include <cstddef>
#include <type_traits>
#include <typeinfo>

#include "detail/ConstexprString.hpp"
#include "ConceptMap.hpp"
#include "Concept.hpp"

namespace caramel::poly {

constexpr auto STORAGE_INFO_LABEL = POLY_FUNCTION_LABEL("storage-info");
constexpr auto TYPEID_LABEL = POLY_FUNCTION_LABEL("typeid");
constexpr auto DESTRUCT_LABEL = POLY_FUNCTION_LABEL("destruct");
constexpr auto COPY_CONSTRUCT_LABEL = POLY_FUNCTION_LABEL("copy-construct");
constexpr auto MOVE_CONSTRUCT_LABEL = POLY_FUNCTION_LABEL("move-construct");
constexpr auto EQUAL_LABEL = POLY_FUNCTION_LABEL("equal");

// Encapsulates the minimal amount of information required to allocate
// storage for an object of a given type.
//
// This should never be created explicitly; always use `caramel::poly::storageInfoFor`.
struct StorageInfo {
	std::size_t size;
	std::size_t alignment;

	constexpr StorageInfo(std::size_t s, std::size_t a) :
		size(s),
		alignment(a)
	{
	}
};

template <typename T>
constexpr auto storageInfoFor = StorageInfo{ sizeof(T), alignof(T) };

struct Storable : decltype(caramel::poly::requires(
	STORAGE_INFO_LABEL = caramel::poly::function<caramel::poly::StorageInfo()>
	))
{
};

template <typename T>
auto const defaultConceptMap<Storable, T> = caramel::poly::makeConceptMap(
	// Can't use storageInfoFor here, for Visual C++ it returns { 0, 0 }
	STORAGE_INFO_LABEL = []() { return caramel::poly::StorageInfo{ sizeof(T), alignof(T) }; }
	);


struct TypeId : decltype(caramel::poly::requires(
	TYPEID_LABEL = caramel::poly::function<const std::type_info&()>
	))
{
};

template <typename T>
auto const defaultConceptMap<TypeId, T> = caramel::poly::makeConceptMap(
	TYPEID_LABEL = []() -> const std::type_info& { return typeid(T); }
	);


struct DefaultConstructible : decltype(caramel::poly::requires(
	TYPEID_LABEL = caramel::poly::function<void (void*)>
	))
{
};

template <typename T>
auto const defaultConceptMap<DefaultConstructible, T, std::enable_if_t<std::is_default_constructible<T>::value>
	> = caramel::poly::makeConceptMap(
		TYPEID_LABEL = [](void* p) { new (p) T(); }
		);

struct Destructible : decltype(caramel::poly::requires(
	DESTRUCT_LABEL = caramel::poly::function<void (caramel::poly::SelfPlaceholder&)>
	))
{
};

template <typename T>
auto const defaultConceptMap<Destructible, T, std::enable_if_t<std::is_destructible<T>::value>
	> = caramel::poly::makeConceptMap(
		DESTRUCT_LABEL = [](T& self) { self.~T(); }
		);

struct MoveConstructible : decltype(caramel::poly::requires(
	MOVE_CONSTRUCT_LABEL = caramel::poly::function<void (void*, caramel::poly::SelfPlaceholder&&)>
	))
{
};

template <typename T>
auto const defaultConceptMap<MoveConstructible, T, std::enable_if_t<std::is_move_constructible<T>::value>
	> = caramel::poly::makeConceptMap(
		MOVE_CONSTRUCT_LABEL = [](void* p, T&& other) { new (p) T(std::move(other)); }
		);


struct CopyConstructible : decltype(caramel::poly::requires(
	caramel::poly::Storable{},
	//caramel::poly::MoveConstructible{},
	COPY_CONSTRUCT_LABEL = caramel::poly::function<void (void*, const caramel::poly::SelfPlaceholder&)>
	))
{
};

template <typename T>
auto const defaultConceptMap<CopyConstructible, T, std::enable_if_t<std::is_copy_constructible<T>::value>
	> = caramel::poly::makeConceptMap(
		COPY_CONSTRUCT_LABEL = [](void* p, T const& other) { new (p) T(other); }
		);


struct MoveAssignable : decltype(caramel::poly::requires(
	// No virtual function required to support this so far
	))
{
};


struct CopyAssignable : decltype(caramel::poly::requires(
	caramel::poly::MoveAssignable{}
	// No additional virtual functions required to support this so far
	))
{
};


struct Swappable : decltype(caramel::poly::requires(
	// No virtual functions required to support this so far
	))
{
};


struct EqualityComparable : decltype(caramel::poly::requires(
	EQUAL_LABEL = caramel::poly::function<
		bool (const caramel::poly::SelfPlaceholder&, const caramel::poly::SelfPlaceholder&)>
	))
{
};

template <typename T>
auto const defaultConceptMap<EqualityComparable, T, decltype((void)(std::declval<T>() == std::declval<T>()))
	> = caramel::poly::makeConceptMap(
		EQUAL_LABEL = [](T const& a, T const& b) -> bool { return a == b; }
		);


} // namespace caramel::poly

#endif /* CARAMELPOLY_BUILTIN_HPP__ */
