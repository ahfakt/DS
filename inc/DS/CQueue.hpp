#pragma once

#include "../../src/DS/Container.tpp"
#include "../../src/DS/CQNode.tpp"
#include "CountedPtr.hpp"
#include "IntrusivePtr.hpp"

namespace DS {

/**
 * @brief	Concurrent queue implementation.
 * @class	CQueue CQueue.hpp "DS/CQueue.hpp"
 * @tparam	T Value type to be stored in CQueue
 */
template <typename T>
class CQueue : public Container<true> {
	alignas(64 /* std::hardware_destructive_interference_size */) std::atomic<CountedPtr<CQNode<T>>> mHead{nullptr};
	alignas(64 /* std::hardware_destructive_interference_size */) std::atomic<CQNode<T>*> mTail{nullptr};
	std::byte padTail[64 - sizeof(CQNode<T>*)];

	bool
	enqueue(CQNode<T>* q) noexcept;

public:
	CQueue() noexcept = default;

	/**
	 * @brief	Copy constructor deleted
	 */
	CQueue(CQueue const&) = delete;

	/**
	 * @brief	Move constructor deleted
	 */
	CQueue(CQueue&&) = delete;

	/**
	 * @brief	Copy assignment deleted
	 */
	CQueue&
	operator=(CQueue const&) = delete;

	/**
	 * @brief	Move assignment deleted
	 */
	CQueue&
	operator=(CQueue&&) = delete;

	/**
	 * @brief	Destructor
	 */
	~CQueue();

	bool
	enqueue(auto&& ... tArgs);

	template <Derived<T> DT>
	bool
	enqueue(auto&& ... dtArgs);

	template <typename ... Args>
	bool
	enqueue(DP::CreateInfo<T, Args ...> const& createInfo, auto&& ... args);

	IntrusivePtr<T>
	dequeue() noexcept;
};//class DS::CQueue<T>

}//namespace DS

#include "../../src/DS/CQueue.tpp"