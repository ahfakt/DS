#ifndef DS_LIST_HPP
#define DS_LIST_HPP

#include "LNode.hpp"

namespace DS {

template <typename V>
List<V>::List() noexcept:
	mHead(nullptr), mTail(nullptr) {}

template <typename V>
List<V>::List(List&& other) noexcept:
	Container(other.mSize), mHead(other.mHead), mTail(other.mTail) {
	other.mSize = 0;
	other.mHead = other.mTail = nullptr;
}

template <typename V>
List<V>::List(List const& other) : List() {
	if(LNode<V>* src = other.mHead) {
		mTail = mHead = new LNode<V>(reinterpret_cast<LNode<V>*>(0), nullptr, src->value);
		++mSize;
		while (src = src->next) {
			mTail = mTail->next = new LNode<V>(mTail, nullptr, src->value);
			++mSize;
		}
	}
}

template <typename V>
List<V>&
List<V>::operator=(List&& other) noexcept {
	if (this == &other) return *this;
	this->~List();
	return *new(this) List(std::move(other));
}

template <typename V>
List<V>&
List<V>::operator=(List const& other) {
	// TODO: use allocated nodes
	if (this == &other) return *this;
	this->~List();
	return *new(this) List(other);
}

template <typename V>
List<V>::~List() {
	while (mTail != mHead)
		delete (mTail = mTail->prev)->next;
	delete mHead;
}

template <typename V>
template <typename v>
List<V>::List(std::enable_if_t<Stream::isDeserializable<v> || std::is_trivially_copyable_v<v>, Stream::Input>& is) : List() {
	std::uint64_t size;
	if (!(is >> size)) throw Stream::Input::Exception("Insufficient");
	if(size) {
		mTail = mHead = new LNode<V>(reinterpret_cast<LNode<V>*>(0), nullptr, is);
		++mSize;
		while (--size) {
			mTail = mTail->next = new LNode<V>(mTail, nullptr, is);
			++mSize;
		}
	}
}

template <typename V>
template <typename v>
List<V>&
List<V>::operator=(std::enable_if_t<Stream::isDeserializable<v> || std::is_trivially_copyable_v<v>, Stream::Input>& is) {
	// TODO: use allocated nodes
	this->~List();
	return *new(this) List(is);
}

template <typename V>
std::enable_if_t<Stream::isDeserializable<V> || std::is_trivially_copyable_v<V>, Stream::Output&>
operator<<(Stream::Output& os, List<V> const& list) {
	if (!(os << list.mSize)) throw Stream::Output::Exception("Insufficient");
	for(LNode<V> const* node = list.mHead; node; node = node->next)
		node->serialize(os);
	return os;
}

template <typename V>
List<V>&
List<V>::operator+=(List&& other) noexcept {
	if (this != &other && other.mHead) {
		if (mTail) {
			(other.mHead->prev = mTail)->next = other.mHead;
			mSize += other.mSize;
		}
		else {
			mSize = other.mSize;
			mHead = other.mHead;
		}
		mTail = other.mTail;
		other.mSize = 0;
		other.mHead = nullptr;
		other.mTail = nullptr;
	}
	return *this;
}

template <typename V>
List<V>&
List<V>::sort() noexcept {
	qsort(mHead, mTail, mHead, mTail);
	return *this;
}

template <typename V>
template <DIRECTION Direction, CONSTNESS Constness, typename ... Args>
class List<V>::Iterator<DIRECTION::Forward, CONSTNESS::NonConst>
List<V>::insert(Iterator<Direction, Constness> at, Args&& ... args) {
	LNode<V>*& iter = (at.pos ? at.pos->prev : mTail);
	iter = (iter ? iter->next : mHead) = new LNode<V>(iter, at.pos, std::forward<Args>(args) ...);
	++mSize;
	return iter;
}

template <typename V>
template <typename D, DIRECTION Direction, CONSTNESS Constness, typename ... Args>
class List<V>::Iterator<DIRECTION::Forward, CONSTNESS::NonConst>
List<V>::insert(Iterator<Direction, Constness> at, Args&& ... args) {
	LNode<V>*& iter = (at.pos ? at.pos->prev : mTail);
	iter = (iter ? iter->next : mHead) = reinterpret_cast<LNode<V>*>(
		new LNode<D>(
			reinterpret_cast<LNode<D>*>(iter),
			reinterpret_cast<LNode<D>*>(at.pos),
			std::forward<Args>(args) ...)
	);
	++mSize;
	return iter;
}

template <typename V>
template <typename ... Args>
class List<V>::Iterator<DIRECTION::Forward, CONSTNESS::NonConst>
List<V>::pushBack(Args&& ... args) {
	mTail = (mTail ? mTail->next : mHead) = new LNode<V>(mTail, nullptr, std::forward<Args>(args) ...);
	++mSize;
	return mTail;
}

template <typename V>
template <typename D, typename ... Args>
class List<V>::Iterator<DIRECTION::Forward, CONSTNESS::NonConst>
List<V>::pushBack(Args&& ... args) {
	mTail = (mTail ? mTail->next : mHead) = reinterpret_cast<LNode<V>*>(
		new LNode<D>(reinterpret_cast<LNode<D>*>(mTail),
			nullptr, std::forward<Args>(args) ...));
	++mSize;
	return mTail;
}

template <typename V>
template <typename ... Args>
class List<V>::Iterator<DIRECTION::Forward, CONSTNESS::NonConst>
List<V>::pushFront(Args&& ... args) {
	mHead = (mHead ? mHead->prev : mTail) = new LNode<V>(nullptr, mHead, std::forward<Args>(args) ...);
	++mSize;
	return mHead;
}

template <typename V>
template <typename D, typename ... Args>
class List<V>::Iterator<DIRECTION::Forward, CONSTNESS::NonConst>
List<V>::pushFront(Args&& ... args) {
	mHead = (mHead ? mHead->prev : mTail) = reinterpret_cast<LNode<V>*>(
		new LNode<D>(nullptr, reinterpret_cast<LNode<D>*>(mHead),
			std::forward<Args>(args) ...));
	++mSize;
	return mHead;
}

template <typename V>
template <DIRECTION Direction, CONSTNESS Constness>
void
List<V>::remove(Iterator<Direction, Constness> at) noexcept {
	if (at.pos) {
		delete at.pos->remove(mHead, mTail);
		--mSize;
	}
}

template <typename V>
void
List<V>::popBack() noexcept {
	if (mTail) {
		if(mTail != mHead) {
			mTail = mTail->prev;
			delete mTail->next;
		}
		else {
			delete mHead;
			mHead = mTail = nullptr;
		}
		--mSize;
	}
}

template <typename V>
void
List<V>::popFront() noexcept {
	if (mHead) {
		if(mHead != mTail) {
			mHead = mHead->next;
			delete mHead->prev;
		}
		else {
			delete mTail;
			mTail = mHead = nullptr;
		}
		--mSize;
	}
}

template <typename V>
class List<V>::Iterator<DIRECTION::Forward, CONSTNESS::NonConst>
List<V>::begin() noexcept
{ return mHead; }

template <typename V>
class List<V>::Iterator<DIRECTION::Backward, CONSTNESS::NonConst>
List<V>::rbegin() noexcept
{ return mTail; }

template <typename V>
class List<V>::Iterator<DIRECTION::Forward, CONSTNESS::Const>
List<V>::begin() const noexcept
{ return mHead; }

template <typename V>
class List<V>::Iterator<DIRECTION::Backward, CONSTNESS::Const>
List<V>::rbegin() const noexcept
{ return mTail; }

template <typename V>
class List<V>::Iterator<DIRECTION::Forward, CONSTNESS::Const>
List<V>::end() const noexcept
{ return nullptr; }

template <typename V>
class List<V>::Iterator<DIRECTION::Backward, CONSTNESS::Const>
List<V>::rend() const noexcept
{ return nullptr; }

template <typename V>
template <DIRECTION Direction, CONSTNESS Constness>
List<V>::Iterator<Direction, Constness>::Iterator(LNode<V>* pos) noexcept:
	pos(pos) {}

template <typename V>
template <DIRECTION Direction, CONSTNESS Constness>
template <DIRECTION OtherDirection, CONSTNESS OtherConstness, typename>
List<V>::Iterator<Direction, Constness>::Iterator(Iterator<OtherDirection, OtherConstness> const& other) noexcept:
	pos(other.pos) {}

template <typename V>
template <DIRECTION Direction, CONSTNESS Constness>
template <DIRECTION OtherDirection, CONSTNESS OtherConstness, typename>
class List<V>::Iterator<Direction, Constness>&
List<V>::Iterator<Direction, Constness>::operator=(Iterator<OtherDirection, OtherConstness> const& other) noexcept {
	pos = other.pos;
	return *this;
}

template <typename V>
template <DIRECTION Direction, CONSTNESS Constness>
class List<V>::Iterator<Direction, Constness>&
List<V>::Iterator<Direction, Constness>::operator++() noexcept {
	pos = Direction == DIRECTION::Forward ? pos->next : pos->prev;
	return *this;
}

template <typename V>
template <DIRECTION Direction, CONSTNESS Constness>
class List<V>::Iterator<Direction, Constness>
List<V>::Iterator<Direction, Constness>::operator++(int) noexcept {
	LNode<V>* r = pos;
	pos = Direction == DIRECTION::Forward ? pos->next : pos->prev;
	return r;
}

template <typename V>
template <DIRECTION Direction, CONSTNESS Constness>
class List<V>::Iterator<Direction, Constness>&
List<V>::Iterator<Direction, Constness>::operator--() noexcept {
	pos = Direction == DIRECTION::Forward ? pos->prev : pos->next;
	return *this;
}

template <typename V>
template <DIRECTION Direction, CONSTNESS Constness>
class List<V>::Iterator<Direction, Constness>
List<V>::Iterator<Direction, Constness>::operator--(int) noexcept {
	LNode<V>* r = pos;
	pos = Direction == DIRECTION::Forward ? pos->prev : pos->next;
	return r;
}

template <typename V>
template <DIRECTION Direction, CONSTNESS Constness>
class List<V>::Iterator<Direction, Constness>
List<V>::Iterator<Direction, Constness>::operator+(std::uint64_t i) const noexcept {
	Iterator r(pos);
	for (; i; ++r, --i);
	return r;
}

template <typename V>
template <DIRECTION Direction, CONSTNESS Constness>
class List<V>::Iterator<Direction, Constness>
List<V>::Iterator<Direction, Constness>::operator-(std::uint64_t i) const noexcept {
	Iterator r(pos);
	for (; i; --r, --i);
	return r;
}

template <typename V>
template <DIRECTION Direction, CONSTNESS Constness>
std::conditional_t<Constness == CONSTNESS::Const, V const, V>&
List<V>::Iterator<Direction, Constness>::operator*() const noexcept
{ return pos->value; }

template <typename V>
template <DIRECTION Direction, CONSTNESS Constness>
std::conditional_t<Constness == CONSTNESS::Const, V const, V>*
List<V>::Iterator<Direction, Constness>::operator->() const noexcept
{ return &pos->value; }

template <typename V>
template <DIRECTION Direction, CONSTNESS Constness>
template <DIRECTION OtherDirection, CONSTNESS OtherConstness>
bool
List<V>::Iterator<Direction, Constness>::operator==(Iterator<OtherDirection, OtherConstness> const& other) const noexcept
{ return pos == other.pos; }

template <typename V>
template <DIRECTION Direction, CONSTNESS Constness>
template <DIRECTION OtherDirection, CONSTNESS OtherConstness>
bool
List<V>::Iterator<Direction, Constness>::operator!=(Iterator<OtherDirection, OtherConstness> const& other) const noexcept
{ return pos != other.pos; }

template <typename V>
template <DIRECTION Direction, CONSTNESS Constness>
List<V>::Iterator<Direction, Constness>::operator bool() const noexcept
{ return pos; }

}//namespace DS

#endif //DS_LIST_HPP
