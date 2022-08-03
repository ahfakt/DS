#ifndef DS_VECTOR_HPP
#define DS_VECTOR_HPP

#include "Container.hpp"
#include "../../src/Holder.tpp"
#include <DP/Factory.hpp>

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

	explicit Vector(Stream::Input& input, auto&& ... tArgs)
	requires Stream::DeserializableWith<T, Stream::Input, decltype(tArgs) ...>;

	template <typename IDType, typename ... Args>
	Vector(Stream::Input& input, DP::Factory<T, IDType, Args ...> const& factory);

	template <typename t>
	friend Stream::Output&
	operator<<(Stream::Output& output, Vector<t> const& vector)
	requires Stream::InsertableTo<t, Stream::Output>;

	~Vector();

	iterator
	pushBack(auto&& ... tArgs);

	template <EqDerived<T> DT>
	iterator
	pushBack(auto&& ... dtArgs);

	template <typename ... Args>
	iterator
	pushBack(DP::CreateInfo<T, Args ...> const& createInfo, auto&& ... args);

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
};//class DS::Vector<T>

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
};//class DS::Vector<T>::Iterator<Direction, Constness>

}//namespace DS

#include "../../src/Vector.tpp"

#endif //DS_VECTOR_HPP
