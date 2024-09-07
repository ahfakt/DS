#include "DS/List.hpp"
#include "DS/Test/Aggregate.hpp"
#include "DS/Test/ConstructWithIntInt.hpp"
#include "DS/Test/ConstructWithStream.hpp"
#include "DS/Test/ConstructWithStreamIntInt.hpp"
#include "Stream/Buffer.hpp"
#include <cstring>
#include <cassert>

int main()
{
	{ // Primitive deserialize
		std::uint64_t data[]{
			2, // size
			1, 2
		};
		Stream::BufferInput bufferInput(data, sizeof data);

		static_assert(std::is_trivially_default_constructible_v<std::uint64_t>);
		static_assert(std::is_trivially_copyable_v<std::uint64_t>);
		DS::List<std::uint64_t> l(bufferInput);

		auto i = l.begin();
		assert(*i == 1);

		assert(*(++i) == 2);

		assert(++i == l.end());
	}

	{ // Aggregate deserialize
		std::uint64_t data[]{
			2, // size
			1, 2, 3,
			4, 5, 6
		};
		Stream::BufferInput bufferInput(data, sizeof data);

		static_assert(std::is_trivially_default_constructible_v<DS::Test::Aggregate>);
		static_assert(std::is_trivially_copyable_v<DS::Test::Aggregate>);
		DS::List<DS::Test::Aggregate> l(bufferInput);

		auto i = l.begin();
		assert((i->inner.a == 1 && i->inner.b == 2 && i->c == 3));

		++i;
		assert((i->inner.a == 4 && i->inner.b == 5 && i->c == 6));

		assert(++i == l.end());
	}

	{
		std::uint64_t data[]{
			2, // size
			1, 2,
			3, 4
		};
		Stream::BufferInput bufferInput(data, sizeof data);

		static_assert(std::is_trivially_default_constructible_v<DS::Test::ConstructWithIntInt>);
		static_assert(std::is_trivially_copyable_v<DS::Test::ConstructWithIntInt>);
		DS::List<DS::Test::ConstructWithIntInt> l(bufferInput);

		auto i = l.begin();
		assert((i->a == 1 && i->b == 2));

		++i;
		assert((i->a == 3 && i->b == 4));

		assert(++i == l.end());
	}

	{
		std::uint64_t data[]{
			2, // size
			1, 2, 3,
			4, 5, 6
		};
		Stream::BufferInput bufferInput(data, sizeof data);

		static_assert(Stream::Initializable<DS::Test::ConstructWithStream, decltype((bufferInput))>);
		DS::List<DS::Test::ConstructWithStream> l(bufferInput);

		auto i = l.begin();
		assert((i->a == 1 && i->b == 2 && i->c == 3));

		++i;
		assert((i->a == 4 && i->b == 5 && i->c == 6));

		assert(++i == l.end());
	}

	{
		std::uint64_t data[]{
			2, // size
			1, // 4, 5,
			2, // 4, 5
		};
		Stream::BufferInput bufferInput(data, sizeof data);

		static_assert(Stream::Initializable<DS::Test::ConstructWithStreamIntInt, decltype((bufferInput)), std::uint64_t&&, std::uint64_t&&>);
		DS::List<DS::Test::ConstructWithStreamIntInt> l(bufferInput, 4ull, 5ull);

		auto i = l.begin();
		assert((i->a == 1 && i->b == 4 && i->c == 5));

		++i;
		assert((i->a == 2 && i->b == 4 && i->c == 5));

		assert(++i == l.end());
	}

	{
		char data[]{
			2,0,0,0,0,0,0,0, // size
			7,0,0,0,0,0,0,0, 's','t','r','i','n','g','1',
			7,0,0,0,0,0,0,0, 's','t','r','i','n','g','2'
		};
		Stream::BufferInput bufferInput(data, sizeof data);

		static_assert(std::is_default_constructible_v<std::string>);
		static_assert(Stream::HasExtraction<std::string, decltype((bufferInput))>);
		DS::List<std::string> l(bufferInput);

		auto i = l.begin();
		assert(std::strncmp(i->data(), data + 16, 7) == 0);

		++i;
		assert(std::strncmp(i->data(), data + 31, 7) == 0);

		assert(++i == l.end());
	}

	{
		std::uint64_t data[]{
			2, // size
			/* 4, 5, 6, */ 1, 
			/* 4, 5, 6, */ 2,
		};
		Stream::BufferInput bufferInput(data, sizeof data);

		static_assert(Stream::Initializable<DS::Test::AggregateExtractable, std::uint64_t&&, std::uint64_t&&, std::uint64_t&&>);
		static_assert(Stream::HasExtraction<DS::Test::AggregateExtractable, decltype((bufferInput))>);
		DS::List<DS::Test::AggregateExtractable> l(bufferInput, 4ull, 5ull, 6ull);

		auto i = l.begin();
		assert((i->inner.a == 4 && i->inner.b == 5 && i->c == 6 && i->d == 1));

		++i;
		assert((i->inner.a == 4 && i->inner.b == 5 && i->c == 6 && i->d == 2));

		assert(++i == l.end());
	}

	{
		std::uint64_t data[]{
			2, // size
			/* 4, 5, */ 1, 
			/* 4, 5, */ 2,
		};
		Stream::BufferInput bufferInput(data, sizeof data);

		static_assert(Stream::Initializable<DS::Test::ConstructWithIntIntExtractable, std::uint64_t&&, std::uint64_t&&>);
		static_assert(Stream::HasExtraction<DS::Test::ConstructWithIntIntExtractable, decltype((bufferInput))>);
		DS::List<DS::Test::ConstructWithIntIntExtractable> l(bufferInput, 4ull, 5ull);

		auto i = l.begin();
		assert((i->a == 4 && i->b == 5 && i->c == 1));

		++i;
		assert((i->a == 4 && i->b == 5 && i->c == 2));

		assert(++i == l.end());
	}

	return 0;
}