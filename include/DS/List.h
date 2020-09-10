#ifndef DS_LIST_H
#define DS_LIST_H

#include "../src/LNode.hpp"

namespace DS {

template <typename V>
class List:
public Container {
	LNode<V> *mHead, *mTail;
public:
	template <DIRECTION, CONSTNESS>
	class Iterator;

	List() noexcept;
	List(List&&) noexcept;
	List(List const&);
	List& operator=(List&&) noexcept;
	List& operator=(List const&);
	~List();

	template <typename v = V>
	List(std::enable_if_t<Stream::isDeserializable<v> || std::is_trivially_copyable_v<v>, Stream::Input>&);

	template <typename v = V>
	List& operator=(std::enable_if_t<Stream::isDeserializable<v> || std::is_trivially_copyable_v<v>, Stream::Input>&);

	template <typename v>
	friend std::enable_if_t<Stream::isDeserializable<v> || std::is_trivially_copyable_v<v>, Stream::Output&>
	operator<<(Stream::Output&, List<v> const&);

	List& operator+=(List&&) noexcept;

	List& sort() noexcept;

	template <DIRECTION Direction, CONSTNESS Constness, typename ... Args>
	Iterator<DIRECTION::Forward, CONSTNESS::NonConst>
	insert(Iterator<Direction, Constness> at, Args&& ...);

	template <typename D, DIRECTION Direction, CONSTNESS Constness, typename ... Args>
	Iterator<DIRECTION::Forward, CONSTNESS::NonConst>
	insert(Iterator<Direction, Constness> at, Args&& ...);

	template <typename ... Args>
	Iterator<DIRECTION::Forward, CONSTNESS::NonConst>
	pushBack(Args&& ...);

	template <typename D, typename ... Args>
	Iterator<DIRECTION::Forward, CONSTNESS::NonConst>
	pushBack(Args&& ...);

	template <typename ... Args>
	Iterator<DIRECTION::Forward, CONSTNESS::NonConst>
	pushFront(Args&& ...);

	template <typename D, typename ... Args>
	Iterator<DIRECTION::Forward, CONSTNESS::NonConst>
	pushFront(Args&& ...);

	template <DIRECTION Direction, CONSTNESS Constness>
	void remove(Iterator<Direction, Constness> at) noexcept;

	void popBack() noexcept;
	void popFront() noexcept;

	Iterator<DIRECTION::Forward, CONSTNESS::NonConst> begin() noexcept;
	Iterator<DIRECTION::Backward, CONSTNESS::NonConst> rbegin() noexcept;
	Iterator<DIRECTION::Forward, CONSTNESS::Const> begin() const noexcept;
	Iterator<DIRECTION::Backward, CONSTNESS::Const> rbegin() const noexcept;
	Iterator<DIRECTION::Forward, CONSTNESS::Const> end() const noexcept;
	Iterator<DIRECTION::Backward, CONSTNESS::Const> rend() const noexcept;
};//class List<V>

template <typename V>
template <DIRECTION Direction, CONSTNESS Constness>
class List<V>::Iterator {
	friend class List;
protected:
	LNode<V>* pos;
	Iterator(LNode<V>* pos) noexcept;
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
};//class List<V>::Iterator<Const>

}//namespace DS

#include "../src/List.hpp"

#endif //DS_LIST_H
