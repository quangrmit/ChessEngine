#include <iostream>

#include "alpha_beta.h"

// #include <chrono>

using namespace std;

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





    // string test = "r1bqkbnr/pppppppp/2n5/3P4/8/8/PPP1PPPP/RNBQKBNR b KQkq - 0 2";
    // string test1 = "1rbqkbnr/pppppppp/2P5/8/8/8/PPP1PPPP/RNBQKBNR b KQk - 0 3";

    // int e = eval(test);
    // int e1 = eval(test1);
    // cout << "test " << e << endl;
    // cout << "test1 " << e1 << endl;







//     auto start2 = std::chrono::high_resolution_clock::now();

//     Move move2 = bestMoveHeap(fen, 3);

//     auto stop2 = std::chrono::high_resolution_clock::now();

//     auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(stop2 - start2);
 
//     cout << "Time taken by ab pruning: "
//          << duration2.count() << " microseconds" << endl;


//     auto start1 = std::chrono::high_resolution_clock::now();

    Move move1 = bestMove(fen,2);

//     auto stop1 = std::chrono::high_resolution_clock::now();

//     auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(stop1 - start1);
 
//     cout << "Time taken by minimax: "
//          << duration1.count() << " microseconds" << endl;




    
    // cout << "The Best move is " << endl;
    // cout << algebraic(move.from) << " " << algebraic(move.to) << endl;
    cout << move1.lan << endl;
    // cout << move2.lan << endl;

    return 0;
}