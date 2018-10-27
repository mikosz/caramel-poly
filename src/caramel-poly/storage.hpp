// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)
//
// This is Louis Dionne's Dyno library adapted to work on Visual Studio 2017 and
// without Boost, modified to my taste. All credit for the design and delivery goes
// to Louis. His original implementation may be found here:
// https://github.com/ldionne/dyno

#ifndef CARAMELPOLY_STORAGE_HPP__
#define CARAMELPOLY_STORAGE_HPP__

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <memory>
#include <type_traits>
#include <utility>

#include "dsl.hpp"
#include "builtin.hpp"

namespace caramel_poly {

struct MallocAllocator {
	static void* allocate(std::size_t size) {
		return std::malloc(size);
	}

	static void free(void* ptr) {
		std::free(ptr);
	}

	template <class T, class... Args>
	static std::shared_ptr<T> makeShared(Args&&... args) {
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
};

// concept PolymorphicStorage
//
// The PolymorphicStorage concept represents storage that can be used to store
// an object of an arbitrary type. In a sense, it is like a special-purpose
// allocator that could only ever allocate a single object.
//
// The nice thing about these PolymorphicStorage classes is that they have a
// single type and their ABI does not change even when the type of what they
// hold changes. However, they provide this functionality at the cost of
// "forgetting" the type information of what they hold, which is why they
// must be passed a vtable to perform most operations.
//
// A type `Storage` satisfying the `PolymorphicStorage` concept must provide
// the following functions as part of its interface:
//
// template <class T> explicit Storage(T&&);
//  Semantics: Construct an object of type `std::decay_t<T>` in the polymorphic
//             storage, forward the argument to the constructor of the object
//             being created. A particular `Storage` class is not expected to
//             support being constructed from any such `T`; for example, `T`
//             could be too large to fit in a predefined buffer size, in which
//             case this call would not compile.
//
// template <class VTable> Storage(const Storage&, const VTable&);
//  Semantics: Copy-construct the contents of the polymorphic storage,
//             assuming the contents of the source storage can be
//             manipulated using the provided vtable.
//
// template <class VTable> Storage(Storage&&, const VTable&);
//  Semantics: Move-construct the contents of the polymorphic storage,
//             assuming the contents of the source storage can be
//             manipulated using the provided vtable.
//
// template <class MyVTable, class OtherVTable>
// void swap(const MyVTable&, Storage&, const OtherVTable&);
//  Semantics: Swap the contents of the two polymorphic storages, assuming
//             `*this` can be manipulated using `MyVTable` and the other
//             storage can be manipulated using `OtherVTable`.
//
// template <class VTable> void destruct(const VTable&);
//  Semantics: Destruct the object held inside the polymorphic storage, assuming
//             that object can be manipulated using the provided vtable. This
//             must also free any resource required for storing the object.
//             However, this is not the same as destructing the polymorphic
//             storage itself (the wrapper), for which the destructor must
//             still be called.
//
//             WARNING: Since this is not using the usual destructor mechanism,
//             it is of utmost importance that users of these storage classes
//             call the `destruct` method explicitly in their destructor.
//             Furthermore, it should be noted that if an exception is thrown
//             in the constructor of a class (say `any_iterator`) using a
//             storage class defined here, the cleanup will NOT be done
//             automatically because the destructor of `any_iterator` will
//             not be called.
//
// template <class T = void> T* get();
//  Semantics: Return a pointer of type `T` to the object inside the polymorphic
//             storage. If `T` is not the actual type of the object stored
//             inside the polymorphic storage, the behavior is undefined.
//
// template <class T = void> const T* get() const;
//  Semantics: Return a pointer of type `T` to the object inside the polymorphic
//             storage. If `T` is not the actual type of the object stored
//             inside the polymorphic storage, the behavior is undefined.
//
// static constexpr bool canStore(caramel_poly::StorageInfo);
//  Semantics: Return whether the polymorphic storage can store an object with
//             the specified type information.

// Class implementing the small buffer optimization (SBO).
//
// This class represents a value of an unknown type that is stored either on
// the heap, or on the stack if it fits in the specific small buffer size.
//
// TODO: - Consider having ptr_ always point to either sb_ or the heap.
//       - Alternatively, if we had a way to access the vtable here, we could
//         retrieve the size of the type from it and get rid of `uses_heap_`.
//       - We could also use the low bits of the pointer to the vtable for
//         `uses_heap_`.
template <
	std::size_t SIZE,
	std::size_t ALIGN = static_cast<std::size_t>(-1),
	class Allocator = MallocAllocator
	>
class SBOStorage {
public:

	static constexpr std::size_t SBSIZE = (SIZE < sizeof(void*)) ? sizeof(void*) : SIZE;
	static constexpr std::size_t SBALIGN =
		(ALIGN == static_cast<std::size_t>(-1)) ? alignof(std::aligned_storage_t<SBSIZE>) : ALIGN;

	SBOStorage() = delete;
	SBOStorage(const SBOStorage&) = delete;
	SBOStorage(SBOStorage&&) = delete;
	SBOStorage& operator=(SBOStorage&&) = delete;
	SBOStorage& operator=(const SBOStorage&) = delete;

	template <class T, class RawT = std::decay_t<T>>
	explicit SBOStorage(T&& t) {
		// TODO: We could also construct the object at an aligned address within
		// the buffer, which would require computing the right address everytime
		// we access the buffer as a T, but would allow more Ts to fit in the SBO.
		if constexpr (canStore(caramel_poly::storageInfoFor<RawT>)) {
			usesHeap_ = false;
			new (&sb_) RawT(std::forward<T>(t));
		} else {
			usesHeap_ = true;
			ptr_ = Allocator::allocate(sizeof(RawT));
			// TODO: Allocating and then calling the constructor is not
			//       exception-safe if the constructor throws.
			// TODO: That's not a really nice way to handle this
			assert(ptr_ != nullptr && "Memory allocation failed, we're doomed");
			new (ptr_) RawT(std::forward<T>(t));
		}
	}

	template <class VTable>
	SBOStorage(const SBOStorage& other, const VTable& vtable) {
		if (other.usesHeap_) {
			auto info = vtable[STORAGE_INFO_LABEL]();
			usesHeap_ = true;
			ptr_ = Allocator::allocate(info.size);
			// TODO: That's not a really nice way to handle this
			assert(ptr_ != nullptr && "Memory allocation failed, we're doomed");
			vtable[COPY_CONSTRUCT_LABEL](ptr_, other.get());
		} else {
			usesHeap_ = false;
			vtable[COPY_CONSTRUCT_LABEL](&sb_, other.get());
		}
	}

	template <class VTable>
	SBOStorage(SBOStorage&& other, const VTable& vtable) :
		usesHeap_{other.usesHeap_}
	{
		if (usesHeap_) {
			this->ptr_ = other.ptr_;
			other.ptr_ = nullptr;
		} else {
			vtable[MOVE_CONSTRUCT_LABEL](this->get(), other.get());
		}
	}

	template <class ThisVTable, class OtherVTable>
	void swap(const ThisVTable& thisVTable, SBOStorage& other, const OtherVTable& otherVTable) {
		if (this == &other) {
			return;
		}

		if (this->usesHeap_) {
			if (other.usesHeap_) {
				using std::swap;
				swap(this->ptr_, other.ptr_);
			} else {
				void* ptr = this->ptr_;

				// Bring `other`'s contents to `*this`, destructively
				otherVTable[MOVE_CONSTRUCT_LABEL](&this->sb_, &other.sb_);
				otherVTable[DESTRUCT_LABEL](&other.sb_);
				this->usesHeap_ = false;

				// Bring `*this`'s stuff to `other`
				other.ptr_ = ptr;
				other.usesHeap_ = true;
			}
		} else {
			if (other.usesHeap_) {
				void *ptr = other.ptr_;

				// Bring `*this`'s contents to `other`, destructively
				thisVTable[MOVE_CONSTRUCT_LABEL](&other.sb_, &this->sb_);
				thisVTable[DESTRUCT_LABEL](&this->sb_);
				other.usesHeap_ = false;

				// Bring `other`'s stuff to `*this`
				this->ptr_ = ptr;
				this->usesHeap_ = true;

			} else {
				// Move `other` into temporary local storage, destructively.
				SBStorage tmp;
				otherVTable[MOVE_CONSTRUCT_LABEL](&tmp, &other.sb_);
				otherVTable[DESTRUCT_LABEL](&other.sb_);

				// Move `*this` into `other`, destructively.
				thisVTable[MOVE_CONSTRUCT_LABEL](&other.sb_, &this->sb_);
				thisVTable[DESTRUCT_LABEL](&this->sb_);

				// Now, bring `tmp` into `*this`, destructively.
				otherVTable[MOVE_CONSTRUCT_LABEL](&this->sb_, &tmp);
				otherVTable[DESTRUCT_LABEL](&tmp);
			}
		}
	}

	template <class VTable>
	void destruct(const VTable& vtable) {
		if (usesHeap_) {
			// If we've been moved from, don't do anything.
			if (ptr_ == nullptr)
				return;

			vtable[DESTRUCT_LABEL](ptr_);
			Allocator::free(ptr_);
		} else {
			vtable[DESTRUCT_LABEL](&sb_);
		}
	}

	template <class T = void>
	T* get() {
		return static_cast<T*>(usesHeap_ ? ptr_ : &sb_);
	}

	template <class T = void>
	const T* get() const {
		return static_cast<const T*>(usesHeap_ ? ptr_ : &sb_);
	}

private:

	using SBStorage = std::aligned_storage_t<SBSIZE, SBALIGN>;

	union {
		void* ptr_;
		SBStorage sb_;
	};

	// TODO: It might be possible to pack this bool inside the union somehow.
	bool usesHeap_;

	static constexpr bool canStore(caramel_poly::StorageInfo info) {
		return (info.size <= SBSIZE) && (SBALIGN % info.alignment == 0);
	}

};

// Class implementing storage on the heap. Just like the `SBOStorage`, it
// only handles allocation and deallocation; construction and destruction
// must be handled externally.
template <class Allocator = MallocAllocator>
struct RemoteStorage {
	RemoteStorage() = delete;
	RemoteStorage(const RemoteStorage&) = delete;
	RemoteStorage(RemoteStorage&&) = delete;
	RemoteStorage& operator=(RemoteStorage&&) = delete;
	RemoteStorage& operator=(const RemoteStorage&) = delete;

	template <class T, class RawT = std::decay_t<T>>
	explicit RemoteStorage(T&& t) :
		ptr_{Allocator::allocate(sizeof(RawT))}
	{
		// TODO: That's not a really nice way to handle this
		assert(ptr_ != nullptr && "Memory allocation failed, we're doomed");

		new (ptr_) RawT(std::forward<T>(t));
	}

	template <class VTable>
	RemoteStorage(const RemoteStorage& other, const VTable& vtable) :
		ptr_{Allocator::allocate(vtable[STORAGE_INFO_LABEL]().size)}
	{
		// TODO: That's not a really nice way to handle this
		assert(ptr_ != nullptr && "Memory allocation failed, we're doomed");

		vtable[COPY_CONSTRUCT_LABEL](this->get(), other.get());
	}

	template <class VTable>
	RemoteStorage(RemoteStorage&& other, const VTable&) :
		ptr_{other.ptr_}
	{
		other.ptr_ = nullptr;
	}

	template <class ThisVTable, class OtherVTable>
	void swap(const ThisVTable&, RemoteStorage& other, const OtherVTable&) {
		std::swap(this->ptr_, other.ptr_);
	}

	template <class VTable>
	void destruct(const VTable& vtable) {
		// If we've been moved from, don't do anything.
		if (ptr_ == nullptr) {
			return;
		}

		vtable[DESTRUCT_LABEL](ptr_);
		Allocator::free(ptr_);
	}

	template <class T = void>
	T* get() {
		return static_cast<T*>(ptr_);
	}

	template <class T = void>
	const T* get() const {
		return static_cast<const T*>(ptr_);
	}

private:

	void* ptr_;

};

// Class implementing shared remote storage.
//
// This is basically the same as using a `std::shared_ptr` to store the
// polymorphic object.
//
// TODO:
// - Using `std::shared_ptr` in the implementation is suboptimal, because it
//   reimplements type erasure for the deleter, but we could really reuse our
//   vtable instead.
// #TODO_Caramel: I want to implement my own shared ptr at some point, maybe replace
// std::shared_ptr with it then?
// - For remote storage policies, should it be possible to specify whether the
//   pointed-to storage is const?
template <class Allocator = MallocAllocator>
struct SharedRemoteStorage {
	SharedRemoteStorage() = delete;
	SharedRemoteStorage(const SharedRemoteStorage&) = delete;
	SharedRemoteStorage(SharedRemoteStorage&&) = delete;
	SharedRemoteStorage& operator=(SharedRemoteStorage&&) = delete;
	SharedRemoteStorage& operator=(const SharedRemoteStorage&) = delete;

	template <class T, class RawT = std::decay_t<T>>
	explicit SharedRemoteStorage(T&& t) :
		ptr_{Allocator::template makeShared<RawT>(std::forward<T>(t))}
	{
	}

	template <class VTable>
	SharedRemoteStorage(const SharedRemoteStorage& other, const VTable&) :
		ptr_{other.ptr_}
	{
	}

	template <class VTable>
	SharedRemoteStorage(SharedRemoteStorage&& other, const VTable&) :
		ptr_{std::move(other.ptr_)}
	{
	}

	template <class ThisVTable, class OtherVTable>
	void swap(const ThisVTable&, SharedRemoteStorage& other, const OtherVTable&) {
		using std::swap;
		swap(this->ptr_, other.ptr_);
	}

	template <class VTable>
	void destruct(const VTable&) {
		ptr_.reset();
	}

	template <class T = void>
	T* get() {
		return static_cast<T*>(ptr_.get());
	}

	template <class T = void>
	const T* get() const {
		return static_cast<const T*>(ptr_.get());
	}

private:

	std::shared_ptr<void> ptr_;

};

// Class implementing unconditional storage in a local buffer.
//
// This is like a small buffer optimization, except the behavior is undefined
// when the object can't fit inside the buffer. Since we know the object always
// sits inside the local buffer, we can get rid of a branch when accessing the
// object.
template <
	std::size_t SIZE,
	std::size_t ALIGN = static_cast<std::size_t>(-1)
	>
class LocalStorage {
public:
	LocalStorage() = delete;
	LocalStorage(const LocalStorage&) = delete;
	LocalStorage(LocalStorage&&) = delete;
	LocalStorage& operator=(LocalStorage&&) = delete;
	LocalStorage& operator=(const LocalStorage&) = delete;

	template <class T, class RawT = std::decay_t<T>>
	explicit LocalStorage(T&& t) {
		// TODO: We could also construct the object at an aligned address within
		// the buffer, which would require computing the right address everytime
		// we access the buffer as a T, but would allow more Ts to fit inside it.
		static_assert(canStore(caramel_poly::storageInfoFor<RawT>),
			"caramel_poly::LocalStorage: Trying to construct from an object that won't fit "
			"in the local storage."
			);

		new (&buffer_) RawT(std::forward<T>(t));
	}

	template <class VTable>
	LocalStorage(const LocalStorage& other, const VTable& vtable) {
		assert(canStore(vtable[STORAGE_INFO_LABEL]()) &&
			"caramel_poly::LocalStorage: Trying to copy-construct using a vtable that "
			"describes an object that won't fit in the storage.");

		vtable[COPY_CONSTRUCT_LABEL](this->get(), other.get());
	}

	template <class VTable>
	LocalStorage(LocalStorage&& other, const VTable& vtable) {
		assert(canStore(vtable[STORAGE_INFO_LABEL]()) &&
			"caramel_poly::LocalStorage: Trying to move-construct using a vtable that "
			"describes an object that won't fit in the storage.");

		vtable[MOVE_CONSTRUCT_LABEL](this->get(), other.get());
	}

	template <class ThisVTable, class OtherVTable>
	void swap(const ThisVTable& thisVTable, LocalStorage& other, const OtherVTable& otherVTable) {
		if (this == &other) {
			return;
		}

		// Move `other` into temporary local storage, destructively.
		Buffer tmp;
		otherVTable[MOVE_CONSTRUCT_LABEL](&tmp, &other.buffer_);
		otherVTable[DESTRUCT_LABEL](&other.buffer_);

		// Move `*this` into `other`, destructively.
		thisVTable[MOVE_CONSTRUCT_LABEL](&other.buffer_, &this->buffer_);
		thisVTable[DESTRUCT_LABEL](&this->buffer_);

		// Now, bring `tmp` into `*this`, destructively.
		otherVTable[MOVE_CONSTRUCT_LABEL](&this->buffer_, &tmp);
		otherVTable[DESTRUCT_LABEL](&tmp);
	}

	template <class VTable>
	void destruct(const VTable& vtable) {
		vtable[DESTRUCT_LABEL](&buffer_);
	}

	template <class T = void>
	T* get() {
		return static_cast<T*>(static_cast<void*>(&buffer_));
	}

	template <class T = void>
	const T* get() const {
		return static_cast<const T*>(static_cast<const void*>(&buffer_));
	}

	static constexpr bool canStore(caramel_poly::StorageInfo info) {
		return
			info.size <= sizeof(Buffer) &&
			alignof(Buffer) % info.alignment == 0
			;
	}

private:

	using Buffer = std::aligned_storage_t<
		SIZE,
		ALIGN == static_cast<size_t>(-1) ? alignof(std::aligned_storage_t<SIZE>) : ALIGN
		>;
	
	Buffer buffer_;

};

// Class implementing a non-owning polymorphic reference. Unlike the other
// storage classes, this one does not own the object it holds, and hence it
// does not construct or destruct it. The referenced object must outlive the
// polymorphic storage that references it, otherwise the behavior is undefined.
struct NonOwningStorage {
	NonOwningStorage() = delete;
	NonOwningStorage(const NonOwningStorage&) = delete;
	NonOwningStorage(NonOwningStorage&&) = delete;
	NonOwningStorage& operator=(NonOwningStorage&&) = delete;
	NonOwningStorage& operator=(const NonOwningStorage&) = delete;

	template <class T>
	explicit NonOwningStorage(T& t) :
		ptr_{&t}
	{
	}

	template <class VTable>
	NonOwningStorage(const NonOwningStorage& other, const VTable&) :
		ptr_{other.ptr_}
	{
	}

	template <class VTable>
	NonOwningStorage(NonOwningStorage&& other, const VTable&) :
		ptr_{other.ptr_}
	{
	}

	template <class ThisVTable, class OtherVTable>
	void swap(const ThisVTable&, NonOwningStorage& other, const OtherVTable&) {
		std::swap(this->ptr_, other.ptr_);
	}

	template <class VTable>
	void destruct(const VTable&) {
	}

	template <class T = void>
	T* get() {
		return static_cast<T*>(ptr_);
	}

	template <class T = void>
	const T* get() const {
		return static_cast<const T*>(ptr_);
	}

	static constexpr bool canStore(caramel_poly::StorageInfo) {
		return true;
	}

private:

	void* ptr_;

};

// #TODO_Caramel: dropped fallback storage, as I don't really see the need to have it. Copy it if necessary.

} // namespace caramel_poly

#endif /* CARAMELPOLY_STORAGE_HPP__ */
