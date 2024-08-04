#pragma once

#include "DS/CountedPtr.hpp"

namespace DS {

template <typename T>
CountedPtr<T>::CountedPtr(T* ptr) noexcept
{ reinterpret_cast<T*&>(*this) = ptr; }

template <typename T>
T&
CountedPtr<T>::operator*() const noexcept
{ return *reinterpret_cast<T*>(ptr); }

template <typename T>
T*
CountedPtr<T>::operator->() const noexcept
{ return reinterpret_cast<T*>(ptr); }

template <typename T>
CountedPtr<T>::operator T*() const noexcept
{ return reinterpret_cast<T*>(ptr); }

template <typename T>
CountedPtr<T>::operator bool() const noexcept
{ return ptr; }

template <typename T>
bool
CountedPtr<T>::operator==(CountedPtr const& other) const noexcept
{ return ptr == other.ptr; }

}//namespace DS

namespace std {

template <typename T>
constexpr atomic<DS::CountedPtr<T>>::atomic(DS::CountedPtr<T> desired) noexcept
		: val{desired}
{}

template <typename T>
DS::CountedPtr<T>
atomic<DS::CountedPtr<T>>::operator=(DS::CountedPtr<T> desired) noexcept
{
	store(desired);
	return desired;
}

template <typename T>
bool
atomic<DS::CountedPtr<T>>::is_lock_free() const noexcept
{ return true; }

template <typename T>
void
atomic<DS::CountedPtr<T>>::store(DS::CountedPtr<T> desired, std::memory_order order) noexcept
{ std::atomic_ref{val}.store(desired, order); }

template <typename T>
DS::CountedPtr<T>
atomic<DS::CountedPtr<T>>::load(std::memory_order order) const noexcept
{ return std::atomic_ref{val}.load(order); }

template <typename T>
atomic<DS::CountedPtr<T>>::operator DS::CountedPtr<T>() const noexcept
{ return load(); }

template <typename T>
DS::CountedPtr<T>
atomic<DS::CountedPtr<T>>::exchange(DS::CountedPtr<T> desired, std::memory_order order) noexcept
{ return std::atomic_ref{val}.exchange(desired, order); }

template <typename T>
bool
atomic<DS::CountedPtr<T>>::compare_exchange_weak(DS::CountedPtr<T>& expected, DS::CountedPtr<T> desired,
	std::memory_order success, std::memory_order failure) noexcept
{ return std::atomic_ref{val}.compare_exchange_weak(expected, desired, success, failure); }

template <typename T>
bool
atomic<DS::CountedPtr<T>>::compare_exchange_weak(DS::CountedPtr<T>& expected, DS::CountedPtr<T> desired,
	std::memory_order order) noexcept
{ return std::atomic_ref{val}.compare_exchange_weak(expected, desired, order); }

template <typename T>
bool
atomic<DS::CountedPtr<T>>::compare_exchange_strong(DS::CountedPtr<T>& expected, DS::CountedPtr<T> desired,
	std::memory_order success, std::memory_order failure) noexcept
{ return std::atomic_ref{val}.compare_exchange_strong(expected, desired, success, failure); }

template <typename T>
bool
atomic<DS::CountedPtr<T>>::compare_exchange_strong(DS::CountedPtr<T>& expected, DS::CountedPtr<T> desired,
	std::memory_order order) noexcept
{ return std::atomic_ref{val}.compare_exchange_strong(expected, desired, order); }

template <typename T>
void
atomic<DS::CountedPtr<T>>::wait(DS::CountedPtr<T> old, std::memory_order order) const noexcept
{ std::atomic_ref{val}.wait(old, order); }

template <typename T>
void
atomic<DS::CountedPtr<T>>::notify_one() const noexcept
{ std::atomic_ref{val}.notify_one(); }

template <typename T>
void
atomic<DS::CountedPtr<T>>::notify_all() const noexcept
{ std::atomic_ref{val}.notify_all(); }

template <typename T>
void
atomic<DS::CountedPtr<T>>::storeExclusively(DS::CountedPtr<T> desired) noexcept
{ val = desired; }

template <typename T>
DS::CountedPtr<T>
atomic<DS::CountedPtr<T>>::loadExclusively() const noexcept
{ return val; }

template <typename T>
DS::CountedPtr<T>
atomic<DS::CountedPtr<T>>::fetchAddCount(std::size_t count, std::memory_order order) noexcept
{ return reinterpret_cast<T*>(std::atomic_ref{reinterpret_cast<std::uintptr_t&>(val)}.fetch_add(count << 48, order)); }

template <typename T>
DS::CountedPtr<T>
atomic<DS::CountedPtr<T>>::fetchSubCount(std::size_t count, std::memory_order order) noexcept
{ return reinterpret_cast<T*>(std::atomic_ref{reinterpret_cast<std::uintptr_t&>(val)}.fetch_sub(count << 48, order)); }

}//namespace std