#include "DS/List.h"
#include <cassert>

using namespace DS;

int main()
{
	List<int> listDefault;
	assert(listDefault.size() == 0);
	assert(listDefault.begin() == listDefault.end());
	assert(listDefault.rbegin() == listDefault.rend());

	return 0;
}