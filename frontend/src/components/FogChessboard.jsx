import { Chess } from "chess.js";
import { Chessboard } from "react-chessboard";
import Clock from "./Clock";
import React from "react";
import { useState } from "react";

function FogChessboard() {
    const [fen, setFen] = useState("start");
    const [game, setGame] = useState(new Chess());
    const [isWhiteTurn, setIsWhiteTurn] = useState(true);

    function makeAMove(move) {
        try {
            const result = game.move(move);
            return result;
        } catch (e) {
            console.log(e); // Information of incorrect move
            console.log("Illegal move / Not your turn");
            return null;
        }
    }
    // function makeRandomMove() {
    //   const possibleMoves = game.moves();
    //   if (game.game_over() || game.in_draw() || possibleMoves.length === 0)
    //     return; // exit if the game is over
    //   const randomIndex = Math.floor(Math.random() * possibleMoves.length);
    //   makeAMove(possibleMoves[randomIndex]);
    // }

    function onDrop(sourceSquare, targetSquare) {
        const move = makeAMove({
            from: sourceSquare,
            to: targetSquare,
        });

        // illegal move
        if (move === null) return false;
        setFen(game.fen());
        //   setTimeout(makeRandomMove, 200);
        // switch clock
        setIsWhiteTurn(prev => !prev)

        return true;
    }

    return (
        <>
            <div id="containerBoard">
                <Clock isWhite={false} ticking={!isWhiteTurn}/>
                    <Chessboard position={fen} onPieceDrop={onDrop} />
                <Clock isWhite={true} ticking={isWhiteTurn}/>
            </div>
        </>
    );
}

export default FogChessboard;
