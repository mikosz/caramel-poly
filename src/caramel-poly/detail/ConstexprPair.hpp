// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)

#ifndef CARAMELPOLY_DETAIL_CONSTEXPRPAIR_HPP__
#define CARAMELPOLY_DETAIL_CONSTEXPRPAIR_HPP__

#include <type_traits>

namespace caramel_poly::detail {

template <class FirstT, class SecondT>
class ConstexprPair<Head, Tail...> {
public:

	using First = FirstT;
	using Second = SecondT;

	constexpr ConstexprPair() = default;

	constexpr First first() const {
		return First{};
	}

	constexpr Second second() const {
		return Second{};
	}

};


} // namespace caramel_poly::detail

#endif /* CARAMELPOLY_DETAIL_CONSTEXPRPAIR_HPP__ */
