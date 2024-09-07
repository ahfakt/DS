#pragma once

#include <atomic>

namespace DS {

class alignas(64 /* std::hardware_destructive_interference_size */) RefCounter {
	std::atomic<std::size_t> mutable mCount;

protected:
	explicit
	RefCounter(std::size_t count = 0) noexcept;

	virtual void
	onDestroy() const noexcept;

public:
	virtual
	~RefCounter() = default;

	void
	addRef(std::size_t i = 1) const noexcept;

	void
	subRef(std::size_t i = 1) const noexcept;
};//class RefCounter

}//namespace DS

#include "../../src/DS/RefCounter.tpp"