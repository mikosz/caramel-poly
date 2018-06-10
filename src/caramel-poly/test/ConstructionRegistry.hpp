#ifndef CARAMELPOLY_TEST_CONSTRUCTIONREGISTRY_HPP__
#define CARAMELPOLY_TEST_CONSTRUCTIONREGISTRY_HPP__

#include <cassert>
#include <unordered_map>

namespace caramel_poly::test {

class ConstructionRegistry {
public:

	class Object;

	struct State {
		bool constructed = false;
		bool copyConstructed = false;
		bool copyAssigned = false;
		bool moveConstructed = false;
		bool moveAssigned = false;
		bool destructed = false;
		const Object* original = nullptr;
	};

	class Object {
	public:

		Object(ConstructionRegistry& registry) :
			registry_(registry)
		{
			registry_.get(this).constructed = true;
			registry_.get(this).original = this;
		}

		Object(const Object& other) :
			registry_(other.registry_)
		{
			registry_.get(this).copyConstructed = true;
			registry_.get(this).original = registry_.states_[&other].original;
		}

		Object& operator=(const Object& other) {
			assert(&registry_ == &other.registry_);
			registry_.get(this).copyAssigned = true;
			registry_.get(this).original = registry_.states_[&other].original;
			return *this;
		}

		Object(Object&& other) :
			registry_(other.registry_)
		{
			registry_.get(this).moveConstructed = true;
			registry_.get(this).original = registry_.states_[&other].original;
		}

		Object& operator=(Object&& other) {
			assert(&registry_ == &other.registry_);
			registry_.get(this).moveAssigned = true;
			registry_.get(this).original = registry_.states_[&other].original;
			return *this;
		}

		~Object() {
			registry_.get(this).destructed = true;
		}

		State& state() const {
			return registry_.get(this);
		}

	private:

		ConstructionRegistry& registry_;

	};

	State& get(const Object* object) const {
		return states_[object];
	}

private:

	mutable std::unordered_map<const Object*, State> states_;

};

} // namespace caramel_poly::test

#endif /* CARAMELPOLY_TEST_CONSTRUCTIONREGISTRY_HPP__ */

