#ifndef DS_VNODE_HPP
#define DS_VNODE_HPP

#include "ENode.hpp"

namespace DS {

template <typename V, typename E>
struct VNode {
	std::uint64_t inDegree;
	ENode<V, E> *inHead, *inTail;
	std::uint64_t outDegree;
	ENode<V, E> *outHead, *outTail;
	union {
		char state;
		struct {
			uint8_t isWhite:1;
			uint8_t isGray:1;
			uint8_t isBlack:1;
			uint8_t :5;
		};
	};
	union {
		V value;
		std::byte vraw[sizeof(V)];
	};

	template <typename ... Args>
	VNode(Args&& ... args):
		inDegree(0), inHead(nullptr), inTail(nullptr),
		outDegree(0), outHead(nullptr), outTail(nullptr),
		state(1), value(std::forward<Args>(args) ...) {}

	template <typename v = V, typename ...>
	VNode(std::enable_if_t<Stream::isDeserializable<v>, Stream::Input&> is, Map<std::uint64_t, VNode<V, E>*>& map):
		inDegree(0), inHead(nullptr), inTail(nullptr),
		outDegree(0), outHead(nullptr), outTail(nullptr),
		state(1), value(is) {
		map.addKey(map.size()).setValue(this);
	}

	template <typename v = V, typename ...>
	VNode(std::enable_if_t<!Stream::isDeserializable<v> && std::is_trivially_copyable_v<v>, Stream::Input&> is, Map<std::uint64_t, VNode<V, E>*>& map):
		inDegree(0), inHead(nullptr), inTail(nullptr),
		outDegree(0), outHead(nullptr), outTail(nullptr),
		state(1) {
		if (!(is >> Stream::Raw(vraw, sizeof(v)))) throw Stream::Input::Exception("Insufficient");
		map.addKey(map.size()).setValue(this);
	}

	template <typename v = V>
	std::enable_if_t<Stream::isSerializable<v> || std::is_trivially_copyable_v<v>, Stream::Output&>
	serialize(Stream::Output& os, Map<VNode<V, E> const*, std::uint64_t>& map) const {
		if (Stream::isSerializable<V> ? os << value : os << Stream::Raw(vraw, sizeof(V))) {
			map.addKey(this).setValue(map.size());
			return os;
		}
		throw Stream::Output::Exception("Insufficient");
	}

	~VNode() { reinterpret_cast<V*>(vraw)->~V(); }
};//struct VNode<V, E>

}//namespace DS

#endif //DS_VNODE_HPP
