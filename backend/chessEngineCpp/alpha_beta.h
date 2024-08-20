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

#include "chessEngine.h"

using std::map;
using std::string;
using std::variant;

// string defaultFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

int eval(string fen);

vector<string> split(string s, char del = ' ');

map<string, variant<string, int, vector<std::any>>> minimax(string position, int depth);

map<string, variant<string, int, vector<std::any>>> alphaBetaPrunning(string position, int depth, int alpha, int beta);

#endif