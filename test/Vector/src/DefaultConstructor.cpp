#include "DS/Vector.h"
#include <cassert>

using namespace DS;

int main()
{
	Vector<int> vectorDefault;
	assert(vectorDefault.size() == 0);
	assert(vectorDefault.begin() == vectorDefault.end());
	assert(vectorDefault.rbegin() == vectorDefault.rend());

	return 0;
}