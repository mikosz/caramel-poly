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

template <class SelfType, class HeadNameString, class HeadLambda, class... TailEntries>
class ConceptMap<SelfType, ConceptMapEntry<HeadNameString, HeadLambda>, TailEntries...> :
	ConceptMap<SelfType, TailEntries...>
{
public:

	using Self = SelfType;

	template <class NameString>
	struct LambdaType {
		using Type = typename ConceptMap<SelfType, TailEntries...>::template LambdaType<NameString>::Type;
	};

	template <>
	struct LambdaType<HeadNameString> {
		using Type = ConceptMapEntry<HeadNameString, HeadLambda>;
	};

};

template <class SelfType>
class ConceptMap<SelfType> {
public:

	using Self = SelfType;

	struct w00t {
		using Lambda = void;
	};

	template <class NameString>
	struct LambdaType {
		using Type = w00t;
	};

};

template <class SelfType, class... Entries>
constexpr auto makeConceptMap(Entries... /*entries*/) {
	return ConceptMap<SelfType, Entries...>{};
}

} // namespace caramel_poly::vtable

#endif /* CARAMELPOLY_VTABLE_CONCEPTMAP_HPP__ */
