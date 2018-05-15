#ifndef CARAMELPOLY_COMPILETIME_STRING_HPP__
#define CARAMELPOLY_COMPILETIME_STRING_HPP__

#include <utility>

namespace caramel_poly {
namespace compile_time {

template <char... CHARS>
class CompileTimeString final {
public:

	constexpr CompileTimeString() = default;

	constexpr size_t length() const {
		return sizeof...(CHARS);
	}

	constexpr const char* c_str() const {
		return string_detail::COMPILE_STRING_STORAGE<CHARS...>;
	}

	constexpr char operator[](size_t index) const {
		return c_str()[index];
	}

	template <char... OTHER_CHARS>
	constexpr bool operator==([[maybe_unused]] CompileTimeString<OTHER_CHARS...> other) const {
		return std::is_same_v<CompileTimeString, CompileTimeString<OTHER_CHARS...>>;
	}

	template <char... OTHER_CHARS>
	constexpr bool operator!=(CompileTimeString<OTHER_CHARS...> other) const {
		return !(*this == other);
	}

};

namespace string_detail {

template <char... CHARS>
constexpr const char COMPILE_STRING_STORAGE[] = { CHARS..., '\0' };

template <class S, std::size_t... N>
constexpr auto prepare_impl(S, std::index_sequence<N...>)
{
	return CompileTimeString<S::get()[N]...>{};
}

template <typename S>
constexpr decltype(auto) prepare(S s) {
	return prepare_impl(s, std::make_index_sequence<sizeof(S::get()) - 1>{});
}

} // namespace string_detail

} // namespace compile_time
} // namespace caramel_poly

#define COMPILE_TIME_STRING(s)                                              \
    (::caramel_poly::compile_time::string_detail::prepare([]{               \
        struct tmp {                                                        \
            static constexpr decltype(auto) get() {                         \
				return s;                                                   \
			}                                                               \
        };                                                                  \
        return tmp{};                                                       \
    }()))

#endif /* CARAMELPOLY_COMPILETIME_STRING_HPP__ */
