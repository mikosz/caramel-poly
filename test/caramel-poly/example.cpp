#include <gtest/gtest.h>

#include <cassert>
#include <memory>

#include "caramel-poly/storage/SBORemote.hpp"
#include "caramel-poly/storage/SharedRemote.hpp"

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
	constexpr MP(Head head, TailMethods... tail) :
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

	Head method_;

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
	constexpr VT(Methods... methods) :
		mp_(methods...)
	{
	}

	template <int ID, class ReturnType, class... Args>
	ReturnType invoke(Args&&... args) const {
		return (*mp_.get<ID>())(std::forward<Args>(args)...);
	}

private:

	MP<Entries...> mp_;

};

template <class T>
void polyDes(const void* self) {
	static_cast<const T*>(self)->~T();
}

template <class T>
bool polyPoly(const void* self, int arg) {
	return static_cast<const T*>(self)->polymorphicFunction(arg);
}

struct VTConcept {
	using VTType = VT<
		Entry<0, void (*)(const void*)>,
		Entry<1, bool (*)(const void*, int)>
		>;
};

template <class T>
struct VTMapping {
	static constexpr auto vt = VTConcept::VTType(&polyDes<T>, &polyPoly<T>);
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
}

} // anonymous namespace
