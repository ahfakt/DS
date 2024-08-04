#pragma once

#include <random>

namespace DS {

template <typename Distribution, typename Engine = std::default_random_engine>
class Random {
protected:
	static inline std::random_device seed;
	static inline Engine engine{seed()};
	static inline Distribution distribution{};
public:
	typename Distribution::result_type
	operator()() const
	{ return distribution(engine); }

	typename Distribution::result_type
	operator()(auto&& ... args) const
	{ return distribution(engine, typename Distribution::param_type{std::forward<decltype(args)>(args) ...}); }
};//class DS::Random<Distribution, Engine>

template <typename S, typename T, typename ... Args>
concept Selector = requires (T const& a, T const& b, Args&& ... args)
{ { std::declval<S>().operator()(a, b, std::forward<Args>(args) ...) } -> std::same_as<T const&>; };

template <typename T, typename Engine = std::default_random_engine>
struct RandomSelector : protected Random<std::bernoulli_distribution, Engine> {
	T const&
	operator()(T const& a, T const& b) const
	{ return Random<std::bernoulli_distribution, Engine>::operator()() ? a : b; }

	T const&
	operator()(T const& a, T const& b, double p) const
	{ return Random<std::bernoulli_distribution, Engine>::operator()(p) ? a : b; }
};

template <typename T>
using LeftSelector = decltype([](T const& a, T const& b) noexcept -> T const& { return a; });

template <typename T>
using RightSelector = decltype([](T const& a, T const& b) noexcept -> T const& { return b; });

}//namespace DS
