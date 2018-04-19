#ifndef CARAMELPOLY_STORAGE_SHAREDREMOTE_HPP__
#define CARAMELPOLY_STORAGE_SHAREDREMOTE_HPP__

#include <memory>

namespace caramel_poly::storage {

class SharedRemote final {
public:

	template <class T>
	SharedRemote(T data) :
		data_(std::make_shared<T>(std::move(data)))
	{
	}

	const void* get() const {
		return data_.get();
	}

private:

	std::shared_ptr<void> data_;

};

} // namespace caramel_poly::storage

#endif /* CARAMELPOLY_STORAGE_SHAREDREMOTE_HPP__ */
