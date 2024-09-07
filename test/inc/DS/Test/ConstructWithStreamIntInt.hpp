#pragma once

#include <Stream/InOut.hpp>

namespace DS::Test {

struct ConstructWithStreamIntInt {
	std::uint64_t a;
	std::uint64_t b;
	std::uint64_t c;

	ConstructWithStreamIntInt(Stream::Input& input, std::uint64_t b, std::uint64_t c)
			: a{Stream::Get<std::uint64_t>(input)}
			, b{b}
			, c{c} {}

	ConstructWithStreamIntInt(std::uint64_t b, std::uint64_t c)
			: a{0}
			, b{b}
			, c{c} {}

	ConstructWithStreamIntInt() = default;

	friend Stream::Input&
	operator>>(Stream::Input& input, ConstructWithStreamIntInt&)
	{ return input; }
};//struct DS::Test::ConstructWithStreamIntInt

}//namespace DS::Test