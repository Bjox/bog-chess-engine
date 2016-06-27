#include "Board.h"
#include "Bitops.h"
#include "MoveGenerator.h"

using namespace chessengine;

const unsigned int Board::PIECE_VALUE[6] = { 1, 5, 3, 3, 9, 0 };
const std::string Board::PIECE_NAME[6] = { "PAWN", "ROOK", "KNIGHT", "BISHOP", "QUEEN", "KING" };

Board::Board()
{
	for (unsigned int i = 0; i < Board::NUM_OF_BITBOARDS; i++)
	{
		bitboard[i] = 0; // init to 0
	}
}


Board::~Board()
{
}


void Board::print(const uint64_t &bitboard)
{
	for (int rank = Board::NUM_OF_RANKS-1; rank >= 0; --rank)
	{
		std::string line = std::bitset<8>(bitboard >> rank * 8).to_string();

		std::reverse(line.begin(), line.end());
		std::replace(line.begin(), line.end(), '0', '.');
		std::replace(line.begin(), line.end(), '1', '#');

		std::string line_f = std::to_string(rank + 1) + " ";
		
		for
		(
			std::string::const_iterator it = line.begin();
			it != line.end();
			it++
		)
		{
			line_f += *it;
			line_f += " ";
		}

		std::cout << line_f << std::endl;
	}

	std::cout << "  a b c d e f g h" << std::endl;
}


void chessengine::Board::printFull() const
{
	for (rank r = NUM_OF_RANKS - 1; r >= 0; r--)
	{
		std::string line = "";
		line += std::to_string(r + 1);
		line += "   ";

		for (file f = 0; f < NUM_OF_FILES; f++)
		{
			piece_p pos = r * 8 + f;
			piece_t t = pieceType(pos);
			color col = pieceColor(1ui64 << pos);

			if (col != -1)
			{
				char abbrv;
				switch (t)
				{
				case PAWN: abbrv = 'P'; break;
				case ROOK: abbrv = 'R'; break;
				case BISHOP: abbrv = 'B'; break;
				case KNIGHT: abbrv = 'N'; break;
				case QUEEN: abbrv = 'Q'; break;
				case KING: abbrv = 'K'; break;
				}
				if (col == BLACK) abbrv += 32;

				line += abbrv;
			}
			else
			{
				line += ".";
			}
			line += " ";
		}

		std::cout << line << std::endl;
	}
	std::cout << std::endl << "    a b c d e f g h" << std::endl;
}


void Board::shiftToRank(uint64_t &val, const rank rank)
{
	val <<= (rank * 8);
}


uint64_t Board::shiftToRankCopy(const uint64_t &val, const rank rank)
{
	uint64_t newVal = val;
	shiftToRank(newVal, rank);
	return newVal;
}


std::string Board::squareNotation(const piece_p pos)
{
	chessengine::file f = pos % 8;
	chessengine::rank r = pos / 8;
	char fc = 'a' + f;
	char rc = '1' + r;
	std::string ret;
	ret.resize(2);
	ret[0] = fc;
	ret[1] = rc;
	return ret;
}


// Resets the board.
void Board::init()
{
	// Pawns
	bitboard[WHITE + PAWN] = shiftToRankCopy(PAWN_RANK_VAL, RANK_2);
	bitboard[BLACK + PAWN] = shiftToRankCopy(PAWN_RANK_VAL, RANK_7);

	// Rooks
	bitboard[WHITE + ROOK] = shiftToRankCopy(ROOK_RANK_VAL, RANK_1);
	bitboard[BLACK + ROOK] = shiftToRankCopy(ROOK_RANK_VAL, RANK_8);

	// Knights
	bitboard[WHITE + KNIGHT] = shiftToRankCopy(KNIGHT_RANK_VAL, RANK_1);
	bitboard[BLACK + KNIGHT] = shiftToRankCopy(KNIGHT_RANK_VAL, RANK_8);

	// Bishops
	bitboard[WHITE + BISHOP] = shiftToRankCopy(BISHOP_RANK_VAL, RANK_1);
	bitboard[BLACK + BISHOP] = shiftToRankCopy(BISHOP_RANK_VAL, RANK_8);

	// Queens
	bitboard[WHITE + QUEEN] = shiftToRankCopy(QUEEN_RANK_VAL, RANK_1);
	bitboard[BLACK + QUEEN] = shiftToRankCopy(QUEEN_RANK_VAL, RANK_8);

	// Kings
	bitboard[WHITE + KING] = shiftToRankCopy(KING_RANK_VAL, RANK_1);
	bitboard[BLACK + KING] = shiftToRankCopy(KING_RANK_VAL, RANK_8);
}

// Creates a mask marking occupied squares on the board.
uint64_t Board::positionMask() const
{
	uint64_t mask = 0;
	for (piece_t i = PAWN; i <= KING; i++)
	{
		mask |= bitboard[i + WHITE];
		mask |= bitboard[i + BLACK];
	}
	return mask;
}

// Creates a mask marking occupied squares on the board by a specific color.
uint64_t Board::colorPositionMask(const color &color) const
{
	uint64_t mask = 0;
	for (piece_t i = PAWN; i <= KING; i++)
	{
		mask |= bitboard[i + color];
	}
	return mask;
}

// Retrieves the color of the piece at a given position.
color Board::pieceColor(const uint64_t &posmask) const
{
	for (piece_t i = PAWN; i <= KING; i++)
	{
		if (bitboard[i + WHITE] & posmask) return WHITE;
		if (bitboard[i + BLACK] & posmask) return BLACK;
	}
	return -1;
}


void Board::movePiece(const piece_p pos, const piece_t type, const color color, const piece_p dest)
{
	bitboard[color + type] &= ~(1ui64 << pos);	// clear pos square
	clearSquare(dest);	// clear dest square

	piece_t t = type;

	if (type == PAWN)
	{
		if (
			(color == WHITE && (dest / 8 == RANK_8))
			||
			(color == BLACK && (dest / 8 == RANK_1))
			)
		{
			t = QUEEN;
		}
	}

	bitboard[color + t] |= 1ui64 << dest;
}


void Board::setSquare(const piece_p pos, const piece_t type, const color color)
{
	clearSquare(pos);
	bitboard[type + color] |= 1ui64 << pos;
}


void Board::clearSquare(const piece_p pos)
{
	uint64_t clearMask = ~(1ui64 << pos);
	for (size_t i = 0; i < NUM_OF_BITBOARDS; i++)
	{
		bitboard[i] &= clearMask;
	}
}


bool chessengine::Board::isKingCheck(const color col) const
{
	const uint64_t kingPosMask = bitboard[col + KING];
	const uint64_t pieceMask = positionMask();
	const color enemyColor = col ^ BLACK;
	const uint64_t enemyPieces = colorPositionMask(enemyColor);
	uint64_t checkMask = kingPosMask;
	piece_p kingPos;

	unsigned long index;
	_BitScanForward64(&index, kingPosMask);
	kingPos = (piece_p)index;
	
	// north
	checkMask <<= 8;
	while (checkMask)
	{
		if (checkMask & pieceMask)
		{
			if ((checkMask & enemyPieces) &&
				(checkMask & (bitboard[enemyColor + ROOK] | bitboard[enemyColor + QUEEN]))
				)
			{
				return true;
			}
			else
			{
				break;
			}
		}
		checkMask <<= 8;
	}

	// east
	checkMask = kingPosMask << 1;
	for (int i = 7 - kingPos % 8; i > 0; i--)
	{
		if (checkMask & pieceMask)
		{
			if ((checkMask & enemyPieces) &&
				(checkMask & (bitboard[enemyColor + ROOK] | bitboard[enemyColor + QUEEN]))
				)
			{
				return true;
			}
			else
			{
				break;
			}
		}
		checkMask <<= 1;
	}

	// south
	checkMask = kingPosMask >> 8;
	while (checkMask)
	{
		if (checkMask & pieceMask)
		{
			if ((checkMask & enemyPieces) &&
				(checkMask & (bitboard[enemyColor + ROOK] | bitboard[enemyColor + QUEEN]))
				)
			{
				return true;
			}
			else
			{
				break;
			}
		}
		checkMask >>= 8;
	}

	// west
	checkMask = kingPosMask >> 1;
	for (int i = kingPos % 8; i > 0; i--)
	{
		if (checkMask & pieceMask)
		{
			if ((checkMask & enemyPieces) &&
				(checkMask & (bitboard[enemyColor + ROOK] | bitboard[enemyColor + QUEEN]))
				)
			{
				return true;
			}
			else
			{
				break;
			}
		}
		checkMask >>= 1;
	}

	// diagonal north east
	checkMask = kingPosMask << 9;
	for (int i = 7 - kingPos % 8; i > 0; i--)
	{
		if (!checkMask)
		{
			break;
		}

		if (checkMask & pieceMask)
		{
			if ((checkMask & enemyPieces) &&
				(checkMask & (bitboard[enemyColor + BISHOP] | bitboard[enemyColor + QUEEN]))
				)
			{
				return true;
			}
			else
			{
				break;
			}
		}
		checkMask <<= 9;
	}

	// diagonal north west
	checkMask = kingPosMask << 7;
	for (int i = kingPos % 8; i > 0; i--)
	{
		if (!checkMask)
		{
			break;
		}

		if (checkMask & pieceMask)
		{
			if ((checkMask & enemyPieces) &&
				(checkMask & (bitboard[enemyColor + BISHOP] | bitboard[enemyColor + QUEEN]))
				)
			{
				return true;
			}
			else
			{
				break;
			}
		}
		checkMask <<= 7;
	}

	// diagonal south east
	checkMask = kingPosMask >> 7;
	for (int i = 7 - kingPos % 8; i > 0; i--)
	{
		if (!checkMask)
		{
			break;
		}

		if (checkMask & pieceMask)
		{
			if ((checkMask & enemyPieces) &&
				(checkMask & (bitboard[enemyColor + BISHOP] | bitboard[enemyColor + QUEEN]))
				)
			{
				return true;
			}
			else
			{
				break;
			}
		}
		checkMask >>= 7;
	}

	// diagonal south west
	checkMask = kingPosMask >> 9;
	for (int i = kingPos % 8; i > 0; i--)
	{
		if (!checkMask)
		{
			break;
		}

		if (checkMask & pieceMask)
		{
			if ((checkMask & enemyPieces) &&
				(checkMask & (bitboard[enemyColor + BISHOP] | bitboard[enemyColor + QUEEN]))
				)
			{
				return true;
			}
			else
			{
				break;
			}
		}
		checkMask >>= 9;
	}

	// knight check
	if (MoveGenerator::knight(kingPos) & bitboard[enemyColor + KNIGHT])
	{
		return true;
	}

	// king check
	if (MoveGenerator::king(kingPos) & bitboard[enemyColor + KING])
	{
		return true;
	}

	// pawn check
	checkMask = 0ui64;
	file f = kingPos % 8;
	if (col == WHITE)
	{
		if (f != A_FILE) checkMask |= kingPosMask << 7; 
		if (f != H_FILE) checkMask |= kingPosMask << 9;
		if (checkMask & bitboard[BLACK + PAWN]) return true;
	}
	else
	{
		if (f != A_FILE) checkMask |= kingPosMask >> 9;
		if (f != H_FILE) checkMask |= kingPosMask >> 7;
		if (checkMask & bitboard[WHITE + PAWN]) return true;
	}

	return false;
}


piece_t chessengine::Board::pieceType(const piece_p pos) const
{
	uint64_t posMask = 1ui64 << pos;

	for (piece_t t = PAWN; t <= KING; t++) {
		if ((bitboard[WHITE + t] & posMask) != 0
			||
			(bitboard[BLACK + t] & posMask) != 0) {
			return t;
		}
	}

	return -1;
}
