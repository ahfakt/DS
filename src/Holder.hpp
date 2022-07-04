#ifndef DS_HOLDER_HPP
#define DS_HOLDER_HPP

#include "DP/CreateInfo.h"
#include "Stream/InOut.h"

namespace DS {

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

template <typename T, typename InputRef, typename ... Args>
concept Deserializable = Stream::Constructible<T, InputRef, Args ...> || Stream::Extractable<T, InputRef, Args ...>;

template <typename D, typename T>
concept Derived = std::is_base_of_v<T, D>;

template <typename D, typename T>
concept EqDerived = sizeof(D) == sizeof(T) && Derived<D, T>;

template <Constness lhs, Constness rhs>
concept ConstCompat = lhs == Constness::CONST || rhs == Constness::NCONST;

template <typename T, Constness c>
using TConstness = std::conditional_t<c == Constness::CONST, T const, T>;

template <typename T>
class alignas(T) Holder {
	std::byte raw[sizeof(T)];

public:
	Holder() noexcept = default;

	Holder(T const& other)
	{ ::new(reinterpret_cast<void*>(raw)) T(other); }

	template <typename ... Args>
	Holder(Args&& ... args)
	requires std::constructible_from<T, Args ...>
	{ ::new(reinterpret_cast<void*>(raw)) T(std::forward<Args>(args) ...); }

	template <typename ... CFArgs, typename ... CArgs>
	Holder(DP::CreateFunc<T, CFArgs ...> const& createFunc, CArgs&& ... cArgs)
	{ createFunc(reinterpret_cast<void*>(raw), std::forward<CArgs>(cArgs) ...); }

	template <typename ... Args>
	Holder(Stream::In auto& input, Args&& ... args)
	requires std::constructible_from<T, decltype(input), Args ...>
	{ ::new(reinterpret_cast<void*>(raw)) T(input, std::forward<Args>(args) ...); }

	template <typename ... Args>
	Holder(Stream::In auto& input, Args&& ... args)
	requires std::constructible_from<T, Args ...> && Stream::Deserializable<T, decltype(input)>
	{ input >> *new(reinterpret_cast<void*>(raw)) T(std::forward<Args>(args) ...); }

	Holder(Stream::In auto& input)
	requires std::is_trivially_default_constructible_v<T> && Stream::Deserializable<T, decltype(input)>
	{ input >> *reinterpret_cast<T*>(raw); }

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
};

template <typename T, typename U>
std::size_t
Offset(U T::* m)
{ return reinterpret_cast<std::byte*>(&(reinterpret_cast<T*>(0)->*m)) - reinterpret_cast<std::byte*>(0); }

template <typename Node, typename T>
Node*
ValToNode(T* val, Holder<T> Node::* m)
{ return reinterpret_cast<Node*>(reinterpret_cast<std::byte*>(val) - reinterpret_cast<std::byte*>(&(reinterpret_cast<Node*>(0)->*m))); }

template<std::size_t N, typename First, typename ... Others>
struct nth
{ using type = typename nth<N - 1, Others ...>::type; };

template<typename First, typename ... Others>
struct nth<0, First, Others ...>
{ using type = First; };

}//namespace DS

#endif //DS_HOLDER_HPP
