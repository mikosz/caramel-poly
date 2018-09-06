// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)
//
// This is Louis Dionne's Dyno library adapted to work on Visual Studio 2017 and
// without Boost, modified to my taste. All credit for the design and delivery goes
// to Louis. His original implementation may be found here:
// https://github.com/ldionne/dyno

#ifndef CARAMELPOLY_POLY_HPP__
#define CARAMELPOLY_POLY_HPP__

#include <type_traits>
#include <utility>

#include "builtin.hpp"
#include "Concept.hpp"
#include "ConceptMap.hpp"
#include "storage.hpp"
#include "vtable.hpp"

namespace caramel_poly {

// A `caramel_poly::Poly` encapsulates an object of a polymorphic type that supports the
// interface of the given `Concept`.
//
// `caramel_poly::Poly` is meant to be used as a holder for a polymorphic object. It can
// manage the lifetime of that object and provide access to its dynamically-
// dispatched methods. However, it does not directly implement any specific
// interface beyond what's strictly necessary for managing the lifetime
// (constructor, destructor, assignment, swap, etc..). Instead, it provides
// a `virtual_` method that gives access to dynamically-dispatched methods of
// the object it manages.
//
// The intended use case is for users to create their very own type-erased
// wrappers on top of `caramel_poly::Poly`, defining their interface as they wish and
// using the dynamic dispatch provided by the library to implement runtime
// polymorphism.
//
// Different aspects of a `caramel_poly::Poly` can also be customized:
//  `Concept`
//    The concept satisfied by `caramel_poly::Poly`. This determines which methods will
//    be available for dynamic dispatching.
//
//  `Storage`
//    The type used to provide the storage for the managed object. This must
//    be a model of the `PolymorphicStorage` concept.
//
//  `VTable`
//    The policy specifying how to implement the dynamic dispatching mechanism
//    for methods. This must be a specialization of `caramel_poly::vtable`.
//    See `caramel_poly::vtable` for details.
//
// TODO:
// - How to combine the storage of the object with that of the vtable?
//   For example, how would we allow storing the vtable inside the rest
//   of the storage?
// - Is it actually OK to require Destructible and Storable all the time?
// - Test that we can't call e.g. a non-const method on a const poly.
template <
	class Concept,
	class Storage = caramel_poly::RemoteStorage,
	class VTablePolicy = caramel_poly::VTable<caramel_poly::Remote<caramel_poly::Everything>>
	>
struct Poly {
public:

	template <class T, class RawT = std::decay_t<T>, class ConceptMap>
	Poly(T&& t, ConceptMap map) :
		vtable_{caramel_poly::completeConceptMap<ActualConcept, RawT>(map)},
		storage_{std::forward<T>(t)}
	{
	}

	template <
		class T,
		class RawT = std::decay_t<T>,
		class = std::enable_if_t<!std::is_same<RawT, Poly>::value>,
		class = std::enable_if_t<caramel_poly::models<ActualConcept, RawT>>
		>
	Poly(T&& t) :
		Poly{std::forward<T>(t), caramel_poly::conceptMap<ActualConcept, RawT>}
	{
	}

	~Poly() {
		storage_.destruct(vtable_);
	}

	Poly(const Poly& other) :
		vtable_{other.vtable_},
		storage_{other.storage_, vtable_}
	{
	}

	Poly(Poly&& other) :
		vtable_{std::move(other.vtable_)},
		storage_{std::move(other.storage_), vtable_}
	{
	}

	Poly& operator=(const Poly& other) {
		Poly(other).swap(*this);
		return *this;
	}

	Poly& operator=(Poly&& other) {
		Poly(std::move(other)).swap(*this);
		return *this;
	}

	void swap(Poly& other) {
		storage_.swap(vtable_, other.storage_, other.vtable_);
		using std::swap;
		swap(vtable_, other.vtable_);
	}

	friend void swap(Poly& a, Poly& b) {
		a.swap(b);
	}

	template <class... T, class Name, class... Args>
	decltype(auto) operator->*(caramel_poly::detail::delayedCall<Name, Args...>&& delayed) {
		auto f = virtual_(Name{});
		auto injected = [f,this](auto&&... args) -> decltype(auto) {
			return f(*this, static_cast<decltype(args)&&>(args)...);
		};
		return unpack(std::move(delayed.args), injected);
	}

	template <
		class Function,
		bool HasClause = decltype(contains(caramel_poly::clauseNames(Concept{}), Function{})){},
		std::enable_if_t<HasClause>* = nullptr
		>
	constexpr decltype(auto) invoke(Function name) const & {
		auto clauses = caramel_poly::detail::makeConstexprMap(caramel_poly::clauses(Concept{}));
		return virtualImpl(clauses[name], name);
	}

	template <
		class Function,
		bool HasClause = decltype(contains(caramel_poly::clauseNames(Concept{}), Function{})){},
		std::enable_if_t<HasClause>* = nullptr
		>
	constexpr decltype(auto) invoke(Function name) & {
		auto clauses = caramel_poly::detail::makeConstexprMap(caramel_poly::clauses(Concept{}));
		return virtualImpl(clauses[name], name);
	}

	template <
		class Function,
		bool HasClause = decltype(contains(caramel_poly::clauseNames(Concept{}), Function{})){},
		std::enable_if_t<HasClause>* = nullptr
		>
	constexpr decltype(auto) invoke(Function name) && {
		auto clauses = caramel_poly::detail::makeConstexprMap(caramel_poly::clauses(Concept{}));
		return virtualImpl(clauses[name], name);
	}

	template <
		class Function,
		bool HasClause = decltype(contains(caramel_poly::clauseNames(Concept{}), Function{})){},
		std::enable_if_t<HasClause>* = nullptr
		>
	constexpr decltype(auto) invoke(Function name) const {
		auto clauses = caramel_poly::detail::makeConstexprMap(caramel_poly::clauses(Concept{}));
		return virtualImpl(clauses[name], name);
	}

	// Returns a pointer to the underlying storage.
	//
	// The pointer is potentially invalidated whenever the poly is modified;
	// the specific storage policy should be consulted to know when pointers
	// to the underlying storage are invalidated.
	//
	// The behavior is undefined if the requested type is not cv-qualified `void`
	// and the underlying storage is not of the requested type.
	template <class T>
	T* unsafeGet() {
		return storage_.template get<T>();
	}

	template <class T>
	const T* unsafeGet() const {
		return storage_.template get<T>();
	}

private:

	using ActualConcept = decltype(caramel_poly::requires(
		Concept{},
		caramel_poly::Destructible{},
		caramel_poly::Storable{}
		));

	using VTable = class VTablePolicy::template Type<ActualConcept>;

	VTable vtable_;

	Storage storage_;

	// Handle caramel_poly::function
	template <class R, class... T, class Function>
	constexpr decltype(auto) virtual_impl(caramel_poly::function_t<R(T...)>, Function name) const {
		auto fptr = vtable_[name];
		return [fptr](auto&&... args) -> decltype(auto) {
			return fptr(Poly::unerase_poly<T>(static_cast<decltype(args)&&>(args))...);
		};
	}

	// Handle caramel_poly::method
	template <class R, class... T, class Function>
	constexpr decltype(auto) virtual_impl(caramel_poly::method_t<R(T...)>, Function name) & {
		auto fptr = vtable_[name];
		return [fptr, this](auto&&... args) -> decltype(auto) {
			return fptr(Poly::unerase_poly<caramel_poly::T&>(*this),
				Poly::unerase_poly<T>(static_cast<decltype(args)&&>(args))...);
		};
	}
	template <class R, class... T, class Function>
	constexpr decltype(auto) virtual_impl(caramel_poly::method_t<R(T...)&>, Function name) & {
		auto fptr = vtable_[name];
		return [fptr, this](auto&&... args) -> decltype(auto) {
			return fptr(Poly::unerase_poly<caramel_poly::T&>(*this),
				Poly::unerase_poly<T>(static_cast<decltype(args)&&>(args))...);
		};
	}
	template <class R, class... T, class Function>
	constexpr decltype(auto) virtual_impl(caramel_poly::method_t<R(T...)&&>, Function name) && {
		auto fptr = vtable_[name];
		return [fptr, this](auto&&... args) -> decltype(auto) {
			return fptr(Poly::unerase_poly<caramel_poly::T&&>(*this),
				Poly::unerase_poly<T>(static_cast<decltype(args)&&>(args))...);
		};
	}
	template <class R, class... T, class Function>
	constexpr decltype(auto) virtual_impl(caramel_poly::method_t<R(T...) const>, Function name) const {
		auto fptr = vtable_[name];
		return [fptr, this](auto&&... args) -> decltype(auto) {
			return fptr(Poly::unerase_poly<caramel_poly::T const&>(*this),
				Poly::unerase_poly<T>(static_cast<decltype(args)&&>(args))...);
		};
	}
	template <class R, class... T, class Function>
	constexpr decltype(auto) virtual_impl(caramel_poly::method_t<R(T...) const&>, Function name) const {
		auto fptr = vtable_[name];
		return [fptr, this](auto&&... args) -> decltype(auto) {
			return fptr(Poly::unerase_poly<caramel_poly::T const&>(*this),
				Poly::unerase_poly<T>(static_cast<decltype(args)&&>(args))...);
		};
	}

	// unerase_poly helper
	template <class T, class Arg, std::enable_if_t<!detail::is_placeholder<T>::value, int> = 0>
	static constexpr decltype(auto) unerase_poly(Arg&& arg)
	{ return static_cast<Arg&&>(arg); }

	template <class T, class Arg, std::enable_if_t<detail::is_placeholder<T>::value, int> = 0>
	static constexpr decltype(auto) unerase_poly(Arg&& arg) {
		using RawArg = std::remove_cv_t<std::remove_reference_t<Arg>>;
		constexpr bool is_poly = std::is_same<Poly, RawArg>::value;
		static_assert(is_poly,
			"caramel_poly::Poly::virtual_: Passing a non-poly object as an argument to a virtual "
			"function that specified a placeholder for that parameter.");
		return static_cast<Arg&&>(arg).storage_.get();
	}
	template <class T, class Arg, std::enable_if_t<detail::is_placeholder<T>::value, int> = 0>
	static constexpr decltype(auto) unerase_poly(Arg* arg) {
		using RawArg = std::remove_cv_t<Arg>;
		constexpr bool is_poly = std::is_same<Poly, RawArg>::value;
		static_assert(is_poly,
			"caramel_poly::Poly::virtual_: Passing a non-poly object as an argument to a virtual "
			"function that specified a placeholder for that parameter.");
		return arg->storage_.get();
	}
};

} // end namespace caramel_poly

#endif /* CARAMELPOLY_POLY_HPP__ */
