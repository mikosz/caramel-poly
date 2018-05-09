#include <gtest/gtest.h>

#include <cassert>
#include <memory>

#include "caramel-poly/storage/SBORemote.hpp"
#include "caramel-poly/storage/SharedRemote.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;
using namespace caramel_poly::storage;

class VT {
public:

	using Destructor = void (*)(const void*);

	using PolymorphicFunction = bool (*)(const void*, int);

	VT(Destructor destructor, PolymorphicFunction polymorphicFunction) :
		destructor_(destructor),
		polymorphicFunction_(polymorphicFunction)
	{
	}

	template <int fun, class ReturnType, class... Args>
	ReturnType invoke(Args&&... args) const {
		if constexpr (fun == 0) {
			return (*destructor_)(std::forward<Args>(args)...);
		} else if constexpr (fun == 1) {
			return (*polymorphicFunction_)(std::forward<Args>(args)...);
		} else {
			throw std::out_of_range("Bad fun");
		}
	}

private:

	Destructor destructor_;

	PolymorphicFunction polymorphicFunction_;

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
VT makeVT() {
	return VT(&polyDes<T>, &polyPoly<T>);
}

class Poly final {
public:

	template <class T>
	Poly(T model) :
		storage_(std::move(model)),
		vtable_(makeVT<T>())
	{
	}

	template <int fun, class ReturnType, class... Args>
	ReturnType invoke(Args&&... args) const {
		return vtable_.invoke<fun, ReturnType>(storage_.get(), std::forward<Args>(args)...);
	}

private:

	SBORemote<SharedRemote> storage_;

	VT vtable_;

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
