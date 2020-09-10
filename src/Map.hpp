#ifndef DS_MAP_HPP
#define DS_MAP_HPP

#include "MNode.hpp"

namespace DS {

template <typename K, typename V>
Map<K, V>::Map() noexcept:
	mRoot(nullptr) {}

template <typename K, typename V>
Map<K, V>::Map(Map&& other) noexcept:
	Container(other.mSize), mRoot(other.mRoot) {
	other.mSize = 0;
	other.mRoot = nullptr;
}

template <typename K, typename V>
Map<K, V>::Map(Map const& other) : Map() {
	if (other.mRoot) {
		mRoot = new MNode<K, V>(reinterpret_cast<MNode<K, V>*>(0), other.mRoot);
		mRoot->up = mRoot;
		mSize = other.mSize;
	}
}

template <typename K, typename V>
Map<K, V>&
Map<K, V>::operator=(Map&& other) noexcept {
	if (this == &other) return *this;
	this->~Map();
	return *new(this) Map(std::move(other));
}

template <typename K, typename V>
Map<K, V>&
Map<K, V>::operator=(Map const& other) {
	// TODO: use allocated nodes
	if (this == &other) return *this;
	this->~Map();
	return *new(this) Map(other);
}

template <typename K, typename V>
Map<K, V>::~Map()
{ delete mRoot; }

template <typename K, typename V>
template <typename k, typename v>
Map<K, V>::Map(std::enable_if_t<(Stream::isDeserializable<k> || std::is_trivially_copyable_v<k>) && (Stream::isDeserializable<v> || std::is_trivially_copyable_v<v>), Stream::Input>& is) : Map() {
	std::uint64_t size;
	if (!(is >> size)) throw Stream::Input::Exception("Insufficient");
	if(size) {
		mRoot = new MNode<K, V>(reinterpret_cast<MNode<K, V>*>(0), is);
		mRoot->up = mRoot;
		mSize = size;
	}
}

template <typename K, typename V>
template <typename k, typename v>
Map<K, V>&
Map<K, V>::operator=(std::enable_if_t<(Stream::isDeserializable<k> || std::is_trivially_copyable_v<k>) && (Stream::isDeserializable<v> || std::is_trivially_copyable_v<v>), Stream::Input>& is) {
	// TODO: use allocated nodes
	this->~Map();
	return *new(this) Map(is);
}

template <typename K, typename V>
std::enable_if_t<(Stream::isDeserializable<K> || std::is_trivially_copyable_v<K>) && (Stream::isDeserializable<V> || std::is_trivially_copyable_v<V>), Stream::Output&>
operator<<(Stream::Output& os, Map<K, V> const& map) {
	if (!(os << map.mSize)) throw Stream::Output::Exception("Insufficient");
	return map.mRoot->serialize(os);
}

template <typename K, typename V>
MNode<K, V>*
Map<K,V>::addNode(MNode<K, V>* created) {
	if (mRoot) {
		MNode<K, V>* tmp = created;
		if (MNode<K, V>* node = mRoot->add(created)) {
			if (mRoot != node) (mRoot = node)->up = node;
		}
		if (created == tmp) ++mSize;
		else delete tmp;
	}
	else {
		(mRoot = created)->up = created;
		++mSize;
	}
	return created;
}

template <typename K, typename V>
template <typename ... Args>
class Map<K, V>::Iterator<DIRECTION::Forward, CONSTNESS::NonConst>
Map<K, V>::addKey(Args&& ... args)
{ return addNode(new MNode<K, V>(std::forward<Args>(args) ...)); }

template <typename K, typename V>
template <typename D, typename ... Args>
class Map<K, V>::Iterator<DIRECTION::Forward, CONSTNESS::NonConst>
Map<K, V>::addKey(Args&& ... args)
{ return addNode(reinterpret_cast<MNode<K, V>*>(new MNode<K, D>(std::forward<Args>(args) ...))); }

template <typename K, typename V>
template <DIRECTION Direction, CONSTNESS Constness>
class Map<K, V>::Iterator<DIRECTION::Forward, CONSTNESS::NonConst>
Map<K, V>::add(Iterator<Direction, Constness> const& iter) {
	Iterator tmp = addNode(new MNode<K, V>(iter.pos->key));
	if (iter.pos->hasValue) tmp.setValue(iter.pos->value);
	else tmp.unsetValue();
	return tmp;
}

template <typename K, typename V>
template <typename T>
bool
Map<K, V>::remove(T&& key) noexcept {
	MNode<K, V>* toDel = nullptr;
	if (mRoot) {
		if (MNode<K, V>* node = mRoot->remove(std::forward<T>(key), toDel)) {
			if (mRoot != node) (mRoot = node)->up = node;
			else if (mRoot == toDel) mRoot = nullptr;
		}
		if (toDel) {
			delete toDel;
			--mSize;
		}
	}
	return toDel;
}

template <typename K, typename V>
template <DIRECTION Direction, CONSTNESS Constness>
bool
Map<K, V>::remove(Iterator<Direction, Constness> const& at) noexcept
{ return remove(at.key()); }

template <typename K, typename V>
template <typename T>
class Map<K, V>::Iterator<DIRECTION::Forward, CONSTNESS::NonConst>
Map<K, V>::get(T&& key) noexcept
{ return mRoot ? mRoot->get(std::forward<T>(key)) : nullptr; }

template <typename K, typename V>
template <typename T>
class Map<K, V>::Iterator<DIRECTION::Forward, CONSTNESS::Const>
Map<K, V>::get(T&& key) const noexcept
{ return mRoot ? mRoot->get(std::forward<T>(key)) : nullptr; }

template <typename K, typename V>
class Map<K, V>::Iterator<DIRECTION::Forward, CONSTNESS::NonConst>
Map<K, V>::operator[](std::uint64_t i) noexcept {
	Iterator<DIRECTION::Forward, CONSTNESS::NonConst> iter;
	if (i < mSize/2) {
		iter.pos = first();
		for (std::uint64_t j = 0; j < i; ++j) ++iter;
	}
	else {
		iter.pos = last();
		for (std::uint64_t j = mSize - 1; i < j; --j) --iter;
	}
	return iter;
}

template <typename K, typename V>
class Map<K, V>::Iterator<DIRECTION::Forward, CONSTNESS::Const>
Map<K, V>::operator[](std::uint64_t i) const noexcept
{ return const_cast<Map*>(this)->operator[](i); }

template <typename K, typename V>
class Map<K, V>::Iterator<DIRECTION::Forward, CONSTNESS::NonConst>
Map<K, V>::begin() noexcept
{ return first(); }

template <typename K, typename V>
class Map<K, V>::Iterator<DIRECTION::Backward, CONSTNESS::NonConst>
Map<K, V>::rbegin() noexcept
{ return last(); }

template <typename K, typename V>
class Map<K, V>::Iterator<DIRECTION::Forward, CONSTNESS::Const>
Map<K, V>::begin() const noexcept
{ return first(); }

template <typename K, typename V>
class Map<K, V>::Iterator<DIRECTION::Backward, CONSTNESS::Const>
Map<K, V>::rbegin() const noexcept
{ return last(); }

template <typename K, typename V>
class Map<K, V>::Iterator<DIRECTION::Forward, CONSTNESS::Const>
Map<K, V>::end() const noexcept
{ return nullptr; }

template <typename K, typename V>
class Map<K, V>::Iterator<DIRECTION::Backward, CONSTNESS::Const>
Map<K, V>::rend() const noexcept
{ return nullptr; }

template <typename K, typename V>
MNode<K, V>*
Map<K, V>::first() const noexcept {
	MNode<K, V>* tmp;
	if (tmp = mRoot) while (tmp->left) tmp = tmp->left;
	return tmp;
}

template <typename K, typename V>
MNode<K, V>*
Map<K, V>::last() const noexcept {
	MNode<K, V>* tmp;
	if (tmp = mRoot) while (tmp->right) tmp = tmp->right;
	return tmp;
}

template <typename K, typename V>
template <DIRECTION Direction, CONSTNESS Constness>
Map<K, V>::Iterator<Direction, Constness>::Iterator(MNode<K, V>* pos) noexcept:
	pos(pos) {}

template <typename K, typename V>
template <DIRECTION Direction, CONSTNESS Constness>
template <DIRECTION OtherDirection, CONSTNESS OtherConstness, typename>
Map<K, V>::Iterator<Direction, Constness>::Iterator(Iterator<OtherDirection, OtherConstness> const& other) noexcept:
	pos(other.pos) {}

template <typename K, typename V>
template <DIRECTION Direction, CONSTNESS Constness>
template <DIRECTION OtherDirection, CONSTNESS OtherConstness, typename>
class Map<K, V>::Iterator<Direction, Constness>&
Map<K, V>::Iterator<Direction, Constness>::operator=(Iterator<OtherDirection, OtherConstness> const& other) noexcept {
	pos = other.pos;
	return *this;
}

template <typename K, typename V>
template <DIRECTION Direction, CONSTNESS Constness>
bool
Map<K, V>::Iterator<Direction, Constness>::hasValue() const noexcept
{ return pos->hasValue; }

template <typename K, typename V>
template <DIRECTION Direction, CONSTNESS Constness>
template <typename T>
std::enable_if_t<Constness == CONSTNESS::NonConst, T>
Map<K, V>::Iterator<Direction, Constness>::unsetValue() const noexcept
{ pos->unsetValue(); }

template <typename K, typename V>
template <DIRECTION Direction, CONSTNESS Constness>
template <typename ... Args, typename v>
std::enable_if_t<Constness == CONSTNESS::NonConst, v>&
Map<K, V>::Iterator<Direction, Constness>::setValue(Args&& ... args) const
{ return pos->setValue(std::forward<Args>(args) ...); }

template <typename K, typename V>
template <DIRECTION Direction, CONSTNESS Constness>
template <typename D, typename ... Args>
std::enable_if_t<Constness == CONSTNESS::NonConst, D>&
Map<K, V>::Iterator<Direction, Constness>::setValue(Args&& ... args) const
{ return reinterpret_cast<MNode<K, D>*>(pos)->setValue(std::forward<Args>(args) ...); }

template <typename K, typename V>
template <DIRECTION Direction, CONSTNESS Constness>
class Map<K, V>::Iterator<Direction, Constness>&
Map<K, V>::Iterator<Direction, Constness>::operator++() noexcept {
	if constexpr (Direction == DIRECTION::Forward) {
		if (pos->right) {
			pos = pos->right;
			while (pos->left) pos = pos->left;
		}
		else {
			while (pos->up->right == pos) pos = pos->up;
			pos = pos->up != pos ? pos->up : nullptr;
		}
	}
	else {
		if (pos->left) {
			pos = pos->left;
			while (pos->right) pos = pos->right;
		}
		else {
			while (pos->up->left == pos) pos = pos->up;
			pos = pos->up != pos ? pos->up : nullptr;
		}
	}
	return *this;
}

template <typename K, typename V>
template <DIRECTION Direction, CONSTNESS Constness>
class Map<K, V>::Iterator<Direction, Constness>
Map<K, V>::Iterator<Direction, Constness>::operator++(int) noexcept {
	MNode<K, V>* r = pos;
	++(*this);
	return r;
}

template <typename K, typename V>
template <DIRECTION Direction, CONSTNESS Constness>
class Map<K, V>::Iterator<Direction, Constness>&
Map<K, V>::Iterator<Direction, Constness>::operator--() noexcept {
	if constexpr (Direction == DIRECTION::Forward) {
		if (pos->left) {
			pos = pos->left;
			while (pos->right) pos = pos->right;
		}
		else {
			while (pos->up->left == pos) pos = pos->up;
			pos = pos->up != pos ? pos->up : nullptr;
		}
	}
	else {
		if (pos->right) {
			pos = pos->right;
			while (pos->left) pos = pos->left;
		}
		else {
			while (pos->up->right == pos) pos = pos->up;
			pos = pos->up != pos ? pos->up : nullptr;
		}
	}
	return *this;
}

template <typename K, typename V>
template <DIRECTION Direction, CONSTNESS Constness>
class Map<K, V>::Iterator<Direction, Constness>
Map<K, V>::Iterator<Direction, Constness>::operator--(int) noexcept {
	MNode<K, V>* r = pos;
	--(*this);
	return r;
}

template <typename K, typename V>
template <DIRECTION Direction, CONSTNESS Constness>
class Map<K, V>::Iterator<Direction, Constness>
Map<K, V>::Iterator<Direction, Constness>::operator+(std::uint64_t i) const noexcept {
	Iterator r(pos);
	for (; i; ++r, --i);
	return r;
}

template <typename K, typename V>
template <DIRECTION Direction, CONSTNESS Constness>
class Map<K, V>::Iterator<Direction, Constness>
Map<K, V>::Iterator<Direction, Constness>::operator-(std::uint64_t i) const noexcept {
	Iterator r(pos);
	for (; i; --r, --i);
	return r;
}

template <typename K, typename V>
template <DIRECTION Direction, CONSTNESS Constness>
struct Map<K, V>::Entry<Constness>&
Map<K, V>::Iterator<Direction, Constness>::operator*() const noexcept
{ return *reinterpret_cast<Entry<Constness>*>(pos->kraw); }

template <typename K, typename V>
template <DIRECTION Direction, CONSTNESS Constness>
struct Map<K, V>::Entry<Constness>*
Map<K, V>::Iterator<Direction, Constness>::operator->() const noexcept
{ return reinterpret_cast<Entry<Constness>*>(pos->kraw); }

template <typename K, typename V>
template <DIRECTION Direction, CONSTNESS Constness>
template <DIRECTION OtherDirection, CONSTNESS OtherConstness>
bool
Map<K, V>::Iterator<Direction, Constness>::operator==(Iterator<OtherDirection, OtherConstness> const& other) const noexcept
{ return pos == other.pos; }

template <typename K, typename V>
template <DIRECTION Direction, CONSTNESS Constness>
template <DIRECTION OtherDirection, CONSTNESS OtherConstness>
bool
Map<K, V>::Iterator<Direction, Constness>::operator!=(Iterator<OtherDirection, OtherConstness> const& other) const noexcept
{ return pos != other.pos; }

template <typename K, typename V>
template <DIRECTION Direction, CONSTNESS Constness>
Map<K, V>::Iterator<Direction, Constness>::operator bool() const noexcept
{ return pos; }

}//namespace DS

#endif //DS_MAP_HPP
