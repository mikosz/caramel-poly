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
		vtable_(vtable::conceptMap<Concept, std::decay_t<T>>)
	{
	}

	template <class MethodNameT, class... Args>
	decltype(auto) invoke(MethodNameT, Args&&... args) {
		// TODO: TEMP, do it like dyno does, probably
		auto& self = *static_cast<vtable::Object*>(storage_.get());

		return vtable_.invoke(MethodNameT{}, self, std::forward<Args>(args)...);
	}

	template <class MethodNameT, class... Args>
	decltype(auto) invoke(MethodNameT, Args&&... args) const {
		// TODO: TEMP, do it like dyno does, probably
		const auto& self = *static_cast<const vtable::Object*>(storage_.get());

		return vtable_.invoke(MethodNameT{}, self, std::forward<Args>(args)...);
	}

private:

	Storage storage_;

	VTable vtable_;

};

} // namespace caramel_poly

#endif /* CARAMELPOLY_POLY_HPP__ */
