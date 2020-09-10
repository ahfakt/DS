#ifndef DS_VECTOR_H
#define DS_VECTOR_H

#include "../../src/Container.hpp"

namespace DS {

template <typename V>
class Vector:
public Container {
	V *mHead;
	std::uint64_t mCapacity;
public:
	template <DIRECTION, CONSTNESS>
	class Iterator;

	Vector();
	Vector(std::uint64_t reserve);
	Vector(Vector&&) noexcept;
	Vector(Vector const&);
	Vector& operator=(Vector&&) noexcept;
	Vector& operator=(Vector const&);
	~Vector();

	template <typename v = V>
	Vector(std::enable_if_t<Stream::isDeserializable<v> || std::is_trivially_copyable_v<v>, Stream::Input>&);

	template <typename v = V>
	Vector& operator=(std::enable_if_t<Stream::isDeserializable<v> || std::is_trivially_copyable_v<v>, Stream::Input>&);

	template <typename v>
	friend std::enable_if_t<Stream::isDeserializable<v> || std::is_trivially_copyable_v<v>, Stream::Output&>
	operator<<(Stream::Output&, Vector<v> const&);

	template <typename ... Args>
	Iterator<DIRECTION::Forward, CONSTNESS::NonConst> pushBack(Args&& ...);

	std::uint64_t capacity() const noexcept;

	V* write(std::uint64_t) noexcept;

	V& operator[](std::uint64_t i) noexcept;
	V const& operator[](std::uint64_t i) const noexcept;

	operator V*() noexcept;
	operator V const*() const noexcept;

	Iterator<DIRECTION::Forward, CONSTNESS::NonConst> begin() noexcept;
	Iterator<DIRECTION::Backward, CONSTNESS::NonConst> rbegin() noexcept;
	Iterator<DIRECTION::Forward, CONSTNESS::Const> begin() const noexcept;
	Iterator<DIRECTION::Backward, CONSTNESS::Const> rbegin() const noexcept;
	Iterator<DIRECTION::Forward, CONSTNESS::Const> end() const noexcept;
	Iterator<DIRECTION::Backward, CONSTNESS::Const> rend() const noexcept;
};//class Vector<V>

template <typename V>
template <DIRECTION Direction, CONSTNESS Constness>
class Vector<V>::Iterator {
	friend class Vector;
protected:
	V* pos;
	Iterator(V* pos) noexcept;
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

	std::conditional_t<Constness == CONSTNESS::Const, V const, V>&
	operator*() const noexcept;

	std::conditional_t<Constness == CONSTNESS::Const, V const, V>*
	operator->() const noexcept;

	template <DIRECTION OtherDirection, CONSTNESS OtherConstness>
	bool operator==(Iterator<OtherDirection, OtherConstness> const&) const noexcept;

	template <DIRECTION OtherDirection, CONSTNESS OtherConstness>
	bool operator!=(Iterator<OtherDirection, OtherConstness> const&) const noexcept;

	explicit operator bool() const noexcept;
};//class Vector<V>::Iterator<Const>

}//namespace DS

#include "../src/Vector.hpp"

#endif //DS_VECTOR_H
