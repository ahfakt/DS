#include "DS/Set.hpp"

namespace DS {

template <typename K, typename C, typename ... Cs>
Set<K, C, Cs ...>::Set(Set const& other)
requires std::is_copy_constructible_v<K>
		: Container(other.mSize)
{
	if (mSize) {
		mRoot[0] = SNode<K, C, Cs ...>::Create(nullptr, nullptr, other.mRoot[0]);
		if constexpr(sizeof...(Cs) > 0)
			SNode<K, C, Cs ...>::template BuildTree<SNode<K, C, Cs ...>, Exception>(mRoot);
	}
}

template <typename K, typename C, typename ... Cs>
Set<K, C, Cs ...>::Set(Set&& other) noexcept
{ swap(*this, other); }

template <typename K, typename C, typename ... Cs>
void
swap(Set<K, C, Cs ...>& a, Set<K, C, Cs ...>& b) noexcept
{
	std::swap(a.mSize, b.mSize);
	std::swap_ranges(a.mRoot, a.mRoot + sizeof...(Cs) + 1, b.mRoot);
}

template <typename K, typename C, typename ... Cs>
Set<K, C, Cs ...>&
Set<K, C, Cs ...>::operator=(Set value) noexcept
{
	swap(*this, value);
	return *this;
}

template <typename K, typename C, typename ... Cs>
Set<K, C, Cs ...>::Set(Stream::Input& input, auto&& ... kArgs)
requires Stream::DeserializableWith<K, decltype(input), decltype(kArgs) ...>
		: Container(Stream::Get<std::uint64_t>(input))
{
	if (mSize) {
		mRoot[0] = SNode<K, C, Cs ...>::Create(nullptr, nullptr, input, std::forward<decltype(kArgs)>(kArgs) ...);
		if constexpr(sizeof...(Cs) > 0)
			SNode<K, C, Cs ...>::template BuildTree<SNode<K, C, Cs ...>, Exception>(mRoot);
	}
}

template <typename K, typename C, typename ... Cs>
template <typename Type, typename ... Args>
Set<K, C, Cs ...>::Set(Stream::Input& input, DP::Factory<K, Type, Args ...>, auto&& ... kArgs)
		: Container(Stream::Get<std::uint64_t>(input))
{
	if (mSize) {
		mRoot[0] = SNode<K, C, Cs ...>::Create(nullptr, nullptr, input, DP::Factory<K, Type, Args ...>{}, std::forward<decltype(kArgs)>(kArgs) ...);
		if constexpr(sizeof...(Cs) > 0)
			SNode<K, C, Cs ...>::template BuildTree<SNode<K, C, Cs ...>, Exception>(mRoot);
	}
}

template <typename K, typename C, typename ... Cs>
Stream::Output&
operator<<(Stream::Output& output, Set<K, C, Cs ...> const& set)
requires Stream::InsertableTo<K, decltype(output)>
{
	output << set.mSize;
	if (set.mRoot[0])
		reinterpret_cast<SNode<K, C, Cs ...> const*>(set.mRoot[0])->serialize(output);
	return output;
}

template <typename K, typename C, typename... Cs>
template <std::size_t N>
Format::DotOutput&
Set<K, C, Cs...>::toDot(Format::DotOutput& dotOutput) const
requires Stream::InsertableTo<K, decltype(dotOutput)>
{
	mRoot[N]->template toDot<N>(dotOutput);
	reinterpret_cast<SNode<K, C, Cs ...>*>(mRoot[N])->template toDot<N>(dotOutput);
	if constexpr (N < sizeof...(Cs))
		return toDot<N + 1>(dotOutput);
	return dotOutput;
}

template <typename K, typename C, typename ... Cs>
Format::DotOutput&
operator<<(Format::DotOutput& dotOutput, Set<K, C, Cs ...> const& set)
requires Stream::InsertableTo<K, decltype(dotOutput)>
{
	dotOutput << "digraph G {\nsplines=false\nnode[shape=circle style=filled fillcolor=\"white;0.9:black\"]\n";
	if (set.mRoot[0])
		set.toDot(dotOutput);
	dotOutput << "}\n";
	return dotOutput;
}

template <typename K, typename C, typename ... Cs>
Set<K, C, Cs ...>::~Set()
{
	if (mRoot[0])
		mRoot[0]->template deleteTree<SNode<K, C, Cs ...>>();
}

template <typename K, typename C, typename ... Cs>
template <std::size_t N>
SNode<K, C, Cs ...>*
Set<K, C, Cs ...>::putAsRoot(SNode<K, C, Cs ...>* created) noexcept
{
	mRoot[N] = created;
	created->template left<N>(nullptr);
	created->template right<N>(nullptr);
	created->template state<N>(2);
	if constexpr (N < sizeof...(Cs))
		return putAsRoot<N + 1>(created);
	++mSize;
	return created;
}

template <typename K, typename C, typename ... Cs>
template <std::size_t N>
SNode<K, C, Cs ...>*
Set<K, C, Cs ...>::putToRoot(SNode<K, C, Cs ...>* created) noexcept
{
	auto* s = created;
	if (auto* t = reinterpret_cast<SNode<K, C, Cs ...>*>(mRoot[N])->template attach<N>(reinterpret_cast<TNode<sizeof...(Cs) + 1>**>(&created)))
		mRoot[N] = t;
	else if (s != created) { // found an existing node
		delete s;
		return created;
	}
	if constexpr (N < sizeof...(Cs)) {
		created = putToRoot<N + 1>(created);
		if (s != created) { // later comparators found an existing node
			// rollback last attachment
			if (auto* t = reinterpret_cast<SNode<K, C, Cs ...>*>(mRoot[N])->template detach<N>(reinterpret_cast<TNode<sizeof...(Cs) + 1>**>(&s)))
				mRoot[N] = t;
		}
		return created;
	}
	// last comparator is successful
	++mSize;
	return created;
}

template <typename K, typename C, typename ... Cs>
SNode<K, C, Cs ...>*
Set<K, C, Cs ...>::put(SNode<K, C, Cs ...>* created) noexcept
{ return mRoot[0] ? putToRoot(created) : putAsRoot(created); }


template <typename K, typename C, typename ... Cs>
typename Set<K, C, Cs ...>::template const_iterator<>
Set<K, C, Cs ...>::put(auto&& ... kArgs)
{ return put(::new SNode<K, C, Cs ...>(std::forward<decltype(kArgs)>(kArgs) ...)); }

template <typename K, typename C, typename ... Cs>
template <Derived<K> DK>
typename Set<K, C, Cs ...>::template const_iterator<>
Set<K, C, Cs ...>::put(auto&& ... dkArgs)
{ return put(reinterpret_cast<SNode<K, C, Cs ...>*>(::new SNode<DK>(std::forward<decltype(dkArgs)>(dkArgs) ...))); }

template <typename K, typename C, typename ... Cs>
template <typename ... Args>
typename Set<K, C, Cs ...>::template const_iterator<>
Set<K, C, Cs ...>::put(DP::CreateInfo<K, Args ...> const& createInfo, auto&& ... args)
{ return put(new(createInfo.size) SNode<K, C, Cs ...>(createInfo, std::forward<decltype(args)>(args) ...)); }


template <typename K, typename C, typename ... Cs>
template <std::size_t N>
SNode<K, C, Cs ...>*
Set<K, C, Cs ...>::remove(SNode<K, C, Cs ...>* toDel) noexcept
{
	auto* s = toDel;
	if (auto* t = reinterpret_cast<SNode<K, C, Cs ...>*>(mRoot[N])->template detach<N>(reinterpret_cast<TNode<sizeof...(Cs) + 1>**>(&toDel)))
		mRoot[N] = t;
	else if (s != toDel)
		return toDel;
	if constexpr (N < sizeof...(Cs)) {
		toDel = remove<N + 1>(toDel);
		if (s != toDel) {
			// later comparators found another one or could not find it at all
			// if it is root it means nothing happened already
			if (s != mRoot[N]) {
				// rollback last detachment
				if (auto* t = reinterpret_cast<SNode<K, C, Cs ...>*>(mRoot[N])->template attach<N>(reinterpret_cast<TNode<sizeof...(Cs) + 1>**>(&s)))
					mRoot[N] = t;
			}
		} else if (s == mRoot[N])
			mRoot[N] = nullptr;
		return toDel;
	}
	// last comparator is successful
	if (s == mRoot[N])
		mRoot[N] = nullptr;
	delete s;
	--mSize;
	return toDel;
}

template <typename K, typename C, typename... Cs>
template <Direction d, std::size_t N>
bool
Set<K, C, Cs...>::remove(Iterator<d, N> i) noexcept
{ return mRoot[N] && i.pos && i.pos == remove(reinterpret_cast<SNode<K, C, Cs ...>*>(i.pos)); }

template <typename K, typename C, typename ... Cs>
template <std::size_t N>
bool
Set<K, C, Cs ...>::remove(auto&& ... args) noexcept
{
	if (mRoot[N]) {
		if (auto* t = reinterpret_cast<SNode<K, C, Cs ...>*>(mRoot[N])->template get<N>(std::forward<decltype(args)>(args) ...))
			return t == remove(reinterpret_cast<SNode<K, C, Cs ...>*>(t));
	}
	return false;
}

template <typename K, typename C, typename ... Cs>
template <std::size_t N>
typename Set<K, C, Cs ...>::template const_iterator<N>
Set<K, C, Cs ...>::get(auto&& ... args) const noexcept
{ return mRoot[N] ? reinterpret_cast<SNode<K, C, Cs ...>*>(mRoot[N])->template get<N>(std::forward<decltype(args)>(args) ...) : nullptr; }

template <typename K, typename C, typename ... Cs>
template <std::size_t N>
typename Set<K, C, Cs ...>::template const_iterator<N>
Set<K, C, Cs ...>::at(std::uint64_t i) const noexcept
{ // TODO: O(n)->O(logn) childCount?
	const_iterator<N> it(nullptr);
	if (mRoot[N]) {
		if (i < mSize / 2) {
			it.pos = mRoot[N]->template leftMost<N>();
			for (std::uint64_t j{0}; j < i; ++j)
				++it;
		} else {
			it.pos = mRoot[N]->template rightMost<N>();
			for (std::uint64_t j = mSize - 1; i < j; --j)
				--it;
		}
	}
	return it;
}

template <typename K, typename C, typename ... Cs>
template <std::size_t N>
typename Set<K, C, Cs ...>::template const_iterator<N>
Set<K, C, Cs ...>::begin() const noexcept
{ return mRoot[N] ? mRoot[N]->template leftMost<N>() : nullptr; }

template <typename K, typename C, typename ... Cs>
template <std::size_t N>
typename Set<K, C, Cs ...>::template const_iterator<N>
Set<K, C, Cs ...>::end() const noexcept
{ return nullptr; }

template <typename K, typename C, typename ... Cs>
template <std::size_t N>
typename Set<K, C, Cs ...>::template const_reverse_iterator<N>
Set<K, C, Cs ...>::rbegin() const noexcept
{ return mRoot[N] ? mRoot[N]->template rightMost<N>() : nullptr; }

template <typename K, typename C, typename ... Cs>
template <std::size_t N>
typename Set<K, C, Cs ...>::template const_reverse_iterator<N>
Set<K, C, Cs ...>::rend() const noexcept
{ return nullptr; }

template <typename K, typename C, typename ... Cs>
template <std::size_t N>
typename Set<K, C, Cs ...>::template const_iterator<N>
Set<K, C, Cs ...>::cbegin() const noexcept
{ return begin<N>(); }

template <typename K, typename C, typename ... Cs>
template <std::size_t N>
typename Set<K, C, Cs ...>::template const_iterator<N>
Set<K, C, Cs ...>::cend() const noexcept
{ return end<N>(); }

template <typename K, typename C, typename ... Cs>
template <std::size_t N>
typename Set<K, C, Cs ...>::template const_reverse_iterator<N>
Set<K, C, Cs ...>::crbegin() const noexcept
{ return rbegin<N>(); }

template <typename K, typename C, typename ... Cs>
template <std::size_t N>
typename Set<K, C, Cs ...>::template const_reverse_iterator<N>
Set<K, C, Cs ...>::crend() const noexcept
{ return rend<N>(); }

template <typename K, typename C, typename ... Cs>
template <Direction d, std::size_t n>
Set<K, C, Cs ...>::Iterator<d, n>::Iterator(TNode<sizeof...(Cs) + 1>* pos) noexcept
		: pos(pos)
{}

template <typename K, typename C, typename ... Cs>
template <Direction d, std::size_t n>
template <Direction od, std::size_t on>
Set<K, C, Cs ...>::Iterator<d, n>::Iterator(Iterator<od, on> const& other) noexcept
		: pos(other.pos)
{}

template <typename K, typename C, typename ... Cs>
template <Direction d, std::size_t n>
template <Direction od, std::size_t on>
class Set<K, C, Cs ...>::Iterator<d, n>&
Set<K, C, Cs ...>::Iterator<d, n>::operator=(Iterator<od, on> const& other) noexcept
{
	pos = other.pos;
	return *this;
}

template <typename K, typename C, typename ... Cs>
template <Direction d, std::size_t n>
class Set<K, C, Cs ...>::Iterator<d, n>&
Set<K, C, Cs ...>::Iterator<d, n>::operator++() noexcept
{
	pos = (d == Direction::FORWARD ? pos->template next<n>() : pos->template prev<n>());
	return *this;
}

template <typename K, typename C, typename ... Cs>
template <Direction d, std::size_t n>
class Set<K, C, Cs ...>::Iterator<d, n>
Set<K, C, Cs ...>::Iterator<d, n>::operator++(int) noexcept
{
	auto* r = pos;
	pos = (d == Direction::FORWARD ? pos->template next<n>() : pos->template prev<n>());
	return r;
}

template <typename K, typename C, typename ... Cs>
template <Direction d, std::size_t n>
class Set<K, C, Cs ...>::Iterator<d, n>&
Set<K, C, Cs ...>::Iterator<d, n>::operator--() noexcept
{
	pos = (d == Direction::FORWARD ? pos->template prev<n>() : pos->template next<n>());
	return *this;
}

template <typename K, typename C, typename ... Cs>
template <Direction d, std::size_t n>
class Set<K, C, Cs ...>::Iterator<d, n>
Set<K, C, Cs ...>::Iterator<d, n>::operator--(int) noexcept
{
	auto* r = pos;
	pos = (d == Direction::FORWARD ? pos->template prev<n>() : pos->template next<n>());
	return r;
}

template <typename K, typename C, typename ... Cs>
template <Direction d, std::size_t n>
K const&
Set<K, C, Cs ...>::Iterator<d, n>::operator*() const noexcept
{ return reinterpret_cast<K const&>(reinterpret_cast<SNode<K, C, Cs ...>*>(pos)->key); }

template <typename K, typename C, typename ... Cs>
template <Direction d, std::size_t n>
K const*
Set<K, C, Cs ...>::Iterator<d, n>::operator->() const noexcept
{ return reinterpret_cast<K const*>(&reinterpret_cast<SNode<K, C, Cs ...>*>(pos)->key); }

template <typename K, typename C, typename ... Cs>
template <Direction d, std::size_t n>
template <Direction od, std::size_t on>
bool
Set<K, C, Cs ...>::Iterator<d, n>::operator==(Iterator<od, on> const& other) const noexcept
{ return pos == other.pos; }

template <typename K, typename C, typename ... Cs>
template <Direction d, std::size_t n>
Set<K, C, Cs ...>::Iterator<d, n>::operator bool() const noexcept
{ return pos; }

template <typename K, typename C, typename... Cs>
template <std::size_t N>
Set<K, C, Cs...>
Set<K, C, Cs...>::Difference<N>::operator()(Set const& a, Set const& b) const
{
	Set set;
	if (a) {
		if (b) {
			DP::Type<N, C, Cs ...> cmp;
			auto* i = a.mRoot[N]->template leftMost<N, SNode<K, C, Cs...>>();
			auto* j = b.mRoot[N]->template leftMost<N, SNode<K, C, Cs...>>();

			do {
				if (cmp(static_cast<K const&>(i->key), static_cast<K const&>(j->key))) {
					set.put(static_cast<K const&>(i->key));
					i = i->template next<N, SNode<K, C, Cs...>>();
					continue;
				}
				if (cmp(static_cast<K const&>(j->key), static_cast<K const&>(i->key))) {
					j = j->template next<N, SNode<K, C, Cs...>>();
					continue;
				}
				i = i->template next<N, SNode<K, C, Cs...>>();
				j = j->template next<N, SNode<K, C, Cs...>>();
			} while (i && j);

			while (i) {
				set.put(static_cast<K const&>(i->key));
				i = i->template next<N, SNode<K, C, Cs...>>();
			}
		} else
			set = a;
	}
	return set;
}

template <typename K, typename C, typename... Cs>
template <typename S, std::size_t N>
Set<K, C, Cs...>
Set<K, C, Cs...>::Intersection<S, N>::operator()(Set const& a, Set const& b, auto&& ... args) const
requires Selector<S, K, decltype(args) ...>
{
	Set set;
	if (a && b) {
		DP::Type<N, C, Cs ...> cmp;
		S selector;
		auto* i = a.mRoot[N]->template leftMost<N, SNode<K, C, Cs...>>();
		auto* j = b.mRoot[N]->template leftMost<N, SNode<K, C, Cs...>>();

		do {
			if (cmp(static_cast<K const&>(i->key), static_cast<K const&>(j->key))) {
				i = i->template next<N, SNode<K, C, Cs...>>();
				continue;
			}
			if (cmp(static_cast<K const&>(j->key), static_cast<K const&>(i->key))) {
				j = j->template next<N, SNode<K, C, Cs...>>();
				continue;
			}
			set.put(selector(static_cast<K const&>(i->key), static_cast<K const&>(j->key), std::forward<decltype(args)>(args) ...));
			i = i->template next<N, SNode<K, C, Cs...>>();
			j = j->template next<N, SNode<K, C, Cs...>>();
		} while (i && j);
	}
	return set;
}

template <typename K, typename C, typename... Cs>
template <typename S, std::size_t N>
Set<K, C, Cs...>
Set<K, C, Cs...>::LeftJoin<S, N>::operator()(Set const& a, Set const& b, auto&& ... args) const
requires Selector<S, K, decltype(args) ...>
{
	Set set;
	if (a) {
		if (b) {
			DP::Type<N, C, Cs ...> cmp;
			S selector;
			auto* i = a.mRoot[N]->template leftMost<N, SNode < K, C, Cs...>>();
			auto* j = b.mRoot[N]->template leftMost<N, SNode < K, C, Cs...>>();

			do {
				if (cmp(static_cast<K const&>(i->key), static_cast<K const&>(j->key))) {
					set.put(static_cast<K const&>(i->key));
					i = i->template next<N, SNode < K, C, Cs...>>();
					continue;
				}
				if (cmp(static_cast<K const&>(j->key), static_cast<K const&>(i->key))) {
					j = j->template next<N, SNode < K, C, Cs...>>();
					continue;
				}
				set.put(selector(static_cast<K const&>(i->key), static_cast<K const&>(j->key), std::forward<decltype(args)>(args) ...));
				i = i->template next<N, SNode < K, C, Cs...>>();
				j = j->template next<N, SNode < K, C, Cs...>>();
			} while (i && j);

			while (i) {
				set.put(static_cast<K const&>(i->key));
				i = i->template next<N, SNode < K, C, Cs...>>();
			}
		} else
			set = a;
	}
	return set;
}

template <typename K, typename C, typename... Cs>
template <typename S, std::size_t N>
Set<K, C, Cs...>
Set<K, C, Cs...>::Union<S, N>::operator()(Set const& a, Set const& b, auto&& ... args) const
requires Selector<S, K, decltype(args) ...>
{
	Set set;
	if (a) {
		if (b) {
			DP::Type<N, C, Cs ...> cmp;
			S selector;
			auto* i = a.mRoot[N]->template leftMost<N, SNode < K, C, Cs...>>();
			auto* j = b.mRoot[N]->template leftMost<N, SNode < K, C, Cs...>>();

			do {
				if (cmp(static_cast<K const&>(i->key), static_cast<K const&>(j->key))) {
					set.put(static_cast<K const&>(i->key));
					i = i->template next<N, SNode < K, C, Cs...>>();
					continue;
				}
				if (cmp(static_cast<K const&>(j->key), static_cast<K const&>(i->key))) {
					set.put(static_cast<K const&>(j->key));
					j = j->template next<N, SNode < K, C, Cs...>>();
					continue;
				}
				set.put(selector(static_cast<K const&>(i->key), static_cast<K const&>(j->key), std::forward<decltype(args)>(args) ...));
				i = i->template next<N, SNode < K, C, Cs...>>();
				j = j->template next<N, SNode < K, C, Cs...>>();
			} while (i && j);

			while (i) {
				set.put(static_cast<K const&>(i->key));
				i = i->template next<N, SNode < K, C, Cs...>>();
			}

			while (j) {
				set.put(static_cast<K const&>(j->key));
				j = j->template next<N, SNode < K, C, Cs...>>();
			}
		} else
			set = a;
	} else if (b)
		set = b;
	return set;
}

}//namespace DS
