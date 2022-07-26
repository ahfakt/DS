#include "DS/List.h"
#include <cassert>
#include <iostream>

using namespace DS;

struct Aggregate {
	struct {
		int a;
		int b;
	} inner;
	int c;
};

struct AggregateAndDeserialize : Aggregate {
	int d;
	friend Stream::Input&
	operator>>(Stream::Input& input, AggregateAndDeserialize& aggregateAndDeserialize)
	{ return input >> aggregateAndDeserialize.d; }
};

struct Construct {
	int a;
	int b;
	Construct(int a, int b) noexcept : a(a), b(b) {}
};

struct ConstructAndDeserialize : Construct {
	int c;
	ConstructAndDeserialize(int a, int b) noexcept : Construct(a, b) {}

	friend Stream::Input&
	operator>>(Stream::Input& input, ConstructAndDeserialize& constructAndDeserialize)
	{ return input >> constructAndDeserialize.c; }
};

struct StreamConstruct {
	int a;
	int b;
	int c;
	StreamConstruct(Stream::Input& input) : a(Stream::Get<int>(input)), b(Stream::Get<int>(input)), c(Stream::Get<int>(input)) {}
	StreamConstruct() = default;

	friend Stream::Input&
	operator>>(Stream::Input& input, StreamConstruct& streamConstruct)
	{ return input >> streamConstruct.c; }
};

struct StreamIntIntConstruct {
	int a;
	int b;
	int c;
	StreamIntIntConstruct(Stream::Input& input, int a, int b) : a(a), b(b), c(Stream::Get<int>(input)) {}
	StreamIntIntConstruct(int b, int a) : a(a), b(b) {}

	friend Stream::Input&
	operator>>(Stream::Input& input, StreamIntIntConstruct& streamIntIntConstruct)
	{ return input >> streamIntIntConstruct.c; }
};

struct TrivialDefaultConstruct {
	int a;
	int b;
	int c;
};

int main()
{
	{
		Holder<int> h(5);
		assert(*h == 5);
	}

	{
		Holder<Aggregate> h(5, 8, 13);
		assert((h->inner.a == 5 && h->inner.b == 8 && h->c == 13));
	}

	{
		int d = 21;
		Stream::BufferInput bufferInput(&d, sizeof(int));
		Holder<AggregateAndDeserialize> h(bufferInput, 5, 8, 13);
		assert((h->inner.a == 5 && h->inner.b == 8 && h->c == 13 && h->d == 21));
	}

	{
		Holder<Construct> h(5, 8);
		assert((h->a == 5 && h->b == 8));
	}

	{
		int c = 13;
		Stream::BufferInput bufferInput(&c, sizeof(int));
		Holder<ConstructAndDeserialize> h(bufferInput, 5, 8);
		assert((h->a == 5 && h->b == 8 && h->c == 13));
	}

	{
		int abc[] = {5, 8, 13};
		Stream::BufferInput bufferInput(abc, sizeof(abc));
		Holder<StreamConstruct> h(bufferInput);
		assert((h->a == 5 && h->b == 8 && h->c == 13));
	}

	{
		int c = 13;
		Stream::BufferInput bufferInput(&c, sizeof(int));
		Holder<StreamIntIntConstruct> h(bufferInput, 5, 8);
		assert((h->a == 5 && h->b == 8 && h->c == 13));
	}

	{
		int a = 5;
		Stream::BufferInput bufferInput(&a, sizeof(a));
		Holder<int> h(bufferInput);
		assert(*h == 5);
	}

	{
		int abc[] = {5, 8, 13};
		Stream::BufferInput bufferInput(abc, sizeof(abc));
		Holder<TrivialDefaultConstruct> h(bufferInput);
		assert((h->a == 5 && h->b == 8 && h->c == 13));
	}

	{
		char s[] = "string";
		Stream::BufferInput bufferInput(s, sizeof(s));
		Holder<std::string> h(bufferInput);
		assert(*h == s);
	}

	return 0;
}