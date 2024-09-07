#pragma once

#include <Stream/InOut.hpp>

namespace DS::Test {

struct ConstructWithIntInt {
	std::uint64_t a;
	std::uint64_t b;

	ConstructWithIntInt(std::uint64_t a, std::uint64_t b) noexcept
		: a{a}
		, b{b}
	{}

	ConstructWithIntInt() = default;
};//struct DS::Test::ConstructWithIntInt

struct ConstructWithIntIntExtractable : ConstructWithIntInt {
	std::uint64_t c;

	ConstructWithIntIntExtractable(std::uint64_t a, std::uint64_t b) noexcept
		: ConstructWithIntInt(a, b)
		, c{0}
	{}

	ConstructWithIntIntExtractable() = default;

	friend Stream::Input&
	operator>>(Stream::Input& input, ConstructWithIntIntExtractable& constructWithIntIntExtractable)
	{ return input >> constructWithIntIntExtractable.c; }
};//struct DS::Test::ConstructWithIntInt

}//namespace DS::Test