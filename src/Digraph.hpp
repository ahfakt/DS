#ifndef DS_DIGRAPH_HPP
#define DS_DIGRAPH_HPP

#include "VNode.hpp"
#include "DS/Map.h"

namespace DS {

template <typename V, typename E>
Digraph<V, E>::Digraph() noexcept:
	mVertices(), mEdges() {}

template <typename V, typename E>
Digraph<V, E>::Digraph(Digraph&& other) noexcept:
	mVertices(std::move(other.mVertices)), mEdges(std::move(other.mEdges)) {}

template <typename V, typename E>
Digraph<V, E>::Digraph(Digraph const& other) : Digraph() {
	Map<VNode<V, E>*, VNode<V, E>*> map;
	if (LNode<VNode<V, E>>* src = other.mVHead) {
		mVTail = mVHead = new LNode<VNode<V, E>>(reinterpret_cast<LNode<VNode<V, E>>*>(0), nullptr, src->value.value);
		++mVerticesSize;
		map.addKey(&src->value).setValue(&mVHead->value);
		while (src = src->next) {
			mVTail = mVTail->next = new LNode<VNode<V, E>>(mVTail, nullptr, src->value.value);
			++mVerticesSize;
			map.addKey(&src->value).setValue(&mVTail->value);
		}
	}
	if (LNode<ENode<V, E>>* src = other.mEHead) {
		mETail = mEHead = new LNode<ENode<V, E>>(reinterpret_cast<LNode<ENode<V, E>>*>(0), nullptr, src->value.value);
		++mEdgesSize;
		if (src->value.in) mEHead->value.setIn(*map.get(src->value.in));
		if (src->value.out) mEHead->value.setOut(*map.get(src->value.out));
		while (src = src->next) {
			mETail = mETail->next = new LNode<ENode<V, E>>(mETail, nullptr, src->value.value);
			++mEdgesSize;
			if (src->value.in) mETail->value.setIn(*map.get(src->value.in));
			if (src->value.out) mETail->value.setOut(*map.get(src->value.out));
		}
	}
}

template <typename V, typename E>
Digraph<V, E>&
Digraph<V, E>::operator=(Digraph&& other) noexcept {
	if (this == &other) return *this;
	this->~Digraph();
	return *new(this) Digraph(std::move(other));
}

template <typename V, typename E>
Digraph<V, E>&
Digraph<V, E>::operator=(Digraph const& other) {
	// TODO: use allocated nodes
	if (this == &other) return *this;
	this->~Digraph();
	return *new(this) Digraph(other);
}

template <typename V, typename E>
Digraph<V, E>::~Digraph() {
	mVertices.~List();
	mEdges.~List();
}

template <typename V, typename E>
template <typename v, typename e>
Digraph<V, E>::Digraph(std::enable_if_t<(Stream::isDeserializable<v> || std::is_trivially_copyable_v<v>) && (Stream::isDeserializable<e> || std::is_trivially_copyable_v<e>), Stream::Input>& is) : Digraph() {
	Map<std::uint64_t, VNode<V, E>*> map;
	map.addKey(0).setValue(nullptr);

	std::uint64_t size;
	if (!(is >> size)) throw Stream::Input::Exception("Insufficient");

	if(size) {
		mVTail = mVHead = new LNode<VNode<V, E>>(reinterpret_cast<LNode<VNode<V, E>>*>(0), nullptr, is, map);
		++mVerticesSize;
		while (--size) {
			mVTail = mVTail->next = new LNode<VNode<V, E>>(mVTail, nullptr, is, map);
			++mVerticesSize;
		}
	}

	if (!(is >> size)) throw Stream::Input::Exception("Insufficient");
	if(size) {
		mETail = mEHead = new LNode<ENode<V, E>>(reinterpret_cast<LNode<ENode<V, E>>*>(0), nullptr, is, map);
		++mEdgesSize;
		while (--size) {
			mETail = mETail->next = new LNode<ENode<V, E>>(mETail, nullptr, is, map);
			++mEdgesSize;
		}
	}
}

template <typename V, typename E>
template <typename v, typename e>
Digraph<V, E>&
Digraph<V, E>::operator=(std::enable_if_t<(Stream::isDeserializable<v> || std::is_trivially_copyable_v<v>) && (Stream::isDeserializable<e> || std::is_trivially_copyable_v<e>), Stream::Input>& is) {
	// TODO: use allocated nodes
	this->~Digraph();
	return *new(this) Digraph(is);
}

template<typename V, typename E>
std::enable_if_t<(Stream::isDeserializable<V> || std::is_trivially_copyable_v<V>) && (Stream::isDeserializable<E> || std::is_trivially_copyable_v<E>), Stream::Output&>
operator<<(Stream::Output& os, Digraph<V, E> const& digraph) {
	Map<VNode<V, E> const*, std::uint64_t> map;

	if (!(os << digraph.mVerticesSize)) throw Stream::Output::Exception("Insufficient");
	for(LNode<VNode<V, E>> const* v = digraph.mVHead; v; v = v->next) v->value.serialize(os, map);

	map.addKey(nullptr).setValue(0);

	if (!(os << digraph.mEdgesSize)) throw Stream::Output::Exception("Insufficient");
	for(LNode<ENode<V, E>> const* e = digraph.mEHead; e; e = e->next) e->value.serialize(os, map);

	return os;
}

template <typename V, typename E>
template <typename ... Args>
class Digraph<V, E>::Vertex<CONSTNESS::NonConst>
Digraph<V, E>::addVertex(Args&& ... args) {
	mVertices.pushBack(std::forward<Args>(args) ...);
	return &(mVTail->value);
}

template <typename V, typename E>
template <typename D, typename ... Args>
class Digraph<V, E>::Vertex<CONSTNESS::NonConst>
Digraph<V, E>::addVertex(Args&& ... args) {
	reinterpret_cast<List<VNode<D, E>>&>(mVertices).pushBack(std::forward<Args>(args) ...);
	return &(mVTail->value);
}

template <typename V, typename E>
template <typename ... Args>
class Digraph<V, E>::Edge<CONSTNESS::NonConst>
Digraph<V, E>::addEdge(Args&& ... args) {
	mEdges.pushBack(std::forward<Args>(args) ...);
	return &(mETail->value);
}

template <typename V, typename E>
template <typename D, typename ... Args>
class Digraph<V, E>::Edge<CONSTNESS::NonConst>
Digraph<V, E>::addEdge(Args&& ... args) {
	reinterpret_cast<List<VNode<V, D>>&>(mEdges).pushBack(std::forward<Args>(args) ...);
	return &(mETail->value);
}

template <typename V, typename E>
template <CONSTNESS Constness>
void
Digraph<V, E>::remove(Vertex<Constness> v) noexcept {
	for(ENode<V, E>* e = v.pos->outTail; e; e = e->down) e->unsetIn();
	for(ENode<V, E>* e = v.pos->inTail; e; e = e->left) e->unsetOut();
	delete reinterpret_cast<LNode<VNode<V, E>>*>(reinterpret_cast<char*>(v.pos)-2*sizeof(char*))
		->detach(mVHead, mVTail);
	--mVerticesSize;
}

template <typename V, typename E>
template <CONSTNESS Constness>
void
Digraph<V, E>::remove(Edge<Constness> e) noexcept {
	e.pos->unsetIn();
	e.pos->unsetOut();
	delete reinterpret_cast<LNode<ENode<V, E>>*>(reinterpret_cast<char*>(e.pos)-2*sizeof(char*))
		->detach(mEHead, mETail);
	--mEdgesSize;
}

template <typename V, typename E>
template <TRAVERSE Traverse, ORDER Order, DIRECTION Direction>
void
Digraph<V, E>::traverse(List<VNode<V, E>*>& result) const {
	if constexpr (Traverse == TRAVERSE::DepthFirst) {
		auto v = result.begin();
		if constexpr (Order == ORDER::Preorder) {
			do {
				if constexpr (Direction == DIRECTION::Forward) {
					for(ENode<V, E>* e = (*v)->outTail; e; e = e->down) {
						if (e->out && e->out->isWhite) {
							e->out->state = 4; // Black
							result.insert(v + 1, e->out);
						}
					}
				}
				else { // Backward
					for(ENode<V, E>* e = (*v)->inTail; e; e = e->left) {
						if (e->in && e->in->isWhite) {
							e->in->state = 4; // Black
							result.insert(v + 1, e->in);
						}
					}
				}
			} while (++v);
		}
		else { // Postorder
			do {
				if ((*v)->isBlack) {
					++v;
					continue;
				}
				(*v)->state = 4; // Black
				if constexpr (Direction == DIRECTION::Forward) {
					for(ENode<V, E>* e = (*v)->outTail; e; e = e->down) {
						if (e->out && e->out->isWhite) {
							e->out->state = 2; // Gray
							v = result.insert(v, e->out);
						}
					}
				}
				else { // Backward
					for(ENode<V, E>* e = (*v)->inTail; e; e = e->left) {
						if (e->in && e->in->isWhite) {
							e->in->state = 2; // Gray
							v = result.insert(v, e->in);
						}
					}
				}
				if((*v)->isBlack) ++v; // v is leaf
			} while (v);
		}
	}
	else { // BreadthFirst
		if constexpr (Order == ORDER::Preorder) {
			auto v = result.begin();
			do {
				if constexpr (Direction == DIRECTION::Forward) {
					for(ENode<V, E>* e = (*v)->outHead; e; e = e->up) {
						if (e->out && e->out->isWhite) {
							e->out->state = 4; // Black
							result.pushBack(e->out);
						}
					}
				}
				else { // Backward
					for(ENode<V, E>* e = (*v)->inHead; e; e = e->right) {
						if (e->in && e->in->isWhite) {
							e->in->state = 4; // Black
							result.pushBack(e->in);
						}
					}
				}
			} while (++v);
		}
		else { // Postorder
			auto v = result.rbegin();
			do {
				if constexpr (Direction == DIRECTION::Forward) {
					for(ENode<V, E>* e = (*v)->outTail; e; e = e->down) {
						if (e->out && e->out->isWhite) {
							e->out->state = 4; // Black
							result.pushFront(e->out);
						}
					}
				}
				else { // Backward
					for(ENode<V, E>* e = (*v)->inTail; e; e = e->left) {
						if (e->in && e->in->isWhite) {
							e->in->state = 4; // Black
							result.pushFront(e->in);
						}
					}
				}
			} while (++v);
		}
	}
}

template <typename V, typename E>
template <TRAVERSE Traverse, ORDER Order, DIRECTION Direction>
void
Digraph<V, E>::traverse(List<VNode<V, E>*>& result, VNode<V, E>* vertex) const try {
	if constexpr (Traverse == TRAVERSE::DepthFirst) {
		result.pushBack(vertex);
		if constexpr (Order == ORDER::Preorder)
			(*result.begin())->state = 4; // Black
		traverse<TRAVERSE::DepthFirst, Order, Direction>(result);
	}
	else { // BreadthFirst
		(*result.pushBack(vertex))->state = 4; // Black
		traverse<TRAVERSE::BreadthFirst, Order, Direction>(result);
	}
	for (auto v : result) v->state = 1; // reset
} catch(std::bad_alloc exc) { // Make them unchanged to support const
	for (auto v : result) v->state = 1;
	throw exc;
}

template <typename V, typename E>
template <TRAVERSE Traverse, ORDER Order, DIRECTION Direction>
void
Digraph<V, E>::traverse(List<VNode<V, E>*>& result, List<VNode<V, E>*> const& vertexList) const try {
	for (auto vertex : vertexList)
		if (vertex)
			(*result.pushBack(vertex))->state = (Traverse == TRAVERSE::DepthFirst && Order == ORDER::Postorder) ? 2 : 4; // Gray : Black
	if (result)
		traverse<Traverse, Order, Direction>(result);
	for (auto v : result) v->state = 1; // reset
} catch(std::bad_alloc exc) { // Make them unchanged to support const
	for (auto v : result) v->state = 1;
	throw exc;
}

template <typename V, typename E>
template <TRAVERSE Traverse, ORDER Order, DIRECTION Direction, CONSTNESS Constness>
List<class Digraph<V, E>::Vertex<CONSTNESS::NonConst>>
Digraph<V, E>::traverse(Vertex<Constness> const& vertex) {
	List<Vertex<CONSTNESS::NonConst>> r;
	if (vertex.pos)
		traverse<Traverse, Order, Direction>(reinterpret_cast<List<VNode<V, E>*>&>(r), vertex.pos);
	return r;
}

template <typename V, typename E>
template <TRAVERSE Traverse, ORDER Order, DIRECTION Direction, CONSTNESS Constness>
List<class Digraph<V, E>::Vertex<CONSTNESS::Const>>
Digraph<V, E>::traverse(Vertex<Constness> const& vertex) const {
	List<Vertex<CONSTNESS::Const>> r;
	if (vertex.pos)
		traverse<Traverse, Order, Direction>(reinterpret_cast<List<VNode<V, E>*>&>(r), vertex.pos);
	return r;
}

template <typename V, typename E>
template <TRAVERSE Traverse, ORDER Order, DIRECTION Direction, CONSTNESS Constness>
List<class Digraph<V, E>::Vertex<CONSTNESS::NonConst>>
Digraph<V, E>::traverse(List<Vertex<Constness>> const& vertexList) {
	List<Vertex<CONSTNESS::NonConst>> r;
	if (vertexList)
		traverse<Traverse, Order, Direction>(reinterpret_cast<List<VNode<V, E>*>&>(r), reinterpret_cast<List<VNode<V, E>*> const&>(vertexList));
	return r;
}

template <typename V, typename E>
template <TRAVERSE Traverse, ORDER Order, DIRECTION Direction, CONSTNESS Constness>
List<class Digraph<V, E>::Vertex<CONSTNESS::Const>>
Digraph<V, E>::traverse(List<Vertex<Constness>> const& vertexList) const {
	List<Vertex<CONSTNESS::Const>> r;
	if (vertexList)
		traverse<Traverse, Order, Direction>(reinterpret_cast<List<VNode<V, E>*>&>(r), reinterpret_cast<List<VNode<V, E>*> const&>(vertexList));
	return r;
}

template <typename V, typename E>
template <CONSTNESS Constness>
Digraph<V, E>::Vertex<Constness>::Vertex(VNode<V, E>* pos) noexcept:
	pos(pos) {}

template <typename V, typename E>
template <CONSTNESS Constness>
template <CONSTNESS OtherConstness, typename>
Digraph<V, E>::Vertex<Constness>::Vertex(Vertex<OtherConstness> const& other) noexcept:
	pos(other.pos) {}

template <typename V, typename E>
template <CONSTNESS Constness>
template <CONSTNESS OtherConstness, typename>
class Digraph<V, E>::Vertex<Constness>&
Digraph<V, E>::Vertex<Constness>::operator=(Vertex<OtherConstness> const& other) noexcept {
	pos = other.pos;
	return *this;
}

template <typename V, typename E>
template <CONSTNESS Constness>
class Digraph<V, E>::Vertex<Constness>&
Digraph<V, E>::Vertex<Constness>::setVisited(bool b) const noexcept {
	pos->state = b ? 4 : 1; // Black : White
	return *this;
}

template <typename V, typename E>
template <CONSTNESS Constness>
std::uint64_t
Digraph<V, E>::Vertex<Constness>::getInDegree() const noexcept
{ return pos->inDegree; }

template <typename V, typename E>
template <CONSTNESS Constness>
std::uint64_t
Digraph<V, E>::Vertex<Constness>::getOutDegree() const noexcept
{ return pos->outDegree; }

template <typename V, typename E>
template <CONSTNESS Constness>
class Digraph<V, E>::AdjacencyList<DIRECTION::Backward, Constness>
Digraph<V, E>::Vertex<Constness>::getInList() const noexcept
{ return pos; }

template <typename V, typename E>
template <CONSTNESS Constness>
class Digraph<V, E>::AdjacencyList<DIRECTION::Forward, Constness>
Digraph<V, E>::Vertex<Constness>::getOutList() const noexcept
{ return pos; }

template <typename V, typename E>
template <CONSTNESS Constness>
std::conditional_t<Constness == CONSTNESS::Const, V const, V>&
Digraph<V, E>::Vertex<Constness>::operator*() const noexcept
{ return pos->value; }

template <typename V, typename E>
template <CONSTNESS Constness>
std::conditional_t<Constness == CONSTNESS::Const, V const, V>*
Digraph<V, E>::Vertex<Constness>::operator->() const noexcept
{ return &pos->value; }

template <typename V, typename E>
template <CONSTNESS Constness>
template <CONSTNESS OtherConstness>
bool
Digraph<V, E>::Vertex<Constness>::operator==(Vertex<OtherConstness> const& other) const noexcept
{ return pos == other.pos; }

template <typename V, typename E>
template <CONSTNESS Constness>
template <CONSTNESS OtherConstness>
bool
Digraph<V, E>::Vertex<Constness>::operator!=(Vertex<OtherConstness> const& other) const noexcept
{ return pos != other.pos; }

template <typename V, typename E>
template <CONSTNESS Constness>
Digraph<V, E>::Vertex<Constness>::operator bool() const noexcept
{ return pos; }

template <typename V, typename E>
template <CONSTNESS Constness>
Digraph<V, E>::Edge<Constness>::Edge(ENode<V, E>* pos) noexcept:
	pos(pos) {}

template <typename V, typename E>
template <CONSTNESS Constness>
template <CONSTNESS OtherConstness, typename>
Digraph<V, E>::Edge<Constness>::Edge(Edge<OtherConstness> const& other) noexcept:
	pos(other.pos) {}

template <typename V, typename E>
template <CONSTNESS Constness>
template <CONSTNESS OtherConstness, typename>
class Digraph<V, E>::Edge<Constness>&
Digraph<V, E>::Edge<Constness>::operator=(Edge<OtherConstness> const& other) noexcept {
	pos = other.pos;
	return *this;
}

template <typename V, typename E>
template <CONSTNESS Constness>
class Digraph<V, E>::Vertex<Constness>
Digraph<V, E>::Edge<Constness>::getIn() const noexcept
{ return pos->in; }

template <typename V, typename E>
template <CONSTNESS Constness>
class Digraph<V, E>::Vertex<Constness>
Digraph<V, E>::Edge<Constness>::getOut() const noexcept
{ return pos->out; }

template <typename V, typename E>
template <CONSTNESS Constness>
template <typename>
class Digraph<V, E>::Edge<CONSTNESS::NonConst>&
Digraph<V, E>::Edge<Constness>::setIn(Vertex<CONSTNESS::NonConst>& in) noexcept {
	pos->setIn(in.pos);
	return *this;
}

template <typename V, typename E>
template <CONSTNESS Constness>
template <typename>
class Digraph<V, E>::Edge<CONSTNESS::NonConst> const&
Digraph<V, E>::Edge<Constness>::setIn(Vertex<CONSTNESS::NonConst>& in) const noexcept
{ return const_cast<Edge<Constness>*>(this)->setIn(in); }

template <typename V, typename E>
template <CONSTNESS Constness>
template <typename>
class Digraph<V, E>::Edge<CONSTNESS::NonConst>&
Digraph<V, E>::Edge<Constness>::setOut(Vertex<CONSTNESS::NonConst>& out) noexcept {
	pos->setOut(out.pos);
	return *this;
}

template <typename V, typename E>
template <CONSTNESS Constness>
template <typename>
class Digraph<V, E>::Edge<CONSTNESS::NonConst> const&
Digraph<V, E>::Edge<Constness>::setOut(Vertex<CONSTNESS::NonConst>& out) const noexcept
{ return const_cast<Edge<Constness>*>(this)->setOut(out); }

template <typename V, typename E>
template <CONSTNESS Constness>
std::conditional_t<Constness == CONSTNESS::Const, E const, E>&
Digraph<V, E>::Edge<Constness>::operator*() const noexcept
{ return pos->value; }

template <typename V, typename E>
template <CONSTNESS Constness>
std::conditional_t<Constness == CONSTNESS::Const, E const, E>*
Digraph<V, E>::Edge<Constness>::operator->() const noexcept
{ return &(pos->value); }

template <typename V, typename E>
template <CONSTNESS Constness>
template <CONSTNESS OtherConstness>
bool
Digraph<V, E>::Edge<Constness>::operator==(Edge<OtherConstness> const& other) const noexcept
{ return pos == other.pos; }

template <typename V, typename E>
template <CONSTNESS Constness>
template <CONSTNESS OtherConstness>
bool
Digraph<V, E>::Edge<Constness>::operator!=(Edge<OtherConstness> const& other) const noexcept
{ return pos != other.pos; }

template <typename V, typename E>
template <CONSTNESS Constness>
Digraph<V, E>::Edge<Constness>::operator bool() const noexcept
{ return pos; }

template <typename V, typename E>
template <DIRECTION Direction, CONSTNESS Constness>
Digraph<V, E>::AdjacencyList<Direction, Constness>::AdjacencyList(VNode<V, E>* pos) noexcept:
	pos(pos) {}

template <typename V, typename E>
template <DIRECTION Direction, CONSTNESS Constness>
template <DIRECTION OtherDirection, CONSTNESS OtherConstness, typename>
Digraph<V, E>::AdjacencyList<Direction, Constness>::AdjacencyList(AdjacencyList<OtherDirection, OtherConstness> const& other) noexcept:
	pos(other.pos) {}

template <typename V, typename E>
template <DIRECTION Direction, CONSTNESS Constness>
template <DIRECTION OtherDirection, CONSTNESS OtherConstness, typename>
class Digraph<V, E>::AdjacencyList<Direction, Constness>&
Digraph<V, E>::AdjacencyList<Direction, Constness>::operator=(AdjacencyList<OtherDirection, OtherConstness> const& other) noexcept {
	pos = other.pos;
	return *this;
}

template <typename V, typename E>
template <DIRECTION Direction, CONSTNESS Constness>
Digraph<V, E>::AdjacencyList<Direction, Constness>::operator bool() const noexcept
{ return pos; }

template <typename V, typename E>
template <DIRECTION Direction, CONSTNESS Constness>
template <std::enable_if_t<Constness == CONSTNESS::NonConst, CONSTNESS> IConstness>
class Digraph<V, E>::AdjacencyList<Direction, Constness>::Iterator<DIRECTION::Forward, IConstness>
Digraph<V, E>::AdjacencyList<Direction, Constness>::begin() noexcept
{ return Direction == DIRECTION::Forward ? pos->outHead : pos->inHead; }

template <typename V, typename E>
template <DIRECTION Direction, CONSTNESS Constness>
template <std::enable_if_t<Constness == CONSTNESS::NonConst, CONSTNESS> IConstness>
class Digraph<V, E>::AdjacencyList<Direction, Constness>::Iterator<DIRECTION::Backward, IConstness>
Digraph<V, E>::AdjacencyList<Direction, Constness>::rbegin() noexcept
{ return Direction == DIRECTION::Forward ? pos->outTail : pos->inTail; }

template <typename V, typename E>
template <DIRECTION Direction, CONSTNESS Constness>
class Digraph<V, E>::AdjacencyList<Direction, Constness>::Iterator<DIRECTION::Forward, CONSTNESS::Const>
Digraph<V, E>::AdjacencyList<Direction, Constness>::begin() const noexcept
{ return Direction == DIRECTION::Forward ? pos->outHead : pos->inHead; }

template <typename V, typename E>
template <DIRECTION Direction, CONSTNESS Constness>
class Digraph<V, E>::AdjacencyList<Direction, Constness>::Iterator<DIRECTION::Backward, CONSTNESS::Const>
Digraph<V, E>::AdjacencyList<Direction, Constness>::rbegin() const noexcept
{ return Direction == DIRECTION::Forward ? pos->outTail : pos->inTail; }

template <typename V, typename E>
template <DIRECTION Direction, CONSTNESS Constness>
class Digraph<V, E>::AdjacencyList<Direction, Constness>::Iterator<DIRECTION::Forward, CONSTNESS::Const>
Digraph<V, E>::AdjacencyList<Direction, Constness>::end() const noexcept
{ return nullptr; }

template <typename V, typename E>
template <DIRECTION Direction, CONSTNESS Constness>
class Digraph<V, E>::AdjacencyList<Direction, Constness>::Iterator<DIRECTION::Backward, CONSTNESS::Const>
Digraph<V, E>::AdjacencyList<Direction, Constness>::rend() const noexcept
{ return nullptr; }

template <typename V, typename E>
template <DIRECTION ALDirection, CONSTNESS ALConstness>
template <DIRECTION Direction, CONSTNESS Constness>
Digraph<V, E>::AdjacencyList<ALDirection, ALConstness>::Iterator<Direction, Constness>::Iterator(ENode<V, E>* pos) noexcept:
	pos(pos) {}

template <typename V, typename E>
template <DIRECTION ALDirection, CONSTNESS ALConstness>
template <DIRECTION Direction, CONSTNESS Constness>
template <DIRECTION OtherDirection, CONSTNESS OtherConstness, typename>
Digraph<V, E>::AdjacencyList<ALDirection, ALConstness>::Iterator<Direction, Constness>::Iterator(Iterator<OtherDirection, OtherConstness> const& other) noexcept:
	pos(other.pos) {}

template <typename V, typename E>
template <DIRECTION ALDirection, CONSTNESS ALConstness>
template <DIRECTION Direction, CONSTNESS Constness>
template <DIRECTION OtherDirection, CONSTNESS OtherConstness, typename>
class Digraph<V, E>::AdjacencyList<ALDirection, ALConstness>::Iterator<Direction, Constness>&
Digraph<V, E>::AdjacencyList<ALDirection, ALConstness>::Iterator<Direction, Constness>::operator=(Iterator<OtherDirection, OtherConstness> const& other) noexcept {
	pos = other.pos;
	return *this;
}

template <typename V, typename E>
template <DIRECTION ALDirection, CONSTNESS ALConstness>
template <DIRECTION Direction, CONSTNESS Constness>
class Digraph<V, E>::AdjacencyList<ALDirection, ALConstness>::Iterator<Direction, Constness>&
Digraph<V, E>::AdjacencyList<ALDirection, ALConstness>::Iterator<Direction, Constness>::operator++() noexcept {
	pos = ALDirection == DIRECTION::Forward ?
			(Direction == DIRECTION::Forward ? pos->up : pos->down):
			(Direction == DIRECTION::Forward ? pos->right : pos->left);
	return *this;
}

template <typename V, typename E>
template <DIRECTION ALDirection, CONSTNESS ALConstness>
template <DIRECTION Direction, CONSTNESS Constness>
class Digraph<V, E>::AdjacencyList<ALDirection, ALConstness>::Iterator<Direction, Constness>
Digraph<V, E>::AdjacencyList<ALDirection, ALConstness>::Iterator<Direction, Constness>::operator++(int) noexcept {
	LNode<V>* r = pos;
	pos = ALDirection == DIRECTION::Forward ?
		  (Direction == DIRECTION::Forward ? pos->up : pos->down):
		  (Direction == DIRECTION::Forward ? pos->right : pos->left);
	return r;
}

template <typename V, typename E>
template <DIRECTION ALDirection, CONSTNESS ALConstness>
template <DIRECTION Direction, CONSTNESS Constness>
class Digraph<V, E>::AdjacencyList<ALDirection, ALConstness>::Iterator<Direction, Constness>&
Digraph<V, E>::AdjacencyList<ALDirection, ALConstness>::Iterator<Direction, Constness>::operator--() noexcept {
	pos = ALDirection == DIRECTION::Forward ?
		  (Direction == DIRECTION::Forward ? pos->down : pos->up):
		  (Direction == DIRECTION::Forward ? pos->left : pos->right);
	return *this;
}

template <typename V, typename E>
template <DIRECTION ALDirection, CONSTNESS ALConstness>
template <DIRECTION Direction, CONSTNESS Constness>
class Digraph<V, E>::AdjacencyList<ALDirection, ALConstness>::Iterator<Direction, Constness>
Digraph<V, E>::AdjacencyList<ALDirection, ALConstness>::Iterator<Direction, Constness>::operator--(int) noexcept {
	LNode<V>* r = pos;
	pos = ALDirection == DIRECTION::Forward ?
		  (Direction == DIRECTION::Forward ? pos->down : pos->up):
		  (Direction == DIRECTION::Forward ? pos->left : pos->right);
	return r;
}

template <typename V, typename E>
template <DIRECTION ALDirection, CONSTNESS ALConstness>
template <DIRECTION Direction, CONSTNESS Constness>
class Digraph<V, E>::AdjacencyList<ALDirection, ALConstness>::Iterator<Direction, Constness>
Digraph<V, E>::AdjacencyList<ALDirection, ALConstness>::Iterator<Direction, Constness>::operator+(std::uint64_t i) const noexcept {
	Iterator r(pos);
	for (; i; ++r, --i);
	return r;
}

template <typename V, typename E>
template <DIRECTION ALDirection, CONSTNESS ALConstness>
template <DIRECTION Direction, CONSTNESS Constness>
class Digraph<V, E>::AdjacencyList<ALDirection, ALConstness>::Iterator<Direction, Constness>
Digraph<V, E>::AdjacencyList<ALDirection, ALConstness>::Iterator<Direction, Constness>::operator-(std::uint64_t i) const noexcept {
	Iterator r(pos);
	for (; i; --r, --i);
	return r;
}

template <typename V, typename E>
template <DIRECTION ALDirection, CONSTNESS ALConstness>
template <DIRECTION Direction, CONSTNESS Constness>
class Digraph<V, E>::Adjacent<Constness>
Digraph<V, E>::AdjacencyList<ALDirection, ALConstness>::Iterator<Direction, Constness>::operator*() const noexcept
{ return {ALDirection == DIRECTION::Forward ? pos->out : pos->in, pos}; }

template <typename V, typename E>
template <DIRECTION ALDirection, CONSTNESS ALConstness>
template <DIRECTION Direction, CONSTNESS Constness>
template <DIRECTION OtherDirection, CONSTNESS OtherConstness>
bool
Digraph<V, E>::AdjacencyList<ALDirection, ALConstness>::Iterator<Direction, Constness>::operator==(Iterator<OtherDirection, OtherConstness> const& other) const noexcept
{ return pos == other.pos; }

template <typename V, typename E>
template <DIRECTION ALDirection, CONSTNESS ALConstness>
template <DIRECTION Direction, CONSTNESS Constness>
template <DIRECTION OtherDirection, CONSTNESS OtherConstness>
bool
Digraph<V, E>::AdjacencyList<ALDirection, ALConstness>::Iterator<Direction, Constness>::operator!=(Iterator<OtherDirection, OtherConstness> const& other) const noexcept
{ return pos != other.pos; }

template <typename V, typename E>
template <DIRECTION ALDirection, CONSTNESS ALConstness>
template <DIRECTION Direction, CONSTNESS Constness>
Digraph<V, E>::AdjacencyList<ALDirection, ALConstness>::Iterator<Direction, Constness>::operator bool() const noexcept
{ return pos; }

}//namespace DS

#endif //DS_DIGRAPH_HPP
