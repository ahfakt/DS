#pragma once

#include "DS/Digraph.hpp"

namespace DS {

template <typename V, typename E>
Digraph<V, E>::Digraph(Digraph const& other)
requires std::is_copy_constructible_v<V> && std::is_copy_constructible_v<E>
{
	Map<VNode<V, E> const*, VNode<V, E>*> map;

	if ((mVerticesSize = other.mVerticesSize)) {
		(mVTail = mVHead = ::new LNode<VNode<V, E>>(static_cast<V const&>(static_cast<VNode<V, E> const*>(other.mVHead->val)->val)))->prev = nullptr;
		try {
			map.put(static_cast<VNode<V, E> const*>(other.mVHead->val))
				.set(static_cast<VNode<V, E>*>(mVHead->val));
			LNode<VNode<V, E>> const* src = other.mVHead;
			while ((src = src->next)) {
				(mVTail->next = ::new LNode<VNode<V, E>>(static_cast<V const&>(static_cast<VNode<V, E> const*>(src->val)->val)))->prev = mVTail;
				mVTail = mVTail->next;
				map.put(static_cast<VNode<V, E> const*>(src->val))
					.set(static_cast<VNode<V, E>*>(mVTail->val));
			}
		} catch (...) {
			mVertices.~List();
			throw;
		}
		mVTail->next = nullptr;
	}

	try {
		if ((mEdgesSize = other.mEdgesSize)) {
			(mETail = mEHead = ::new LNode<ENode<V, E>>(static_cast<E const&>(static_cast<ENode<V, E> const*>(other.mEHead->val)->val)))->prev = nullptr;
			if (static_cast<ENode<V, E> const*>(other.mEHead->val)->in)
				static_cast<ENode<V, E>*>(mEHead->val)
					->setIn(map[static_cast<ENode<V, E> const*>(other.mEHead->val)->in]->value);
			if (static_cast<ENode<V, E> const*>(other.mEHead->val)->out)
				static_cast<ENode<V, E>*>(mEHead->val)
					->setOut(map[static_cast<ENode<V, E> const*>(other.mEHead->val)->out]->value);
			LNode<ENode<V, E>> const* src = other.mEHead;
			while ((src = src->next)) {
				try {
					(mETail->next = ::new LNode<ENode<V, E>>(static_cast<E const&>(static_cast<ENode<V, E> const*>(src->val)->val)))->prev = mETail;
					mETail = mETail->next;
				} catch (...) {
					mEdges.~List();
					throw;
				}
				if (static_cast<ENode<V, E> const*>(src->val)->in)
					static_cast<ENode<V, E>*>(mETail->val)
						->setIn(map[static_cast<ENode<V, E> const*>(src->val)->in]->value);
				if (static_cast<ENode<V, E> const*>(src->val)->out)
					static_cast<ENode<V, E>*>(mETail->val)
						->setOut(map[static_cast<ENode<V, E> const*>(src->val)->out]->value);
			}
			mETail->next = nullptr;
		}
	} catch (...) {
		mVertices.~List();
		throw;
	}
}

template <typename V, typename E>
Digraph<V, E>::Digraph(Digraph&& other) noexcept
{ swap(*this, other); }

template <typename V, typename E>
void
swap(Digraph<V, E>& a, Digraph<V, E>& b) noexcept
{
	swap(a.mVertices, b.mVertices);
	swap(a.mEdges, b.mEdges);
}

template <typename V, typename E>
Digraph<V, E>&
Digraph<V, E>::operator=(Digraph value) noexcept
{
	swap(*this, value);
	return *this;
}

template <typename V, typename E>
Digraph<V, E>::Digraph(auto&& ... vArgs, Stream::Input& input, auto&& ... eArgs)
requires
	Stream::Deserializable<V, decltype(input), decltype(vArgs) ...> &&
	Stream::Deserializable<E, decltype(input), decltype(eArgs) ...>
{ deserializeEdges(deserializeVertices(input, std::forward<decltype(vArgs)>(vArgs) ...), input, std::forward<decltype(eArgs)>(eArgs) ...); }

template <typename V, typename E>
template <typename EType, typename ... EArgs>
Digraph<V, E>::Digraph(auto&& ... vArgs, Stream::Input& input, DP::Factory<E, EType, EArgs ...>, auto&& ... eArgs)
requires Stream::Deserializable<V, decltype(input), decltype(vArgs) ...>
{ deserializeEdges(deserializeVertices(input, std::forward<decltype(vArgs)>(vArgs) ...), input, DP::Factory<E, EType, EArgs ...>{}, std::forward<decltype(eArgs)>(eArgs) ...); }

template <typename V, typename E>
template <typename VType, typename ... VArgs>
Digraph<V, E>::Digraph(DP::Factory<V, VType, VArgs ...>, auto&& ... vArgs, Stream::Input& input, auto&& ... eArgs)
requires Stream::Deserializable<E, decltype(input), decltype(eArgs) ...>
{ deserializeEdges(deserializeVertices(input, DP::Factory<V, VType, VArgs ...>{}, std::forward<decltype(vArgs)>(vArgs) ...), input, std::forward<decltype(eArgs)>(eArgs) ...); }

template <typename V, typename E>
template <typename VType, typename ... VArgs, typename EType, typename ... EArgs>
Digraph<V, E>::Digraph(DP::Factory<V, VType, VArgs ...>, auto&& ... vArgs, Stream::Input& input, DP::Factory<E, EType, EArgs ...>, auto&& ... eArgs)
{ deserializeEdges(deserializeVertices(input, DP::Factory<V, VType, VArgs ...>{}, std::forward<decltype(vArgs)>(vArgs) ...), input, DP::Factory<E, EType, EArgs ...>{}, std::forward<decltype(eArgs)>(eArgs) ...); }

template <typename V, typename E>
Stream::Output&
operator<<(Stream::Output& output, Digraph<V, E> const& digraph)
requires Stream::InsertableTo<V, decltype(output)> && Stream::InsertableTo<E, decltype(output)>
{
	Map<VNode<V, E> const*, std::uint64_t> map;

	output << digraph.mVerticesSize;
	for (auto const* lv = digraph.mVHead; lv; lv = lv->next) {
		output << static_cast<V const&>(static_cast<VNode<V, E> const*>(lv->val)->val);
		map.put(static_cast<VNode<V, E> const*>(lv->val)).set(map.size());
	}

	map.put(nullptr).set(0);

	output << digraph.mEdgesSize;
	for (auto const* le = digraph.mEHead; le; le = le->next)
		output << static_cast<E const&>(static_cast<ENode<V, E> const*>(le->val)->val)
			<< map[static_cast<ENode<V, E> const*>(le->val)->in]->value
			<< map[static_cast<ENode<V, E> const*>(le->val)->out]->value;

	return output;
}

template <typename V, typename E>
Digraph<V, E>::~Digraph()
{
	mEdges.~List();
	mVertices.~List();
}

template <typename V, typename E>
Vector<VNode<V, E>*>
Digraph<V, E>::deserializeVertices(Stream::Input& input, auto&& ... vArgs)
requires Stream::Deserializable<V, decltype(input), decltype(vArgs) ...>
{
	mVerticesSize = Stream::Get<std::uint64_t>(input);
	Vector<VNode<V, E>*> vs(mVerticesSize + 1);
	vs.pushBack(nullptr);

	if (mVerticesSize) {
		(mVTail = mVHead = ::new LNode<VNode<V, E>>(input, std::forward<decltype(vArgs)>(vArgs) ...))->prev = nullptr;
		vs.pushBack(static_cast<VNode<V, E>*>(mVHead->val));
		std::uint64_t size = mVerticesSize;
		while (--size) {
			try {
				(mVTail->next = ::new LNode<VNode<V, E>>(input, std::forward<decltype(vArgs)>(vArgs) ...))->prev = mVTail;
				mVTail = mVTail->next;
			} catch (...) {
				mVertices.~List();
				throw;
			}
			vs.pushBack(static_cast<VNode<V, E>*>(mVTail->val));
		}
		mVTail->next = nullptr;
	}

	return vs;
}

template <typename V, typename E>
template <typename VType, typename ... VArgs>
Vector<VNode<V, E>*>
Digraph<V, E>::deserializeVertices(Stream::Input& input, DP::Factory<V, VType, VArgs ...>, auto&& ... vArgs)
{
	using vseq = std::make_index_sequence<sizeof...(VArgs) - sizeof...(vArgs)>;
	mVerticesSize = Stream::Get<std::uint64_t>(input);
	Vector<VNode<V, E>*> vs(mVerticesSize + 1);
	vs.pushBack(nullptr);

	if (mVerticesSize) {
		{
			auto const& vCreateInfo = DP::Factory<V, VType, VArgs ...>::GetCreateInfo(Stream::Get<VType>(input));
			(mVTail = mVHead = new(Offset(&VNode<V, E>::val) + vCreateInfo.size) LNode<VNode<V, E>>(vCreateInfo, input, vseq{}, std::forward<decltype(vArgs)>(vArgs) ...))->prev = nullptr;
		}
		vs.pushBack(static_cast<VNode<V, E>*>(mVHead->val));
		std::uint64_t size = mVerticesSize;
		while (--size) {
			try {
				auto const& vCreateInfo = DP::Factory<V, VType, VArgs ...>::GetCreateInfo(Stream::Get<VType>(input));
				(mVTail->next = new(Offset(&VNode<V, E>::val) + vCreateInfo.size) LNode<VNode<V, E>>(vCreateInfo, input, vseq{}, std::forward<decltype(vArgs)>(vArgs) ...))->prev = mVTail;
				mVTail = mVTail->next;
			} catch (...) {
				mVertices.~List();
				throw;
			}
			vs.pushBack(static_cast<VNode<V, E>*>(mVTail->val));
		}
		mVTail->next = nullptr;
	}

	return vs;
}

template <typename V, typename E>
void
Digraph<V, E>::deserializeEdges(Vector<VNode<V, E>*> vs, Stream::Input& input, auto&& ... eArgs)
requires Stream::Deserializable<E, decltype(input), decltype(eArgs) ...>
{
	try {
		if ((mEdgesSize = Stream::Get<std::uint64_t>(input))) {
			(mETail = mEHead = ::new LNode<ENode<V, E>>(input, std::forward<decltype(eArgs)>(eArgs) ...))->prev = nullptr;
			try {
				static_cast<ENode<V, E>*>(mEHead->val)->setIn(vs[Stream::Get<std::uint64_t>(input)]);
				static_cast<ENode<V, E>*>(mEHead->val)->setOut(vs[Stream::Get<std::uint64_t>(input)]);
				std::uint64_t size = mEdgesSize;
				while (--size) {
					(mETail->next = ::new LNode<ENode<V, E>>(input, std::forward<decltype(eArgs)>(eArgs) ...))->prev = mETail;
					mETail = mETail->next;
					static_cast<ENode<V, E>*>(mETail->val)->setIn(vs[Stream::Get<std::uint64_t>(input)]);
					static_cast<ENode<V, E>*>(mETail->val)->setOut(vs[Stream::Get<std::uint64_t>(input)]);
				}
			} catch (...) {
				mEdges.~List();
				throw;
			}
			mETail->next = nullptr;
		}
	} catch (...) {
		mVertices.~List();
		throw;
	}
}

template <typename V, typename E>
template <typename EType, typename ... EArgs>
void
Digraph<V, E>::deserializeEdges(Vector<VNode<V, E>*> vs, Stream::Input& input, DP::Factory<E, EType, EArgs ...>, auto&& ... eArgs)
{
	using eseq = std::make_index_sequence<sizeof...(EArgs) - sizeof...(eArgs)>;
	try {
		if ((mEdgesSize = Stream::Get<std::uint64_t>(input))) {
			{
				auto const& eCreateInfo = DP::Factory<E, EType, EArgs ...>::GetCreateInfo(Stream::Get<EType>(input));
				(mETail = mEHead = new(Offset(&ENode<V, E>::val) + eCreateInfo.size) LNode<ENode<V, E>>(eCreateInfo, input, eseq{}, std::forward<decltype(eArgs)>(eArgs) ...))->prev = nullptr;
			}
			try {
				static_cast<ENode<V, E>*>(mEHead->val)->setIn(vs[Stream::Get<std::uint64_t>(input)]);
				static_cast<ENode<V, E>*>(mEHead->val)->setOut(vs[Stream::Get<std::uint64_t>(input)]);
				std::uint64_t size = mEdgesSize;
				while (--size) {
					auto const& eCreateInfo = DP::Factory<E, EType, EArgs ...>::GetCreateInfo(Stream::Get<EType>(input));
					(mETail->next = new(Offset(&ENode<V, E>::val) + eCreateInfo.size) LNode<ENode<V, E>>(eCreateInfo, input, eseq{}, std::forward<decltype(eArgs)>(eArgs) ...))->prev = mETail;
					mETail = mETail->next;
					static_cast<ENode<V, E>*>(mETail->val)->setIn(vs[Stream::Get<std::uint64_t>(input)]);
					static_cast<ENode<V, E>*>(mETail->val)->setOut(vs[Stream::Get<std::uint64_t>(input)]);
				}
			} catch (...) {
				mEdges.~List();
				throw;
			}
			mETail->next = nullptr;
		}
	} catch (...) {
		mVertices.~List();
		throw;
	}
}

template <typename V, typename E>
class Digraph<V, E>::VDescriptor<Constness::NCONST>
Digraph<V, E>::addVertex(auto&& ... vArgs)
{
	mVertices.pushFront(std::forward<decltype(vArgs)>(vArgs) ...);
	return static_cast<VNode<V, E>*>(mVHead->val);
}

template <typename V, typename E>
template <Derived<V> DV>
class Digraph<V, E>::VDescriptor<Constness::NCONST>
Digraph<V, E>::addVertex(auto&& ... dvArgs)
{
	reinterpret_cast<List<VNode<DV, E>>&>(mVertices).pushFront(std::forward<decltype(dvArgs)>(dvArgs) ...);
	return static_cast<VNode<V, E>*>(mVHead->val);
}

template <typename V, typename E>
template <typename ... VArgs>
class Digraph<V, E>::VDescriptor<Constness::NCONST>
Digraph<V, E>::addVertex(DP::CreateInfo<V, VArgs ...> const& vCreateInfo, auto&& ... vArgs)
{
	mVertices.pushFront(new(Offset(&VNode<V, E>::val) + vCreateInfo.size)
		LNode<VNode<V, E>>(vCreateInfo, std::forward<decltype(vArgs)>(vArgs) ...));
	return static_cast<VNode<V, E>*>(mVHead->val);
}

template <typename V, typename E>
class Digraph<V, E>::EDescriptor<Constness::NCONST>
Digraph<V, E>::addEdge(auto&& ... eArgs)
{
	mEdges.pushFront(std::forward<decltype(eArgs)>(eArgs) ...);
	return static_cast<ENode<V, E>*>(mEHead->val);
}

template <typename V, typename E>
template <Derived<E> DE>
class Digraph<V, E>::EDescriptor<Constness::NCONST>
Digraph<V, E>::addEdge(auto&& ... deArgs)
{
	reinterpret_cast<List<ENode<V, DE>>&>(mEdges).pushFront(std::forward<decltype(deArgs)>(deArgs) ...);
	return static_cast<ENode<V, E>*>(mEHead->val);
}

template <typename V, typename E>
template <typename ... EArgs>
class Digraph<V, E>::EDescriptor<Constness::NCONST>
Digraph<V, E>::addEdge(DP::CreateInfo<E, EArgs ...> const& eCreateInfo, auto&& ... eArgs)
{
	mEdges.pushFront(new(Offset(&ENode<V, E>::val) + eCreateInfo.size)
		LNode<ENode<V, E>>(eCreateInfo, std::forward<decltype(eArgs)>(eArgs) ...));
	return static_cast<ENode<V, E>*>(mEHead->val);
}

template <typename V, typename E>
template <Constness c>
Digraph<V, E>&
Digraph<V, E>::remove(VDescriptor<c> v) noexcept
{
	if (v.hasIn())
		v.unsetIn();
	if (v.hasOut())
		v.unsetOut();
	if (v.pos == static_cast<VNode<V, E>*>(mVPivot->val))
		mVPivot = mVPivot->next;
	mVertices.remove(ToParent(&LNode<VNode<V, E>>::val, v.pos));
	return *this;
}

template <typename V, typename E>
template <Constness c>
Digraph<V, E>&
Digraph<V, E>::remove(EDescriptor<c> e) noexcept
{
	if (e.hasIn())
		e.unsetIn();
	if (e.hasOut())
		e.unsetOut();
	if (e.pos == static_cast<ENode<V, E>*>(mEPivot->val))
		mEPivot = mEPivot->next;
	mEdges.remove(ToParent(&LNode<ENode<V, E>>::val, e.pos));
	return *this;
}

template <typename V, typename E>
template <Constness c>
Digraph<V, E>&
Digraph<V, E>::prepareTraverse(VDescriptor<c> v) noexcept
{
	if (v.pos->isWhite) {
		moveBack(ToParent(v.pos, &LNode<VNode<V, E>>::val), mVHead, mVTail);
		v.pos->color = 2; // mark as gray
	}
	if (!mVPivot)
		mVPivot = ToParent(v.pos, &LNode<VNode<V, E>>::val);
	return *this;
}

template <typename V, typename E>
template <Traverse t, Order o, Direction d>
Digraph<V, E>&
Digraph<V, E>::prepareTraverse() noexcept
{
	if constexpr (o == Order::PREORDER) {
		for (auto* lv = mVPivot; lv; lv = lv->next) {
			lv->val->color = 4; // Black
			if constexpr (t == Traverse::BREADTH_FIRST) {
				if constexpr (d == Direction::FORWARD) {
					for (auto* e = lv->val->outHead; e; e = e->up)
						if (e->out && e->out->isWhite) {
							moveBack(ToParent(e->out, &LNode<VNode<V, E>>::val), mVHead, mVTail);
							e->out->color = 2; // Gray
						}
				} else {
					for (auto* e = lv->val->inHead; e; e = e->right)
						if (e->in && e->in->isWhite) {
							moveBack(ToParent(e->in, &LNode<VNode<V, E>>::val), mVHead, mVTail);
							e->in->color = 2; // Gray
						}
				}
			} else {
				if constexpr (d == Direction::FORWARD) {
					for (auto* e = lv->val->outTail; e; e = e->down)
						if (e->out && !e->out->isBlack) {
							moveAfter(lv, ToParent(e->out, &LNode<VNode<V, E>>::val), mVHead, mVTail);
							e->out->color = 2; // Gray
						}
				} else {
					for (auto* e = lv->val->inTail; e; e = e->left)
						if (e->in && !e->in->isBlack) {
							moveAfter(lv, ToParent(e->in, &LNode<VNode<V, E>>::val), mVHead, mVTail);
							e->in->color = 2; // Gray
						}
				}
			}
		}
	} else {
		if (mVPivot) {
			// move the traverse list to the front
			(mVHead->prev = mVTail)->next = mVHead;
			mVTail = mVPivot->prev;
			mVPivot = mVHead->prev;
			mVHead = mVTail->next;
			mVHead->prev = mVTail->next = nullptr;

			if constexpr (t == Traverse::BREADTH_FIRST) {
				do {
					auto* head = mVHead;
					auto* lv = mVHead;
					do {
						lv->val->color = 4; // Black
						if constexpr (d == Direction::FORWARD) {
							for (auto* e = lv->val->outHead; e; e = e->up)
								if (e->out && e->out->isWhite) {
									moveBefore(head, ToParent(e->out, &LNode<VNode<V, E>>::val), mVHead, mVTail);
									e->out->color = 2; // Gray
								}
						} else {
							for (auto* e = lv->val->inHead; e; e = e->right)
								if (e->in && e->in->isWhite) {
									moveBefore(head, ToParent(e->in, &LNode<VNode<V, E>>::val), mVHead, mVTail);
									e->in->color = 2; // Gray
								}
						}
						lv = lv->next;
					} while (lv && lv->val->isGray); // add all neighbours of gray nodes to the front
				} while (mVHead->val->isGray); // there is gray nodes at the front
			} else {
				mVPivot = nullptr;
				auto* lv = mVHead;
				do {
					do {
						lv->val->color = 4; // Black
						if constexpr (d == Direction::FORWARD) {
							for (auto* e = lv->val->outTail; e; e = e->down)
								if (e->out && !e->out->isBlack) {
									moveAfter(mVPivot, ToParent(e->out, &LNode<VNode<V, E>>::val), mVHead, mVTail);
									e->out->color = 2; // Gray
								}
						} else {
							for (auto* e = lv->val->inTail; e; e = e->left)
								if (e->in && !e->in->isBlack) {
									moveAfter(mVPivot, ToParent(e->in, &LNode<VNode<V, E>>::val), mVHead, mVTail);
									e->in->color = 2; // Gray
								}
						}
						lv = mVPivot ? mVPivot->next : mVHead;
					} while (lv->val->isGray); // expand all gray nodes recursively

					do {
						mVPivot = lv;
						lv = lv->next;
					} while (lv && lv->val->isBlack); // skip black nodes
				} while (lv && lv->val->isGray);
			}

			// move the traverse list to the back again
			(mVHead->prev = mVTail)->next = mVHead;
			mVHead = mVPivot->next;
			mVPivot = mVTail->next;
			mVTail = mVHead->prev;
			mVHead->prev = mVTail->next = nullptr;
		}
	}
	return *this;
}

template <typename V, typename E>
Digraph<V, E>&
Digraph<V, E>::resetTraverse() noexcept
{
	if (mVPivot) {
		auto* lv = mVPivot;
		do {
			lv->val->color = 1;
			lv = lv->next;
		} while (lv);
		mVPivot = nullptr;
	}
	return *this;
}

template <typename V, typename E>
class Digraph<V, E>::VView<Constness::NCONST>
Digraph<V, E>::getVertexView() noexcept
{ return {mVPivot ? mVPivot : mVHead, mVTail}; }

template <typename V, typename E>
class Digraph<V, E>::EView<Constness::NCONST>
Digraph<V, E>::getEdgeView() noexcept
{ return {mEPivot ? mEPivot : mEHead, mETail}; }

template <typename V, typename E>
template <Constness c>
Digraph<V, E>::VDescriptor<c>::VDescriptor(VNode<V, E>* pos) noexcept
		: pos(pos)
{}

template <typename V, typename E>
template <Constness c>
template <Constness oc>
Digraph<V, E>::VDescriptor<c>::VDescriptor(VDescriptor<oc> const& other) noexcept
requires ConstCompat<c, oc>
		: pos(other.pos)
{}

template <typename V, typename E>
template <Constness c>
template <Constness oc>
class Digraph<V, E>::VDescriptor<c>&
Digraph<V, E>::VDescriptor<c>::operator=(VDescriptor<oc> const& other) noexcept
requires ConstCompat<c, oc>
{
	pos = other.pos;
	return *this;
}

template <typename V, typename E>
template <Constness c>
bool
Digraph<V, E>::VDescriptor<c>::hasIn() const noexcept
{ return pos->inDegree; }

template <typename V, typename E>
template <Constness c>
bool
Digraph<V, E>::VDescriptor<c>::hasOut() const noexcept
{ return pos->outDegree; }

template <typename V, typename E>
template <Constness c>
std::uint64_t
Digraph<V, E>::VDescriptor<c>::getInDegree() const noexcept
{ return pos->inDegree; }

template <typename V, typename E>
template <Constness c>
std::uint64_t
Digraph<V, E>::VDescriptor<c>::getOutDegree() const noexcept
{ return pos->outDegree; }

template <typename V, typename E>
template <Constness c>
template <Direction d>
class Digraph<V, E>::AdjacencyList<d, c>
Digraph<V, E>::VDescriptor<c>::getAdjacencyList() const noexcept
{ return pos; }

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::VDescriptor<c> const&
Digraph<V, E>::VDescriptor<c>::unsetIn() const noexcept
requires (c == Constness::NCONST)
{
	pos->unsetIn();
	return *this;
}

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::VDescriptor<c> const&
Digraph<V, E>::VDescriptor<c>::unsetOut() const noexcept
requires (c == Constness::NCONST)
{
	pos->unsetOut();
	return *this;
}

template <typename V, typename E>
template <Constness c>
TConstness<V, c>&
Digraph<V, E>::VDescriptor<c>::operator*() const noexcept
{ return static_cast<TConstness<V, c>&>(pos->val); }

template <typename V, typename E>
template <Constness c>
TConstness<V, c>*
Digraph<V, E>::VDescriptor<c>::operator->() const noexcept
{ return static_cast<TConstness<V, c>*>(pos->val); }

template <typename V, typename E>
template <Constness c>
template <Constness oc>
bool
Digraph<V, E>::VDescriptor<c>::operator==(VDescriptor<oc> const& other) const noexcept
{ return pos == other.pos; }

template <typename V, typename E>
template <Constness c>
Digraph<V, E>::VDescriptor<c>::operator bool() const noexcept
{ return pos; }

template <typename V, typename E>
template <Constness c>
Digraph<V, E>::EDescriptor<c>::EDescriptor(ENode<V, E>* pos) noexcept
		: pos(pos)
{}

template <typename V, typename E>
template <Constness c>
template <Constness oc>
Digraph<V, E>::EDescriptor<c>::EDescriptor(EDescriptor<oc> const& other) noexcept
requires ConstCompat<c, oc>
		: pos(other.pos)
{}

template <typename V, typename E>
template <Constness c>
template <Constness oc>
class Digraph<V, E>::EDescriptor<c>&
Digraph<V, E>::EDescriptor<c>::operator=(EDescriptor<oc> const& other) noexcept
requires ConstCompat<c, oc>
{
	pos = other.pos;
	return *this;
}

template <typename V, typename E>
template <Constness c>
bool
Digraph<V, E>::EDescriptor<c>::hasIn() const noexcept
{ return pos->in; }

template <typename V, typename E>
template <Constness c>
bool
Digraph<V, E>::EDescriptor<c>::hasOut() const noexcept
{ return pos->out; }

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::VDescriptor<c>
Digraph<V, E>::EDescriptor<c>::getIn() const noexcept
{ return pos->in; }

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::VDescriptor<c>
Digraph<V, E>::EDescriptor<c>::getOut() const noexcept
{ return pos->out; }

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::EDescriptor<c> const&
Digraph<V, E>::EDescriptor<c>::unsetIn() const noexcept
requires (c == Constness::NCONST)
{
	pos->unsetIn();
	return *this;
}

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::EDescriptor<c> const&
Digraph<V, E>::EDescriptor<c>::unsetOut() const noexcept
requires (c == Constness::NCONST)
{
	pos->unsetOut();
	return *this;
}

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::EDescriptor<c> const&
Digraph<V, E>::EDescriptor<c>::setIn(VDescriptor<c> v) const noexcept
requires (c == Constness::NCONST)
{
	if (pos->in)
		pos->unsetIn();
	pos->setIn(v.pos);
	return *this;
}

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::EDescriptor<c> const&
Digraph<V, E>::EDescriptor<c>::setOut(VDescriptor<c> v) const noexcept
requires (c == Constness::NCONST)
{
	if (pos->out)
		pos->unsetOut();
	pos->setOut(v.pos);
	return *this;
}

template <typename V, typename E>
template <Constness c>
TConstness<E, c>&
Digraph<V, E>::EDescriptor<c>::operator*() const noexcept
{ return static_cast<TConstness<E, c>&>(pos->val); }

template <typename V, typename E>
template <Constness c>
TConstness<E, c>*
Digraph<V, E>::EDescriptor<c>::operator->() const noexcept
{ return static_cast<TConstness<E, c>*>(pos->val); }

template <typename V, typename E>
template <Constness c>
template <Constness oc>
bool
Digraph<V, E>::EDescriptor<c>::operator==(EDescriptor<oc> const& other) const noexcept
{ return pos == other.pos; }

template <typename V, typename E>
template <Constness c>
Digraph<V, E>::EDescriptor<c>::operator bool() const noexcept
{ return pos; }

template <typename V, typename E>
template <Constness c>
Digraph<V, E>::VView<c>::VView(LNode<VNode<V, E>>* head, LNode<VNode<V, E>>* tail) noexcept
		: mHead(head)
		, mTail(tail)
{}

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::VView<c>::Iterator<Direction::FORWARD, Constness::NCONST>
Digraph<V, E>::VView<c>::begin() noexcept
requires (c == Constness::NCONST)
{ return static_cast<VNode<V, E>*>(mHead->val); }

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::VView<c>::Iterator<Direction::FORWARD, Constness::CONST>
Digraph<V, E>::VView<c>::begin() const noexcept
{ return static_cast<VNode<V, E>*>(mHead->val); }

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::VView<c>::Iterator<Direction::FORWARD, Constness::NCONST>
Digraph<V, E>::VView<c>::end() noexcept
requires (c == Constness::NCONST)
{ return nullptr; }

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::VView<c>::Iterator<Direction::FORWARD, Constness::CONST>
Digraph<V, E>::VView<c>::end() const noexcept
{ return nullptr; }

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::VView<c>::Iterator<Direction::BACKWARD, Constness::NCONST>
Digraph<V, E>::VView<c>::rbegin() noexcept
requires (c == Constness::NCONST)
{ return static_cast<VNode<V, E>*>(mTail->val); }

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::VView<c>::Iterator<Direction::BACKWARD, Constness::CONST>
Digraph<V, E>::VView<c>::rbegin() const noexcept
{ return static_cast<VNode<V, E>*>(mTail->val); }

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::VView<c>::Iterator<Direction::BACKWARD, Constness::NCONST>
Digraph<V, E>::VView<c>::rend() noexcept
requires (c == Constness::NCONST)
{ return nullptr; }

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::VView<c>::Iterator<Direction::BACKWARD, Constness::CONST>
Digraph<V, E>::VView<c>::rend() const noexcept
{ return nullptr; }

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::VView<c>::Iterator<Direction::FORWARD, Constness::CONST>
Digraph<V, E>::VView<c>::cbegin() const noexcept
{ return static_cast<VNode<V, E>*>(mHead->val); }

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::VView<c>::Iterator<Direction::FORWARD, Constness::CONST>
Digraph<V, E>::VView<c>::cend() const noexcept
{ return nullptr; }

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::VView<c>::Iterator<Direction::BACKWARD, Constness::CONST>
Digraph<V, E>::VView<c>::crbegin() const noexcept
{ return static_cast<VNode<V, E>*>(mTail->val); }

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::VView<c>::Iterator<Direction::BACKWARD, Constness::CONST>
Digraph<V, E>::VView<c>::crend() const noexcept
{ return nullptr; }

template <typename V, typename E>
template <Constness vvc>
template <Direction d, Constness c>
template <Constness oc>
class Digraph<V, E>::VView<vvc>::Iterator<d, c>&
Digraph<V, E>::VView<vvc>::Iterator<d, c>::operator=(VDescriptor<oc> const& other) noexcept
requires ConstCompat<c, oc>
{
	this->pos = other.pos;
	return *this;
}

template <typename V, typename E>
template <Constness vvc>
template <Direction d, Constness c>
class Digraph<V, E>::VView<vvc>::Iterator<d, c>&
Digraph<V, E>::VView<vvc>::Iterator<d, c>::operator++() noexcept
{
	auto* lv = d == Direction::FORWARD
		? ToParent(this->pos, &LNode<VNode<V, E>>::val)->next
		: ToParent(this->pos, &LNode<VNode<V, E>>::val)->prev;
	this->pos = lv ? static_cast<VNode<V, E>*>(lv->val) : nullptr;
	return *this;
}

template <typename V, typename E>
template <Constness vvc>
template <Direction d, Constness c>
class Digraph<V, E>::VView<vvc>::Iterator<d, c>
Digraph<V, E>::VView<vvc>::Iterator<d, c>::operator++(int) noexcept
{
	auto* r = this->pos;
	auto* lv = d == Direction::FORWARD
		? ToParent(this->pos, &LNode<VNode<V, E>>::val)->next
		: ToParent(this->pos, &LNode<VNode<V, E>>::val)->prev;
	this->pos = lv ? static_cast<VNode<V, E>*>(lv->val) : nullptr;
	return r;
}

template <typename V, typename E>
template <Constness vvc>
template <Direction d, Constness c>
class Digraph<V, E>::VView<vvc>::Iterator<d, c>&
Digraph<V, E>::VView<vvc>::Iterator<d, c>::operator--() noexcept
{
	auto* lv = d == Direction::FORWARD
		? ToParent(this->pos, &LNode<VNode<V, E>>::val)->prev
		: ToParent(this->pos, &LNode<VNode<V, E>>::val)->next;
	this->pos = lv ? static_cast<VNode<V, E>*>(lv->val) : nullptr;
	return *this;
}

template <typename V, typename E>
template <Constness vvc>
template <Direction d, Constness c>
class Digraph<V, E>::VView<vvc>::Iterator<d, c>
Digraph<V, E>::VView<vvc>::Iterator<d, c>::operator--(int) noexcept
{
	auto* r = this->pos;
	auto* lv = d == Direction::FORWARD
		? ToParent(this->pos, &LNode<VNode<V, E>>::val)->prev
		: ToParent(this->pos, &LNode<VNode<V, E>>::val)->next;
	this->pos = lv ? static_cast<VNode<V, E>*>(lv->val) : nullptr;
	return r;
}

template <typename V, typename E>
template <Constness vvc>
template <Direction d, Constness c>
class Digraph<V, E>::VDescriptor<c> const&
Digraph<V, E>::VView<vvc>::Iterator<d, c>::operator*() const noexcept
{ return *this; }

template <typename V, typename E>
template <Constness vvc>
template <Direction d, Constness c>
class Digraph<V, E>::VDescriptor<c> const*
Digraph<V, E>::VView<vvc>::Iterator<d, c>::operator->() const noexcept
{ return this; }

template <typename V, typename E>
template <Constness c>
Digraph<V, E>::EView<c>::EView(LNode<ENode<V, E>>* head, LNode<ENode<V, E>>* tail) noexcept
		: mHead(head)
		, mTail(tail)
{}

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::EView<c>::Iterator<Direction::FORWARD, Constness::NCONST>
Digraph<V, E>::EView<c>::begin() noexcept
requires (c == Constness::NCONST)
{ return static_cast<ENode<V, E>*>(mHead->val); }

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::EView<c>::Iterator<Direction::FORWARD, Constness::CONST>
Digraph<V, E>::EView<c>::begin() const noexcept
{ return static_cast<ENode<V, E>*>(mHead->val); }

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::EView<c>::Iterator<Direction::FORWARD, Constness::NCONST>
Digraph<V, E>::EView<c>::end() noexcept
requires (c == Constness::NCONST)
{ return nullptr; }

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::EView<c>::Iterator<Direction::FORWARD, Constness::CONST>
Digraph<V, E>::EView<c>::end() const noexcept
{ return nullptr; }

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::EView<c>::Iterator<Direction::BACKWARD, Constness::NCONST>
Digraph<V, E>::EView<c>::rbegin() noexcept
requires (c == Constness::NCONST)
{ return static_cast<ENode<V, E>*>(mTail->val); }

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::EView<c>::Iterator<Direction::BACKWARD, Constness::CONST>
Digraph<V, E>::EView<c>::rbegin() const noexcept
{ return static_cast<ENode<V, E>*>(mTail->val); }

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::EView<c>::Iterator<Direction::BACKWARD, Constness::NCONST>
Digraph<V, E>::EView<c>::rend() noexcept
requires (c == Constness::NCONST)
{ return nullptr; }

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::EView<c>::Iterator<Direction::BACKWARD, Constness::CONST>
Digraph<V, E>::EView<c>::rend() const noexcept
{ return nullptr; }

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::EView<c>::Iterator<Direction::FORWARD, Constness::CONST>
Digraph<V, E>::EView<c>::cbegin() const noexcept
{ return static_cast<ENode<V, E>*>(mHead->val); }

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::EView<c>::Iterator<Direction::FORWARD, Constness::CONST>
Digraph<V, E>::EView<c>::cend() const noexcept
{ return nullptr; }

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::EView<c>::Iterator<Direction::BACKWARD, Constness::CONST>
Digraph<V, E>::EView<c>::crbegin() const noexcept
{ return static_cast<ENode<V, E>*>(mTail->val); }

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::EView<c>::Iterator<Direction::BACKWARD, Constness::CONST>
Digraph<V, E>::EView<c>::crend() const noexcept
{ return nullptr; }

template <typename V, typename E>
template <Constness evc>
template <Direction d, Constness c>
template <Constness oc>
class Digraph<V, E>::EView<evc>::Iterator<d, c>&
Digraph<V, E>::EView<evc>::Iterator<d, c>::operator=(EDescriptor<oc> const& other) noexcept
requires ConstCompat<c, oc>
{
	this->pos = other.pos;
	return *this;
}

template <typename V, typename E>
template <Constness evc>
template <Direction d, Constness c>
class Digraph<V, E>::EView<evc>::Iterator<d, c>&
Digraph<V, E>::EView<evc>::Iterator<d, c>::operator++() noexcept
{
	auto* le = d == Direction::FORWARD
		? ToParent(this->pos, &LNode<ENode<V, E>>::val)->next
		: ToParent(this->pos, &LNode<ENode<V, E>>::val)->prev;
	this->pos = le ? static_cast<ENode<V, E>*>(le->val) : nullptr;
	return *this;
}

template <typename V, typename E>
template <Constness evc>
template <Direction d, Constness c>
class Digraph<V, E>::EView<evc>::Iterator<d, c>
Digraph<V, E>::EView<evc>::Iterator<d, c>::operator++(int) noexcept
{
	auto* r = this->pos;
	auto* le = d == Direction::FORWARD
		? ToParent(this->pos, &LNode<ENode<V, E>>::val)->next
		: ToParent(this->pos, &LNode<ENode<V, E>>::val)->prev;
	this->pos = le ? static_cast<ENode<V, E>*>(le->val) : nullptr;
	return r;
}

template <typename V, typename E>
template <Constness evc>
template <Direction d, Constness c>
class Digraph<V, E>::EView<evc>::Iterator<d, c>&
Digraph<V, E>::EView<evc>::Iterator<d, c>::operator--() noexcept
{
	auto* le = d == Direction::FORWARD
		? ToParent(this->pos, &LNode<ENode<V, E>>::val)->prev
		: ToParent(this->pos, &LNode<ENode<V, E>>::val)->next;
	this->pos = le ? static_cast<ENode<V, E>*>(le->val) : nullptr;
	return *this;
}

template <typename V, typename E>
template <Constness evc>
template <Direction d, Constness c>
class Digraph<V, E>::EView<evc>::Iterator<d, c>
Digraph<V, E>::EView<evc>::Iterator<d, c>::operator--(int) noexcept
{
	auto* r = this->pos;
	auto* le = d == Direction::FORWARD
		? ToParent(this->pos, &LNode<ENode<V, E>>::val)->prev
		: ToParent(this->pos, &LNode<ENode<V, E>>::val)->next;
	this->pos = le ? static_cast<ENode<V, E>*>(le->val) : nullptr;
	return r;
}

template <typename V, typename E>
template <Constness evc>
template <Direction d, Constness c>
class Digraph<V, E>::EDescriptor<c> const&
Digraph<V, E>::EView<evc>::Iterator<d, c>::operator*() const noexcept
{ return *this; }

template <typename V, typename E>
template <Constness evc>
template <Direction d, Constness c>
class Digraph<V, E>::EDescriptor<c> const*
Digraph<V, E>::EView<evc>::Iterator<d, c>::operator->() const noexcept
{ return this; }

template <typename V, typename E>
template <Direction d, Constness c>
template <Constness oc>
class Digraph<V, E>::AdjacencyList<d, c>&
Digraph<V, E>::AdjacencyList<d, c>::operator=(VDescriptor<oc> const& other) noexcept
requires ConstCompat<c, oc>
{
	this->pos = other.pos;
	return *this;
}

template <typename V, typename E>
template <Direction d, Constness c>
class Digraph<V, E>::AdjacencyList<d, c>::Iterator<Direction::FORWARD, Constness::NCONST>
Digraph<V, E>::AdjacencyList<d, c>::begin() noexcept
requires (c == Constness::NCONST)
{ return d == Direction::FORWARD ? this->pos->outHead : this->pos->inHead; }

template <typename V, typename E>
template <Direction d, Constness c>
class Digraph<V, E>::AdjacencyList<d, c>::Iterator<Direction::FORWARD, Constness::CONST>
Digraph<V, E>::AdjacencyList<d, c>::begin() const noexcept
{ return d == Direction::FORWARD ? this->pos->outHead : this->pos->inHead; }

template <typename V, typename E>
template <Direction d, Constness c>
class Digraph<V, E>::AdjacencyList<d, c>::Iterator<Direction::FORWARD, Constness::NCONST>
Digraph<V, E>::AdjacencyList<d, c>::end() noexcept
requires (c == Constness::NCONST)
{ return nullptr; }

template <typename V, typename E>
template <Direction d, Constness c>
class Digraph<V, E>::AdjacencyList<d, c>::Iterator<Direction::FORWARD, Constness::CONST>
Digraph<V, E>::AdjacencyList<d, c>::end() const noexcept
{ return nullptr; }

template <typename V, typename E>
template <Direction d, Constness c>
class Digraph<V, E>::AdjacencyList<d, c>::Iterator<Direction::BACKWARD, Constness::NCONST>
Digraph<V, E>::AdjacencyList<d, c>::rbegin() noexcept
requires (c == Constness::NCONST)
{ return d == Direction::FORWARD ? this->pos->outTail : this->pos->inTail; }

template <typename V, typename E>
template <Direction d, Constness c>
class Digraph<V, E>::AdjacencyList<d, c>::Iterator<Direction::BACKWARD, Constness::CONST>
Digraph<V, E>::AdjacencyList<d, c>::rbegin() const noexcept
{ return d == Direction::FORWARD ? this->pos->outTail : this->pos->inTail; }

template <typename V, typename E>
template <Direction d, Constness c>
class Digraph<V, E>::AdjacencyList<d, c>::Iterator<Direction::BACKWARD, Constness::NCONST>
Digraph<V, E>::AdjacencyList<d, c>::rend() noexcept
requires (c == Constness::NCONST)
{ return nullptr; }

template <typename V, typename E>
template <Direction d, Constness c>
class Digraph<V, E>::AdjacencyList<d, c>::Iterator<Direction::BACKWARD, Constness::CONST>
Digraph<V, E>::AdjacencyList<d, c>::rend() const noexcept
{ return nullptr; }

template <typename V, typename E>
template <Direction d, Constness c>
class Digraph<V, E>::AdjacencyList<d, c>::Iterator<Direction::FORWARD, Constness::CONST>
Digraph<V, E>::AdjacencyList<d, c>::cbegin() const noexcept
{ return d == Direction::FORWARD ? this->pos->outHead : this->pos->inHead; }

template <typename V, typename E>
template <Direction d, Constness c>
class Digraph<V, E>::AdjacencyList<d, c>::Iterator<Direction::FORWARD, Constness::CONST>
Digraph<V, E>::AdjacencyList<d, c>::cend() const noexcept
{ return nullptr; }

template <typename V, typename E>
template <Direction d, Constness c>
class Digraph<V, E>::AdjacencyList<d, c>::Iterator<Direction::BACKWARD, Constness::CONST>
Digraph<V, E>::AdjacencyList<d, c>::crbegin() const noexcept
{ return d == Direction::FORWARD ? this->pos->outTail : this->pos->inTail; }

template <typename V, typename E>
template <Direction d, Constness c>
class Digraph<V, E>::AdjacencyList<d, c>::Iterator<Direction::BACKWARD, Constness::CONST>
Digraph<V, E>::AdjacencyList<d, c>::crend() const noexcept
{ return nullptr; }

template <typename V, typename E>
template <Direction ald, Constness alc>
template <Direction d, Constness c>
template <Constness oc>
class Digraph<V, E>::AdjacencyList<ald, alc>::Iterator<d, c>&
Digraph<V, E>::AdjacencyList<ald, alc>::Iterator<d, c>::operator=(EDescriptor<oc> const& other) noexcept
requires ConstCompat<c, oc>
{
	this->pos = other.pos;
	return *this;
}

template <typename V, typename E>
template <Direction ald, Constness alc>
template <Direction d, Constness c>
class Digraph<V, E>::AdjacencyList<ald, alc>::Iterator<d, c>&
Digraph<V, E>::AdjacencyList<ald, alc>::Iterator<d, c>::operator++() noexcept
{
	this->pos = ald == Direction::FORWARD
			? (d == Direction::FORWARD ? this->pos->up : this->pos->down)
			: (d == Direction::FORWARD ? this->pos->right : this->pos->left);
	return *this;
}

template <typename V, typename E>
template <Direction ald, Constness alc>
template <Direction d, Constness c>
class Digraph<V, E>::AdjacencyList<ald, alc>::Iterator<d, c>
Digraph<V, E>::AdjacencyList<ald, alc>::Iterator<d, c>::operator++(int) noexcept
{
	ENode<V, E>* r = this->pos;
	this->pos = ald == Direction::FORWARD
			? (d == Direction::FORWARD ? this->pos->up : this->pos->down)
			: (d == Direction::FORWARD ? this->pos->right : this->pos->left);
	return r;
}

template <typename V, typename E>
template <Direction ald, Constness alc>
template <Direction d, Constness c>
class Digraph<V, E>::AdjacencyList<ald, alc>::Iterator<d, c>&
Digraph<V, E>::AdjacencyList<ald, alc>::Iterator<d, c>::operator--() noexcept
{
	this->pos = ald == Direction::FORWARD
			? (d == Direction::FORWARD ? this->pos->down : this->pos->up)
			: (d == Direction::FORWARD ? this->pos->left : this->pos->right);
	return *this;
}

template <typename V, typename E>
template <Direction ald, Constness alc>
template <Direction d, Constness c>
class Digraph<V, E>::AdjacencyList<ald, alc>::Iterator<d, c>
Digraph<V, E>::AdjacencyList<ald, alc>::Iterator<d, c>::operator--(int) noexcept
{
	ENode<V, E>* r = this->pos;
	this->pos = ald == Direction::FORWARD
			? (d == Direction::FORWARD ? this->pos->down : this->pos->up)
			: (d == Direction::FORWARD ? this->pos->left : this->pos->right);
	return r;
}

template <typename V, typename E>
template <Direction ald, Constness alc>
template <Direction d, Constness c>
class Digraph<V, E>::EDescriptor<c> const&
Digraph<V, E>::AdjacencyList<ald, alc>::Iterator<d, c>::operator*() const noexcept
{ return *this; }

template <typename V, typename E>
template <Direction ald, Constness alc>
template <Direction d, Constness c>
class Digraph<V, E>::EDescriptor<c> const*
Digraph<V, E>::AdjacencyList<ald, alc>::Iterator<d, c>::operator->() const noexcept
{ return this; }

}//namespace DS
