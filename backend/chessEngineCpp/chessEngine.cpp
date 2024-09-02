#include "chessEngine.h"

#include <iostream>
#include <variant>

ChessEngine *chessEngine = nullptr;

int rank(int square) {
    return square >> 4;
}

int file(int square) {
    return square & 0xf;
}

bool isDigit(char c) {
    return isdigit(c);
}

string algebraic(int square) {
    int f = file(square);
    int r = rank(square);
    string col = "abcdefgh";
    string row = "87654321";
    return col.substr(f, 1) + row.substr(r, 1);
}

fenResult validateFen(const string fen) {
    vector<string> tokens;
    string currentToken;

    for (char c : fen) {
        if (c == ' ') {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
        } else {
            currentToken += c;
        }
    }
    tokens.push_back(currentToken);

    fenResult result;

    if (tokens.size() != 6) {
        result.status = false;
        result.errorMessage = "Invalid FEN: must contain six space-delimited fields";
        return result;
    }

    string moveNumberStr = tokens[5];
    for (char c : moveNumberStr) {
        if (!isdigit(c)) {
            result.status = false;
            result.errorMessage = "Invalid FEN: move number must be a positive integer";
            return result;
        }
    }
    int moveNumber = stoi(moveNumberStr);
    if (moveNumber <= 0) {
        result.status = false;
        result.errorMessage = "Invalid FEN: move number must be a positive integer";
        return result;
    }

    string halfMovesStr = tokens[4];
    for (char c : halfMovesStr) {
        if (!isdigit(c)) {
            result.status = false;
            result.errorMessage = "Invalid FEN: half move counter number must be non-negative";
            return result;
        }
    }
    int halfMoves = stoi(halfMovesStr);
    if (halfMoves < 0) {
        result.status = false;
        result.errorMessage = "Invalid FEN: half move counter number must be non-negative";
        return result;
    }

    result.status = true;
    result.errorMessage = "";
    return result;
}

char swapColor(char color) {
    if (color == WHITE) {
        return BLACK;
    }
    return WHITE;
}

string getDisambiguator(Move move, vector<Move> moves) {
    int from = move.from;
    int to = move.to;
    char piece = move.piece;
    int ambiguities = 0, sameRank = 0, sameFile = 0;

    for (Move ambigMove : moves) {
        int ambigFrom = ambigMove.from;
        int ambigTo = ambigMove.to;
        char ambigPiece = ambigMove.piece;

        if (piece == ambigPiece && from != ambigFrom && to == ambigTo) {
            ambiguities++;
            if (rank(from) == rank(ambigFrom)) {
                sameRank++;
            }
            if (file(from) == file(ambigFrom)) {
                sameFile++;
            }
        }
    }

    if (ambiguities > 0) {
        string algebraicFrom = algebraic(from);
        if (sameRank > 0 && sameFile > 0) {
            return algebraicFrom;
        } else if (sameFile > 0) {
            return algebraicFrom.substr(1, 1);
        } else {
            return algebraicFrom.substr(0, 1);
        }
    }
    return "";
}

void addMove(vector<Move> &moves, char color, int from, int to, char piece, char captured, int flags) {
    int r = rank(to);
    if (piece == PAWN && (r == RANK_1 || r == RANK_8)) {
        for (char promotion : PROMOTIONS) {
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
    } else {
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

char inferPieceType(string san) {
    char pieceType = san[0];
    if (pieceType >= 'a' && pieceType <= 'h') {
        std::regex pattern("[a-h]\\d.*[a-h]\\d");
        if (std::regex_search(san, pattern)) {
            return '\0';  // Null character
        }
        return PAWN;
    }
    pieceType = tolower(pieceType);
    if (pieceType == 'o') {
        return KING;
    }
    return pieceType;
}

string strippedSan(string move) {
    string result = std::regex_replace(move, std::regex("="), "");
    return std::regex_replace(result, std::regex("[+#]?[?!]*$"), "");
}

string trimFen(string fen) {
    string result;
    int empty = 0;

    for (char c : fen) {
        if (c == ' ') {
            empty++;
            if (empty == 4) {
                return result;
            }
        }
        if (empty < 4) {
            result += c;
        }
    }

    return result;
}
ChessEngine *ChessEngine::sin = nullptr;
ChessEngine *ChessEngine::getInstance(string fen) {
    if (sin == nullptr) {
        sin = new ChessEngine(fen);
    }
    if (sin->fen() != fen) {
        sin = new ChessEngine(fen);
    }

    return sin;
}
ChessEngine::ChessEngine() {}
ChessEngine::~ChessEngine() {}

ChessEngine::ChessEngine(string fen) {
    _board = {};
    _turn = WHITE;
    _header = {};
    _kings = {{'w', EMPTY}, {'b', EMPTY}};
    _epSquare = -1;
    _halfMoves = 0;
    _moveNumber = 0;
    _history = {};
    _comments = {};
    _castling = {{'w', 0}, {'b', 0}};
    _positionCount = {};
    load(fen);
}

void ChessEngine::clear(map<string, bool> config) {
    _board = {};
    _kings = {{'w', EMPTY}, {'b', EMPTY}};
    _turn = WHITE;
    _castling = {{'w', 0}, {'b', 0}};
    _epSquare = EMPTY;
    _halfMoves = 0;
    _moveNumber = 1;
    _history = {};
    _comments = {};
    _positionCount = {};

    if (!config["preserveHeaders"]) {
        _header = {};
        return;
    }
    _header.erase("SetUp");
    _header.erase("FEN");
}

void ChessEngine::load(string fen, map<string, bool> config) {
    // Split fen to tokens
    vector<string> tokens;
    string currToken;
    for (char c : fen) {
        if (c == ' ') {
            tokens.push_back(currToken);
            currToken = "";
        } else {
            currToken += c;
        }
    }
    tokens.push_back(currToken);

    // load fen
    if (tokens.size() >= 2 && tokens.size() < 6) {
        vector<string> adjustment = {"-", "-", "0", "1"};
        // could be wrong here
        tokens.insert(tokens.end(), adjustment.end() + (6 - tokens.size()), adjustment.end());
        fen = "";
        for (string t : tokens) {
            fen += t + " ";
        }
        fen.pop_back();  // remove last space
    }

    // Re-split the adjusted FEN
    tokens.clear();
    currToken = "";
    for (char c : fen) {
        if (c == ' ') {
            tokens.push_back(currToken);
            currToken = "";
        } else {
            currToken += c;
        }
    }
    tokens.push_back(currToken);

    if (!config["skipValidation"]) {
        fenResult validation = validateFen(fen);
        if (!validation.status) {
            throw std::runtime_error(validation.errorMessage);
        }
    }

    string position = tokens[0];
    int square = 0;
    clear({{"preserveHeaders", config["preserveHeaders"]}});

    for (char piece : position) {
        if (piece == '/') {
            square += 8;
        } else if (isdigit(piece)) {
            square += piece - '0';
        } else {
            char color = BLACK;
            if (piece < 'a') {
                color = WHITE;
            }
            map<string, char> newConfig = {
                {"type", tolower(piece)},
                {"color", color}};
            _put(newConfig, algebraic(square));
            square += 1;
        }
    }

    _turn = tokens[1][0];  // convert from string to char
    if (tokens[2].find('K') != string::npos) {
        _castling['w'] |= BITS.at("KSIDE_CASTLE");
    }
    if (tokens[2].find('Q') != string::npos) {
        _castling['w'] |= BITS.at("QSIDE_CASTLE");
    }
    if (tokens[2].find('k') != string::npos) {
        _castling['b'] |= BITS.at("KSIDE_CASTLE");
    }
    if (tokens[2].find('q') != string::npos) {
        _castling['b'] |= BITS.at("QSIDE_CASTLE");
    }
    if (tokens[3][0] == '-') {
        _epSquare = EMPTY;
    } else {
        _epSquare = Ox88.at(tokens[3]);
    }

    _halfMoves = stoi(tokens[4]);
    _moveNumber = stoi(tokens[5]);

    _updateSetup(fen);
    _incPositionCount(fen);
}

string ChessEngine::fen() {
    int empty = 0;
    string fen = "";
    int i = 0;

    while (i <= Ox88.at("h1")) {
        if (_board.find(i) != _board.end()) {
            if (empty > 0) {
                fen += std::to_string(empty);
                empty = 0;
            }
            map<string, char> data = _board[i];
            if (data["color"] == WHITE) {
                fen += toupper(data["type"]);
            } else {
                fen += tolower(data["type"]);
            }
        } else {
            empty += 1;
        }

        if (((i + 1) & 0x88)) {
            if (empty > 0) {
                fen += std::to_string(empty);
            }
            if (i != Ox88.at("h1")) {
                fen += "/";
            }
            empty = 0;
            i += 9;
            continue;
        }
        i += 1;
    }

    string castling = "";
    if (_castling[WHITE] & BITS.at("KSIDE_CASTLE")) {
        castling += 'K';
    }
    if (_castling[WHITE] & BITS.at("QSIDE_CASTLE")) {
        castling += 'Q';
    }
    if (_castling[BLACK] & BITS.at("KSIDE_CASTLE")) {
        castling += 'k';
    }
    if (_castling[BLACK] & BITS.at("QSIDE_CASTLE")) {
        castling += 'q';
    }

    castling = castling.empty() ? "-" : castling;
    string eqSquare = "-";

    if (_epSquare != EMPTY) {
        try {
            int bigPawnSquare = _epSquare + ((_turn == WHITE) ? 16 : -16);
            vector<int> squares = {bigPawnSquare + 1, bigPawnSquare - 1};
            for (int square : squares) {
                if ((square & 0x88)) {
                    continue;
                }
                char color = _turn;
                if (_board.find(square) != _board.end()) {
                    if (_board[square]["color"] == color && _board[square]["type"] == PAWN) {
                        Move move;
                        move.color = color;
                        move.from = square;
                        move.to = _epSquare;
                        move.piece = PAWN;
                        move.captured = PAWN;
                        move.flags = BITS.at("EP_CAPTURE");
                        _makeMove(move);
                        bool isLegal = true;
                        _undoMove();
                        if (isLegal) {
                            eqSquare = algebraic(_epSquare);
                        }
                    }
                }
            }
        } catch (...) {
            std::cout << "h";
        }
    }

    return fen + " " + _turn + " " + castling + " " + eqSquare + " " + std::to_string(_halfMoves) + " " + std::to_string(_moveNumber);
}

void ChessEngine::_updateSetup(string fen) {
    if (_history.size() > 0) {
        return;
    }
    if (fen != DEFAULT_POSITION) {
        // Avoid an issue relating to converting between c string and string, which affect core allocator library
        string key1 = "SetUp";
        string key2 = "FEN";
        string val1 = "1";
        _header[key1] = val1;
        _header[key2] = fen;
        // _header.insert(key1.c_str(), val1);
        // _header.insert(key2, fen);
    } else {
        if (_header.find("Setup") != _header.end()) {
            _header.erase("Setup");
        }
        if (_header.find("FEN") != _header.end()) {
            _header.erase("FEN");
        }
    }
}

void ChessEngine::reset() {
    load(DEFAULT_POSITION);
}

map<string, char> ChessEngine::get(string square) {
    if (Ox88.find(square) == Ox88.end()) {
        return {{"status", 'f'}};
    } else {
        if (_board.find(Ox88.at(square)) == _board.end()) {
            // not found
            return {{"status", 'f'}};
        }
        // found
    }
    return _board.at(Ox88.at(square));
}

bool ChessEngine::put(map<string, char> config, string square) {
    if (_put(config, square)) {
        _updateCastlingRights();
        _updateEnPassantSquare();
        _updateSetup(fen());
        return true;
    }
    return false;
}

bool ChessEngine::_put(map<string, char> config, string square) {
    if (!(SYMBOLS.find(config.at("type")) < SYMBOLS.length())) {
        // not found
        return false;
    }
    if (Ox88.find(square) == Ox88.end()) {
        // not found
        return false;
    }
    int sq = Ox88.at(square);
    if (config.at("type") == KING &&
            !(_kings.at(config.at("color")) == EMPTY) ||
        (_kings.at(config.at("color")) == sq)) {
        return false;
    }
    map<string, char> currentPieceOnSquare = {};

    if (_board.find(sq) != _board.end()) {
        // found
        currentPieceOnSquare = _board.at(sq);
    }

    if (!currentPieceOnSquare.empty() && (currentPieceOnSquare.at("type") == KING)) {
        _kings.at(currentPieceOnSquare.at("color")) = EMPTY;
    }
    _board[sq] = {
        {"type", config.at("type")},
        {"color", config.at("color")}};

    if (config.at("type") == KING) {
        _kings[config.at("color")] = sq;
    }
    return true;
}

// No implementation in header file
// map<string, char> ChessEngine::remove(string square)
// {
//     map<string, char> piece = get(square);
//     _board.erase(Ox88.at(square));

//     if ((piece.find("status") == piece.end()) && piece.at("type") == KING)
//     {
//         _kings[piece.at("color")] = EMPTY;
//     }
//     _updateCastlingRights();
//     _updateEnPassantSquare();
//     _updateSetup(fen());
//     return piece;
// }

void ChessEngine::_updateCastlingRights() {
    // todo: refactor this code to be compatible with null checks from js:
    // have some variables representative for each null check -> use these variables in lines with null checking

    // Returns false if null
    bool e1nc = _board.find(Ox88.at("e1")) != _board.end();
    bool e8nc = _board.find(Ox88.at("e8")) != _board.end();
    bool a1nc = _board.find(Ox88.at("a1")) != _board.end();
    bool h1nc = _board.find(Ox88.at("h1")) != _board.end();
    bool a8nc = _board.find(Ox88.at("a8")) != _board.end();
    bool h8nc = _board.find(Ox88.at("h8")) != _board.end();

    bool whiteKingInPlace = false;
    if (e1nc) {
        if (_board.at(Ox88.at("e1")).at("type") == KING && _board.at(Ox88.at("e1")).at("color") == WHITE) {
            whiteKingInPlace = true;
        }
    }

    bool blackKingInPlace = false;
    if (!e8nc) {
        if (_board.at(Ox88.at("e8")).at("type") == KING && _board.at(Ox88.at("e8")).at("color") == BLACK) {
            blackKingInPlace = true;
        }
    }

    if (!a1nc) {
        _castling['w'] &= ~BITS.at("QSIDE_CASTLE");
    } else {
        if (!whiteKingInPlace || _board.at(Ox88.at("a1")).at("type") != ROOK || _board.at(Ox88.at("a1")).at("color") != WHITE) {
            _castling['w'] &= ~BITS.at("QSIDE_CASTLE");
        }
    }

    if (!h1nc) {
        _castling['w'] &= ~BITS.at("KSIDE_CASTLE");
    } else {
        if (!whiteKingInPlace || _board.at(Ox88.at("h1")).at("type") != ROOK || _board.at(Ox88.at("h1")).at("color") != WHITE) {
            _castling['w'] &= ~BITS.at("KSIDE_CASTLE");
        }
    }

    if (!a8nc) {
        _castling['b'] &= ~BITS.at("QSIDE_CASTLE");
    } else {
        if (!blackKingInPlace || _board.at(Ox88.at("a8")).at("type") != ROOK || _board.at(Ox88.at("a8")).at("color") != BLACK) {
            _castling['b'] &= ~BITS.at("QSIDE_CASTLE");
        }
    }

    if (!h8nc) {
        _castling['b'] &= ~BITS.at("KSIDE_CASTLE");
    } else {
        if (!blackKingInPlace || _board.at(Ox88.at("h8")).at("type") != ROOK || _board.at(Ox88.at("h8")).at("color") != BLACK) {
            _castling['b'] &= ~BITS.at("KSIDE_CASTLE");
        }
    }
}

bool ChessEngine::canCapture(int square) {
    if (_board.find(square) == _board.end()) {
        return false;
    }
    return (
        !(square & 0x88) &&
        _board.at(square).at("color") == _turn &&
        _board.at(square).at("type") == PAWN);
}

void ChessEngine::_updateEnPassantSquare() {
    if (_epSquare == EMPTY) {
        return;
    }

    const int startSquare = _epSquare + (_turn == WHITE ? -16 : 16);
    const int currentSquare = _epSquare + (_turn == WHITE ? -16 : 16);
    const int attackers[2] = {currentSquare + 1, currentSquare - 1};

    // returns false if null
    bool currentSquarenc = _board.find(currentSquare) != _board.end();

    if (!currentSquare) {
        _epSquare = EMPTY;
        return;
    } else {
        if (
            _board.find(startSquare) != _board.end() ||
            _board.find(_epSquare) != _board.end() ||
            _board.at(currentSquare).at("color") != swapColor(_turn) ||
            _board.at(currentSquare).at("type") != PAWN) {
            _epSquare = EMPTY;
            return;
        }
    }
    for (int attacker : attackers) {
        if (canCapture(attacker)) {
            _epSquare = EMPTY;
            return;
        }
    }
}

bool ChessEngine::_attacked(char color, int from, int square) {
    int i = from;
    if (i & 0x88) {
        i += 7;
    }
    if (_board.find(i) == _board.end() || _board.at(i).at("color") != color) {
        // continue;
        return false;
    }

    const map<string, char> piece = _board.at(i);
    const int difference = i - square;

    if (difference == 0) {
        return false;
        // continue;
    }

    const int index = difference + 119;

    if (ATTACKS[index] & PIECE_MASKS.at(piece.at("type"))) {
        if (piece.at("type") == PAWN) {
            if (difference > 0) {
                if (piece.at("color") == WHITE)
                    return true;
            } else {
                if (piece.at("color") == BLACK)
                    return true;
            }
            // continue;
            return false;
        }
        if (piece.at("type") == 'n' || piece.at("type") == 'k')
            return true;
        const int offset = RAYS[index];
        int j = i + offset;
        bool blocked = false;
        while (j != square) {
            if (_board.find(j) != _board.end()) {
                blocked = true;
                break;
            }
            j += offset;
        }
        if (!blocked)
            return true;
    }
    return false;
}

bool ChessEngine::_attacked(char color, int square) {
    for (int i = Ox88.at("a8"); i <= Ox88.at("h1"); i++) {
        if (i & 0x88) {
            i += 7;
            continue;
        }
        if (_board.find(i) == _board.end() || _board.at(i).at("color") != color) {
            continue;
        }

        const map<string, char> piece = _board.at(i);
        const int difference = i - square;

        if (difference == 0) {
            continue;
        }

        const int index = difference + 119;

        if (ATTACKS[index] & PIECE_MASKS.at(piece.at("type"))) {
            if (piece.at("type") == PAWN) {
                if (difference > 0) {
                    if (piece.at("color") == WHITE)
                        return true;
                } else {
                    if (piece.at("color") == BLACK)
                        return true;
                }
                continue;
            }
            if (piece.at("type") == 'n' || piece.at("type") == 'k')
                return true;
            const int offset = RAYS[index];
            int j = i + offset;
            bool blocked = false;
            while (j != square) {
                if (_board.find(j) != _board.end()) {
                    blocked = true;
                    break;
                }
                j += offset;
            }
            if (!blocked)
                return true;
        }
    }
    return false;
}

bool ChessEngine::_isKingAttacked(char color) {
    int square = _kings.at(color);
    return square == -1 ? false : _attacked(swapColor(color), square);
}
Move ChessEngine::move(map<string, string> move, map<string, bool> config) {
    // remove detect move type to be string, can use variant to fix it
    Move moveObj = Move();
    if (config.find("strict") == config.end()) {
        config.insert({"strict", false});
    }
    vector<Move> moves = _moves();

    for (int i = 0; i < moves.size(); i++) {

        if (move["from"] == algebraic(moves[i].from) && move["to"] == algebraic(moves[i].to) &&
            ((moves[i].promotion == '\0') || move["promotion"][0] == moves[i].promotion)) {
            moveObj = moves[i];
            break;
        }
    }
    if (moveObj.color == '\0') {
        string moveInfo = "Invalid move";
        std::cout << move.at("from") << std::endl;

        std::cout << moveInfo << std::endl;
        throw std::runtime_error(moveInfo);
    }
    Move prettyMove = _makePretty(moveObj);
    _makeMove(moveObj);
    _incPositionCount(prettyMove.after);
    return prettyMove;
}

Move ChessEngine::move(vector<Move> moves, map<string, string> move, map<string, bool> config) {
    // remove detect move type to be string, can use variant to fix it
    Move moveObj = Move();
    if (config.find("strict") == config.end()) {
        config.insert({"strict", false});
    }

    for (int i = 0; i < moves.size(); i++) {
        // std::cout << "starting" << std::endl;
        // std::cout << move["from"] << " " << algebraic(moves[i].from) << std::endl;
        // std::cout << move["to"] << " " << algebraic(moves[i].to) << std::endl;
        // std::cout << move["promotion"][0] << " " << moves[i].promotion << std::endl;

        if (move["from"] == algebraic(moves[i].from) && move["to"] == algebraic(moves[i].to)){
            if (move["from"] == "f7" && move["to"] == "g8") {
                std::cout << "Here comes the atarget" << std::endl;

                std::cout << fen() << std:: endl;
                if (moves[i].promotion == '\0'){
                    std::cout << "moves[i] promotion is null" << std::endl;
                }
                std::cout << "moves[i] promotion: " << moves[i].promotion << std::endl;
                std::cout << "move[promotion] " << move["promotion"][0] << std::endl;

                if (moves[i].promotion == move["promotion"][0]){
                    std::cout << "The 2 promotion values are equal" <<  std::endl;
                }
            }
        }

        if (
            move["from"] == algebraic(moves[i].from) && move["to"] == algebraic(moves[i].to) &&
            ((moves[i].promotion == '\0') || move["promotion"][0] == moves[i].promotion)) {
            moveObj = moves[i];
            break;
        }
    }
    if (moveObj.color == '\0') {
        string moveInfo = "Invalid move";
        std::cout << move.at("from") << std::endl;

        std::cout << moveInfo << std::endl;
        throw std::runtime_error(moveInfo);
    }
    Move prettyMove = _makePretty(moveObj);

    _makeMove(moveObj);
    _incPositionCount(prettyMove.after);
    return prettyMove;
}

void ChessEngine::_push(Move move) {
    HistoryMove newMove;
    newMove.move = move;
    newMove.kings = {{'b', _kings['b']}, {'w', _kings['w']}};
    newMove.turn = _turn;
    newMove.castling = {{'b', _castling['b']},
                        {'w', _castling['w']}};

    newMove.epSquare = _epSquare;
    newMove.halfMoves = _halfMoves;
    newMove.moveNumber = _moveNumber;
    _history.push_back(newMove);
}



void ChessEngine::_makeMove(Move move) {
    char us = _turn;
    char them = swapColor(us);
    _push(move);

    if (_board.find(move.from) != _board.end()) {
        _board[move.to] = _board[move.from];
        _board.erase(move.from);
    }

    if (move.flags & BITS.at("EP_CAPTURE")) {
        if (_turn == BLACK) {
            _board.erase(move.to - 16);
        } else {
            _board.erase(move.to + 16);
        }
    }

    if (move.promotion != '\0') {
        map<string, char> newMap = {{"type", move.promotion}, {"color", us}};
        _board[move.to] = newMap;
    }

    if (_board.find(move.to) != _board.end()) {
        if (_board[move.to]["type"] == KING) {
            _kings[us] = move.to;
            int castlingTo, castlingFrom;
            if (move.flags & BITS.at("KSIDE_CASTLE")) {
                castlingTo = move.to - 1;
                castlingFrom = move.to + 1;
                _board[castlingTo] = _board[castlingFrom];
                _board.erase(castlingFrom);
            } else if (move.flags & BITS.at("QSIDE_CASTLE")) {
                castlingTo = move.to + 1;
                castlingFrom = move.to - 2;
                _board[castlingTo] = _board[castlingFrom];
                _board.erase(castlingFrom);
            }
            _castling[us] = 0;
        }
    }

    if (_castling.find(us) != _castling.end()) {
        for (int i = 0; i < ROOKS.at(us).size(); i++) {
            if (move.from == ROOKS.at(us)[i].at("square") && (_castling[us] & ROOKS.at(us)[i].at("flag"))) {
                _castling[us] ^= ROOKS.at(us)[i].at("flag");
                break;
            }
        }
    }

    if (_castling.find(them) != _castling.end()) {
        for (int i = 0; i < ROOKS.at(them).size(); i++) {
            if (move.to == ROOKS.at(them)[i].at("square") && (_castling[them] & ROOKS.at(them)[i].at("flag"))) {
                _castling[them] ^= ROOKS.at(them)[i].at("flag");
            }
        }
    }

    if (move.flags & BITS.at("BIG_PAWN")) {
        if (us == BLACK) {
            _epSquare = move.to - 16;
        } else {
            _epSquare = move.to + 16;
        }
    } else {
        _epSquare = EMPTY;
    }

    if (move.piece == PAWN) {
        _halfMoves = 0;
    } else if (move.flags & (BITS.at("CAPTURE") | BITS.at("EP_CAPTURE"))) {
        _halfMoves = 0;
    } else {
        _halfMoves++;
    }

    if (us == BLACK) {
        _moveNumber++;
    }

    _turn = them;
}

Move ChessEngine::undo() {
    // Consider using nullptr
    Move move = _undoMove();
    if (move.color != '\0')  // A move is undefined if its color is also undefined
    {
        Move prettyMove = _makePretty(move);
        _decPositionCount(prettyMove.after);
        return prettyMove;
    }
    return Move();
}

Move ChessEngine::_undoMove() {

    if (_history.empty()) {
        return Move();  // Return an empty or default Move
    }

    HistoryMove old = _history.back();
    _history.pop_back();

    Move move = old.move;
    _kings = old.kings;
    _turn = old.turn;
    _castling = old.castling;
    _epSquare = old.epSquare;
    _halfMoves = old.halfMoves;
    _moveNumber = old.moveNumber;

    char us = _turn;
    char them = swapColor(us);

    if (_board.find(move.to) != _board.end()) {
        _board[move.from] = _board[move.to];
        _board[move.from]["type"] = move.piece;  // To undo any promotions
        _board.erase(move.to);
    }

    if (move.captured != '\0') {
        if (move.flags & BITS.at("EP_CAPTURE")) {
            int index = (us == BLACK) ? move.to - 16 : move.to + 16;
            map<string, char> newMap = {{"type", PAWN}, {"color", them}};
            _board[index] = newMap;
        } else {
            map<string, char> newMap = {{"type", move.captured}, {"color", them}};
            _board[move.to] = newMap;
        }
    }

    if (move.flags & (BITS.at("KSIDE_CASTLE") | BITS.at("QSIDE_CASTLE"))) {
        int castlingTo, castlingFrom;
        if (move.flags & BITS.at("KSIDE_CASTLE")) {
            castlingTo = move.to + 1;
            castlingFrom = move.to - 1;
        } else {
            castlingTo = move.to - 2;
            castlingFrom = move.to + 1;
        }
        _board[castlingTo] = _board[castlingFrom];
        _board.erase(castlingFrom);
    }



    return move;
}

// map<string, string> ChessEngine::header(vector<string> args);

string ChessEngine::_moveToSan(Move move, vector<Move> moves) {
    string output;

    if (move.flags & BITS.at("KSIDE_CASTLE")) {
        output = "O-O";
    } else if (move.flags & BITS.at("QSIDE_CASTLE")) {
        output = "O-O-O";
    } else {
        if (move.piece != PAWN) {
            string disambiguator = getDisambiguator(move, moves);
            output += static_cast<char>(toupper(move.piece)) + disambiguator;
        }
        if (move.flags & (BITS.at("CAPTURE") | BITS.at("EP_CAPTURE"))) {
            if (move.piece == PAWN) {
                output += algebraic(move.from)[0];
            }
            output += "x";
        }
        output += algebraic(move.to);
        if (move.promotion != '\0') {
            output += "=" + toupper(static_cast<char>(move.promotion));
        }
    }

    _makeMove(move);
    if (false) {
        output += isCheckmate() ? "#" : "+";
    }
    _undoMove();

    return output;
}

Move ChessEngine::_moveFromSan(string move, bool strict) {
    string cleanMove = strippedSan(move);
    char pieceType = inferPieceType(cleanMove);
    std::vector<Move> moves = _moves(true, pieceType);

    for (int i = 0; i < moves.size(); i++) {
        if (cleanMove == strippedSan(_moveToSan(moves[i], moves))) {
            return moves[i];
        }
    }

    if (strict) {
        // return empty struct
        return Move();
    }

    string piece, from, to, promotion;
    piece = "";
    bool overlyDisambiguated = false;
    std::smatch matches;
    std::regex pattern("([pnbrqkPNBRQK])?([a-h][1-8])x?-?([a-h][1-8])([qrbnQRBN])?");

    if (std::regex_search(cleanMove, matches, pattern)) {
        piece = matches[1];
        from = matches[2];
        to = matches[3];
        promotion = matches[4];
        if (from.size() == 1) {
            overlyDisambiguated = true;
        }
    } else {
        pattern = std::regex("([pnbrqkPNBRQK])?([a-h]?[1-8]?)x?-?([a-h][1-8])([qrbnQRBN])?");
        if (std::regex_search(cleanMove, matches, pattern)) {
            piece = matches[1];
            from = matches[2];
            to = matches[3];
            promotion = matches[4];
            if (from.size() == 1) {
                overlyDisambiguated = true;
            }
        }
    }

    pieceType = inferPieceType(cleanMove);
    moves = {};
    if (piece != "") {
        // convert string to char
        moves = _moves(true, piece[0]);
    } else {
        moves = _moves(true, pieceType);
    }
    if (to.empty()) {
        return Move();
    }

    for (int i = 0; i < moves.size(); i++) {
        if (from.empty()) {
            string ssan = strippedSan(_moveToSan(moves[i], moves));

            if (cleanMove == ssan.replace(ssan.begin(), ssan.end(), 'x', 'y')) {
                return moves[i];
            } else if (
                (!piece.empty() && piece[0] == moves[i].piece) &&
                from == algebraic(moves[i].from) &&
                to == algebraic(moves[i].to) &&
                (promotion.empty() || promotion[0] == moves[i].promotion)) {
                return moves[i];
            } else if (overlyDisambiguated) {
                string square = algebraic(moves[i].from);
                if (
                    (piece.empty() && tolower(piece[0]) == moves[i].piece) &&
                    Ox88.at(to) == moves[i].to &&
                    (from[0] == square[0] || from[0] == square[1]) &&
                    (promotion.empty() || tolower(promotion[0]) == moves[i].promotion)) {
                    return moves[i];
                }
            }
        }
    }

    return Move();
}
// Move ChessEngine::_makePretty(Move uglyMove)
// {

//     char color = uglyMove.color;
//     char piece = uglyMove.piece;
//     char flags = uglyMove.flags;
//     int from = uglyMove.from;
//     int to = uglyMove.to;
//     char captured = uglyMove.captured;
//     char promotion = uglyMove.promotion;

//     string prettyFlags = "";
//     for (auto flag : BITS)
//     {
//         if (flag.second & flags)
//         {
//             prettyFlags += FLAGS.at(flag.first);
//         }
//     }

//     string fromAlgebraic = algebraic(from);
//     string toAlgebraic = algebraic(to);

//     Move move;
//     move.color = color;
//     move.piece = piece;
//     move.from = from;
//     move.to = to;
//     move.san = _moveToSan(uglyMove, _moves(true));
//     move.flags = prettyFlags[0];
//     move.lan = fromAlgebraic + toAlgebraic;
//     move.before = fen();
//     move.after = "";

//     _makeMove(uglyMove);
//     move.after = fen();
//     _undoMove();

//     if (captured)
//     {
//         move.captured = captured;
//     }

//     if (promotion)
//     {
//         move.promotion = promotion;
//         move.lan += promotion;
//     }

//     return move;
// }

bool ChessEngine::isAttacked(string square, char attackedBy) {
    return _attacked(attackedBy, Ox88.at(square));
}
bool ChessEngine::isCheck() {
    return _isKingAttacked(_turn);
}

bool ChessEngine::inCheck() {
    return isCheck();
}

bool ChessEngine::isCheckmate() {
    return isCheck() && _moves().size() == 0;
}

bool ChessEngine::isStalemate() {
    return !isCheck() && _moves().size() == 0;
}

bool ChessEngine::isSufficientMaterial() {
    map<char, int> pieces = {
        {'b', 0},
        {'n', 0},
        {'r', 0},
        {'q', 0},
        {'k', 0},
        {'p', 0}};
    vector<int> bishops;
    int numPieces = 0;
    int squareColor = 0;
    for (int i = Ox88.at("a8"); i <= Ox88.at("h1"); i++) {
        squareColor = (squareColor + 1) % 2;
        if (i & 0x88) {
            i += 7;
            continue;
        }
        std::optional<map<string, char>> piece;
        if (_board.find(i) != _board.end()) {
            piece = _board.at(i);
        }
        if (piece) {
            char typeInPiece = piece.value().at("type");
            pieces[typeInPiece] = pieces.find(typeInPiece) != pieces.end() ? pieces[typeInPiece] + 1 : 1;
            if (typeInPiece == BISHOP) {
                bishops.push_back(squareColor);
            }
            numPieces++;
        }
    }

    if (numPieces == 2) {
        return true;
    } else if (
        numPieces == 3 &&
        (pieces[BISHOP] == 1 || pieces[KNIGHT] == 1)) {
        return true;
    } else if (numPieces == pieces[BISHOP] + 2) {
        int sum = 0;
        int len = bishops.size();
        for (int i = 0; i < len; i++) {
            sum += bishops[i];
        }
        if (sum == 0 || sum == len) {
            return true;
        }
    }
    return false;
}

bool ChessEngine::isThreefoldRepetition() {
    return _getPositionCount(fen()) >= 3;
}

bool ChessEngine::isDraw() {
    return (
        _halfMoves >= 100 ||
        isStalemate() ||
        isSufficientMaterial() ||
        isThreefoldRepetition());
}

bool ChessEngine::isGameOver() {
    return isCheckmate() || isStalemate() || isDraw();
}

std::variant<vector<Move>, vector<string>> ChessEngine::moves(bool verbose, std::optional<string> square, std::optional<char> piece) {
    vector<Move> moves = _moves(false, piece, square);
    vector<Move> verboseResult;
    vector<string> stringResult;
    if (verbose) {
        for (Move move : moves) {
            verboseResult.push_back(_makePretty(move));
        }
        return verboseResult;
    } else {
        for (Move move : moves) {
            stringResult.push_back(_moveToSan(move, moves));
        }
        return stringResult;
    }

    return verboseResult;
}

vector<Move> ChessEngine::_moves(bool legal, std::optional<char> piece, std::optional<string> square) {
    string forSquare;

    if (square) {
        forSquare = square.value();
        for (auto &x : forSquare) {
            x = tolower(x);
        }
    }

    char forPiece;
    if (piece) {
        forPiece = tolower(piece.value());
    }
    vector<Move> moves = {};
    char us = _turn;
    char them = swapColor(us);
    int firstSquare = Ox88.at("a8");
    int lastSquare = Ox88.at("h1");
    bool singleSquare = false;

    if (forSquare != "") {
        if (Ox88.find(forSquare) == Ox88.end()) {
            return {};
        } else {
            firstSquare = lastSquare = Ox88.at(forSquare);
            singleSquare = true;
        }
    }

    for (int from = firstSquare; from <= lastSquare; from++) {
        if (from & 0x88) {
            from += 7;
            continue;
        }
        if (_board.find(from) == _board.end() || _board.at(from).at("color") == them) {
            continue;
        }
        char type = _board.at(from).at("type");
        int to;

        if (type == PAWN) {
            if (forPiece != '\0' && forPiece != type)
                continue;
            to = from + PAWN_OFFSETS.at(us).at(0);

            if (_board.find(to) == _board.end()) {
                addMove(moves, us, from, to, PAWN);

                to = from + PAWN_OFFSETS.at(us).at(1);
                if (SECOND_RANK.at(us) == rank(from) && _board.find(to) == _board.end()) {
                    addMove(
                        moves,
                        us,
                        from,
                        to,
                        PAWN,
                        '\0',
                        BITS.at("BIG_PAWN"));
                }
            }
            for (int j = 2; j < 4; j++) {
                to = from + PAWN_OFFSETS.at(us).at(j);
                if (to & 0x88)
                    continue;
                if (_board.find(to) != _board.end()) {
                    if (_board.at(to).at("color") == them) {
                        addMove(
                            moves,
                            us,
                            from,
                            to,
                            PAWN,
                            _board.at(to).at("type"),
                            BITS.at("CAPTURE"));
                    }
                } else if (to == _epSquare) {
                    addMove(
                        moves, us, from, to, PAWN, PAWN, BITS.at("EP_CAPTURE"));
                }
            }
        } else {
            if (forPiece != '\0' && forPiece != type)
                continue;
            // for (int j = 0, int len = PIECE_OFFSETS.at(type).size(); j < len; j++)
            for (int j = 0; j < PIECE_OFFSETS.at(type).size(); j++) {
                int offset = PIECE_OFFSETS.at(type).at(j);
                to = from;
                while (true) {
                    to += offset;
                    if (to & 0x88)
                        break;
                    if (_board.find(to) == _board.end()) {
                        addMove(moves, us, from, to, type);
                    } else {
                        if (_board.at(to).at("color") == us)
                            break;
                        addMove(
                            moves,
                            us,
                            from,
                            to,
                            type,
                            _board.at(to).at("type"),
                            BITS.at("CAPTURE"));
                        break;
                    }
                    if (type == KNIGHT || type == KING)
                        break;
                }
            }
        }
    }
    if (forPiece == '\0' || forPiece == KING) {
        if (!singleSquare || lastSquare == _kings.at(us)) {
            if (_castling.at(us) & BITS.at("KSIDE_CASTLE")) {
                int castlingFrom = _kings.at(us);
                int castlingTo = castlingFrom + 2;
                if (
                    _board.find(castlingFrom + 1) == _board.end() &&
                    _board.find(castlingTo) == _board.end() &&
                    !_attacked(them, _kings.at(us)) &&
                    !_attacked(them, castlingFrom + 1) &&
                    !_attacked(them, castlingTo)) {
                    addMove(
                        moves,
                        us,
                        _kings.at(us),
                        castlingTo,
                        KING,
                        '\0',
                        BITS.at("KSIDE_CASTLE"));
                }
            }
            if (_castling.at(us) & BITS.at("QSIDE_CASTLE")) {
                int castlingFrom = _kings.at(us);
                int castlingTo = castlingFrom - 2;
                if (
                    _board.find(castlingFrom - 1) == _board.end() &&
                    _board.find(castlingFrom - 2) == _board.end() &&
                    _board.find(castlingFrom - 3) == _board.end() &&
                    !_attacked(them, _kings.at(us)) &&
                    !_attacked(them, castlingFrom - 1) &&
                    !_attacked(them, castlingTo)) {
                    addMove(
                        moves,
                        us,
                        _kings.at(us),
                        castlingTo,
                        KING,
                        '\0',
                        BITS.at("QSIDE_CASTLE"));
                }
            }
        }
    }
    if (!legal || _kings.at(us) == -1) {
        return moves;
    }

    vector<Move> legalMoves = {};
    for (int i = 0, len = moves.size(); i < len; i++) {
        _makeMove(moves.at(i));

        if (!_isKingAttacked(us)) {
            legalMoves.push_back(moves.at(i));
        } 
        _undoMove();
    }

    return legalMoves;
}

Move ChessEngine::_makePretty(Move uglyMove) {
    char color = uglyMove.color;
    char piece = uglyMove.piece;
    char flags = uglyMove.flags;
    int from = uglyMove.from;
    int to = uglyMove.to;
    char captured = uglyMove.captured;
    char promotion = uglyMove.promotion;

    string prettyFlags = "";
    for (auto flag : BITS) {
        if (flag.second & flags) {
            prettyFlags += FLAGS.at(flag.first);
        }
    }

    string fromAlgebraic = algebraic(from);
    string toAlgebraic = algebraic(to);

    Move move;
    move.color = color;
    move.piece = piece;
    move.from = from;
    move.to = to;
    move.san = _moveToSan(uglyMove, _moves(true));
    move.flags = prettyFlags[0];
    move.lan = fromAlgebraic + toAlgebraic;
    move.before = fen();
    move.after = "";
    _makeMove(uglyMove);

    move.after = fen();
    _undoMove();

    if (captured) {
        move.captured = captured;
    }

    if (promotion) {
        move.promotion = promotion;
        move.lan += promotion;
    }

    return move;
}

char ChessEngine::turn() {
    return _turn;
}

vector<vector<ChessEngine::Row>> ChessEngine::board() {
    vector<vector<Row>> output;
    vector<Row> row;
    for (int i = 0; i < 129; i++) {
        if (_board.find(i) == _board.end()) {
            Row tempRow;
            row.push_back(tempRow);
        } else {
            Row tempRow;
            tempRow.square = algebraic(i);
            tempRow.type = _board[i]["type"];
            tempRow.color = _board[i]["color"];
            row.push_back(tempRow);
        }
        if ((i + 1) & 0x88) {
            output.push_back(row);
            row = {};
            i += 8;
        }
    }
    return output;
}

int ChessEngine::_getPositionCount(string fen)

{
    string trimmedFen = trimFen(fen);
    if (_positionCount.find(trimmedFen) != _positionCount.end()) {
        return _positionCount[trimmedFen];
    }
    return 0;
}

void ChessEngine::_incPositionCount(string fen) {
    string trimmedFen = trimFen(fen);
    if (_positionCount.find(trimmedFen) == _positionCount.end()) {
        _positionCount[trimmedFen] = 0;
    }
    _positionCount[trimmedFen] += 1;
}

void ChessEngine::_decPositionCount(string fen) {
    string trimmedFen = trimFen(fen);
    if (_positionCount[trimmedFen] == 1) {
        _positionCount[trimmedFen] = 0;
    }
    _positionCount[trimmedFen] += 1;
}
