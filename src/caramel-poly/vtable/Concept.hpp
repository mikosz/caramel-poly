#ifndef CARAMELPOLY_VTABLE_CONCEPT_HPP__
#define CARAMELPOLY_VTABLE_CONCEPT_HPP__

#include <type_traits>

namespace caramel_poly::vtable {

template <class NameString, class SignatureType>
struct ConceptEntry {
	using Name = NameString;
	using Signature = SignatureType;
};

template <class NameString, class SignatureType>
constexpr auto makeConceptEntry([[maybe_unused]] NameString nameString, [[maybe_unused]] SignatureType signature) {
	static_assert(std::is_empty_v<SignatureType>, "Concept function signature must be stateless");
	return ConceptEntry<NameString, SignatureType>{};
}

template <class... Entries>
struct Concept;

template <class HeadNameString, class HeadSignature, class... TailEntries>
struct Concept<ConceptEntry<HeadNameString, HeadSignature>, TailEntries...> : Concept<TailEntries...> {
//private:
//
//	template <class NameString>
//	struct SignatureHelper {
//		using Signature = typename Concept<TailEntries...>::template Signature<NameString>;
//	};
//
//	template <>
//	struct SignatureHelper<HeadNameString> {
//		using Signature = HeadSignature;
//	};
//
//public:
//
//	template <class NameString>
//	using Signature = typename SignatureHelper<NameString>::Signature;

	template <class NameString>
	constexpr auto methodSignature(NameString nameString) const {
		if constexpr (nameString == HeadNameString{}) {
			return HeadSignature{};
		} else {
			return Concept<TailEntries...>::methodSignature(nameString);
		}
	}

};

template <>
struct Concept<> {

	template <class NameString>
	constexpr auto methodSignature([[maybe_unused]] NameString nameString) const {
		static_assert(false, "Method not declared in concept");
	}

};

template <class... Entries>
constexpr auto makeConcept(Entries... /* entries */) {
	return Concept<Entries...>{};
}

} // namespace caramel_poly::vtable

#endif /* CARAMELPOLY_VTABLE_CONCEPT_HPP__ */
