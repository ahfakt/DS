#pragma once

#include <DP/CreateInfo.hpp>
#include <Stream/InOut.hpp>
#include <random>
#include <utility>

namespace DS {

template <typename T>
struct typeOf
{ using type = void; };

template <typename T, typename M>
struct typeOf<M T::*>
{ using type = M; };

// Type of member
template <auto MemberPtr>
using TypeOf = typename typeOf<decltype(MemberPtr)>::type;


template <std::size_t i, typename T, typename ... U>
struct typeAt
{ using type = typename typeAt<i - 1, U ...>::type; };

template <typename T, typename ... U>
struct typeAt<0, T, U ...>
{ using type = T; };

// Type of T[i]
template <std::size_t i, typename ... T>
using TypeAt = typename typeAt<i, T ...>::type;


template <std::size_t i, auto V, auto ... U>
struct valueAt
{ static inline constexpr TypeAt<i - 1, decltype(U) ...> value = valueAt<i - 1, U ...>::value; };

template <auto V, auto ... U>
struct valueAt<0, V, U ...>
{ static inline constexpr decltype(V) value = V; };

// Value of non-type V[i]
template <std::size_t i, auto ... V>
inline constexpr TypeAt<i, decltype(V) ...> ValueAt = valueAt<i, V ...>::value;


template <typename T, typename ... U>
struct Pack
{
	T value;
	Pack<U ...> next;
};

template <typename T>
struct Pack<T>
{
	T value;
};

template <std::size_t I>
struct getAt {
	template <typename ... T>
	static TypeAt<I, T ...>& get(Pack<T ...>& p)
	{ return getAt<I - 1>::get(p.next); }
};

template <>
struct getAt<0> {
	template <typename ... T>
	static TypeAt<0, T ...>& get(Pack<T ...>& p)
	{ return p.value; }
};

template <std::size_t I, typename ... T>
TypeAt<I, T ...>&
GetAt(Pack<T ...>& p)
{ return getAt<I>::get(p); }


template <typename T>
class alignas(T) Raw {
protected:
	std::byte raw[sizeof(T)];

public:
	T*
	operator->() noexcept
	{ return reinterpret_cast<T*>(raw); }

	T const*
	operator->() const noexcept
	{ return reinterpret_cast<T const*>(raw); }

	T&
	operator*() noexcept
	{ return *reinterpret_cast<T*>(raw); }

	T const&
	operator*() const noexcept
	{ return *reinterpret_cast<T const*>(raw); }

	operator T&() noexcept
	{ return *reinterpret_cast<T*>(raw); }

	operator T const&() const noexcept
	{ return *reinterpret_cast<T const*>(raw); }

	explicit operator T*() noexcept
	{ return reinterpret_cast<T*>(raw); }

	explicit operator T const*() const noexcept
	{ return reinterpret_cast<T const*>(raw); }

	explicit operator void*() noexcept
	{ return raw; }

	explicit operator void const*() const noexcept
	{ return raw; }

	auto
	operator<=>(Raw const& other) const noexcept
	requires requires(T const& a, T const& b) { a <=> b; }
	{ return *reinterpret_cast<T const*>(raw) <=> *reinterpret_cast<T const*>(other.raw); }
};//class DS::Raw<T>

template <typename T>
struct Holder : Raw<T> {
	Holder() noexcept = default;

	explicit Holder(auto&& ... args)
	requires Stream::InitializableFrom<T, decltype(args) ...>
	{ ::new(this) T{std::forward<decltype(args)>(args) ...}; }

	explicit Holder(Stream::Source auto& input, auto&& ... args)
	requires Stream::InitializableExtractableFrom<T, decltype(input), decltype(args) ...>
	{ input >> *::new(this) T{std::forward<decltype(args)>(args) ...}; }

	explicit Holder(Stream::Source auto& input)
	requires Stream::TriviallyExtractableFrom<T, decltype(input)>
	{ input >> *reinterpret_cast<T*>(this); }

	template <typename ... Args, std::size_t ... I>
	explicit Holder(Stream::Source auto& input, Pack<Args ...>& args, std::index_sequence<I ...>)
			: Holder(input, GetAt<I>(args) ...) {}

	template <typename ... Args>
	explicit Holder(DP::CreateInfo<T, Args ...> const& createInfo, auto&& ... args)
	{ createInfo.constructor(this, std::forward<decltype(args)>(args) ...); }

	template <typename ... Args, std::size_t ... I>
	explicit Holder(DP::CreateInfo<T, Args ...> const& createInfo, Stream::Source auto& input, std::index_sequence<I ...>, auto&& ... args)
	{ createInfo.constructor(this, Stream::Get<std::remove_cvref_t<TypeAt<I, Args ...>>>(input) ..., std::forward<decltype(args)>(args) ...); }
};//struct DS::Holder<T>

///enums

/**
 * @brief	The enum class used to specify the constness of the value accessed to by iterators.
 * @enum	Constness
 */
enum class Constness : bool {
	NCONST	= false,
	CONST	= true
};

/**
 * @brief	The enum class used to specify the direction of iterators.
 * @enum	Direction
 */
enum class Direction : bool {
	FORWARD = false,
	BACKWARD = true
};

enum class Traverse : bool {
	BREADTH_FIRST	= false,
	DEPTH_FIRST		= true
};

enum class Order : bool {
	PREORDER	= false,
	POSTORDER	= true
};

///concepts

template <typename D, typename T>
concept Derived = std::derived_from<D, T>;

template <typename D, typename T>
concept EqDerived = sizeof(D) == sizeof(T) && Derived<D, T>;

template <Constness lhs, Constness rhs>
concept ConstCompat = lhs == Constness::CONST || rhs == Constness::NCONST;

template <typename S, typename T, typename ... Args>
concept Selector = requires (T const& a, T const& b, Args&& ... args)
{ { std::declval<S>().operator()(a, b, std::forward<Args>(args) ...) } -> std::same_as<T const&>; };

/// types

template <typename E>
class Engine {
	static inline std::random_device s;
protected:
	static inline E e{s()};
};

template <typename D, typename E = std::default_random_engine>
class Random : protected Engine<E> {
protected:
	static inline D d{};
public:
	typename D::result_type
	operator()() const
	{ return d(Engine<E>::e); }

	typename D::result_type
	operator()(auto&& ... args) const
	{ return d(Engine<E>::e, typename D::param_type{std::forward<decltype(args)>(args) ...}); }
};

template <typename T, typename E = std::default_random_engine>
struct RandomSelector : protected Random<std::bernoulli_distribution, E> {
	T const&
	operator()(T const& a, T const& b) const
	{ return Random<std::bernoulli_distribution, E>::operator()() ? a : b; }

	T const&
	operator()(T const& a, T const& b, double p) const
	{ return Random<std::bernoulli_distribution, E>::operator()(p) ? a : b; }
};

template <typename T>
using LeftSelector = decltype([](T const& a, T const& b) -> T const& { return a; });

template <typename T>
using RightSelector = decltype([](T const& a, T const& b) -> T const& { return b; });

template <typename T, Constness c>
using TConstness = std::conditional_t<c == Constness::CONST, T const, T>;

template <typename ... Types>
struct Aggregation : Types ... {};

/// functions

template <typename P>
constexpr std::size_t
Offset(auto P::* m)
{ return reinterpret_cast<std::byte*>(&(reinterpret_cast<P*>(0)->*m)) - reinterpret_cast<std::byte*>(0); }

template <typename P>
auto*
ToParent(void* val, auto P::* m)
{ return reinterpret_cast<P*>(reinterpret_cast<std::byte*>(val) - Offset(m)); }

}//namespace DS
