#include "DS/Map.hpp"
#include <cassert>

using namespace DS;

int main()
{
	Map<int, int> map1D;
	for (int i = 0; i < 13; ++i)
		map1D.put(i).set(i);

	Map<int, int> map1DCopied(map1D);

	assert((std::equal(map1D.begin(), map1D.end(), map1DCopied.begin())));
	assert((std::equal(map1D.rbegin(), map1D.rend(), map1DCopied.rbegin())));

	//
	Map<int, int, std::less<>, std::greater<>> map2D;
	for (int i = 0; i < 13; ++i)
		map2D.put(i).set(i);

	Map<int, int, std::less<>, std::greater<>> map2DCopied(map2D);

	assert((std::equal(map2D.begin<0>(), map2D.end<0>(), map2DCopied.begin<0>())));
	assert((std::equal(map2D.rbegin<0>(), map2D.rend<0>(), map2DCopied.rbegin<0>())));

	assert((std::equal(map2D.begin<1>(), map2D.end<1>(), map2DCopied.begin<1>())));
	assert((std::equal(map2D.rbegin<1>(), map2D.rend<1>(), map2DCopied.rbegin<1>())));

	return 0;
}