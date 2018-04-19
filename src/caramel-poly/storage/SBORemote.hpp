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
			(sizeof(data) <= BUFFER_SIZE_BYTES) &&
			(alignof(T) <= BUFFER_ALIGNMENT_BYTES)
			)
		{
			data_.emplace<Local>();
			new (&std::get<Local>(data_)) T(std::move(data));
		} else {
			data_.emplace<Remote>(std::move(data));
		}
	}

	const void* get() const {
		auto* local = std::get_if<Local>(&data_);
		if (local == nullptr) {
			return std::get<Remote>(data_).get();
		} else {
			return local;
		}
	}

private:

	using Local = std::aligned_storage_t<BUFFER_SIZE_BYTES, BUFFER_ALIGNMENT_BYTES>;

	using Remote = RemoteStorageType;

	std::variant<Local, Remote> data_;

};

} // namespace caramel_poly::storage

#endif /* CARAMELPOLY_STORAGE_SBOREMOTE_HPP__ */
