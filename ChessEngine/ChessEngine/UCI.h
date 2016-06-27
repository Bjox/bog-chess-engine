#pragma once
#include "Board.h"
#include <string>

class UCI
{

public:
	UCI(unsigned int threads, unsigned int depth);
	~UCI();

	void start();
	chessengine::Board createBoardFromFen(const std::string& fenstr, chessengine::color& activeColor);

private:
	chessengine::color turnColor;
	chessengine::Board board;
	unsigned int threads;
	unsigned int depth;

};
