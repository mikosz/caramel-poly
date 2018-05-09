#ifndef CARAMELPOLY_VTABLE_CONTAINER_HPP__
#define CARAMELPOLY_VTABLE_CONTAINER_HPP__

#include <stdexcept>

namespace caramel_poly::vtable {

namespace detail {

template <class Head, class... Tail>
class Container : Container<Tail...> {
public:

	template <class Key, Key key>
	constexpr auto get() const {
		if constexpr (MapEntryKey<Head>{}() == key) {
			return Head{};
		} else {
			return Container<Tail...>::get<Key, key>();
		}
	}

};

template <class Head>
class Container<Head> {
public:

	template <class Key, Key key>
	constexpr auto get() const {
		if constexpr (MapEntryKey<Head>{}() == key) {
			return Head{};
		} else {
			throw std::out_of_range("Key not found");
		}
	}

};

} // namespace detail

template <class... Entries>
class Map {
public:

	template <class Key, Key key>
	constexpr auto get() const {
		return elements_.get<Key, key>();
	}

private:

	detail::Container<Entries...> elements_;

};

template <class... Entries>
constexpr auto makeMap(Entries...) {
	return Map<Entries...>();
}

template <class Entry>
struct MapEntryKey;

} // namespace caramel_poly::vtable

#endif /* CARAMELPOLY_VTABLE_CONTAINER_HPP__ */
