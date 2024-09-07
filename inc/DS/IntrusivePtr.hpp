#pragma once

#include "RefCounter.hpp"

namespace DS {

template <typename T>
class IntrusivePtr {
	std::uintptr_t mPtr:48;
	std::size_t mOffset:16;

public:
	IntrusivePtr(IntrusivePtr const& other) noexcept;

	IntrusivePtr(IntrusivePtr&& other) noexcept;

	IntrusivePtr&
	operator=(IntrusivePtr other) noexcept;

	explicit
	IntrusivePtr(T* ptr = nullptr, std::size_t offset = 0) noexcept;

	~IntrusivePtr();

	T&
	operator*() const noexcept;

	T*
	operator->() const noexcept;

	operator T*() const noexcept;

	explicit
	operator bool() const noexcept;
};//class IntrusivePtr<T>

}//namespace DS

#include "../../src/DS/IntrusivePtr.tpp"