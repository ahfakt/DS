#ifndef DS_TNODE_HPP
#define DS_TNODE_HPP

#include "Holder.hpp"
#include <StreamFormat/Dot.h>

namespace DS {

template <std::size_t Dim>
struct TNode {
	struct {
		union {
			std::uintptr_t l;
			std::uintptr_t sl:4;
			std::uintptr_t balance:3;
			struct {
				std::uintptr_t isRight:1;
				std::uintptr_t isBalanced:1;
				std::uintptr_t isLeft:1;
				std::uintptr_t u1:1;
			};
		};
		union {
			std::uintptr_t r;
			std::uintptr_t sr:4;
			std::uintptr_t has:3;
			struct {
				std::uintptr_t hasRight:1;
				std::uintptr_t hasValue:1;
				std::uintptr_t hasLeft:1;
				std::uintptr_t u2:1;
			};
		};
	} d[Dim];

	template <typename Node>
	void
	deleteTree()
	{
		if (d[0].hasRight)
			right()->template deleteTree<Node>();
		if (d[0].hasLeft)
			left()->template deleteTree<Node>();
		delete reinterpret_cast<Node*>(this);
	}

	template <std::size_t N = 0>
	[[nodiscard]] std::uint8_t
	state() const noexcept
	{ return d[N].sl | d[N].sr << 4; }

	template <std::size_t N = 0>
	void
	state(std::uint8_t s) noexcept
	{
		d[N].sl = s & 0xF;
		d[N].sr = s >> 4;
	}

	template <std::size_t N = 0, typename Node = TNode>
	Node*
	left() noexcept
	{ return reinterpret_cast<Node*>(d[N].l & ~std::uintptr_t{0xF}); }

	template <std::size_t N = 0, typename Node = TNode>
	Node const*
	left() const noexcept
	{ return const_cast<TNode*>(this)->template left<N, Node>(); }

	template <std::size_t N = 0>
	void
	left(TNode* p) noexcept
	{ d[N].l = reinterpret_cast<std::uintptr_t>(p) | d[N].sl; }

	template <std::size_t N = 0, typename Node = TNode>
	Node*
	right() noexcept
	{ return reinterpret_cast<Node*>(d[N].r & ~std::uintptr_t{0xF}); }

	template <std::size_t N = 0, typename Node = TNode>
	Node const*
	right() const noexcept
	{ return const_cast<TNode*>(this)->template right<N, Node>(); }

	template <std::size_t N = 0>
	void
	right(TNode* p) noexcept
	{ d[N].r = reinterpret_cast<std::uintptr_t>(p) | d[N].sr; }

	template <std::size_t N = 0>
	auto*
	leftRotate() noexcept
	{
		TNode* Y = right<N>();
		if (Y->d[N].hasLeft)
			right<N>(Y->left<N>());
		else {
			d[N].hasRight = false;
			Y->d[N].hasLeft = true;
		}
		Y->left<N>(this);
		return Y;
	}

	template <std::size_t N = 0>
	auto*
	rightRotate() noexcept
	{
		TNode* Y = left<N>();
		if (Y->d[N].hasRight)
			left<N>(Y->right<N>());
		else {
			d[N].hasLeft = false;
			Y->d[N].hasRight = true;
		}
		Y->right<N>(this);
		return Y;
	}

	template <std::size_t N = 0>
	auto*
	ll() noexcept
	{
		d[N].balance = left<N>()->d[N].balance = 2;
		return rightRotate<N>();
	}

	template <std::size_t N = 0>
	auto*
	rr() noexcept
	{
		d[N].balance = right<N>()->d[N].balance = 2;
		return leftRotate<N>();
	}

	template <std::size_t N = 0>
	auto*
	lr() noexcept
	{
		d[N].balance = left<N>()->template right<N>()->d[N].isLeft ? 1 : 2;
		left<N>()->d[N].balance = left<N>()->template right<N>()->d[N].isRight ? 4 : 2;
		left<N>()->template right<N>()->d[N].balance = 2;
		left<N>(left<N>()->template leftRotate<N>());
		return rightRotate<N>();
	}

	template <std::size_t N = 0>
	auto*
	rl() noexcept
	{
		d[N].balance = right<N>()->template left<N>()->d[N].isRight ? 4 : 2;
		right<N>()->d[N].balance = right<N>()->template left<N>()->d[N].isLeft ? 1 : 2;
		right<N>()->template left<N>()->d[N].balance = 2;
		right<N>(right<N>()->template rightRotate<N>());
		return leftRotate<N>();
	}

	template <std::size_t N = 0, typename Node = TNode>
	Node*
	leftMost() noexcept
	{
		TNode* t = this;
		while (t->d[N].hasLeft)
			t = t->left<N>();
		return reinterpret_cast<Node*>(t);
	}

	template <std::size_t N = 0, typename Node = TNode>
	Node const*
	leftMost() const noexcept
	{ return const_cast<TNode*>(this)->template leftMost<N, Node>(); }

	template <std::size_t N = 0, typename Node = TNode>
	Node*
	rightMost() noexcept
	{
		TNode* t = this;
		while (t->d[N].hasRight)
			t = t->right<N>();
		return reinterpret_cast<Node*>(t);
	}

	template <std::size_t N = 0, typename Node = TNode>
	Node const*
	rightMost() const noexcept
	{ return const_cast<TNode*>(this)->template rightMost<N, Node>(); }

	template <std::size_t N = 0, typename Node = TNode>
	Node*
	prev() noexcept
	{ return d[N].hasLeft ? left<N>()->template rightMost<N, Node>() : left<N, Node>(); }

	template <std::size_t N = 0, typename Node = TNode>
	Node const*
	prev() const noexcept
	{ return const_cast<TNode*>(this)->template prev<N, Node>(); }

	template <std::size_t N = 0, typename Node = TNode>
	Node*
	next() noexcept
	{ return d[N].hasRight ? right<N>()->template leftMost<N, Node>() : right<N, Node>(); }

	template <std::size_t N = 0, typename Node = TNode>
	Node const*
	next() const noexcept
	{ return const_cast<TNode*>(this)->template next<N, Node>(); }

	template <std::size_t N>
	TNode*
	attachedToLeft(TNode* t) noexcept
	{
		if (t) {
			if (t == left<N>()) {
				if (d[N].isLeft)
					return left<N>()->d[N].isLeft ? ll<N>() : lr<N>();
				d[N].balance <<= 1;
				if (d[N].isLeft)
					return this;
			} else
				left<N>(t);
		}
		return nullptr;
	}

	template <std::size_t N>
	TNode*
	attachedToRight(TNode* t) noexcept
	{
		if (t) {
			if (t == right<N>()) {
				if (d[N].isRight)
					return right<N>()->d[N].isRight ? rr<N>() : rl<N>();
				d[N].balance >>= 1;
				if (d[N].isRight)
					return this;
			} else
				right<N>(t);
		}
		return nullptr;
	}

	template <std::size_t N>
	auto*
	attachToLeft(TNode* t) noexcept
	{
		t->template left<N>(left<N>());
		t->template right<N>(this);
		t->template state<N>(2);
		d[N].hasLeft = true;
		left<N>(t);
		d[N].balance <<= 1;
		return d[N].isLeft ? this : nullptr;
	}

	template <std::size_t N>
	auto*
	attachToRight(TNode* t) noexcept
	{
		t->template right<N>(right<N>());
		t->template left<N>(this);
		t->template state<N>(2);
		d[N].hasRight = true;
		right<N>(t);
		d[N].balance >>= 1;
		return d[N].isRight ? this : nullptr;
	}

	template <std::size_t N, typename Node = TNode>
	Node*
	swapWithPrev() noexcept
	{
		auto* P = left<N>()->template rightMost<N, Node>();
		right<N>()->template leftMost<N>()->template left<N>(P);

		auto* Pleft = P->template left<N>();
		P->template left<N>(P != left<N>() ? left<N>() : this);
		P->template right<N>(right<N>());
		if (P->d[N].hasLeft) {
			Pleft->template right<N>(this);
			if (P != left<N>())
				Pleft->template left<N>()->template right<N>(this);
		} else {
			if (P != left<N>())
				Pleft->template right<N>(this);
		}
		left<N>(Pleft);
		right<N>(P);
		P->d[N].balance ^= d[N].balance;
		d[N].balance ^= P->d[N].balance;
		P->d[N].balance ^= d[N].balance;
		P->d[N].has ^= d[N].has;
		d[N].has ^= P->d[N].has;
		P->d[N].has ^= d[N].has;
		return P;
	}

	template <std::size_t N, typename Node = TNode>
	Node*
	swapWithNext() noexcept
	{
		auto* S = right<N>()->template leftMost<N, Node>();
		left<N>()->template rightMost<N>()->template right<N>(S);

		auto* Sright = S->template right<N>();
		S->template right<N>(S != right<N>() ? right<N>() : this);
		S->template left<N>(left<N>());
		if (S->d[N].hasRight) {
			Sright->template left<N>(this);
			if (S != right<N>())
				Sright->template right<N>()->template left<N>(this);
		} else {
			if (S != right<N>())
				Sright->template left<N>(this);
		}
		right<N>(Sright);
		left<N>(S);
		S->d[N].balance ^= d[N].balance;
		d[N].balance ^= S->d[N].balance;
		S->d[N].balance ^= d[N].balance;
		S->d[N].has ^= d[N].has;
		d[N].has ^= S->d[N].has;
		S->d[N].has ^= d[N].has;
		return S;
	}

	template <std::size_t N>
	Stream::Format::DotOutput&
	toDot(Stream::Format::DotOutput& dotOutput) const
	{
		dotOutput << "subgraph {\n";
		if (d[N].hasLeft)
			left<N>()->template toDot<N>(dotOutput);
		dotOutput << "c" << N << this << "[style=invis]\n";
		if (d[N].hasRight)
			right<N>()->template toDot<N>(dotOutput);

		dotOutput << N << this << "[gradientangle=" << (d[N].isLeft ? "315" : (d[N].isRight ? "225" : "270")) << "]\n";

		if (d[N].hasLeft)
			dotOutput << N << this << ":sw->" << N << left<N>() << ":n\n";
		else if (left<N>())
			dotOutput << N << this << ":nw->" << N << left<N>() << ":s[style=dotted constraint=false]\n";
		dotOutput << N << this << ":s->c" << N << this << ":n[style=invis weight=100]\n";
		if (d[N].hasRight)
			dotOutput << N << this << ":se->" << N << right<N>() << ":n\n";
		else if (right<N>())
			dotOutput << N << this << ":ne->" << N << right<N>() << ":s[style=dotted constraint=false]\n";
		dotOutput << "}\n";
		return dotOutput;
	}
};//struct TNode<std::size_t>

}//namespace DS

#endif //DS_TNODE_HPP
