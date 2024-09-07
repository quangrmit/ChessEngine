#include "alpha_beta.h"

Move chosen = Move();

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

string fenToKey(const string& fen){
    vector<string> vs = split(fen, ' ');
    string res = vs[0];
    for (int i = 1; i < 3; i++){
        res += " ";
        res += vs[i];
    }
    return res;
}

int spacialAdvantageWithColor(char color, string fen) {
    // This shows how many squares the pieces control

    int count = 0;

    // split and change the fen for move generation
    vector<string> sp = split(fen, ' ');

    sp[1] = string(1, color);

    string res = sp[0];

    for (int i = 1; i < sp.size(); i++) {
        res += " ";
        res += sp[i];
    }

    ChessEngine c = ChessEngine(res);
    vector<Move> moves = c._moves();

    for (Move move : moves) {
        int currPiecePos = move.from;
        for (auto const& [k, v] : Ox88){
            if (c._attacked(color, currPiecePos, Ox88.at(k))){
                count ++ ;
            }
        }
    }

    return count;
}

int spacialAdvantage(string fen){
    return spacialAdvantageWithColor('w', fen) - spacialAdvantageWithColor('b', fen);
}

void printMap(const map<string, int>& m){
    std::cout << "{" << std::endl;
    for (auto const& [key, val] : m){
        std::cout << key << ": " << val << "," << std::endl;

    }
    std::cout << "}" << std::endl;
}

int centerControlWithColor(char color, std::string fen) {
    int center[4] = {
        Ox88.at("e4"),
        Ox88.at("e5"),
        Ox88.at("d4"),
        Ox88.at("d5"),
    };

    int count = 0;

    // split and change the fen for move generation
    vector<string> sp = split(fen, ' ');

    sp[1] = string(1, color);

    string res = sp[0];

    for (int i = 1; i < sp.size(); i++) {
        res += " ";
        res += sp[i];
    }

    ChessEngine c = ChessEngine(res);
    vector<Move> moves = c._moves();

    for (Move move : moves) {
        int currPiecePos = move.from;
        for (int s : center) {
            if (c._attacked(color, currPiecePos, s)) {
                count++;
            }
        }
    }

    return count;
}

int centerControl(string fen) {
    int whiteCenterControl = centerControlWithColor('w', fen);
    int blackCenterControl = centerControlWithColor('b', fen);
    return whiteCenterControl - blackCenterControl;
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

    if (!validateFen(fen).status)
        return 0;
    vector<string> materials = split(split(fen)[0], '/');

    string allMaterial = "";
    for (string material : materials) {
        allMaterial += material;
    }
    for (char c : allMaterial) {
        if (isDigit(c)) {
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
    return 2* materialEval(fen) + centerControlWeight* centerControl(fen) + spacialAdvantage(fen);
}

// Alpha pruning not complete
map<string, variant<string, int, vector<std::any>>> alphaBetaPrunning(string position, int depth, int primeDepth, map<string, int> &transTable,int& countNodes, int alpha, int beta) {
    countNodes ++;
    if (transTable.find(fenToKey(position)) != transTable.end()) {
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

    // ChessEngine *c = new ChessEngine(position);
    ChessEngine *c = ChessEngine::getInstance(position);
    vector<Move> moves = c->_moves();

    if (split(position).at(1) == "w") {
        int maxScore = -std::numeric_limits<int>::max();
        int count = 0;
        vector<std::any> children = {};
        // vector<Move> moves = c->_moves();
        for (Move move : moves) {
            map<string, char> piece = c->_board.at(move.from);
            map<string, string> moveToMake = {
                {"from", algebraic(move.from)},
                {"to", algebraic(move.to)},
                {"color", string(1, move.color)},
                {"piece", string(1, move.piece)},
                {"captured", string(1, move.captured)},
                {"promotion", string(1, move.promotion)},
                {"san", move.san},
                {"lan", move.lan},
                {"before", move.before},
                {"after", move.after},
                {"flags", std::to_string(move.flags)}

            };
            string newPosition = c->move(moves, moveToMake).after;

            auto curr = alphaBetaPrunning(newPosition, depth - 1, primeDepth, transTable, countNodes);
            children.push_back(curr);
            int currScore = std::get<int>(curr.at("eval"));
            if (currScore > maxScore) {
                maxScore = currScore;
                if (depth == primeDepth) {
                    chosen = move;
                }
            }
            alpha = std::max(alpha, currScore);
            if (beta <= alpha) 
            {
                std::cout << "pruned at depth " << depth << std::endl;
            break;
            }
            // int maxScore = std::max(maxScore, currScore);

            // Remember to undomove
            c->_undoMove();
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

        for (Move move : moves) {
            map<string, char> piece = c->_board.at(move.from);

            map<string, string> moveToMake = {
                {"from", algebraic(move.from)},
                {"to", algebraic(move.to)},
                {"color", string(1, move.color)},
                {"piece", string(1, move.piece)},
                {"captured", string(1, move.captured)},
                {"promotion", string(1, move.promotion)},
                {"san", move.san},
                {"lan", move.lan},
                {"before", move.before},
                {"after", move.after},
                {"flags", std::to_string(move.flags)}

            };
            string newPosition = c->move(
                                      moves,
                                      moveToMake)
                                     .after;

            auto curr = alphaBetaPrunning(newPosition, depth - 1, primeDepth, transTable, countNodes);

            children.push_back(curr);
            int currScore = std::get<int>(curr.at("eval"));
            if (currScore < minScore) {
                minScore = currScore;
                if (depth == primeDepth) {
                    chosen = move;
                }
            }
            beta = std::min(beta, currScore);
            if (beta <= alpha)
            {
                std::cout << "pruned at depth " << depth << std::endl; 
             break;

            }
            // int minScore = std::min(minScore, currScore);

            // Remember to undomove
            c->_undoMove();
        }
        transTable[fenToKey(position)] = minScore;

        map<string, variant<string, int, vector<std::any>>> returnMap = {
            {"fen", position},
            {"eval", minScore},
            {"children", children}};
        return returnMap;
    }
}

// BestMove minimax(string position, int depth, int dummy)
// {
// }

int perft(int depth){
    ChessEngine c = ChessEngine("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    vector<Move> moves = c._moves();
    if (depth == 1){
        return moves.size();

    }
    int nodes = 0;
    for (Move move : moves){
        c._makeMove(move);
        nodes += perft(depth - 1);
        c._undoMove();
    }
    return nodes;
}

Move bestMove(string fen, int depth) {

    ChessEngine *c = ChessEngine::getInstance(fen);

    // Find best move
    vector<Move> moves = c->_moves();

    map<string, int> transTable = {};

    int count = 0;

    auto result = minimax(fen, depth, depth, transTable, count);

    // std::cout << "Total nodes searched minimax " << count << std::endl;
    if (chosen.color != '\0') {
        return c->_makePretty(chosen);
    }

    Move res = c->_makePretty(moves[moves.size() - 1]);
    return res;
}


Move bestMoveAB(string fen, int depth) {
    ChessEngine *c = ChessEngine::getInstance(fen);

    // Find best move
    vector<Move> moves = c->_moves();

    map<string, int> transTable = {};

    int count = 0;

    auto result = alphaBetaPrunning(fen, depth, depth, transTable, count);

    std::cout << "Total nodes searched ab: " << count << std::endl;

    if (chosen.color != '\0') {
        return c->_makePretty(chosen);
    }

    Move res = c->_makePretty(moves[moves.size() - 1]);
    return res;
}



map<string, variant<string, int, vector<std::any>>> minimax(string position, int depth, int primeDepth, map<string, int> &transTable, int& countNode) {
    auto minimaxStart = std::chrono::high_resolution_clock::now();

    auto minimaxStop = std::chrono::high_resolution_clock::now();

    countNode ++;
    
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

    // ChessEngine *c = new ChessEngine(position);
    ChessEngine *c = ChessEngine::getInstance(position);
    vector<Move> moves = c->_moves();
    auto start = std::chrono::high_resolution_clock::now();

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);


    if (split(position).at(1) == "w") {
        int maxScore = -std::numeric_limits<int>::max();
        int count = 0;
        vector<std::any> children = {};
        // vector<Move> moves = c->_moves();
        for (Move move : moves) {
            map<string, char> piece = c->_board.at(move.from);
            map<string, string> moveToMake = {
                {"from", algebraic(move.from)},
                {"to", algebraic(move.to)},
                {"color", string(1, move.color)},
                {"piece", string(1, move.piece)},
                {"captured", string(1, move.captured)},
                {"promotion", string(1, move.promotion)},
                {"san", move.san},
                {"lan", move.lan},
                {"before", move.before},
                {"after", move.after},
                {"flags", std::to_string(move.flags)}

            };
            string newPosition = c->move(moves, moveToMake).after;
            auto curr = minimax(newPosition, depth - 1, primeDepth, transTable, countNode);
            children.push_back(curr);
            int currScore = std::get<int>(curr.at("eval"));
            if (currScore > maxScore) {
                maxScore = currScore;
                if (depth == primeDepth) {
                    chosen = move;
                }
            }
            // int maxScore = std::max(maxScore, currScore);

            // Remember to undomove
            c->_undoMove();
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

        for (Move move : moves) {
            map<string, char> piece = c->_board.at(move.from);

            map<string, string> moveToMake = {
                {"from", algebraic(move.from)},
                {"to", algebraic(move.to)},
                {"color", string(1, move.color)},
                {"piece", string(1, move.piece)},
                {"captured", string(1, move.captured)},
                {"promotion", string(1, move.promotion)},
                {"san", move.san},
                {"lan", move.lan},
                {"before", move.before},
                {"after", move.after},
                {"flags", std::to_string(move.flags)}

            };
            string newPosition = c->move(
                                      moves,
                                      moveToMake)
                                     .after;

            auto curr = minimax(newPosition, depth - 1, primeDepth, transTable, countNode);

            children.push_back(curr);
            int currScore = std::get<int>(curr.at("eval"));
            if (currScore < minScore) {
                minScore = currScore;
                if (depth == primeDepth) {
                    chosen = move;
                }
            }
            // int minScore = std::min(minScore, currScore);

            // Remember to undomove
            c->_undoMove();
        }
        transTable[fenToKey(position)] = minScore;

        map<string, variant<string, int, vector<std::any>>> returnMap = {
            {"fen", position},
            {"eval", minScore},
            {"children", children}};
        return returnMap;
    }
}