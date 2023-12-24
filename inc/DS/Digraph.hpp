#pragma once

#include "List.hpp"
#include "Map.hpp"
#include "Vector.hpp"
#include "../../src/DS/VNode.tpp"

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

	LNode<VNode<V, E>>* mVPivot{nullptr};
	LNode<ENode<V, E>>* mEPivot{nullptr};

	Vector<VNode<V, E>*>
	deserializeVertices(Stream::Input& input, auto&& ... vArgs)
	requires Stream::Deserializable<V, decltype(input), decltype(vArgs) ...>;

	template <typename VType, typename ... VArgs>
	Vector<VNode<V, E>*>
	deserializeVertices(Stream::Input& input, DP::Factory<V, VType, VArgs ...>, auto&& ... vArgs);

	void
	deserializeEdges(Vector<VNode<V, E>*> vs, Stream::Input& input, auto&& ... eArgs)
	requires Stream::Deserializable<E, decltype(input), decltype(eArgs) ...>;

	template <typename EType, typename ... EArgs>
	void
	deserializeEdges(Vector<VNode<V, E>*> vs, Stream::Input& input, DP::Factory<E, EType, EArgs ...>, auto&& ... eArgs);

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

	explicit Digraph(auto&& ... vArgs, Stream::Input& input, auto&& ... eArgs)
	requires Stream::Deserializable<V, decltype(input), decltype(vArgs) ...> && Stream::Deserializable<E, decltype(input), decltype(eArgs) ...>;

	template <typename EType, typename ... EArgs>
	Digraph(auto&& ... vArgs, Stream::Input& input, DP::Factory<E, EType, EArgs ...>, auto&& ... eArgs)
	requires Stream::Deserializable<V, decltype(input), decltype(vArgs) ...>;

	template <typename VType, typename ... VArgs>
	Digraph(DP::Factory<V, VType, VArgs ...>, auto&& ... vArgs, Stream::Input& input, auto&& ... eArgs)
	requires Stream::Deserializable<E, decltype(input), decltype(eArgs) ...>;

	template <typename VType, typename ... VArgs, typename EType, typename ... EArgs>
	Digraph(DP::Factory<V, VType, VArgs ...>, auto&& ... vArgs, Stream::Input& input, DP::Factory<E, EType, EArgs ...>, auto&& ... eArgs);

	template <typename v, typename e>
	friend Stream::Output&
	operator<<(Stream::Output& output, Digraph<v, e> const& digraph)
	requires Stream::InsertableTo<v, decltype(output)> && Stream::InsertableTo<e, decltype(output)>;

	~Digraph();

	vertex
	addVertex(auto&& ... vArgs);

	template <Derived<V> DV>
	vertex
	addVertex(auto&& ... dvArgs);

	template <typename ... VArgs>
	vertex
	addVertex(DP::CreateInfo<V, VArgs ...> const& vCreateInfo, auto&& ... vArgs);

	edge
	addEdge(auto&& ... eArgs);

	template <Derived<E> DE>
	edge
	addEdge(auto&& ... deArgs);

	template <typename ... EArgs>
	edge
	addEdge(DP::CreateInfo<E, EArgs ...> const& eCreateInfo, auto&& ... eArgs);

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
};//class DS::Digraph<V, E>

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
};//class DS::Digraph<V, E>::VDescriptor<Constness>

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
};//class DS::Digraph<V, E>::EDescriptor<Constness>

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
};//class DS::Digraph<V, E>::VView<Constness>

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
};//class DS::Digraph<V, E>::VView<Constness>::Iterator<Direction, Constness>

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
};//class DS::Digraph<V, E>::EView<Constness>

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
};//class DS::Digraph<V, E>::EView<Constness>::Iterator<Direction, Constness>

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
};//class DS::Digraph<V, E>::AdjacencyList<Direction, Constness>

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
};//class DS::Digraph<V, E>::AdjacencyList<Direction, Constness>::Iterator<Direction, Constness>

}//namespace DS

#include "../../src/DS/Digraph.tpp"
