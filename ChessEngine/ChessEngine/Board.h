#pragma once
#include <cstdint>
#include <string>
#include <bitset>
#include <string>
#include <iostream>
#include <algorithm>

namespace chessengine
{
	typedef int8_t rank;		// Board rank.
	typedef int8_t file;		// Board file.
	typedef int8_t piece_t;		// Piece type.
	typedef int8_t color;		// Color (white/black). color XOR 6 to toggle between white and black. (color ^ Board::BLACK)
	typedef uint8_t piece_p;	// Piece position.

	class Board
	{

	public:

		// Board size

		static const unsigned int NUM_OF_BITBOARDS = 12;
		static const unsigned int NUM_OF_RANKS = 8;
		static const unsigned int NUM_OF_FILES = 8;

		// Files

		static const file A_FILE = 0;
		static const file B_FILE = 1;
		static const file C_FILE = 2;
		static const file D_FILE = 3;
		static const file E_FILE = 4;
		static const file F_FILE = 5;
		static const file G_FILE = 6;
		static const file H_FILE = 7;

		// Ranks

		static const rank RANK_1 = 0;
		static const rank RANK_2 = 1;
		static const rank RANK_3 = 2;
		static const rank RANK_4 = 3;
		static const rank RANK_5 = 4;
		static const rank RANK_6 = 5;
		static const rank RANK_7 = 6;
		static const rank RANK_8 = 7;

		// Piece rank values at game start

		static const uint64_t PAWN_RANK_VAL = 0xFF;
		static const uint64_t ROOK_RANK_VAL = 0x81;
		static const uint64_t KNIGHT_RANK_VAL = 0x42;
		static const uint64_t BISHOP_RANK_VAL = 0x24;
		static const uint64_t QUEEN_RANK_VAL = 0x08;
		static const uint64_t KING_RANK_VAL = 0x10;

		// Color

		static const color WHITE = 0;
		static const color BLACK = 6;

		// Game pieces

		static const piece_t PAWN = 0;
		static const piece_t ROOK = 1;
		static const piece_t KNIGHT = 2;
		static const piece_t BISHOP = 3;
		static const piece_t QUEEN = 4;
		static const piece_t KING = 5;

		static const unsigned int PIECE_VALUE[6];
		static const std::string PIECE_NAME[6];

		// Board border masks

		static const uint64_t WEST = 0xF0F0F0F0F0F0F0Fui64;
		static const uint64_t NORTH = 0xFFFFFFFF00000000ui64;
		static const uint64_t EAST = ~WEST;
		static const uint64_t SOUTH = ~NORTH;

		static const uint64_t WEST_BORDER = 0x303030303030303ui64;
		static const uint64_t NORTH_BORDER = 0xFFFF000000000000ui64;
		static const uint64_t EAST_BORDER = WEST_BORDER << 6;
		static const uint64_t SOUTH_BORDER = NORTH_BORDER >> 48;


		// Bitboards containing the position of all game pieces.
		uint64_t bitboard[NUM_OF_BITBOARDS];

		Board();
		~Board();

		void init();
		uint64_t positionMask() const;
		uint64_t colorPositionMask(const color &color) const;
		color pieceColor(const uint64_t &posmask) const;
		void movePiece(const piece_p pos, const piece_t type, const color color, const piece_p destination);
		void setSquare(const piece_p pos, const piece_t type, const color color);
		void clearSquare(const piece_p pos);
		bool isKingCheck(const color color) const;
		piece_t pieceType(const piece_p pos) const;

		static void print(const uint64_t &);
		void printFull() const;
		static void shiftToRank(uint64_t &, const rank);
		static uint64_t shiftToRankCopy(const uint64_t &, const rank);
		static std::string squareNotation(const piece_p pos);

	private:

	};

}