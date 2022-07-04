#ifndef DS_DIGRAPH_H
#define DS_DIGRAPH_H

#include "DS/List.h"
#include "DS/Map.h"
#include "DS/Vector.h"
#include "../src/VNode.hpp"

namespace DS {

template <typename V, typename E>
class Digraph {
	union {
		List<VNode<V, E>> mVertices{};
		struct {
			std::uint64_t mVerticesSize;
			LNode<VNode<V, E>>* mVHead;
			LNode<VNode<V, E>>* mVTail;
		};
	};
	union {
		List<ENode<V, E>> mEdges{};
		struct {
			std::uint64_t mEdgesSize;
			LNode<ENode<V, E>>* mEHead;
			LNode<ENode<V, E>>* mETail;
		};
	};

	LNode<VNode<V, E>>* mVPivot = nullptr;
	LNode<ENode<V, E>>* mEPivot = nullptr;

	template <typename ... VArgs>
	Vector<VNode<V, E>*>
	deserializeVertices(Stream::Input& input, VArgs&& ... vArgs)
	requires Deserializable<V, Stream::Input&, VArgs ...>;

	template <typename VIDType, typename ... VFArgs>
	Vector<VNode<V, E>*>
	deserializeVertices(Stream::Input& input, DP::Factory<V, VIDType, VFArgs ...> const& vFactory);

	template <typename ... EArgs>
	void
	deserializeEdges(Vector<VNode<V, E>*> vs, Stream::Input& input, EArgs&& ... eArgs)
	requires Deserializable<E, Stream::Input&, EArgs ...>;

	template <typename EIDType, typename ... EFArgs>
	void
	deserializeEdges(Vector<VNode<V, E>*> vs, Stream::Input& input, DP::Factory<E, EIDType, EFArgs ...> const& eFactory);

public:
	template <Constness>
	class VDescriptor;

	using vertex = VDescriptor<Constness::NCONST>;
	using const_vertex = VDescriptor<Constness::CONST>;

	template <Constness>
	class EDescriptor;

	using edge = EDescriptor<Constness::NCONST>;
	using const_edge = EDescriptor<Constness::CONST>;

	template <Constness>
	class VView;

	using vertex_view = VView<Constness::NCONST>;
	using const_vertex_view = VView<Constness::CONST>;

	template <Constness>
	class EView;

	using edge_view = EView<Constness::NCONST>;
	using const_edge_view = EView<Constness::CONST>;

	template <Direction, Constness>
	class AdjacencyList;

	Digraph() noexcept = default;

	Digraph(Digraph const& other)
	requires std::is_copy_constructible_v<V> && std::is_copy_constructible_v<E>;

	Digraph(Digraph&& other) noexcept;

	template <typename v, typename e>
	friend void
	swap(Digraph<v, e>& a, Digraph<v, e>& b) noexcept;

	Digraph&
	operator=(Digraph value) noexcept;

	template <typename ... VArgs, typename ... EArgs>
	Digraph(VArgs&& ... vArgs, Stream::Input& input, EArgs&& ... eArgs)
	requires Deserializable<V, Stream::Input&, VArgs ...> && Deserializable<E, Stream::Input&, EArgs ...>;

	template <typename ... VArgs, typename EIDType, typename ... EFArgs>
	Digraph(VArgs&& ... vArgs, Stream::Input& input, DP::Factory<E, EIDType, EFArgs ...> const& eFactory)
	requires Deserializable<V, Stream::Input&, VArgs ...>;

	template <typename VIDType, typename ... VFArgs, typename ... EArgs>
	Digraph(DP::Factory<V, VIDType, VFArgs ...> const& vFactory, Stream::Input& input, EArgs&& ... eArgs)
	requires Deserializable<E, Stream::Input&, EArgs ...>;

	template <typename VIDType, typename ... VFArgs, typename EIDType, typename ... EFArgs>
	Digraph(DP::Factory<V, VIDType, VFArgs ...> const& vFactory, Stream::Input& input, DP::Factory<E, EIDType, EFArgs ...> const& eFactory);

	template <typename v, typename e>
	friend Stream::Output&
	operator<<(Stream::Output& output, Digraph<v, e> const& digraph)
	requires Stream::Serializable<v, Stream::Output&> && Stream::Serializable<e, Stream::Output&>;

	~Digraph();

	template <typename ... VArgs>
	vertex
	addVertex(VArgs&& ... vArgs);

	template <Derived<V> DV, typename ... DVArgs>
	vertex
	addVertex(DVArgs&& ... dvArgs);

	template <typename ... VCIArgs, typename ... VCArgs>
	vertex
	addVertex(DP::CreateInfo<V, VCIArgs ...> const& vCreateInfo, VCArgs&& ... cArgs);

	template <typename ... EArgs>
	edge
	addEdge(EArgs&& ... eArgs);

	template <Derived<E> DE, typename ... DEArgs>
	edge
	addEdge(DEArgs&& ... deArgs);

	template <typename ... ECIArgs, typename ... ECArgs>
	edge
	addEdge(DP::CreateInfo<E, ECIArgs ...> const& eCreateInfo, ECArgs&& ... ecArgs);

	template <Constness c>
	Digraph&
	remove(VDescriptor<c> v) noexcept;

	template <Constness c>
	Digraph&
	remove(EDescriptor<c> e) noexcept;

	template <Constness c>
	Digraph&
	prepareTraverse(VDescriptor<c> v) noexcept;

	template <Traverse t, Order o = Order::PREORDER, Direction d = Direction::FORWARD>
	Digraph&
	prepareTraverse() noexcept;

	Digraph&
	resetTraverse() noexcept;

	vertex_view
	getVertexView() noexcept;

	edge_view
	getEdgeView() noexcept;
};//class Digraph<V, E>

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::VDescriptor {
	friend class Digraph;

protected:
	VNode<V, E>* pos;

	VDescriptor(VNode<V, E>* pos) noexcept;

public:
	template <Constness oc>
	explicit VDescriptor(VDescriptor<oc> const& other) noexcept
	requires ConstCompat<c, oc>;

	template <Constness oc>
	VDescriptor&
	operator=(VDescriptor<oc> const& other) noexcept
	requires ConstCompat<c, oc>;

	[[nodiscard]] bool
	hasIn() const noexcept;

	[[nodiscard]] bool
	hasOut() const noexcept;

	[[nodiscard]] std::uint64_t
	getInDegree() const noexcept;

	[[nodiscard]] std::uint64_t
	getOutDegree() const noexcept;

	template <Direction d>
	AdjacencyList<d, c>
	getAdjacencyList() const noexcept;

	VDescriptor const&
	unsetIn() const noexcept
	requires (c == Constness::NCONST);

	VDescriptor const&
	unsetOut() const noexcept
	requires (c == Constness::NCONST);

	TConstness<V, c>&
	operator*() const noexcept;

	TConstness<V, c>*
	operator->() const noexcept;

	template <Constness oc>
	bool
	operator==(VDescriptor<oc> const& other) const noexcept;

	explicit operator bool() const noexcept;
};//class Digraph<V, E>::VDescriptor<Constness>

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::EDescriptor {
	friend class Digraph;

protected:
	ENode<V, E>* pos;

	EDescriptor(ENode<V, E>* pos) noexcept;

public:
	template <Constness oc>
	explicit EDescriptor(EDescriptor<oc> const& other) noexcept
	requires ConstCompat<c, oc>;

	template <Constness oc>
	EDescriptor&
	operator=(EDescriptor<oc> const& other) noexcept
	requires ConstCompat<c, oc>;

	[[nodiscard]] bool
	hasIn() const noexcept;

	[[nodiscard]] bool
	hasOut() const noexcept;

	VDescriptor<c>
	getIn() const noexcept;

	VDescriptor<c>
	getOut() const noexcept;

	EDescriptor const&
	unsetIn() const noexcept
	requires (c == Constness::NCONST);

	EDescriptor const&
	unsetOut() const noexcept
	requires (c == Constness::NCONST);

	EDescriptor const&
	setIn(VDescriptor<c> v) const noexcept
	requires (c == Constness::NCONST);

	EDescriptor const&
	setOut(VDescriptor<c> v) const noexcept
	requires (c == Constness::NCONST);

	TConstness<E, c>&
	operator*() const noexcept;

	TConstness<E, c>*
	operator->() const noexcept;

	template <Constness oc>
	bool
	operator==(EDescriptor<oc> const& other) const noexcept;

	explicit operator bool() const noexcept;
};//class Digraph<V, E>::EDescriptor<Constness>

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::VView {
	friend class Digraph;

protected:
	LNode<VNode<V, E>>* mHead;
	LNode<VNode<V, E>>* mTail;

	VView(LNode<VNode<V, E>>* head, LNode<VNode<V, E>>* tail) noexcept;

public:
	template <Direction, Constness>
	class Iterator;

	using iterator = Iterator<Direction::FORWARD, Constness::NCONST>;
	using reverse_iterator = Iterator<Direction::BACKWARD, Constness::NCONST>;
	using const_iterator = Iterator<Direction::FORWARD, Constness::CONST>;
	using const_reverse_iterator = Iterator<Direction::BACKWARD, Constness::CONST>;

	iterator
	begin() noexcept
	requires (c == Constness::NCONST);

	const_iterator
	begin() const noexcept;

	iterator
	end() noexcept
	requires (c == Constness::NCONST);

	const_iterator
	end() const noexcept;

	reverse_iterator
	rbegin() noexcept
	requires (c == Constness::NCONST);

	const_reverse_iterator
	rbegin() const noexcept;

	reverse_iterator
	rend() noexcept
	requires (c == Constness::NCONST);

	const_reverse_iterator
	rend() const noexcept;

	const_iterator
	cbegin() const noexcept;

	const_iterator
	cend() const noexcept;

	const_reverse_iterator
	crbegin() const noexcept;

	const_reverse_iterator
	crend() const noexcept;
};//class Digraph<V, E>::VView<Constness>

template <typename V, typename E>
template <Constness vvc>
template <Direction d, Constness c>
class Digraph<V, E>::VView<vvc>::Iterator : public VDescriptor<c> {
	friend class VView;

public:
	using VDescriptor<c>::VDescriptor;

	template <Constness oc>
	Iterator&
	operator=(VDescriptor<oc> const& other) noexcept
	requires ConstCompat<c, oc>;

	Iterator&
	operator++() noexcept;

	Iterator
	operator++(int) noexcept;

	Iterator&
	operator--() noexcept;

	Iterator
	operator--(int) noexcept;

	VDescriptor<c> const&
	operator*() const noexcept;

	VDescriptor<c> const*
	operator->() const noexcept;
};//class Digraph<V, E>::VView<Constness>::Iterator<Direction, Constness>

template <typename V, typename E>
template <Constness c>
class Digraph<V, E>::EView {
	friend class Digraph;

protected:
	LNode<ENode<V, E>>* mHead;
	LNode<ENode<V, E>>* mTail;

	EView(LNode<ENode<V, E>>* head, LNode<ENode<V, E>>* tail) noexcept;

public:
	template <Direction, Constness>
	class Iterator;

	using iterator = Iterator<Direction::FORWARD, Constness::NCONST>;
	using reverse_iterator = Iterator<Direction::BACKWARD, Constness::NCONST>;
	using const_iterator = Iterator<Direction::FORWARD, Constness::CONST>;
	using const_reverse_iterator = Iterator<Direction::BACKWARD, Constness::CONST>;

	iterator
	begin() noexcept
	requires (c == Constness::NCONST);

	const_iterator
	begin() const noexcept;

	iterator
	end() noexcept
	requires (c == Constness::NCONST);

	const_iterator
	end() const noexcept;

	reverse_iterator
	rbegin() noexcept
	requires (c == Constness::NCONST);

	const_reverse_iterator
	rbegin() const noexcept;

	reverse_iterator
	rend() noexcept
	requires (c == Constness::NCONST);

	const_reverse_iterator
	rend() const noexcept;

	const_iterator
	cbegin() const noexcept;

	const_iterator
	cend() const noexcept;

	const_reverse_iterator
	crbegin() const noexcept;

	const_reverse_iterator
	crend() const noexcept;
};//class Digraph<V, E>::EView<Constness>

template <typename V, typename E>
template <Constness evc>
template <Direction d, Constness c>
class Digraph<V, E>::EView<evc>::Iterator : public EDescriptor<c> {
	friend class EView;

public:
	using EDescriptor<c>::EDescriptor;

	template <Constness oc>
	Iterator&
	operator=(EDescriptor<oc> const& other) noexcept
	requires ConstCompat<c, oc>;

	Iterator&
	operator++() noexcept;

	Iterator
	operator++(int) noexcept;

	Iterator&
	operator--() noexcept;

	Iterator
	operator--(int) noexcept;

	EDescriptor<c> const&
	operator*() const noexcept;

	EDescriptor<c> const*
	operator->() const noexcept;
};//class Digraph<V, E>::EView<Constness>::Iterator<Direction, Constness>

template <typename V, typename E>
template <Direction d, Constness c>
class Digraph<V, E>::AdjacencyList : public VDescriptor<c> {
	friend class VDescriptor<c>;

public:
	using VDescriptor<c>::VDescriptor;

	template <Constness oc>
	AdjacencyList&
	operator=(VDescriptor<oc> const& other) noexcept
	requires ConstCompat<c, oc>;

	template <Direction, Constness>
	class Iterator;

	using iterator = Iterator<Direction::FORWARD, Constness::NCONST>;
	using reverse_iterator = Iterator<Direction::BACKWARD, Constness::NCONST>;
	using const_iterator = Iterator<Direction::FORWARD, Constness::CONST>;
	using const_reverse_iterator = Iterator<Direction::BACKWARD, Constness::CONST>;

	iterator
	begin() noexcept
	requires (c == Constness::NCONST);

	const_iterator
	begin() const noexcept;

	iterator
	end() noexcept
	requires (c == Constness::NCONST);

	const_iterator
	end() const noexcept;

	reverse_iterator
	rbegin() noexcept
	requires (c == Constness::NCONST);

	const_reverse_iterator
	rbegin() const noexcept;

	reverse_iterator
	rend() noexcept
	requires (c == Constness::NCONST);

	const_reverse_iterator
	rend() const noexcept;

	const_iterator
	cbegin() const noexcept;

	const_iterator
	cend() const noexcept;

	const_reverse_iterator
	crbegin() const noexcept;

	const_reverse_iterator
	crend() const noexcept;
};//class Digraph<V, E>::AdjacencyList<Direction, Constness>

template <typename V, typename E>
template <Direction ald, Constness alc>
template <Direction d, Constness c>
class Digraph<V, E>::AdjacencyList<ald, alc>::Iterator : public EDescriptor<c> {
	friend class AdjacencyList;

public:
	using EDescriptor<c>::EDescriptor;

	template <Constness oc>
	Iterator&
	operator=(EDescriptor<oc> const& other) noexcept
	requires ConstCompat<c, oc>;

	Iterator&
	operator++() noexcept;

	Iterator
	operator++(int) noexcept;

	Iterator&
	operator--() noexcept;

	Iterator
	operator--(int) noexcept;

	EDescriptor<c> const&
	operator*() const noexcept;

	EDescriptor<c> const*
	operator->() const noexcept;
};//class Digraph<V, E>::AdjacencyList<Direction, Constness>::Iterator<Direction, Constness>

}//namespace DS

#include "../src/Digraph.hpp"

#endif //DS_DIGRAPH_H
