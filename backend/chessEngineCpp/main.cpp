#include <iostream>

#include "alpha_beta.h"

using namespace std;

int main(int argc, char *argv[])
{
    string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    map<string, variant<string, int, vector<any>>> res = minimax(fen, 0);
    cout << get<string>(res["fen"]) << endl;
    getchar();

    return 0;
}