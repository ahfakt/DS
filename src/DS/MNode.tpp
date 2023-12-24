#pragma once

#include "SNode.tpp"

namespace DS {

struct MException {
	enum class Code : int {
		LargerKey = 1
	};
};//struct MException

std::error_code
make_error_code(MException::Code e) noexcept;

}//namespace DS

namespace std {

template<>
struct is_error_code_enum<DS::MException::Code> : true_type {};

}//namespace std

namespace DS {

inline std::error_code
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

template <typename K, typename V, typename ... Cs>
struct MNode : SNode<K, Cs ...> {
	Holder<V> val;

	static void*
	operator new(std::size_t, std::size_t valSize)
	{ return ::operator new(Offset(&MNode::val) + valSize); }

	static void
	operator delete(void* ptr)
	{ ::operator delete(ptr); }

	explicit MNode(auto&& ... kArgs)
			: SNode<K, Cs ...>(std::forward<decltype(kArgs)>(kArgs) ...)
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

	static MNode*
	Create(MNode* P, MNode* S,
			Stream::Input& input)
	requires
		Stream::Deserializable<K, decltype(input)> &&
		Stream::Deserializable<V, decltype(input)>
	{
		auto state = Stream::Get<std::uint8_t>(input);
		auto* t = reinterpret_cast<MNode*>(::operator new(sizeof(MNode)));
		t->template state(2);

		try {
			if (state & 0x20) {
				::new(static_cast<void*>(t->val)) Holder<V>(input);
				t->d[0].hasValue = true;
			}
			::new(static_cast<void*>(t->key)) Holder<K>(input);
		} catch (...) {
			if (t->d[0].hasValue)
				t->val->~V();
			::operator delete(t);
			throw;
		}

		try {
			t->left(state & 0x40 ? MNode::Create(P, t, input) : P);
			t->right(state & 0x10 ? MNode::Create(t, S, input) : S);
			t->state(state);
			return t;
		} catch (...) {
			delete t;
			throw;
		}
	}

	template <typename ... VArgs>
	static MNode*
	Create(MNode* P, MNode* S,
			Stream::Input& input, Pack<VArgs ...>& vArgs)
	requires
		Stream::Deserializable<K, decltype(input)> &&
		Stream::Deserializable<V, decltype(input), VArgs ...>
	{
		auto state = Stream::Get<std::uint8_t>(input);
		auto* t = reinterpret_cast<MNode*>(::operator new(sizeof(MNode)));
		t->template state(2);

		try {
			if (state & 0x20) {
				::new(static_cast<void*>(t->val)) Holder<V>(input, vArgs, std::make_index_sequence<sizeof...(VArgs)>{});
				t->d[0].hasValue = true;
			}
			::new(static_cast<void*>(t->key)) Holder<K>(input);
		} catch (...) {
			if (t->d[0].hasValue)
				t->val->~V();
			::operator delete(t);
			throw;
		}

		try {
			t->left(state & 0x40 ? MNode::Create(P, t, input, vArgs) : P);
			t->right(state & 0x10 ? MNode::Create(t, S, input, vArgs) : S);
			t->state(state);
			return t;
		} catch (...) {
			delete t;
			throw;
		}
	}

	template <typename ... KArgs>
	static MNode*
	Create(MNode* P, MNode* S,
			Pack<KArgs ...>& kArgs, Stream::Input& input)
	requires
		Stream::Deserializable<K, decltype(input), KArgs ...> &&
		Stream::Deserializable<V, decltype(input)>
	{
		auto state = Stream::Get<std::uint8_t>(input);
		auto* t = reinterpret_cast<MNode*>(::operator new(sizeof(MNode)));
		t->template state(2);

		try {
			if (state & 0x20) {
				::new(static_cast<void*>(t->val)) Holder<V>(input);
				t->d[0].hasValue = true;
			}
			::new(static_cast<void*>(t->key)) Holder<K>(input, kArgs, std::make_index_sequence<sizeof...(KArgs)>{});
		} catch (...) {
			if (t->d[0].hasValue)
				t->val->~V();
			::operator delete(t);
			throw;
		}

		try {
			t->left(state & 0x40 ? MNode::Create(P, t, kArgs, input) : P);
			t->right(state & 0x10 ? MNode::Create(t, S, kArgs, input) : S);
			t->state(state);
			return t;
		} catch (...) {
			delete t;
			throw;
		}
	}

	template <typename ... KArgs, typename ... VArgs>
	static MNode*
	Create(MNode* P, MNode* S,
			Pack<KArgs ...>& kArgs, Stream::Input& input, Pack<VArgs ...>& vArgs)
	requires
		Stream::Deserializable<K, decltype(input), KArgs ...> &&
		Stream::Deserializable<V, decltype(input), VArgs ...>
	{
		auto state = Stream::Get<std::uint8_t>(input);
		auto* t = reinterpret_cast<MNode*>(::operator new(sizeof(MNode)));
		t->template state(2);

		try {
			if (state & 0x20) {
				::new(static_cast<void*>(t->val)) Holder<V>(input, vArgs, std::make_index_sequence<sizeof...(VArgs)>{});
				t->d[0].hasValue = true;
			}
			::new(static_cast<void*>(t->key)) Holder<K>(input, kArgs, std::make_index_sequence<sizeof...(KArgs)>{});
		} catch (...) {
			if (t->d[0].hasValue)
				t->val->~V();
			::operator delete(t);
			throw;
		}

		try {
			t->left(state & 0x40 ? MNode::Create(P, t, kArgs, input, vArgs) : P);
			t->right(state & 0x10 ? MNode::Create(t, S, kArgs, input, vArgs) : S);
			t->state(state);
			return t;
		} catch (...) {
			delete t;
			throw;
		}
	}

	template <typename VType, typename ... VArgs>
	static MNode*
	Create(MNode* P, MNode* S,
			auto&& ... kArgs, Stream::Input& input, DP::Factory<V, VType, VArgs ...>, auto&& ... vArgs)
	requires Stream::Deserializable<K, decltype(input), decltype(kArgs) ...>
	{
		auto state = Stream::Get<std::uint8_t>(input);
		MNode* t;

		if (state & 0x20) {
			auto const& vCreateInfo = DP::Factory<V, VType, VArgs ...>::GetCreateInfo(Stream::Get<VType>(input));
			t = reinterpret_cast<MNode*>(operator new(sizeof(MNode), vCreateInfo.size));
			t->template state(2);
			try {
				::new(static_cast<void*>(t->val)) Holder<V>(vCreateInfo, input, std::make_index_sequence<sizeof...(VArgs) - sizeof...(vArgs)>{}, std::forward<decltype(vArgs)>(vArgs) ...);
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
			::new(static_cast<void*>(t->key)) Holder<K>(input, std::forward<decltype(kArgs)>(kArgs) ...);
		} catch (...) {
			if (t->d[0].hasValue)
				t->val->~V();
			operator delete(t);
			throw;
		}

		try {
			t->left(state & 0x40 ? MNode::Create(P, t, std::forward<decltype(kArgs)>(kArgs) ..., input, DP::Factory<V, VType, VArgs ...>{}, std::forward<decltype(vArgs)>(vArgs) ...) : P);
			t->right(state & 0x10 ? MNode::Create(t, S, std::forward<decltype(kArgs)>(kArgs) ..., input, DP::Factory<V, VType, VArgs ...>{}, std::forward<decltype(vArgs)>(vArgs) ...) : S);
			t->state(state);
			return t;
		} catch (...) {
			delete t;
			throw;
		}
	}

	template <typename KType, typename ... KArgs>
	static MNode*
	Create(MNode* P, MNode* S,
			DP::Factory<K, KType, KArgs ...>, auto&& ... kArgs, Stream::Input& input, auto&& ... vArgs)
	requires Stream::Deserializable<V, decltype(input), decltype(vArgs) ...>
	{
		auto state = Stream::Get<std::uint8_t>(input);
		auto* t = reinterpret_cast<MNode*>(::operator new(sizeof(MNode)));
		t->template state(2);

		try {
			if (state & 0x20) {
				::new(static_cast<void*>(t->val)) Holder<V>(input, std::forward<decltype(vArgs)>(vArgs) ...);
				t->d[0].hasValue = true;
			}
			auto const& kCreateInfo = DP::Factory<K, KType, KArgs ...>::GetCreateInfo(Stream::Get<KType>(input));
			if (kCreateInfo.size > sizeof(K))
				throw std::system_error(MException::Code::LargerKey, "kCreateInfo.size is greater than the size of K.");

			::new(static_cast<void*>(t->key)) Holder<K>(kCreateInfo, input, std::make_index_sequence<sizeof...(KArgs) - sizeof...(kArgs)>{}, std::forward<decltype(kArgs)>(kArgs) ...);
		} catch (...) {
			if (t->d[0].hasValue)
				t->val->~V();
			::operator delete(t);
			throw;
		}

		try {
			t->left(state & 0x40 ? MNode::Create(P, t, DP::Factory<K, KType, KArgs ...>{}, std::forward<decltype(kArgs)>(kArgs) ..., input, std::forward<decltype(vArgs)>(vArgs) ...) : P);
			t->right(state & 0x10 ? MNode::Create(t, S, DP::Factory<K, KType, KArgs ...>{}, std::forward<decltype(kArgs)>(kArgs) ..., input, std::forward<decltype(vArgs)>(vArgs) ...) : S);
			t->state(state);
			return t;
		} catch (...) {
			delete t;
			throw;
		}
	}

	template <typename KType, typename ... KArgs, typename VType, typename ... VArgs>
	static MNode*
	Create(MNode* P, MNode* S,
			DP::Factory<K, KType, KArgs ...>, auto&& ... kArgs, Stream::Input& input, DP::Factory<V, VType, VArgs ...>, auto&& ... vArgs)
	{
		auto state = Stream::Get<std::uint8_t>(input);
		MNode* t;

		if (state & 0x20) {
			auto const& vCreateInfo = DP::Factory<V, VType, VArgs ...>::GetCreateInfo(Stream::Get<VType>(input));
			t = reinterpret_cast<MNode*>(operator new(sizeof(MNode), vCreateInfo.size));
			t->template state(2);
			try {
				::new(static_cast<void*>(t->val)) Holder<V>(vCreateInfo, input, std::make_index_sequence<sizeof...(VArgs) - sizeof...(vArgs)>{}, std::forward<decltype(vArgs)>(vArgs) ...);
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
			auto const& kCreateInfo = DP::Factory<K, KType, KArgs ...>::GetCreateInfo(Stream::Get<KType>(input));
			if (kCreateInfo.size > sizeof(K))
				throw std::system_error(MException::Code::LargerKey, "kCreateInfo.size is greater than the size of K.");

			::new(static_cast<void*>(t->key)) Holder<K>(kCreateInfo, input, std::make_index_sequence<sizeof...(KArgs) - sizeof...(kArgs)>{}, std::forward<decltype(kArgs)>(kArgs) ...);
		} catch (...) {
			if (t->d[0].hasValue)
				t->val->~V();
			operator delete(t);
			throw;
		}

		try {
			t->left(state & 0x40 ? MNode::Create(P, t, DP::Factory<K, KType, KArgs ...>{}, std::forward<decltype(kArgs)>(kArgs) ..., input, DP::Factory<V, VType, VArgs ...>{}, std::forward<decltype(vArgs)>(vArgs) ...) : P);
			t->right(state & 0x10 ? MNode::Create(t, S, DP::Factory<K, KType, KArgs ...>{}, std::forward<decltype(kArgs)>(kArgs) ..., input, DP::Factory<V, VType, VArgs ...>{}, std::forward<decltype(vArgs)>(vArgs) ...) : S);
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

	V&
	set(auto&& ... args)
	{
		::new(static_cast<void*>(val)) Holder<V>(std::forward<decltype(args)>(args) ...);
		this->d[0].hasValue = true;
		return *val;
	}

	void
	serialize(Stream::Output& output) const
	requires Stream::InsertableTo<K, decltype(output)> && Stream::InsertableTo<V, decltype(output)>
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
	Format::DotOutput&
	toDot(Format::DotOutput& dotOutput) const
	requires Stream::InsertableTo<V, decltype(dotOutput)>
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

}//namespace DS
