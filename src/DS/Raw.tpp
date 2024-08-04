#pragma once

#include <cstddef>

namespace DS {

template <typename T>
class alignas(T) Raw {
protected:
	std::byte raw[sizeof(T)];

public:
	T*
	operator->() noexcept
	{ return reinterpret_cast<T*>(raw); }

	T const*
	operator->() const noexcept
	{ return reinterpret_cast<T const*>(raw); }

	T&
	operator*() noexcept
	{ return *reinterpret_cast<T*>(raw); }

	T const&
	operator*() const noexcept
	{ return *reinterpret_cast<T const*>(raw); }

	operator T&() noexcept
	{ return *reinterpret_cast<T*>(raw); }

	operator T const&() const noexcept
	{ return *reinterpret_cast<T const*>(raw); }

	explicit operator T*() noexcept
	{ return reinterpret_cast<T*>(raw); }

	explicit operator T const*() const noexcept
	{ return reinterpret_cast<T const*>(raw); }

	explicit operator void*() noexcept
	{ return raw; }

	explicit operator void const*() const noexcept
	{ return raw; }

	auto
	operator<=>(Raw const& other) const noexcept
	requires requires(T const& a, T const& b) { a <=> b; }
	{ return *reinterpret_cast<T const*>(raw) <=> *reinterpret_cast<T const*>(other.raw); }
};//class DS::Raw<T>

}//namespace DS
