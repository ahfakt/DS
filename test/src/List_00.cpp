#include <Stream/Pipe.h>
#include <cassert>
#include <functional>
#include "DS/List.h"
#include "DSTest/Util.h"

using namespace DS;
using namespace DSTest;

void
testStreamConstructor()
{
	IO::Pipe io;
	Stream::Pipe stream;
	io <=> stream;

	std::array<int, 3>			intArr = {1, 2, 3};
	{// For each element in stream, trivial construct and deserialize
		stream << std::uint64_t(intArr.size());
		for (auto i: intArr)
			stream << i;
		List<int> intList(stream);
		assert((std::equal(std::begin(intArr), std::end(intArr), intList.begin())));
	}
	{// For each element in stream, construct with 0 and deserialize
		stream << std::uint64_t(intArr.size());
		for (auto i: intArr)
			stream << i;
		List<int> intList(stream, 0);
		assert((std::equal(std::begin(intArr), std::end(intArr), intList.begin())));
	}

	std::array<std::string, 3>	strArr = {"1", "2", "3"};
	{// For each element in stream, default construct and deserialize
		stream << std::uint64_t(strArr.size());
		for (auto const& i: strArr)
			stream << i;
		List<std::string> strList(stream);
		assert((std::equal(std::begin(strArr), std::end(strArr), strList.begin())));
	}
	{// For each element in stream, construct with "0" and deserialize
		stream << std::uint64_t(strArr.size());
		for (auto const& i: strArr)
			stream << i;
		List<std::string> strList(stream, "0");
		assert((std::equal(std::begin(strArr), std::end(strArr), strList.begin())));
	}
}
struct D3VBaseIntString : VBaseIntString, public FVBaseIntString::Registrar<D3VBaseIntString, 3> {
	D3VBaseIntString(int const &i, const std::basic_string<char> &s) : VBaseIntString(i, s) {}
};

void
testPushBack()
{
	List<VBaseIntString> vBaseIntStringList;
	vBaseIntStringList.pushBack(0, "0");
	vBaseIntStringList.pushBack<D2VBaseIntString>(2, "2");
	vBaseIntStringList.pushBack(FVBaseIntString::GetCreateInfo({3}), 3, "3");

	for (auto i : vBaseIntStringList)
		std::cout << i.mInt << " " << i.mString << "\n";
}

int main() {
	testStreamConstructor();
	testPushBack();
	return 0;
}
