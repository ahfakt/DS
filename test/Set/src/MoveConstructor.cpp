#include "DS/Set.hpp"
#include <cassert>

using namespace DS;

int main()
{
	Set<int> set1DDefault;
	Set<int> set1D;
	for (int i{0}; i < 13; ++i)
		set1D.put(i);

	Set<int> set1DCopied(set1D);
	Set<int> set1DMoved(std::move(set1D));

	assert((std::equal(set1D.begin(), set1D.end(), set1DDefault.begin())));
	assert((std::equal(set1D.rbegin(), set1D.rend(), set1DDefault.rbegin())));

	assert((std::equal(set1DMoved.begin(), set1DMoved.end(), set1DCopied.begin())));
	assert((std::equal(set1DMoved.rbegin(), set1DMoved.rend(), set1DCopied.rbegin())));

	//
	Set<int, std::less<>, std::greater<>> set2DDefault;
	Set<int, std::less<>, std::greater<>> set2D;
	for (int i{0}; i < 13; ++i)
		set2D.put(i);

	Set<int, std::less<>, std::greater<>> set2DCopied(set2D);
	Set<int, std::less<>, std::greater<>> set2DMoved(std::move(set2D));

	assert((std::equal(set2D.begin<0>(), set2D.end<0>(), set2DDefault.begin<0>())));
	assert((std::equal(set2D.rbegin<0>(), set2D.rend<0>(), set2DDefault.rbegin<0>())));

	assert((std::equal(set2DMoved.begin<0>(), set2DMoved.end<0>(), set2DCopied.begin<0>())));
	assert((std::equal(set2DMoved.rbegin<0>(), set2DMoved.rend<0>(), set2DCopied.rbegin<0>())));

	assert((std::equal(set2D.begin<1>(), set2D.end<1>(), set2DDefault.begin<1>())));
	assert((std::equal(set2D.rbegin<1>(), set2D.rend<1>(), set2DDefault.rbegin<1>())));

	assert((std::equal(set2DMoved.begin<1>(), set2DMoved.end<1>(), set2DCopied.begin<1>())));
	assert((std::equal(set2DMoved.rbegin<1>(), set2DMoved.rend<1>(), set2DCopied.rbegin<1>())));

	return 0;
}