#include "DS/Map.hpp"
#include <cassert>

using namespace DS;

int main()
{
	Map<int, int> map1DDefault;
	assert(map1DDefault.size() == 0);
	assert(map1DDefault.begin() == map1DDefault.end());
	assert(map1DDefault.rbegin() == map1DDefault.rend());

	//
	Map<int, int, std::less<>, std::greater<>> map2DDefault;
	assert(map2DDefault.size() == 0);
	assert(map2DDefault.begin() == map2DDefault.end());
	assert(map2DDefault.rbegin() == map2DDefault.rend());

	return 0;
}