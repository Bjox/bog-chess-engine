#include "MoveGenerator.h"
#include "Bitops.h"
#include "Validator.h"
#include "MinMax.h"

using namespace chessengine;

std::mutex global_queue_mutex;
std::queue<Node*> global_queue;

MoveGenerator::MoveGenerator(const unsigned int n_threads, const unsigned int max_depth, const Board &board)
	: MAX_THREADS(n_threads), activeThreads(0), baseBoard(board), MAX_DEPTH(max_depth)
{
}


MoveGenerator::~MoveGenerator()
{
}


Node *MoveGenerator::createTree(const color turnColor)
{
	clock_t start_t = clock();
	Node *root = new Node();
	root->setColor(turnColor);
	root->board = baseBoard;

	if (MAX_DEPTH < 3 || MAX_THREADS == 1) // Skip multithreading if depth < 3
	{
		std::cout << "SINGLETHREAD MODE" << std::endl;
		processNodeFull(root);
	}
	else
	{
		std::cout << "MULTITHREAD MODE (" << MAX_THREADS << " threads)" << std::endl;
		global_queue.push(root);

		Node *head = nullptr;

		while (global_queue.size() < MAX_THREADS)	// While num_of_leaf_nodes < num_of_threads
		{
			head = global_queue.front();
			global_queue.pop();

			processNode(head);	// create more nodes

			for (unsigned int i = 0; i < head->fields.num_childs; i++)
			{
				global_queue.push(head->childrenPtr[i]);
			}

			if (global_queue.empty())
			{
				// no legal moves => checkmate
				std::cout << "Queue empty" << std::endl;
				return root;
			}
		}

		std::vector<std::thread*> threads;
		std::vector<Node*> startNodes;
		threads.resize(MAX_THREADS);
		startNodes.resize(MAX_THREADS);
		
		for (unsigned int i = 0; i < MAX_THREADS; i++) {
			startNodes[i] = global_queue.front();
			global_queue.pop();
		}

		for (unsigned int i = 0; i < MAX_THREADS; i++) {
			threads[i] = createThreadWorker(startNodes[i]);
		}

		for (unsigned int i = 0; i < MAX_THREADS; i++) {
			threads[i]->join();
			delete threads[i];
			threads[i] = nullptr;
		}

		MinMax::applyMinMax(root);
	}

	std::cout << std::endl;
	printstat(MAX_DEPTH-1, start_t);

	return root;
}

// Creates a thread for processing a node.
std::thread *MoveGenerator::createThreadWorker(Node *root)
{
	std::thread *worker = new std::thread(&MoveGenerator::processNodeStart, this, root);
	return worker;
}

void chessengine::MoveGenerator::processNodeStart(Node *root)
{
	while (root != nullptr)
	{
		processNodeFull(root);

		global_queue_mutex.lock();	 // START CRITICAL SECTION

		if (global_queue.empty())
		{
			global_queue_mutex.unlock(); // END CRITICAL SECTION
			root = nullptr;
			break;
		}
		root = global_queue.front();
		global_queue.pop();
		std::cout << "\r" << global_queue.size() << " unexplored subtrees    ";

		global_queue_mutex.unlock(); // END CRITICAL SECTION
	}
}

// Create subtree from node.
void MoveGenerator::processNodeFull(Node *root)
{
	//root->board.printFull();
	if (root->fields.depth < MAX_DEPTH)
	{
		// create child nodes
		processNode(root);

		for (unsigned int i = 0; i < root->fields.num_childs; i++)
		{
			// recursively expand tree
			processNodeFull(root->childrenPtr[i]);
		}
	}
	
	if (root->fields.num_childs) // num_childs > 0
	{
		unsigned int num_childs = root->fields.num_childs;
		short bestValue = root->childrenPtr[0]->value;
		short childValue;

		if (root->fields.color == Board::WHITE)
		{
			for (unsigned int i = 0; i < num_childs; i++)
			{
				childValue = root->childrenPtr[i]->value;
				if (childValue > bestValue) bestValue = childValue;
			}
		}
		else // BLACK
		{
			for (unsigned int i = 0; i < num_childs; i++)
			{
				childValue = root->childrenPtr[i]->value;
				if (childValue < bestValue) bestValue = childValue;
			}
		}
		
		root->value = bestValue;

		if (root->fields.depth)	// depth > 0
		{
			for (unsigned int i = 0; i < num_childs; i++)
			{
				delete root->childrenPtr[i];
				root->childrenPtr[i] = nullptr;
			}

			root->fields.num_childs = 0;

			delete[] root->childrenPtr;
			root->childrenPtr = nullptr;
		}
	}
	else // num_childs == 0
	{
		root->validate();
	}

}

// Create child nodes.
void MoveGenerator::processNode(Node *node)
{
	// The board being processed.
	Board board = node->board;

	// the processing color
	const color nodeColor = node->getColor();

	// true if current player is in check.
	const bool check = board.isKingCheck(nodeColor);

	// Counter of legal moves.
	unsigned int num_legalMoves = 0;

	// Iterate through piece types.
	for (piece_t type = Board::PAWN; type <= Board::KING; type++)
	{
		// Table containing the position of every piece of respective type.
		unsigned int pps_count;
		uint8_t *pps = bit_i(board.bitboard[nodeColor + type], pps_count);

		// Iterate through this vector
		for (unsigned int i = 0; i < pps_count; i++)
		{
			// A piece position
			piece_p piecePos = pps[i];

			// Movement mask for the piece being processed.
			uint64_t pieceMovMask = pieceMovementMask(piecePos, type, nodeColor, board);

			// Positions where said piece can move.
			unsigned int pds_count;
			uint8_t *pds = bit_i(pieceMovMask, pds_count);

			// Iterate through the destinations table.
			for (unsigned int j = 0; j < pds_count; j++)
			{
				// Move destination.
				piece_p pieceDest = pds[j];

				// Copy board and perform the move.
				Board candidateMove = board;
				candidateMove.movePiece(piecePos, type, nodeColor, pieceDest);

				// The move must set the player out of check in order to be a valid move.
				if (!candidateMove.isKingCheck(nodeColor)) {
					// Create the child node.
					Node *child = new Node();

					// Set child fields.
					child->setColor(nodeColor ^ Board::BLACK);
					child->fields.depth = node->fields.depth + 1;
					child->fields.position = piecePos;
					child->fields.destination = pieceDest;
					child->fields.piece_t = type;
					child->board = candidateMove;

					// Add child to node.
					node->addChild(child);

					// Increment legal moves counter.
					num_legalMoves++;

				}
			}

			delete[] pds;
			pds = nullptr;
		}

		delete[] pps;
		pps = nullptr;
	}

	// no legal moves and check => checkmate
	if (num_legalMoves == 0 && check)
	{
		node->value = node->getColor() == Board::WHITE ? SHRT_MIN : SHRT_MAX;
		node->fields.validated = 1;
	}
}


void MoveGenerator::printstat(const unsigned int &depth, const clock_t &start_t)
{
	clock_t clock_diff = clock() - start_t;
	unsigned long ms = clock_diff / (CLOCKS_PER_SEC / 1000);
	std::cout << "Depth " << depth+1 << " completed\telapsed time " << ms << " ms" << std::endl;
}


uint64_t MoveGenerator::pieceMovementMask(const piece_p pos, const piece_t type, const color color, const Board &board)
{
	uint64_t movementMask = 0;
	uint64_t pieceMask = board.positionMask();
	uint64_t colorMask = board.colorPositionMask(color);

	switch (type) {
	case Board::PAWN:
		movementMask = pawn(pos, color, pieceMask, board.colorPositionMask(color ^ Board::BLACK));
		break;

	case Board::ROOK:
		movementMask = rook(pos, color, pieceMask, colorMask);
		break;

	case Board::BISHOP:
		movementMask = bishop(pos, color, pieceMask, colorMask);
		break;

	case Board::KNIGHT:
		movementMask = knight(pos) & ~board.colorPositionMask(color);
		break;

	case Board::QUEEN:
		movementMask = queen(pos, color, pieceMask, colorMask);
		break;

	case Board::KING:
		movementMask = king(pos) & ~board.colorPositionMask(color);
		break;
	}

	return movementMask;
}


uint64_t MoveGenerator::pawn(const piece_p pos, const color color, const uint64_t pieceMask, const uint64_t otherColorMask)
{
	uint64_t movMask = 0x100;
	uint64_t capMask = 0x280;
	uint64_t posMask = 1ui64 << pos;
	rank rank = pos / 8;

	if (color == Board::WHITE)
	{
		if (rank == 1)
			movMask = 0x10100;

		movMask <<= pos;

		if ((pieceMask & (posMask << 8)))
			movMask = 0;

		else if ((pieceMask & (posMask << 16)))
			movMask &= ~(posMask << 16);

		capMask <<= pos;
	}
	else
	{
		movMask = rank == 6 ? 0x80800000000000ui64 : 0x80000000000000ui64;
		capMask = 0x140000000000000ui64;

		movMask >>= 63 - pos;

		if ((pieceMask & (posMask >> 8)))
			movMask = 0;

		else if ((pieceMask & (posMask >> 16)))
			movMask &= ~(posMask >> 16);

		capMask >>= 63 - pos;
	}

	applyBorderMasks(capMask, posMask);
	capMask &= otherColorMask;
	movMask |= capMask;

	return movMask;
}


uint64_t MoveGenerator::rook(const piece_p pos, const color color, const uint64_t pieceMask, const uint64_t colorMask)
{
	uint64_t movMask = 0;
	uint64_t posMask = 1ui64 << pos;

	// north
	uint64_t checkMask = posMask << 8;

	while (checkMask) {
		if ((checkMask & pieceMask) == 0)
			movMask |= checkMask;

		else {
			movMask |= checkMask & ~colorMask;
			break;
		}

		checkMask <<= 8;
	}

	// south
	checkMask = posMask >> 8;

	while (checkMask != 0) {
		if ((checkMask & pieceMask) == 0) {
			movMask |= checkMask;
		}
		else {
			movMask |= checkMask & ~colorMask;
			break;
		}

		checkMask >>= 8;
	}

	// east
	checkMask = posMask;
	int n = 7 - pos % 8;

	for (int i = 0; i < n; i++) {
		checkMask <<= 1;

		if ((checkMask & pieceMask) == 0) {
			movMask |= checkMask;
		}
		else {
			movMask |= checkMask & ~colorMask;
			break;
		}
	}

	// west
	checkMask = posMask;
	n = pos % 8;

	for (int i = 0; i < n; i++) {
		checkMask >>= 1;

		if ((checkMask & pieceMask) == 0) {
			movMask |= checkMask;
		}
		else {
			movMask |= checkMask & ~colorMask;
			break;
		}
	}

	return movMask;
}

// slower than rook()
uint64_t MoveGenerator::rook2(const piece_p pos, const color color, const uint64_t pieceMask, const uint64_t colorMask)
{
	const uint64_t posMask = 1ui64 << pos;
	uint64_t movMask = 0;
	uint64_t dir;
	int64_t col;

	// north
	dir = 0x101010101010101ui64 << pos;
	dir <<= 8;
	col = dir & pieceMask;
	col ^= -col;
	col &= dir;
	dir &= ~col;
	movMask |= dir;

	// south
	dir = 0x8080808080808080ui64 >> (63 - pos);
	dir >>= 8;
	uint64_t rev = pieceMask & dir;
	reverseBits64(rev);
	rev ^= -(int64_t)rev;
	reverseBits64(rev);
	dir &= ~rev;
	movMask |= dir;

	// east
	dir = 0xFFui64 << (pos / 8) * 8;
	dir &= (0xFFui64 << pos) << 1;
	col = dir & pieceMask;
	col ^= -col;
	col &= dir;
	dir &= ~col;
	movMask |= dir;

	// west
	file f = (pos / 8) * 8;
	dir = 0xFFui64 << f;
	dir &= (0xFF00000000000000ui64 >> (63 - pos)) >> 1;
	uint8_t f_slice = (uint8_t)((pieceMask & dir) >> f);
	f_slice = bitReverseTable8[f_slice];
	f_slice ^= -f_slice;
	f_slice = bitReverseTable8[f_slice];
	col = f_slice;
	col <<= f;
	dir &= ~col;
	movMask |= dir;

	movMask &= ~colorMask;

	return movMask;
}


uint64_t MoveGenerator::bishop(const piece_p pos, const color color, const uint64_t pieceMask, const uint64_t colorMask)
{
	uint64_t movMask = 0ui64;
	uint64_t posMask = 1ui64 << pos;

	// north-east
	uint64_t checkMask = posMask;
	int n = 7 - pos % 8;

	for (int i = 0; i < n; i++)
	{
		checkMask <<= 9;

		if (checkMask == 0)
			break;

		if ((checkMask & pieceMask) == 0)
			movMask |= checkMask;

		else
		{
			movMask |= checkMask & ~colorMask;
			break;
		}
	}

	// south-east
	checkMask = posMask;

	for (int i = 0; i < n; i++)
	{
		checkMask >>= 7;

		if (checkMask == 0)
			break;

		if ((checkMask & pieceMask) == 0)
			movMask |= checkMask;

		else
		{
			movMask |= checkMask & ~colorMask;
			break;
		}
	}

	// north-west
	checkMask = posMask;
	n = pos % 8;

	for (int i = 0; i < n; i++)
	{
		checkMask <<= 7;

		if (checkMask == 0)
			break;

		if ((checkMask & pieceMask) == 0)
			movMask |= checkMask;

		else
		{
			movMask |= checkMask & ~colorMask;
			break;
		}
	}

	// south-west
	checkMask = posMask;

	for (int i = 0; i < n; i++)
	{
		checkMask >>= 9;

		if (checkMask == 0)
			break;

		if ((checkMask & pieceMask) == 0)
			movMask |= checkMask;

		else
		{
			movMask |= checkMask & ~colorMask;
			break;
		}
	}

	return movMask;
}


uint64_t MoveGenerator::knight(const piece_p pos)
{
	return basemovement(0x442800000028440ui64, pos);
}


uint64_t MoveGenerator::queen(const piece_p pos, const color color, const uint64_t pieceMask, const uint64_t colorMask)
{
	return rook(pos, color, pieceMask, colorMask) | bishop(pos, color, pieceMask, colorMask);
}


uint64_t MoveGenerator::king(const piece_p pos)
{
	return basemovement(0x8380000000000382ui64, pos);
}


uint64_t MoveGenerator::basemovement(const uint64_t base, const piece_p pos)
{
	uint64_t posMask = 1ui64 << pos;
	uint64_t legalMovement = base;
	uint64_t movementMask = ~(0ui64);

	rotl64(legalMovement, pos);
	applyBorderMasks(movementMask, posMask);
	legalMovement &= movementMask;

	return legalMovement;
}


void MoveGenerator::applyBorderMasks(uint64_t &mask, const uint64_t posMask)
{
	if (posMask & Board::WEST_BORDER)  mask &= Board::WEST;
	if (posMask & Board::EAST_BORDER)  mask &= Board::EAST;
	if (posMask & Board::NORTH_BORDER) mask &= Board::NORTH;
	if (posMask & Board::SOUTH_BORDER) mask &= Board::SOUTH;
}


