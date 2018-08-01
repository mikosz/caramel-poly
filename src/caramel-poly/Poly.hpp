#ifndef CARAMELPOLY_POLY_HPP__
#define CARAMELPOLY_POLY_HPP__

#include "storage/SharedRemote.hpp"
#include "vtable/Local.hpp"
#include "vtable/ConceptMap.hpp"

namespace caramel_poly {

template <
	class Concept,
	class Storage = storage::SharedRemote,
	class VTable = vtable::Local<Concept>
	>
class Poly {
public:
	
	template <class T>
	Poly(T&& object) :
		storage_(std::forward<T>(object)),
		vtable_(conceptMap<Concept, std::decay_t<object>>)
	{
	}

	template <class MethodNameT, class... Args>
	decltype(auto) invoke(Args&&... args) {

	}

private:

	Storage storage_;

	VTable vtable_;

};

} // namespace caramel_poly

#endif /* CARAMELPOLY_POLY_HPP__ */
