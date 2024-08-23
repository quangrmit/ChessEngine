#ifndef _ALPHABETA_H
#define _ALPHABETA_H

#include <algorithm>
#include <any>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

// random move generation
// #include <ctime>
// #include <cstdlib>

#include "chessEngine.h"

using std::map;
using std::string;
using std::variant;

// string defaultFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

// map<char, int> material_points = {{'r', 5}, {'n', 3}, {'b', 3}, {'q', 9}, {'p', 1}, {'k', 200}, {'R', 5}, {'N', 3}, {'B', 3}, {'Q', 9}, {'P', 1}, {'K', 200}};

extern Move chosen;

int eval(string fen);

vector<string> split(string s, char del = ' ');

map<string, variant<string, int, vector<std::any>>> minimax(string position, int depth, int primeDepth);

map<string, variant<string, int, vector<std::any>>> alphaBetaPrunning(string position, int depth, int alpha, int beta);

// test call from js
struct BestMove
{
    int score = 0;
    Move move = Move();
};

// BestMove minimax(string position, int depth, int dummy);

Move bestMove(string fen);

#endif