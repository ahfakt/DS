#ifndef DS_LNODE_HPP
#define DS_LNODE_HPP

#include "Holder.hpp"

namespace DS {

template <typename T>
struct LNode {
	LNode* prev;
	LNode* next;
	Holder<T> val;

	static void*
	operator new(std::size_t, std::size_t const valSize)
	{ return ::operator new(Offset(&LNode::val) + valSize); }

	// Custom delete operator to keep C++14 from using the global operator delete(void*, size_t),
	// which would result in the compiler error:
	// "exception cleanup for this placement new selects non-placement operator delete"
	static void
	operator delete(void* ptr)
	{ ::operator delete(ptr); }

	explicit LNode(auto&& ... args)
			: val(std::forward<decltype(args)>(args) ...)
	{}

	~LNode()
	{ val->~T(); }
};//struct LNode<T>

template <typename T>
void
swap(LNode<T>* a, LNode<T>* b, LNode<T>*& head, LNode<T>*& tail) noexcept
{
	(a->prev ? a->prev->next : head) = b;
	(b->next ? b->next->prev : tail) = a;

	if (a->next == b) {
		a->next = b->next;
		b->prev = a->prev;
		a->prev = b;
		b->next = a;
	} else {
		a->next->prev = b;
		b->prev->next = a;
		std::swap(a->prev, b->prev);
		std::swap(a->next, b->next);
	}
}

template <typename T>
LNode<T>*
moveFront(LNode<T>* l, LNode<T>*& head, LNode<T>*& tail) noexcept
{
	if (l->prev) {
		(l->next ? l->next->prev : tail) = l->prev;
		l->prev->next = l->next;
		l->next = head;
		l->prev = nullptr;
		head = head->prev = l;
	}
	return l;
}

template <typename T>
LNode<T>*
moveBack(LNode<T>* l, LNode<T>*& head, LNode<T>*& tail) noexcept
{
	if (l->next) {
		(l->prev ? l->prev->next : head) = l->next;
		l->next->prev = l->prev;
		l->prev = tail;
		l->next = nullptr;
		tail = tail->next = l;
	}
	return l;
}

template <typename T>
LNode<T>*
moveBefore(LNode<T>* p, LNode<T>* l, LNode<T>*& head, LNode<T>*& tail) noexcept
{
	if (p == head)
		return moveFront(l, head, tail);
	if (p) {
		(l->prev ? l->prev->next : head) = l->next;
		(l->next ? l->next->prev : tail) = l->prev;
		return p->prev = (l->prev = (l->next = p)->prev)->next = l;
	}
	return moveBack(l, head, tail);
}

template <typename T>
LNode<T>*
moveAfter(LNode<T>* p, LNode<T>* l, LNode<T>*& head, LNode<T>*& tail) noexcept
{
	if (p == tail)
		return moveBack(l, head, tail);
	if (p) {
		(l->prev ? l->prev->next : head) = l->next;
		(l->next ? l->next->prev : tail) = l->prev;
		return p->next = (l->next = (l->prev = p)->next)->prev = l;
	}
	return moveFront(l, head, tail);
}

template <typename T>
void
qsort(LNode<T>* l, LNode<T>* r, LNode<T>*& head, LNode<T>*& tail) noexcept
{
	LNode<T>* p = r;
	LNode<T>* a = l;
	LNode<T>* b = r;

	while (a != b) {
		if (p == b) {
			if (static_cast<T const&>(p->val) < static_cast<T const&>(a->val)) {
				if (b == r)
					r = a;
				if (a == l)
					l = b;
				swap(a, p, head, tail);
				b = a->prev;
				a = p;
			} else
				a = a->next;
		} else {
			if (static_cast<T const&>(b->val) < static_cast<T const&>(p->val)) {
				if (b == r)
					r = a;
				if (a == l)
					l = b;
				swap(p, b, head, tail);
				a = b->next;
				b = p;
			} else
				b = b->prev;
		}
	}

	if (p != r && p->next != r)
		qsort(p->next, r, head, tail);
	if (l != p && l != p->prev)
		qsort(l, p->prev, head, tail);
}

}//namespace DS

#endif //DS_LNODE_HPP
