#include <gtest/gtest.h>

#include <cassert>
#include <memory>

#include "caramel-poly/storage/SBORemote.hpp"
#include "caramel-poly/storage/SharedRemote.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;
using namespace caramel_poly::storage;

template <int... MethodIds>
class MP;

template <int METHOD_ID, int... MethodIds>
class MP<METHOD_ID, MethodIds...> : MP<MethodIds...> {
public:

	template <class Method, class... Methods>
	MP(Method&& method, Methods&&... methods) :
		MP<MethodIds...>(std::forward<Methods>(methods)...),
		method_(std::forward<Method>(method))
	{
	}

	template <int ID>
	const void* const get() const {
		if constexpr (ID == METHOD_ID) {
			return method_;
		} else {
			return MP<MethodIds...>::get<ID>();
		}
	}

private:

	const void* const method_;

};

template <>
class MP<> {
public:

	template <int ID>
	const void* get() const {
		throw std::out_of_range("No such function");
	}

};

template <int... MethodIds>
class VT {
public:

	template <class... Methods>
	VT(Methods&&... methods) :
		mp_(std::forward<Methods>(methods)...)
	{
	}

	template <int fun, class ReturnType, class... Args>
	ReturnType invoke(Args&&... args) const {
		// this is a bug: due to args being passed by a universal-reference, we cast a function
		// that takes a by-value argument to a function taking a rvalue-reference.
		return (*static_cast<ReturnType (*)(Args...)>(mp_.get<fun>()))(std::forward<Args>(args)...);
	}

private:

	MP<MethodIds...> mp_;

};

template <class T>
void polyDes(const void* self) {
	static_cast<const T*>(self)->~T();
}

template <class T>
bool polyPoly(const void* self, int arg) {
	return static_cast<const T*>(self)->polymorphicFunction(arg);
}

template <class T>
VT<0, 1> makeVT() {
	return VT<0, 1>(&polyDes<T>, &polyPoly<T>);
}

class Poly final {
public:

	template <class T>
	Poly(T model) :
		vtable_(makeVT<T>()),
		storage_(std::move(model))
	{
	}

	template <int fun, class ReturnType, class... Args>
	ReturnType invoke(Args&&... args) const {
		return vtable_.invoke<fun, ReturnType>(storage_.get(), std::forward<Args>(args)...);
	}

private:

	VT<0, 1> vtable_;

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
}

} // anonymous namespace
