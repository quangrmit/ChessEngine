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
#include <chrono>

// random move generation
// #include <ctime>
// #include <cstdlib>
#include "chess-library/include/chess.hpp"
// #include "chessEngine.h"

using std::map;
using std::string;
using std::variant;
using std::vector;

// string defaultFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

// map<char, int> material_points = {{'r', 5}, {'n', 3}, {'b', 3}, {'q', 9}, {'p', 1}, {'k', 200}, {'R', 5}, {'N', 3}, {'B', 3}, {'Q', 9}, {'P', 1}, {'K', 200}};

// extern Move chosen;


void printMap(const map<string, int>& m);

// int spacialAdvantageWithColor(char color, string fen);

// int spacialAdvantage(string fen);

// int centerControlWithColor(char color, string fen);

int centerControl(string fen);

int squareControl(string fen);

int materialEval(string fen);

int eval(string fen);

string fenToKey(const string& fen);

vector<string> split(string s, char del = ' ');

map<string, variant<string, int, vector<std::any>>> minimax(string position, int depth, int primeDepth, map<string, int>& transTable, int& countNodes);






map<string, variant<string, int, vector<std::any>>> alphaBetaPrunning(string position, int depth, int primeDepth, map<string, int>& transTable,int& countNodes, int alpha=std::numeric_limits<int>::min(), int beta=std::numeric_limits<int>::max());

// test call from js
// struct BestMove
// {
//     int score = 0;
//     Move move = Move();
// };

// BestMove minimax(string position, int depth, int dummy);

string bestMove(string fen, int depth);


#endif