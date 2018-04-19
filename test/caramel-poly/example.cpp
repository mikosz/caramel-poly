#include <gtest/gtest.h>

#include <cassert>
#include <memory>

#include "caramel-poly/storage/SBORemote.hpp"
#include "caramel-poly/storage/SharedRemote.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;
using namespace caramel_poly::storage;

class Interface final {
public:

	template <class T>
	Interface(T model) :
		storage_(Model<T>(std::move(model)))
	{
	}

	bool polymorphicFunction(int argument) const {
		return reinterpret_cast<const Concept*>(storage_.get())->polymorphicFunction(argument);
	}

private:

	class Concept {
	public:

		virtual ~Concept() = default;

		virtual bool polymorphicFunction(int argument) const = 0;

	};

	template <class T>
	class Model : public Concept {
	public:

		Model(T model) :
			model_(std::move(model))
		{
		}

		bool polymorphicFunction(int argument) const override {
			return polymorphicFunctionImpl(model_, argument);
		}

	private:
		
		T model_;

	};

	SBORemote<SharedRemote> storage_;

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

private:

	mutable Registry* registry_ = nullptr;

};

bool polymorphicFunctionImpl(const UserClass& object, int argument) {
	object.registry().functionArgument = argument;
	object.registry().objectAddress = &object;
	return true;
}

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
