#ifndef DS_SET_HPP
#define DS_SET_HPP

#include "Container.hpp"
#include "../../src/DS/SNode.tpp"
#include <StreamFormat/Dot.hpp>

namespace DS {

/**
 * @brief	Balanced multi-index search tree implementation.
 * @class	Set Set.hpp "DS/Set.hpp"
 * @tparam	K Key type to be stored in Set
 * @tparam	C Primary comparator
 * @tparam	Cs Other comparators
 * @details	K can be any type, including abstract class
 */
template <typename K, typename C = std::less<>, typename ... Cs>
class Set : public Container {
	TNode<sizeof...(Cs) + 1>* mRoot[sizeof...(Cs) + 1] = {};

	template <std::size_t N = 0>
	SNode<K, C, Cs ...>*
	putAsRoot(SNode<K, C, Cs ...>* created) noexcept;

	template <std::size_t N = 0>
	SNode<K, C, Cs ...>*
	putToRoot(SNode<K, C, Cs ...>* created) noexcept;

	SNode<K, C, Cs ...>*
	put(SNode<K, C, Cs ...>* created) noexcept;

	template <std::size_t N = 0>
	SNode<K, C, Cs ...>*
	remove(SNode<K, C, Cs ...>* toDel) noexcept;

	template <std::size_t N = 0>
	Stream::Format::DotOutput&
	toDot(Stream::Format::DotOutput& dotOutput) const
	requires Stream::InsertableTo<K, decltype(dotOutput)>;

public:
	template <Direction, std::size_t N = 0>
	class Iterator;

	template <std::size_t N = 0>
	using const_iterator = Iterator<Direction::FORWARD, N>;

	template <std::size_t N = 0>
	using const_reverse_iterator = Iterator<Direction::BACKWARD, N>;

	struct Exception : std::system_error
	{ using std::system_error::system_error; };

	template <std::size_t N = 0>
	struct Difference {
		Set
		operator()(Set const& a, Set const& b) const;
	};//struct DS::Set<K, C, Cs ...>::Difference<N>

	template <typename S, std::size_t N = 0>
	struct Intersection {
		Set
		operator()(Set const& a, Set const& b, auto&& ... args) const
		requires Selector<S, K, decltype(args) ...>;
	};//struct DS::Set<K, C, Cs ...>::Intersection<S, N>

	template <typename S, std::size_t N = 0>
	struct LeftJoin {
		Set
		operator()(Set const& a, Set const& b, auto&& ... args) const
		requires Selector<S, K, decltype(args) ...>;
	};//struct DS::Set<K, C, Cs ...>::LeftJoin<S, N>

	template <typename S, std::size_t N = 0>
	struct Union {
		Set
		operator()(Set const& a, Set const& b, auto&& ... args) const
		requires Selector<S, K, decltype(args) ...>;
	};//struct DS::Set<K, C, Cs ...>::Union<S, N>

	Set() noexcept = default;

	Set(Set const& other)
	requires std::is_copy_constructible_v<K>;

	Set(Set&& other) noexcept;

	template <typename k, typename c, typename ... cs>
	friend void
	swap(Set<k, c, cs ...>& a, Set<k, c, cs ...>& b) noexcept;

	Set&
	operator=(Set value) noexcept;

	explicit Set(Stream::Input& input, auto&& ... kArgs)
	requires Stream::DeserializableWith<K, decltype(input), decltype(kArgs) ...>;

	template <typename ID, typename ... Args>
	Set(Stream::Input& input, DP::Factory<K, ID, Args ...>, auto&& ... kArgs);

	template <typename k, typename c, typename ... cs>
	friend Stream::Output&
	operator<<(Stream::Output& output, Set<k, c, cs ...> const& set)
	requires Stream::InsertableTo<k, decltype(output)>;

	template <typename k, typename c, typename ... cs>
	friend Stream::Format::DotOutput&
	operator<<(Stream::Format::DotOutput& dotOutput, Set<k, c, cs ...> const& set)
	requires Stream::InsertableTo<k, decltype(dotOutput)>;

	~Set();

	const_iterator<>
	put(auto&& ... kArgs);

	template <Derived<K> DK>
	const_iterator<>
	put(auto&& ... dkArgs);

	template <typename ... Args>
	const_iterator<>
	put(DP::CreateInfo<K, Args ...> const& createInfo, auto&& ... args);

	template <Direction d, std::size_t N = 0>
	bool
	remove(Iterator<d, N> i) noexcept;

	template <std::size_t N = 0>
	bool
	remove(auto&& ... args) noexcept;

	template <std::size_t N = 0>
	const_iterator<N>
	get(auto&& ... args) const noexcept;

	template <std::size_t N = 0>
	const_iterator<N>
	at(std::uint64_t i) const noexcept;

	template <std::size_t N = 0>
	const_iterator<N>
	begin() const noexcept;

	template <std::size_t N = 0>
	const_iterator<N>
	end() const noexcept;

	template <std::size_t N = 0>
	const_reverse_iterator<N>
	rbegin() const noexcept;

	template <std::size_t N = 0>
	const_reverse_iterator<N>
	rend() const noexcept;

	template <std::size_t N = 0>
	const_iterator<N>
	cbegin() const noexcept;

	template <std::size_t N = 0>
	const_iterator<N>
	cend() const noexcept;

	template <std::size_t N = 0>
	const_reverse_iterator<N>
	crbegin() const noexcept;

	template <std::size_t N = 0>
	const_reverse_iterator<N>
	crend() const noexcept;
};//class DS::Set<K, C, Cs ...>

template <typename K, typename C, typename ... Cs>
template <Direction d, std::size_t n>
class Set<K, C, Cs ...>::Iterator {
	friend class Set;

protected:
	TNode<sizeof...(Cs) + 1>* pos;

	Iterator(TNode<sizeof...(Cs) + 1>* pos) noexcept;

public:
	template <Direction od, std::size_t on>
	explicit Iterator(Iterator<od, on> const& other) noexcept;

	template <Direction od, std::size_t on>
	Iterator&
	operator=(Iterator<od, on> const& other) noexcept;

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

	template <Direction od, std::size_t on>
	bool
	operator==(Iterator<od, on> const& other) const noexcept;

	explicit operator bool() const noexcept;
};//class DS::Set<K, C, Cs ...>::Iterator<Direction, std::size_t>

}//namespace DS

#include "../../src/DS/Set.tpp"

#endif //DS_SET_HPP
