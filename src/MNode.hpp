#ifndef DS_MNODE_HPP
#define DS_MNODE_HPP

#include "Container.hpp"

namespace DS {

template <typename K, typename V>
struct MNode {
	MNode *up, *left, *right;

	union {
		char state;
		union {
			struct {
				bool isRight:1;
				bool isBalanced:1;
				bool isLeft:1;
				uint8_t :2;
				uint8_t hasPack:3;
			};
			struct {
				uint8_t balance:3;
				uint8_t :2;
				bool hasRight:1;
				bool hasValue:1;
				bool hasLeft:1;
			};
		};
	};
	struct {
		union {
			K const key;
			std::byte kraw[sizeof(K)];
		};
		union {
			V value;
			std::byte vraw[sizeof(V)];
		};
	};

	template <typename ... Args>
	MNode(Args&& ... args):
		up(nullptr), left(nullptr), right(nullptr),
		state(2), key(std::forward<Args>(args) ...) {}

	template <typename k = K, typename v = V, typename ...>
	MNode(MNode<k, v>* parent, MNode<k, v>* other):
		up(parent),
		left(other->left ? new MNode(this, other->left) : nullptr),
		right(other->right ? new MNode(this, other->right) : nullptr),
		state(other->state), key(other->key) {
		if (other->hasValue) new (vraw) V(other->value);
	}

	template <typename k = K, typename v = V, typename ...>
	MNode(MNode<k, v>* parent,
		std::enable_if_t<(Stream::isDeserializable<k> || std::is_trivially_copyable_v<k>) &&
		(Stream::isDeserializable<v> || std::is_trivially_copyable_v<v>), Stream::Input&> is):
		up(parent), left(nullptr), right(nullptr) {

		if (!(is >> state)) throw Stream::Input::Exception("Insufficient");

		if constexpr (Stream::isDeserializable<K>) new (kraw) K(is);
		else if (!(is >> Stream::Raw(kraw, sizeof(K)))) throw Stream::Input::Exception("Insufficient");

		if (hasValue) {
			if constexpr (Stream::isDeserializable<V>) new (vraw) V(is);
			else if (!(is >> Stream::Raw(vraw, sizeof(V)))) throw Stream::Input::Exception("Insufficient");
		}

		if (hasLeft) {
			left = new MNode(this, is);
			hasLeft = false;
		}
		if (hasRight) {
			right = new MNode(this, is);
			hasRight = false;
		}
	}

	template <typename k = K, typename v = V>
	std::enable_if_t<(Stream::isSerializable<k> || std::is_trivially_copyable_v<k>) && (Stream::isSerializable<v> || std::is_trivially_copyable_v<v>), Stream::Output&>
	serialize(Stream::Output& os) const {
		char s = state;
		if (left)	s |= 0b10000000;
		if (right)	s |= 0b00100000;
		if (!(os << s)) throw Stream::Output::Exception("Insufficient");

		if (!(Stream::isSerializable<K> ? os << key : os << Stream::Raw(kraw, sizeof(K)))) throw Stream::Output::Exception("Insufficient");
		if (hasValue && !(Stream::isSerializable<V> ? os << value : os << Stream::Raw(vraw, sizeof(V)))) throw Stream::Output::Exception("Insufficient");

		if (left)	left->serialize(os);
		if (right)	right->serialize(os);

		return os;
	}

	~MNode() {
		delete right;
		if (hasValue) reinterpret_cast<V*>(vraw)->~V();
		reinterpret_cast<K*>(kraw)->~K();
		delete left;
	}
	void unsetValue() noexcept {
		if(hasValue) {
			reinterpret_cast<V*>(vraw)->~V();
			hasValue = false;
		}
	}
	template <typename ... Args>
	V& setValue(Args&& ... args) {
		unsetValue();
		new(vraw) V(std::forward<Args>(args) ...);
		hasValue = true;
		return value;
	}

	MNode* add(MNode*& created) noexcept {
		if (key < created->key) {
			if (right) {
				if (MNode* node = right->add(created)) {
					if (right != node) {
						right = node;
						return nullptr;
					}
					if (isRight) {
						if (right->isRight) return rightRight();
						if (right->isLeft) return rightLeft();
					}
					balance >>= 1;
					if (isRight) return this;
				}
				return nullptr;
			}
			(right = created)->up = this;
			balance >>= 1;
			return isRight ? this : nullptr;
		}
		if (created->key < key) {
			if (left) {
				if (MNode* node = left->add(created)) {
					if (left != node) {
						left = node;
						return nullptr;
					}
					if (isLeft) {
						if (left->isLeft) return leftLeft();
						if (left->isRight) return leftRight();
					}
					balance <<= 1;
					if (isLeft) return this;
				}
				return nullptr;
			}
			(left = created)->up = this;
			balance <<= 1;
			return isLeft ? this : nullptr;
		}
		created = this;
		return nullptr;
	}

	template <typename T>
	MNode* get(T&& key) noexcept {
		if (key < this->key) return left ? left->get(std::forward<T>(key)) : nullptr;
		if (this->key < key) return right ? right->get(std::forward<T>(key)) : nullptr;
		return this;
	}

	template <typename T>
	MNode* remove(T&& key, MNode*& toDel) noexcept {
		if (key < this->key) {
			if (left) {
				bool unbalanced = !left->isBalanced;
				if (MNode* node = left->remove(std::forward<T>(key), toDel)) {
					if (left == node || (this != node ? ((left = node)->isBalanced && unbalanced) : !(left = nullptr))) {
						if (isRight) {
							if (right->isLeft) return rightLeft();
							if (right->isRight) return rightRight();
							right->balance <<= 1;
							return leftRotate();
						}
						balance >>= 1;
						if (isBalanced) return this;
					}
				}
			}
			return nullptr;
		}
		if (this->key < key) {
			if (right) {
				bool unbalanced = !right->isBalanced;
				if (MNode* node = right->remove(std::forward<T>(key), toDel)) {
					if (right == node || (this != node ? ((right = node)->isBalanced && unbalanced) : !(right = nullptr))) {
						if (isLeft) {
							if (left->isRight) return leftRight();
							if (left->isLeft) return leftLeft();
							left->balance >>= 1;
							return rightRotate();
						}
						balance <<= 1;
						if (isBalanced) return this;
					}
				}
			}
			return nullptr;
		}
		MNode *parent = up, *node = down();
		toDel = this;
		return node ? node : parent;
	}

	MNode* down() noexcept {
		if (left) {
			if (right) {
				MNode* parent;
				if (isLeft) {
					balance >>= 1 + (parent = rightRotate())->isLeft;
					parent->right = down();
					if (parent->isRight) {
						if (parent->right->isRight) return parent->rightRight();
						if (parent->right->isLeft) return parent->rightLeft();
						(parent = parent->leftRotate())->balance <<= 1;
						return parent;
					}
					parent->balance >>= 1 + (parent->isLeft && !parent->right->isBalanced);
					return parent;
				}
				if (isRight) {
					balance <<= 1 + (parent = leftRotate())->isRight;
					parent->left = down();
					if (parent->isLeft) {
						if (parent->left->isLeft) return parent->leftLeft();
						if (parent->left->isRight) return parent->leftRight();
						(parent = parent->rightRotate())->balance >>= 1;
						return parent;
					}
					parent->balance <<= 1 + (parent->isRight && !parent->left->isBalanced);
					return parent;
				}
				if (right->isBalanced) {
					parent = rightRight();
					parent->left = down();
					return (parent->left->isBalanced ? parent : (parent->left->isRight ? parent->leftRight() : parent->leftLeft()));
				}
				if (left->isBalanced) {
					parent = leftLeft();
					parent->right = down();
					return (parent->right->isBalanced ? parent : (parent->right->isLeft ? parent->rightLeft() : parent->rightRight()));
				}
				parent = leftRotate();
				balance <<= 1;
				if (parent->isLeft) {
					parent->left = down();
					if (parent->left->isRight) {
						parent->left = parent->left->leftRotate();
						parent = parent->rightRotate();
						if (parent->isRight) {
							parent->left->balance <<= 2;
							parent->balance <<= 1;
						}
						else {
							if (parent->isLeft) parent->right->balance >>= 1;
							parent->left->balance <<= 1;
						}
						return parent;
					}
					(parent = parent->rightRotate())->balance >>= 1;
					return parent;
				}
				parent->left = left->isRight ? leftRight() : leftLeft();
				bool unbalanced = !isBalanced;
				if (!(parent->left->right = down()) || (unbalanced && parent->left->right->isBalanced)) parent->left->balance <<= 1;
				parent->balance <<= 2;
				return parent;
			}
			left->up = up;
			up = left;
			left = nullptr;
			return up;
		}
		if (right) {
			right->up = up;
			up = right;
			right = nullptr;
			return up;
		}
		return nullptr;
	}

	MNode* leftLeft() noexcept {
		balance >>= 1;
		left->balance >>= 1;
		return rightRotate();
	}

	MNode* rightRight() noexcept {
		balance <<= 1;
		right->balance <<= 1;
		return leftRotate();
	}

	MNode* leftRight() noexcept {
		balance >>= 1 + left->right->isLeft;
		left->balance <<= 1 + left->right->isRight;
		left->right->balance = 2;
		left = left->leftRotate();
		return rightRotate();
	}

	MNode* rightLeft() noexcept {
		balance <<= 1 + right->left->isRight;
		right->balance >>= 1 + right->left->isLeft;
		right->left->balance = 2;
		right = right->rightRotate();
		return leftRotate();
	}

	MNode* leftRotate() noexcept {
		MNode* Y = right;
		if (right = Y->left) Y->left->up = this;
		Y->left = this;
		Y->up = up;
		up = Y;
		return Y;
	}

	MNode* rightRotate() noexcept {
		MNode* Y = left;
		if (left = Y->right) Y->right->up = this;
		Y->right = this;
		Y->up = up;
		up = Y;
		return Y;
	}
};//struct MNode<K, V>

}//namespace DS

#endif //DS_MNODE_HPP
