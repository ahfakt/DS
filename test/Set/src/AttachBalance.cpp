#include "DS/Set.hpp"
#include "DS/Test/Balance.hpp"
#include <random>

using namespace DS;

int main()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distrib;

	Set<int> set1D;
	auto* containerSize1D = reinterpret_cast<std::byte*>(&set1D);
	auto** rootNode1D = reinterpret_cast<TNode<1>**>(containerSize1D + sizeof(std::uint64_t));
	for (int i{0}; i < 1024; ++i) {
		set1D.put(distrib(gen));
		DS::Test::TestBalance<1, 0>(rootNode1D[0]);
	}

	//
	Set<int, std::less<>, std::greater<>> set2D;
	auto* containerSize2D = reinterpret_cast<std::byte*>(&set2D);
	auto** rootNode2D = reinterpret_cast<TNode<2>**>(containerSize2D + sizeof(std::uint64_t));
	for (int i{0}; i < 1024; ++i) {
		set2D.put(distrib(gen));
		DS::Test::TestBalance<2, 0>(rootNode2D[0]);
		DS::Test::TestBalance<2, 1>(rootNode2D[1]);
	}

	return 0;
}