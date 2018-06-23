#ifndef CARAMELPOLY_DETAIL_BINDSIGNATURE_HPP__
#define CARAMELPOLY_DETAIL_BINDSIGNATURE_HPP__

#include "TransformSignature.hpp"

namespace caramel_poly::detail {

template <typename Old, typename New, typename T>
struct ReplaceImpl {
	using Type = T;
};

template <typename Old, typename New>
struct ReplaceImpl<Old, New, Old> {
	using Type = New;
};

template <typename Old, typename New>
struct ReplaceImpl<Old, New, Old&> {
	using Type = New&;
};

template <typename Old, typename New>
struct ReplaceImpl<Old, New, Old&&> {
	using Type = New&&;
};

template <typename Old, typename New>
struct ReplaceImpl<Old, New, Old*> {
	using Type = New*;
};

template <typename Old, typename New>
struct ReplaceImpl<Old, New, const Old> {
	using Type = const New;
};

template <typename Old, typename New>
struct ReplaceImpl<Old, New, const Old&> {
	using Type = const New&;
};

template <typename Old, typename New>
struct ReplaceImpl<Old, New, const Old&&> {
	using Type = const New&&;
};

template <typename Old, typename New>
struct ReplaceImpl<Old, New, const Old*> {
	using Type = const New*;
};

template <typename Old, typename New>
struct Replace {
	template <typename T>
	using apply = ReplaceImpl<Old, New, T>;
};

template <typename Signature, typename T>
using BindSignature = TransformSignature<
	Signature, Replace<vtable::Object, T>::template apply
	>;

} // namespace caramel_poly::detail

#endif /* CARAMELPOLY_DETAIL_BINDSIGNATURE_HPP__ */
