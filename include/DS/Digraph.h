#ifndef DS_DIGRAPH_H
#define DS_DIGRAPH_H

#include "List.h"
#include "../src/VNode.hpp"

namespace DS {

template <typename V, typename E>
class Digraph {
	union {
		List<VNode<V, E>> mVertices;
		struct {
			std::uint64_t mVerticesSize;
			LNode<VNode<V, E>> *mVHead, *mVTail;
		};
	};
	union {
		List<ENode<V, E>> mEdges;
		struct {
			std::uint64_t mEdgesSize;
			LNode<ENode<V, E>> *mEHead, *mETail;
		};
	};
	template <TRAVERSE, ORDER, DIRECTION>
	void traverse(List<VNode<V, E>*>& result) const;
	template <TRAVERSE, ORDER, DIRECTION>
	void traverse(List<VNode<V, E>*>& result, VNode<V, E>* vertex) const;
	template <TRAVERSE, ORDER, DIRECTION>
	void traverse(List<VNode<V, E>*>& result, List<VNode<V, E>*> const& vertexList) const;
public:
	template <CONSTNESS>
	class Vertex;

	template <CONSTNESS>
	class Edge;

	template <DIRECTION, CONSTNESS>
	class AdjacencyList;

	template <CONSTNESS Constness>
	struct Adjacent {
		Vertex<Constness> vertex;
		Edge<Constness> edge;
	};

	Digraph() noexcept;
	Digraph(Digraph&&) noexcept;
	Digraph(Digraph const&);
	Digraph& operator=(Digraph&&) noexcept;
	Digraph& operator=(Digraph const&);
	~Digraph();

	template <typename v = V, typename e = E>
	Digraph(std::enable_if_t<(Stream::isDeserializable<v> || std::is_trivially_copyable_v<v>) && (Stream::isDeserializable<e> || std::is_trivially_copyable_v<e>), Stream::Input>&);

	template <typename v = V, typename e = E>
	Digraph& operator=(std::enable_if_t<(Stream::isDeserializable<v> || std::is_trivially_copyable_v<v>) && (Stream::isDeserializable<e> || std::is_trivially_copyable_v<e>), Stream::Input>&);

	template <typename v, typename e>
	friend std::enable_if_t<(Stream::isDeserializable<v> || std::is_trivially_copyable_v<v>) && (Stream::isDeserializable<e> || std::is_trivially_copyable_v<e>), Stream::Output&>
	operator<<(Stream::Output& os, Digraph<v, e> const& digraph);

	template <typename ... Args>
	Vertex<CONSTNESS::NonConst> addVertex(Args&& ...);

	template <typename D, typename ... Args>
	Vertex<CONSTNESS::NonConst> addVertex(Args&& ...);

	template <typename ... Args>
	Edge<CONSTNESS::NonConst> addEdge(Args&& ...);

	template <typename D, typename ... Args>
	Edge<CONSTNESS::NonConst> addEdge(Args&& ...);

	template <CONSTNESS Constness>
	void remove(Vertex<Constness> v) noexcept;

	template <CONSTNESS Constness>
	void remove(Edge<Constness> e) noexcept;

	template <TRAVERSE, ORDER, DIRECTION, CONSTNESS Constness>
	List<Vertex<CONSTNESS::NonConst>>
	traverse(Vertex<Constness> const& vertex);

	template <TRAVERSE, ORDER, DIRECTION, CONSTNESS Constness>
	List<Vertex<CONSTNESS::Const>>
	traverse(Vertex<Constness> const& vertex) const;

	template <TRAVERSE, ORDER, DIRECTION, CONSTNESS Constness>
	List<Vertex<CONSTNESS::NonConst>>
	traverse(List<Vertex<Constness>> const& vertexList);

	template <TRAVERSE, ORDER, DIRECTION, CONSTNESS Constness>
	List<Vertex<CONSTNESS::Const>>
	traverse(List<Vertex<Constness>> const& vertexList) const;
};//class Digraph<V, E>

template <typename V, typename E>
template <CONSTNESS Constness>
class Digraph<V, E>::Vertex {
	friend class Digraph;
protected:
	VNode<V, E>* pos;
	Vertex(VNode<V, E>* pos) noexcept;
public:
	Vertex(Vertex const&) = default;
	Vertex& operator=(Vertex const&) = default;

	template <CONSTNESS OtherConstness,
			typename = std::enable_if_t<Constness == CONSTNESS::Const && OtherConstness == CONSTNESS::NonConst>>
	Vertex(Vertex<OtherConstness> const&) noexcept;

	template <CONSTNESS OtherConstness,
		typename = std::enable_if_t<Constness == CONSTNESS::Const && OtherConstness == CONSTNESS::NonConst>>
	Vertex& operator=(Vertex<OtherConstness> const&) noexcept;

	Vertex&
	setVisited(bool) const noexcept;

	std::uint64_t
	getInDegree() const noexcept;

	std::uint64_t
	getOutDegree() const noexcept;

	AdjacencyList<DIRECTION::Backward, Constness>
	getInList() const noexcept;

	AdjacencyList<DIRECTION::Forward, Constness>
	getOutList() const noexcept;

	std::conditional_t<Constness == CONSTNESS::Const, V const, V>&
	operator*() const noexcept;

	std::conditional_t<Constness == CONSTNESS::Const, V const, V>*
	operator->() const noexcept;

	template <CONSTNESS OtherConstness>
	bool operator==(Vertex<OtherConstness> const&) const noexcept;

	template <CONSTNESS OtherConstness>
	bool operator!=(Vertex<OtherConstness> const&) const noexcept;

	explicit operator bool() const noexcept;
};//class Digraph<V, E>::Vertex<CONSTNESS>

template <typename V, typename E>
template <CONSTNESS Constness>
class Digraph<V, E>::Edge {
	friend class Digraph<V, E>;
protected:
	ENode<V, E>* pos;
	Edge(ENode<V, E>* pos) noexcept;
public:
	Edge(Edge const&) = default;
	Edge& operator=(Edge const&) = default;

	template <CONSTNESS OtherConstness,
			typename = std::enable_if_t<Constness == CONSTNESS::Const && OtherConstness == CONSTNESS::NonConst>>
	Edge(Edge<OtherConstness> const&) noexcept;

	template <CONSTNESS OtherConstness,
		typename = std::enable_if_t<Constness == CONSTNESS::Const && OtherConstness == CONSTNESS::NonConst>>
	Edge& operator=(Edge<OtherConstness> const&) noexcept;

	Vertex<Constness> getIn() const noexcept;
	Vertex<Constness> getOut() const noexcept;

	template <typename = std::enable_if_t<Constness == CONSTNESS::NonConst>>
	Edge<CONSTNESS::NonConst>&
	setIn(Vertex<CONSTNESS::NonConst>& in) noexcept;

	template <typename = std::enable_if_t<Constness == CONSTNESS::NonConst>>
	Edge<CONSTNESS::NonConst> const&
	setIn(Vertex<CONSTNESS::NonConst>& in) const noexcept;

	template <typename = std::enable_if_t<Constness == CONSTNESS::NonConst>>
	Edge<CONSTNESS::NonConst>&
	setOut(Vertex<CONSTNESS::NonConst>& out) noexcept;

	template <typename = std::enable_if_t<Constness == CONSTNESS::NonConst>>
	Edge<CONSTNESS::NonConst> const&
	setOut(Vertex<CONSTNESS::NonConst>& out) const noexcept;

	std::conditional_t<Constness == CONSTNESS::Const, E const, E>&
	operator*() const noexcept;

	std::conditional_t<Constness == CONSTNESS::Const, E const, E>*
	operator->() const noexcept;

	template <CONSTNESS OtherConstness>
	bool operator==(Edge<OtherConstness> const&) const noexcept;

	template <CONSTNESS OtherConstness>
	bool operator!=(Edge<OtherConstness> const&) const noexcept;

	explicit operator bool() const noexcept;
};//class Digraph<V, E>::Edge<CONSTNESS>

template <typename V, typename E>
template <DIRECTION Direction, CONSTNESS Constness>
class Digraph<V, E>::AdjacencyList {
	friend class Vertex<Constness>;
protected:
	VNode<V, E>* pos;
	AdjacencyList(VNode<V, E>* pos) noexcept;
public:
	template <DIRECTION, CONSTNESS>
	class Iterator;

	AdjacencyList(AdjacencyList const&) = default;
	AdjacencyList& operator=(AdjacencyList const&) = default;

	template <DIRECTION OtherDirection, CONSTNESS OtherConstness,
			typename = std::enable_if_t<Constness == CONSTNESS::Const && OtherConstness == CONSTNESS::NonConst>>
	AdjacencyList(AdjacencyList<OtherDirection, OtherConstness> const&) noexcept;

	template <DIRECTION OtherDirection, CONSTNESS OtherConstness,
		typename = std::enable_if_t<Constness == CONSTNESS::Const && OtherConstness == CONSTNESS::NonConst>>
	AdjacencyList& operator=(AdjacencyList<OtherDirection, OtherConstness> const&) noexcept;

	explicit operator bool() const noexcept;

	template <std::enable_if_t<Constness == CONSTNESS::NonConst, CONSTNESS> IConstness = Constness>
	Iterator<DIRECTION::Forward, IConstness> begin() noexcept;

	template <std::enable_if_t<Constness == CONSTNESS::NonConst, CONSTNESS> IConstness = Constness>
	Iterator<DIRECTION::Backward, IConstness> rbegin() noexcept;

	Iterator<DIRECTION::Forward, CONSTNESS::Const> begin() const noexcept;
	Iterator<DIRECTION::Backward, CONSTNESS::Const> rbegin() const noexcept;
	Iterator<DIRECTION::Forward, CONSTNESS::Const> end() const noexcept;
	Iterator<DIRECTION::Backward, CONSTNESS::Const> rend() const noexcept;
};//class Digraph<V, E>::AdjacencyList<DIRECTION, CONSTNESS>

template <typename V, typename E>
template <DIRECTION ALDirection, CONSTNESS ALConstness>
template <DIRECTION Direction, CONSTNESS Constness>
class Digraph<V, E>::AdjacencyList<ALDirection, ALConstness>::Iterator {
	friend class AdjacencyList<ALDirection, ALConstness>;
protected:
	ENode<V, E>* pos;
	Iterator(ENode<V, E>* pos) noexcept;
public:
	Iterator(Iterator const&) = default;
	Iterator& operator=(Iterator const&) = default;

	template <DIRECTION OtherDirection, CONSTNESS OtherConstness,
			typename = std::enable_if_t<Constness == CONSTNESS::Const && OtherConstness == CONSTNESS::NonConst>>
	Iterator(Iterator<OtherDirection, OtherConstness> const&) noexcept;

	template <DIRECTION OtherDirection, CONSTNESS OtherConstness,
			typename = std::enable_if_t<Constness == CONSTNESS::Const && OtherConstness == CONSTNESS::NonConst>>
	Iterator& operator=(Iterator<OtherDirection, OtherConstness> const&) noexcept;

	Iterator& operator++() noexcept;
	Iterator operator++(int) noexcept;
	Iterator& operator--() noexcept;
	Iterator operator--(int) noexcept;

	Iterator operator+(std::uint64_t) const noexcept;
	Iterator operator-(std::uint64_t) const noexcept;

	Adjacent<Constness>
	operator*() const noexcept;

	template <DIRECTION OtherDirection, CONSTNESS OtherConstness>
	bool operator==(Iterator<OtherDirection, OtherConstness> const&) const noexcept;

	template <DIRECTION OtherDirection, CONSTNESS OtherConstness>
	bool operator!=(Iterator<OtherDirection, OtherConstness> const&) const noexcept;

	explicit operator bool() const noexcept;
};//class Digraph<V, E>::AdjacencyList<DIRECTION, CONSTNESS>::Iterator<DIRECTION, CONSTNESS>

}//namespace DS

#include "../src/Digraph.hpp"

#endif //DS_DIGRAPH_H
