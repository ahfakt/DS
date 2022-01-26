#ifndef DS_MNODE_HPP
#define DS_MNODE_HPP

#include "Holder.hpp"

namespace DS {

template <typename K, typename V>
struct MNode {
	MNode* left;
	MNode* right;
	union state_t {
		std::uint8_t _ = 2;
		std::uint8_t balance:3;
		struct {
			bool isRight:1;
			bool isBalanced:1;
			bool isLeft:1;
			bool :2;
			bool hasRight:1;
			bool hasValue:1;
			bool hasLeft:1;
		};
	} state;
	Holder<K> key;
	Holder<V> val;

	static void*
	operator new(std::size_t, std::size_t valSize)
	{ return ::operator new(Offset(&MNode::val) + valSize); }

	static void
	operator delete(void* ptr)
	{ ::operator delete(ptr); }

	struct Exception : std::runtime_error
	{ using std::runtime_error::runtime_error; };

	template <typename ... KArgs>
	explicit MNode(KArgs&& ... kArgs)
			: key(std::forward<KArgs>(kArgs) ...)
	{}

	~MNode()
	{
		if (state.hasRight)
			delete right;
		if (state.hasLeft)
			delete left;
		if (state.hasValue)
			val->~V();
		key->~K();
	}

	static MNode*
	Create(MNode* P, MNode* S, MNode const* other)
	{
		auto* t = ::new MNode(static_cast<K const&>(other->key));
		try {
			if (other->state.hasValue) {
				::new(static_cast<void*>(t->val)) Holder<V>(static_cast<V const&>(other->val));
				t->state.hasValue = true;
			}
			if (other->state.hasLeft) {
				t->left = MNode::Create(P, t, other->left);
				t->state.hasLeft = true;
			} else
				t->left = P;
			if (other->state.hasRight) {
				t->right = MNode::Create(t, S, other->right);
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

	template <typename ... KArgs, typename ... VArgs>
	static MNode*
	Create(MNode* P, MNode* S,
			KArgs&& ... kArgs, Stream::Input& input, VArgs&& ... vArgs)
	requires Deserializable<K, Stream::Input, KArgs ...> && Deserializable<V, Stream::Input, VArgs ...>
	{
		auto state = Stream::Get<state_t>(input);
		auto* t = reinterpret_cast<MNode*>(::operator new(sizeof(MNode)));
		t->state._ = 2;

		try {
			if (state.hasValue) {
				::new(static_cast<void*>(t->val)) Holder<V>(input, std::forward<VArgs>(vArgs) ...);
				t->state.hasValue = true;
			}
			::new(static_cast<void*>(t->key)) Holder<K>(input, std::forward<KArgs>(kArgs) ...);
		} catch (...) {
			if (t->state.hasValue)
				t->val->~V();
			::operator delete(t);
			throw;
		}

		try {
			if (state.hasLeft) {
				t->left = MNode::Create(P, t, std::forward<KArgs>(kArgs) ..., input, std::forward<VArgs>(vArgs) ...);
				t->state.hasLeft = true;
			} else
				t->left = P;
			if (state.hasRight) {
				t->right = MNode::Create(t, S, std::forward<KArgs>(kArgs) ..., input, std::forward<VArgs>(vArgs) ...);
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

	template <typename ... KArgs, typename VIDType, typename ... VFArgs>
	static MNode*
	Create(MNode* P, MNode* S,
			KArgs&& ... kArgs, Stream::Input& input, DP::Factory<V, VIDType, VFArgs ...> const& vFactory)
	requires Deserializable<K, Stream::Input, KArgs ...>
	{
		auto state = Stream::Get<state_t>(input);
		MNode* t;

		if (state.hasValue) {
			auto const& vCreateInfo = DP::Factory<V, VIDType, VFArgs ...>::GetCreateInfo(Stream::Get<VIDType>(input));
			t = reinterpret_cast<MNode*>(operator new(sizeof(MNode), vCreateInfo.size));
			t->state._ = 2;
			try {
				::new(static_cast<void*>(t->val)) Holder<V>(vCreateInfo.create, Stream::Get<std::remove_cvref_t<VFArgs>>(input) ...);
				t->state.hasValue = true;
			} catch (...) {
				operator delete(t);
				throw;
			}
		} else {
			t = reinterpret_cast<MNode*>(operator new(sizeof(MNode), sizeof(V)));
			t->state._ = 2;
		}

		try {
			::new(static_cast<void*>(t->key)) Holder<K>(input, std::forward<KArgs>(kArgs) ...);
		} catch (...) {
			if (t->state.hasValue)
				t->val->~V();
			operator delete(t);
			throw;
		}

		try {
			if (state.hasLeft) {
				t->left = MNode::Create(P, t, std::forward<KArgs>(kArgs) ..., input, vFactory);
				t->state.hasLeft = true;
			} else
				t->left = P;
			if (state.hasRight) {
				t->right = MNode::Create(t, S, std::forward<KArgs>(kArgs) ..., input, vFactory);
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

	template <typename KIDType, typename ... KFArgs, typename ... VArgs>
	static MNode*
	Create(MNode* P, MNode* S,
			DP::Factory<K, KIDType, KFArgs ...> const& kFactory, Stream::Input& input, VArgs&& ... vArgs)
	requires Deserializable<V, Stream::Input, VArgs ...>
	{
		auto state = Stream::Get<state_t>(input);
		auto* t = reinterpret_cast<MNode*>(::operator new(sizeof(MNode)));
		t->state._ = 2;

		try {
			if (state.hasValue) {
				::new(static_cast<void*>(t->val)) Holder<V>(input, std::forward<VArgs>(vArgs) ...);
				t->state.hasValue = true;
			}
			auto const& kCreateInfo = DP::Factory<K, KIDType, KFArgs ...>::GetCreateInfo(Stream::Get<KIDType>(input));
			if (kCreateInfo.size > sizeof(K))
				throw Exception("kCreateInfo.size is greater than the size of K.");

			::new(static_cast<void*>(t->key)) Holder<K>(kCreateInfo.create, Stream::Get<std::remove_cvref_t<KFArgs>>(input) ...);
		} catch (...) {
			if (t->state.hasValue)
				t->val->~V();
			::operator delete(t);
			throw;
		}

		try {
			if (state.hasLeft) {
				t->left = MNode::Create(P, t, kFactory, input, std::forward<VArgs>(vArgs) ...);
				t->state.hasLeft = true;
			} else
				t->left = P;
			if (state.hasRight) {
				t->right = MNode::Create(t, S, kFactory, input, std::forward<VArgs>(vArgs) ...);
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

	template <typename KIDType, typename ... KFArgs, typename VIDType, typename ... VFArgs>
	static MNode*
	Create(MNode* P, MNode* S,
			DP::Factory<K, KIDType, KFArgs ...> const& kFactory, Stream::Input& input, DP::Factory<V, VIDType, VFArgs ...> const& vFactory)
	{
		auto state = Stream::Get<state_t>(input);
		MNode* t;

		if (state.hasValue) {
			auto const& vCreateInfo = DP::Factory<V, VIDType, VFArgs ...>::GetCreateInfo(Stream::Get<VIDType>(input));
			t = reinterpret_cast<MNode*>(operator new(sizeof(MNode), vCreateInfo.size));
			t->state._ = 2;
			try {
				::new(static_cast<void*>(t->val)) Holder<V>(vCreateInfo.create, Stream::Get<std::remove_cvref_t<VFArgs>>(input) ...);
				t->state.hasValue = true;
			} catch (...) {
				operator delete(t);
				throw;
			}
		} else {
			t = reinterpret_cast<MNode*>(operator new(sizeof(MNode), sizeof(V)));
			t->state._ = 2;
		}

		try {
			auto const& kCreateInfo = DP::Factory<K, KIDType, KFArgs ...>::GetCreateInfo(Stream::Get<KIDType>(input));
			if (kCreateInfo.size > sizeof(K))
				throw Exception("kCreateInfo.size is greater than the size of K.");

			::new(static_cast<void*>(t->key)) Holder<K>(kCreateInfo.create, Stream::Get<std::remove_cvref_t<KFArgs>>(input) ...);
		} catch (...) {
			if (t->state.hasValue)
				t->val->~V();
			operator delete(t);
			throw;
		}

		try {
			if (state.hasLeft) {
				t->left = MNode::Create(P, t, kFactory, input, vFactory);
				t->state.hasLeft = true;
			} else
				t->left = P;
			if (state.hasRight) {
				t->right = MNode::Create(t, S, kFactory, input, vFactory);
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
	requires Stream::Serializable<K, Stream::Output> && Stream::Serializable<V, Stream::Output>
	{
		output << state;
		if (state.hasValue)
			output << static_cast<V const&>(val);
		output << static_cast<K const&>(key);
		if (state.hasLeft)
			left->serialize(output);
		if (state.hasRight)
			right->serialize(output);
	}

	void
	unset() noexcept
	{
		val->~V();
		state.hasValue = false;
	}

	template <typename ... Args>
	V&
	set(Args&& ... args)
	{
		::new(static_cast<void*>(val)) Holder<V>(std::forward<Args>(args) ...);
		state.hasValue = true;
		return *val;
	}

	MNode*
	attach(MNode*& created) noexcept
	{
		if (static_cast<K const&>(created->key) < static_cast<K const&>(key)) {
			if (state.hasLeft) {
				if (MNode* m = left->attach(created)) {
					if (left != m) {
						left = m;
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
				if (MNode* m = right->attach(created)) {
					if (right != m) {
						right = m;
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
	MNode*
	detach(T&& k, MNode*& toDel) noexcept
	{
		if (k < static_cast<K const&>(key)) {
			if (state.hasLeft) {
				bool leftWasBalanced = left->isBalanced;
				if (MNode* m = left->detach(std::forward<T>(k), toDel)) {
					if (left != m) {
						left = m;
						if (leftWasBalanced || !left->isBalanced)
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
				if (MNode* m = right->detach(std::forward<T>(k), toDel)) {
					if (right != m) {
						right = m;
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
				MNode* P = left, *S = right;
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
				if (MNode* m = S->right->detach(std::forward<T>(k), toDel)) {
					if (S->right != m) {
						S->right = m;
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

	MNode*
	ll() noexcept
	{
		state.balance = left->state.balance = 2;
		return rightRotate();
	}

	MNode*
	rr() noexcept
	{
		state.balance = right->state.balance = 2;
		return leftRotate();
	}

	MNode*
	lr() noexcept
	{
		state.balance = left->right->state.isLeft ? 1 : 2;
		left->state.balance = left->right->state.isRight ? 4 : 2;
		left->right->state.balance = 2;
		left = left->leftRotate();
		return rightRotate();
	}

	MNode*
	rl() noexcept
	{
		state.balance = right->left->state.isRight ? 4 : 2;
		right->state.balance = right->left->state.isLeft ? 1 : 2;
		right->left->state.balance = 2;
		right = right->rightRotate();
		return leftRotate();
	}

	MNode*
	leftRotate() noexcept
	{
		MNode* Y = right;
		if (Y->state.hasLeft)
			right = Y->left;
		else {
			state.hasRight = false;
			Y->state.hasLeft = true;
		}
		Y->left = this;
		return Y;
	}

	MNode*
	rightRotate() noexcept
	{
		MNode* Y = left;
		if (Y->state.hasRight)
			left = Y->right;
		else {
			state.hasLeft = false;
			Y->state.hasRight = true;
		}
		Y->right = this;
		return Y;
	}
};//struct MNode<K, V>

}//namespace DS

#endif //DS_MNODE_HPP
