#ifndef DS_LNODE_HPP
#define DS_LNODE_HPP

#include "Container.hpp"

namespace DS {

template <typename V>
struct LNode {
	LNode *prev, *next;
	union {
		V value;
		std::byte vraw[sizeof(V)];
	};

	template <typename ... Args>
	LNode(LNode* prev, LNode* next, Args&& ... args):
		prev(prev), next(next),
		value(std::forward<Args>(args) ...) {}

	template <typename v = V, typename ...>
	LNode(LNode<v>* prev, LNode<v>* next,
		std::enable_if_t<Stream::isDeserializable<v>, Stream::Input&> is):
		prev(prev), next(next),
		value(is) {}

	template <typename v = V, typename ...>
	LNode(LNode<v>* prev, LNode<v>* next,
		std::enable_if_t<!Stream::isDeserializable<v> && std::is_trivially_copyable_v<v>, Stream::Input&> is):
		prev(prev), next(next) {
		if (!(is >> Stream::Raw(vraw, sizeof(v)))) throw Stream::Input::Exception("Insufficient");
	}

	template <typename v = V>
	std::enable_if_t<Stream::isSerializable<v> || std::is_trivially_copyable_v<v>, Stream::Output&>
	serialize(Stream::Output& os) const {
		if (Stream::isSerializable<V> ? os << value : os << Stream::Raw(vraw, sizeof(V))) return os;
		throw Stream::Output::Exception("Insufficient");
	}

	~LNode() { reinterpret_cast<V*>(vraw)->~V(); }

	LNode* detach(LNode*& head, LNode*& tail) noexcept {
		(prev ? prev->next : head) = next;
		(next ? next->prev : tail) = prev;
		return this;
	}
	LNode* attach(LNode* at, LNode*& head, LNode*& tail) noexcept {
		LNode*& p = ((next = at) ? at->prev : tail);
		p = ((prev = p) ? p->next : head) = this;
		return this;
	}
};//struct LNode<V>

template <typename V>
void
swap(LNode<V>* a, LNode<V>* b, LNode<V>*& head, LNode<V>*& tail) noexcept {
	if (a->prev) a->prev->next = b;
	else head = b;

	if (b->next) b->next->prev = a;
	else tail = a;

	if (a->next == b) {
		a->next = b->next;
		b->prev = a->prev;
		a->prev = b;
		b->next = a;
	}
	else {
		a->next->prev = b;
		b->prev->next = a;
		LNode<V>* tmp = a->prev;
		a->prev = b->prev;
		b->prev = tmp;
		tmp = a->next;
		a->next = b->next;
		b->next = tmp;
	}
}

template <typename V>
void
qsort(LNode<V>* left, LNode<V>* right, LNode<V>*& head, LNode<V>*& tail) noexcept {
	LNode<V>* pivot = right;
	LNode<V>* a = left;
	LNode<V>* b = right;

	while (a != b) {
		if (pivot == b) {
			if (pivot->value < a->value) {
				if (b == right) right = a;
				if (a == left) left = b;
				swap(a, pivot, head, tail);
				b = a->prev;
				a = pivot;
			}
			else a = a->next;
		}
		else {
			if (b->value < pivot->value) {
				if (b == right) right = a;
				if (a == left) left = b;
				swap(pivot, b, head, tail);
				a = b->next;
				b = pivot;
			}
			else b = b->prev;
		}
	}

	if (pivot != right && pivot->next != right) qsort(pivot->next, right, head, tail);
	if (left != pivot && left != pivot->prev) qsort(left, pivot->prev, head, tail);
}

}//namespace DS

#endif //DS_LNODE_HPP
