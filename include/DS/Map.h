#ifndef DS_MAP_H
#define DS_MAP_H

#include "Container.h"
#include "../../src/MNode.hpp"
#include <DP/Factory.h>
#include <StreamFormat/Dot.h>

namespace DS {

/**
 * @brief	Balanced search tree implementation.
 * @class	Map Map.h "DS/Map.h"
 * @tparam	K Key type of the mapped type to be stored in Map
 * @tparam	V Value type to be mapped by K in Map
 * @details	K can map any V and V-based objects in a Map object, even if V is an abstract class.
 */
template <typename K, typename V, typename C = std::less<>, typename ... Cs>
class Map : public Container {
	TNode<sizeof...(Cs) + 1>* mRoot[sizeof...(Cs) + 1] = {};

	template <std::size_t N = 0>
	MNode<K, V, C, Cs ...>*
	putAsRoot(MNode<K, V, C, Cs ...>* created) noexcept;

	template <std::size_t N = 0>
	MNode<K, V, C, Cs ...>*
	putToRoot(MNode<K, V, C, Cs ...>* created) noexcept;

	MNode<K, V, C, Cs ...>*
	put(MNode<K, V, C, Cs ...>* created) noexcept;

	template <std::size_t N = 0>
	MNode<K, V, C, Cs ...>*
	remove(MNode<K, V, C, Cs ...>* toDel) noexcept;

	template <std::size_t N = 0>
	Stream::Format::DotOutput&
	toDot(Stream::Format::DotOutput& dotOutput) const
	requires Stream::InsertableTo<K, Stream::Format::StringOutput> && Stream::InsertableTo<V, Stream::Format::StringOutput>;

public:
	template <Direction, Constness, std::size_t N = 0>
	class Iterator;

	template <std::size_t N = 0>
	using iterator = Iterator<Direction::FORWARD, Constness::NCONST, N>;

	template <std::size_t N = 0>
	using reverse_iterator = Iterator<Direction::BACKWARD, Constness::NCONST, N>;

	template <std::size_t N = 0>
	using const_iterator = Iterator<Direction::FORWARD, Constness::CONST, N>;

	template <std::size_t N = 0>
	using const_reverse_iterator = Iterator<Direction::BACKWARD, Constness::CONST, N>;

	struct Exception : std::system_error
	{ using std::system_error::system_error; };

	template <std::size_t N = 0>
	struct Difference {
		Map
		operator()(Map const& a, Map const& b) const;
	};

	template <typename S, std::size_t N = 0>
	struct Intersection {
		Map
		operator()(Map const& a, Map const& b, auto&& ... args) const
		requires Selector<S, K, decltype(args) ...>;
	};

	template <typename S, std::size_t N = 0>
	struct LeftJoin {
		Map
		operator()(Map const& a, Map const& b, auto&& ... args) const
		requires Selector<S, K, decltype(args) ...>;
	};

	template <typename S, std::size_t N = 0>
	struct Union {
		Map
		operator()(Map const& a, Map const& b, auto&& ... args) const
		requires Selector<S, K, decltype(args) ...>;
	};

	Map() noexcept = default;

	Map(Map const& other)
	requires std::is_copy_constructible_v<K> && std::is_copy_constructible_v<V>;

	Map(Map&& other) noexcept;

	template <typename k, typename v, typename c, typename ... cs>
	friend void
	swap(Map<k, v, c, cs ...>& a, Map<k, v, c, cs ...>& b) noexcept;

	Map&
	operator=(Map value) noexcept;

	explicit Map(auto&& ... kArgs, Stream::Input& input, auto&& ... vArgs)
	requires Stream::DeserializableWith<K, Stream::Input, decltype(kArgs) ...> && Stream::DeserializableWith<V, Stream::Input, decltype(vArgs) ...>;

	template <typename VIDType, typename ... VFArgs>
	Map(auto&& ... kArgs, Stream::Input& input, DP::Factory<V, VIDType, VFArgs ...> const& vFactory)
	requires Stream::DeserializableWith<K, Stream::Input, decltype(kArgs) ...>;

	template <typename KIDType, typename ... KFArgs>
	Map(DP::Factory<K, KIDType, KFArgs ...> const& kFactory, Stream::Input& input, auto&& ... vArgs)
	requires Stream::DeserializableWith<V, Stream::Input, decltype(vArgs) ...>;

	template <typename KIDType, typename ... KFArgs, typename VIDType, typename ... VFArgs>
	Map(DP::Factory<K, KIDType, KFArgs ...> const& kFactory, Stream::Input& input, DP::Factory<V, VIDType, VFArgs ...> const& vFactory);

	template <typename k, typename v, typename c, typename ... cs>
	friend Stream::Output&
	operator<<(Stream::Output& output, Map<k, v, c, cs ...> const& map)
	requires Stream::InsertableTo<k, Stream::Output> && Stream::InsertableTo<v, Stream::Output>;

	template <typename k, typename v, typename c, typename ... cs>
	friend Stream::Format::DotOutput&
	operator<<(Stream::Format::DotOutput& dotOutput, Map<k, v, c, cs ...> const& map)
	requires Stream::InsertableTo<k, Stream::Format::StringOutput> && Stream::InsertableTo<v, Stream::Format::StringOutput>;

	~Map();

	iterator<>
	put(auto&& ... kArgs);

	template <Derived<V> DV>
	iterator<>
	put(auto&& ... kArgs);

	template <typename ... VCIArgs>
	iterator<>
	put(auto&& ... kArgs, DP::CreateInfo<V, VCIArgs ...> const& vCreateInfo);


	template <EqDerived<K> DK>
	iterator<>
	put(auto&& ... dkArgs);

	template <EqDerived<K> DK, Derived<V> DV>
	iterator<>
	put(auto&& ... dkArgs);

	template <EqDerived<K> DK, typename ... VCIArgs>
	iterator<>
	put(auto&& ... dkArgs, DP::CreateInfo<V, VCIArgs ...> const& vCreateInfo);


	template <typename ... KCIArgs>
	iterator<>
	put(DP::CreateInfo<K, KCIArgs ...> const& kCreateInfo, auto&& ... kcArgs);

	template <typename ... KCIArgs, Derived<V> DV>
	iterator<>
	put(DP::CreateInfo<K, KCIArgs ...> const& kCreateInfo, auto&& ... kcArgs);

	template <typename ... KCIArgs, typename ... VCIArgs>
	iterator<>
	put(DP::CreateInfo<K, KCIArgs ...> const& kCreateInfo, auto&& ... kcArgs, DP::CreateInfo<V, VCIArgs ...> const& vCreateInfo);

	template <std::size_t N = 0>
	bool
	remove(auto&& ... args) noexcept;

	template <std::size_t N = 0>
	iterator<N>
	get(auto&& ... args) noexcept;

	template <std::size_t N = 0>
	const_iterator<N>
	get(auto&& ... args) const noexcept;

	template <std::size_t N = 0>
	iterator<N>
	at(std::uint64_t i) noexcept;

	template <std::size_t N = 0>
	const_iterator<N>
	at(std::uint64_t i) const noexcept;

	template <std::size_t N = 0>
	iterator<N>
	begin() noexcept;

	template <std::size_t N = 0>
	const_iterator<N>
	begin() const noexcept;

	template <std::size_t N = 0>
	iterator<N>
	end() noexcept;

	template <std::size_t N = 0>
	const_iterator<N>
	end() const noexcept;

	template <std::size_t N = 0>
	reverse_iterator<N>
	rbegin() noexcept;

	template <std::size_t N = 0>
	const_reverse_iterator<N>
	rbegin() const noexcept;

	template <std::size_t N = 0>
	reverse_iterator<N>
	rend() noexcept;

	template <std::size_t N = 0>
	const_reverse_iterator<N>
	rend() const noexcept;

	template <std::size_t N = 0>
	const_iterator<N>
	cbegin() const noexcept;

	template <std::size_t N = 0>
	const_iterator<N>
	cend() const noexcept;

	template <std::size_t N = 0>
	const_reverse_iterator<N>
	crbegin() const noexcept;

	template <std::size_t N = 0>
	const_reverse_iterator<N>
	crend() const noexcept;
};//class Map<K, V, C, Cs ...>

template <typename K, typename V, typename C, typename ... Cs>
template <Direction d, Constness c, std::size_t n>
class Map<K, V, C, Cs ...>::Iterator {
	friend class Map;

protected:
	TNode<sizeof...(Cs) + 1>* pos;

	Iterator(TNode<sizeof...(Cs) + 1>* pos) noexcept;

public:
	template <Direction od, Constness oc, std::size_t on>
	Iterator(Iterator<od, oc, on> const& other) noexcept
	requires ConstCompat<c, oc>;

	template <Direction od, Constness oc, std::size_t on>
	Iterator&
	operator=(Iterator<od, oc, on> const& other) noexcept
	requires ConstCompat<c, oc>;

	[[nodiscard]] bool
	hasValue() const noexcept;

	void
	unset() const noexcept
	requires (c == Constness::NCONST);

	V&
	set(auto&& ... vArgs) const
	requires (c == Constness::NCONST);

	template <Derived<V> DV>
	DV&
	set(auto&& ... dvArgs) const
	requires (c == Constness::NCONST);

	template <typename ... VCIArgs>
	V&
	set(DP::CreateInfo<V, VCIArgs ...> const& vCreateInfo, auto&& ... vcArgs) const
	requires (c == Constness::NCONST);

	Iterator&
	operator++() noexcept;

	Iterator
	operator++(int) noexcept;

	Iterator&
	operator--() noexcept;

	Iterator
	operator--(int) noexcept;

	std::pair<K const, TConstness<V, c>>&
	operator*() const noexcept;

	std::pair<K const, TConstness<V, c>>*
	operator->() const noexcept;

	template <Direction od, Constness oc, std::size_t on>
	bool
	operator==(Iterator<od, oc, on> const& other) const noexcept;

	explicit operator bool() const noexcept;
};//class Map<K, V, C, Cs ...>::Iterator<Direction, Constness, std::size_t>

}//namespace DS

#include "../../src/Map.hpp"

#endif //DS_MAP_H
