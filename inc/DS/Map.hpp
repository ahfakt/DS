#pragma once

#include "../../src/DS/Container.tpp"
#include "../../src/DS/MNode.tpp"
#include "Random.hpp"
#include <DP/Factory.hpp>
#include <Format/Dot.hpp>

namespace DS {

/**
 * @brief	Balanced multi-index search tree implementation.
 * @class	Map Map.hpp "DS/Map.hpp"
 * @tparam	K Key type of the mapped type to be stored in %Map
 * @tparam	V Value type to be mapped by K in %Map
 * @details	K and V can be any type, including abstract class
 */
template <typename K, typename V, typename C = std::less<>, typename ... Cs>
class Map : public Container<> {
	TNode<sizeof...(Cs) + 1>* mRoot[sizeof...(Cs) + 1] = {};

	template <std::size_t N = 0>
	MNode<K, V, C, Cs ...>*
	putAsRoot(MNode<K, V, C, Cs ...>* created) noexcept;

	template <std::size_t N = 0>
	MNode<K, V, C, Cs ...>*
	putToRoot(MNode<K, V, C, Cs ...>* created) noexcept;

	MNode<K, V, C, Cs ...>*
	put(MNode<K, V, C, Cs ...>* created) noexcept;

	template <std::size_t N = 0>
	MNode<K, V, C, Cs ...>*
	remove(MNode<K, V, C, Cs ...>* toDel) noexcept;

	template <std::size_t N = 0>
	Format::DotOutput&
	toDot(Format::DotOutput& dotOutput) const
	requires
		Stream::InsertableTo<K, decltype(dotOutput)> &&
		Stream::InsertableTo<V, decltype(dotOutput)>;

public:
	template <Direction, Constness, std::size_t N = 0>
	class Iterator;

	template <std::size_t N = 0>
	using iterator = Iterator<Direction::FORWARD, Constness::NCONST, N>;

	template <std::size_t N = 0>
	using reverse_iterator = Iterator<Direction::BACKWARD, Constness::NCONST, N>;

	template <std::size_t N = 0>
	using const_iterator = Iterator<Direction::FORWARD, Constness::CONST, N>;

	template <std::size_t N = 0>
	using const_reverse_iterator = Iterator<Direction::BACKWARD, Constness::CONST, N>;

	struct Exception : std::system_error
	{ using std::system_error::system_error; };

	template <std::size_t N = 0>
	struct Difference {
		Map
		operator()(Map const& a, Map const& b) const;
	};//struct DS::Map<K, V, C, Cs ...>::Difference<N>

	template <typename S, std::size_t N = 0>
	struct Intersection {
		Map
		operator()(Map const& a, Map const& b, auto&& ... args) const
		requires Selector<S, K, decltype(args) ...>;
	};//struct DS::Map<K, V, C, Cs ...>::Intersection<S, N>

	template <typename S, std::size_t N = 0>
	struct Join {
		Map
		operator()(Map const& a, Map const& b, auto&& ... args) const
		requires Selector<S, K, decltype(args) ...>;
	};//struct DS::Map<K, V, C, Cs ...>::Join<S, N>

	template <typename S, std::size_t N = 0>
	struct Union {
		Map
		operator()(Map const& a, Map const& b, auto&& ... args) const
		requires Selector<S, K, decltype(args) ...>;
	};//struct DS::Map<K, V, C, Cs ...>::Union<S, N>

	template <typename k, typename v>
	struct Entry {
		k key;
		v value;
		auto operator<=>(Entry const& other) const = default;
	};//struct DS::Map<K, V, C, Cs ...>::Entry<k, v>

	/**
	 * @brief	Default constructor
	 */
	Map() noexcept = default;

	/**
	 * @brief	Copy constructor
	 */
	Map(Map const& other)
	requires std::is_copy_constructible_v<K> && std::is_copy_constructible_v<V>;

	/**
	 * @brief	Move constructor
	 */
	Map(Map&& other) noexcept;

	/**
	 * @brief	Swap
	 */
	template <typename k, typename v, typename c, typename ... cs>
	friend void
	swap(Map<k, v, c, cs ...>& a, Map<k, v, c, cs ...>& b) noexcept;

	/**
	 * @brief	Value assignment
	 */
	Map&
	operator=(Map value) noexcept;

	/**
	 * @brief	Construct K(input), V(input)
	 * @param	input Stream::Input to be used to deserialize from
	 */
	explicit Map(Stream::Input& input)
	requires
		Stream::Deserializable<K, decltype(input)> &&
		Stream::Deserializable<V, decltype(input)>;

	/**
	 * @brief	Construct K(input), V (input, vArgs ...)
	 * @tparam	VArgs Deduced types of the vArgs
	 * @param	input Stream::Input to be used to deserialize from
	 * @param	vArgs Trailing parameters to be passed for every V along with the input
	 */
	template <typename ... VArgs>
	Map(Stream::Input& input, Pack<VArgs ...> vArgs)
	requires
		Stream::Deserializable<K, decltype(input)> &&
		Stream::Deserializable<V, decltype(input), VArgs ...>;

	/**
	 * @brief	Construct K(input, kArgs ...), V(input)
	 * @tparam	KArgs Deduced types of the kArgs
	 * @param	kArgs Trailing parameters to be passed for every K along with the input
	 * @param	input Stream::Input to be used to deserialize from
	 */
	template <typename ... KArgs>
	Map(Pack<KArgs ...> kArgs, Stream::Input& input)
	requires
		Stream::Deserializable<K, decltype(input), KArgs ...> &&
		Stream::Deserializable<V, decltype(input)>;

	/**
	 * @brief	Construct K(input, kArgs ...), V(input, vArgs ...)
	 * @tparam	KArgs Deduced types of the kArgs
	 * @tparam	VArgs Deduced types of the vArgs
	 * @param	kArgs Trailing parameters to be passed for every K along with the input
	 * @param	input Stream::Input to be used to deserialize from
	 * @param	vArgs Trailing parameters to be passed for every V along with the input
	 */
	template <typename ... KArgs, typename ... VArgs>
	Map(Pack<KArgs ...> kArgs, Stream::Input& input, Pack<VArgs ...> vArgs)
	requires
		Stream::Deserializable<K, decltype(input), KArgs ...> &&
		Stream::Deserializable<V, decltype(input), VArgs ...>;

/*
	// TODO
	template <typename VType, typename ... VFArgs, typename ... VArgs>
	Map(Stream::Input& input, DP::Factory<V, VType, VFArgs ...> vFactory, Pack<VArgs ...> vArgs)
	requires
		Stream::Deserializable<K, decltype(input)> &&
		Stream::Deserializable<V, decltype(input), VFArgs ...>;


	template <typename VType, typename ... KArgs, typename ... VArgs>
	Map(Pack<KArgs ...> kArgs, Stream::Input& input, DP::Factory<V, VType, VArgs ...>, auto&& ... vArgs)
	requires Stream::Deserializable<K, decltype(input), decltype(kArgs) ...>;

	template <typename KType, typename ... KArgs>
	Map(DP::Factory<K, KType, KArgs ...>, auto&& ... kArgs, Stream::Input& input, auto&& ... vArgs)
	requires Stream::Deserializable<V, decltype(input), decltype(vArgs) ...>;

	template <typename KType, typename ... KArgs, typename VType, typename ... VArgs>
	Map(DP::Factory<K, KType, KArgs ...>, auto&& ... kArgs, Stream::Input& input, DP::Factory<V, VType, VArgs ...>, auto&& ... vArgs);
*/
	template <typename k, typename v, typename c, typename ... cs>
	friend Stream::Output&
	operator<<(Stream::Output& output, Map<k, v, c, cs ...> const& map)
	requires
		Stream::InsertableTo<k, decltype(output)> &&
		Stream::InsertableTo<v, decltype(output)>;

	template <typename k, typename v, typename c, typename ... cs>
	friend Format::DotOutput&
	operator<<(Format::DotOutput& dotOutput, Map<k, v, c, cs ...> const& map)
	requires
		Stream::InsertableTo<k, decltype(dotOutput)> &&
		Stream::InsertableTo<v, decltype(dotOutput)>;

	/**
	 * @brief	Destructor
	 */
	~Map();

	/**
	 * @brief	Construct K with kArgs.
	 */
	iterator<>
	put(auto&& ... kArgs);

	/**
	 * @brief	Construct DK with dkArgs.
	 */
	template <EqDerived<K> DK>
	iterator<>
	put(auto&& ... dkArgs);

	/**
	 * @brief	Construct K with kCreateInfo and kArgs.
	 * @throws	Map::Exception
	 */
	template <typename ... KArgs>
	iterator<>
	put(DP::CreateInfo<K, KArgs ...> const& kCreateInfo, auto&& ... kArgs);


	/**
	 * @brief	Allocate enough memory for DV and construct K with kArgs.
	 */
	template <Derived<V> DV>
	iterator<>
	putDV(auto&& ... kArgs);

	/**
	 * @brief	Allocate enough memory for DV and construct DK with dkArgs.
	 */
	template <EqDerived<K> DK, Derived<V> DV>
	iterator<>
	putDV(auto&& ... dkArgs);

	/**
	 * @brief	Allocate enough memory for DV and construct K with kCreateInfo and kArgs.
	 * @throws	Map::Exception
	 */
	template <typename ... KArgs, Derived<V> DV>
	iterator<>
	putDV(DP::CreateInfo<K, KArgs ...> const& kCreateInfo, auto&& ... kArgs);


	/**
	 * @brief	Allocate as much memory for V as specified in the vCreateInfo.size and construct K with kArgs.
	 */
	template <typename ... VArgs>
	iterator<>
	putFV(DP::CreateInfo<V, VArgs ...> const& vCreateInfo, auto&& ... kArgs);

	/**
	 * @brief	Allocate as much memory for V as specified in the vCreateInfo.size and construct DK with dkArgs.
	 */
	template <EqDerived<K> DK, typename ... VArgs>
	iterator<>
	putFV(DP::CreateInfo<V, VArgs ...> const& vCreateInfo, auto&& ... dkArgs);

	/**
	 * @brief	Allocate as much memory for V as specified in the vCreateInfo.size and construct K with kCreateInfo and kArgs.
	 * @throws	Map::Exception
	 */
	template <typename ... KArgs, typename ... VArgs>
	iterator<>
	putFV(DP::CreateInfo<V, VArgs ...> const& vCreateInfo, DP::CreateInfo<K, KArgs ...> const& kCreateInfo, auto&& ... kArgs);


	template <Direction d, Constness c, std::size_t N = 0>
	bool
	remove(Iterator<d, c, N> i) noexcept;

	template <std::size_t N = 0>
	bool
	remove(auto&& ... args) noexcept;

	template <std::size_t N = 0>
	iterator<N>
	get(auto&& ... args) noexcept;

	template <std::size_t N = 0>
	const_iterator<N>
	get(auto&& ... args) const noexcept;

	template <std::size_t N = 0>
	iterator<N>
	at(std::uint64_t i) noexcept;

	template <std::size_t N = 0>
	const_iterator<N>
	at(std::uint64_t i) const noexcept;

	template <std::size_t N = 0>
	iterator<N>
	begin() noexcept;

	template <std::size_t N = 0>
	const_iterator<N>
	begin() const noexcept;

	template <std::size_t N = 0>
	iterator<N>
	end() noexcept;

	template <std::size_t N = 0>
	const_iterator<N>
	end() const noexcept;

	template <std::size_t N = 0>
	reverse_iterator<N>
	rbegin() noexcept;

	template <std::size_t N = 0>
	const_reverse_iterator<N>
	rbegin() const noexcept;

	template <std::size_t N = 0>
	reverse_iterator<N>
	rend() noexcept;

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
};//class DS::Map<K, V, C, Cs ...>

template <typename K, typename V, typename C, typename ... Cs>
template <Direction d, Constness c, std::size_t n>
class Map<K, V, C, Cs ...>::Iterator {
	friend class Map;

protected:
	TNode<sizeof...(Cs) + 1>* pos;

	Iterator(TNode<sizeof...(Cs) + 1>* pos) noexcept;

public:
	using Entry = Map<K, V, C, Cs ...>::Entry<
		std::conditional_t<std::is_abstract_v<K>, Raw<K>, K> const,
		TConstness<std::conditional_t<std::is_abstract_v<V>, Raw<V>, V>, c>>;

	template <Direction od, Constness oc, std::size_t on>
	Iterator(Iterator<od, oc, on> const& other) noexcept
	requires ConstCompat<c, oc>;

	template <Direction od, Constness oc, std::size_t on>
	Iterator&
	operator=(Iterator<od, oc, on> const& other) noexcept
	requires ConstCompat<c, oc>;

	[[nodiscard]] bool
	hasValue() const noexcept;

	void
	unset() const noexcept
	requires (c == Constness::NCONST);

	V&
	set(auto&& ... vArgs) const
	requires (c == Constness::NCONST);

	template <Derived<V> DV>
	DV&
	set(auto&& ... dvArgs) const
	requires (c == Constness::NCONST);

	Iterator&
	operator++() noexcept;

	Iterator
	operator++(int) noexcept;

	Iterator&
	operator--() noexcept;

	Iterator
	operator--(int) noexcept;

	Entry&
	operator*() const noexcept;

	Entry*
	operator->() const noexcept;

	template <Direction od, Constness oc, std::size_t on>
	bool
	operator==(Iterator<od, oc, on> const& other) const noexcept;

	explicit operator bool() const noexcept;
};//class DS::Map<K, V, C, Cs ...>::Iterator<Direction, Constness, std::size_t>

}//namespace DS

#include "../../src/DS/Map.tpp"
