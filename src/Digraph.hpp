#include "DS/Digraph.h"

namespace DS {

template <typename V, typename E>
Digraph<V, E>::Digraph(Digraph const& other)
requires std::is_copy_constructible_v<V> && std::is_copy_constructible_v<E>
{
	Map<VNode<V, E> const*, VNode<V, E>*> map;

	if (other.mVerticesSize) {
		mVerticesSize = other.mVerticesSize;
		(mVTail = mVHead = ::new LNode<VNode<V, E>>(static_cast<V const&>(static_cast<VNode<V, E> const*>(other.mVHead->val)->val)))->prev = nullptr;
		try {
			map.put(static_cast<VNode<V, E> const*>(other.mVHead->val))
				.set(static_cast<VNode<V, E>*>(mVHead->val));
			LNode<VNode<V, E>> const* src = other.mVHead;
			while (src = src->next) {
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
		if (other.mEdgesSize) {
			mEdgesSize = other.mEdgesSize;
			(mETail = mEHead = ::new LNode<ENode<V, E>>(static_cast<E const&>(static_cast<ENode<V, E> const*>(other.mEHead->val)->val)))->prev = nullptr;
			if (static_cast<ENode<V, E> const*>(other.mEHead->val)->in)
				static_cast<ENode<V, E>*>(mEHead->val)
					->setIn(map.get(static_cast<ENode<V, E> const*>(other.mEHead->val)->in)->value);
			if (static_cast<ENode<V, E> const*>(other.mEHead->val)->out)
				static_cast<ENode<V, E>*>(mEHead->val)
					->setOut(map.get(static_cast<ENode<V, E> const*>(other.mEHead->val)->out)->value);
			LNode<ENode<V, E>> const* src = other.mEHead;
			while (src = src->next) {
				try {
					(mETail->next = ::new LNode<ENode<V, E>>(static_cast<E const&>(static_cast<ENode<V, E> const*>(src->val)->val)))->prev = mETail;
					mETail = mETail->next;
				} catch (...) {
					mEdges.~List();
					throw;
				}
				if (static_cast<ENode<V, E> const*>(src->val)->in)
					static_cast<ENode<V, E>*>(mETail->val)
						->setIn(map.get(static_cast<ENode<V, E> const*>(src->val)->in)->value);
				if (static_cast<ENode<V, E> const*>(src->val)->out)
					static_cast<ENode<V, E>*>(mETail->val)
						->setOut(map.get(static_cast<ENode<V, E> const*>(src->val)->out)->value);
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
template <typename ... VArgs, typename ... EArgs>
Digraph<V, E>::Digraph(Stream::Input& vInput, VArgs&& ... vArgs, Stream::Input& eInput, EArgs&& ... eArgs)
requires Deserializable<V, Stream::Input, VArgs ...> && Deserializable<E, Stream::Input, EArgs ...>
{ deserializeEdges(deserializeVertices(vInput, std::forward<VArgs>(vArgs) ...), eInput, std::forward<EArgs>(eArgs) ...); }

template <typename V, typename E>
template <typename VIDType, typename ... VFArgs, typename EIDType, typename ... EFArgs>
Digraph<V, E>::Digraph(Stream::Input& vInput, DP::Factory<V, VIDType, VFArgs ...> const& vFactory, Stream::Input& eInput, DP::Factory<E, EIDType, EFArgs ...> const& eFactory)
{ deserializeEdges(deserializeVertices(vInput, vFactory), eInput, eFactory); }

template <typename V, typename E>
template <typename ... VArgs, typename EIDType, typename ... EFArgs>
Digraph<V, E>::Digraph(Stream::Input& vInput, VArgs&& ... vArgs, Stream::Input& eInput, DP::Factory<E, EIDType, EFArgs ...> const& eFactory)
requires Deserializable<V, Stream::Input, VArgs ...>
{ deserializeEdges(deserializeVertices(vInput, std::forward<VArgs>(vArgs) ...), eInput, eFactory); }

template <typename V, typename E>
template <typename VIDType, typename ... VFArgs, typename ... EArgs>
Digraph<V, E>::Digraph(Stream::Input& vInput, DP::Factory<V, VIDType, VFArgs ...> const& vFactory, Stream::Input& eInput, EArgs&& ... eArgs)
requires Deserializable<E, Stream::Input, EArgs ...>
{ deserializeEdges(deserializeVertices(vInput, vFactory), eInput, std::forward<EArgs>(eArgs) ...); }

template <typename V, typename E>
Stream::Output&
operator<<(Stream::Output& output, Digraph<V, E> const& digraph)
requires Stream::Serializable<V, Stream::Output> && Stream::Serializable<E, Stream::Output>
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
			<< map.get(static_cast<ENode<V, E> const*>(le->val)->in)->value
			<< map.get(static_cast<ENode<V, E> const*>(le->val)->out)->value;

	return output;
}

template <typename V, typename E>
Digraph<V, E>::~Digraph()
{
	mVertices.~List();
	mEdges.~List();
}

template <typename V, typename E>
template <typename ... VArgs>
Vector<VNode<V, E>*>
Digraph<V, E>::deserializeVertices(Stream::Input& vInput, VArgs&& ... vArgs)
requires Deserializable<V, Stream::Input, VArgs ...>
{
	mVerticesSize = Stream::Get<std::uint64_t>(vInput);
	Vector<VNode<V, E>*> vs(mVerticesSize + 1);
	vs.pushBack(nullptr);

	if (mVerticesSize) {
		(mVTail = mVHead = ::new LNode<VNode<V, E>>(vInput, std::forward<VArgs>(vArgs) ...))->prev = nullptr;
		vs.pushBack(static_cast<VNode<V, E>*>(mVHead->val));
		std::uint64_t size = mVerticesSize;
		while (--size) {
			try {
				(mVTail->next = ::new LNode<VNode<V, E>>(vInput, std::forward<VArgs>(vArgs) ...))->prev = mVTail;
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
template <typename VIDType, typename ... VFArgs>
Vector<VNode<V, E>*>
Digraph<V, E>::deserializeVertices(Stream::Input& vInput, DP::Factory<V, VIDType, VFArgs ...> const&)
{
	mVerticesSize = Stream::Get<std::uint64_t>(vInput);
	Vector<VNode<V, E>*> vs(mVerticesSize + 1);
	vs.pushBack(nullptr);

	if (mVerticesSize) {
		{
			auto const& vCreateInfo = DP::Factory<V, VIDType, VFArgs ...>::GetCreateInfo(Stream::Get<VIDType>(vInput));
			(mVTail = mVHead = new(Offset(&VNode<V, E>::val) + vCreateInfo.size) LNode<VNode<V, E>>(vCreateInfo.create, Stream::Get<std::remove_cvref_t<VFArgs>>(vInput) ...))->prev = nullptr;
		}
		vs.pushBack(static_cast<VNode<V, E>*>(mVHead->val));
		std::uint64_t size = mVerticesSize;
		while (--size) {
			try {
				auto const& vCreateInfo = DP::Factory<V, VIDType, VFArgs ...>::GetCreateInfo(Stream::Get<VIDType>(vInput));
				(mVTail->next = new(Offset(&VNode<V, E>::val) + vCreateInfo.size) LNode<VNode<V, E>>(vCreateInfo.create, Stream::Get<std::remove_cvref_t<VFArgs>>(vInput) ...))->prev = mVTail;
				mVTail = mVTail->next;
			} catch (...) {
				mVertices.~List();
				throw;
			}
			vs.pushBack(static_cast<VNode<V, E>*>(mVTail->val));
		}
		mVTail->next = nullptr;
	}
	else
		mVTail = mVHead = nullptr;

	return vs;
}

template <typename V, typename E>
template <typename ... EArgs>
void
Digraph<V, E>::deserializeEdges(Vector<VNode<V, E>*> vs, Stream::Input& eInput, EArgs&& ... eArgs)
requires Deserializable<E, Stream::Input, EArgs ...>
{
	try {
		mEdgesSize = Stream::Get<std::uint64_t>(eInput);
		if (mEdgesSize) {
			(mETail = mEHead = ::new LNode<ENode<V, E>>(eInput, std::forward<EArgs>(eArgs) ...))->prev = nullptr;
			try {
				static_cast<ENode<V, E>*>(mEHead->val)->setIn(vs[Stream::Get<std::uint64_t>(eInput)]);
				static_cast<ENode<V, E>*>(mEHead->val)->setOut(vs[Stream::Get<std::uint64_t>(eInput)]);
				std::uint64_t size = mEdgesSize;
				while (--size) {
					(mETail->next = ::new LNode<ENode<V, E>>(eInput, std::forward<EArgs>(eArgs) ...))->prev = mETail;
					mETail = mETail->next;
					static_cast<ENode<V, E>*>(mETail->val)->setIn(vs[Stream::Get<std::uint64_t>(eInput)]);
					static_cast<ENode<V, E>*>(mETail->val)->setOut(vs[Stream::Get<std::uint64_t>(eInput)]);
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
template <typename EIDType, typename ... EFArgs>
void
Digraph<V, E>::deserializeEdges(Vector<VNode<V, E>*> vs, Stream::Input& eInput, DP::Factory<E, EIDType, EFArgs ...> const&)
{
	try {
		mEdgesSize = Stream::Get<std::uint64_t>(eInput);
		if (mEdgesSize) {
			{
				auto const& eCreateInfo = DP::Factory<E, EIDType, EFArgs ...>::GetCreateInfo(Stream::Get<EIDType>(eInput));
				(mETail = mEHead = new(Offset(&ENode<V, E>::val) + eCreateInfo.size) LNode<ENode<V, E>>(eCreateInfo.create, Stream::Get<std::remove_cvref_t<EFArgs>>(eInput) ...))->prev = nullptr;
			}
			try {
				static_cast<ENode<V, E>*>(mEHead->val)->setIn(vs[Stream::Get<std::uint64_t>(eInput)]);
				static_cast<ENode<V, E>*>(mEHead->val)->setOut(vs[Stream::Get<std::uint64_t>(eInput)]);
				std::uint64_t size = mEdgesSize;
				while (--size) {
					auto const& eCreateInfo = DP::Factory<E, EIDType, EFArgs ...>::GetCreateInfo(Stream::Get<EIDType>(eInput));
					(mETail->next = new(Offset(&ENode<V, E>::val) + eCreateInfo.size) LNode<ENode<V, E>>(eCreateInfo.create, Stream::Get<std::remove_cvref_t<EFArgs>>(eInput) ...))->prev = mETail;
					mETail = mETail->next;
					static_cast<ENode<V, E>*>(mETail->val)->setIn(vs[Stream::Get<std::uint64_t>(eInput)]);
					static_cast<ENode<V, E>*>(mETail->val)->setOut(vs[Stream::Get<std::uint64_t>(eInput)]);
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
template <typename ... VArgs>
class Digraph<V, E>::VDescriptor<Constness::NCONST>
Digraph<V, E>::addVertex(VArgs&& ... vArgs)
{
	mVertices.pushFront(std::forward<VArgs>(vArgs) ...);
	return static_cast<VNode<V, E>*>(mVHead->val);
}

template <typename V, typename E>
template <Derived<V> DV, typename ... DVArgs>
class Digraph<V, E>::VDescriptor<Constness::NCONST>
Digraph<V, E>::addVertex(DVArgs&& ... dvArgs)
{
	reinterpret_cast<List<VNode<DV, E>>&>(mVertices).pushFront(std::forward<DVArgs>(dvArgs) ...);
	return static_cast<VNode<V, E>*>(mVHead->val);
}

template <typename V, typename E>
template <typename ... VCIArgs, typename ... VCArgs>
class Digraph<V, E>::VDescriptor<Constness::NCONST>
Digraph<V, E>::addVertex(DP::CreateInfo<V, VCIArgs ...> const& vCreateInfo, VCArgs&& ... cArgs)
{
	mVertices.pushFront(new(Offset(&VNode<V, E>::val) + vCreateInfo.size)
			LNode<VNode<V, E>>(vCreateInfo.create, std::forward<VCArgs>(cArgs) ...));
	return static_cast<VNode<V, E>*>(mVHead->val);
}

template <typename V, typename E>
template <typename ... EArgs>
class Digraph<V, E>::EDescriptor<Constness::NCONST>
Digraph<V, E>::addEdge(EArgs&& ... eArgs)
{
	mEdges.pushFront(std::forward<EArgs>(eArgs) ...);
	return static_cast<ENode<V, E>*>(mEHead->val);
}

template <typename V, typename E>
template <Derived<E> DE, typename ... DEArgs>
class Digraph<V, E>::EDescriptor<Constness::NCONST>
Digraph<V, E>::addEdge(DEArgs&& ... deArgs)
{
	reinterpret_cast<List<ENode<V, DE>>&>(mEdges).pushFront(std::forward<DEArgs>(deArgs) ...);
	return static_cast<ENode<V, E>*>(mEHead->val);
}

template <typename V, typename E>
template <typename ... ECIArgs, typename ... ECArgs>
class Digraph<V, E>::EDescriptor<Constness::NCONST>
Digraph<V, E>::addEdge(DP::CreateInfo<E, ECIArgs ...> const& eCreateInfo, ECArgs&& ... cArgs)
{
	mEdges.pushFront(new(Offset(&ENode<V, E>::val) + eCreateInfo.size)
		LNode<ENode<V, E>>(eCreateInfo.create, std::forward<ECArgs>(cArgs) ...));
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
		moveBack(ValToNode(v.pos, &LNode<VNode<V, E>>::val), mVHead, mVTail);
		v.pos->color = 2; // mark as gray
	}
	if (!mVPivot)
		mVPivot = ValToNode(v.pos, &LNode<VNode<V, E>>::val);
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
							moveBack(ValToNode(e->out, &LNode<VNode<V, E>>::val), mVHead, mVTail);
							e->out->color = 2; // Gray
						}
				} else {
					for (auto* e = lv->val->inHead; e; e = e->right)
						if (e->in && e->in->isWhite) {
							moveBack(ValToNode(e->in, &LNode<VNode<V, E>>::val), mVHead, mVTail);
							e->in->color = 2; // Gray
						}
				}
			} else {
				if constexpr (d == Direction::FORWARD) {
					for (auto* e = lv->val->outTail; e; e = e->down)
						if (e->out && !e->out->isBlack) {
							moveAfter(lv, ValToNode(e->out, &LNode<VNode<V, E>>::val), mVHead, mVTail);
							e->out->color = 2; // Gray
						}
				} else {
					for (auto* e = lv->val->inTail; e; e = e->left)
						if (e->in && !e->in->isBlack) {
							moveAfter(lv, ValToNode(e->in, &LNode<VNode<V, E>>::val), mVHead, mVTail);
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
									moveBefore(head, ValToNode(e->out, &LNode<VNode<V, E>>::val), mVHead, mVTail);
									e->out->color = 2; // Gray
								}
						} else {
							for (auto* e = lv->val->inHead; e; e = e->right)
								if (e->in && e->in->isWhite) {
									moveBefore(head, ValToNode(e->in, &LNode<VNode<V, E>>::val), mVHead, mVTail);
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
									moveAfter(mVPivot, ValToNode(e->out, &LNode<VNode<V, E>>::val), mVHead, mVTail);
									e->out->color = 2; // Gray
								}
						} else {
							for (auto* e = lv->val->inTail; e; e = e->left)
								if (e->in && !e->in->isBlack) {
									moveAfter(mVPivot, ValToNode(e->in, &LNode<VNode<V, E>>::val), mVHead, mVTail);
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
class Digraph<V, E>::AdjacencyList<Direction::BACKWARD, c>
Digraph<V, E>::VDescriptor<c>::getIn() const noexcept
{ return pos; }

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::AdjacencyList<Direction::FORWARD, c>
Digraph<V, E>::VDescriptor<c>::getOut() const noexcept
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
		? ValToNode(this->pos, &LNode<VNode<V, E>>::val)->next
		: ValToNode(this->pos, &LNode<VNode<V, E>>::val)->prev;
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
		? ValToNode(this->pos, &LNode<VNode<V, E>>::val)->next
		: ValToNode(this->pos, &LNode<VNode<V, E>>::val)->prev;
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
		? ValToNode(this->pos, &LNode<VNode<V, E>>::val)->prev
		: ValToNode(this->pos, &LNode<VNode<V, E>>::val)->next;
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
		? ValToNode(this->pos, &LNode<VNode<V, E>>::val)->prev
		: ValToNode(this->pos, &LNode<VNode<V, E>>::val)->next;
	this->pos = lv ? static_cast<VNode<V, E>*>(lv->val) : nullptr;
	return r;
}

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
		? ValToNode(this->pos, &LNode<ENode<V, E>>::val)->next
		: ValToNode(this->pos, &LNode<ENode<V, E>>::val)->prev;
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
		? ValToNode(this->pos, &LNode<ENode<V, E>>::val)->next
		: ValToNode(this->pos, &LNode<ENode<V, E>>::val)->prev;
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
		? ValToNode(this->pos, &LNode<ENode<V, E>>::val)->prev
		: ValToNode(this->pos, &LNode<ENode<V, E>>::val)->next;
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
		? ValToNode(this->pos, &LNode<ENode<V, E>>::val)->prev
		: ValToNode(this->pos, &LNode<ENode<V, E>>::val)->next;
	this->pos = le ? static_cast<ENode<V, E>*>(le->val) : nullptr;
	return r;
}

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

}//namespace DS
