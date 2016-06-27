#pragma once
#include "Board.h"
#include "Bitops.h"

using namespace chessengine;

const uint64_t PIECE_POS_SCORE_MASK[3] = { 0x7E7E7E7E7E7E00ui64, 0x3C3C3C3C0000ui64, 0x1818000000ui64 };

class Validator
{
public:
	static short validate(const Board &board);

private:
	static long pieceScore(const Board &board);
	static long posScore(const Board &board);
	static void posScoreHelper(uint64_t p_typePosMask, long &out);
};

