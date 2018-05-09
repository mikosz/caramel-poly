#ifndef CARAMELPOLY_VTABLE_METHOD_HPP__
#define CARAMELPOLY_VTABLE_METHOD_HPP__

#include "caramel-poly/const-string/ConstString.hpp"

namespace caramel_poly::vtable {

template <uint32_t NAME_CRC, class Function>
class Method {
public:

	template <class... Args>
	auto invoke(Args&&... args) const {
		return Function{}(std::forward<Args>(args)...);
	}

};

} // namespace caramel_poly::vtable

#endif /* CARAMELPOLY_VTABLE_METHOD_HPP__ */
