#ifndef CARAMELPOLY_VTABLE_STATIC_HPP__
#define CARAMELPOLY_VTABLE_STATIC_HPP__

#include "caramel-poly/const-string/ConstString.hpp"
#include "Method.hpp"
#include "Map.hpp"

namespace caramel_poly::vtable {

template <class... Methods>
class Static {
public:

	constexpr Static(Methods... methods) :
		methods_(makeMap(methods)...)
	{
	}

	template <class ReturnType, class... Args>
	constexpr ReturnType invoke(const_string::ConstString name, Args&&... args) const {
		return methods_[name].invoke<ReturnType>(std::forward<Args>(args)...);
	}

private:

	Map<Methods...> methods_;

};

template <class... Methods>
constexpr auto makeStatic(Methods... methods) {
	return Static<Methods...>(methods...);
}

template <uint32_t MethodNameCrc, class Function>
struct MapEntryKey<Method<MethodNameCrc, Function>> {
	constexpr auto operator()() const {
		return MethodNameCrc;
	}
};

} // namespace caramel_poly::vtable

#endif /* CARAMELPOLY_VTABLE_STATIC_HPP__ */
