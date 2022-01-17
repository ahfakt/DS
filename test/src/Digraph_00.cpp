#include <cassert>
#include "DS/Digraph.h"
#include "DSTest/Util.h"

using namespace DS;
using namespace DSTest;

void
testTraverse()
{
	Digraph<int, bool> digraph;
	Vector<Digraph<int, bool>::vertex> vertices(31);

	for(auto i : {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30})
		vertices.pushBack(digraph.addVertex(i));

	digraph.addEdge(true).setIn(vertices[8]).setOut(vertices[4]);
		digraph.addEdge(true).setIn(vertices[4]).setOut(vertices[2]);
			digraph.addEdge(true).setIn(vertices[2]).setOut(vertices[1]);
			digraph.addEdge(true).setIn(vertices[2]).setOut(vertices[3]);
		digraph.addEdge(true).setIn(vertices[4]).setOut(vertices[6]);
			digraph.addEdge(true).setIn(vertices[6]).setOut(vertices[5]);
			digraph.addEdge(true).setIn(vertices[6]).setOut(vertices[7]);
	digraph.addEdge(true).setIn(vertices[8]).setOut(vertices[12]);
		digraph.addEdge(true).setIn(vertices[12]).setOut(vertices[10]);
			digraph.addEdge(true).setIn(vertices[10]).setOut(vertices[9]);
			digraph.addEdge(true).setIn(vertices[10]).setOut(vertices[11]);
		digraph.addEdge(true).setIn(vertices[12]).setOut(vertices[14]);
			digraph.addEdge(true).setIn(vertices[14]).setOut(vertices[13]);
			digraph.addEdge(true).setIn(vertices[14]).setOut(vertices[15]);

	{
		auto vertexView = digraph.getVertexView();
		for (auto v = vertexView.begin(), ve = vertexView.end(); v != ve; ++v) {
			std::cout << *v;
			if (v.hasOut()) {
				std::cout << "\n\t";
				auto outList = v.getOut();
				for (auto e = outList.begin(), ee = outList.end(); e != ee; ++e) {
					std::cout << " > " << *e.getOut();
				}
			}
			if (v.hasIn()) {
				std::cout << "\n\t";
				auto inList = v.getIn();
				for (auto e = inList.begin(), ee = inList.end(); e != ee; ++e) {
					std::cout << " > " << *e.getIn();
				}
			}
			std::cout << "\n";
		}
	}

	{
		for (auto v : {vertices[8]})
			digraph.prepareTraverse(v);
		auto vertexView = digraph.prepareTraverse<Traverse::BREADTH_FIRST>().getVertexView();
		digraph.resetTraverse();

		std::array<int, 15> bfsPre = {8, 4, 12, 2, 6, 10, 14, 1, 3, 5, 7, 9, 11, 13, 15};
		assert(std::equal(vertexView.begin(), vertexView.end(), bfsPre.begin()));
	}

	{
		for (auto v : {vertices[8]})
			digraph.prepareTraverse(v);
		auto vertexView = digraph.prepareTraverse<Traverse::DEPTH_FIRST>().getVertexView();
		digraph.resetTraverse();

		std::array<int, 15> dfsPre = {8, 4, 2, 1, 3, 6, 5, 7, 12, 10, 9, 11, 14, 13, 15};
		assert(std::equal(vertexView.begin(), vertexView.end(), dfsPre.begin()));
	}

	{
		for (auto v : {vertices[8]})
			digraph.prepareTraverse(v);
		auto vertexView = digraph.prepareTraverse<Traverse::BREADTH_FIRST, Order::POSTORDER>().getVertexView();
		digraph.resetTraverse();

		std::array<int, 15> bfsPost = {1, 3, 5, 7, 9, 11, 13, 15, 2, 6, 10, 14, 4, 12, 8};
		assert(std::equal(vertexView.begin(), vertexView.end(), bfsPost.begin()));
	}

	{
		for (auto v : {vertices[8]})
			digraph.prepareTraverse(v);
		auto vertexView = digraph.prepareTraverse<Traverse::DEPTH_FIRST, Order::POSTORDER>().getVertexView();

		std::array<int, 15> dfsPost = {1, 3, 2, 5, 7, 6, 4, 9, 11, 10, 13, 15, 14, 12, 8};
		assert(std::equal(vertexView.begin(), vertexView.end(), dfsPost.begin()));
	}
}

int main() {
	testTraverse();
	return 0;
}
