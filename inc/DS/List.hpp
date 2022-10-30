#ifndef DS_LIST_HPP
#define DS_LIST_HPP

#include "Container.hpp"
#include "../../src/DS/LNode.tpp"
#include <DP/Factory.hpp>
#include <StreamFormat/Dot.hpp>

namespace DS {

/**
 * @brief	Doubly linked list implementation.
 * @class	List List.hpp "DS/List.hpp"
 * @tparam	T Value type to be stored in List
 */
template <typename T>
class List : public Container {
	template <typename v, typename e>
	friend class Digraph;

	LNode<T>* mHead{nullptr};
	LNode<T>* mTail{nullptr};

	LNode<T>*
	pushFront(LNode<T>* l) noexcept;

	LNode<T>*
	pushBack(LNode<T>* l) noexcept;

	LNode<T>*
	insertBefore(LNode<T>* p, LNode<T>* l) noexcept;

	LNode<T>*
	insertAfter(LNode<T>* p, LNode<T>* l) noexcept;

public:
	/**
	 * @brief	%Iterator class to be used to iterate over List elements.
	 * @class	Iterator List.hpp "DS/List.hpp"
	 * @tparam	d Direction::FORWARD or Direction::BACKWARD
	 * @tparam	c Constness::CONST or Constness::NCONST
	 */
	template <Direction d, Constness c>
	class Iterator;

	using iterator = Iterator<Direction::FORWARD, Constness::NCONST>;
	using reverse_iterator = Iterator<Direction::BACKWARD, Constness::NCONST>;
	using const_iterator = Iterator<Direction::FORWARD, Constness::CONST>;
	using const_reverse_iterator = Iterator<Direction::BACKWARD, Constness::CONST>;

	List() noexcept = default;

	List(List const& other)
	requires std::is_copy_constructible_v<T>;

	List(List&& other) noexcept;

	template <typename t>
	friend void
	swap(List<t>& a, List<t>& b) noexcept;

	List&
	operator=(List value) noexcept;

	explicit List(Stream::Input& input, auto&& ... tArgs)
	requires Stream::DeserializableWith<T, decltype(input), decltype(tArgs) ...>;

	template <typename Type, typename ... Args>
	List(Stream::Input& input, DP::Factory<T, Type, Args ...>, auto&& ... tArgs);

	template <typename t>
	friend Stream::Output&
	operator<<(Stream::Output& output, List<t> const& list)
	requires Stream::InsertableTo<t, decltype(output)>;

	template <typename t>
	friend Stream::Format::DotOutput&
	operator<<(Stream::Format::DotOutput& dotOutput, List<t> const& list)
	requires Stream::InsertableTo<t, decltype(dotOutput)>;

	~List();

	List&
	pushFront(List value) noexcept;

	iterator
	pushFront(auto&& ... tArgs);

	template <Derived<T> DT>
	iterator
	pushFront(auto&& ... dtArgs);

	template <typename ... Args>
	iterator
	pushFront(DP::CreateInfo<T, Args ...> const& createInfo, auto&& ... args);

	List&
	pushBack(List value) noexcept;

	iterator
	pushBack(auto&& ... tArgs);

	template <Derived<T> DT>
	iterator
	pushBack(auto&& ... dtArgs);

	template <typename ... Args>
	iterator
	pushBack(DP::CreateInfo<T, Args ...> const& createInfo, auto&& ... args);

	template <Direction d, Constness c>
	iterator
	insertBefore(Iterator<d, c> at, auto&& ... tArgs);

	template <Derived<T> DT, Direction d, Constness c>
	iterator
	insertBefore(Iterator<d, c> at, auto&& ... dtArgs);

	template <typename ... Args, Direction d, Constness c>
	iterator
	insertBefore(Iterator<d, c> at, DP::CreateInfo<T, Args ...> const& createInfo, auto&& ... args);

	template <Direction d, Constness c>
	iterator
	insertAfter(Iterator<d, c> at, auto&& ... tArgs);

	template <Derived<T> DT, Direction d, Constness c>
	iterator
	insertAfter(Iterator<d, c> at, auto&& ... dtArgs);

	template <typename ... Args, Direction d, Constness c>
	iterator
	insertAfter(Iterator<d, c> at, DP::CreateInfo<T, Args ...> const& createInfo, auto&& ... args);

	template <Direction d, Constness c>
	void
	remove(Iterator<d, c> at) noexcept;

	void
	popBack() noexcept;

	void
	popFront() noexcept;

	List&
	sort() noexcept;

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
};//class DS::List<T>

template <typename T>
template <Direction d, Constness c>
class List<T>::Iterator {
	friend class List;

protected:
	LNode<T>* pos;

	Iterator(LNode<T>* pos) noexcept;

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
	bool
	operator==(Iterator<od, oc> const& other) const noexcept;

	explicit operator bool() const noexcept;
};//class DS::List<T>::Iterator<Direction, Constness>

}//namespace DS

#include "../../src/DS/List.tpp"

#endif //DS_LIST_HPP
