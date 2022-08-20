#ifndef DS_ENODE_TPP
#define DS_ENODE_TPP

#include "Holder.tpp"

namespace DS {

template <typename V, typename E>
struct VNode;

template <typename V, typename E>
struct ENode {
	ENode* left = nullptr;
	ENode* right = nullptr;
	ENode* down = nullptr;
	ENode* up = nullptr;
	VNode<V, E>* in = nullptr;
	VNode<V, E>* out = nullptr;
	Holder<E> val;

	static void*
	operator new(std::size_t, std::size_t valSize)
	{ return ::operator new(Offset(&ENode::val) + valSize); }

	static void
	operator delete(void* ptr)
	{ ::operator delete(ptr); }

	explicit ENode(auto&& ... args)
			: val(std::forward<decltype(args)>(args) ...)
	{}

	~ENode()
	{ val->~E(); }

	void
	unsetIn() noexcept
	{
		(down ? down->up : in->outHead) = up;
		(up ? up->down : in->outTail) = down;
		down = up = nullptr;
		--in->outDegree;
		in = nullptr;
	}

	void
	setIn(VNode<V, E>* v) noexcept
	{
		(in = v)->outTail = ((down = v->outTail) ? v->outTail->up : v->outHead) = this;
		++v->outDegree;
	}

	void
	unsetOut() noexcept
	{
		(left ? left->right : out->inHead) = right;
		(right ? right->left : out->inTail) = left;
		left = right = nullptr;
		--out->inDegree;
		out = nullptr;
	}

	void
	setOut(VNode<V, E>* v) noexcept
	{
		(out = v)->inTail = ((left = v->inTail) ? v->inTail->right : v->inHead) = this;
		++v->inDegree;
	}
};//struct ENode<V, E>

}//namespace DS

#endif //DS_ENODE_TPP
