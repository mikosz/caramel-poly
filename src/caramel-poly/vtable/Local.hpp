#ifndef CARAMELPOLY_VTABLE_LOCAL_HPP__
#define CARAMELPOLY_VTABLE_LOCAL_HPP__

#include "caramel-poly/detail/Method.hpp"
#include "Concept.hpp"
#include "ConceptMap.hpp"

namespace caramel_poly::vtable {

namespace detail {

template <class SelfType, class ConceptType>
struct Methods;

template <class SelfType, class HeadNameString, class HeadSignature, class... TailEntries>
struct Methods<
		SelfType,
		Concept<
			ConceptEntry<HeadNameString, HeadSignature>,
			TailEntries...
			>
		> /*: Methods<SelfType, Concept<TailEntries...>>*/ {
public:

	template <class ConceptMap>
	constexpr Methods(ConceptMap conceptMap) :
		//Parent(conceptMap),
		method_(conceptMap.get(HeadNameString{}))
	{
	}

	template <class NameString>
	constexpr auto operator[]([[maybe_unused]] NameString name) const {
		if constexpr (NameString{} == HeadNameString{}) {
			return method_;
		} else {
			return Parent::operator[](name);
		}
	}

private:

	//using Parent = Methods<SelfType, Concept<TailEntries...>>;

	using Concept = Concept<ConceptEntry<HeadNameString, HeadSignature>, TailEntries...>;

	using MappingSignature = typename decltype(Concept{}.methodSignature(HeadNameString{}))::MappingSignature;

	caramel_poly::detail::Method<SelfType, MappingSignature> method_;

};

template <class SelfType>
struct Methods<SelfType, Concept<>> {

	template <class ConceptMap>
	constexpr Methods([[maybe_unused]] ConceptMap conceptMap)
	{
	}

	template <class NameString>
	constexpr auto operator[]([[maybe_unused]] NameString name) const {
		static_assert(false, "Undefined method");
	}

};

} // namespace detail

template <class SelfType, class ConceptType>
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

	vtable::detail::Methods<SelfType, ConceptType> methods_;

};

template <class SelfType, class Concept, class ConceptMap>
constexpr auto makeLocal([[maybe_unused]] Concept concept, ConceptMap conceptMap) {
	return Local<SelfType, Concept>{ conceptMap };
}

} // namespace caramel_poly::vtable

#endif /* CARAMELPOLY_VTABLE_LOCAL_HPP__ */
