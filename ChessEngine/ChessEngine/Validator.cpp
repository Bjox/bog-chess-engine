#include "Validator.h"


short Validator::validate(const Board & board)
{
	long score = 0;
	score += pieceScore(board) * 1;// +posScore(board);
	return (short)score;
}


long Validator::pieceScore(const Board & board)
{
	long score = 0;

	for (piece_t t = Board::PAWN; t <= Board::QUEEN; t++)
	{
		score += (bitcount(board.bitboard[t + Board::WHITE]) - bitcount(board.bitboard[t + Board::BLACK])) * Board::PIECE_VALUE[t];
	}

	return score;
}


long Validator::posScore(const Board & board)
{
	long score = 0;

	posScoreHelper(board.bitboard[Board::BLACK + Board::PAWN], score);
	posScoreHelper(board.bitboard[Board::BLACK + Board::ROOK], score);
	posScoreHelper(board.bitboard[Board::BLACK + Board::KNIGHT], score);
	posScoreHelper(board.bitboard[Board::BLACK + Board::BISHOP], score);
	posScoreHelper(board.bitboard[Board::BLACK + Board::QUEEN], score);

	score *= -1;

	posScoreHelper(board.bitboard[Board::WHITE + Board::PAWN], score);
	posScoreHelper(board.bitboard[Board::WHITE + Board::ROOK], score);
	posScoreHelper(board.bitboard[Board::WHITE + Board::KNIGHT], score);
	posScoreHelper(board.bitboard[Board::WHITE + Board::BISHOP], score);
	posScoreHelper(board.bitboard[Board::WHITE + Board::QUEEN], score);

	return score;
}


void Validator::posScoreHelper(uint64_t p_typePosMask, long &out)
{
	for (uint64_t mask : PIECE_POS_SCORE_MASK) out += bitcount(mask & p_typePosMask);
}
