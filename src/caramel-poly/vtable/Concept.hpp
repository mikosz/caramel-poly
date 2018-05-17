#ifndef CARAMELPOLY_VTABLE_CONCEPT_HPP__
#define CARAMELPOLY_VTABLE_CONCEPT_HPP__

#include <stdexcept>

namespace caramel_poly::vtable {

template <class NameString, class SignatureType>
struct ConceptEntry {
	using Name = NameString;
	using Signature = SignatureType;
};

template <class... Entries>
class Concept;

template <class HeadNameString, class HeadSignature, class... TailEntries>
class Concept<ConceptEntry<HeadNameString, HeadSignature>, TailEntries...> : Concept<TailEntries...> {
private:

	template <class NameString>
	struct SignatureHelper {
		using Signature = typename Concept<TailEntries...>::Signature;
	};

	template <>
	struct SignatureHelper<HeadNameString> {
		using Signature = HeadSignature;
	};

public:

	template <class NameString>
	using Signature = typename SignatureHelper<NameString>::Signature;

};

template <>
class Concept<> {
};

} // namespace caramel_poly::vtable

#endif /* CARAMELPOLY_VTABLE_CONCEPT_HPP__ */
