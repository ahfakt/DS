#pragma once

#include "DS/RefCounter.hpp"
#include "Holder.tpp"

namespace DS {

template <typename T>
struct CQNode : RefCounter {
	alignas(64) CQNode<T>* next{nullptr};
	alignas(64) Holder<T> val;

	static void*
	operator new(std::size_t, std::size_t const valSize)
	{ return ::operator new(Offset(&CQNode::val) + valSize); }

	// Custom delete operator to keep C++14 from using the global operator delete(void*, size_t),
	// which would result in the compiler error:
	// "exception cleanup for this placement new selects non-placement operator delete"
	static void
	operator delete(void* ptr)
	{ ::operator delete(ptr); }

	explicit CQNode(auto&& ... args)
			: val(std::forward<decltype(args)>(args) ...)
	{}

	~CQNode()
	{ val->~T(); }
};//struct CQNode<T>

}//namespace DS
