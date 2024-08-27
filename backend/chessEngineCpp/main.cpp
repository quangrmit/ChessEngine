#include <iostream>

#include "alpha_beta.h"

using namespace std;

int main(int argc, char *argv[])
{
    string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    // string fen = argv[1];
    map<string, variant<string, int, vector<any>>> res = minimax(fen, 1);

    // ChessEngine* c = new ChessEngine("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1");

    // std::cout << validateFen("1r6/3bqkb1/8/pppppppr/4P3/8/PPPP1PPP/RNBQKBNR w - - 0 23").status;


    // std::cout << c->fen() ;

    // ChessEngine* c = new ChessEngine();

    // _moves()
    // _makePretty()


    // cout << get<string>(res["fen"]) << " " << "" << endl;
    // cout << "hello";
    // getchar();
    // cout << argv[1] << endl;
    return 0;
}