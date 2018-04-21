#ifndef CARAMELPOLY_CONST_STRING_CONSTSTRING_HPP__
#define CARAMELPOLY_CONST_STRING_CONSTSTRING_HPP__

#include <stdexcept>

namespace caramel_poly::const_string {

/**
 * Compile-time strings.
 * Based on Scott Schurr's str_const presented at C++ Now 2012
 */
class ConstString {
public:

	template <size_t SIZE>
	constexpr ConstString(const char(&chars)[SIZE]) :
		chars_(chars),
		length_(SIZE - 1)
	{
	}

	constexpr size_t length() const {
		return length_;
	}

	constexpr char operator[](size_t n) const {
		if (n > length()) {
			throw std::out_of_range("Index larger than string length");
		} else {
			return chars_[n];
		}
	}

	friend constexpr bool operator==(const ConstString& lhs, const ConstString& rhs) {
		if (lhs.length() != rhs.length()) {
			return false;
		}

		for (size_t i = 0; i < lhs.length(); ++i) {
			if (lhs[i] != rhs[i]) {
				return false;
			}
		}

		return true;
	}

	friend constexpr bool operator!=(const ConstString& lhs, const ConstString& rhs) {
		return !(lhs == rhs);
	}

private:

	const char* const chars_;

	const size_t length_;

};

} // namespace caramel_poly::const_string

#endif /* CARAMELPOLY_CONST_STRING_CONSTSTRING_HPP__ */
