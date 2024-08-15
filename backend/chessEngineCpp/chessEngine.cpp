#include <chessEngine.h>

ChessEngine *chessEngine = nullptr;

int rank(int square)
{
    return square >> 4;
}

int file(int square)
{
    return square & 0xf;
}

bool isDigit(char c)
{
    return isDigit(c);
}

string algebraic(int square)
{
    int f = file(square);
    int r = rank(square);
    string col = "abcdefgh";
    string row = "87654321";
    return col.substr(f, 1) + row.substr(r, 1);
}

fenResult validateFen(const string fen)
{
    vector<string> tokens;
    string currentToken;

    for (char c : fen)
    {
        if (c == ' ')
        {
            if (!currentToken.empty())
            {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
        }
        else
        {
            currentToken += c;
        }
    }
    if (!currentToken.empty())
    {
        tokens.push_back(currentToken);
    }

    fenResult result;

    if (tokens.size() != 6)
    {
        result.status = false;
        result.errorMessage = "Invalid FEN: must contain six space-delimited fields";
        return result;
    }

    string moveNumberStr = tokens[5];
    for (char c : moveNumberStr)
    {
        if (!isdigit(c))
        {
            result.status = false;
            result.errorMessage = "Invalid FEN: move number must be a positive integer";
            return result;
        }
    }
    int moveNumber = stoi(moveNumberStr);
    if (moveNumber <= 0)
    {
        result.status = false;
        result.errorMessage = "Invalid FEN: move number must be a positive integer";
        return result;
    }

    string halfMovesStr = tokens[4];
    for (char c : halfMovesStr)
    {
        if (!isdigit(c))
        {
            result.status = false;
            result.errorMessage = "Invalid FEN: half move counter number must be non-negative";
            return result;
        }
    }
    int halfMoves = stoi(halfMovesStr);
    if (halfMoves < 0)
    {
        result.status = false;
        result.errorMessage = "Invalid FEN: half move counter number must be non-negative";
        return result;
    }

    result.status = true;
    result.errorMessage = "";
    return result;
}

char swapColor(char color)
{
    if (color == WHITE)
    {
        return BLACK;
    }
    return WHITE;
}

string getDisambiguator(Move move, vector<Move> moves)
{
    int from = move.from;
    int to = move.to;
    char piece = move.piece;
    int ambiguities = 0, sameRank = 0, sameFile = 0;

    for (Move ambigMove : moves)
    {
        int ambigFrom = ambigMove.from;
        int ambigTo = ambigMove.to;
        char ambigPiece = ambigMove.piece;

        if (piece == ambigPiece && from != ambigFrom && to == ambigTo)
        {
            ambiguities++;
            if (rank(from) == rank(ambigFrom))
            {
                sameRank++;
            }
            if (file(from) == file(ambigFrom))
            {
                sameFile++;
            }
        }
    }

    if (ambiguities > 0)
    {
        string algebraicFrom = algebraic(from);
        if (sameRank > 0 && sameFile > 0)
        {
            return algebraicFrom;
        }
        else if (sameFile > 0)
        {
            return algebraicFrom.substr(1, 1);
        }
        else
        {
            return algebraicFrom.substr(0, 1);
        }
    }
    return "";
}

void addMove(vector<Move> moves, char color, int from, int to, char piece, char captured, int flags = BITS.at("NORMAL"))
{
    int r = rank(to);
    if (piece == PAWN && (r == RANK_1 || r == RANK_8))
    {
        for (char promotion : PROMOTIONS)
        {
            Move move;
            move.color = color;
            move.from = from;
            move.to = to;
            move.piece = piece;
            move.captured = captured;
            move.promotion = promotion;
            move.flags = flags | BITS.at("PROMOTION");
            moves.push_back(move);
        }
    }
    else
    {
        Move move;
        move.color = color;
        move.from = from;
        move.to = to;
        move.piece = piece;
        move.captured = captured;
        move.flags = flags;
        moves.push_back(move);
    }
}

char inferPieceType(string san)
{
    char pieceType = san[0];
    if (pieceType >= 'a' && pieceType <= 'h')
    {
        std::regex pattern("[a-h]\\d.*[a-h]\\d");
        if (std::regex_search(san, pattern))
        {
            return '\0'; // Null character
        }
        return PAWN;
    }
    pieceType = tolower(pieceType);
    if (pieceType == 'o')
    {
        return KING;
    }
    return pieceType;
}

string strippedSan(string move)
{
    string result = std::regex_replace(move, std::regex("="), "");
    return std::regex_replace(result, std::regex("[+#]?[?!]*$"), "");
}

string trimFen(string fen)
{

    string result;
    int empty = 0;

    for (char c : fen)
    {
        if (c == ' ')
        {
            empty++;
            if (empty == 4)
            {
                return result;
            }
        }
        if (empty < 4)
        {
            result += c;
        }
    }

    return result;
}

ChessEngine::ChessEngine() {}

ChessEngine::ChessEngine(string fen = DEFAULT_POSITION)
{
}

ChessEngine::~ChessEngine() {}
