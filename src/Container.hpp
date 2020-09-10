#ifndef DS_CONTAINER_H
#define DS_CONTAINER_H

#include <cstddef>
#include <new>
#include <type_traits>
#include <utility>
#include <cstdint>
#include <stdexcept>
#include "Stream/Input.h"
#include "Stream/Output.h"

namespace DS {

class Container {
protected:
	std::uint64_t mSize;
	Container(std::uint64_t size = 0) noexcept:
		mSize(size) {}
public:
	std::uint64_t size() const noexcept { return mSize; }
	explicit operator bool() const noexcept { return mSize; }
};//class Container

enum class CONSTNESS : bool { NonConst = false, Const = true };
enum class DIRECTION : bool { Backward = false, Forward = true };
enum class ORDER    : bool { Preorder = false, Postorder = true };
enum class TRAVERSE : bool { DepthFirst = false, BreadthFirst = true };
enum class ADJACENT : bool { Incoming = false, Outgoing = true };

}//namespace DS

#endif //DS_CONTAINER_H
