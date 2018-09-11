// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)
//
// This is Louis Dionne's Dyno library adapted to work on Visual Studio 2017 and
// without Boost, modified to my taste. All credit for the design and delivery goes
// to Louis. His original implementation may be found here:
// https://github.com/ldionne/dyno

#ifndef BENCHMARK_ANY_ITERATOR_VIRTUAL_HPP__
#define BENCHMARK_ANY_ITERATOR_VIRTUAL_HPP__

#include <memory>

namespace vrt {

class IAnyIterator {
public:
	virtual ~IAnyIterator() = default;
	virtual void increment() = 0;
	virtual void* dereference() = 0;
	virtual bool equal(const IAnyIterator& other) const = 0;
};

template <class Iterator>
class AnyIteratorImpl : public IAnyIterator {
public:

	AnyIteratorImpl(Iterator it) :
		it_(std::move(it))
	{
	}

	void increment() override {
		++it_;
	}

	void* dereference() override {
		return &*it_;
	}

	bool equal(const IAnyIterator& other) const {
		return it_ == reinterpret_cast<const AnyIteratorImpl<Iterator>&>(other).it_;
	}

private:

	Iterator it_;

};

template <class Value, class Reference = Value&>
struct any_iterator {
	using value_type = typename Value;
	using reference = typename Reference;

	template <typename Iterator>
	explicit any_iterator(Iterator it)
		: self_{new AnyIteratorImpl<Iterator>{std::move(it)}}
	{ }

	any_iterator(any_iterator&& other)
		: self_{std::move(other.self_)}
	{ }

	any_iterator& operator++() {
		self_->increment();
		return *this;
	}

	reference operator*() {
		return *static_cast<Value*>(self_->dereference());
	}

	friend bool operator==(any_iterator const& a, any_iterator const& b) {
		return a.self_->equal(*b.self_);
	}

private:
	std::unique_ptr<IAnyIterator> self_;
};

} // namespace vrt

#endif // BENCHMARK_ANY_ITERATOR_VIRTUAL_HPP__
