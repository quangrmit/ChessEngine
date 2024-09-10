#include "alpha_beta.h"

#include "chess-library/include/chess.hpp"

using namespace chess;

chess::Move chosen;

vector<string> split(string s, char del) {
    vector<string> res;
    std::stringstream ss(s);
    string word;
    while (!ss.eof()) {
        getline(ss, word, del);
        res.push_back(word);
    }
    return res;
}

string fenToKey(const string& fen) {
    vector<string> vs = split(fen, ' ');
    string res = vs[0];
    for (int i = 1; i < 3; i++) {
        res += " ";
        res += vs[i];
    }
    return res;
}

void printMap(const map<string, int>& m) {
    std::cout << "{" << std::endl;
    for (auto const& [key, val] : m) {
        std::cout << key << ": " << val << "," << std::endl;
    }
    std::cout << "}" << std::endl;
}

int squareControl(string fen) {
    const int N = 64;
    int sqs[N];
    for (int i = 0; i < N; i ++){
        sqs[i] = i;
    }

    Board board = Board(fen);
    int count = 0;

    for (int i = 0; i < N; i ++){
        if(board.isAttacked(sqs[i], Color::WHITE)){
            count ++;
        }
        if (board.isAttacked(sqs[i], Color::BLACK)) {
            count --;
        }
    }
    return count;
}

int centerControl(string fen) {
    vector<Square::underlying> center = {Square::underlying::SQ_E4, Square::underlying::SQ_E5, Square::underlying::SQ_D4, Square::underlying::SQ_D5};

    Board board = Board(fen);

    int count = 0;

    for (Square::underlying sq : center) {
        count += board.timesAttacked(sq, Color::WHITE);
        count -= board.timesAttacked(sq, Color::BLACK);
    }
    return count;
}

int materialEval(string fen) {
    map<char, int> materialPoints = {{'r', 5}, {'n', 3}, {'b', 3}, {'q', 9}, {'p', 1}, {'k', 200}, {'R', 5}, {'N', 3}, {'B', 3}, {'Q', 9}, {'P', 1}, {'K', 200}};

    int materialWeight = 0;
    int numWhitePieces = 0;
    int numBlackPieces = 0;
    int turn;
    if (split(fen)[1][0] == 'w') {
        turn = 1;
    } else {
        turn = -1;
    }
    int whiteMaterialWeight = 0;
    int blackMaterialWeight = 0;

    vector<string> materials = split(split(fen)[0], '/');

    string allMaterial = "";
    for (string material : materials) {
        allMaterial += material;
    }
    for (char c : allMaterial) {
        if (isdigit(c)) {
            continue;
        }
        if (isupper(c)) {
            numWhitePieces += 1;
            whiteMaterialWeight += materialPoints[c];
        } else {
            numBlackPieces += 1;
            blackMaterialWeight += materialPoints[c];
        }

        // return abs(whiteMaterialWeight - blackMaterialWeight) * (numWhitePieces - numBlackPieces) * turn;
    }
    // return 0;
    return whiteMaterialWeight - blackMaterialWeight;
}

int eval(string fen) {
    int centerControlWeight = 1;

    Board board = Board(fen);
    std::pair<GameResultReason, GameResult> res = board.isGameOver();
    if (static_cast<int>(res.first) == 0 && static_cast<int>(res.second) == 1){
        return -1000;
    }
    if (static_cast<int>(res.first) == 0 && static_cast<int>(res.second) == 0){
        return 1000;
    }


    return 2 * materialEval(fen) + centerControl(fen) + squareControl(fen);
}

map<string, variant<string, int, vector<std::any>>> alphaBetaPrunning(string position, int depth, int primeDepth, map<string, int>& transTable, int& countNodes, int alpha, int beta) {
    countNodes++;

    if (transTable.find(fenToKey(position)) != transTable.end()) {
        // position already exists in transtable
        vector<std::any> noChildren;
        map<string, variant<string, int, vector<std::any>>> obj = {
            {"fen", position},
            {"eval", transTable.at(fenToKey(position))},
            {"children", noChildren},

        };
        return obj;
    }
    if (depth == 0) {
        vector<std::any> noChildren;
        map<string, variant<string, int, vector<std::any>>> obj = {
            {"fen", position},
            {"eval", eval(position)},
            {"children", noChildren},

        };
        return obj;
    }
    Board board = Board(position);
    Movelist moves;
    movegen::legalmoves(moves, board);

    if (split(position).at(1) == "w") {
        int maxScore = -std::numeric_limits<int>::max();
        // int count = 0;
        vector<std::any> children = {};

        for (int i = 0; i < moves.size(); i++) {
            const auto move = moves[i];
            board.makeMove(move);
            string newPostion = board.getFen();
            auto curr = alphaBetaPrunning(newPostion, depth - 1, primeDepth, transTable, countNodes);
            children.push_back(curr);
            int currScore = std::get<int>(curr.at("eval"));
            if (currScore > maxScore) {
                maxScore = currScore;
                if (depth == primeDepth) {
                    chosen = move;
                }
            }
            alpha = std::max(alpha, currScore);
            if (beta <= alpha) break;

            board.unmakeMove(move);
        }
        transTable[fenToKey(position)] = maxScore;

        map<string, variant<string, int, vector<std::any>>> returnMap = {
            {"fen", position},
            {"eval", maxScore},
            {"children", children}};
        return returnMap;
    } else {
        int minScore = std::numeric_limits<int>::max();
        vector<std::any> children;

        for (int i = 0; i < moves.size(); i++) {
            const auto move = moves[i];
            board.makeMove(move);
            string newPostion = board.getFen();
            auto curr = alphaBetaPrunning(newPostion, depth - 1, primeDepth, transTable, countNodes);
            children.push_back(curr);
            int currScore = std::get<int>(curr.at("eval"));
            if (currScore < minScore) {
                minScore = currScore;
                if (depth == primeDepth) {
                    chosen = move;
                }
            }
            beta = std::min(beta, currScore);
            if (beta <= alpha) break;

            board.unmakeMove(move);
        }
        transTable[fenToKey(position)] = minScore;

        map<string, variant<string, int, vector<std::any>>> returnMap = {
            {"fen", position},
            {"eval", minScore},
            {"children", children}};
        return returnMap;
    }
}

string bestMove(string fen, int depth) {
    Board board = Board(fen);

    // Find best move
    // vector<Move> moves = c->_moves();

    map<string, int> transTable = {};

    int count = 0;

    auto result = minimax(fen, depth, depth, transTable, count);

    return uci::moveToUci(chosen);
}

map<string, variant<string, int, vector<std::any>>> minimax(string position, int depth, int primeDepth, map<string, int>& transTable, int& countNode) {
    countNode++;

    if (transTable.find(fenToKey(position)) != transTable.end()) {
        // position already exists in transtable
        vector<std::any> noChildren;
        map<string, variant<string, int, vector<std::any>>> obj = {
            {"fen", position},
            {"eval", transTable.at(fenToKey(position))},
            {"children", noChildren},

        };
        return obj;
    }
    if (depth == 0) {
        vector<std::any> noChildren;
        map<string, variant<string, int, vector<std::any>>> obj = {
            {"fen", position},
            {"eval", eval(position)},
            {"children", noChildren},

        };
        return obj;
    }
    Board board = Board(position);
    Movelist moves;
    movegen::legalmoves(moves, board);

    if (split(position).at(1) == "w") {
        int maxScore = -std::numeric_limits<int>::max();
        // int count = 0;
        vector<std::any> children = {};

        for (int i = 0; i < moves.size(); i++) {
            const auto move = moves[i];
            board.makeMove(move);
            string newPostion = board.getFen();
            auto curr = minimax(newPostion, depth - 1, primeDepth, transTable, countNode);
            children.push_back(curr);
            int currScore = std::get<int>(curr.at("eval"));
            if (currScore > maxScore) {
                maxScore = currScore;
                if (depth == primeDepth) {
                    chosen = move;
                }
            }

            board.unmakeMove(move);
        }
        transTable[fenToKey(position)] = maxScore;

        map<string, variant<string, int, vector<std::any>>> returnMap = {
            {"fen", position},
            {"eval", maxScore},
            {"children", children}};
        return returnMap;
    } else {
        int minScore = std::numeric_limits<int>::max();
        vector<std::any> children;

        for (int i = 0; i < moves.size(); i++) {
            const auto move = moves[i];
            board.makeMove(move);
            string newPostion = board.getFen();
            auto curr = minimax(newPostion, depth - 1, primeDepth, transTable, countNode);
            children.push_back(curr);
            int currScore = std::get<int>(curr.at("eval"));
            if (currScore < minScore) {
                minScore = currScore;
                if (depth == primeDepth) {
                    chosen = move;
                }
            }

            board.unmakeMove(move);
        }
        transTable[fenToKey(position)] = minScore;

        map<string, variant<string, int, vector<std::any>>> returnMap = {
            {"fen", position},
            {"eval", minScore},
            {"children", children}};
        return returnMap;
    }
}