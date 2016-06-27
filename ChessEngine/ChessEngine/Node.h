#pragma once
#include <cstdint>
#include <vector>
#include "Board.h"

using namespace chessengine;

typedef uint8_t field;

struct Fields
{
	uint32_t depth : 5;       // 0..31
	uint32_t color : 1;       // 0..1
	uint32_t position : 6;    // 0..63
	uint32_t piece_t : 3;     // 0..5
	uint32_t destination : 6; // 0..63
	uint32_t num_childs : 6;  // 0..63
	uint32_t validated : 1;   // 0..1
	uint32_t kingcheck : 1;   // 0..1
};

class Node
{

public:
	short value = 0;
	Fields fields = {0, 0, 0, 0, 0, 0, 0, 0};
	Node *parentPtr = nullptr;
	Node **childrenPtr = nullptr;
	Board board;
	
	Node();
	~Node();

	void addChild(Node *node);
	color getColor() const;
	void setColor(const color color);
	void performAllStoredMoves(Board &board);
	uint64_t size() const;
	std::vector<Node*> getBestChildren() const;
	void validate();

private:
	uint8_t max_childs = 0;

	void extendChildrenTable();
	void performStoredMove(Board &board);

};