#include <iostream>

#include "alpha_beta.h"

vector<string> split(string s, char del)
{
    vector<string> res;
    std::stringstream ss(s);
    string word;
    while (!ss.eof())
    {
        getline(ss, word, del);
        res.push_back(word);
    }
    return res;
}

int eval(string fen)
{
    map<char, int> materialPoints = {{'r', 5}, {'n', 3}, {'b', 3}, {'q', 9}, {'p', 1}, {'k', 200}, {'R', 5}, {'N', 3}, {'B', 3}, {'Q', 9}, {'P', 1}, {'K', 200}};

    int materialWeight = 0;
    int numWhitePieces = 0;
    int numBlackPieces = 0;
    int turn;
    if (split(fen)[1][0] == 'w')
    {
        turn = 1;
    }
    else
    {
        turn = -1;
    }
    int whiteMaterialWeight = 0;
    int blackMaterialWeight = 0;

    if (!validateFen(fen).status)
        return 0;
    vector<string> materials = split(split(fen)[0], '/');
    string allMaterial = "";
    for (string material : materials)
    {
        allMaterial += material;
    }
    for (char c : allMaterial)
    {
        if (isDigit(c))
        {
            continue;
        }
        if (isupper(c))
        {
            numWhitePieces += 1;
            whiteMaterialWeight += materialPoints[c];
        }
        else
        {
            numBlackPieces += 1;
            blackMaterialWeight += materialPoints[c];
        }
        return abs(whiteMaterialWeight - blackMaterialWeight) * (numWhitePieces - numBlackPieces) * turn;
    }
    return 0;
}

map<string, variant<string, int, vector<std::any>>> minimax(string position, int depth)
{
    if (depth == 0)
    {
        vector<std::any> noChildren;
        map<string, variant<string, int, vector<std::any>>> obj = {
            {"fen", position},
            {"eval", eval(position)},
            {"children", noChildren},

        };
        return obj;
    }

    ChessEngine *c = new ChessEngine(position);
    if (split(position).at(1) == "w")
    {
        int maxScore = -std::numeric_limits<int>::max();
        int count = 0;
        vector<std::any> children = {};
        for (Move move : c->_moves())
        {
            map<string, string> moveToMake = {
                {"from", algebraic(move.from)},
                {"to", algebraic(move.to)},
            };
            string newPosition = c->move(moveToMake).after;
            std::cout << newPosition << std::endl;
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
    }
    else
    {
        int minScore = std::numeric_limits<int>::max();
        vector<std::any> children;

        for (Move move : c->_moves())
        {

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

map<string, variant<string, int, vector<std::any>>> alphaBetaPrunning(string position, int depth, int alpha, int beta)
{
    if (depth == 0)
    {
        vector<std::any> noChildren;
        map<string, variant<string, int, vector<std::any>>> obj = {
            {"fen", position},
            {"eval", eval(position)},
            {"children", noChildren},

        };
        return obj;
    }

    ChessEngine *c = new ChessEngine(position);
    if (split(position).at(1) == "w")
    {
        int maxScore = -std::numeric_limits<int>::max();
        int count = 0;
        vector<std::any> children = {};
        for (Move move : c->_moves())
        {
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
            if (beta <= alpha)
                break;

            // Remember to undomove
            c->_undoMove();
        }
        map<string, variant<string, int, vector<std::any>>> returnMap = {
            {"fen", position},
            {"eval", maxScore},
            {"children", children}};
        return returnMap;
    }
    else
    {
        int minScore = std::numeric_limits<int>::max();
        vector<std::any> children;

        for (Move move : c->_moves())
        {
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
            if (beta <= alpha)
                break;

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
