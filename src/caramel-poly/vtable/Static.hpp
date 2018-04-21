#ifndef CARAMELPOLY_VTABLE_STATIC_HPP__
#define CARAMELPOLY_VTABLE_STATIC_HPP__

namespace caramel_poly::vtable {

template <class... Methods>
class Static {
public:

	constexpr Static(Methods&&... methods) :
		methods_(std::forward<Methods>(methods)...)
	{
	}

	

private:

	Container<Methods...> methods_;

};

} // namespace caramel_poly::vtable

#endif /* CARAMELPOLY_VTABLE_STATIC_HPP__ */
