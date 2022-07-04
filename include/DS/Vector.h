#ifndef DS_VECTOR_H
#define DS_VECTOR_H

#include "DS/Container.h"
#include "DP/Factory.h"
#include "../../src/Holder.hpp"

namespace DS {

template <typename T>
class Vector : public Container {
	Holder<T>* mHead = nullptr;
	std::uint64_t mCapacity = 0;
public:
	template <Direction, Constness>
	class Iterator;

	using iterator = Iterator<Direction::FORWARD, Constness::NCONST>;
	using reverse_iterator = Iterator<Direction::BACKWARD, Constness::NCONST>;
	using const_iterator = Iterator<Direction::FORWARD, Constness::CONST>;
	using const_reverse_iterator = Iterator<Direction::BACKWARD, Constness::CONST>;

	struct Exception : std::runtime_error
	{ using std::runtime_error::runtime_error; };

	Vector() noexcept = default;

	explicit Vector(std::uint64_t reserve);

	Vector(Vector const& other)
	requires std::is_copy_constructible_v<T>;

	Vector(Vector&& other) noexcept;

	template <typename t>
	friend void
	swap(Vector<t>& a, Vector<t>& b) noexcept;

	Vector&
	operator=(Vector value) noexcept;

	template <typename ... TArgs>
	explicit Vector(Stream::Input& input, TArgs&& ... tArgs)
	requires Deserializable<T, Stream::Input&, TArgs ...>;

	template <typename IDType, typename ... FArgs>
	Vector(Stream::Input& input, DP::Factory<T, IDType, FArgs ...> const& factory);

	template <typename t>
	friend Stream::Output&
	operator<<(Stream::Output& output, Vector<t> const& vector)
	requires Stream::Serializable<t, Stream::Output&>;

	~Vector();

	template <typename ... TArgs>
	iterator
	pushBack(TArgs&& ... tArgs);

	template <EqDerived<T> DT, typename ... DTArgs>
	iterator
	pushBack(DTArgs&& ... dtArgs);

	template <typename ... CIArgs, typename ... CArgs>
	iterator
	pushBack(DP::CreateInfo<T, CIArgs ...> const& createInfo, CArgs&& ... cArgs);

	[[nodiscard]] std::uint64_t
	capacity() const noexcept;

	void
	reserve(std::uint64_t n);

	T&
	operator[](std::uint64_t i) noexcept;

	T const&
	operator[](std::uint64_t i) const noexcept;

	T*
	operator+(std::uint64_t i) noexcept;

	T const*
	operator+(std::uint64_t i) const noexcept;

	explicit operator T*() noexcept;

	explicit operator T const*() const noexcept;

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
};//class Vector<T>

template <typename T>
template <Direction d, Constness c>
class Vector<T>::Iterator {
	friend class Vector;

protected:
	T* pos;

	Iterator(Holder<T>* pos) noexcept;

public:
	template <Direction od, Constness oc>
	Iterator(Iterator<od, oc> const& other) noexcept
	requires ConstCompat<c, oc>;

	template <Direction od, Constness oc>
	Iterator&
	operator=(Iterator<od, oc> const& other) noexcept
	requires ConstCompat<c, oc>;

	Iterator&
	operator++() noexcept;

	Iterator
	operator++(int) noexcept;

	Iterator&
	operator--() noexcept;

	Iterator
	operator--(int) noexcept;

	TConstness<T, c>&
	operator*() const noexcept;

	TConstness<T, c>*
	operator->() const noexcept;

	template <Direction od, Constness oc>
	std::uint64_t
	operator-(Iterator<od, oc> const& other) const noexcept;

	template <Direction od, Constness oc>
	bool
	operator==(Iterator<od, oc> const& other) const noexcept;

	explicit operator bool() const noexcept;
};//class Vector<T>::Iterator<Direction, Constness>

}//namespace DS

#include "../src/Vector.hpp"

#endif //DS_VECTOR_H
