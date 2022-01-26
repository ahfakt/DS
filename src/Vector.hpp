#include "DS/Vector.h"

namespace DS {

template <typename T>
Vector<T>::Vector(std::uint64_t reserve)
		: Container(0)
		, mHead(reserve ? reinterpret_cast<Holder<T>*>(::operator new(sizeof(T) * reserve)) : nullptr)
		, mCapacity(reserve)
{}

template <typename T>
Vector<T>::Vector(Vector const& other)
requires std::is_copy_constructible_v<T>
		: Vector(other.mSize)
{
	if (mCapacity) {
		Holder<T> const* src = other.mHead;
		do {
			try {
				::new(static_cast<void*>(mHead + mSize)) Holder<T>(static_cast<T const&>(*src));
			} catch (...) {
				this->~Vector();
				throw;
			}
			++src;
		} while (++mSize < mCapacity);
	}
}

template <typename T>
Vector<T>::Vector(Vector&& other) noexcept
{ swap(*this, other); }

template <typename T>
void
swap(Vector<T>& a, Vector<T>& b) noexcept
{
	std::swap(a.mSize, b.mSize);
	std::swap(a.mHead, b.mHead);
	std::swap(a.mCapacity, b.mCapacity);
}

template <typename T>
Vector<T>&
Vector<T>::operator=(Vector value) noexcept
{
	swap(*this, value);
	return *this;
}

template <typename T>
template <typename ... TArgs>
Vector<T>::Vector(Stream::Input& input, TArgs&& ... tArgs)
requires Deserializable<T, Stream::Input, TArgs ...>
		: Vector(Stream::Get<std::uint64_t>(input))
{
	while (mSize < mCapacity) {
		try {
			::new(static_cast<void*>(mHead + mSize)) Holder<T>(input, std::forward<TArgs>(tArgs) ...);
		} catch (...) {
			this->~Vector();
			throw;
		}
		++mSize;
	}
}

template <typename T>
template <typename IDType, typename ... FArgs>
Vector<T>::Vector(Stream::Input& input, DP::Factory<T, IDType, FArgs ...> const&)
		: Vector(Stream::Get<std::uint64_t>(input))
{
	while (mSize < mCapacity) {
		try {
			auto const& createInfo = DP::Factory<T, IDType, FArgs ...>::GetCreateInfo({Stream::Get<IDType>(input)});
			if (createInfo.size > sizeof(T))
				throw Exception("createInfo.size is greater than the size of T.");
			::new(static_cast<void*>(mHead + mSize)) Holder<T>(createInfo.create, Stream::Get<std::remove_cvref_t<FArgs>>(input) ...);
		} catch (...) {
			this->~Vector();
			throw;
		}
		++mSize;
	}
}

template <typename T>
Stream::Output&
operator<<(Stream::Output& output, Vector<T> const& vector)
requires Stream::Serializable<T, Stream::Output>
{
	output << vector.mSize;
	Holder<T> const* beg = vector.mHead;
	Holder<T> const* const end = beg + vector.mSize;
	for (; beg < end; ++beg)
		output << static_cast<T const&>(*beg);
	return output;
}

template <typename T>
Vector<T>::~Vector()
{
	while (mSize)
		reinterpret_cast<T*>(mHead + --mSize)->~T();
	::operator delete(mHead);
}

template <typename T>
template <typename ... TArgs>
typename Vector<T>::iterator
Vector<T>::pushBack(TArgs&& ... tArgs)
{
	if (mSize < mCapacity) {
		::new(static_cast<void*>(mHead + mSize)) Holder<T>(std::forward<TArgs>(tArgs) ...);
		return mHead + mSize++;
	}
	Vector v(mCapacity > 1 ? 1.5 * mCapacity : 2);
	::new(static_cast<void*>(v.mHead + mSize)) Holder<T>(std::forward<TArgs>(tArgs) ...);
	try {
		for (std::uint64_t i = 0; i < mSize; ++i)
			v.pushBack(std::move_if_noexcept(*reinterpret_cast<T*>(mHead + i)));
	} catch (...) {
		reinterpret_cast<T*>(v.mHead + mSize)->~T();
		throw;
	}

	swap(*this, v);
	return mHead + mSize++;
}

template <typename T>
template <EqDerived<T> DT, typename ... DTArgs>
typename Vector<T>::iterator
Vector<T>::pushBack(DTArgs&& ... dtArgs)
{
	if (mSize < mCapacity) {
		::new(static_cast<void*>(mHead + mSize)) Holder<DT>(std::forward<DTArgs>(dtArgs) ...);
		return mHead + mSize++;
	}
	Vector v(mCapacity > 1 ? 1.5 * mCapacity : 2);
	::new(static_cast<void*>(v.mHead + mSize)) Holder<DT>(std::forward<DTArgs>(dtArgs) ...);
	try {
		for (std::uint64_t i = 0; i < mSize; ++i)
			v.pushBack(std::move_if_noexcept(*reinterpret_cast<T*>(mHead + i)));
	} catch (...) {
		reinterpret_cast<DT*>(v.mHead + mSize)->~D();
		throw;
	}

	swap(*this, v);
	return mHead + mSize++;
}

template <typename T>
template <typename ... CIArgs, typename ... CArgs>
typename Vector<T>::iterator
Vector<T>::pushBack(DP::CreateInfo<T, CIArgs ...> const& createInfo, CArgs&& ... cArgs)
{
	if (createInfo.size > sizeof(T))
		throw Exception("createInfo.size is greater than the size of T.");

	if (mSize < mCapacity) {
		::new(static_cast<void*>(mHead + mSize)) Holder<T>(createInfo.create, std::forward<CArgs>(cArgs) ...);
		return mHead + mSize++;
	}
	Vector v(mCapacity > 1 ? mCapacity * 1.5 : 2);
	new(static_cast<void*>(v.mHead + mSize)) Holder<T>(createInfo.create, std::forward<CArgs>(cArgs) ...);
	try {
		for (std::uint64_t i = 0; i < mSize; ++i)
			v.pushBack(std::move_if_noexcept(*reinterpret_cast<T*>(mHead + i)));
	} catch (...) {
		reinterpret_cast<T*>(v.mHead + mSize)->~T();
		throw;
	}

	swap(*this, v);
	return mHead + mSize++;
}

template <typename T>
std::uint64_t
Vector<T>::capacity() const noexcept
{ return mCapacity; }

template <typename T>
void
Vector<T>::resize(std::uint64_t i) noexcept
{ mSize = i; }

template <typename T>
T&
Vector<T>::operator[](std::uint64_t i) noexcept
{ return *reinterpret_cast<T*>(mHead + i); }

template <typename T>
T const&
Vector<T>::operator[](std::uint64_t i) const noexcept
{ return *reinterpret_cast<T const*>(mHead + i); }

template <typename T>
T*
Vector<T>::operator+(std::uint64_t i) noexcept
{ return reinterpret_cast<T*>(mHead + i); }

template <typename T>
T const*
Vector<T>::operator+(std::uint64_t i) const noexcept
{ return reinterpret_cast<T const*>(mHead + i); }

template <typename T>
Vector<T>::operator T*() noexcept
{ return reinterpret_cast<T*>(mHead); }

template <typename T>
Vector<T>::operator T const*() const noexcept
{ return reinterpret_cast<T const*>(mHead); }

template <typename T>
typename Vector<T>::iterator
Vector<T>::begin() noexcept
{ return mHead; }

template <typename T>
typename Vector<T>::const_iterator
Vector<T>::begin() const noexcept
{ return mHead; }

template <typename T>
typename Vector<T>::iterator
Vector<T>::end() noexcept
{ return mHead + mSize; }

template <typename T>
typename Vector<T>::const_iterator
Vector<T>::end() const noexcept
{ return mHead + mSize; }

template <typename T>
typename Vector<T>::reverse_iterator
Vector<T>::rbegin() noexcept
{ return mHead + mSize - 1; }

template <typename T>
typename Vector<T>::const_reverse_iterator
Vector<T>::rbegin() const noexcept
{ return mHead + mSize - 1; }

template <typename T>
typename Vector<T>::reverse_iterator
Vector<T>::rend() noexcept
{ return mHead - 1; }

template <typename T>
typename Vector<T>::const_reverse_iterator
Vector<T>::rend() const noexcept
{ return mHead - 1; }

template <typename T>
typename Vector<T>::const_iterator
Vector<T>::cbegin() const noexcept
{ return mHead; }

template <typename T>
typename Vector<T>::const_iterator
Vector<T>::cend() const noexcept
{ return mHead + mSize; }

template <typename T>
typename Vector<T>::const_reverse_iterator
Vector<T>::crbegin() const noexcept
{ return mHead + mSize - 1; }

template <typename T>
typename Vector<T>::const_reverse_iterator
Vector<T>::crend() const noexcept
{ return mHead - 1; }

template <typename T>
template <Direction d, Constness c>
Vector<T>::Iterator<d, c>::Iterator(Holder<T>* pos) noexcept
		: pos(reinterpret_cast<T*>(pos))
{}

template <typename T>
template <Direction d, Constness c>
template <Direction od, Constness oc>
Vector<T>::Iterator<d, c>::Iterator(Iterator<od, oc> const& other) noexcept
requires ConstCompat<c, oc>
		: pos(other.pos)
{}

template <typename T>
template <Direction d, Constness c>
template <Direction od, Constness oc>
class Vector<T>::Iterator<d, c>&
Vector<T>::Iterator<d, c>::operator=(Iterator<od, oc> const& other) noexcept
requires ConstCompat<c, oc>
{
	pos = other.pos;
	return *this;
}

template <typename T>
template <Direction d, Constness c>
class Vector<T>::Iterator<d, c>&
Vector<T>::Iterator<d, c>::operator++() noexcept
{
	d == Direction::FORWARD ? ++pos : --pos;
	return *this;
}

template <typename T>
template <Direction d, Constness c>
class Vector<T>::Iterator<d, c>
Vector<T>::Iterator<d, c>::operator++(int) noexcept
{ return d == Direction::FORWARD ? pos++ : pos--; }

template <typename T>
template <Direction d, Constness c>
class Vector<T>::Iterator<d, c>&
Vector<T>::Iterator<d, c>::operator--() noexcept
{
	d == Direction::FORWARD ? --pos : ++pos;
	return *this;
}

template <typename T>
template <Direction d, Constness c>
class Vector<T>::Iterator<d, c>
Vector<T>::Iterator<d, c>::operator--(int) noexcept
{ return d == Direction::FORWARD ? pos-- : pos++; }

template <typename T>
template <Direction d, Constness c>
TConstness<T, c>&
Vector<T>::Iterator<d, c>::operator*() const noexcept
{ return *pos; }

template <typename T>
template <Direction d, Constness c>
TConstness<T, c>*
Vector<T>::Iterator<d, c>::operator->() const noexcept
{ return pos; }

template <typename T>
template <Direction d, Constness c>
template <Direction od, Constness oc>
bool
Vector<T>::Iterator<d, c>::operator==(Iterator<od, oc> const& other) const noexcept
{ return pos == other.pos; }

template <typename T>
template <Direction d, Constness c>
Vector<T>::Iterator<d, c>::operator bool() const noexcept
{ return pos; }

}//namespace DS
