#ifndef CARAMELPOLY_VTABLE_CONCEPTMAP_HPP__
#define CARAMELPOLY_VTABLE_CONCEPTMAP_HPP__

#include <stdexcept>

namespace caramel_poly::vtable {

template <class NameString, class LambdaType>
struct ConceptMapEntry {
	using Name = NameString;
	using Lambda = LambdaType;
};

template <class NameString, class LambdaType>
constexpr auto makeConceptMapEntry([[maybe_unused]] NameString name, [[maybe_unused]] LambdaType lambda) {
	return ConceptMapEntry<NameString, LambdaType>{};
}

template <class SelfType, class... Entries>
class ConceptMap;

template <class SelfType, class HeadNameStringT, class HeadLambda, class... TailEntries>
class ConceptMap<SelfType, ConceptMapEntry<HeadNameStringT, HeadLambda>, TailEntries...> :
	ConceptMap<SelfType, TailEntries...>
{
private:

	template <class NameStringT>
	struct LambdaTypeInternal {
		using Type = typename ConceptMap<SelfType, TailEntries...>::template LambdaType<NameStringT>;
	};

	template <>
	struct LambdaTypeInternal<HeadNameStringT> {
		using Type = ConceptMapEntry<HeadNameStringT, HeadLambda>;
	};

public:

	using Self = SelfType;

	template <class NameStringT>
	using LambdaType = typename LambdaTypeInternal<std::remove_cv_t<NameStringT>>::Type::Lambda;

};

template <class SelfType>
class ConceptMap<SelfType> {
public:

	using Self = SelfType;

	template <class NameString>
	struct LambdaType {
	};

};

template <class SelfType, class... Entries>
constexpr auto makeConceptMap(Entries... /*entries*/) {
	return ConceptMap<SelfType, Entries...>{};
}

} // namespace caramel_poly::vtable

#endif /* CARAMELPOLY_VTABLE_CONCEPTMAP_HPP__ */
