// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)
//
// This is Louis Dionne's Dyno library adapted to work on Visual Studio 2017 and
// without Boost, modified to my taste. All credit for the design and delivery goes
// to Louis. His original implementation may be found here:
// https://github.com/ldionne/dyno

#include <gtest/gtest.h>

#include <type_traits>

#include "caramel-poly/detail/EraseSignature.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;
using namespace caramel_poly::detail;

TEST(EraseSignatureTest, ErasesAllOccurencesOfPlaceholderWithVoidPtr) {
	using Signature = SelfPlaceholder& (const SelfPlaceholder*, SelfPlaceholder&&);
	using ErasedSignature = EraseSignature<Signature>::Type;
	using ErasedResult = EraseSignature<Signature>::Result;
	using ExpextedErasedSignature = void* (const void*, void*);

	static_assert(std::is_same_v<ErasedSignature, ExpextedErasedSignature>);
	static_assert(std::is_same_v<ErasedResult, void*>);
}

TEST(EraseSignatureTest, KeepsNonPlaceholderTypesAsTheyAre) {
	using Signature = int& (const float*, char&&);
	using ErasedSignature = EraseSignature<Signature>::Type;
	using ErasedResult = EraseSignature<Signature>::Result;

	static_assert(std::is_same_v<ErasedSignature, Signature>);
	static_assert(std::is_same_v<ErasedResult, int&>);
}

} // anonymous namespace
