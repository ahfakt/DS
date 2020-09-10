#ifndef DS_ENODE_HPP
#define DS_ENODE_HPP

#include "DS/Map.h"

namespace DS {

template <typename V, typename E>
struct VNode;

template <typename V, typename E>
struct ENode {
	ENode *left, *right, *down, *up;
	VNode<V, E> *in, *out;
	union {
		E value;
		std::byte eraw[sizeof(V)];
	};

	template <typename ... Args>
	ENode(Args&& ... args):
		left(nullptr), right(nullptr), down(nullptr), up(nullptr),
		in(nullptr), out(nullptr), value(std::forward<Args>(args) ...) {}

	template <typename e = E, typename ...>
	ENode(std::enable_if_t<Stream::isDeserializable<e>, Stream::Input&> is,
		Map<std::uint64_t, VNode<V, E>*>& map):
		left(nullptr), right(nullptr), down(nullptr), up(nullptr),
		in(nullptr), out(nullptr), value(is) {
		std::uint64_t tmp;
		if (!(is >> tmp)) throw Stream::Input::Exception("Insufficient");
		setIn(*map.get(tmp));
		if (!(is >> tmp)) throw Stream::Input::Exception("Insufficient");
		setOut(*map.get(tmp));
	}

	template <typename e = E, typename ...>
	ENode(std::enable_if_t<!Stream::isDeserializable<e> && std::is_trivially_copyable_v<e>, Stream::Input&> is,
		Map<std::uint64_t, VNode<V, E>*>& map):
		left(nullptr), right(nullptr), down(nullptr), up(nullptr),
		in(nullptr), out(nullptr)  {
		is.read(eraw, sizeof(E));
		std::uint64_t tmp;
		if (!(is >> tmp)) throw Stream::Input::Exception("Insufficient");
		setIn(*map.get(tmp));
		if (!(is >> tmp)) throw Stream::Input::Exception("Insufficient");
		setOut(*map.get(tmp));
	}

	template <typename e = E>
	std::enable_if_t<Stream::isSerializable<e> || std::is_trivially_copyable_v<e>, Stream::Output&>
	serialize(Stream::Output& os, Map<VNode<V, E> const*, std::uint64_t> const& map) const {
		if ((Stream::isSerializable<V> ? os << value : os << Stream::Raw(eraw, sizeof(V))) &&
				(os << *map.get(in)) && (os << *map.get(out))) return os;
		throw Stream::Output::Exception("Insufficient");
	}

	~ENode() { reinterpret_cast<E*>(eraw)->~E(); }

	void unsetIn() {
		if (in) {
			(down ? down->up : in->outHead) = up;
			(up ? up->down : in->outTail) = down;
			down = up = nullptr;
			--in->outDegree;
		}
	}

	void setIn(VNode<V, E>* v = nullptr) {
		unsetIn();
		if (in = v) {
			v->outTail = ((down = v->outTail) ? v->outTail->up : v->outHead) = this;
			++in->outDegree;
		}
	}

	void unsetOut() {
		if (out) {
			(left ? left->right : out->inHead) = right;
			(right ? right->left : out->inTail) = left;
			left = right = nullptr;
			--out->inDegree;
		}
	}

	void setOut(VNode<V, E>* v = nullptr) {
		unsetOut();
		if (out = v) {
			v->inTail = ((left = v->inTail) ? v->inTail->right : v->inHead) = this;
			++out->inDegree;
		}
	}
};//struct ENode<V, E>

}//namespace DS

#endif //DS_ENODE_HPP
