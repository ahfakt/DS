#pragma once

#include <atomic>

namespace DS {

template <typename T>
struct CountedPtr {
	std::uintptr_t ptr:48;
	std::size_t cnt:16;

	CountedPtr(T* ptr) noexcept;

	T&
	operator*() const noexcept;

	T*
	operator->() const noexcept;

	operator T*() const noexcept;

	explicit
	operator bool() const noexcept;

	bool
	operator==(CountedPtr const& other) const noexcept;
};//struct DS::CountedPtr<T>

}//namespace DS

namespace std {

template <typename T>
class atomic<DS::CountedPtr<T>> {
	DS::CountedPtr<T> val;

public:
	constexpr
	atomic() noexcept = default;

	constexpr
	atomic(DS::CountedPtr<T> desired) noexcept;

	atomic(atomic const&) = delete;

	DS::CountedPtr<T>
	operator=(DS::CountedPtr<T> desired) noexcept;

	atomic&
	operator=(atomic const&) = delete;

	[[nodiscard]]
	bool
	is_lock_free() const noexcept;

	void
	store(DS::CountedPtr<T> desired, std::memory_order order = std::memory_order::seq_cst) noexcept;

	DS::CountedPtr<T>
	load(std::memory_order order = std::memory_order::seq_cst) const noexcept;

	operator DS::CountedPtr<T>() const noexcept;

	DS::CountedPtr<T>
	exchange(DS::CountedPtr<T> desired, std::memory_order order = std::memory_order::seq_cst) noexcept;

	bool
	compare_exchange_weak(DS::CountedPtr<T>& expected, DS::CountedPtr<T> desired,
		std::memory_order success, std::memory_order failure) noexcept;

	bool
	compare_exchange_weak(DS::CountedPtr<T>& expected, DS::CountedPtr<T> desired,
		std::memory_order order = std::memory_order::seq_cst) noexcept;

	bool
	compare_exchange_strong(DS::CountedPtr<T>& expected, DS::CountedPtr<T> desired,
		std::memory_order success, std::memory_order failure) noexcept;

	bool
	compare_exchange_strong(DS::CountedPtr<T>& expected, DS::CountedPtr<T> desired,
		std::memory_order order = std::memory_order::seq_cst) noexcept;

	void
	wait(DS::CountedPtr<T> old, std::memory_order order = std::memory_order::seq_cst ) const noexcept;

	void
	notify_one() const noexcept;

	void
	notify_all() const noexcept;

	static inline constexpr bool
	is_always_lock_free{std::atomic<std::uintptr_t>::is_always_lock_free};

	// non-atomic store
	void
	storeExclusively(DS::CountedPtr<T> desired) noexcept;

	// non-atomic load
	DS::CountedPtr<T>
	loadExclusively() const noexcept;

	DS::CountedPtr<T>
	fetchAddCount(std::size_t count, std::memory_order order = std::memory_order::seq_cst) noexcept;

	DS::CountedPtr<T>
	fetchSubCount(std::size_t count, std::memory_order order = std::memory_order::seq_cst) noexcept;
};//class std::atomic<DS::CountedPtr<T>>

}//namespace std

#include "../../src/DS/CountedPtr.tpp"