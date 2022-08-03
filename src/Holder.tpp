#ifndef DS_HOLDER_TPP
#define DS_HOLDER_TPP

#include <DP/CreateInfo.hpp>
#include <Stream/InOut.hpp>
#include <random>

namespace DS {

template <typename T>
class alignas(T) Holder {
	std::byte raw[sizeof(T)];

public:
	Holder() noexcept = default;

	Holder(auto&& ... args)
	{ ::new(reinterpret_cast<void*>(raw)) T{std::forward<decltype(args)>(args) ...}; }

	Holder(Stream::ExtractableTo<T> auto& input, auto&& ... args)
	requires (!Stream::ConstructibleFrom<T, decltype(input), decltype(args) ...>)
	{ input >> *::new(reinterpret_cast<void*>(raw)) T{std::forward<decltype(args)>(args) ...}; }

	Holder(Stream::TriviallyExtractableTo<T> auto& input)
	requires (!Stream::ConstructibleFrom<T, decltype(input)>)
	{ input >> *reinterpret_cast<T*>(raw); }

	template <typename ... Args>
	Holder(DP::Constructor<T, Args ...> const& constructor, auto&& ... args)
	{ constructor(reinterpret_cast<void*>(raw), std::forward<decltype(args)>(args) ...); }

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

	explicit operator T*() noexcept
	{ return reinterpret_cast<T*>(raw); }

	explicit operator T const*() const noexcept
	{ return reinterpret_cast<T const*>(raw); }

	explicit operator T&() noexcept
	{ return *reinterpret_cast<T*>(raw); }

	explicit operator T const&() const noexcept
	{ return *reinterpret_cast<T const*>(raw); }

	explicit operator void*() noexcept
	{ return raw; }

	explicit operator void const*() const noexcept
	{ return raw; }
};//class Holder<T>

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
struct Engine {
private:
	static inline std::random_device s;
protected:
	static inline E e{s()};
};

template <typename D, typename E = std::default_random_engine>
struct Random : Engine<E> {
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
struct RandomSelector : Random<std::bernoulli_distribution, E> {
	T const&
	operator()(T const& a, T const& b) const
	{ return Random<E, std::bernoulli_distribution>::operator()() ? a : b; }

	T const&
	operator()(T const& a, T const& b, double p) const
	{ return Random<E, std::bernoulli_distribution>::operator()(p) ? a : b; }
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

#endif //DS_HOLDER_TPP
