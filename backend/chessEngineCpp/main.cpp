#include <iostream>
#include <chrono>
#include "chess-library/include/chess.hpp"
#include "extend_chess.hpp"

using namespace chess;

#include "alpha_beta.h"

// #include <chrono>

using namespace std;

// int countOnes(Bitboard num) {
//     int count = 0;
//     while (num) {
//         num &= (num - 1); // This operation clears the lowest set bit
//         ++count;
//     }
//     return count;
// }

int main(int argc, char *argv[])
{
    string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    // string fen = "rnbqkbnr/ppp1pppp/8/8/8/2P5/PP1PN1pP/RNBQKB1R w KQkq - 0 5";
    // fen = "rnbqkbnr/ppp1pppp/8/8/8/1PP5/P2PN1pP/RNBQKB1R b KQkq - 0 5";

    // string test = "3Rkbnr/ppp1pppp/8/8/4P3/2N2N2/PPP2PPP/R1B3K1 b - - 0 13";
    // string fen = argv[1];
    // map<string, variant<string, int, vector<any>>> res = minimax(fen, 10);
    // cout << get<string>(res["fen"]) << " " << "" << endl;
    // getchar();
    // cout << argv[1] << endl;


    fen = "";
    for (int i = 1; i < argc; i++)
    {
        fen += argv[i];
        fen += " ";
    }
    fen.pop_back();


    // string test1 = "1rb5/p1pp1p2/p1n2n2/2P2k2/6p1/3P4/r7/6K1 b - - 0 30";
    // string test2 = "6r1/2p2p1p/p2pk2p/1p1p4/P6P/7K/P7/3b1n2 b - - 2 32";

    string move = bestMove(fen, 1);
    cout << move << endl;



    // Board b = Board(fen);
    // Movelist moves;
    // movegen::legalmoves(moves, b);


    // for (auto move: moves){
    //     cout << uci::moveToUci(move) << endl;

    // }
    // string m = bestMove(fen, 4);
    // cout << m << endl;



    return 0;
}