#ifndef CARAMELPOLY_VTABLE_CONTAINER_HPP__
#define CARAMELPOLY_VTABLE_CONTAINER_HPP__

#include <stdexcept>

namespace caramel_poly::vtable {

namespace detail {

template <class Head, class... Tail>
class Container : Container<Tail...> {
public:

	constexpr Container(Head element, Tail... tail) :
		Container<Tail...>(tail...),
		element_(element)
	{
	}

	template <class Key>
	constexpr auto operator[](Key key) const {
		if (MapEntryKey<Head>()(element_) == key) {
			return element_;
		} else {
			return Container<Tail...>::operator[](key);
		}
	}

private:

	const Head element_;

};

template <class Head>
class Container<Head> {
public:

	constexpr Container(Head element) :
		element_(std::move(element))
	{
	}

	template <class Key>
	constexpr auto operator[](Key key) const {
		if (MapEntryKey<Head>()(element_) == key) {
			return element_;
		} else {
			throw std::out_of_range("Key not found");
		}
	}

private:

	const Head element_;

};

} // namespace detail

template <class... Entries>
class Map {
public:

	constexpr Map(Entries... entries) :
		elements_(entries...)
	{
	}

	template <class Key>
	constexpr auto operator[](Key key) const {
		return elements_[key];
	}

private:

	detail::Container<Entries...> elements_;

};

template <class... Entries>
constexpr auto makeMap(Entries... entries) {
	return Map<Entries...>(entries...);
}

template <class Entry>
struct MapEntryKey;

} // namespace caramel_poly::vtable

#endif /* CARAMELPOLY_VTABLE_CONTAINER_HPP__ */
