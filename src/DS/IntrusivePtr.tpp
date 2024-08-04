#pragma once

#include "DS/IntrusivePtr.hpp"

namespace DS {

template <typename T>
IntrusivePtr<T>::IntrusivePtr(IntrusivePtr const& other) noexcept
{
	reinterpret_cast<std::uintptr_t&>(*this) = reinterpret_cast<std::uintptr_t const&>(other);
	if (mPtr)
		reinterpret_cast<RefCounter*>(mPtr - mOffset)->addRef();
}

template <typename T>
IntrusivePtr<T>::IntrusivePtr(IntrusivePtr&& other) noexcept
{ std::swap(reinterpret_cast<std::uintptr_t&>(*this), reinterpret_cast<std::uintptr_t&>(other)); }

template <typename T>
IntrusivePtr<T>&
IntrusivePtr<T>::operator=(IntrusivePtr other) noexcept
{
	std::swap(reinterpret_cast<std::uintptr_t&>(*this), reinterpret_cast<std::uintptr_t&>(other));
	return *this;
}

template <typename T>
IntrusivePtr<T>::IntrusivePtr(T* ptr, std::size_t offset) noexcept
	: mPtr{reinterpret_cast<std::uintptr_t>(ptr)}
	, mOffset{offset}
{}

template <typename T>
IntrusivePtr<T>::~IntrusivePtr()
{
	if (mPtr)
		reinterpret_cast<RefCounter*>(mPtr - mOffset)->subRef();
}

template <typename T>
T&
IntrusivePtr<T>::operator*() const noexcept
{ return *reinterpret_cast<T*>(mPtr); }

template <typename T>
T*
IntrusivePtr<T>::operator->() const noexcept
{ return reinterpret_cast<T*>(mPtr); }

template <typename T>
IntrusivePtr<T>::operator T*() const noexcept
{ return reinterpret_cast<T*>(mPtr); }

template <typename T>
IntrusivePtr<T>::operator bool() const noexcept
{ return mPtr; }

}//namespace DS