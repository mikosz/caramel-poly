#ifndef CARAMELPOLY_VTABLE_METHOD_HPP__
#define CARAMELPOLY_VTABLE_METHOD_HPP__

#include "caramel-poly/const-string/ConstString.hpp"

namespace caramel_poly::vtable {

template <class Function>
class Method {
public:

	constexpr Method(const_string::ConstString name, Function function) :
		name_(std::move(name)),
		function_(std::move(function))
	{
	}

	template <class... Args>
	auto invoke(Args&&... args) const {
		return function_(std::forward<Args>(args)...);
	}

	constexpr const const_string::ConstString& name() const {
		return name_;
	}

private:

	const const_string::ConstString name_;

	const Function function_;

};

template <class Function>
constexpr auto createMethod(const_string::ConstString name, Function function) {
	return Method<Function>(std::move(name), std::move(function));
}

} // namespace caramel_poly::vtable

#endif /* CARAMELPOLY_VTABLE_METHOD_HPP__ */
