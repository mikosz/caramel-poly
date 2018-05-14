#include <gtest/gtest.h>

#include <type_traits>
#include <cassert>
#include <memory>

#include "caramel-poly/detail/EmptyObject.hpp"
#include "caramel-poly/storage/SBORemote.hpp"
#include "caramel-poly/storage/SharedRemote.hpp"

// better compile-time string?

namespace w00t {

/// COMPILE TIME STRING

template <char... CHARS>
class CompileTimeString final {
public:

	constexpr CompileTimeString() = default;

	template <char... OTHER_CHARS>
	constexpr bool operator==([[maybe_unused]] CompileTimeString<OTHER_CHARS...> other) const {
		return std::is_same_v<CompileTimeString, CompileTimeString<OTHER_CHARS...>>;
	}

	template <char... OTHER_CHARS>
	constexpr bool operator!=(CompileTimeString<OTHER_CHARS...> other) const {
		return !(*this == other);
	}

};

namespace string_detail {

template <class S, std::size_t... N>
constexpr auto prepare_impl(S, std::index_sequence<N...>)
{
	return CompileTimeString<S::get()[N]...>{};
}

template <typename S>
constexpr decltype(auto) prepare(S s) {
	return prepare_impl(s, std::make_index_sequence<sizeof(S::get()) - 1>{});
}

} // namespace string_detail

#define COMPILE_TIME_STRING(s)                                              \
    (::w00t::string_detail::prepare([]{                                     \
        struct tmp {                                                        \
            static constexpr decltype(auto) get() {                         \
				return s;                                                   \
			}                                                               \
        };                                                                  \
        return tmp{};                                                       \
    }()))

} // namespace w00t

////

//// POINTER TO LAMBDA

template <class F, class Signature>
class Lambda;

template <class LambdaType, class ReturnType, class... Args>
class Lambda<LambdaType, ReturnType(Args...)> {
public:

	static ReturnType invoke(Args... args) {
		const auto lambda = detail::EmptyObject<LambdaType>{}.get();
		return lambda(std::forward<Args>(args)...);
	}

};

template <class LambdaType, class... Args>
class Lambda<LambdaType, void(Args...)> {
public:

	static void invoke(Args... args) {
		const auto lambda = detail::EmptyObject<LambdaType>{}.get();
		lambda(std::forward<Args>(args)...);
	}

};

////

namespace /* anonymous */ {

using namespace caramel_poly;
using namespace caramel_poly::storage;

template <int ID_PARAM, class MethodType>
struct Entry {
	static constexpr auto ID = ID_PARAM;
	using Method = MethodType;
};

template <class... Entries>
class MP;

template <int ID, class Head, class... Tail>
class MP<Entry<ID, Head>, Tail...> : MP<Tail...> {
public:

	template <class... TailMethods>
	constexpr MP(Head* head, TailMethods... tail) :
		MP<Tail...>(tail...),
		method_(head)
	{
	}

	template <int GET_ID>
	constexpr auto get() const {
		if constexpr (GET_ID == ID) {
			return method_;
		} else {
			return MP<Tail...>::get<GET_ID>();
		}
	}

private:

	Head* method_;

};

template <>
class MP<> {
public:

	template <int GET_ID>
	constexpr auto get() const {
		throw std::out_of_range("No such function");
	}

};

template <class... Methods>
constexpr auto makeMethodMap(Methods... methods) {
	return MP<Methods...>(methods...);
}

template <class... Entries>
class VT {
public:

	template <class... Methods>
	constexpr VT(Methods... /*methods*/) :
		mp_(&Lambda<Methods, typename Entries::Method>::invoke...)
	{
	}

	template <int ID, class ReturnType, class... Args>
	ReturnType invoke(Args&&... args) const {
		return (*mp_.get<ID>())(std::forward<Args>(args)...);
	}

private:

	MP<Entries...> mp_;

};

struct VTConcept {
	using VTType = VT<
		Entry<0, void (const void*)>,
		Entry<1, bool (const void*, int)>
		>;
};

template <class T>
struct VTMapping {
	static constexpr auto vt = VTConcept::VTType(
		[](const void* self) { static_cast<const T*>(self)->~T(); },
		[](const void* self, int arg) { return static_cast<const T*>(self)->polymorphicFunction(arg); }
		);
};

class Poly final {
public:

	template <class T>
	Poly(T model) :
		vtable_(VTMapping<T>::vt),
		storage_(std::move(model))
	{
	}

	template <int fun, class ReturnType, class... Args>
	ReturnType invoke(Args&&... args) const {
		return vtable_.invoke<fun, ReturnType>(storage_.get(), std::forward<Args>(args)...);
	}

private:

	VTConcept::VTType vtable_;

	SBORemote<SharedRemote> storage_;

};

class Interface final {
public:

	template <class T>
	Interface(T model) :
		poly_(std::move(model))
	{
	}

	~Interface() {
		poly_.invoke<0, void>();
	}

	bool polymorphicFunction(int argument) const {
		return poly_.invoke<1, bool>(argument);
	}

private:

	Poly poly_;

};

struct Registry final {
	bool constructed = false;
	bool destructed = false;
	int functionArgument = 0;
	const void* objectAddress = nullptr;
};

class UserClass final {
public:

	UserClass() = default;

	UserClass(Registry* registry) :
		registry_(std::move(registry))
	{
		if (registry_) {
			registry_->constructed = true;
		}
	}

	~UserClass() {
		if (registry_) {
			assert(!registry_->destructed);
			registry_->destructed = true;
		}
	}

	UserClass(UserClass&& other) :
		UserClass()
	{
		swap(other);
	}

	UserClass& operator=(UserClass&& other) {
		UserClass(std::move(other)).swap(*this);
		return *this;
	}

	void swap(UserClass& other) {
		std::swap(other.registry_, registry_);
	}

	Registry& registry() const {
		assert(registry_ != nullptr);
		return *registry_;
	}

	bool polymorphicFunction(int argument) const {
		assert(registry_ != nullptr);
		registry_->functionArgument = argument;
		registry_->objectAddress = this;
		return true;
	}

private:

	mutable Registry* registry_ = nullptr;

};

TEST(ExampleTest, ConcreteImplementationsAreCalled) {
	auto registry = Registry();
	auto userObject = UserClass(&registry);

	{
		auto iface = Interface(std::move(userObject));
		EXPECT_TRUE(iface.polymorphicFunction(42));
	}

	EXPECT_TRUE(registry.constructed);
	EXPECT_TRUE(registry.destructed);
	EXPECT_EQ(registry.functionArgument, 42);
	EXPECT_NE(registry.objectAddress, &userObject);

	using namespace w00t;
	static_assert(COMPILE_TIME_STRING("dupa") == COMPILE_TIME_STRING("dupa"));
	static_assert(COMPILE_TIME_STRING("dupa") != COMPILE_TIME_STRING("dojpa"));
}

} // anonymous namespace
