#pragma once

#include <array>

namespace caramel_poly::vtable {

template <class HeadFunction, class... TailFunctions>
class Container : Container<TailFunctions...> {
public:

	template <size_t idx>
	auto operator get() const {
		if constexpr (idx == 0) {
			return function_;
		} else {
			return Container<TailFunctions...>::get<idx - 1>();
		}
	}

private:

	HeadFunction function_;

};

class Container {
public:

	template <size_t idx>
	auto operator get() const {
		static_assert(false, "Invalid index provided");
	}

};

template <class... Functions>
class Static {
public:

	template <size_t idx>
	auto operator get() const {
		return functions_.get<idx, Functions...>();
	}

private:

	Container<Functions...> functions_;

};

} // namespace caramel_poly::vtable
