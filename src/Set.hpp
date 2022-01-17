#include "DS/Set.h"

namespace DS {

template <typename K>
Set<K>::Set(Set const& other)
requires std::is_copy_constructible_v<K>
		: Container(other.mSize)
		, mRoot(other.mRoot ? SNode<K>::Create(nullptr, nullptr, other.mRoot) : nullptr)
{}

template <typename K>
Set<K>::Set(Set&& other) noexcept
{ swap(*this, other); }

template <typename K>
void
swap(Set<K>& a, Set<K>& b) noexcept
{
	std::swap(a.mSize, b.mSize);
	std::swap(a.mRoot, b.mRoot);
}

template <typename K>
Set<K>&
Set<K>::operator=(Set value) noexcept
{
	swap(*this, value);
	return *this;
}

template <typename K>
template <typename ... KArgs>
Set<K>::Set(Stream::Input& input, KArgs&& ... kArgs)
requires Deserializable<K, Stream::Input, KArgs ...>
		: Container(Stream::Get<std::uint64_t>(input))
		, mRoot(mSize ? SNode<K>::Create(nullptr, nullptr, input, std::forward<KArgs>(kArgs) ...) : nullptr)
{}

template <typename K>
template <typename IDType, typename ... FArgs>
Set<K>::Set(Stream::Input& input, DP::Factory<K, IDType, FArgs ...> const& factory)
		: Container(Stream::Get<std::uint64_t>(input))
		, mRoot(mSize ? SNode<K>::Create(nullptr, nullptr, input, factory) : nullptr)
{}

template <typename K>
Stream::Output&
operator<<(Stream::Output& output, Set<K> const& set)
requires Stream::Serializable<K, Stream::Output>
{
	output << set.mSize;
	if (set.mRoot)
		set.mRoot->serialize(output);
	return output;
}

/*
template <typename K>
Set<K>&
Set<K>::toDot(Stream::Output& output) requires Serializable<K>
{
	auto s = begin();
	output << "digraph G {\n\trankdir=\"TB\"\n\tsplines=false\n\tnode [shape=circle style=filled fillcolor=\"whse;0.8:black\" fixedsize=true]\n";
	if (s) {
		do {
			output << "\t" << s.pos->key << " [gradientangle=" << (s.pos->state.isLeft ? "315" : (s.pos->state
																									  .isRight ? "225" : "270")) << "]\n";
			if (s.pos->left)
				output << "\t" << s.pos->key << ":w -> " << s.pos->left->key << (s.pos->state
																					 .hasLeft ? ":n\n" : ":s [style=\"dashed\" constraint=false]\n");
			if (s.pos->right)
				output << "\t" << s.pos->key << ":e -> " << s.pos->right->key << (s.pos->state
																					  .hasRight ? ":n\n" : ":s [style=\"dashed\" constraint=false]\n");
		} while (++s);
	}

	output << "}\n";
	return *this;
}
 */

template <typename K>
Set<K>::~Set()
{ delete mRoot; }

template <typename K>
SNode<K>*
Set<K>::put(SNode<K>* created) noexcept
{
	if (mRoot) {
		SNode<K>* t = created;
		if (SNode<K>* s = mRoot->attach(created)) {
			if (s != mRoot)
				mRoot = s;
		}
		if (created == t)
			++mSize;
		else
			delete t;
	} else {
		mRoot = created;
		created->left = nullptr;
		created->right = nullptr;
		++mSize;
	}
	return created;
}

template <typename K>
template <typename ... KArgs>
typename Set<K>::const_iterator
Set<K>::put(KArgs&& ... kArgs)
{ return put(::new SNode<K>(std::forward<KArgs>(kArgs) ...)); }

template <typename K>
template <Derived<K> DK, typename ... DKArgs>
typename Set<K>::const_iterator
Set<K>::put(DKArgs&& ... dkArgs)
{ return put(reinterpret_cast<SNode<K>*>(::new SNode<DK>(std::forward<DKArgs>(dkArgs) ...))); }

template <typename K>
template <typename ... CIArgs, typename ... CArgs>
typename Set<K>::const_iterator
Set<K>::put(DP::CreateInfo<K, CIArgs ...> const& createInfo, CArgs&& ... cArgs)
{ return put(new(createInfo.size) SNode<K>(createInfo.create, std::forward<CArgs>(cArgs) ...)); }

template <typename K>
template <typename T>
bool
Set<K>::remove(T&& k) noexcept
{
	SNode<K>* toDel = nullptr;
	if (mRoot) {
		if (SNode<K>* s = mRoot->detach(std::forward<T>(k), toDel)) {
			if (s != mRoot)
				mRoot = s;
		} else if (toDel == mRoot)
			mRoot = nullptr;
		if (toDel) {
			delete toDel;
			--mSize;
		}
	}
	return toDel;
}

template <typename K>
template <typename T>
typename Set<K>::const_iterator
Set<K>::get(T&& k) const noexcept
{
	if (SNode<K>* t = mRoot) {
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

template <typename K>
typename Set<K>::const_iterator
Set<K>::operator[](std::uint64_t i) const noexcept
{ // TODO: O(n)->O(logn) childCount?
	const_iterator it;
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

template <typename K>
typename Set<K>::const_iterator
Set<K>::begin() const noexcept
{ return first(); }

template <typename K>
typename Set<K>::const_iterator
Set<K>::end() const noexcept
{ return nullptr; }

template <typename K>
typename Set<K>::const_reverse_iterator
Set<K>::rbegin() const noexcept
{ return last(); }

template <typename K>
typename Set<K>::const_reverse_iterator
Set<K>::rend() const noexcept
{ return nullptr; }

template <typename K>
typename Set<K>::const_iterator
Set<K>::cbegin() const noexcept
{ return first(); }

template <typename K>
typename Set<K>::const_iterator
Set<K>::cend() const noexcept
{ return nullptr; }

template <typename K>
typename Set<K>::const_reverse_iterator
Set<K>::crbegin() const noexcept
{ return last(); }

template <typename K>
typename Set<K>::const_reverse_iterator
Set<K>::crend() const noexcept
{ return nullptr; }

template <typename K>
SNode<K>*
Set<K>::first() const noexcept
{
	SNode<K>* t;
	if (t = mRoot)
		while (t->state.hasLeft)
			t = t->left;
	return t;
}

template <typename K>
SNode<K>*
Set<K>::last() const noexcept
{
	SNode<K>* t;
	if (t = mRoot)
		while (t->state.hasRight)
			t = t->right;
	return t;
}

template <typename K>
template <Direction d>
Set<K>::Iterator<d>::Iterator(SNode<K>* pos) noexcept
		: pos(pos)
{}

template <typename K>
template <Direction d>
template <Direction od>
Set<K>::Iterator<d>::Iterator(Iterator<od> const& other) noexcept
		: pos(other.pos)
{}

template <typename K>
template <Direction d>
template <Direction od>
class Set<K>::Iterator<d>&
Set<K>::Iterator<d>::operator=(Iterator<od> const& other) noexcept
{
	pos = other.pos;
	return *this;
}

template <typename K>
template <Direction d>
class Set<K>::Iterator<d>&
Set<K>::Iterator<d>::inc() noexcept
{
	if (pos->state.hasRight) {
		pos = pos->right;
		while (pos->state.hasLeft)
			pos = pos->left;
	} else
		pos = pos->right;
	return *this;
}

template <typename K>
template <Direction d>
class Set<K>::Iterator<d>&
Set<K>::Iterator<d>::dec() noexcept
{
	if (pos->state.hasLeft) {
		pos = pos->left;
		while (pos->state.hasRight)
			pos = pos->right;
	} else
		pos = pos->left;
	return *this;
}

template <typename K>
template <Direction d>
class Set<K>::Iterator<d>&
Set<K>::Iterator<d>::operator++() noexcept
{ return d == Direction::FORWARD ? inc() : dec(); }

template <typename K>
template <Direction d>
class Set<K>::Iterator<d>
Set<K>::Iterator<d>::operator++(int) noexcept
{
	auto* r = pos;
	this->operator++();
	return r;
}

template <typename K>
template <Direction d>
class Set<K>::Iterator<d>&
Set<K>::Iterator<d>::operator--() noexcept
{ return d == Direction::FORWARD ? dec() : inc(); }

template <typename K>
template <Direction d>
class Set<K>::Iterator<d>
Set<K>::Iterator<d>::operator--(int) noexcept
{
	auto* r = pos;
	this->operator--();
	return r;
}

template <typename K>
template <Direction d>
K const&
Set<K>::Iterator<d>::operator*() const noexcept
{ return static_cast<K const&>(pos->key); }

template <typename K>
template <Direction d>
K const*
Set<K>::Iterator<d>::operator->() const noexcept
{ return static_cast<K const*>(pos->key); }

template <typename K>
template <Direction d>
template <Direction od>
bool
Set<K>::Iterator<d>::operator==(Iterator<od> const& other) const noexcept
{ return pos == other.pos; }

template <typename K>
template <Direction d>
Set<K>::Iterator<d>::operator bool() const noexcept
{ return pos; }

}//namespace DS
