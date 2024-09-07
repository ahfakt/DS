#include "../../../src/DS/Holder.tpp"
#include "DS/Test/Aggregate.hpp"
#include "DS/Test/ConstructWithIntInt.hpp"
#include "DS/Test/ConstructWithStream.hpp"
#include "DS/Test/ConstructWithStreamIntInt.hpp"
#include "Stream/Buffer.hpp"
#include <cassert>


int main()
{
	{ // Primitive initialize
		DS::Holder<std::uint64_t> h(5ull);
		assert(*h == 5);
	}

	{ // Aggregate initialize
		DS::Holder<DS::Test::Aggregate> h(5ull, 8ull, 13ull);
		assert((h->inner.a == 5 && h->inner.b == 8 && h->c == 13));
	}

	{
		DS::Holder<DS::Test::ConstructWithIntInt> h(5ull, 8ull);
		assert((h->a == 5 && h->b == 8));
	}

	{
		std::uint64_t abc[]{5, 8, 13};
		Stream::BufferInput bufferInput(abc, sizeof abc);
		DS::Holder<DS::Test::ConstructWithStream> h(bufferInput);
		assert((h->a == 5 && h->b == 8 && h->c == 13));
	}

	{
		std::uint64_t a{5};
		Stream::BufferInput bufferInput(&a, sizeof a);
		DS::Holder<DS::Test::ConstructWithStreamIntInt> h(bufferInput, 8ull, 13ull);
		assert((h->a == 5 && h->b == 8 && h->c == 13));
	}

	return 0;
}
