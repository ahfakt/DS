#ifndef DS_CONTAINER_HPP
#define DS_CONTAINER_HPP

#include <cstdint>

namespace DS {

/**
 * @brief	Base class for data structures.
 * @class	Container Container.hpp "DS/Container.hpp"
 */
class Container {
protected:
	std::uint64_t mSize{0};

	Container() = default;

	explicit Container(std::uint64_t size) noexcept
			: mSize(size)
	{}

public:
	/**
	 * @return	The number of elements stored in the Container object
	 */
	[[nodiscard]] std::uint64_t
	size() const noexcept
	{ return mSize; }

	/**
	 * @brief	bool conversion operator.
	 * @return	false, if no element is stored in the Container object
	 */
	explicit operator bool() const noexcept
	{ return mSize; }
};//class DS::Container

}//namespace DS

#endif //DS_CONTAINER_HPP
