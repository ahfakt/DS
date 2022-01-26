#ifndef DS_MAP_H
#define DS_MAP_H

#include "DS/Container.h"
#include "DP/Factory.h"
#include "../src/MNode.hpp"

namespace DS {

/**
 * @brief	Balanced search tree implementation.
 * @class	Map Map.h "DS/Map.h"
 * @tparam	K Key type of the mapped type to be stored in Map
 * @tparam	V Value type to be mapped by K in Map
 * @details	K can map any V and V-based objects in a Map object, even if V is an abstract class.
 */
template <typename K, typename V>
class Map : public Container {
	MNode<K, V>* mRoot = nullptr;

	MNode<K, V>*
	first() const noexcept;

	MNode<K, V>*
	last() const noexcept;

	MNode<K, V>*
	put(MNode<K, V>* created) noexcept;

public:
	template <Direction, Constness>
	class Iterator;

	using iterator = Iterator<Direction::FORWARD, Constness::NCONST>;
	using reverse_iterator = Iterator<Direction::BACKWARD, Constness::NCONST>;
	using const_iterator = Iterator<Direction::FORWARD, Constness::CONST>;
	using const_reverse_iterator = Iterator<Direction::BACKWARD, Constness::CONST>;

	template <Constness c>
	class Entry {
		std::uint8_t _;
	public:
		K const key;
		TConstness<V, c> value;
	};

	struct Exception : std::runtime_error
	{ using std::runtime_error::runtime_error; };

	Map() noexcept = default;

	Map(Map const& other)
	requires std::is_copy_constructible_v<K> && std::is_copy_constructible_v<V>;

	Map(Map&& other) noexcept;

	template <typename k, typename v>
	friend void
	swap(Map<k, v>& a, Map<k, v>& b) noexcept;

	Map&
	operator=(Map value) noexcept;

	template <typename ... KArgs, typename ... VArgs>
	Map(KArgs&& ... kArgs, Stream::Input& input, VArgs&& ... vArgs)
	requires Deserializable<K, Stream::Input, KArgs ...> && Deserializable<V, Stream::Input, VArgs ...>;

	template <typename ... KArgs, typename VIDType, typename ... VFArgs>
	Map(KArgs&& ... kArgs, Stream::Input& input, DP::Factory<V, VIDType, VFArgs ...> const& vFactory)
	requires Deserializable<K, Stream::Input, KArgs ...>;

	template <typename KIDType, typename ... KFArgs, typename ... VArgs>
	Map(DP::Factory<K, KIDType, KFArgs ...> const& kFactory, Stream::Input& input, VArgs&& ... vArgs)
	requires Deserializable<V, Stream::Input, VArgs ...>;

	template <typename KIDType, typename ... KFArgs, typename VIDType, typename ... VFArgs>
	Map(DP::Factory<K, KIDType, KFArgs ...> const& kFactory, Stream::Input& input, DP::Factory<V, VIDType, VFArgs ...> const& vFactory);

	template <typename k, typename v>
	friend Stream::Output&
	operator<<(Stream::Output& output, Map<k, v> const& map)
	requires Stream::Serializable<k, Stream::Output> && Stream::Serializable<v, Stream::Output>;

	~Map();

	template <typename ... KArgs>
	iterator
	put(KArgs&& ... kArgs);

	template <typename ... KArgs, Derived<V> DV>
	iterator
	put(KArgs&& ... kArgs);

	template <typename ... KArgs, typename ... VCIArgs>
	iterator
	put(KArgs&& ... kArgs, DP::CreateInfo<V, VCIArgs ...> const& vCreateInfo);


	template <EqDerived<K> DK, typename ... DKArgs>
	iterator
	put(DKArgs&& ... dkArgs);

	template <EqDerived<K> DK, typename ... DKArgs, Derived<V> DV>
	iterator
	put(DKArgs&& ... dkArgs);

	template <EqDerived<K> DK, typename ... DKArgs, typename ... VCIArgs>
	iterator
	put(DKArgs&& ... dkArgs, DP::CreateInfo<V, VCIArgs ...> const& vCreateInfo);


	template <typename ... KCIArgs, typename ... KCArgs>
	iterator
	put(DP::CreateInfo<K, KCIArgs ...> const& kCreateInfo, KCArgs&& ... kcArgs);

	template <typename ... KCIArgs, typename ... KCArgs, Derived<V> DV>
	iterator
	put(DP::CreateInfo<K, KCIArgs ...> const& kCreateInfo, KCArgs&& ... kcArgs);

	template <typename ... KCIArgs, typename ... KCArgs, typename ... VCIArgs>
	iterator
	put(DP::CreateInfo<K, KCIArgs ...> const& kCreateInfo, KCArgs&& ... kcArgs, DP::CreateInfo<V, VCIArgs ...> const& vCreateInfo);


	template <Direction d, Constness c>
	iterator
	add(Iterator<d, c> const&)
	requires std::is_copy_constructible_v<K> && std::is_copy_constructible_v<V> && std::is_copy_assignable_v<V>;

	template <typename T>
	bool
	remove(T&& k) noexcept;

	template <typename T>
	iterator
	operator[](T&& k) noexcept;

	template <typename T>
	const_iterator
	operator[](T&& k) const noexcept;

	iterator
	at(std::uint64_t i) noexcept;

	const_iterator
	at(std::uint64_t i) const noexcept;

	iterator
	begin() noexcept;

	const_iterator
	begin() const noexcept;

	iterator
	end() noexcept;

	const_iterator
	end() const noexcept;

	reverse_iterator
	rbegin() noexcept;

	const_reverse_iterator
	rbegin() const noexcept;

	reverse_iterator
	rend() noexcept;

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
};//class Map<K, V>

template <typename K, typename V>
template <Direction d, Constness c>
class Map<K, V>::Iterator {
	friend class Map;

protected:
	MNode<K, V>* pos;

	Iterator&
	inc() noexcept;

	Iterator&
	dec() noexcept;

	Iterator(MNode<K, V>* pos) noexcept;

public:
	template <Direction od, Constness oc>
	Iterator(Iterator<od, oc> const& other) noexcept
	requires ConstCompat<c, oc>;

	template <Direction od, Constness oc>
	Iterator&
	operator=(Iterator<od, oc> const& other) noexcept
	requires ConstCompat<c, oc>;

	bool
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

	Entry<c>&
	operator*() const noexcept;

	Entry<c>*
	operator->() const noexcept;

	template <Direction od, Constness oc>
	bool
	operator==(Iterator<od, oc> const& other) const noexcept;

	explicit operator bool() const noexcept;
};//class Map<K, V>::Iterator<Direction, Constness>

}//namespace DS

#include "../src/Map.hpp"

#endif //DS_MAP_H
