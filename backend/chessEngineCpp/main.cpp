#include <iostream>
#include <chrono>
#include "chess-library/include/chess.hpp"

using namespace chess;

// #include "alpha_beta.h"

// #include <chrono>

using namespace std;

uint64_t perft(Board &board, int depth)
{
    Movelist moves;
    movegen::legalmoves(moves, board);

    if (depth == 1)
    {
        return moves.size();
    }

    uint64_t nodes = 0;

    for (int i = 0; i < moves.size(); i++)
    {
        const auto move = moves[i];
        board.makeMove(move);
        nodes += perft(board, depth - 1);
        board.unmakeMove(move);
    }

    return nodes;
}

int main(int argc, char *argv[])
{
    string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    // string test = "3Rkbnr/ppp1pppp/8/8/4P3/2N2N2/PPP2PPP/R1B3K1 b - - 0 13";
    // string fen = argv[1];
    // map<string, variant<string, int, vector<any>>> res = minimax(fen, 10);
    // cout << get<string>(res["fen"]) << " " << "" << endl;
    // getchar();
    // cout << argv[1] << endl;
    // fen = "";
    // for (int i = 1; i < argc; i++)
    // {
    //     fen += argv[i];
    //     fen += " ";
    // }
    // fen.pop_back();

    // Move move = bestMove(fen);
    // cout << move.lan << endl;

    Board board = Board(fen);
    Movelist moves;
    movegen::legalmoves(moves, board);

    // for (const auto &move : moves)
    // {
    //     std::cout << uci::moveToUci(move) << std::endl;
    // }

    int depth = 6;

    auto start2 = std::chrono::high_resolution_clock::now();

    int nodes = perft(board, depth);
    cout << nodes << endl;

    auto stop2 = std::chrono::high_resolution_clock::now();

    auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(stop2 - start2);

    cout << "Time taken by perft: "
         << duration2.count() << " miliseconds" << endl;
    // cout << "Hello world" << endl;
    return 0;
}