#include "DS/CQueue.hpp"
#include <cassert>
#include <iostream>
#include <list>
#include <thread>

using namespace DS;

int main(int argc, char* argv[])
{
	CQueue<int> concurrentQueue;
	int testCount{std::stoi(argv[1])};
	int const itemCount{std::stoi(argv[2])};
	int const consumerCount{std::stoi(argv[3])};

	auto const t0{std::chrono::steady_clock::now()};
	for (int t = 0; t < testCount;) {
		for (int i = 0; i < itemCount; ++i)
			concurrentQueue.enqueue(i);

		auto const t1{std::chrono::steady_clock::now()};
		{
			std::list<std::jthread> consumers;
			for (int c = 0; c < consumerCount; ++c) {
				consumers.emplace_back([](CQueue<int>& cq, int start, int end, int step) {
					for (int i = start; i < end; i += step)
						if (cq.dequeue() == nullptr)
							i -= step;
				}, std::ref(concurrentQueue), c, itemCount, consumerCount);
			}
		}
		auto const now{std::chrono::steady_clock::now()};

		assert(concurrentQueue.size() == 0);

		std::cout
			<< ++t << '/' << testCount << '\t'
			<< std::chrono::duration_cast<std::chrono::duration<float>>(now - t1).count() << "s ~"
			<< std::chrono::duration_cast<std::chrono::duration<float>>(now - t0).count()/t << "s\t"
			<< std::chrono::duration_cast<std::chrono::days>(now - t0).count() << ':'
			<< std::chrono::duration_cast<std::chrono::hours>(now - t0).count() % 24 << ':'
			<< std::chrono::duration_cast<std::chrono::minutes>(now - t0).count() % 60 << ':'
			<< std::chrono::duration_cast<std::chrono::seconds>(now - t0).count() % 60
			<< std::endl;
	}

	return 0;
}