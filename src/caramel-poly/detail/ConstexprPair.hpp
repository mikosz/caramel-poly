// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)

#ifndef CARAMELPOLY_DETAIL_CONSTEXPRPAIR_HPP__
#define CARAMELPOLY_DETAIL_CONSTEXPRPAIR_HPP__

#include <type_traits>

namespace caramel_poly::detail {

template <class FirstT, class SecondT>
class ConstexprPair {
public:

	using First = FirstT;
	using Second = SecondT;

	constexpr ConstexprPair(First, Second) {
	}

	constexpr ConstexprPair() = default;

	constexpr First first() const {
		return First{};
	}

	constexpr Second second() const {
		return Second{};
	}

};

template <class First, class Second>
constexpr auto makeConstexprPair(First, Second) {
	return ConstexprPair<First, Second>{};
}

constexpr auto first = [](auto p) { return p.first(); };
constexpr auto second = [](auto p) { return p.second(); };

} // namespace caramel_poly::detail

#endif /* CARAMELPOLY_DETAIL_CONSTEXPRPAIR_HPP__ */
