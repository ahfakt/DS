#pragma once

#include <cstdint>

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

/**
 * @brief	Tuple like structure.
 * @class	Pack
 */
template <typename T, typename ... U>
struct Pack
{
	T value;
	Pack<U ...> next;
};

/**
 * @brief	Tuple like structure.
 * @class	Pack
 */
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

}//namespace DS
