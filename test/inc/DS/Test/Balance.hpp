#pragma once

#include "../../../../src/DS/TNode.tpp"
#include <cassert>

namespace DS::Test {

template <std::size_t M, std::size_t N>
int
TestBalance(DS::TNode<M> const* t, int depth = 0) {
	if (t) {
		++depth;
		int leftDepth = t->d[N].hasLeft ? TestBalance<M, N>(t->template left<N>(), depth) : depth;
		int rightDepth = t->d[N].hasRight ? TestBalance<M, N>(t->template right<N>(), depth) : depth;

		if (leftDepth > rightDepth) {
			assert(!t->d[N].isRight);
			assert(!t->d[N].isBalanced);
			assert(t->d[N].isLeft);
			assert((leftDepth - rightDepth < 2));
			return leftDepth;
		}
		if (rightDepth > leftDepth) {
			assert(t->d[N].isRight);
			assert(!t->d[N].isBalanced);
			assert(!t->d[N].isLeft);
			assert((rightDepth - leftDepth < 2));
			return rightDepth;
		}
		assert(!t->d[N].isRight);
		assert(t->d[N].isBalanced);
		assert(!t->d[N].isLeft);
		return leftDepth;
	}
	return 0;
}

}//namespace DS::Test
