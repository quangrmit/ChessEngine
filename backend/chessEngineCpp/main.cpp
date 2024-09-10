#include <iostream>
#include <chrono>
#include "chess-library/include/chess.hpp"

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




    string move = bestMove(fen, 4);
    cout << move << endl;








    return 0;
}