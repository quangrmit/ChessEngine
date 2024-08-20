#ifndef _CHESSENGINE_H
#define _CHESSENGINE_H

#include <iostream>
#include <map>
#include <unordered_map>
#include <stdexcept>
#include <regex>
#include <optional>
#include <variant>
#include "string"
#include "vector"

using std::map;
using std::string;
using std::unordered_map;
using std::vector;

// Helper function
const char WHITE = 'w';
const char BLACK = 'b';
const char PAWN = 'p';
const char KNIGHT = 'n';
const char BISHOP = 'b';
const char ROOK = 'r';
const char QUEEN = 'q';
const char KING = 'k';
const string DEFAULT_POSITION = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
const int EMPTY = -1;
const map<string, char> FLAGS =
    {{"NORMAL", 'n'},
     {"CAPTURE", 'c'},
     {"BIG_PAWN", 'b'},
     {"EP_CAPTURE", 'e'},
     {"PROMOTION", 'p'},
     {"KSIDE_CASTLE", 'k'},
     {"QSIDE_CASTLE", 'q'}};

const string SQUARES[64] = {
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"};

const map<string, int> BITS = {
    {"NORMAL", 1},
    {"CAPTURE", 2},
    {"BIG_PAWN", 4},
    {"EP_CAPTURE", 8},
    {"PROMOTION", 16},
    {"KSIDE_CASTLE", 32},
    {"QSIDE_CASTLE", 64}};

const map<string, int> Ox88 =
    {{"a8", 0}, {"b8", 1}, {"c8", 2}, {"d8", 3}, {"e8", 4}, {"f8", 5}, {"g8", 6}, {"h8", 7}, {"a7", 16}, {"b7", 17}, {"c7", 18}, {"d7", 19}, {"e7", 20}, {"f7", 21}, {"g7", 22}, {"h7", 23}, {"a6", 32}, {"b6", 33}, {"c6", 34}, {"d6", 35}, {"e6", 36}, {"f6", 37}, {"g6", 38}, {"h6", 39}, {"a5", 48}, {"b5", 49}, {"c5", 50}, {"d5", 51}, {"e5", 52}, {"f5", 53}, {"g5", 54}, {"h5", 55}, {"a4", 64}, {"b4", 65}, {"c4", 66}, {"d4", 67}, {"e4", 68}, {"f4", 69}, {"g4", 70}, {"h4", 71}, {"a3", 80}, {"b3", 81}, {"c3", 82}, {"d3", 83}, {"e3", 84}, {"f3", 85}, {"g3", 86}, {"h3", 87}, {"a2", 96}, {"b2", 97}, {"c2", 98}, {"d2", 99}, {"e2", 100}, {"f2", 101}, {"g2", 102}, {"h2", 103}, {"a1", 112}, {"b1", 113}, {"c1", 114}, {"d1", 115}, {"e1", 116}, {"f1", 117}, {"g1", 118}, {"h1", 119}};

const map<char, vector<int>> PAWN_OFFSETS =
    {{'b', {16, 32, 17, 15}},
     {'w', {-16, -32, -17, -15}}};

const map<char, vector<int>> PIECE_OFFSETS = {
    {'n', {-18, -33, -31, -14, 18, 33, 31, 14}},
    {'b', {-17, -15, 17, 15}},
    {'r', {-16, 1, 16, -1}},
    {'q', {-17, -16, -15, 1, 17, 16, 15, -1}},
    {'k', {-17, -16, -15, 1, 17, 16, 15, -1}}};

const int ATTACKS[240] = {
    20, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 20, 0,
    0, 20, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 20, 0, 0,
    0, 0, 20, 0, 0, 0, 0, 24, 0, 0, 0, 0, 20, 0, 0, 0,
    0, 0, 0, 20, 0, 0, 0, 24, 0, 0, 0, 20, 0, 0, 0, 0,
    0, 0, 0, 0, 20, 0, 0, 24, 0, 0, 20, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 20, 2, 24, 2, 20, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 2, 53, 56, 53, 2, 0, 0, 0, 0, 0, 0,
    24, 24, 24, 24, 24, 24, 56, 0, 56, 24, 24, 24, 24, 24, 24, 0,
    0, 0, 0, 0, 0, 2, 53, 56, 53, 2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 20, 2, 24, 2, 20, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 20, 0, 0, 24, 0, 0, 20, 0, 0, 0, 0, 0,
    0, 0, 0, 20, 0, 0, 0, 24, 0, 0, 0, 20, 0, 0, 0, 0,
    0, 0, 20, 0, 0, 0, 0, 24, 0, 0, 0, 0, 20, 0, 0, 0,
    0, 20, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 20, 0, 0,
    20, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 20};

const int RAYS[240] = {
    17, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 15, 0,
    0, 17, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 15, 0, 0,
    0, 0, 17, 0, 0, 0, 0, 16, 0, 0, 0, 0, 15, 0, 0, 0,
    0, 0, 0, 17, 0, 0, 0, 16, 0, 0, 0, 15, 0, 0, 0, 0,
    0, 0, 0, 0, 17, 0, 0, 16, 0, 0, 15, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 17, 0, 16, 0, 15, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 17, 16, 15, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 0, -1, -1, -1, -1, -1, -1, -1, 0,
    0, 0, 0, 0, 0, 0, -15, -16, -17, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -15, 0, -16, 0, -17, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -15, 0, 0, -16, 0, 0, -17, 0, 0, 0, 0, 0,
    0, 0, 0, -15, 0, 0, 0, -16, 0, 0, 0, -17, 0, 0, 0, 0,
    0, 0, -15, 0, 0, 0, 0, -16, 0, 0, 0, 0, -17, 0, 0, 0,
    0, -15, 0, 0, 0, 0, 0, -16, 0, 0, 0, 0, 0, -17, 0, 0,
    -15, 0, 0, 0, 0, 0, 0, -16, 0, 0, 0, 0, 0, 0, -17};

const string SYMBOLS = "pnbrqkPNBRQK";
const char PROMOTIONS[4] = {KNIGHT, BISHOP, ROOK, QUEEN};
const int RANK_1 = 7;
const int RANK_2 = 6;
const int RANK_7 = 1;
const int RANK_8 = 0;

const map<char, int> PIECE_MASKS = {
    {'p', 0x1},
    {'n', 0x2},
    {'b', 0x4},
    {'r', 0x8},
    {'q', 0x10},
    {'k', 0x20}};

const map<char, int> SIDES = {
    {KING, BITS.at("KSIDE_CASTLE")},
    {QUEEN, BITS.at("QSIDE_CASTLE")}};

const map<char, vector<map<string, int>>> ROOKS = {
    {'w',
     {
         {{"square", Ox88.at("a1")}, {"flag", BITS.at("QSIDE_CASTLE")}},
         {{"square", Ox88.at("h1")}, {"flag", BITS.at("KSIDE_CASTLE")}},
     }},
    {'b',
     {
         {{"square", Ox88.at("a8")}, {"flag", BITS.at("QSIDE_CASTLE")}},
         {{"square", Ox88.at("h8")}, {"flag", BITS.at("KSIDE_CASTLE")}},
     }},
};

const string TERMINATION_MARKERS[4] = {"1-0", "0-1", "1/2-1/2", "*"};

const map<char, int> SECOND_RANK = {{'b', RANK_7}, {'w', RANK_2}};

struct Move
{
    char color = '\0';
    int from;
    int to;
    string san = "";
    string lan = "";
    string before = "";
    string after = "";
    char piece = '\0';
    char captured = '\0';
    char promotion = '\0';
    char flags;
};

// true and false are encoded as strings (eg. 'true' , 'false')

struct fenResult
{
    bool status;
    string errorMessage;
};

int rank(int square);

int file(int square);

bool isDigit(char c);

string algebraic(int square);

char swapColor(char color);

fenResult validateFen(string fen);

string getDisambiguator(Move move, vector<Move> moves);

void addMove(vector<Move> &moves, char color, int from, int to, char piece, char captured = '\0', int flags = BITS.at("NORMAL"));

char inferPieceType(string san);

string strippedSan(string move);

string trimFen(string fen);

// Chess Engine Class
class ChessEngine
{
public:
    struct HistoryMove
    {
        Move move;
        map<char, int> kings;
        char turn;
        map<char, int> castling;
        int epSquare;
        int halfMoves;
        int moveNumber;
    };

    map<int, map<string, char>> _board;
    char _turn;
    map<string, string> _header;
    map<char, int> _kings;
    int _epSquare;
    int _halfMoves;
    int _moveNumber;
    vector<HistoryMove> _history;
    map<string, string> _comments;
    map<char, int> _castling;
    map<string, int> _positionCount;

    ChessEngine();
    ChessEngine(string fen = DEFAULT_POSITION);

    ~ChessEngine();

    void clear(map<string, bool> config = {{"preserveHeaders", false}});

    // void removeHeader() cái này ko cần làm vì ko cần call

    void load(string fen, map<string, bool> config = {{"skipValidation", false}, {"preserveHeaders", false}});

    string fen();

    void _updateSetup(string fen);

    void reset();

    map<string, char> get(string square); // needs reviewing

    // struct _putConfig{
    //     char type;
    //     char color;
    // }
    bool put(map<string, char> config, string square);
    bool _put(map<string, char> config, string square);

    // remove()

    void _updateCastlingRights();

    bool canCapture(int square);

    void _updateEnPassantSquare();

    bool _attacked(char color, int square);

    bool _isKingAttacked(char color);

    bool inCheck();

    bool isCheckmate();

    bool isStalemate();

    bool isSufficientMaterial();

    bool isThreefoldRepetition();

    bool isDraw();

    bool isGameOver();

    // vector<Move> _moves(vector<Move> moves);

    std::variant<vector<Move>, vector<string>> moves(bool verbose = false, std::optional<string> square = std::nullopt, std::optional<char> piece = std::nullopt);

    vector<Move> _moves(bool legal = true, std::optional<char> piece = std::nullopt, std::optional<string> square = std::nullopt);

    Move move(map<string, string> move, map<string, bool> config = {});

    void _push(Move move);

    void _makeMove(Move move);

    Move undo();

    Move _undoMove();

    map<string, string> header(vector<string> args);

    // void loadPgn();

    string _moveToSan(Move move, vector<Move> moves);

    Move _moveFromSan(string move, bool strict = false);

    // void ascii();

    // void perft();

    // use the same struct for both pretty and uglymove
    Move _makePretty(Move uglyMove);

    bool isAttacked(string square, char attackedBy);

    bool isCheck();

    char turn();

    struct Row
    {
        string square;
        char type;
        char color;
    };

    vector<vector<Row>> board();

    // void squareColor();

    // void history();

    int _getPositionCount(string fen);

    void _incPositionCount(string fen);

    void _decPositionCount(string fen);

    // void _pruneComments();

    // void getComment();

    // void setComment(string comment);

    // void deleteComment();

    // void getComments();

    // void deleteComments();

    // bool setCastlingRights(char color, map<char, int> rights);

    // map<vector<char>, bool> getCastlingRights(char color);

    // int moveNumber();
};

#endif