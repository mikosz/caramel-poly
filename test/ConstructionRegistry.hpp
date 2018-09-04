// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)
//
// This is Louis Dionne's Dyno library adapted to work on Visual Studio 2017 and
// without Boost, modified to my taste. All credit for the design and delivery goes
// to Louis. His original implementation may be found here:
// https://github.com/ldionne/dyno

#ifndef CARAMELPOLY_TEST_CONSTRUCTIONREGISTRY_HPP__
#define CARAMELPOLY_TEST_CONSTRUCTIONREGISTRY_HPP__

#include <algorithm>
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
		const Object* immediateOriginal = nullptr;
		const Object* original = nullptr;
	};

	class Object {
	public:

		Object(ConstructionRegistry& registry) :
			registry_(registry)
		{
			registry_.get(this).constructed = true;
			registry_.get(this).original = this;
			registry_.get(this).immediateOriginal = this;
		}

		Object(const Object& other) :
			registry_(other.registry_)
		{
			registry_.get(this).copyConstructed = true;
			registry_.get(this).original = registry_.states_[&other].original;
			registry_.get(this).immediateOriginal = &other;
		}

		Object& operator=(const Object& other) {
			assert(&registry_ == &other.registry_);
			registry_.get(this).copyAssigned = true;
			registry_.get(this).original = registry_.states_[&other].original;
			registry_.get(this).immediateOriginal = &other;
			return *this;
		}

		Object(Object&& other) :
			registry_(other.registry_)
		{
			registry_.get(this).moveConstructed = true;
			registry_.get(this).original = registry_.states_[&other].original;
			registry_.get(this).immediateOriginal = &other;
		}

		Object& operator=(Object&& other) {
			assert(&registry_ == &other.registry_);
			registry_.get(this).moveAssigned = true;
			registry_.get(this).original = registry_.states_[&other].original;
			registry_.get(this).immediateOriginal = &other;
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

	bool allDestructed() const {
		return std::all_of(states_.begin(), states_.end(), [](const auto& stateEntry) {
				return stateEntry.second.destructed;
			});
	}

private:

	mutable std::unordered_map<const Object*, State> states_;

};

} // namespace caramel_poly::test

#endif /* CARAMELPOLY_TEST_CONSTRUCTIONREGISTRY_HPP__ */

