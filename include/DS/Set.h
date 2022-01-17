#ifndef DS_SET_H
#define DS_SET_H

#include "DS/Container.h"
#include "DP/Factory.h"
#include "../src/SNode.hpp"

namespace DS {

/**
 * @brief	Balanced search tree implementation.
 * @class	Set Set.h "DS/Set.h"
 * @tparam	K Key type to be stored in Set
 * @details	K and any K-based objects can be stored in a Set object simultaneously, even if K is an abstract class.
 */
template <typename K>
class Set : public Container {
	SNode<K>* mRoot = nullptr;

	SNode<K>*
	first() const noexcept;

	SNode<K>*
	last() const noexcept;

	SNode<K>*
	put(SNode<K>* created) noexcept;

public:
	template <Direction>
	class Iterator;

	using const_iterator = Iterator<Direction::FORWARD>;
	using const_reverse_iterator = Iterator<Direction::BACKWARD>;

	Set() noexcept = default;

	Set(Set const& other)
	requires std::is_copy_constructible_v<K>;

	Set(Set&& other) noexcept;

	template <typename k>
	friend void
	swap(Set<k>& a, Set<k>& b) noexcept;

	Set&
	operator=(Set value) noexcept;

	template <typename ... KArgs>
	explicit Set(Stream::Input& input, KArgs&& ... kArgs)
	requires Deserializable<K, Stream::Input, KArgs ...>;

	template <typename IDType, typename ... FArgs>
	Set(Stream::Input& input, DP::Factory<K, IDType, FArgs ...> const& factory);

	template <typename k>
	friend Stream::Output&
	operator<<(Stream::Output& output, Set<k> const& set)
	requires Stream::Serializable<k, Stream::Output>;

	/*
	Set&
	toDot(Stream::Output&)
	requires Serializable<K>;
	 */

	~Set();

	template <typename ... KArgs>
	const_iterator
	put(KArgs&& ... kArgs);

	template <Derived<K> DK, typename ... DKArgs>
	const_iterator
	put(DKArgs&& ... dkArgs);

	template <typename ... CIArgs, typename ... CArgs>
	const_iterator
	put(DP::CreateInfo<K, CIArgs ...> const& createInfo, CArgs&& ... cArgs);

	template <typename T>
	bool
	remove(T&& k) noexcept;

	template <typename T>
	const_iterator
	get(T&& k) const noexcept;

	const_iterator
	operator[](std::uint64_t i) const noexcept;

	const_iterator
	begin() const noexcept;

	const_iterator
	end() const noexcept;

	const_reverse_iterator
	rbegin() const noexcept;

	const_reverse_iterator
	rend() const noexcept;

	const_iterator
	cbegin() const noexcept;

	const_iterator
	cend() const noexcept;

	const_reverse_iterator
	crbegin() const noexcept;

	const_reverse_iterator
	crend() const noexcept;
};//class Set<K>

template <typename K>
template <Direction d>
class Set<K>::Iterator {
	friend class Set;

protected:
	SNode<K>* pos;

	Iterator&
	inc() noexcept;

	Iterator&
	dec() noexcept;

	Iterator(SNode<K>* pos) noexcept;

public:
	template <Direction od>
	Iterator(Iterator<od> const& other) noexcept;

	template <Direction od>
	Iterator&
	operator=(Iterator<od> const& other) noexcept;

	Iterator&
	operator++() noexcept;

	Iterator
	operator++(int) noexcept;

	Iterator&
	operator--() noexcept;

	Iterator
	operator--(int) noexcept;

	K const&
	operator*() const noexcept;

	K const*
	operator->() const noexcept;

	template <Direction od>
	bool
	operator==(Iterator<od> const& other) const noexcept;

	explicit operator bool() const noexcept;
};//class Set<K>::Iterator<Direction>

}//namespace DS

#include "../src/Set.hpp"

#endif //DS_SET_H
