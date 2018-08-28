// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)

#ifndef CARAMELPOLY_DETAIL_CONSTEXPRPAIR_HPP__
#define CARAMELPOLY_DETAIL_CONSTEXPRPAIR_HPP__

#include <type_traits>

namespace caramel_poly::detail {

template <class FirstT, class SecondT, class = void>
class ConstexprPairStorage;

template <class FirstT, class SecondT>
class ConstexprPairStorage<FirstT, SecondT, std::enable_if_t<std::is_empty_v<FirstT> && std::is_empty_v<SecondT>>> {
public:

	constexpr ConstexprPairStorage() = default;

	constexpr ConstexprPairStorage(First, Second)
	{
	}

	constexpr FirstT first() const {
		return FirstT{};
	}

	constexpr SecondT second() const {
		return SecondT{};
	}

};

template <class FirstT, class SecondT>
class ConstexprPairStorage<FirstT, SecondT, std::enable_if_t<std::is_empty_v<FirstT> && !std::is_empty_v<SecondT>>> {
public:

	constexpr ConstexprPairStorage() = default;

	constexpr ConstexprPairStorage(First, Second)
	{
	}

	constexpr FirstT first() const {
		return FirstT{};
	}

	constexpr SecondT second() const {
		return second_;
	}

private:

	SecondT second_;

};

template <class FirstT, class SecondT>
class ConstexprPair : public ConstexprPairStorage<FirstT, SecondT> {
public:

	using First = FirstT;
	using Second = SecondT;

	constexpr ConstexprPair() = default;

	constexpr ConstexprPair(First f, Second s) :
		ConstexprPairStorage<FirstT, SecondT>(std::move(f), std::move(s))
	{
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
