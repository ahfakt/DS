#pragma once

#include "DS/CQueue.hpp"

namespace DS {

template <typename T>
CQueue<T>::~CQueue()
{
	CQNode<T>* h{mHead.loadExclusively()};
	while (h) {
		CQNode<T>* q{h};
		h = h->next;
		delete q;
	}
}

template <typename T>
bool
CQueue<T>::enqueue(CQNode<T>* const q) noexcept
{
	CQNode<T>* t{mTail.load(std::memory_order::acquire)};

	// Race on mTail to enqueue
	while (!mTail.compare_exchange_weak(t, q, std::memory_order::release, std::memory_order::relaxed));

	if (t) {
		CQNode<T>* n{nullptr};
		if (std::atomic_ref{t->next}.compare_exchange_strong(n, q, std::memory_order::release, std::memory_order::relaxed)) {
			std::atomic_ref{mSize}.fetch_add(1, std::memory_order::relaxed);
			return false;
		}

		t->subRef();
	}

	// Queue is empty
	mHead.store(q, std::memory_order::release);
	std::atomic_ref{mSize}.fetch_add(1, std::memory_order::relaxed);
	return true;
}

template <typename T>
IntrusivePtr<T>
CQueue<T>::dequeue() noexcept
{
	CountedPtr<CQNode<T>> q{mHead.load(std::memory_order::acquire)};
	CountedPtr<CQNode<T>> h{nullptr};

	while (q && (h = mHead.fetchAddCount(1, std::memory_order::release))) {
		(q = h).cnt += 1;
		CQNode<T>* n{std::atomic_ref{h->next}.load(std::memory_order::acquire)};
		do { // Race on mHead to dequeue
			if (mHead.compare_exchange_weak(q, n, std::memory_order::release, std::memory_order::relaxed)) {
				std::atomic_ref{mSize}.fetch_sub(1, std::memory_order::relaxed);

				if (!n) { // Queue was empty, try to set mTail
					CQNode<T>* t{h};
					if (!mTail.compare_exchange_strong(t, nullptr, std::memory_order::release, std::memory_order::relaxed)) {
						// Queue has a new item, try to set q->next before enqueue
						if (std::atomic_ref{q->next}.compare_exchange_strong(n, q, std::memory_order::release, std::memory_order::relaxed)) {
							// Enqueue did not set q->next yet, it may still has a reference to q
							q->addRef(q.cnt + 1);
							return IntrusivePtr<T>{static_cast<T*>(q->val), Offset(&CQNode<T>::val)};
						}
						// Enqueue has set q->next before dequeue
						// Dequeue is responsible to update mHead
						mHead.store(n, std::memory_order::release);
					}
				}

				q->addRef(q.cnt);
				return IntrusivePtr<T>{static_cast<T*>(q->val), Offset(&CQNode<T>::val)};
			}
		} while (q == h);

		h->subRef();
	}

	return IntrusivePtr<T>{};
}

template <typename T>
bool
CQueue<T>::enqueue(auto&& ... tArgs)
{ return enqueue(::new CQNode<T>(std::forward<decltype(tArgs)>(tArgs) ...)); }

template <typename T>
template <Derived<T> DT>
bool
CQueue<T>::enqueue(auto&& ... dtArgs)
{ return enqueue(reinterpret_cast<CQNode<T>*>(::new CQNode<DT>(std::forward<decltype(dtArgs)>(dtArgs) ...))); }

template <typename T>
template <typename ... Args>
bool
CQueue<T>::enqueue(DP::CreateInfo<T, Args ...> const& createInfo, auto&& ... args)
{ return enqueue(new(createInfo.size) CQNode<T>(createInfo, std::forward<decltype(args)>(args) ...)); }

}//namespace DS
