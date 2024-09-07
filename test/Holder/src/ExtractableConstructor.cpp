#include "../../../src/DS/Holder.tpp"
#include "DS/Test/Aggregate.hpp"
#include "DS/Test/ConstructWithIntInt.hpp"
#include "Stream/Buffer.hpp"
#include <cstring>
#include <cassert>


int main()
{
	{ // Trivial primitive extract
		std::uint64_t a{5};
		Stream::BufferInput bufferInput(&a, sizeof a);

		static_assert(std::is_trivially_default_constructible_v<std::uint64_t>);
		static_assert(std::is_trivially_copyable_v<std::uint64_t>);
		DS::Holder<std::uint64_t> h(bufferInput);
		assert(*h == 5);
	}

	{ // Trivial aggregate extract
		std::uint64_t abc[]{5, 8, 13};
		Stream::BufferInput bufferInput(abc, sizeof abc);

		static_assert(std::is_trivially_default_constructible_v<DS::Test::Aggregate>);
		static_assert(std::is_trivially_copyable_v<DS::Test::Aggregate>);
		DS::Holder<DS::Test::Aggregate> h(bufferInput);
		assert((h->inner.a == 5 && h->inner.b == 8 && h->c == 13));
	}

	{ // Default initialize then extract
		char data[]{
			6,0,0,0,0,0,0,0,
			's','t','r','i','n','g'};
		Stream::BufferInput bufferInput(data, sizeof data);

		static_assert(std::is_default_constructible_v<std::string>);
		static_assert(Stream::HasExtraction<std::string, decltype((bufferInput))>);
		DS::Holder<std::string> h(bufferInput);
		assert(std::strncmp(h->data(), data + 8, 6));
	}

	{ // Aggregate initialize then extract
		std::uint64_t d{21};
		Stream::BufferInput bufferInput(&d, sizeof d);

		static_assert(Stream::Initializable<DS::Test::AggregateExtractable, std::uint64_t&&, std::uint64_t&&, std::uint64_t&&>);
		static_assert(Stream::HasExtraction<DS::Test::AggregateExtractable, decltype((bufferInput))>);
		DS::Holder<DS::Test::AggregateExtractable> h(bufferInput, 5ull, 8ull, 13ull);
		assert((h->inner.a == 5 && h->inner.b == 8 && h->c == 13 && h->d == 21));
	}

	{ // Construct initialize then extract
		std::uint64_t c{13};
		Stream::BufferInput bufferInput(&c, sizeof c);

		static_assert(Stream::Initializable<DS::Test::ConstructWithIntIntExtractable, std::uint64_t&&, std::uint64_t&&>);
		static_assert(Stream::HasExtraction<DS::Test::ConstructWithIntIntExtractable, decltype((bufferInput))>);
		DS::Holder<DS::Test::ConstructWithIntIntExtractable> h(bufferInput, 5ull, 8ull);
		assert((h->a == 5 && h->b == 8 && h->c == 13));
	}

	return 0;
}
