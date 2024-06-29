import Chess from "../modules/Chess";
import { Chessboard } from "react-chessboard";
import Clock from "./Clock";
import React from "react";
import WinDialog from "./WinDialog";

import { useState } from "react";

export default function FogChessboard() {
    const [fen, setFen] = useState("start");
    const [game, setGame] = useState(new Chess());
    const [isWhiteTurn, setIsWhiteTurn] = useState(true);
    const [open, setOpen] = useState(false);
    const [message, setMessage] = useState("");
    const [moveCount, setMoveCount] = useState(0);

    const [moveFrom, setMoveFrom] = useState(null);
    const [optionSquares, setOptionSquares] = useState({});


    // Manage win dialog state
    const resetGame = () => {
        setOpen(false);
        game.reset();
        setFen(game.fen());
    };

    const cancelGame = () => {
        setOpen(false);
    };

    function makeAMove(move) {
        try {
            const result = game.move(move);
            console.log(moveCount);
            return result;
        } catch (e) {
            // console.log(e) // Information of incorrect move
            console.log("Illegal move");
            return null;
        }
    }

    function onDrop(sourceSquare, targetSquare) {
        const move = makeAMove({
            from: sourceSquare,
            to: targetSquare,
        });

        // illegal move
        if (move === null) return false;

        // legal move
        setFen(game.fen());
        setMoveCount(moveCount + 1);
        setIsWhiteTurn((prev) => !prev); // switch clock

        // Check if the King has been captured for restarting the game
        let gameState = game.fen().split(" ")[0];
        // If k (Black King) not on the game field, white wins
        if (!gameState.includes("k")) {
            setMessage(`White win in ${moveCount} moves`);
            setOpen(true);
        }
        // If K (White King) not on the game field, black wins
        else if (!gameState.includes("K")) {
            setMessage(`Black win in ${moveCount} moves`);
            setOpen(true);
        }

        return true;
    }

    function getMoveOptions(square) {
        const moves = game.moves({
            square,
            verbose: true,
        });
        console.log(moves);
        if (moves.length === 0) {
            setOptionSquares({});
            return false;
        }

        const newSquares = {};
        moves.map((move) => {
            newSquares[move.to] = {
                background:
                    game.get(move.to) && game.get(move.to).color !== game.get(square).color
                        ? "radial-gradient(circle, rgba(0,0,0,.1) 85%, transparent 85%)"
                        : "radial-gradient(circle, rgba(0,0,0,.1) 25%, transparent 25%)",
                borderRadius: "50%",
            };
            return move;
        });
        newSquares[square] = {
            background: "rgba(255, 255, 0, 0.4)",
        };
        setOptionSquares(newSquares);
        return true;
    }

    function onSquareClick(square, piece) {        
        if (piece !== undefined){
            // 'w' or 'b'
            let side = piece.slice(0,1)

            // if same color piece as current player turn
            if ((side === 'w' && isWhiteTurn) || (side === 'b' && !isWhiteTurn)){
                const hasMoveOptions = getMoveOptions(square);
                if (hasMoveOptions) setMoveFrom(square);
                return ;
            }
        }

        onDrop(moveFrom, square)
        setMoveFrom(null)
        setOptionSquares({})
        
    }   

    return (
        <>
            <WinDialog
                open={open}
                resetGame={resetGame}
                cancleGame={cancelGame}
                message={message}
            ></WinDialog>
            <div id="containerBoard">
                <Clock isWhite={false} ticking={!isWhiteTurn} />
                <Chessboard
                    position={fen}
                    onPieceDrop={onDrop}
                    onSquareClick={onSquareClick}
                    customSquareStyles={{
                        ...optionSquares
                    }}
                />
                ;
                <Clock isWhite={true} ticking={isWhiteTurn} />
            </div>
        </>
    );
}
