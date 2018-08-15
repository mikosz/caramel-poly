// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)

#ifndef CARAMELPOLY_METHOD_HPP__
#define CARAMELPOLY_METHOD_HPP__

namespace caramel_poly {

template <class SignatureT>
struct Method {
	using Signature = SignatureT;
};

template <class Signature>
constexpr auto method = Method<Signature>{};

} // namespace caramel_poly

#endif /* CARAMELPOLY_METHOD_HPP__ */
