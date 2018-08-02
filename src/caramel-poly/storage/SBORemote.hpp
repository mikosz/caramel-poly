#ifndef CARAMELPOLY_STORAGE_SBOREMOTE_HPP__
#define CARAMELPOLY_STORAGE_SBOREMOTE_HPP__

#include <variant>
#include <memory>

namespace caramel_poly::storage {

template <
	class RemoteStorageType,
	size_t BUFFER_SIZE_BYTES = sizeof(RemoteStorageType),
	size_t BUFFER_ALIGNMENT_BYTES = alignof(RemoteStorageType)
	>
class SBORemote final {
public:

	template <class T>
	SBORemote(T data) {
		if constexpr (
			(sizeof(data) <= sizeof(Buffer)) &&
			(alignof(T) <= BUFFER_ALIGNMENT_BYTES)
			)
		{
			new (&data_) T(std::move(data));
			usingSBO_ = true;
		} else {
			new (&data_) RemoteStorageType(std::move(data));
			usingSBO_ = false;
		}
	}

	~SBORemote() {
		if (!usingSBO_) {
			reinterpret_cast<RemoteStorageType*>(&data_)->~RemoteStorageType();
		}
	}

	void* get() {
		if (usingSBO_) {
			return &data_;
		} else {
			return reinterpret_cast<RemoteStorageType*>(&data_)->get();
		}
	}

	const void* get() const {
		return const_cast<SBORemote&>(*this).get();
	}

private:

	using Buffer = std::aligned_storage_t<
		std::max(BUFFER_SIZE_BYTES, sizeof(RemoteStorageType)), // TODO: if the buffer size is 4B and
			// alignment is 4B, we will get 3 extra bytes at the end of the object, right? If so, we should
			// extend the buffer by the 3 bytes, because why not. Right? Right?
		std::max(BUFFER_ALIGNMENT_BYTES, alignof(RemoteStorageType))
		>;

	Buffer data_;

	bool usingSBO_;

};

} // namespace caramel_poly::storage

#endif /* CARAMELPOLY_STORAGE_SBOREMOTE_HPP__ */
