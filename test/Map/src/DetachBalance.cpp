#include "DS/Map.h"
#include <cassert>
#include <random>
#include "DSTest/Util.h"

using namespace DS;

int main()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distrib;

	Map<int, int> map1D;
	auto* containerSize1D = reinterpret_cast<std::byte*>(&map1D);
	auto** rootNode1D = reinterpret_cast<TNode<1>**>(containerSize1D + sizeof(std::size_t));
	for (int i = 0; i < 1024; ++i)
		map1D.put(distrib(gen));
	for (int i = 0; i < map1D.size(); ++i) {
		map1D.remove(map1D.at(std::uniform_int_distribution<>{0, static_cast<int>(map1D.size() - i - 1)}(gen))->first);
		DSTest::TestBalance<1, 0>(rootNode1D[0]);
	}

	//
	Map<int, int, std::less<>, std::greater<>> map2D;
	auto* containerSize2D = reinterpret_cast<std::byte*>(&map2D);
	auto** rootNode2D = reinterpret_cast<TNode<2>**>(containerSize2D + sizeof(std::size_t));
	for (int i = 0; i < 1024; ++i)
		map2D.put(distrib(gen));
	for (int i = 0; i < map2D.size(); ++i) {
		map2D.remove(map2D.at(std::uniform_int_distribution<>{0, static_cast<int>(map2D.size() - i - 1)}(gen))->first);
		DSTest::TestBalance<2, 0>(rootNode2D[0]);
		DSTest::TestBalance<2, 1>(rootNode2D[1]);
	}

	return 0;
}