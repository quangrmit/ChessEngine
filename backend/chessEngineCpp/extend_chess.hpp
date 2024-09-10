#include <iostream>

#include "chess-library/include/chess.hpp"

using namespace chess;

class Extend_Board : public Board {
   public:
    Extend_Board() : Board() {}
    Extend_Board(std::string_view fen) : Board(fen) {}

    /// @brief Checks how many times a square is attacked by the given color.
    /// @param square
    /// @param color
    /// @return
    [[nodiscard]] int timesAttacked(Square square, Color color) const {
        int count = 0;
        // cheap checks first
        count += (attacks::pawn(~color, square) & pieces(PieceType::PAWN, color)).count();

        count += (attacks::knight(square) & pieces(PieceType::KNIGHT, color)).count();
        count += (attacks::king(square) & pieces(PieceType::KING, color)).count();
        count += (attacks::bishop(square, occ()) & (pieces(PieceType::BISHOP, color) | (pieces(PieceType::QUEEN, color)))).count();

        count += (attacks::rook(square, occ()) & (pieces(PieceType::ROOK, color) | (pieces(PieceType::QUEEN, color)))).count();

        return count;
    }

    virtual void setFen(std::string_view fen) {
        Board::setFen(fen);
        inc = 0;
    }

    int inc = 0;

   protected:
    void placePiece(Piece piece, Square sq) {
        Board::placePiece(piece, sq);
        inc++;
    }

    void removePiece(Piece piece, Square sq) {
        Board::removePiece(piece, sq);
        inc--;
    }
};