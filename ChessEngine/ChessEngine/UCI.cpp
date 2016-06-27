#include "UCI.h"
#include "MoveGenerator.h"
#include "Node.h"
#include <iostream>
#include <fstream>

using namespace chessengine;
using namespace std;

UCI::UCI(unsigned int threads, unsigned int depth)
	: threads(threads), depth(depth)
{
}


UCI::~UCI()
{
}


void UCI::start()
{
	cout.setf(ios::unitbuf);
	srand((unsigned int)time(nullptr));

	string line;
	ofstream stream;

	while (getline(cin, line)) {

		stream.open("log.txt", ios::app);
		stream << line << endl;
		stream.close();

		if (line == "uci") {
			cout << "id name Bogfish" << endl;
			cout << "id author Bjornar W. Alvestad" << endl;
			cout << "uciok" << endl;
		}
		else if (line == "quit") {
			break;
		}
		else if (line == "isready") {
			cout << "readyok" << endl;
		}

		else if (line.substr(0, 3) == "go ") {
			MoveGenerator generator(threads, depth, board);
			Node* root = generator.createTree(turnColor);

			std::vector<Node*> bestMoves = root->getBestChildren();

			// if bestMoves.size == 0, checkmate

			unsigned int rnd = rand() % bestMoves.size();
			Node *n = bestMoves[rnd];

			unsigned int pos = n->fields.position;
			unsigned int dest = n->fields.destination;

			std::string bestmovestr("bestmove " + chessengine::Board::squareNotation(pos) + chessengine::Board::squareNotation(dest));

			stream.open("log.txt", ios::app);
			stream << bestmovestr << endl;
			stream.close();

			delete root;

			cout << bestmovestr << endl;
		}

		else if (line.substr(0, 13) == "position fen ") {
			string fenstr = line.substr(13);
			board = createBoardFromFen(fenstr, turnColor);
		}

	}

}

Board UCI::createBoardFromFen(const string& fenstr, color& activeColor)
{
	Board board;

	uint8_t r = Board::RANK_8;
	file f = Board::A_FILE;

	for (auto it = fenstr.begin(); it != fenstr.end(); it++) {
		char ch = *it;

		if (ch == ' ') {
			it++; // iterate to active color
			activeColor = (*it == 'w' || *it == 'W') ? Board::WHITE : Board::BLACK;
			break;
		}
		if (ch == '/') {
			r--;
			f = Board::A_FILE;
			continue;
		}

		color col = isupper(ch) ? Board::WHITE : Board::BLACK;

		if (isdigit(ch)) {
			int freesquares = int(ch) - 48; // convert char to int
			f += freesquares;
			continue;
		}

		piece_t t;
		switch (ch) {
		case 'p': case 'P':
			t = Board::PAWN; break;
		case 'r': case 'R':
			t = Board::ROOK; break;
		case 'n': case 'N':
			t = Board::KNIGHT; break;
		case 'b': case 'B':
			t = Board::BISHOP; break;
		case 'q': case 'Q':
			t = Board::QUEEN; break;
		case 'k': case 'K':
			t = Board::KING; break;
		}

		board.setSquare(r * 8 + f, t, col);

		f++; // goto next file
	}

	return board;
}


