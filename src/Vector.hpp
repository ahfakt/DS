#ifndef DS_VECTOR_HPP
#define DS_VECTOR_HPP

#include "Container.hpp"

namespace DS {

template <typename V>
Vector<V>::Vector():
	mHead(nullptr), mCapacity(0) {}

template <typename V>
Vector<V>::Vector(std::uint64_t reserve):
	mHead(reserve ? reinterpret_cast<V*>(::operator new(reserve*sizeof(V))) : nullptr),
	mCapacity(reserve) {}

template <typename V>
Vector<V>::Vector(Vector&& other) noexcept:
	Container(other.mSize), mHead(other.mHead), mCapacity(other.mCapacity) {
	other.mHead = nullptr;
	other.mCapacity = other.mSize = 0;
}

template <typename V>
Vector<V>::Vector(Vector const& other) : Vector(other.mSize) {
	for (V *dest = mHead, *src = other.mHead; mSize < other.mSize; ++mSize, ++dest, ++src)
		new (dest) V(*src);
}

template <typename V>
Vector<V>&
Vector<V>::operator=(Vector&& other) noexcept {
	if (this == &other) return *this;
	this->~Vector();
	return *new(this) Vector(std::move(other));
}

template <typename V>
Vector<V>&
Vector<V>::operator=(Vector const& other) {
	if (this == &other) return *this;
	if (mCapacity < other.mSize) {
		this->~Vector();
		return *new (this) Vector(other);
	}
	V *beg = mHead, *src = other.mHead;
	for (V *end = mHead + (mSize <= other.mSize ? mSize : other.mSize); beg < end; ++beg, ++src)
		*beg = *src;
	for (;mSize < other.mSize; ++mSize, ++beg, ++src)
		new (beg) V(*src);
	while (mSize > other.mSize)
		(mHead + --mSize)->~V();
	return *this;
}

template <typename V>
Vector<V>::~Vector() {
	while (mSize) (mHead + --mSize)->~V();
	::operator delete (mHead);
}

template <typename V>
template <typename v>
Vector<V>::Vector(std::enable_if_t<Stream::isDeserializable<v> || std::is_trivially_copyable_v<v>, Stream::Input>& is) {
	std::uint64_t size;
	if (!(is >> size)) throw Stream::Input::Exception("Insufficient");
	if (size) {
		new(this) Vector(size);
		if constexpr (Stream::isDeserializable<V>) {
			while (size--) {
				new(mHead + mSize) V(is);
				++mSize;
			}
		}
		else if (is >> Stream::Raw(mHead, sizeof(V) * size)) mSize = size;
		else throw Stream::Input::Exception("Insufficient");
	}
}

template <typename V>
template <typename v>
Vector<V>&
Vector<V>::operator=(std::enable_if_t<Stream::isDeserializable<v> || std::is_trivially_copyable_v<v>, Stream::Input>& is) {
	std::uint64_t size;
	if (!(is >> size)) throw Stream::Input::Exception("Insufficient");
	if (mCapacity < size) {
		this->~Vector();
		return *new(this) Vector(is);
	}

	std::uint64_t i = 0;
	if (mSize > size) {
		if constexpr (Stream::isDeserializable<V>)
			while (i < size)
				*(mHead + i++) = V(is);
		else if(!(is >> Stream::Raw(mHead, sizeof(V) * size))) throw Stream::Input::Exception("Insufficient");
		while (i < mSize) (mHead + --mSize)->~V();
	}
	else {
		if constexpr (Stream::isDeserializable<V>) {
			while (i < mSize)
				*(mHead + i++) = V(is);
			while (i < size) {
				new(mHead + i++) V(is);
				++mSize;
			}
		}
		else if (is >> Stream::Raw(mHead, sizeof(V) * size)) mSize = size;
		else throw Stream::Input::Exception("Insufficient");
	}
	return *this;
}

template <typename V>
std::enable_if_t<Stream::isDeserializable<V> || std::is_trivially_copyable_v<V>, Stream::Output&>
operator<<(Stream::Output& os, Vector<V> const& vector) {
	if (!(os << vector.mSize)) throw Stream::Output::Exception("Insufficient");
	if constexpr (Stream::isSerializable<V>) for(auto& item : vector) os << item;
	else if (!(os << Stream::Raw(vector.mHead, sizeof(V) * vector.mSize))) throw Stream::Output::Exception("Insufficient");
	return os;
}

template <typename V>
template <typename ... Args>
class Vector<V>::Iterator<DIRECTION::Forward, CONSTNESS::NonConst>
Vector<V>::pushBack(Args&& ... args) {
	if (mSize == mCapacity) {
		Vector v(mCapacity > 1 ? 1.61803398875*mCapacity : 2);
		for (std::uint64_t i = 0; i < mSize; ++i)
			v.pushBack(std::move(*(mHead + i)));
		*this = std::move(v);
	}
	new (mHead + mSize) V(std::forward<Args>(args) ...);
	return mHead + mSize++;
}

template <typename V>
V*
Vector<V>::write(std::uint64_t i) noexcept {
	mSize = i;
	return mHead;
}

template <typename V>
std::uint64_t
Vector<V>::capacity() const noexcept
{ return mCapacity; }

template <typename V>
V&
Vector<V>::operator[](std::uint64_t i) noexcept
{ return *(mHead + i); }

template <typename V>
V const&
Vector<V>::operator[](std::uint64_t i) const noexcept
{ return *(mHead + i); }

template <typename V>
Vector<V>::operator V*() noexcept
{ return mHead; }

template <typename V>
Vector<V>::operator V const*() const noexcept
{ return mHead; }

template <typename V>
class Vector<V>::Iterator<DIRECTION::Forward, CONSTNESS::NonConst>
Vector<V>::begin() noexcept
{ return mHead; }

template <typename V>
class Vector<V>::Iterator<DIRECTION::Backward, CONSTNESS::NonConst>
Vector<V>::rbegin() noexcept
{ return mHead + mSize - 1; }

template <typename V>
class Vector<V>::Iterator<DIRECTION::Forward, CONSTNESS::Const>
Vector<V>::begin() const noexcept
{ return mHead; }

template <typename V>
class Vector<V>::Iterator<DIRECTION::Backward, CONSTNESS::Const>
Vector<V>::rbegin() const noexcept
{ return mHead + mSize - 1; }

template <typename V>
class Vector<V>::Iterator<DIRECTION::Forward, CONSTNESS::Const>
Vector<V>::end() const noexcept
{ return mHead + mSize; }

template <typename V>
class Vector<V>::Iterator<DIRECTION::Backward, CONSTNESS::Const>
Vector<V>::rend() const noexcept
{ return mHead - 1; }

template <typename V>
template <DIRECTION Direction, CONSTNESS Constness>
Vector<V>::Iterator<Direction, Constness>::Iterator(V* pos) noexcept:
	pos(pos) {}

template <typename V>
template <DIRECTION Direction, CONSTNESS Constness>
template <DIRECTION OtherDirection, CONSTNESS OtherConstness, typename>
Vector<V>::Iterator<Direction, Constness>::Iterator(Iterator<OtherDirection, OtherConstness> const& other) noexcept:
	pos(other.pos) {}

template <typename V>
template <DIRECTION Direction, CONSTNESS Constness>
template <DIRECTION OtherDirection, CONSTNESS OtherConstness, typename>
class Vector<V>::Iterator<Direction, Constness>&
Vector<V>::Iterator<Direction, Constness>::operator=(Iterator<OtherDirection, OtherConstness> const& other) noexcept {
	pos = other.pos;
	return *this;
}

template <typename V>
template <DIRECTION Direction, CONSTNESS Constness>
class Vector<V>::Iterator<Direction, Constness>&
Vector<V>::Iterator<Direction, Constness>::operator++() noexcept {
	if constexpr (Direction == DIRECTION::Forward) ++pos;
	else --pos;
	return *this;
}

template <typename V>
template <DIRECTION Direction, CONSTNESS Constness>
class Vector<V>::Iterator<Direction, Constness>
Vector<V>::Iterator<Direction, Constness>::operator++(int) noexcept
{ return Direction == DIRECTION::Forward ? pos++ : pos--; }

template <typename V>
template <DIRECTION Direction, CONSTNESS Constness>
class Vector<V>::Iterator<Direction, Constness>&
Vector<V>::Iterator<Direction, Constness>::operator--() noexcept {
	if constexpr (Direction == DIRECTION::Forward) --pos;
	else ++pos;
	return *this;
}

template <typename V>
template <DIRECTION Direction, CONSTNESS Constness>
class Vector<V>::Iterator<Direction, Constness>
Vector<V>::Iterator<Direction, Constness>::operator--(int) noexcept
{ return Direction == DIRECTION::Forward ? pos-- : pos++; }

template <typename V>
template <DIRECTION Direction, CONSTNESS Constness>
class Vector<V>::Iterator<Direction, Constness>
Vector<V>::Iterator<Direction, Constness>::operator+(std::uint64_t i) const noexcept
{ return DIRECTION::Forward ? pos + i : pos - i; }

template <typename V>
template <DIRECTION Direction, CONSTNESS Constness>
class Vector<V>::Iterator<Direction, Constness>
Vector<V>::Iterator<Direction, Constness>::operator-(std::uint64_t i) const noexcept
{ return DIRECTION::Forward ? pos - i : pos + i; }

template <typename V>
template <DIRECTION Direction, CONSTNESS Constness>
std::conditional_t<Constness == CONSTNESS::Const, V const, V>&
Vector<V>::Iterator<Direction, Constness>::operator*() const noexcept
{ return *pos; }

template <typename V>
template <DIRECTION Direction, CONSTNESS Constness>
std::conditional_t<Constness == CONSTNESS::Const, V const, V>*
Vector<V>::Iterator<Direction, Constness>::operator->() const noexcept
{ return pos; }

template <typename V>
template <DIRECTION Direction, CONSTNESS Constness>
template <DIRECTION OtherDirection, CONSTNESS OtherConstness>
bool
Vector<V>::Iterator<Direction, Constness>::operator==(Iterator<OtherDirection, OtherConstness> const& other) const noexcept
{ return pos == other.pos; }

template <typename V>
template <DIRECTION Direction, CONSTNESS Constness>
template <DIRECTION OtherDirection, CONSTNESS OtherConstness>
bool
Vector<V>::Iterator<Direction, Constness>::operator!=(Iterator<OtherDirection, OtherConstness> const& other) const noexcept
{ return pos != other.pos; }

template <typename V>
template <DIRECTION Direction, CONSTNESS Constness>
Vector<V>::Iterator<Direction, Constness>::operator bool() const noexcept
{ return pos; }

}//namespace DS

#endif //DS_VECTOR_HPP
