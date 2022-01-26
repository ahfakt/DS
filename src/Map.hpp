#include "DS/Map.h"

namespace DS {

template <typename K, typename V>
Map<K, V>::Map(Map const& other)
requires std::is_copy_constructible_v<K> && std::is_copy_constructible_v<V>
		: Container(other.mSize)
		, mRoot(other.mRoot ? MNode<K, V>::Create(nullptr, nullptr, other.mRoot) : nullptr)
{}

template <typename K, typename V>
Map<K, V>::Map(Map&& other) noexcept
{ swap(*this, other); }

template <typename K, typename V>
void
swap(Map<K, V>& a, Map<K, V>& b) noexcept
{
	std::swap(a.mSize, b.mSize);
	std::swap(a.mRoot, b.mRoot);
}

template <typename K, typename V>
Map<K, V>&
Map<K, V>::operator=(Map value) noexcept
{
	swap(*this, value);
	return *this;
}

template <typename K, typename V>
template <typename ... KArgs, typename ... VArgs>
Map<K, V>::Map(KArgs&& ... kArgs, Stream::Input& input, VArgs&& ... vArgs)
requires Deserializable<K, Stream::Input, KArgs ...> && Deserializable<V, Stream::Input, VArgs ...>
		: Container(Stream::Get<std::uint64_t>(input))
		, mRoot(mSize ? MNode<K, V>::Create(nullptr, nullptr, std::forward<KArgs>(kArgs) ..., input, std::forward<VArgs>(vArgs) ...) : nullptr)
{}

template <typename K, typename V>
template <typename ... KArgs, typename VIDType, typename ... VFArgs>
Map<K, V>::Map(KArgs&& ... kArgs, Stream::Input& input, DP::Factory<V, VIDType, VFArgs ...> const& vFactory)
requires Deserializable<K, Stream::Input, KArgs ...>
		: Container(Stream::Get<std::uint64_t>(input))
		, mRoot(mSize ? MNode<K, V>::Create(nullptr, nullptr, std::forward<KArgs>(kArgs) ..., input, vFactory) : nullptr)
{}

template <typename K, typename V>
template <typename KIDType, typename ... KFArgs, typename ... VArgs>
Map<K, V>::Map(DP::Factory<K, KIDType, KFArgs ...> const& kFactory, Stream::Input& input, VArgs&& ... vArgs)
requires Deserializable<V, Stream::Input, VArgs ...>
		: Container(Stream::Get<std::uint64_t>(input))
{
	if (mSize) {
		try {
			mRoot = MNode<K, V>::Create(nullptr, nullptr, kFactory, input, std::forward<VArgs>(vArgs) ...);
		} catch (typename MNode<K, V>::Exception& exc) {
			throw Exception(exc);
		}
	}
}

template <typename K, typename V>
template <typename KIDType, typename ... KFArgs, typename VIDType, typename ... VFArgs>
Map<K, V>::Map(DP::Factory<K, KIDType, KFArgs ...> const& kFactory, Stream::Input& input, DP::Factory<V, VIDType, VFArgs ...> const& vFactory)
		: Container(Stream::Get<std::uint64_t>(input))
{
	if (mSize) {
		try {
			mRoot = MNode<K, V>::Create(nullptr, nullptr, kFactory, input, vFactory);
		} catch (typename MNode<K, V>::Exception& exc) {
			throw Exception(exc);
		}
	}
}

template <typename K, typename V>
Stream::Output&
operator<<(Stream::Output& output, Map<K, V> const& map)
requires Stream::Serializable<K, Stream::Output> && Stream::Serializable<V, Stream::Output>
{
	output << map.mSize;
	if (map.mRoot)
		map.mRoot->serialize(output);
	return output;
}

template <typename K, typename V>
Map<K, V>::~Map()
{ delete mRoot; }

template <typename K, typename V>
MNode<K, V>*
Map<K, V>::put(MNode<K, V>* created) noexcept
{
	if (mRoot) {
		MNode<K, V>* t = created;
		if (MNode<K, V>* m = mRoot->attach(created)) {
			if (m != mRoot)
				mRoot = m;
		}
		if (created == t)
			++mSize;
		else
			delete t;
	} else {
		mRoot = created;
		++mSize;
	}
	return created;
}

template <typename K, typename V>
template <typename ... KArgs>
typename Map<K, V>::iterator
Map<K, V>::put(KArgs&& ... kArgs)
{ return put(::new MNode<K, V>(std::forward<KArgs>(kArgs) ...)); }

template <typename K, typename V>
template <typename ... KArgs, Derived<V> DV>
typename Map<K, V>::iterator
Map<K, V>::put(KArgs&& ... kArgs)
{ return put(reinterpret_cast<MNode<K, V>*>(::new MNode<K, DV>(std::forward<KArgs>(kArgs) ...))); }

template <typename K, typename V>
template <typename ... KArgs, typename ... VCIArgs>
typename Map<K, V>::iterator
Map<K, V>::put(KArgs&& ... kArgs, DP::CreateInfo<V, VCIArgs ...> const& vCreateInfo)
{ return put(new(vCreateInfo.size) MNode<K, V>(std::forward<KArgs>(kArgs) ...)); }


template <typename K, typename V>
template <EqDerived<K> DK, typename ... DKArgs>
typename Map<K, V>::iterator
Map<K, V>::put(DKArgs&& ... dkArgs)
{ return put(reinterpret_cast<MNode<K, V>*>(::new MNode<DK, V>(std::forward<DKArgs>(dkArgs) ...))); }

template <typename K, typename V>
template <EqDerived<K> DK, typename ... DKArgs, Derived<V> DV>
typename Map<K, V>::iterator
Map<K, V>::put(DKArgs&& ... dkArgs)
{ return put(reinterpret_cast<MNode<K, V>*>(::new MNode<DK, DV>(std::forward<DKArgs>(dkArgs) ...))); }

template <typename K, typename V>
template <EqDerived<K> DK, typename ... DKArgs, typename ... VCIArgs>
typename Map<K, V>::iterator
Map<K, V>::put(DKArgs&& ... dkArgs, DP::CreateInfo<V, VCIArgs ...> const& vCreateInfo)
{ return put(reinterpret_cast<MNode<K, V>*>(new(vCreateInfo.size) MNode<DK, V>(std::forward<DKArgs>(dkArgs) ...))); }


template <typename K, typename V>
template <typename ... KCIArgs, typename ... KCArgs>
typename Map<K, V>::iterator
Map<K, V>::put(DP::CreateInfo<K, KCIArgs ...> const& kCreateInfo, KCArgs&& ... kcArgs)
{
	if (kCreateInfo.size > sizeof(K))
		throw Exception("kCreateInfo.size is greater than the size of K.");
	return put(::new MNode<K, V>(kCreateInfo.create, std::forward<KCArgs>(kcArgs) ...));
}

template <typename K, typename V>
template <typename ... KCIArgs, typename ... KCArgs, Derived<V> DV>
typename Map<K, V>::iterator
Map<K, V>::put(DP::CreateInfo<K, KCIArgs ...> const& kCreateInfo, KCArgs&& ... kcArgs)
{
	if (kCreateInfo.size > sizeof(K))
		throw Exception("kCreateInfo.size is greater than the size of K.");
	return put(reinterpret_cast<MNode<K, V>*>(::new MNode<K, DV>(kCreateInfo.create, std::forward<KCArgs>(kcArgs) ...)));
}

template <typename K, typename V>
template <typename ... KCIArgs, typename ... KCArgs, typename ... VCIArgs>
typename Map<K, V>::iterator
Map<K, V>::put(DP::CreateInfo<K, KCIArgs ...> const& kCreateInfo, KCArgs&& ... kcArgs, DP::CreateInfo<V, VCIArgs ...> const& vCreateInfo)
{
	if (kCreateInfo.size > sizeof(K))
		throw Exception("kCreateInfo.size is greater than the size of K.");
	return put(reinterpret_cast<MNode<K, V>*>(new(vCreateInfo.size) MNode<K, V>(kCreateInfo.create, std::forward<KCArgs>(kcArgs) ...)));
}


template <typename K, typename V>
template <Direction d, Constness c>
typename Map<K, V>::iterator
Map<K, V>::add(Iterator<d, c> const& it)
requires std::is_copy_constructible_v<K> && std::is_copy_constructible_v<V> && std::is_copy_assignable_v<V>
{
	MNode<K, V>* t = operator[](it.pos->key).pos;
	if (t) {
		if (t->state.hasValue) {
			if (it.pos->state.hasValue)
				 static_cast<V&>(t->val) = static_cast<V const&>(it.pos->val);
			else
				t->unset();
		} else if (it.pos->state.hasValue)
			t->set(static_cast<V const&>(it.pos->val));
		return t;
	} else {
		t = ::new MNode<K, V>(it.pos->key);
		if (it.pos->state.hasValue) {
			try {
				t->set(*it.pos->val);
			} catch (...) {
				delete t;
				throw;
			}
		}
		return put(t);
	}
}

template <typename K, typename V>
template <typename T>
bool
Map<K, V>::remove(T&& key) noexcept
{
	MNode<K, V>* toDel = nullptr;
	if (mRoot) {
		if (MNode<K, V>* m = mRoot->detach(key, toDel)) {
			if (m != mRoot)
				mRoot = m;
		} else if (toDel == mRoot)
			mRoot = nullptr;
		if (toDel) {
			delete toDel;
			--mSize;
		}
	}
	return toDel;
}

template <typename K, typename V>
template <typename T>
typename Map<K, V>::iterator
Map<K, V>::operator[](T&& k) noexcept
{ return static_cast<Map const&>(*this)[std::forward<T>(k)].pos; }

template <typename K, typename V>
template <typename T>
typename Map<K, V>::const_iterator
Map<K, V>::operator[](T&& k) const noexcept
{
	if (MNode<K, V>* t = mRoot) {
		do {
			if (k < static_cast<K const&>(t->key)) {
				if (t->state.hasLeft)
					t = t->left;
				else
					return nullptr;
			} else if (static_cast<K const&>(t->key) < k) {
				if (t->state.hasRight)
					t = t->right;
				else
					return nullptr;
			} else
				return t;
		} while (true);
	}
	return nullptr;
}

template <typename K, typename V>
typename Map<K, V>::iterator
Map<K, V>::at(std::uint64_t i) noexcept
{ return static_cast<Map const*>(this)->at(i).pos; }

template <typename K, typename V>
typename Map<K, V>::const_iterator
Map<K, V>::at(std::uint64_t i) const noexcept
{ // TODO: O(n)->O(logn) childCount?
	const_iterator it(nullptr);
	if (i < mSize / 2) {
		it.pos = first();
		for (std::uint64_t j = 0; j < i; ++j)
			++it;
	} else {
		it.pos = last();
		for (std::uint64_t j = mSize - 1; i < j; --j)
			--it;
	}
	return it;
}

template <typename K, typename V>
typename Map<K, V>::iterator
Map<K, V>::begin() noexcept
{ return first(); }

template <typename K, typename V>
typename Map<K, V>::const_iterator
Map<K, V>::begin() const noexcept
{ return first(); }

template <typename K, typename V>
typename Map<K, V>::iterator
Map<K, V>::end() noexcept
{ return nullptr; }

template <typename K, typename V>
typename Map<K, V>::const_iterator
Map<K, V>::end() const noexcept
{ return nullptr; }

template <typename K, typename V>
typename Map<K, V>::reverse_iterator
Map<K, V>::rbegin() noexcept
{ return last(); }

template <typename K, typename V>
typename Map<K, V>::const_reverse_iterator
Map<K, V>::rbegin() const noexcept
{ return last(); }

template <typename K, typename V>
typename Map<K, V>::reverse_iterator
Map<K, V>::rend() noexcept
{ return nullptr; }

template <typename K, typename V>
typename Map<K, V>::const_reverse_iterator
Map<K, V>::rend() const noexcept
{ return nullptr; }

template <typename K, typename V>
typename Map<K, V>::const_iterator
Map<K, V>::cbegin() const noexcept
{ return first(); }

template <typename K, typename V>
typename Map<K, V>::const_iterator
Map<K, V>::cend() const noexcept
{ return nullptr; }

template <typename K, typename V>
typename Map<K, V>::const_reverse_iterator
Map<K, V>::crbegin() const noexcept
{ return last(); }

template <typename K, typename V>
typename Map<K, V>::const_reverse_iterator
Map<K, V>::crend() const noexcept
{ return nullptr; }

template <typename K, typename V>
MNode<K, V>*
Map<K, V>::first() const noexcept
{
	MNode<K, V>* t;
	if (t = mRoot)
		while (t->state.hasLeft)
			t = t->left;
	return t;
}

template <typename K, typename V>
MNode<K, V>*
Map<K, V>::last() const noexcept
{
	MNode<K, V>* t;
	if (t = mRoot)
		while (t->state.hasRight)
			t = t->right;
	return t;
}

template <typename K, typename V>
template <Direction d, Constness c>
Map<K, V>::Iterator<d, c>::Iterator(MNode<K, V>* pos) noexcept
		: pos(pos)
{}

template <typename K, typename V>
template <Direction d, Constness c>
template <Direction od, Constness oc>
Map<K, V>::Iterator<d, c>::Iterator(Iterator<od, oc> const& other) noexcept
requires ConstCompat<c, oc>
		: pos(other.pos)
{}

template <typename K, typename V>
template <Direction d, Constness c>
template <Direction od, Constness oc>
class Map<K, V>::Iterator<d, c>&
Map<K, V>::Iterator<d, c>::operator=(Iterator<od, oc> const& other) noexcept
requires ConstCompat<c, oc>
{
	pos = other.pos;
	return *this;
}

template <typename K, typename V>
template <Direction d, Constness c>
bool
Map<K, V>::Iterator<d, c>::hasValue() const noexcept
{ return pos->state.hasValue; }

template <typename K, typename V>
template <Direction d, Constness c>
void
Map<K, V>::Iterator<d, c>::unset() const noexcept
requires (c == Constness::NCONST)
{ pos->unset(); }

template <typename K, typename V>
template <Direction d, Constness c>
template <typename ... VArgs>
V&
Map<K, V>::Iterator<d, c>::set(VArgs&& ... vArgs) const
requires (c == Constness::NCONST)
{ return pos->set(std::forward<VArgs>(vArgs) ...); }

template <typename K, typename V>
template <Direction d, Constness c>
template <Derived<V> DV, typename ... DVArgs>
DV&
Map<K, V>::Iterator<d, c>::set(DVArgs&& ... dvArgs) const
requires (c == Constness::NCONST)
{ return reinterpret_cast<MNode<K, DV>*>(pos)->set(std::forward<DVArgs>(dvArgs) ...); }

template <typename K, typename V>
template <Direction d, Constness c>
template <typename ... VCIArgs, typename ... VCArgs>
V&
Map<K, V>::Iterator<d, c>::set(DP::CreateInfo<V, VCIArgs ...> const& vCreateInfo, VCArgs&& ... vcArgs) const
requires (c == Constness::NCONST)
{ return pos->set(vCreateInfo.create, std::forward<VCArgs>(vcArgs) ...); }

template <typename K, typename V>
template <Direction d, Constness c>
class Map<K, V>::Iterator<d, c>&
Map<K, V>::Iterator<d, c>::inc() noexcept
{
	if (pos->state.hasRight) {
		pos = pos->right;
		while (pos->state.hasLeft)
			pos = pos->left;
	} else
		pos = pos->right;
	return *this;
}

template <typename K, typename V>
template <Direction d, Constness c>
class Map<K, V>::Iterator<d, c>&
Map<K, V>::Iterator<d, c>::dec() noexcept
{
	if (pos->state.hasLeft) {
		pos = pos->left;
		while (pos->state.hasRight)
			pos = pos->right;
	} else
		pos = pos->left;
	return *this;
}

template <typename K, typename V>
template <Direction d, Constness c>
class Map<K, V>::Iterator<d, c>&
Map<K, V>::Iterator<d, c>::operator++() noexcept
{ return d == Direction::FORWARD ? inc() : dec(); }

template <typename K, typename V>
template <Direction d, Constness c>
class Map<K, V>::Iterator<d, c>
Map<K, V>::Iterator<d, c>::operator++(int) noexcept
{
	auto* r = pos;
	this->operator++();
	return r;
}

template <typename K, typename V>
template <Direction d, Constness c>
class Map<K, V>::Iterator<d, c>&
Map<K, V>::Iterator<d, c>::operator--() noexcept
{ return d == Direction::FORWARD ? dec() : inc(); }

template <typename K, typename V>
template <Direction d, Constness c>
class Map<K, V>::Iterator<d, c>
Map<K, V>::Iterator<d, c>::operator--(int) noexcept
{
	auto* r = pos;
	this->operator--();
	return r;
}

template <typename K, typename V>
template <Direction d, Constness c>
class Map<K, V>::template Entry<c>&
Map<K, V>::Iterator<d, c>::operator*() const noexcept
{ return reinterpret_cast<Entry<c>&>(pos->state); }

template <typename K, typename V>
template <Direction d, Constness c>
class Map<K, V>::template Entry<c>*
Map<K, V>::Iterator<d, c>::operator->() const noexcept
{ return reinterpret_cast<Entry<c>*>(&pos->state); }

template <typename K, typename V>
template <Direction d, Constness c>
template <Direction od, Constness oc>
bool
Map<K, V>::Iterator<d, c>::operator==(Iterator<od, oc> const& other) const noexcept
{ return pos == other.pos; }

template <typename K, typename V>
template <Direction d, Constness c>
Map<K, V>::Iterator<d, c>::operator bool() const noexcept
{ return pos; }

}//namespace DS
