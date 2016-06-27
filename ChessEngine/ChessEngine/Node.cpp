#include "Node.h"
#include "Validator.h"
#include <iostream>

Node::Node()
{

}


Node::~Node()
{
	for (size_t i = 0; i < fields.num_childs; i++)
	{
		delete childrenPtr[i];
		childrenPtr[i] = nullptr;
	}
	delete[] childrenPtr;
	childrenPtr = nullptr;
}


void Node::addChild(Node *node)
{
	if (fields.num_childs == max_childs)
		extendChildrenTable();

	childrenPtr[fields.num_childs++] = node;
	node->parentPtr = this;
}


color Node::getColor() const
{
	return fields.color * Board::BLACK;
}


void Node::setColor(const color color)
{
	fields.color = color / Board::BLACK;
}


uint64_t Node::size() const
{
	if (fields.num_childs == 0)
	{
		return 1;
	}

	uint64_t n = 1;
	for (size_t i = 0; i < fields.num_childs; i++)
	{
		n += childrenPtr[i]->size();
	}
	return n;
}


std::vector<Node*> Node::getBestChildren() const
{
	std::vector<Node*> vec;
	for (size_t i = 0; i < fields.num_childs; i++)
	{
		if (childrenPtr[i]->value == value)
			vec.push_back(childrenPtr[i]);
	}
	return vec;
}


void Node::validate()
{
	if (fields.validated == 0) {
		fields.validated = 1;
		value = Validator::validate(board);
	}
}


void Node::performAllStoredMoves(Board &board)
{
	Node **parentsPtr = new Node*[fields.depth];
	Node *nPtr = this;

	unsigned int c = 0;
	while (nPtr->parentPtr != nullptr)
	{
		parentsPtr[c++] = nPtr;
		nPtr = nPtr->parentPtr;
	}

	for (int i = fields.depth - 1; i >= 0; i--)
	{
		parentsPtr[i]->performStoredMove(board);
	}

	delete[] parentsPtr;
}


void Node::performStoredMove(Board &board)
{
	board.movePiece(fields.position, fields.piece_t, parentPtr->getColor(), fields.destination);
}


void Node::extendChildrenTable()
{
	unsigned int new_size = max_childs == 0 ? max_childs = 8 : max_childs *= 2;

	Node **tmpPtr = new Node*[new_size];

	for (size_t i = 0; i < fields.num_childs; i++)
	{
		tmpPtr[i] = childrenPtr[i];
	}

	delete[] childrenPtr;
	childrenPtr = tmpPtr;
}
