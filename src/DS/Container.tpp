#pragma once

#include <cstdint>
#include <atomic>

namespace DS {

/**
 * @brief	Base class for data structures.
 * @class	Container Container.hpp "DS/Container.hpp"
 */
template <bool Concurrent = false>
class alignas(Concurrent ? 64 /* std::hardware_destructive_interference_size */ : alignof(std::uint64_t)) Container {
protected:
	std::uint64_t mSize{0};

	Container() = default;

	explicit
	Container(std::uint64_t size) noexcept
			: mSize{size}
	{}

public:
	/**
	 * @return	The number of elements stored in the Container object
	 */
	[[nodiscard]] std::uint64_t
	size() const noexcept
	{ return Concurrent ? std::atomic_ref{mSize}.load(std::memory_order::acquire) : mSize; }

	/**
	 * @brief	bool conversion operator.
	 * @return	false, if no element is stored in the Container object
	 */
	explicit operator bool() const noexcept
	{ return Concurrent ? std::atomic_ref{mSize}.load(std::memory_order::acquire) : mSize; }
};//class DS::Container<Concurrent>

}//namespace DS