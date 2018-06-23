#ifndef CARAMELPOLY_DETAIL_TRANSFORMSIGNATURE_HPP__
#define CARAMELPOLY_DETAIL_TRANSFORMSIGNATURE_HPP__

namespace caramel_poly::detail {

// Transforms a signature by applying a metafunction to the return type and
// all the arguments of a function signature. This returns a function type,
// not a pointer to a function.
template <typename Signature, template <typename ...> class F>
struct TransformSignature;

template <typename R, typename ...Args, template <typename ...> class F>
struct TransformSignature<R (Args...), F> {
	using Result = typename F<R>::Type;
	using Type = Result (typename F<Args>::Type...);
};

} // namespace caramel_poly::detail

#endif /* CARAMELPOLY_DETAIL_TRANSFORMSIGNATURE_HPP__ */
