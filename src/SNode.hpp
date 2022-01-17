#ifndef DS_SNODE_HPP
#define DS_SNODE_HPP

#include "Holder.hpp"

namespace DS {

template <typename K>
struct SNode {
	SNode* left;
	SNode* right;
	union state_t {
		std::uint8_t _ = 2;
		std::uint8_t balance:3;
		struct {
			bool isRight:1;
			bool isBalanced:1;
			bool isLeft:1;
			bool :2;
			bool hasRight:1;
			bool :1;
			bool hasLeft:1;
		};
	} state;
	Holder<K> key;

	static void*
	operator new(std::size_t, std::size_t const keySize)
	{ return ::operator new(Offset(&SNode::key) + keySize); }

	static void
	operator delete(void* ptr)
	{ ::operator delete(ptr); }

	template <typename ... Args>
	explicit SNode(Args&& ... args)
			: key(std::forward<Args>(args) ...)
	{}

	~SNode()
	{
		if (state.hasRight)
			delete right;
		if (state.hasLeft)
			delete left;
		key->~K();
	}

	static SNode*
	Create(SNode* P, SNode* S, SNode const* other)
	{
		auto* t = ::new SNode(static_cast<K const&>(other->key));
		try {
			if (other->state.hasLeft) {
				t->left = SNode::Create(P, t, other->left);
				t->state.hasLeft = true;
			} else
				t->left = P;
			if (other->state.hasRight) {
				t->right = SNode::Create(t, S, other->right);
				t->state.hasRight = true;
			} else
				t->right = S;
			t->state.balance = other->state.balance;
			return t;
		} catch (...) {
			delete t;
			throw;
		}
	}

	template <typename ... KArgs>
	static SNode*
	Create(SNode* P, SNode* S, Stream::Input& input, KArgs&& ... kArgs)
	requires Deserializable<K, Stream::Input, KArgs ...>
	{
		auto state = Stream::Get<state_t>(input);
		auto* t = ::new SNode(input, std::forward<KArgs>(kArgs) ...);
		try {
			if (state.hasLeft) {
				t->left = SNode::Create(P, t, input, std::forward<KArgs>(kArgs) ...);
				t->state.hasLeft = true;
			} else
				t->left = P;
			if (state.hasRight) {
				t->right = SNode::Create(t, S, input, std::forward<KArgs>(kArgs) ...);
				t->state.hasRight = true;
			} else
				t->right = S;
			t->state.balance = state.balance;
			return t;
		} catch (...) {
			delete t;
			throw;
		}
	}

	template <typename IDType, typename ... FArgs>
	static SNode*
	Create(SNode* P, SNode* S, Stream::Input& input, DP::Factory<K, IDType, FArgs ...> const& factory)
	{
		auto state = Stream::Get<state_t>(input);
		auto const& createInfo = DP::Factory<K, IDType, FArgs ...>::GetCreateInfo(Stream::Get<IDType>(input));
		auto* t = new(createInfo.size) SNode(createInfo.create, Stream::Get<std::remove_cvref_t<FArgs>>(input) ...);
		try {
			if (state.hasLeft) {
				t->left = SNode::Create(P, t, input, factory);
				t->state.hasLeft = true;
			} else
				t->left = P;
			if (state.hasRight) {
				t->right = SNode::Create(t, S, input, factory);
				t->state.hasRight = true;
			} else
				t->right = S;
			t->state.balance = state.balance;
			return t;
		} catch (...) {
			delete t;
			throw;
		}
	}

	void
	serialize(Stream::Output& output) const
	requires Stream::Serializable<K, Stream::Output>
	{
		output << state << static_cast<K const&>(key);
		if (state.hasLeft)
			left->serialize(output);
		if (state.hasRight)
			right->serialize(output);
	}

	SNode*
	attach(SNode*& created) noexcept
	{
		if (static_cast<K const&>(created->key) < static_cast<K const&>(key)) {
			if (state.hasLeft) {
				if (SNode* s = left->attach(created)) {
					if (left != s) {
						left = s;
						return nullptr;
					}
					if (state.isLeft) {
						if (left->state.isLeft)
							return ll();
						if (left->state.isRight)
							return lr();
					}
					state.balance <<= 1;
					if (state.isLeft)
						return this;
				}
				return nullptr;
			}
			state.hasLeft = true;
			created->left = left;
			created->right = this;
			left = created;
			state.balance <<= 1;
			return state.isLeft ? this : nullptr;
		}
		if (static_cast<K const&>(key) < static_cast<K const&>(created->key)) {
			if (state.hasRight) {
				if (SNode* s = right->attach(created)) {
					if (right != s) {
						right = s;
						return nullptr;
					}
					if (state.isRight) {
						if (right->state.isRight)
							return rr();
						if (right->state.isLeft)
							return rl();
					}
					state.balance >>= 1;
					if (state.isRight)
						return this;
				}
				return nullptr;
			}
			state.hasRight = true;
			created->right = right;
			created->left = this;
			right = created;
			state.balance >>= 1;
			return state.isRight ? this : nullptr;
		}
		created = this;
		return nullptr;
	}

	template <typename T>
	SNode*
	detach(T&& k, SNode*& toDel) noexcept
	{
		if (k < static_cast<K const&>(key)) {
			if (state.hasLeft) {
				bool leftWasBalanced = left->state.isBalanced;
				if (SNode* s = left->detach(std::forward<T>(k), toDel)) {
					if (left != s) {
						left = s;
						if (leftWasBalanced || !left->state.isBalanced)
							return nullptr;
					}
				} else if (toDel == left) {
					state.hasLeft = false;
					left = left->left;
				} else
					return nullptr;
				if (state.isRight) {
					if (right->state.isLeft)
						return rl();
					if (right->state.isRight)
						return rr();
					right->state.balance = 4;
					return leftRotate();
				}
				state.balance >>= 1;
				if (state.isBalanced)
					return this;
			}
			return nullptr;
		}
		if (static_cast<K const&>(key) < k) {
			if (state.hasRight) {
				bool rightWasBalanced = right->state.isBalanced;
				if (SNode* s = right->detach(std::forward<T>(k), toDel)) {
					if (right != s) {
						right = s;
						if (rightWasBalanced || !right->state.isBalanced)
							return nullptr;
					}
				} else if (toDel == right) {
					state.hasRight = false;
					right = right->right;
				} else
					return nullptr;
				if (state.isLeft) {
					if (left->state.isRight)
						return lr();
					if (left->state.isLeft)
						return ll();
					left->state.balance = 1;
					return rightRotate();
				}
				state.balance <<= 1;
				if (state.isBalanced)
					return this;
			}
			return nullptr;
		}
		toDel = this;
		if (state.hasRight) {
			if (state.hasLeft) {
				SNode* P = left, *S = right;
				while (P->state.hasRight)
					P = P->right;
				while (S->state.hasLeft)
					S = S->left;

				P->right = S;
				S->left = left;
				S->state.hasLeft = true;
				state.hasLeft = false;
				S->state.balance ^= state.balance;
				state.balance ^= S->state.balance;
				S->state.balance ^= state.balance;

				if (S->state.hasRight) {
					if (S != right)
						S->right->right->left = this;
					S->right->left = this;
				} else {
					state.hasRight = false;
					S->state.hasRight = true;
					if (S != right)
						S->right->left = this;
				}
				left = S->right;
				S->right = S != right ? right : this;
				right = left;
				left = S;

				bool rightWasBalanced = S->right->state.isBalanced;
				if (SNode* s = S->right->detach(std::forward<T>(k), toDel)) {
					if (S->right != s) {
						S->right = s;
						if (rightWasBalanced || !S->right->state.isBalanced)
							return S;
					}
				} else if (toDel == S->right) {
					S->state.hasRight = false;
					S->right = S->right->right;
				} else
					return S;
				if (S->state.isLeft) {
					if (S->left->state.isRight)
						return S->lr();
					if (S->left->state.isLeft)
						return S->ll();
					S->left->state.balance = 1;
					return S->rightRotate();
				}
				S->state.balance <<= 1;
				return S;
			}
			right->left = left;
			state.hasRight = false;
			return right;
		}
		if (state.hasLeft) {
			left->right = right;
			state.hasLeft = false;
			return left;
		}
		return nullptr;
	}

	SNode*
	ll() noexcept
	{
		state.balance = left->state.balance = 2;
		return rightRotate();
	}

	SNode*
	rr() noexcept
	{
		state.balance = right->state.balance = 2;
		return leftRotate();
	}

	SNode*
	lr() noexcept
	{
		state.balance = left->right->state.isLeft ? 1 : 2;
		left->state.balance = left->right->state.isRight ? 4 : 2;
		left->right->state.balance = 2;
		left = left->leftRotate();
		return rightRotate();
	}

	SNode*
	rl() noexcept
	{
		state.balance = right->left->state.isRight ? 4 : 2;
		right->state.balance = right->left->state.isLeft ? 1 : 2;
		right->left->state.balance = 2;
		right = right->rightRotate();
		return leftRotate();
	}

	SNode*
	leftRotate() noexcept
	{
		SNode* Y = right;
		if (Y->state.hasLeft)
			right = Y->left;
		else {
			state.hasRight = false;
			Y->state.hasLeft = true;
		}
		Y->left = this;
		return Y;
	}

	SNode*
	rightRotate() noexcept
	{
		SNode* Y = left;
		if (Y->state.hasRight)
			left = Y->right;
		else {
			state.hasLeft = false;
			Y->state.hasRight = true;
		}
		Y->right = this;
		return Y;
	}
};//struct SNode<K>

}//namespace DS

#endif //DS_SNODE_HPP
