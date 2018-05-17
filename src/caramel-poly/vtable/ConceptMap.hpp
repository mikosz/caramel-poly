#ifndef CARAMELPOLY_VTABLE_CONCEPTMAP_HPP__
#define CARAMELPOLY_VTABLE_CONCEPTMAP_HPP__

#include <stdexcept>

namespace caramel_poly::vtable {

template <class NameString, class MethodType>
struct ConceptMapEntry {
	using Name = NameString;
	using Method = MethodType;
};

template <class NameString, class MethodType>
constexpr auto makeConceptMapEntry([[maybe_unused]] NameString name, [[maybe_unused]] MethodType method) {
	return ConceptMapEntry<NameString, MethodType>{};
}

template <class... Entries>
class ConceptMap;

template <class HeadNameString, class HeadMethod, class... TailEntries>
class ConceptMap<ConceptMapEntry<HeadNameString, HeadMethod>, TailEntries...> : ConceptMap<TailEntries...> {
public:

	template <class NameString>
	constexpr auto get([[maybe_unused]] NameString name) const {
		if constexpr (NameString{} == HeadNameString{}) {
			return HeadMethod{};
		} else {
			return ConceptMap<TailEntries...>::get(name);
		}
	}

};

template <>
class ConceptMap<> {
public:

	template <class NameString>
	constexpr auto get([[maybe_unused]] NameString name) const {
		static_assert(false, "Provided function name not registered in concept map");
	}

};

template <class... Entries>
constexpr auto makeConceptMap(Entries... /*entries*/) {
	return ConceptMap<Entries...>{};
}

} // namespace caramel_poly::vtable

#endif /* CARAMELPOLY_VTABLE_CONCEPTMAP_HPP__ */
