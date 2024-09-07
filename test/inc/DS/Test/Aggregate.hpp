#pragma once

#include <Stream/InOut.hpp>

namespace DS::Test {

struct Aggregate {
	struct {
		std::uint64_t a;
		std::uint64_t b;
	} inner;
	std::uint64_t c;

	// Aggregate() = default; // Makes non-aggregate
};//struct DS::Test::Aggregate

struct AggregateExtractable : Aggregate {
	std::uint64_t d;

	// AggregateExtractable() = default; // Makes non-aggregate

	friend Stream::Input&
	operator>>(Stream::Input& input, AggregateExtractable& AggregateExtractable)
	{ return input >> AggregateExtractable.d; }
};//struct DS::Test::Aggregate

}//namespace DS::Test