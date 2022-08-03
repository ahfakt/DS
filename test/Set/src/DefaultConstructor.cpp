#include "DS/Set.hpp"
#include <cassert>

using namespace DS;

int main()
{
	Set<int> set1DDefault;
	assert(set1DDefault.size() == 0);
	assert(set1DDefault.begin() == set1DDefault.end());
	assert(set1DDefault.rbegin() == set1DDefault.rend());

	//
	Set<int, std::less<>, std::greater<>> set2DDefault;
	assert(set2DDefault.size() == 0);
	assert(set2DDefault.begin() == set2DDefault.end());
	assert(set2DDefault.rbegin() == set2DDefault.rend());

	return 0;
}