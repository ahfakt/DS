#pragma once

#include "DS/Pack.hpp"
#include "Raw.tpp"
#include <DP/CreateInfo.hpp>
#include <Stream/InOut.hpp>
#include <tuple>
#include <utility>

namespace DS {

template <typename T>
struct Holder : Raw<T> {
	Holder() noexcept = default;

	explicit
	Holder(auto&& ... args)
	requires Stream::Initializable<T, decltype(args) ...>
	{ ::new(this) T{std::forward<decltype(args)>(args) ...}; }

	explicit
	Holder(Stream::Source auto& input, auto&& ... args)
	requires Stream::Extractable<T, decltype(input), decltype(args) ...>
	{
		if constexpr (sizeof...(args) == 0 && std::is_trivially_default_constructible_v<T>)
			input >> *reinterpret_cast<T*>(this);
		else
			input >> *::new(this) T{std::forward<decltype(args)>(args) ...};
	}

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

/// concepts

template <typename D, typename T>
concept Derived = std::derived_from<D, T>;

template <typename D, typename T>
concept EqDerived = sizeof(D) == sizeof(T) && Derived<D, T>;

template <Constness lhs, Constness rhs>
concept ConstCompat = lhs == Constness::CONST || rhs == Constness::NCONST;

template <typename T, Constness c>
using TConstness = std::conditional_t<c == Constness::CONST, T const, T>;

template <typename ... Types>
struct Aggregation : Types ... {};

/// functions

template <typename P>
constexpr std::size_t
Offset(auto P::* m) noexcept
{ return reinterpret_cast<std::byte*>(&(reinterpret_cast<P*>(0)->*m)) - reinterpret_cast<std::byte*>(0); }

template <typename P>
P*
ToParent(void* val, auto P::* m) noexcept
{ return reinterpret_cast<P*>(reinterpret_cast<std::byte*>(val) - Offset(m)); }

}//namespace DS
