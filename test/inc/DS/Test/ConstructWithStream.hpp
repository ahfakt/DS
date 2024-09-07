#pragma once

#include <Stream/InOut.hpp>

namespace DS::Test {

struct ConstructWithStream {
	std::uint64_t a;
	std::uint64_t b;
	std::uint64_t c;

	explicit
	ConstructWithStream(Stream::Input& input)
			: a{Stream::Get<std::uint64_t>(input)}
			, b{Stream::Get<std::uint64_t>(input)}
			, c{Stream::Get<std::uint64_t>(input)} {}

	ConstructWithStream() = default;

	friend Stream::Input&
	operator>>(Stream::Input& input, ConstructWithStream&)
	{ return input; }
};//struct DS::Test::ConstructWithStream

}//namespace DS::Test