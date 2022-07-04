#ifndef DS_MNODE_HPP
#define DS_MNODE_HPP

#include "SNode.hpp"

namespace DS {

struct MException {
	enum class Code : int {
		LargerKey = 1
	};
};//struct MException

template <typename K, typename V, typename ... Cs>
struct MNode : SNode<K, Cs ...> {
	Holder<V> val;

	static void*
	operator new(std::size_t, std::size_t valSize)
	{ return ::operator new(Offset(&MNode::val) + valSize); }

	static void
	operator delete(void* ptr)
	{ ::operator delete(ptr); }

	template <typename ... KArgs>
	explicit MNode(KArgs&& ... kArgs)
			: SNode<K, Cs ...>(std::forward<KArgs>(kArgs) ...)
	{}

	~MNode()
	{
		if (this->d[0].hasValue)
			val->~V();
	}

	static TNode<sizeof...(Cs)>*
	Create(TNode<sizeof...(Cs)>* P, TNode<sizeof...(Cs)>* S,
			TNode<sizeof...(Cs)> const* other)
	{
		auto* t = ::new MNode(static_cast<K const&>(reinterpret_cast<MNode const*>(other)->key));
		try {
			if (other->d[0].hasValue)
				::new(static_cast<void*>(t->val)) Holder<V>(static_cast<V const&>(reinterpret_cast<MNode const*>(other)->val));
			t->left(other->d[0].hasLeft ? MNode::Create(P, t, other->left()) : P);
			t->right(other->d[0].hasRight ? MNode::Create(t, S, other->right()) : S);
			t->state(other->state());
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
	requires Deserializable<K, Stream::Input&, KArgs ...> && Deserializable<V, Stream::Input&, VArgs ...>
	{
		auto state = Stream::Get<std::uint8_t>(input);
		auto* t = reinterpret_cast<MNode*>(::operator new(sizeof(MNode)));
		t->template state(2);

		try {
			if (state & 0x20) {
				::new(static_cast<void*>(t->val)) Holder<V>(input, std::forward<VArgs>(vArgs) ...);
				t->d[0].hasValue = true;
			}
			::new(static_cast<void*>(t->key)) Holder<K>(input, std::forward<KArgs>(kArgs) ...);
		} catch (...) {
			if (t->d[0].hasValue)
				t->val->~V();
			::operator delete(t);
			throw;
		}

		try {
			t->left(state & 0x40 ? MNode::Create(P, t, std::forward<KArgs>(kArgs) ..., input, std::forward<VArgs>(vArgs) ...) : P);
			t->right(state & 0x10 ? MNode::Create(t, S, std::forward<KArgs>(kArgs) ..., input, std::forward<VArgs>(vArgs) ...) : S);
			t->state(state);
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
	requires Deserializable<K, Stream::Input&, KArgs ...>
	{
		auto state = Stream::Get<std::uint8_t>(input);
		MNode* t;

		if (state & 0x20) {
			auto const& vCreateInfo = DP::Factory<V, VIDType, VFArgs ...>::GetCreateInfo(Stream::Get<VIDType>(input));
			t = reinterpret_cast<MNode*>(operator new(sizeof(MNode), vCreateInfo.size));
			t->template state(2);
			try {
				::new(static_cast<void*>(t->val)) Holder<V>(vCreateInfo.create, Stream::Get<std::remove_cvref_t<VFArgs>>(input) ...);
				t->d[0].hasValue = true;
			} catch (...) {
				operator delete(t);
				throw;
			}
		} else {
			t = reinterpret_cast<MNode*>(operator new(sizeof(MNode), sizeof(V)));
			t->template state(2);
		}

		try {
			::new(static_cast<void*>(t->key)) Holder<K>(input, std::forward<KArgs>(kArgs) ...);
		} catch (...) {
			if (t->d[0].hasValue)
				t->val->~V();
			operator delete(t);
			throw;
		}

		try {
			t->left(state & 0x40 ? MNode::Create(P, t, std::forward<KArgs>(kArgs) ..., input, vFactory) : P);
			t->right(state & 0x10 ? MNode::Create(t, S, std::forward<KArgs>(kArgs) ..., input, vFactory) : S);
			t->state(state);
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
	requires Deserializable<V, Stream::Input&, VArgs ...>
	{
		auto state = Stream::Get<std::uint8_t>(input);
		auto* t = reinterpret_cast<MNode*>(::operator new(sizeof(MNode)));
		t->template state(2);

		try {
			if (state & 0x20) {
				::new(static_cast<void*>(t->val)) Holder<V>(input, std::forward<VArgs>(vArgs) ...);
				t->d[0].hasValue = true;
			}
			auto const& kCreateInfo = DP::Factory<K, KIDType, KFArgs ...>::GetCreateInfo(Stream::Get<KIDType>(input));
			if (kCreateInfo.size > sizeof(K))
				throw std::system_error(MException::Code::LargerKey, "kCreateInfo.size is greater than the size of K.");

			::new(static_cast<void*>(t->key)) Holder<K>(kCreateInfo.create, Stream::Get<std::remove_cvref_t<KFArgs>>(input) ...);
		} catch (...) {
			if (t->d[0].hasValue)
				t->val->~V();
			::operator delete(t);
			throw;
		}

		try {
			t->left(state & 0x40 ? MNode::Create(P, t, kFactory, input, std::forward<VArgs>(vArgs) ...) : P);
			t->right(state & 0x10 ? MNode::Create(t, S, kFactory, input, std::forward<VArgs>(vArgs) ...) : S);
			t->state(state);
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
		auto state = Stream::Get<std::uint8_t>(input);
		MNode* t;

		if (state & 0x20) {
			auto const& vCreateInfo = DP::Factory<V, VIDType, VFArgs ...>::GetCreateInfo(Stream::Get<VIDType>(input));
			t = reinterpret_cast<MNode*>(operator new(sizeof(MNode), vCreateInfo.size));
			t->template state(2);
			try {
				::new(static_cast<void*>(t->val)) Holder<V>(vCreateInfo.create, Stream::Get<std::remove_cvref_t<VFArgs>>(input) ...);
				t->d[0].hasValue = true;
			} catch (...) {
				operator delete(t);
				throw;
			}
		} else {
			t = reinterpret_cast<MNode*>(operator new(sizeof(MNode), sizeof(V)));
			t->template state(2);
		}

		try {
			auto const& kCreateInfo = DP::Factory<K, KIDType, KFArgs ...>::GetCreateInfo(Stream::Get<KIDType>(input));
			if (kCreateInfo.size > sizeof(K))
				throw std::system_error(MException::Code::LargerKey, "kCreateInfo.size is greater than the size of K.");

			::new(static_cast<void*>(t->key)) Holder<K>(kCreateInfo.create, Stream::Get<std::remove_cvref_t<KFArgs>>(input) ...);
		} catch (...) {
			if (t->d[0].hasValue)
				t->val->~V();
			operator delete(t);
			throw;
		}

		try {
			t->left(state & 0x40 ? MNode::Create(P, t, kFactory, input, vFactory) : P);
			t->right(state & 0x10 ? MNode::Create(t, S, kFactory, input, vFactory) : S);
			t->state(state);
			return t;
		} catch (...) {
			delete t;
			throw;
		}
	}

	void
	unset() noexcept
	{
		val->~V();
		this->d[0].hasValue = false;
	}

	template <typename ... Args>
	V&
	set(Args&& ... args)
	{
		::new(static_cast<void*>(val)) Holder<V>(std::forward<Args>(args) ...);
		this->d[0].hasValue = true;
		return *val;
	}

	void
	serialize(Stream::Output& output) const
	requires Stream::Serializable<K, Stream::Output&> && Stream::Serializable<V, Stream::Output&>
	{
		output << this->state();
		if (this->d[0].hasValue)
			output << static_cast<V const&>(val);
		output << static_cast<K const&>(this->key);
		if (this->d[0].hasLeft)
			this->template left<0, MNode>()->serialize(output);
		if (this->d[0].hasRight)
			this->template right<0, MNode>()->serialize(output);
	}

	template <std::size_t N>
	Stream::Format::DotOutput&
	toDot(Stream::Format::DotOutput& dotOutput) const
	requires Stream::Serializable<V, Stream::Format::StringOutput&>
	{
		if (this->d[N].hasLeft)
			this->template left<N, MNode>()->template toDot<N>(dotOutput);
		if (this->d[N].hasRight)
			this->template right<N, MNode>()->template toDot<N>(dotOutput);
		if (this->d[0].hasValue)
			dotOutput << N << this << "[tooltip=\"" << static_cast<V const&>(val) << "\"]\n";
		return dotOutput;
	}
};//struct MNode<K, V, Cs ...>

std::error_code
make_error_code(MException::Code e) noexcept
{
	static struct : std::error_category {
		[[nodiscard]] char const*
		name() const noexcept override
		{ return "DS::MNode"; }

		[[nodiscard]] std::string
		message(int e) const noexcept override
		{ return e == 1 ? "Larger Key" : "Unknown Error"; }
	} instance;
	return {static_cast<int>(e), instance};
}

}//namespace DS

namespace std {

template<>
struct is_error_code_enum<DS::MException::Code> : true_type {};

}//namespace std

#endif //DS_MNODE_HPP
