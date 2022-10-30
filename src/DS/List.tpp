#include "DS/List.hpp"

namespace DS {

template <typename T>
List<T>::List(List const& other)
requires std::is_copy_constructible_v<T>
		: Container(other.mSize)
{
	if (mSize) {
		(mTail = mHead = ::new LNode<T>(static_cast<T const&>(other.mHead->val)))->prev = nullptr;
		LNode<T> const* src = other.mHead;
		while (src = src->next) {
			try {
				(mTail->next = ::new LNode<T>(static_cast<T const&>(src->val)))->prev = mTail;
				mTail = mTail->next;
			} catch (...) {
				this->~List();
				throw;
			}
		}
		mTail->next = nullptr;
	}
}

template <typename T>
List<T>::List(List&& other) noexcept
{ swap(*this, other); }

template <typename T>
void
swap(List<T>& a, List<T>& b) noexcept
{
	std::swap(a.mSize, b.mSize);
	std::swap(a.mHead, b.mHead);
	std::swap(a.mTail, b.mTail);
}

template <typename T>
List<T>&
List<T>::operator=(List value) noexcept
{
	swap(*this, value);
	return *this;
}

template <typename T>
List<T>::List(Stream::Input& input, auto&& ... tArgs)
requires Stream::DeserializableWith<T, decltype(input), decltype(tArgs) ...>
		: Container(Stream::Get<std::uint64_t>(input))
{
	if (mSize) {
		(mTail = mHead = ::new LNode<T>(input, std::forward<decltype(tArgs)>(tArgs) ...))->prev = nullptr;
		std::uint64_t size = mSize;
		while (--size) {
			try {
				mTail->next = ::new LNode<T>(input, std::forward<decltype(tArgs)>(tArgs) ...);
			} catch (...) {
				this->~List();
				throw;
			}
			mTail->next->prev = mTail;
			mTail = mTail->next;
		}
		mTail->next = nullptr;
	}
}

template <typename T>
template <typename Type, typename ... Args>
List<T>::List(Stream::Input& input, DP::Factory<T, Type, Args ...>, auto&& ... tArgs)
		: Container(Stream::Get<std::uint64_t>(input))
{
	using seq = std::make_index_sequence<sizeof...(Args) - sizeof...(tArgs)>;
	if (mSize) {
		{
			auto const& createInfo = DP::Factory<T, Type, Args ...>::GetCreateInfo(Stream::Get<Type>(input));
			(mTail = mHead = new(createInfo.size) LNode<T>(createInfo, input, seq{}, std::forward<decltype(tArgs)>(tArgs) ...))->prev = nullptr;
		}
		std::uint64_t size = mSize;
		while (--size) {
			try {
				auto const& createInfo = DP::Factory<T, Type, Args ...>::GetCreateInfo(Stream::Get<Type>(input));
				mTail->next = new(createInfo.size) LNode<T>(createInfo, input, seq{}, std::forward<decltype(tArgs)>(tArgs) ...);
			} catch (...) {
				this->~List();
				throw;
			}
			mTail->next->prev = mTail;
			mTail = mTail->next;
		}
		mTail->next = nullptr;
	}
}

template <typename T>
Stream::Output&
operator<<(Stream::Output& output, List<T> const& list)
requires Stream::InsertableTo<T, decltype(output)>
{
	output << list.mSize;
	for (LNode<T> const* l = list.mHead; l; l = l->next)
		output << static_cast<T const&>(l->val);
	return output;
}

template <typename T>
Stream::Format::DotOutput&
operator<<(Stream::Format::DotOutput& dotOutput, List<T> const& list)
requires Stream::InsertableTo<T, decltype(dotOutput)>
{
	dotOutput << "digraph G {\n";
	if (list.mHead) {
		LNode<T>* l = list.mHead;
		do {
			dotOutput << l->val << (l->next ? "->" : "\n");
		} while (l = l->next);
		if (list.mTail != list.mHead) {
			l = list.mTail;
			do {
				dotOutput << l->val << (l->prev ? "->" : "\n");
			} while (l = l->prev);
		}
	}
	dotOutput << "}\n";
	return dotOutput;
}

template <typename T>
List<T>::~List()
{
	while (mTail != mHead)
		delete (mTail = mTail->prev)->next;
	delete mHead;
}

template <typename T>
LNode<T>*
List<T>::pushFront(LNode<T>* l) noexcept
{
	++mSize;
	l->prev = nullptr;
	return mHead = ((l->next = mHead) ? mHead->prev : mTail) = l;
}

template <typename T>
List<T>&
List<T>::pushFront(List value) noexcept
{
	if (value.mTail) {
		if (mHead) {
			mSize += value.mSize;
			(value.mTail->next = mHead)->prev = value.mTail;
		} else {
			mSize = value.mSize;
			mTail = value.mTail;
		}
		mHead = value.mHead;
		value.mSize = 0;
		value.mTail = value.mHead = nullptr;
	}
	return *this;
}

template <typename T>
typename List<T>::iterator
List<T>::pushFront(auto&& ... tArgs)
{ return pushFront(::new LNode<T>(std::forward<decltype(tArgs)>(tArgs) ...)); }

template <typename T>
template <Derived<T> DT>
typename List<T>::iterator
List<T>::pushFront(auto&& ... dtArgs)
{ return pushFront(reinterpret_cast<LNode<T>*>(::new LNode<DT>(std::forward<decltype(dtArgs)>(dtArgs) ...))); }

template <typename T>
template <typename ... Args>
typename List<T>::iterator
List<T>::pushFront(DP::CreateInfo<T, Args ...> const& createInfo, auto&& ... args)
{ return pushFront(new(createInfo.size) LNode<T>(createInfo, std::forward<decltype(args)>(args) ...)); }

template <typename T>
LNode<T>*
List<T>::pushBack(LNode<T>* l) noexcept
{
	++mSize;
	l->next = nullptr;
	return mTail = ((l->prev = mTail) ? mTail->next : mHead) = l;
}

template <typename T>
List<T>&
List<T>::pushBack(List value) noexcept
{
	if (value.mHead) {
		if (mTail) {
			mSize += value.mSize;
			(value.mHead->prev = mTail)->next = value.mHead;
		} else {
			mSize = value.mSize;
			mHead = value.mHead;
		}
		mTail = value.mTail;
		value.mSize = 0;
		value.mTail = value.mHead = nullptr;
	}
	return *this;
}

template <typename T>
typename List<T>::iterator
List<T>::pushBack(auto&& ... tArgs)
{ return pushBack(::new LNode<T>(std::forward<decltype(tArgs)>(tArgs) ...)); }

template <typename T>
template <Derived<T> DT>
typename List<T>::iterator
List<T>::pushBack(auto&& ... dtArgs)
{ return pushBack(reinterpret_cast<LNode<T>*>(::new LNode<DT>(std::forward<decltype(dtArgs)>(dtArgs) ...))); }

template <typename T>
template <typename ... Args>
typename List<T>::iterator
List<T>::pushBack(DP::CreateInfo<T, Args ...> const& createInfo, auto&& ... args)
{ return pushBack(new(createInfo.size) LNode<T>(createInfo, std::forward<decltype(args)>(args) ...)); }

template <typename T>
LNode<T>*
List<T>::insertBefore(LNode<T>* p, LNode<T>* l) noexcept
{
	if (p == mHead)
		return pushFront(l);
	if (p) {
		++mSize;
		return p->prev = (l->prev = (l->next = p)->prev)->next = l;
	}
	return pushBack(l);
}

template <typename T>
template <Direction d, Constness c>
typename List<T>::iterator
List<T>::insertBefore(Iterator<d, c> at, auto&& ... tArgs)
{ return insertBefore(at.pos, ::new LNode<T>(std::forward<decltype(tArgs)>(tArgs) ...)); }

template <typename T>
template <Derived<T> DT, Direction d, Constness c>
typename List<T>::iterator
List<T>::insertBefore(Iterator<d, c> at, auto&& ... dtArgs)
{ return insertBefore(at.pos, reinterpret_cast<LNode<T>*>(::new LNode<DT>(std::forward<decltype(dtArgs)>(dtArgs) ...))); }

template <typename T>
template <typename ... Args, Direction d, Constness c>
typename List<T>::iterator
List<T>::insertBefore(Iterator<d, c> at, DP::CreateInfo<T, Args ...> const& createInfo, auto&& ... args)
{ return insertBefore(at.pos, new(createInfo.size) LNode<T>(createInfo, std::forward<decltype(args)>(args) ...)); }

template <typename T>
LNode<T>*
List<T>::insertAfter(LNode<T>* p, LNode<T>* l) noexcept
{
	if (p == mTail)
		return pushBack(l);
	if (p) {
		++mSize;
		return p->next = (l->next = (l->prev = p)->next)->prev = l;
	}
	return pushFront(l);
}

template <typename T>
template <Direction d, Constness c>
typename List<T>::iterator
List<T>::insertAfter(Iterator<d, c> at, auto&& ... tArgs)
{ return insertAfter(at.pos, ::new LNode<T>(std::forward<decltype(tArgs)>(tArgs) ...)); }

template <typename T>
template <Derived<T> DT, Direction d, Constness c>
typename List<T>::iterator
List<T>::insertAfter(Iterator<d, c> at, auto&& ... dtArgs)
{ return insertAfter(at.pos, reinterpret_cast<LNode<T>*>(::new LNode<DT>(std::forward<decltype(dtArgs)>(dtArgs) ...))); }

template <typename T>
template <typename ... Args, Direction d, Constness c>
typename List<T>::iterator
List<T>::insertAfter(Iterator<d, c> at, DP::CreateInfo<T, Args ...> const& createInfo, auto&& ... args)
{ return insertAfter(at.pos, new(createInfo.size) LNode<T>(createInfo, std::forward<decltype(args)>(args) ...)); }

template <typename T>
template <Direction d, Constness c>
void
List<T>::remove(Iterator<d, c> at) noexcept
{
	(at.pos->prev ? at.pos->prev->next : mHead) = at.pos->next;
	(at.pos->next ? at.pos->next->prev : mTail) = at.pos->prev;
	delete at.pos;
	--mSize;
}

template <typename T>
void
List<T>::popBack() noexcept
{
	if (mTail != mHead) {
		delete (mTail = mTail->prev)->next;
		mTail->next = nullptr;
	} else {
		delete mHead;
		mTail = mHead = nullptr;
	}
	--mSize;
}

template <typename T>
void
List<T>::popFront() noexcept
{
	if (mHead != mTail) {
		delete (mHead = mHead->next)->prev;
		mHead->prev = nullptr;
	} else {
		delete mTail;
		mTail = mHead = nullptr;
	}
	--mSize;
}

template <typename T>
List<T>&
List<T>::sort() noexcept
{
	qsort(mHead, mTail, mHead, mTail);
	return *this;
}

template <typename T>
typename List<T>::iterator
List<T>::begin() noexcept
{ return mHead; }

template <typename T>
typename List<T>::const_iterator
List<T>::begin() const noexcept
{ return mHead; }

template <typename T>
typename List<T>::iterator
List<T>::end() noexcept
{ return nullptr; }

template <typename T>
typename List<T>::const_iterator
List<T>::end() const noexcept
{ return nullptr; }

template <typename T>
typename List<T>::reverse_iterator
List<T>::rbegin() noexcept
{ return mTail; }

template <typename T>
typename List<T>::const_reverse_iterator
List<T>::rbegin() const noexcept
{ return mTail; }

template <typename T>
typename List<T>::reverse_iterator
List<T>::rend() noexcept
{ return nullptr; }

template <typename T>
typename List<T>::const_reverse_iterator
List<T>::rend() const noexcept
{ return nullptr; }

template <typename T>
typename List<T>::const_iterator
List<T>::cbegin() const noexcept
{ return mHead; }

template <typename T>
typename List<T>::const_iterator
List<T>::cend() const noexcept
{ return nullptr; }

template <typename T>
typename List<T>::const_reverse_iterator
List<T>::crbegin() const noexcept
{ return mTail; }

template <typename T>
typename List<T>::const_reverse_iterator
List<T>::crend() const noexcept
{ return nullptr; }

template <typename T>
template <Direction d, Constness c>
List<T>::Iterator<d, c>::Iterator(LNode<T>* pos) noexcept
		: pos(pos)
{}

template <typename T>
template <Direction d, Constness c>
template <Direction od, Constness oc>
List<T>::Iterator<d, c>::Iterator(Iterator<od, oc> const& other) noexcept
requires ConstCompat<c, oc>
		: pos(other.pos)
{}

template <typename T>
template <Direction d, Constness c>
template <Direction od, Constness oc>
class List<T>::Iterator<d, c>&
List<T>::Iterator<d, c>::operator=(Iterator<od, oc> const& other) noexcept
requires ConstCompat<c, oc>
{
	pos = other.pos;
	return *this;
}

template <typename T>
template <Direction d, Constness c>
class List<T>::Iterator<d, c>&
List<T>::Iterator<d, c>::operator++() noexcept
{
	pos = d == Direction::FORWARD ? pos->next : pos->prev;
	return *this;
}

template <typename T>
template <Direction d, Constness c>
class List<T>::Iterator<d, c>
List<T>::Iterator<d, c>::operator++(int) noexcept
{
	auto* r = pos;
	pos = d == Direction::FORWARD ? pos->next : pos->prev;
	return r;
}

template <typename T>
template <Direction d, Constness c>
class List<T>::Iterator<d, c>&
List<T>::Iterator<d, c>::operator--() noexcept
{
	pos = d == Direction::FORWARD ? pos->prev : pos->next;
	return *this;
}

template <typename T>
template <Direction d, Constness c>
class List<T>::Iterator<d, c>
List<T>::Iterator<d, c>::operator--(int) noexcept
{
	auto* r = pos;
	pos = d == Direction::FORWARD ? pos->prev : pos->next;
	return r;
}

template <typename T>
template <Direction d, Constness c>
TConstness<T, c>&
List<T>::Iterator<d, c>::operator*() const noexcept
{ return static_cast<TConstness<T, c>&>(pos->val); }

template <typename T>
template <Direction d, Constness c>
TConstness<T, c>*
List<T>::Iterator<d, c>::operator->() const noexcept
{ return static_cast<TConstness<T, c>*>(pos->val); }

template <typename T>
template <Direction d, Constness c>
template <Direction od, Constness oc>
bool
List<T>::Iterator<d, c>::operator==(Iterator<od, oc> const& other) const noexcept
{ return pos == other.pos; }

template <typename T>
template <Direction d, Constness c>
List<T>::Iterator<d, c>::operator bool() const noexcept
{ return pos; }

}//namespace DS
