#ifndef CARAMELPOLY_VTABLE_LOCAL_HPP__
#define CARAMELPOLY_VTABLE_LOCAL_HPP__

#include "caramel-poly/detail/Method.hpp"
#include "Concept.hpp"

namespace caramel_poly::vtable {

namespace detail {

template <class Concept>
struct Methods;

template <class HeadNameString, class HeadSignature, class... TailEntries>
struct Methods<Concept<HeadNameString, HeadSignature, TailEntries...>> : Methods<Concept<TailEntries...>> {
public:

	template <ConceptMap>
	Methods(ConceptMap conceptMap) :
		Methods<Concept<TailEntries...>>(conceptMap),
		method_(conceptMap.get(HeadNameString{}))
	{
	}

	template <class NameString>
	constexpr auto operator[]([[maybe_unused]] NameString name) const {
		if constexpr (NameString{} == HeadNameString{}) {
			return method_;
		} else {
			return Methods<Concept<TailEntries...>>::operator[](name);
		}
	}

private:

	using Concept = Concept<HeadNameString, HeadSignature, TailEntries...>;

	using MappingSignature = decltype(
		typename Concept::template methodSignature<HeadNameString>::MappingSignature
		);

	Method<MappingSignature> method_;

};

} // namespace detail

template <class Concept>
class Local {
public:

	template <class ConceptMap>
	constexpr Local(ConceptMap conceptMap) :
		methods_(conceptMap)
	{
	}

	template <class ReturnType, class NameString, class... Args>
	ReturnType invoke([[maybe_unused]] NameString name, Args&&... args) const {
		constexpr auto method = methods_[NameString{}];
		return method.invoke<ReturnType>(std::forward<Args>(args)...);
	}

private:

	detail::Methods<Concept> methods_;

};

} // namespace caramel_poly::vtable

#endif /* CARAMELPOLY_VTABLE_LOCAL_HPP__ */
