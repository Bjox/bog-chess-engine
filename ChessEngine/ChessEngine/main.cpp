#include <iostream>
#include <bitset>
#include <string>
#include <vector>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <time.h>

#include "Board.h"
#include "Node.h"
#include "MoveGenerator.h"
#include "Bitops.h"
#include "Validator.h"
#include "MinMax.h"
#include "UCI.h"

using namespace std;
using namespace chessengine;

const piece_p a1 = 0;
const piece_p b1 = 1;
const piece_p c1 = 2;
const piece_p d1 = 3;
const piece_p e1 = 4;
const piece_p f1 = 5;
const piece_p g1 = 6;
const piece_p h1 = 7;
const piece_p a2 = 8;
const piece_p b2 = 9;
const piece_p c2 = 10;
const piece_p d2 = 11;
const piece_p e2 = 12;
const piece_p f2 = 13;
const piece_p g2 = 14;
const piece_p h2 = 15;
const piece_p a3 = 16;
const piece_p b3 = 17;
const piece_p c3 = 18;
const piece_p d3 = 19;
const piece_p e3 = 20;
const piece_p f3 = 21;
const piece_p g3 = 22;
const piece_p h3 = 23;
const piece_p a4 = 24;
const piece_p b4 = 25;
const piece_p c4 = 26;
const piece_p d4 = 27;
const piece_p e4 = 28;
const piece_p f4 = 29;
const piece_p g4 = 30;
const piece_p h4 = 31;
const piece_p a5 = 32;
const piece_p b5 = 33;
const piece_p c5 = 34;
const piece_p d5 = 35;
const piece_p e5 = 36;
const piece_p f5 = 37;
const piece_p g5 = 38;
const piece_p h5 = 39;
const piece_p a6 = 40;
const piece_p b6 = 41;
const piece_p c6 = 42;
const piece_p d6 = 43;
const piece_p e6 = 44;
const piece_p f6 = 45;
const piece_p g6 = 46;
const piece_p h6 = 47;
const piece_p a7 = 48;
const piece_p b7 = 49;
const piece_p c7 = 50;
const piece_p d7 = 51;
const piece_p e7 = 52;
const piece_p f7 = 53;
const piece_p g7 = 54;
const piece_p h7 = 55;
const piece_p a8 = 56;
const piece_p b8 = 57;
const piece_p c8 = 58;
const piece_p d8 = 59;
const piece_p e8 = 60;
const piece_p f8 = 61;
const piece_p g8 = 62;
const piece_p h8 = 63;

const piece_t pawn = Board::PAWN;
const piece_t rook = Board::ROOK;
const piece_t knight = Board::KNIGHT;
const piece_t bishop = Board::BISHOP;
const piece_t king = Board::KING;
const piece_t queen = Board::QUEEN;

const color white = Board::WHITE;
const color black = Board::BLACK;

std::string squareNotation(const piece_p pos)
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

unsigned int posNotation(const std::string &in)
{
	unsigned int pos = 0;
	pos += in[0] - 'a';
	pos += (in[1] - '1') * 8;
	return pos;
}

void computergame(unsigned int thread_count, unsigned int depth)
{
	srand((unsigned int)time(nullptr));
	Board board;
	board.init();
	color turn = Board::WHITE;

	board.printFull();

	while (1)
	{
		MoveGenerator generator(thread_count, depth, board);
		Node *root = generator.createTree(turn);

		std::vector<Node*> bestMoves = root->getBestChildren();
		if (bestMoves.size() == 0)
		{
			// checkmate
			delete root;
			break;
		}

		unsigned int rnd = rand() % bestMoves.size();
		Node *n = bestMoves[rnd];

		unsigned int pos = n->fields.position;
		unsigned int dest = n->fields.destination;
		piece_t t = n->fields.piece_t;

		//system("pause");
		system("cls");
		board.movePiece(pos, t, turn, dest);
		board.printFull();
		cout << squareNotation(pos) << " " << Board::PIECE_NAME[t] << " to " << squareNotation(dest) << endl;
		
		int boardVal = Validator::validate(board);
		cout << "Board value: " << boardVal << (boardVal > 0 ? " WHITE" : (boardVal < 0 ? " BLACK" : " EVEN")) << endl;

		turn ^= Board::BLACK;

		delete root;
	}
}

void test(unsigned int thread_count, unsigned int depth)
{
	srand((unsigned int)time(nullptr)); // seed

	Board board; // create board
	board.init(); // init pieces

	color turn = white;

	MoveGenerator generator(thread_count, depth, board);
	Node *root = generator.createTree(turn);

	std::vector<Node*> bestMoves = root->getBestChildren();

	cout << "Root value: " << root->value << endl;

	if (bestMoves.size() == 0)
	{
		// checkmate
		delete root;
		return;
	}

	for (auto it = bestMoves.begin(); it != bestMoves.end(); it++)
	{
		Node *n = *it;

		unsigned int pos = n->fields.position;
		unsigned int dest = n->fields.destination;
		piece_t t = n->fields.piece_t;

		cout << squareNotation(pos) << " " << Board::PIECE_NAME[t] << " to " << squareNotation(dest) << endl;
	}
	

	delete root;
}


void play(unsigned int thread_count, unsigned int depth) {
	UCI uci(thread_count, depth);
	uci.start();
}


int main(int argc, char** argv)
{
	int thread_count = 16;
	int depth = 5; // TODO: 5 = OK, 6 = HASSARD. must be a bug somewhere


	// first argument: number of threads
	if (argc > 1) {
		thread_count = std::atoi(argv[1]);
	}
	// second argument: max search depth
	if (argc > 2) {
		depth = std::atoi(argv[2]);
	}

	play(thread_count, depth);
	//test(thread_count, depth);
	//computergame(thread_count, depth);

	system("pause");

	return 0;
}

