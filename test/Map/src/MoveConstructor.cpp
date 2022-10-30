#include "DS/Map.hpp"
#include <cassert>

using namespace DS;

int main()
{
	Map<int, int> map1DDefault;
	Map<int, int> map1D;
	for (int i{0}; i < 13; ++i)
		map1D.put(i).set(i);

	Map<int, int> map1DCopied(map1D);
	Map<int, int> map1DMoved(std::move(map1D));

	assert((std::equal(map1D.begin(), map1D.end(), map1DDefault.begin())));
	assert((std::equal(map1D.rbegin(), map1D.rend(), map1DDefault.rbegin())));

	assert((std::equal(map1DMoved.begin(), map1DMoved.end(), map1DCopied.begin())));
	assert((std::equal(map1DMoved.rbegin(), map1DMoved.rend(), map1DCopied.rbegin())));

	//
	Map<int, int, std::less<>, std::greater<>> map2DDefault;
	Map<int, int, std::less<>, std::greater<>> map2D;
	for (int i{0}; i < 13; ++i)
		map2D.put(i).set(i);

	Map<int, int, std::less<>, std::greater<>> map2DCopied(map2D);
	Map<int, int, std::less<>, std::greater<>> map2DMoved(std::move(map2D));

	assert((std::equal(map2D.begin<0>(), map2D.end<0>(), map2DDefault.begin<0>())));
	assert((std::equal(map2D.rbegin<0>(), map2D.rend<0>(), map2DDefault.rbegin<0>())));

	assert((std::equal(map2DMoved.begin<0>(), map2DMoved.end<0>(), map2DCopied.begin<0>())));
	assert((std::equal(map2DMoved.rbegin<0>(), map2DMoved.rend<0>(), map2DCopied.rbegin<0>())));

	assert((std::equal(map2D.begin<1>(), map2D.end<1>(), map2DDefault.begin<1>())));
	assert((std::equal(map2D.rbegin<1>(), map2D.rend<1>(), map2DDefault.rbegin<1>())));

	assert((std::equal(map2DMoved.begin<1>(), map2DMoved.end<1>(), map2DCopied.begin<1>())));
	assert((std::equal(map2DMoved.rbegin<1>(), map2DMoved.rend<1>(), map2DCopied.rbegin<1>())));

	return 0;
}