#pragma once
#include <cstdint>
#include <time.h>
#include <bitset>
#include <queue>
#include <stack>
#include <thread>
#include <mutex>
#include <climits>
#include <iostream>
#include <thread>
#include "Board.h"
#include "Node.h"

namespace chessengine
{

	class MoveGenerator
	{

	public:
		MoveGenerator(const unsigned int n_threads, const unsigned int max_depth, const Board &board);
		~MoveGenerator();

		Node *createTree(const color turnColor);
		uint64_t pieceMovementMask(const piece_p pos, const piece_t type, const color color, const Board &board);

		static uint64_t pawn(const piece_p, const color, const uint64_t pieceMask, const uint64_t otherColorMask);
		static uint64_t rook(const piece_p, const color, const uint64_t pieceMask, const uint64_t colorMask);
		static uint64_t rook2(const piece_p, const color, const uint64_t pieceMask, const uint64_t colorMask);
		static uint64_t bishop(const piece_p, const color, const uint64_t pieceMask, const uint64_t colorMask);
		static uint64_t knight(const piece_p);
		static uint64_t queen(const piece_p, const color, const uint64_t pieceMask, const uint64_t colorMask);
		static uint64_t king(const piece_p);

	private:
		static uint64_t basemovement(const uint64_t, const piece_p);
		static void applyBorderMasks(uint64_t &, const uint64_t);
		static void printstat(const unsigned int &, const clock_t &);

		void processNode(Node *root);
		void processNodeFull(Node *root);
		void processNodeStart(Node *root);
		std::thread *createThreadWorker(Node *root);

		const unsigned int MAX_THREADS;
		const unsigned int MAX_DEPTH;
		const Board &baseBoard;
		unsigned int activeThreads;

	};

}


