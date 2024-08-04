#pragma once

#include "DS/RefCounter.hpp"

namespace DS {

RefCounter::RefCounter(std::size_t count) noexcept
		: mCount{count}
{}

void
RefCounter::onDestroy() const noexcept
{ delete this; }

void
RefCounter::addRef(std::size_t i) const noexcept
{ mCount.fetch_add(i, std::memory_order::relaxed); }

void
RefCounter::subRef(std::size_t i) const noexcept
{
	if (mCount.fetch_sub(i, std::memory_order::release) == 1) {
		std::atomic_thread_fence(std::memory_order::acquire);
		onDestroy();
	}
}

}//namespace DS