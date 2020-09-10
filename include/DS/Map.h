#ifndef DS_MAP_H
#define DS_MAP_H

#include "../src/MNode.hpp"

namespace DS {

template <typename K, typename V>
class Map:
public Container {
	MNode<K, V> *mRoot;
	MNode<K, V>* first() const noexcept;
	MNode<K, V>* last() const noexcept;
	MNode<K, V>* addNode(MNode<K, V>*);
public:
	template <DIRECTION, CONSTNESS>
	class Iterator;

	template <CONSTNESS Constness>
	struct Entry {
		K const key;
		std::conditional_t<Constness == CONSTNESS::Const, V const, V> value;
	};

	Map() noexcept;
	Map(Map&&) noexcept;
	Map(Map const&);
	Map& operator=(Map&&) noexcept;
	Map& operator=(Map const&);
	~Map();

	template <typename k = K, typename v = V>
	Map(std::enable_if_t<(Stream::isDeserializable<k> || std::is_trivially_copyable_v<k>) && (Stream::isDeserializable<v> || std::is_trivially_copyable_v<v>), Stream::Input>&);

	template <typename k = K, typename v = V>
	Map& operator=(std::enable_if_t<(Stream::isDeserializable<k> || std::is_trivially_copyable_v<k>) && (Stream::isDeserializable<v> || std::is_trivially_copyable_v<v>), Stream::Input>&);

	template <typename k, typename v>
	friend std::enable_if_t<(Stream::isDeserializable<k> || std::is_trivially_copyable_v<k>) && (Stream::isDeserializable<v> || std::is_trivially_copyable_v<v>), Stream::Output&>
	operator<<(Stream::Output& os, Map<k, v> const& map);

	template <typename ... Args>
	Iterator<DIRECTION::Forward, CONSTNESS::NonConst>
	addKey(Args&& ...);

	template <typename D, typename ... Args>
	Iterator<DIRECTION::Forward, CONSTNESS::NonConst>
	addKey(Args&& ...);

	template <DIRECTION OtherDirection, CONSTNESS OtherConstness>
	Iterator<DIRECTION::Forward, CONSTNESS::NonConst>
	add(Iterator<OtherDirection, OtherConstness> const& iter);

	template <typename T>
	bool remove(T&& key) noexcept;

	template <DIRECTION Direction, CONSTNESS Constness>
	bool remove(Iterator<Direction, Constness> const& at) noexcept;

	template <typename T>
	Iterator<DIRECTION::Forward, CONSTNESS::NonConst>
	get(T&& key) noexcept;

	template <typename T>
	Iterator<DIRECTION::Forward, CONSTNESS::Const>
	get(T&& key) const noexcept;

	Iterator<DIRECTION::Forward, CONSTNESS::NonConst>
	operator[](std::uint64_t) noexcept;

	Iterator<DIRECTION::Forward, CONSTNESS::Const>
	operator[](std::uint64_t) const noexcept;

	Iterator<DIRECTION::Forward, CONSTNESS::NonConst> begin() noexcept;
	Iterator<DIRECTION::Backward, CONSTNESS::NonConst> rbegin() noexcept;
	Iterator<DIRECTION::Forward, CONSTNESS::Const> begin() const noexcept;
	Iterator<DIRECTION::Backward, CONSTNESS::Const> rbegin() const noexcept;
	Iterator<DIRECTION::Forward, CONSTNESS::Const> end() const noexcept;
	Iterator<DIRECTION::Backward, CONSTNESS::Const> rend() const noexcept;
};//class Map<K, V>

template <typename K, typename V>
template <DIRECTION Direction, CONSTNESS Constness>
class Map<K, V>::Iterator {
	friend class Map;
protected:
	MNode<K, V>* pos;
	Iterator(MNode<K, V>* pos) noexcept;
public:
	Iterator(Iterator const&) = default;
	Iterator& operator=(Iterator const&) = default;

	template <DIRECTION OtherDirection, CONSTNESS OtherConstness,
			typename = std::enable_if_t<Constness == CONSTNESS::Const && OtherConstness == CONSTNESS::NonConst>>
	Iterator(Iterator<OtherDirection, OtherConstness> const&) noexcept;

	template <DIRECTION OtherDirection, CONSTNESS OtherConstness,
		typename = std::enable_if_t<Constness == CONSTNESS::Const && OtherConstness == CONSTNESS::NonConst>>
	Iterator& operator=(Iterator<OtherDirection, OtherConstness> const&) noexcept;

	bool hasValue() const noexcept;

	template <typename T = void>
	std::enable_if_t<Constness == CONSTNESS::NonConst, T> unsetValue() const noexcept;

	template <typename ... Args, typename v = V>
	std::enable_if_t<Constness == CONSTNESS::NonConst, v>&
	setValue(Args&& ...) const;

	template <typename D, typename ... Args>
	std::enable_if_t<Constness == CONSTNESS::NonConst, D>&
	setValue(Args&& ...) const;

	Iterator& operator++() noexcept;
	Iterator operator++(int) noexcept;
	Iterator& operator--() noexcept;
	Iterator operator--(int) noexcept;

	Iterator operator+(std::uint64_t) const noexcept;
	Iterator operator-(std::uint64_t) const noexcept;

	Entry<Constness>&
	operator*() const noexcept;

	Entry<Constness>*
	operator->() const noexcept;

	template <DIRECTION OtherDirection, CONSTNESS OtherConstness>
	bool operator==(Iterator<OtherDirection, OtherConstness> const&) const noexcept;

	template <DIRECTION OtherDirection, CONSTNESS OtherConstness>
	bool operator!=(Iterator<OtherDirection, OtherConstness> const&) const noexcept;

	explicit operator bool() const noexcept;
};//class Map<K, V>::Iterator<Const>

}//namespace DS

#include "../src/Map.hpp"

#endif //DS_MAP_H
