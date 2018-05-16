#ifndef CARAMELPOLY_VTABLE_CONCEPTMAP_HPP__
#define CARAMELPOLY_VTABLE_CONCEPTMAP_HPP__

#include <stdexcept>

namespace caramel_poly::vtable {

template <int ID_PARAM, class MethodType>
struct ConceptMapEntry {
	static constexpr auto ID = ID_PARAM;
	using Method = MethodType;
};

template <class... Entries>
class ConceptMap;

template <int ID, class Head, class... Tail>
class ConceptMap<ConceptMapEntry<ID, Head>, Tail...> : ConceptMap<Tail...> {
public:

	template <class... TailMethods>
	constexpr ConceptMap(Head* head, TailMethods... tail) :
		ConceptMap<Tail...>(tail...),
		method_(head)
	{
	}

	template <int GET_ID>
	constexpr auto get() const {
		if constexpr (GET_ID == ID) {
			return method_;
		} else {
			return ConceptMap<Tail...>::get<GET_ID>();
		}
	}

private:

	Head* method_;

};

template <>
class ConceptMap<> {
public:

	template <int GET_ID>
	constexpr auto get() const {
		throw std::out_of_range("No such function");
	}

};

template <class... Methods>
constexpr auto makeMethodMap(Methods... methods) {
	return ConceptMap<Methods...>(methods...);
}

} // namespace caramel_poly::vtable

#endif /* CARAMELPOLY_VTABLE_CONCEPTMAP_HPP__ */
