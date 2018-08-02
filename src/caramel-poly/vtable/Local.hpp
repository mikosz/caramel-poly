#ifndef CARAMELPOLY_VTABLE_LOCAL_HPP__
#define CARAMELPOLY_VTABLE_LOCAL_HPP__

#include "caramel-poly/detail/Method.hpp"
#include "caramel-poly/detail/BindSignature.hpp"
#include "Concept.hpp"
#include "ConceptMap.hpp"

namespace caramel_poly::vtable {

namespace detail {

template <class ConceptType>
struct Methods;

template <class HeadNameString, class HeadSignature, class... TailEntries>
struct Methods<
		Concept<
			ConceptEntry<HeadNameString, HeadSignature>,
			TailEntries...
			>
		> : Methods<Concept<TailEntries...>> {
public:

	using ConceptType = Concept<ConceptEntry<HeadNameString, HeadSignature>, TailEntries...>;
	typename decltype(ConceptType{}.methodSignature(HeadNameString{}))::MappingSignature d = 3;

	template <class ConceptMap>
	constexpr explicit Methods(ConceptMap conceptMap) :
		Parent(conceptMap),
		method_(
			DefaultConstructibleLambda<
				ConceptMap::LambdaType<HeadNameString>,
				typename caramel_poly::detail::BindSignature<
					typename decltype(ConceptType{}.methodSignature(HeadNameString{}))::MappingSignature,
					typename ConceptMap::Self
					>::Type
				> {}
			)
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

	using Parent = Methods<Concept<TailEntries...>>;

	//using ConceptType = Concept<ConceptEntry<HeadNameString, HeadSignature>, TailEntries...>;

	using MappingSignature =
		typename decltype(ConceptType{}.methodSignature(HeadNameString{}))::MappingSignature;

	caramel_poly::detail::Method<MappingSignature> method_;

};

template <>
struct Methods<Concept<>> {

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

template <class ConceptType>
class Local {
public:

	template <class ConceptMap>
	constexpr Local(ConceptMap conceptMap) :
		methods_(conceptMap)
	{
	}

	template <class NameString, class... Args>
	decltype(auto) invoke([[maybe_unused]] NameString name, Args&&... args) const {
		/*constexpr */ const auto method = methods_[NameString{}];
		return method.invoke(std::forward<Args>(args)...);
	}

private:

	vtable::detail::Methods<typename ConceptType::ConceptType> methods_;

};

template <class Concept, class ConceptMap>
constexpr auto makeLocal([[maybe_unused]] Concept concept, ConceptMap conceptMap) {
	return Local<Concept>{ conceptMap };
}

} // namespace caramel_poly::vtable

#endif /* CARAMELPOLY_VTABLE_LOCAL_HPP__ */
