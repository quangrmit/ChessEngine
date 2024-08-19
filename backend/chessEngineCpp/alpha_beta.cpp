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

string defaultFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

int eval(string fen = defaultFen) {
    return 0;
}

vector<string> split(string s, char del = ' ') {
    vector<string> res;
    std::stringstream ss(s);
    string word;
    while (!ss.eof()) {
        getline(ss, word, del);
        res.push_back(word);
    }
    return res;
}

map<string, variant<string, int, vector<std::any>>> minimax(string position, int depth) {
    if (depth == 0) {
        vector<std::any> noChildren;
        map<string, variant<string, int, vector<std::any>>> obj = {
            {"fen", position},
            {"eval", eval(position)},
            {"children", noChildren},

        };
        return obj;
    }

    ChessEngine* c = new ChessEngine(position);
    if (split(position).at(1) == "w") {
        int maxScore = -std::numeric_limits<int>::max();
        int count = 0;
        vector<std::any> children = {};
        for (Move move : c->_moves()) {
            map<string, string> moveToMake = {
                {"from", algebraic(move.from)},
                {"to", algebraic(move.to)},
            };
            string newPosition = c->move(
                                      moveToMake)
                                     .after;
            auto curr = minimax(newPosition, depth - 1);
            children.push_back(curr);
            int currScore = std::get<int>(curr.at("eval"));
            int maxScore = std::max(maxScore, currScore);

            // Remember to undomove
            c->_undoMove();
        }
        map<string, variant<string, int, vector<std::any>>> returnMap = {
            {"fen", position},
            {"eval", maxScore},
            {"children", children}};
        return returnMap;
    } else {
        int minScore = std::numeric_limits<int>::max();
        vector<std::any> children;

        for (Move move : c->_moves()) {
            map<string, string> moveToMake = {
                {"from", algebraic(move.from)},
                {"to", algebraic(move.to)},
            };
            string newPosition = c->move(
                                      moveToMake)
                                     .after;
            auto curr = minimax(newPosition, depth - 1);
            children.push_back(curr);
            int currScore = std::get<int>(curr.at("eval"));
            int minScore = std::min(minScore, currScore);

            // Remember to undomove
            c->_undoMove();
        }
        map<string, variant<string, int, vector<std::any>>> returnMap = {
            {"fen", position},
            {"eval", minScore},
            {"children", children}};
        return returnMap;
    }
}



map<string, variant<string, int, vector<std::any>>> alphaBetaPrunning(string position, int depth, int alpha, int beta) {
    if (depth == 0) {
        vector<std::any> noChildren;
        map<string, variant<string, int, vector<std::any>>> obj = {
            {"fen", position},
            {"eval", eval(position)},
            {"children", noChildren},

        };
        return obj;
    }

    ChessEngine* c = new ChessEngine(position);
    if (split(position).at(1) == "w") {
        int maxScore = -std::numeric_limits<int>::max();
        int count = 0;
        vector<std::any> children = {};
        for (Move move : c->_moves()) {
            map<string, string> moveToMake = {
                {"from", algebraic(move.from)},
                {"to", algebraic(move.to)},
            };
            string newPosition = c->move(
                                      moveToMake)
                                     .after;
            auto curr = minimax(newPosition, depth - 1);
            children.push_back(curr);
            int currScore = std::get<int>(curr.at("eval"));
            int maxScore = std::max(maxScore, currScore);
            alpha = std::max(alpha, currScore);
            if (beta <= alpha) break;

            // Remember to undomove
            c->_undoMove();
        }
        map<string, variant<string, int, vector<std::any>>> returnMap = {
            {"fen", position},
            {"eval", maxScore},
            {"children", children}};
        return returnMap;
    } else {
        int minScore = std::numeric_limits<int>::max();
        vector<std::any> children;

        for (Move move : c->_moves()) {
            map<string, string> moveToMake = {
                {"from", algebraic(move.from)},
                {"to", algebraic(move.to)},
            };
            string newPosition = c->move(
                                      moveToMake)
                                     .after;
            auto curr = minimax(newPosition, depth - 1);
            children.push_back(curr);
            int currScore = std::get<int>(curr.at("eval"));
            int minScore = std::min(minScore, currScore);

            beta = std::min(beta, currScore);
            if (beta <= alpha) break;

            // Remember to undomove
            c->_undoMove();
        }
        map<string, variant<string, int, vector<std::any>>> returnMap = {
            {"fen", position},
            {"eval", minScore},
            {"children", children}};
        return returnMap;
    }
}
