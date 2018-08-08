// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)
//
// This is Louis Dionne's Dyno library adapted to work on Visual Studio 2017 and
// without Boost, modified to my taste. All credit for the design and delivery goes
// to Louis. His original implementation may be found here:
// https://github.com/ldionne/dyno

#include <gtest/gtest.h>

#include <type_traits>

#include "caramel-poly/detail/EraserTraits.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;
using namespace caramel_poly::detail;

struct S {
};

TEST(EraserTraitsTest, ErasePlaceholderReturnsSameTypeForNonPlaceholder) {
	static_assert(std::is_same_v<ErasePlaceholder<void, S>::Type, S>);
	static_assert(std::is_same_v<ErasePlaceholder<void, S&>::Type, S&>);
	static_assert(std::is_same_v<ErasePlaceholder<void, const S&>::Type, const S&>);
	static_assert(std::is_same_v<ErasePlaceholder<void, S&&>::Type, S&&>);
	static_assert(std::is_same_v<ErasePlaceholder<void, S*>::Type, S*>);
	static_assert(std::is_same_v<ErasePlaceholder<void, const S*>::Type, const S*>);
}

TEST(EraserTraitsTest, ErasePlaceholderTransformsPlaceholderToVoidPointer) {
	static_assert(std::is_same_v<ErasePlaceholder<void, SelfPlaceholder&>::Type, void*>);
	static_assert(std::is_same_v<ErasePlaceholder<void, const SelfPlaceholder&>::Type, const void*>);
	static_assert(std::is_same_v<ErasePlaceholder<void, SelfPlaceholder&&>::Type, void*>);
	static_assert(std::is_same_v<ErasePlaceholder<void, SelfPlaceholder*>::Type, void*>);
	static_assert(std::is_same_v<ErasePlaceholder<void, const SelfPlaceholder*>::Type, const void*>);
}

TEST(EraserTraitsTest, EraseTransformsPlaceholderToVoidPointer) {
	static_assert(std::is_same_v<decltype(Erase<void, SelfPlaceholder&>::apply(std::declval<S&>())), void*>);
	static_assert(
		std::is_same_v<
			decltype(Erase<void, const SelfPlaceholder&>::apply(std::declval<const S&>())),
			const void*
			>
		);
	static_assert(
		std::is_same_v<
			decltype(Erase<void, SelfPlaceholder&&>::apply(std::declval<S&&>())),
			void*
			>
		);
	static_assert(std::is_same_v<decltype(Erase<void, SelfPlaceholder*>::apply(std::declval<S*>())), void*>);
	static_assert(
		std::is_same_v<
			decltype(Erase<void, const SelfPlaceholder*>::apply(std::declval<const S*>())),
			const void*
			>
		);

	auto s = S{};
	auto& ref = s;
	const auto& cref = s;
	auto* ptr = &s;
	const auto& cptr = &s;
	auto* voidPtr = reinterpret_cast<void*>(&s);
	const auto* cvoidPtr = reinterpret_cast<const void*>(&s);

	EXPECT_EQ((Erase<void, SelfPlaceholder&>::apply(ref)), voidPtr);
	EXPECT_EQ((Erase<void, const SelfPlaceholder&>::apply(cref)), cvoidPtr);
	EXPECT_EQ((Erase<void, SelfPlaceholder&&>::apply(std::move(s))), voidPtr);
	EXPECT_EQ((Erase<void, SelfPlaceholder*>::apply(ptr)), voidPtr);
	EXPECT_EQ((Erase<void, const SelfPlaceholder*>::apply(cptr)), cvoidPtr);
}

TEST(EraserTraitsTest, UneraseTransformsVoidPointerToErasedObject) {
	auto s = S{};
	auto* voidPtr = reinterpret_cast<void*>(&s);
	const auto* cvoidPtr = reinterpret_cast<const void*>(&s);

	static_assert(std::is_same_v<decltype(Unerase<void, SelfPlaceholder&, S&>::apply(voidPtr)), S&>);
	static_assert(
		std::is_same_v<
			decltype(Unerase<void, const SelfPlaceholder&, const S&>::apply(cvoidPtr)),
			const S&
			>
		);
	static_assert(std::is_same_v<decltype(Unerase<void, SelfPlaceholder&&, S&&>::apply(voidPtr)), S&&>);
	static_assert(std::is_same_v<decltype(Unerase<void, SelfPlaceholder*, S*>::apply(voidPtr)), S*>);
	static_assert(
		std::is_same_v<
			decltype(Unerase<void, const SelfPlaceholder*, const S*>::apply(cvoidPtr)),
			const S*
			>
		);

	EXPECT_EQ((&Unerase<void, SelfPlaceholder&, S&>::apply(voidPtr)), &s);
	EXPECT_EQ((&Unerase<void, const SelfPlaceholder&, const S&>::apply(cvoidPtr)), &s);
	EXPECT_EQ((Unerase<void, SelfPlaceholder*, S*>::apply(voidPtr)), &s);
	EXPECT_EQ((Unerase<void, const SelfPlaceholder*, const S*>::apply(cvoidPtr)), &s);

	auto&& rvalueRef = Unerase<void, SelfPlaceholder&&, S&&>::apply(voidPtr);
	EXPECT_EQ(&rvalueRef, voidPtr);
}

} // anonymous namespace
