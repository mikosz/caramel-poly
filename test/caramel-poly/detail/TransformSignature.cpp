// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)
//
// This is Louis Dionne's Dyno library adapted to work on Visual Studio 2017 and
// without Boost, modified to my taste. All credit for the design and delivery goes
// to Louis. His original implementation may be found here:
// https://github.com/ldionne/dyno

#include <gtest/gtest.h>

#include <type_traits>

#include "caramel-poly/detail/TransformSignature.hpp"
#include "caramel-poly/detail/EraserTraits.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;
using namespace caramel_poly::detail;

struct S {
};

template <class T>
struct Converted {
	using Type = T;
};

template <class T>
struct Metafunction {
	using Type = Converted<T>;
};

TEST(TransformSignatureTest, AppliesMetafunctionToAllTypesInSignature) {
	using Signature = int& (char&&, void*);
	using TransformedSignature = TransformSignature<Signature, Metafunction>::Type;
	using ExpectedTransformedSignature = Converted<int&> (Converted<char&&>, Converted<void*>);
	static_assert(std::is_same_v<TransformedSignature, ExpectedTransformedSignature>);
}

} // anonymous namespace
