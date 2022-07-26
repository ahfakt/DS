#include "DS/Map.h"

namespace DS {

template <typename K, typename V, typename C, typename ... Cs>
Map<K, V, C, Cs ...>::Map(Map const& other)
requires std::is_copy_constructible_v<K> && std::is_copy_constructible_v<V>
		: Container(other.mSize)
{
	if (mSize) {
		mRoot[0] = MNode<K, V, C, Cs ...>::Create(nullptr, nullptr, other.mRoot[0]);
		if constexpr(sizeof...(Cs) > 0)
			SNode<K, C, Cs ...>::template BuildTree<MNode<K, V, C, Cs ...>, Exception>(mRoot);
	}
}

template <typename K, typename V, typename C, typename ... Cs>
Map<K, V, C, Cs ...>::Map(Map&& other) noexcept
{ swap(*this, other); }

template <typename K, typename V, typename C, typename ... Cs>
void
swap(Map<K, V, C, Cs ...>& a, Map<K, V, C, Cs ...>& b) noexcept
{
	std::swap(a.mSize, b.mSize);
	std::swap_ranges(a.mRoot, a.mRoot + sizeof...(Cs) + 1, b.mRoot);
}

template <typename K, typename V, typename C, typename ... Cs>
Map<K, V, C, Cs ...>&
Map<K, V, C, Cs ...>::operator=(Map value) noexcept
{
	swap(*this, value);
	return *this;
}

template <typename K, typename V, typename C, typename ... Cs>
Map<K, V, C, Cs ...>::Map(auto&& ... kArgs, Stream::Input& input, auto&& ... vArgs)
requires Stream::DeserializableWith<K, Stream::Input, decltype(kArgs) ...> && Stream::DeserializableWith<V, Stream::Input, decltype(vArgs) ...>
		: Container(Stream::Get<std::uint64_t>(input))
{
	if (mSize) {
		mRoot[0] = MNode<K, V, C, Cs ...>::Create(nullptr, nullptr, std::forward<decltype(kArgs)>(kArgs) ..., input, std::forward<decltype(vArgs)>(vArgs) ...);
		if constexpr(sizeof...(Cs) > 0)
			SNode<K, C, Cs ...>::template BuildTree<MNode<K, V, C, Cs ...>, Exception>(mRoot);
	}
}

template <typename K, typename V, typename C, typename ... Cs>
template <typename VIDType, typename ... VFArgs>
Map<K, V, C, Cs ...>::Map(auto&& ... kArgs, Stream::Input& input, DP::Factory<V, VIDType, VFArgs ...> const& vFactory)
requires Stream::DeserializableWith<K, Stream::Input, decltype(kArgs) ...>
		: Container(Stream::Get<std::uint64_t>(input))
{
	if (mSize) {
		mRoot[0] = MNode<K, V, C, Cs ...>::Create(nullptr, nullptr, std::forward<decltype(kArgs)>(kArgs) ..., input, vFactory);
		if constexpr(sizeof...(Cs) > 0)
			SNode<K, C, Cs ...>::template BuildTree<MNode<K, V, C, Cs ...>, Exception>(mRoot);
	}
}

template <typename K, typename V, typename C, typename ... Cs>
template <typename KIDType, typename ... KFArgs>
Map<K, V, C, Cs ...>::Map(DP::Factory<K, KIDType, KFArgs ...> const& kFactory, Stream::Input& input, auto&& ... vArgs)
requires Stream::DeserializableWith<V, Stream::Input, decltype(vArgs) ...>
		: Container(Stream::Get<std::uint64_t>(input))
{
	if (mSize) {
		mRoot[0] = MNode<K, V, C, Cs ...>::Create(nullptr, nullptr, kFactory, input, std::forward<decltype(vArgs)>(vArgs) ...);
		if constexpr(sizeof...(Cs) > 0)
			SNode<K, C, Cs ...>::template BuildTree<MNode<K, V, C, Cs ...>, Exception>(mRoot);
	}
}

template <typename K, typename V, typename C, typename ... Cs>
template <typename KIDType, typename ... KFArgs, typename VIDType, typename ... VFArgs>
Map<K, V, C, Cs ...>::Map(DP::Factory<K, KIDType, KFArgs ...> const& kFactory, Stream::Input& input, DP::Factory<V, VIDType, VFArgs ...> const& vFactory)
		: Container(Stream::Get<std::uint64_t>(input))
{
	if (mSize) {
		mRoot[0] = MNode<K, V, C, Cs ...>::Create(nullptr, nullptr, kFactory, input, vFactory);
		if constexpr(sizeof...(Cs) > 0)
			SNode<K, C, Cs ...>::template BuildTree<MNode<K, V, C, Cs ...>, Exception>(mRoot);
	}
}

template <typename K, typename V, typename C, typename ... Cs>
Stream::Output&
operator<<(Stream::Output& output, Map<K, V, C, Cs ...> const& map)
requires Stream::InsertableTo<K, Stream::Output> && Stream::InsertableTo<V, Stream::Output>
{
	output << map.mSize;
	if (map.mRoot[0])
		reinterpret_cast<MNode<K, V, C, Cs ...> const*>(map.mRoot[0])->serialize(output);
	return output;
}

template <typename K, typename V, typename C, typename ... Cs>
template <std::size_t N>
Stream::Format::DotOutput&
Map<K, V, C, Cs ...>::toDot(Stream::Format::DotOutput& dotOutput) const
requires Stream::InsertableTo<K, Stream::Format::StringOutput> && Stream::InsertableTo<V, Stream::Format::StringOutput>
{
	mRoot[N]->template toDot<N>(dotOutput);
	reinterpret_cast<SNode<K, C, Cs ...>*>(mRoot[N])->template toDot<N>(dotOutput);
	reinterpret_cast<MNode<K, V, C, Cs ...>*>(mRoot[N])->template toDot<N>(dotOutput);
	if constexpr (N < sizeof...(Cs))
		return toDot<N + 1>(dotOutput);
	return dotOutput;
}

template <typename K, typename V, typename C, typename ... Cs>
Stream::Format::DotOutput&
operator<<(Stream::Format::DotOutput& dotOutput, Map<K, V, C, Cs ...> const& map)
requires Stream::InsertableTo<K, Stream::Format::StringOutput> && Stream::InsertableTo<V, Stream::Format::StringOutput>
{
	dotOutput << "digraph G {\nsplines=false\nnode[shape=circle style=filled fillcolor=\"white;0.9:black\"]\n";
	if (map.mRoot[0])
		map.toDot(dotOutput);
	dotOutput << "}\n";
	return dotOutput;
}

template <typename K, typename V, typename C, typename ... Cs>
Map<K, V, C, Cs ...>::~Map()
{
	if (mRoot[0])
		mRoot[0]->template deleteTree<MNode<K, V, C, Cs ...>>();
}

template <typename K, typename V, typename C, typename ... Cs>
template <std::size_t N>
MNode<K, V, C, Cs ...>*
Map<K, V, C, Cs ...>::putAsRoot(MNode<K, V, C, Cs ...>* created) noexcept
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

template <typename K, typename V, typename C, typename ... Cs>
template <std::size_t N>
MNode<K, V, C, Cs ...>*
Map<K, V, C, Cs ...>::putToRoot(MNode<K, V, C, Cs ...>* created) noexcept
{
	auto* m = created;
	if (auto* t = reinterpret_cast<SNode<K, C, Cs ...>*>(mRoot[N])->template attach<N>(reinterpret_cast<TNode<sizeof...(Cs) + 1>**>(&created)))
		mRoot[N] = t;
	else if (m != created) { // found an existing node
		delete m;
		return created;
	}
	if constexpr (N < sizeof...(Cs)) {
		created = putToRoot<N + 1>(created);
		if (m != created) { // later comparators found an existing node
			// rollback last attachment
			if (auto* t = reinterpret_cast<SNode<K, C, Cs ...>*>(mRoot[N])->template detach<N>(reinterpret_cast<TNode<sizeof...(Cs) + 1>**>(&m)))
				mRoot[N] = t;
		}
		return created;
	}
	// last comparator is successful
	++mSize;
	return created;
}

template <typename K, typename V, typename C, typename ... Cs>
MNode<K, V, C, Cs ...>*
Map<K, V, C, Cs ...>::put(MNode<K, V, C, Cs ...>* created) noexcept
{ return mRoot[0] ? putToRoot(created) : putAsRoot(created); }


template <typename K, typename V, typename C, typename ... Cs>
typename Map<K, V, C, Cs ...>::template iterator<>
Map<K, V, C, Cs ...>::put(auto&& ... kArgs)
{ return put(::new MNode<K, V, C, Cs ...>(std::forward<decltype(kArgs)>(kArgs) ...)); }

template <typename K, typename V, typename C, typename ... Cs>
template <Derived<V> DV>
typename Map<K, V, C, Cs ...>::template iterator<>
Map<K, V, C, Cs ...>::put(auto&& ... kArgs)
{ return put(reinterpret_cast<MNode<K, V, C, Cs ...>*>(::new MNode<K, DV>(std::forward<decltype(kArgs)>(kArgs) ...))); }

template <typename K, typename V, typename C, typename ... Cs>
template <typename ... VCIArgs>
typename Map<K, V, C, Cs ...>::template iterator<>
Map<K, V, C, Cs ...>::put(auto&& ... kArgs, DP::CreateInfo<V, VCIArgs ...> const& vCreateInfo)
{ return put(new(vCreateInfo.size) MNode<K, V, C, Cs ...>(std::forward<decltype(kArgs)>(kArgs) ...)); }


template <typename K, typename V, typename C, typename ... Cs>
template <EqDerived<K> DK>
typename Map<K, V, C, Cs ...>::template iterator<>
Map<K, V, C, Cs ...>::put(auto&& ... dkArgs)
{ return put(reinterpret_cast<MNode<K, V, C, Cs ...>*>(::new MNode<DK, V>(std::forward<decltype(dkArgs)>(dkArgs) ...))); }

template <typename K, typename V, typename C, typename ... Cs>
template <EqDerived<K> DK, Derived<V> DV>
typename Map<K, V, C, Cs ...>::template iterator<>
Map<K, V, C, Cs ...>::put(auto&& ... dkArgs)
{ return put(reinterpret_cast<MNode<K, V, C, Cs ...>*>(::new MNode<DK, DV>(std::forward<decltype(dkArgs)>(dkArgs) ...))); }

template <typename K, typename V, typename C, typename ... Cs>
template <EqDerived<K> DK, typename ... VCIArgs>
typename Map<K, V, C, Cs ...>::template iterator<>
Map<K, V, C, Cs ...>::put(auto&& ... dkArgs, DP::CreateInfo<V, VCIArgs ...> const& vCreateInfo)
{ return put(reinterpret_cast<MNode<K, V, C, Cs ...>*>(new(vCreateInfo.size) MNode<DK, V>(std::forward<decltype(dkArgs)>(dkArgs) ...))); }


template <typename K, typename V, typename C, typename ... Cs>
template <typename ... KCIArgs>
typename Map<K, V, C, Cs ...>::template iterator<>
Map<K, V, C, Cs ...>::put(DP::CreateInfo<K, KCIArgs ...> const& kCreateInfo, auto&& ... kcArgs)
{
	if (kCreateInfo.size > sizeof(K))
		throw Exception(MException::Code::LargerKey, "kCreateInfo.size is greater than the size of K.");
	return put(::new MNode<K, V, C, Cs ...>(kCreateInfo.create, std::forward<decltype(kcArgs)>(kcArgs) ...));
}

template <typename K, typename V, typename C, typename ... Cs>
template <typename ... KCIArgs, Derived<V> DV>
typename Map<K, V, C, Cs ...>::template iterator<>
Map<K, V, C, Cs ...>::put(DP::CreateInfo<K, KCIArgs ...> const& kCreateInfo, auto&& ... kcArgs)
{
	if (kCreateInfo.size > sizeof(K))
		throw Exception(MException::Code::LargerKey, "kCreateInfo.size is greater than the size of K.");
	return put(reinterpret_cast<MNode<K, V, C, Cs ...>*>(::new MNode<K, DV>(kCreateInfo.create, std::forward<decltype(kcArgs)>(kcArgs) ...)));
}

template <typename K, typename V, typename C, typename ... Cs>
template <typename ... KCIArgs, typename ... VCIArgs>
typename Map<K, V, C, Cs ...>::template iterator<>
Map<K, V, C, Cs ...>::put(DP::CreateInfo<K, KCIArgs ...> const& kCreateInfo, auto&& ... kcArgs, DP::CreateInfo<V, VCIArgs ...> const& vCreateInfo)
{
	if (kCreateInfo.size > sizeof(K))
		throw Exception(MException::Code::LargerKey, "kCreateInfo.size is greater than the size of K.");
	return put(reinterpret_cast<MNode<K, V, C, Cs ...>*>(new(vCreateInfo.size) MNode<K, V, C, Cs ...>(kCreateInfo.create, std::forward<decltype(kcArgs)>(kcArgs) ...)));
}


template <typename K, typename V, typename C, typename ... Cs>
template <std::size_t N>
MNode<K, V, C, Cs ...>*
Map<K, V, C, Cs ...>::remove(MNode<K, V, C, Cs ...>* toDel) noexcept
{
	auto* m = toDel;
	if (auto* t = reinterpret_cast<SNode<K, C, Cs ...>*>(mRoot[N])->template detach<N>(reinterpret_cast<TNode<sizeof...(Cs) + 1>**>(&toDel)))
		mRoot[N] = t;
	else if (m != toDel)
		return toDel;
	if constexpr (N < sizeof...(Cs)) {
		toDel = remove<N + 1>(toDel);
		if (m != toDel) {
			// later comparators found another one or could not find it at all
			// if it is root it means nothing happened already
			if (m != mRoot[N]) {
				// rollback last detachment
				if (auto* t = reinterpret_cast<SNode<K, C, Cs ...>*>(mRoot[N])->template attach<N>(reinterpret_cast<TNode<sizeof...(Cs) + 1>**>(&m)))
					mRoot[N] = t;
			}
		} else if (m == mRoot[N])
			mRoot[N] = nullptr;
		return toDel;
	}
	// last comparator is successful
	if (m == mRoot[N])
		mRoot[N] = nullptr;
	delete m;
	--mSize;
	return toDel;
}

template <typename K, typename V, typename C, typename ... Cs>
template <std::size_t N>
bool
Map<K, V, C, Cs ...>::remove(auto&& ... args) noexcept
{
	if (mRoot[N]) {
		if (auto* t = reinterpret_cast<SNode<K, C, Cs ...>*>(mRoot[N])->template get<N>(std::forward<decltype(args)>(args) ...))
			return t == remove(reinterpret_cast<MNode<K, V, C, Cs ...>*>(t));
	}
	return false;
}

template <typename K, typename V, typename C, typename ... Cs>
template <std::size_t N>
typename Map<K, V, C, Cs ...>::template iterator<N>
Map<K, V, C, Cs ...>::get(auto&& ... args) noexcept
{ return mRoot[N] ? reinterpret_cast<SNode<K, C, Cs ...>*>(mRoot[N])->template get<N>(std::forward<decltype(args)>(args) ...) : nullptr; }

template <typename K, typename V, typename C, typename ... Cs>
template <std::size_t N>
typename Map<K, V, C, Cs ...>::template const_iterator<N>
Map<K, V, C, Cs ...>::get(auto&& ... args) const noexcept
{ return const_cast<Map*>(this)->template get<N>(std::forward<decltype(args)>(args) ...); }

template <typename K, typename V, typename C, typename ... Cs>
template <std::size_t N>
typename Map<K, V, C, Cs ...>::template iterator<N>
Map<K, V, C, Cs ...>::at(std::uint64_t i) noexcept
{ // TODO: O(n)->O(logn) childCount?
	iterator<N> it(nullptr);
	if (mRoot[N]) {
		if (i < mSize / 2) {
			it.pos = mRoot[N]->template leftMost<N>();
			for (std::uint64_t j = 0; j < i; ++j)
				++it;
		} else {
			it.pos = mRoot[N]->template rightMost<N>();
			for (std::uint64_t j = mSize - 1; i < j; --j)
				--it;
		}
	}
	return it;
}

template <typename K, typename V, typename C, typename ... Cs>
template <std::size_t N>
typename Map<K, V, C, Cs ...>::template const_iterator<N>
Map<K, V, C, Cs ...>::at(std::uint64_t i) const noexcept
{ return const_cast<Map*>(this)->at<N>(i); }

template <typename K, typename V, typename C, typename ... Cs>
template <std::size_t N>
typename Map<K, V, C, Cs ...>::template iterator<N>
Map<K, V, C, Cs ...>::begin() noexcept
{ return mRoot[N] ? mRoot[N]->template leftMost<N>() : nullptr; }

template <typename K, typename V, typename C, typename ... Cs>
template <std::size_t N>
typename Map<K, V, C, Cs ...>::template const_iterator<N>
Map<K, V, C, Cs ...>::begin() const noexcept
{ return const_cast<Map*>(this)->begin<N>(); }

template <typename K, typename V, typename C, typename ... Cs>
template <std::size_t N>
typename Map<K, V, C, Cs ...>::template iterator<N>
Map<K, V, C, Cs ...>::end() noexcept
{ return nullptr; }

template <typename K, typename V, typename C, typename ... Cs>
template <std::size_t N>
typename Map<K, V, C, Cs ...>::template const_iterator<N>
Map<K, V, C, Cs ...>::end() const noexcept
{ return const_cast<Map*>(this)->end<N>(); }

template <typename K, typename V, typename C, typename ... Cs>
template <std::size_t N>
typename Map<K, V, C, Cs ...>::template reverse_iterator<N>
Map<K, V, C, Cs ...>::rbegin() noexcept
{ return mRoot[N] ? mRoot[N]->template rightMost<N>() : nullptr; }

template <typename K, typename V, typename C, typename ... Cs>
template <std::size_t N>
typename Map<K, V, C, Cs ...>::template const_reverse_iterator<N>
Map<K, V, C, Cs ...>::rbegin() const noexcept
{ return const_cast<Map*>(this)->rbegin<N>(); }

template <typename K, typename V, typename C, typename ... Cs>
template <std::size_t N>
typename Map<K, V, C, Cs ...>::template reverse_iterator<N>
Map<K, V, C, Cs ...>::rend() noexcept
{ return nullptr; }

template <typename K, typename V, typename C, typename ... Cs>
template <std::size_t N>
typename Map<K, V, C, Cs ...>::template const_reverse_iterator<N>
Map<K, V, C, Cs ...>::rend() const noexcept
{ return const_cast<Map*>(this)->rend<N>(); }

template <typename K, typename V, typename C, typename ... Cs>
template <std::size_t N>
typename Map<K, V, C, Cs ...>::template const_iterator<N>
Map<K, V, C, Cs ...>::cbegin() const noexcept
{ return begin<N>(); }

template <typename K, typename V, typename C, typename ... Cs>
template <std::size_t N>
typename Map<K, V, C, Cs ...>::template const_iterator<N>
Map<K, V, C, Cs ...>::cend() const noexcept
{ return end<N>(); }

template <typename K, typename V, typename C, typename ... Cs>
template <std::size_t N>
typename Map<K, V, C, Cs ...>::template const_reverse_iterator<N>
Map<K, V, C, Cs ...>::crbegin() const noexcept
{ return rbegin<N>(); }

template <typename K, typename V, typename C, typename ... Cs>
template <std::size_t N>
typename Map<K, V, C, Cs ...>::template const_reverse_iterator<N>
Map<K, V, C, Cs ...>::crend() const noexcept
{ return rend<N>(); }

template <typename K, typename V, typename C, typename ... Cs>
template <Direction d, Constness c, std::size_t n>
Map<K, V, C, Cs ...>::Iterator<d, c, n>::Iterator(TNode<sizeof...(Cs) + 1>* pos) noexcept
		: pos(pos)
{}

template <typename K, typename V, typename C, typename ... Cs>
template <Direction d, Constness c, std::size_t n>
template <Direction od, Constness oc, std::size_t on>
Map<K, V, C, Cs ...>::Iterator<d, c, n>::Iterator(Iterator<od, oc, on> const& other) noexcept
requires ConstCompat<c, oc>
		: pos(other.pos)
{}

template <typename K, typename V, typename C, typename ... Cs>
template <Direction d, Constness c, std::size_t n>
template <Direction od, Constness oc, std::size_t on>
class Map<K, V, C, Cs ...>::Iterator<d, c, n>&
Map<K, V, C, Cs ...>::Iterator<d, c, n>::operator=(Iterator<od, oc, on> const& other) noexcept
requires ConstCompat<c, oc>
{
	pos = other.pos;
	return *this;
}

template <typename K, typename V, typename C, typename ... Cs>
template <Direction d, Constness c, std::size_t n>
bool
Map<K, V, C, Cs ...>::Iterator<d, c, n>::hasValue() const noexcept
{ return reinterpret_cast<MNode<K, V, C, Cs ...>*>(pos)->d[0].hasValue; }

template <typename K, typename V, typename C, typename ... Cs>
template <Direction d, Constness c, std::size_t n>
void
Map<K, V, C, Cs ...>::Iterator<d, c, n>::unset() const noexcept
requires (c == Constness::NCONST)
{ reinterpret_cast<MNode<K, V, C, Cs ...>*>(pos)->unset(); }

template <typename K, typename V, typename C, typename ... Cs>
template <Direction d, Constness c, std::size_t n>
V&
Map<K, V, C, Cs ...>::Iterator<d, c, n>::set(auto&& ... vArgs) const
requires (c == Constness::NCONST)
{ return reinterpret_cast<MNode<K, V, C, Cs ...>*>(pos)->set(std::forward<decltype(vArgs)>(vArgs) ...); }

template <typename K, typename V, typename C, typename ... Cs>
template <Direction d, Constness c, std::size_t n>
template <Derived<V> DV>
DV&
Map<K, V, C, Cs ...>::Iterator<d, c, n>::set(auto&& ... dvArgs) const
requires (c == Constness::NCONST)
{ return reinterpret_cast<MNode<K, DV, C, Cs ...>*>(pos)->set(std::forward<decltype(dvArgs)>(dvArgs) ...); }

template <typename K, typename V, typename C, typename ... Cs>
template <Direction d, Constness c, std::size_t n>
template <typename ... VCIArgs>
V&
Map<K, V, C, Cs ...>::Iterator<d, c, n>::set(DP::CreateInfo<V, VCIArgs ...> const& vCreateInfo, auto&& ... vcArgs) const
requires (c == Constness::NCONST)
{ return reinterpret_cast<MNode<K, V, C, Cs ...>*>(pos)->set(vCreateInfo.create, std::forward<decltype(vcArgs)>(vcArgs) ...); }

template <typename K, typename V, typename C, typename ... Cs>
template <Direction d, Constness c, std::size_t n>
class Map<K, V, C, Cs ...>::Iterator<d, c, n>&
Map<K, V, C, Cs ...>::Iterator<d, c, n>::operator++() noexcept
{
	pos = (d == Direction::FORWARD ? pos->template next<n>() : pos->template prev<n>());
	return *this;
}

template <typename K, typename V, typename C, typename ... Cs>
template <Direction d, Constness c, std::size_t n>
class Map<K, V, C, Cs ...>::Iterator<d, c, n>
Map<K, V, C, Cs ...>::Iterator<d, c, n>::operator++(int) noexcept
{
	auto* r = pos;
	pos = (d == Direction::FORWARD ? pos->template next<n>() : pos->template prev<n>());
	return r;
}

template <typename K, typename V, typename C, typename ... Cs>
template <Direction d, Constness c, std::size_t n>
class Map<K, V, C, Cs ...>::Iterator<d, c, n>&
Map<K, V, C, Cs ...>::Iterator<d, c, n>::operator--() noexcept
{
	pos = (d == Direction::FORWARD ? pos->template prev<n>() : pos->template next<n>());
	return *this;
}

template <typename K, typename V, typename C, typename ... Cs>
template <Direction d, Constness c, std::size_t n>
class Map<K, V, C, Cs ...>::Iterator<d, c, n>
Map<K, V, C, Cs ...>::Iterator<d, c, n>::operator--(int) noexcept
{
	auto* r = pos;
	pos = (d == Direction::FORWARD ? pos->template prev<n>() : pos->template next<n>());
	return r;
}

template <typename K, typename V, typename C, typename ... Cs>
template <Direction d, Constness c, std::size_t n>
std::pair<K const, TConstness<V, c>>&
Map<K, V, C, Cs ...>::Iterator<d, c, n>::operator*() const noexcept
{ return reinterpret_cast<std::pair<K const, TConstness<V, c>>&>(reinterpret_cast<SNode<K, C, Cs ...>*>(pos)->key); }

template <typename K, typename V, typename C, typename ... Cs>
template <Direction d, Constness c, std::size_t n>
std::pair<K const, TConstness<V, c>>*
Map<K, V, C, Cs ...>::Iterator<d, c, n>::operator->() const noexcept
{ return reinterpret_cast<std::pair<K const, TConstness<V, c>>*>(&reinterpret_cast<SNode<K, C, Cs ...>*>(pos)->key); }

template <typename K, typename V, typename C, typename ... Cs>
template <Direction d, Constness c, std::size_t n>
template <Direction od, Constness oc, std::size_t on>
bool
Map<K, V, C, Cs ...>::Iterator<d, c, n>::operator==(Iterator<od, oc, on> const& other) const noexcept
{ return pos == other.pos; }

template <typename K, typename V, typename C, typename ... Cs>
template <Direction d, Constness c, std::size_t n>
Map<K, V, C, Cs ...>::Iterator<d, c, n>::operator bool() const noexcept
{ return pos; }

}//namespace DS
