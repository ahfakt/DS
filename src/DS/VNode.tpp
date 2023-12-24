#pragma once

#include "ENode.tpp"

namespace DS {

template <typename V, typename E>
struct VNode {
	std::uint64_t inDegree{0};
	ENode<V, E>* inHead{nullptr};
	ENode<V, E>* inTail{nullptr};
	std::uint64_t outDegree{0};
	ENode<V, E>* outHead{nullptr};
	ENode<V, E>* outTail{nullptr};
	union {
		std::uint8_t _ = 1;
		std::uint8_t color:3;
		struct {
			bool isWhite:1;
			bool isGray:1;
			bool isBlack:1;
			bool :5;
		};
	};
	Holder<V> val;

	static void*
	operator new(std::size_t, std::size_t valSize)
	{ return ::operator new(Offset(&VNode::val) + valSize); }

	static void
	operator delete(void* ptr)
	{ ::operator delete(ptr); }

	explicit VNode(auto&& ... args)
			: val(std::forward<decltype(args)>(args) ...)
	{}

	~VNode()
	{ val->~V(); }

	void
	unsetIn() noexcept
	{
		while (inTail != inHead) {
			inTail->out = nullptr;
			(inTail = inTail->left)->right->left = nullptr;
			inTail->right = nullptr;
		}
		inHead->out = nullptr;
		inDegree = 0;
		inTail = inHead = nullptr;
	}

	void
	unsetOut() noexcept
	{
		while (outTail != outHead) {
			outTail->in = nullptr;
			(outTail = outTail->down)->up->down = nullptr;
			outTail->up = nullptr;
		}
		outHead->in = nullptr;
		outDegree = 0;
		outTail = outHead = nullptr;
	}
};//struct VNode<V, E>

}//namespace DS
