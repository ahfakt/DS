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
		for (auto v : digraph.getVertexView()) {
			std::cout << *v;
			if (v.hasOut()) {
				std::cout << "\n\t";
				for (auto e : v.getAdjacencyList<Direction::FORWARD>()) {
					std::cout << " > " << *e.getOut();
				}
			}
			if (v.hasIn()) {
				std::cout << "\n\t";
				for (auto e : v.getAdjacencyList<Direction::BACKWARD>()) {
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

		std::array<Digraph<int, bool>::vertex, 15> bfsPre = {vertices[8], vertices[4], vertices[12], vertices[2], vertices[6], vertices[10], vertices[14], vertices[1], vertices[3], vertices[5], vertices[7], vertices[9], vertices[11], vertices[13], vertices[15]};
		assert(std::equal(vertexView.begin(), vertexView.end(), bfsPre.begin()));
	}

	{
		for (auto v : {vertices[8]})
			digraph.prepareTraverse(v);
		auto vertexView = digraph.prepareTraverse<Traverse::DEPTH_FIRST>().getVertexView();
		digraph.resetTraverse();

		std::array<Digraph<int, bool>::vertex, 15> dfsPre = {vertices[8], vertices[4], vertices[2], vertices[1], vertices[3], vertices[6], vertices[5], vertices[7], vertices[12], vertices[10], vertices[9], vertices[11], vertices[14], vertices[13], vertices[15]};
		assert(std::equal(vertexView.begin(), vertexView.end(), dfsPre.begin()));
	}

	{
		for (auto v : {vertices[8]})
			digraph.prepareTraverse(v);
		auto vertexView = digraph.prepareTraverse<Traverse::BREADTH_FIRST, Order::POSTORDER>().getVertexView();
		digraph.resetTraverse();

		std::array<Digraph<int, bool>::vertex, 15> bfsPost = {vertices[1], vertices[3], vertices[5], vertices[7], vertices[9], vertices[11], vertices[13], vertices[15], vertices[2], vertices[6], vertices[10], vertices[14], vertices[4], vertices[12], vertices[8]};
		assert(std::equal(vertexView.begin(), vertexView.end(), bfsPost.begin()));
	}

	{
		for (auto v : {vertices[8]})
			digraph.prepareTraverse(v);
		auto vertexView = digraph.prepareTraverse<Traverse::DEPTH_FIRST, Order::POSTORDER>().getVertexView();

		std::array<Digraph<int, bool>::vertex, 15> dfsPost = {vertices[1], vertices[3], vertices[2], vertices[5], vertices[7], vertices[6], vertices[4], vertices[9], vertices[11], vertices[10], vertices[13], vertices[15], vertices[14], vertices[12], vertices[8]};
		assert(std::equal(vertexView.begin(), vertexView.end(), dfsPost.begin()));
	}
}

int main() {
	testTraverse();
	return 0;
}
