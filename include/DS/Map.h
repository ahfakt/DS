#ifndef DS_MAP_H
#define DS_MAP_H

#include "DS/Container.h"
#include "DP/Factory.h"
#include "../src/MNode.hpp"
#include "StreamFormat/Dot.h"

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
	toDot(Stream::Format::DotOutput& dotOutput) const;

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

	Map() noexcept = default;

	Map(Map const& other)
	requires std::is_copy_constructible_v<K> && std::is_copy_constructible_v<V>;

	Map(Map&& other) noexcept;

	template <typename k, typename v, typename c, typename ... cs>
	friend void
	swap(Map<k, v, c, cs ...>& a, Map<k, v, c, cs ...>& b) noexcept;

	Map&
	operator=(Map value) noexcept;

	template <typename ... KArgs, typename ... VArgs>
	explicit Map(KArgs&& ... kArgs, Stream::Input& input, VArgs&& ... vArgs)
	requires Deserializable<K, Stream::Input&, KArgs ...> && Deserializable<V, Stream::Input&, VArgs ...>;

	template <typename ... KArgs, typename VIDType, typename ... VFArgs>
	Map(KArgs&& ... kArgs, Stream::Input& input, DP::Factory<V, VIDType, VFArgs ...> const& vFactory)
	requires Deserializable<K, Stream::Input&, KArgs ...>;

	template <typename KIDType, typename ... KFArgs, typename ... VArgs>
	Map(DP::Factory<K, KIDType, KFArgs ...> const& kFactory, Stream::Input& input, VArgs&& ... vArgs)
	requires Deserializable<V, Stream::Input&, VArgs ...>;

	template <typename KIDType, typename ... KFArgs, typename VIDType, typename ... VFArgs>
	Map(DP::Factory<K, KIDType, KFArgs ...> const& kFactory, Stream::Input& input, DP::Factory<V, VIDType, VFArgs ...> const& vFactory);

	template <typename k, typename v, typename c, typename ... cs>
	friend Stream::Output&
	operator<<(Stream::Output& output, Map<k, v, c, cs ...> const& map)
	requires Stream::Serializable<k, Stream::Output&> && Stream::Serializable<v, Stream::Output&>;

	template <typename k, typename v, typename c, typename ... cs>
	friend Stream::Format::DotOutput&
	operator<<(Stream::Format::DotOutput& dotOutput, Map<k, v, c, cs ...> const& map)
	requires Stream::Serializable<k, Stream::Format::DotOutput&> && Stream::Serializable<v, Stream::Format::DotOutput&>;

	~Map();

	template <typename ... KArgs>
	iterator<>
	put(KArgs&& ... kArgs);

	template <typename ... KArgs, Derived<V> DV>
	iterator<>
	put(KArgs&& ... kArgs);

	template <typename ... KArgs, typename ... VCIArgs>
	iterator<>
	put(KArgs&& ... kArgs, DP::CreateInfo<V, VCIArgs ...> const& vCreateInfo);


	template <EqDerived<K> DK, typename ... DKArgs>
	iterator<>
	put(DKArgs&& ... dkArgs);

	template <EqDerived<K> DK, typename ... DKArgs, Derived<V> DV>
	iterator<>
	put(DKArgs&& ... dkArgs);

	template <EqDerived<K> DK, typename ... DKArgs, typename ... VCIArgs>
	iterator<>
	put(DKArgs&& ... dkArgs, DP::CreateInfo<V, VCIArgs ...> const& vCreateInfo);


	template <typename ... KCIArgs, typename ... KCArgs>
	iterator<>
	put(DP::CreateInfo<K, KCIArgs ...> const& kCreateInfo, KCArgs&& ... kcArgs);

	template <typename ... KCIArgs, typename ... KCArgs, Derived<V> DV>
	iterator<>
	put(DP::CreateInfo<K, KCIArgs ...> const& kCreateInfo, KCArgs&& ... kcArgs);

	template <typename ... KCIArgs, typename ... KCArgs, typename ... VCIArgs>
	iterator<>
	put(DP::CreateInfo<K, KCIArgs ...> const& kCreateInfo, KCArgs&& ... kcArgs, DP::CreateInfo<V, VCIArgs ...> const& vCreateInfo);

	template <typename T, std::size_t N = 0>
	bool
	remove(T&& k) noexcept;

	template <typename T, std::size_t N = 0>
	iterator<N>
	get(T&& k) noexcept;

	template <typename T, std::size_t N = 0>
	const_iterator<N>
	get(T&& k) const noexcept;

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

	template <typename ... VArgs>
	V&
	set(VArgs&& ... vArgs) const
	requires (c == Constness::NCONST);

	template <Derived<V> DV, typename ... DVArgs>
	DV&
	set(DVArgs&& ... dvArgs) const
	requires (c == Constness::NCONST);

	template <typename ... VCIArgs, typename ... VCArgs>
	V&
	set(DP::CreateInfo<V, VCIArgs ...> const& vCreateInfo, VCArgs&& ... vcArgs) const
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

#include "../src/Map.hpp"

#endif //DS_MAP_H
