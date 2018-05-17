#ifndef CARAMELPOLY_VTABLE_LOCAL_HPP__
#define CARAMELPOLY_VTABLE_LOCAL_HPP__

namespace caramel_poly::vtable {

namespace detail {

} // namespace detail

template <class Concept>
class Local {
public:

	template <class ConceptMap>
	constexpr Local(Methods... methods) :
		methods_(makeMap(methods)...)
	{
	}

	template <class ReturnType, class... Args>
	constexpr ReturnType invoke(const_string::ConstString name, Args&&... args) const {
		return methods_[name].invoke<ReturnType>(std::forward<Args>(args)...);
	}

private:

	detail::Container<Concept>

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

#endif /* CARAMELPOLY_VTABLE_LOCAL_HPP__ */
