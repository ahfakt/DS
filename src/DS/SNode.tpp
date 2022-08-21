#ifndef DS_SNODE_TPP
#define DS_SNODE_TPP

#include "TNode.tpp"
#include "Holder.tpp"
#include <DP/Builder.hpp>
#include <DP/Factory.hpp>

namespace DS {

struct SException {
	enum class Code : int {
		KeyCollision = 1
	};
};//struct SException

}//namespace DS

namespace std {

template<>
struct is_error_code_enum<DS::SException::Code> : true_type {};

}//namespace std

namespace DS {

inline std::error_code
make_error_code(SException::Code e) noexcept
{
	static struct : std::error_category {
		[[nodiscard]] char const*
		name() const noexcept override
		{ return "DS::SNode"; }

		[[nodiscard]] std::string
		message(int e) const noexcept override
		{ return e == 1 ? "Key Collision" : "Unknown Error"; }
	} instance;
	return {static_cast<int>(e), instance};
}

template <typename K, typename ... Cs>
struct SNode : TNode<sizeof...(Cs)> {
	Holder<K> key;

	static void*
	operator new(std::size_t, std::size_t const keySize)
	{ return ::operator new(Offset(&SNode::key) + keySize); }

	static void
	operator delete(void* ptr)
	{ ::operator delete(ptr); }

	explicit SNode(auto&& ... args)
			: key(std::forward<decltype(args)>(args) ...)
	{}

	~SNode()
	{ key->~K(); }

	static TNode<sizeof...(Cs)>*
	Create(TNode<sizeof...(Cs)>* P, TNode<sizeof...(Cs)>* S,
			TNode<sizeof...(Cs)> const* other)
	{
		auto* t = ::new SNode(static_cast<K const&>(reinterpret_cast<SNode const*>(other)->key));
		try {
			t->left(other->d[0].hasLeft ? SNode::Create(P, t, other->left()) : P);
			t->right(other->d[0].hasRight ? SNode::Create(t, S, other->right()) : S);
			t->state(other->state());
			return t;
		} catch (...) {
			delete t;
			throw;
		}
	}

	static TNode<sizeof...(Cs)>*
	Create(TNode<sizeof...(Cs)>* P, TNode<sizeof...(Cs)>* S,
			Stream::Input& input, auto&& ... kArgs)
	requires Stream::DeserializableWith<K, decltype(input), decltype(kArgs) ...>
	{
		auto state = Stream::Get<std::uint8_t>(input);
		auto* t = ::new SNode(input, std::forward<decltype(kArgs)>(kArgs) ...);
		try {
			t->left(state & 0x40 ? SNode::Create(P, t, input, std::forward<decltype(kArgs)>(kArgs) ...) : P);
			t->right(state & 0x10 ? SNode::Create(t, S, input, std::forward<decltype(kArgs)>(kArgs) ...) : S);
			t->state(state);
			return t;
		} catch (...) {
			delete t;
			throw;
		}
	}

	template <typename ID, typename ... Args>
	static TNode<sizeof...(Cs)>*
	Create(TNode<sizeof...(Cs)>* P, TNode<sizeof...(Cs)>* S,
			Stream::Input& input, DP::Factory<K, ID, Args ...>, auto&& ... kArgs)
	{
		using seq = std::make_index_sequence<sizeof...(Args) - sizeof...(kArgs)>;
		auto state = Stream::Get<std::uint8_t>(input);
		auto const& createInfo = DP::Factory<K, ID, Args ...>::GetCreateInfo(Stream::Get<ID>(input));
		auto* t = new(createInfo.size) SNode(createInfo, input, seq{}, std::forward<decltype(kArgs)>(kArgs) ...);
		try {
			t->left(state & 0x40 ? SNode::Create(P, t, input, DP::Factory<K, ID, Args ...>{}) : P);
			t->right(state & 0x10 ? SNode::Create(t, S, input, DP::Factory<K, ID, Args ...>{}) : S);
			t->state(state);
			return t;
		} catch (...) {
			delete t;
			throw;
		}
	}

	template <typename Node, typename Exception, std::size_t N>
	static void
	Attach(TNode<sizeof...(Cs)>* root[], TNode<sizeof...(Cs)>* created)
	{
		auto* s = created;
		if (auto* t = reinterpret_cast<SNode*>(root[N])->template attach<N>(&created))
			root[N] = t;
		else if (s != created) {
			root[0]->template deleteTree<Node>();
			throw Exception(SException::Code::KeyCollision, std::to_string(N + 1) + ". comparator");
		}
		if (created->d[0].hasLeft)
			Attach<Node, Exception, N>(root, created->template left<0>());
		if (created->d[0].hasRight)
			Attach<Node, Exception, N>(root, created->template right<0>());
	}

	template <typename Node, typename Exception, std::size_t N = 1>
	static void
	BuildTree(TNode<sizeof...(Cs)>* root[])
	{
		root[N] = root[0];
		root[N]->template left<N>(nullptr);
		root[N]->template right<N>(nullptr);
		root[N]->template state<N>(2);
		if (root[0]->d[0].hasLeft)
			Attach<Node, Exception, N>(root, root[0]->template left<0>());
		if (root[0]->d[0].hasRight)
			Attach<Node, Exception, N>(root, root[0]->template right<0>());
		if constexpr(N < sizeof...(Cs) - 1)
			BuildTree<Node, Exception, N + 1>(root);
	}

	template <std::size_t N>
	TNode<sizeof...(Cs)>*
	get(auto&& ... args)
	{
		auto* t = this;
		DP::Type<N, Cs ...> cmp;
		while(true) {
			if (cmp(std::forward<decltype(args)>(args) ..., static_cast<K const&>(t->key))) {
				if (t->d[N].hasLeft) {
					t = t->template left<N, SNode>();
					continue;
				}
				return nullptr;
			}
			if (cmp(static_cast<K const&>(t->key), std::forward<decltype(args)>(args) ...)) {
				if (t->d[N].hasRight) {
					t = t->template right<N, SNode>();
					continue;
				}
				return nullptr;
			}
			return t;
		}
	}

	template <std::size_t N>
	TNode<sizeof...(Cs)>*
	attach(TNode<sizeof...(Cs)>** created) noexcept
	{
		if (DP::Type<N, Cs ...>{}(static_cast<K const&>(reinterpret_cast<SNode*>(*created)->key), static_cast<K const&>(key)))
			return this->d[N].hasLeft
				? this->template attachedToLeft<N>(this->template left<N, SNode>()->template attach<N>(created))
				: this->template attachToLeft<N>(*created);

		if (DP::Type<N, Cs ...>{}(static_cast<K const&>(key), static_cast<K const&>(reinterpret_cast<SNode*>(*created)->key)))
			return this->d[N].hasRight
				? this->template attachedToRight<N>(this->template right<N, SNode>()->template attach<N>(created))
				: this->template attachToRight<N>(*created);

		*created = this;
		return nullptr;
	}

	template <std::size_t N>
	TNode<sizeof...(Cs)>*
	detachFromLeft(TNode<sizeof...(Cs)>** toDel)
	{
		if (this->d[N].hasLeft) {
			bool leftWasBalanced = this->template left<N>()->d[N].isBalanced;
			if (auto* t = this->template left<N, SNode>()->template detach<N>(toDel)) {
				if (t != this->template left<N>()) {
					this->template left<N>(t);
					if (leftWasBalanced || !this->template left<N>()->d[N].isBalanced)
						return nullptr;
				} else if (t == *toDel) {
					this->d[N].hasLeft = false;
					this->template left<N>(this->template left<N>()->template left<N>());
				}
				if (this->d[N].isRight) {
					if (this->template right<N>()->d[N].isLeft)
						return this->template rl<N>();
					if (this->template right<N>()->d[N].isRight)
						return this->template rr<N>();
					this->template right<N>()->d[N].balance = 4;
					return this->template leftRotate<N>();
				}
				this->d[N].balance >>= 1;
				if (this->d[N].isBalanced)
					return this;
			}
		}
		return nullptr;
	}

	template <std::size_t N>
	TNode<sizeof...(Cs)>*
	detachFromRight(TNode<sizeof...(Cs)>** toDel)
	{
		if (this->d[N].hasRight) {
			bool rightWasBalanced = this->template right<N>()->d[N].isBalanced;
			if (auto* t = this->template right<N, SNode>()->template detach<N>(toDel)) {
				if (t != this->template right<N>()) {
					this->template right<N>(t);
					if (rightWasBalanced || !this->template right<N>()->d[N].isBalanced)
						return nullptr;
				} else if (t == *toDel) {
					this->d[N].hasRight = false;
					this->template right<N>(this->template right<N>()->template right<N>());
				}
				if (this->d[N].isLeft) {
					if (this->template left<N>()->d[N].isRight)
						return this->template lr<N>();
					if (this->template left<N>()->d[N].isLeft)
						return this->template ll<N>();
					this->template left<N>()->d[N].balance = 1;
					return this->template rightRotate<N>();
				}
				this->d[N].balance <<= 1;
				if (this->d[N].isBalanced)
					return this;
			}
		}
		return nullptr;
	}

	template <std::size_t N>
	TNode<sizeof...(Cs)>*
	detach(TNode<sizeof...(Cs)>** toDel) noexcept
	{
		if (DP::Type<N, Cs ...>{}(static_cast<K const&>(reinterpret_cast<SNode*>(*toDel)->key), static_cast<K const&>(key)))
			return detachFromLeft<N>(toDel);
		if (DP::Type<N, Cs ...>{}(static_cast<K const&>(key), static_cast<K const&>(reinterpret_cast<SNode*>(*toDel)->key)))
			return detachFromRight<N>(toDel);
		if (this != *toDel)
			return *toDel = nullptr;
		if (this->d[N].hasRight) {
			if (this->d[N].hasLeft) {
				auto* S = this->template swapWithNext<N, SNode>();
				if (auto* s = S->template detachFromRight<N>(toDel))
					return s;
				return S;
			}
			this->template right<N>()->template left<N>(this->template left<N>());
			return this->template right<N>();
		}
		if (this->d[N].hasLeft) {
			this->template left<N>()->template right<N>(this->template right<N>());
			return this->template left<N>();
		}
		return this;
	}

	void
	serialize(Stream::Output& output) const
	requires Stream::InsertableTo<K, decltype(output)>
	{
		output << this->state() << static_cast<K const&>(key);
		if (this->d[0].hasLeft)
			this->template left<0, SNode>()->serialize(output);
		if (this->d[0].hasRight)
			this->template right<0, SNode>()->serialize(output);
	}

	template <std::size_t N>
	Stream::Format::DotOutput&
	toDot(Stream::Format::DotOutput& dotOutput) const
	requires Stream::InsertableTo<K, decltype(dotOutput)>
	{
		if (this->d[N].hasLeft)
			this->template left<N, SNode>()->template toDot<N>(dotOutput);
		if (this->d[N].hasRight)
			this->template right<N, SNode>()->template toDot<N>(dotOutput);
		dotOutput << N << this << "[label=\"" << static_cast<K const&>(key) << "\"]\n";
		return dotOutput;
	}
};//struct SNode<K, Cs ...>

}//namespace DS

#endif //DS_SNODE_TPP
