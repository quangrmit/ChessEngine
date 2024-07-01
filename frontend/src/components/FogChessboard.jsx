import Chess from "../modules/Chess";
import SQUARES from '../modules/Chess'
import { Chessboard } from "react-chessboard";
import Clock from "./Clock";
import React, { useEffect } from "react";
import WinDialog from "./WinDialog";
import Sidebar from "./Sidebar";

import { useState } from "react";
import { useStepContext } from "@mui/material";
import FocusTrap from "@mui/material/Unstable_TrapFocus";

export default function FogChessboard() {
    const [fen, setFen] = useState("start");
    const [game, setGame] = useState(new Chess());
    const [isWhiteTurn, setIsWhiteTurn] = useState(true);
    const [open, setOpen] = useState(false)
    const [message, setMessage] = useState("")
    const [moveCount, setMoveCount] = useState(0)
    const [controlGame, setControlGame] = useState(true)
    const [playBack, setPlayBack] = useState(false)
    const [currentPlayBackIndex, setCurrentPlayBackIndex] = useState(0)
    const [moveFrom, setMoveFrom] = useState(null);
    const [optionSquares, setOptionSquares] = useState({});
    const [whitePieceSquares, setWhitePieceSquare] = useState( new Set([
                'a2', 'b2', 'c2', 'd2', 'e2', 'f2', 'g2', 'h2',
        'a1', 'b1', 'c1', 'd1', 'e1', 'f1', 'g1', 'h1'
    ]))

    // Current problem: cannot generate possible moves if not in the correct turn
    // TODO: solve


    const genPossibleMoves = (wpsArray) => {
        console.log('this is wps array')
        console.log(wpsArray)
        const allMoves = []

        for (let i = 0; i < wpsArray.length; i++){
            let square = wpsArray[i]
            const moves = game.moves({
                square,
                verbose: true
            })

            for (let j = 0; j < moves.length; j ++){
                moves[j] = moves[j].to
            }
            allMoves.push(...moves)
        }
        return new Set(allMoves)
    }   


    const possibleMovesSquares = genPossibleMoves(Array.from(whitePieceSquares))
    console.log(possibleMovesSquares)

    const allSquares = [
        'a8', 'b8', 'c8', 'd8', 'e8', 'f8', 'g8', 'h8',
        'a7', 'b7', 'c7', 'd7', 'e7', 'f7', 'g7', 'h7',
        'a6', 'b6', 'c6', 'd6', 'e6', 'f6', 'g6', 'h6',
        'a5', 'b5', 'c5', 'd5', 'e5', 'f5', 'g5', 'h5',
        'a4', 'b4', 'c4', 'd4', 'e4', 'f4', 'g4', 'h4',
        'a3', 'b3', 'c3', 'd3', 'e3', 'f3', 'g3', 'h3',
        'a2', 'b2', 'c2', 'd2', 'e2', 'f2', 'g2', 'h2',
        'a1', 'b1', 'c1', 'd1', 'e1', 'f1', 'g1', 'h1'
    ];




    const fogSquares = Array.from((new Set(allSquares)).difference(new Set([...possibleMovesSquares, ...whitePieceSquares])))

    useEffect(() => {
        console.log('This is fog square')
        console.log(fogSquares)

        console.log('this is white piece squares')
        console.log(whitePieceSquares)

        console.log('this is possible moves squares')
        console.log(possibleMovesSquares)

    }, [fogSquares])


    function test() {

        // Have white piece squares
        // Have possible moves



        const allMoves = []
        for (let i = 0; i < whitePieceSquares.length; i ++){
            let square = whitePieceSquares[i]
            const moves = game.moves({
                square,
                verbose: true,
            });

            allMoves.push(...moves)
        }

        const newSquares = {}

        const allMovesSimple = allMoves.map((move) => move.to)

        // All possible moves square
        console.log(allMovesSimple)

        // All squares that has white piece


        for (let i = 0; i < whitePieceSquares.length; i ++){
            let square = whitePieceSquares[i]
            allMoves.map((move) => {
                newSquares[move.to] = {
                    background:
                        game.get(move.to) && game.get(move.to).color !== game.get(square).color
                            ? "radial-gradient(circle, rgba(0,0,0,.1) 85%, transparent 85%)"
                            : "radial-gradient(circle, rgba(0,0,0,.1) 25%, transparent 25%)",
                    borderRadius: "50%",
                };
                return move;
            });

        }
        console.log(newSquares)
        setOptionSquares(newSquares)

    }


    useEffect(() => {
        console.log('changed white piece squares')
        console.log(whitePieceSquares)
    }, [whitePieceSquares])

    // Manage win dialog state
    const resetGame = () => {
        setOpen(false)
        setControlGame(true)
        setMoveCount(0)
        game.reset()
        setFen(game.fen())
    }

    const cancelGame = () => {
        setOpen(false)
    }

    // Step through moves of a recorded game
    const stepPlaybackMove = () => {
        if (currentPlayBackIndex === preMoveList.length) {
            setCurrentPlayBackIndex(0);
            setFen(game.reset())
            return;
        }
        let currentMove = preMoveList[currentPlayBackIndex];
        let fromPos = currentMove.substring(currentMove.length - 4, currentMove.length - 2);
        let toPos = currentMove.substring(currentMove.length - 2);
        makeAMove({
            from: fromPos,
            to: toPos
        })
        setFen(game.fen());
        setCurrentPlayBackIndex(currentPlayBackIndex + 1);

    }    
    
    function makeAMove(move) {
        // if (!controlGame) return null;
        try {
            const result = game.move(move)
            console.log(result);
            // recordMoveList(result);
            return result;
        } catch (e) {
            // console.log(e) // Information of incorrect move
            console.log("No more move");
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
      
      // Everytime make a move set white piece squares to new value
      let newWhitePieceSquares = new Set(whitePieceSquares)
      newWhitePieceSquares.delete(sourceSquare)
      newWhitePieceSquares.add(targetSquare)
      setWhitePieceSquare(newWhitePieceSquares)

      // Check if a king has been captured
      if (!move.captured) return true;
      if (move.captured !== 'k') return true;
      if (move.color === 'b') setMessage(`Black win in ${moveCount} moves`)
      else setMessage(`White win in ${moveCount} moves`)
      setOpen(true)
      setControlGame(false)  
      return true;
    }

    function getMoveOptions(square) {
        console.log(square)
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
        if (piece !== undefined) {
            // 'w' or 'b'
            let side = piece.slice(0, 1);

            // if same color piece as current player turn
            if ((side === "w" && isWhiteTurn) || (side === "b" && !isWhiteTurn)) {
                const hasMoveOptions = getMoveOptions(square);
                if (hasMoveOptions) setMoveFrom(square);
                return;
            }
        }

        onDrop(moveFrom, square);
        setMoveFrom(null);
        setOptionSquares({});
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
                        ...optionSquares,
                    }}
                />
                ;
                <Clock isWhite={true} ticking={isWhiteTurn} />
            </div>
            <Sidebar resetGame={resetGame} stepPlaybackMove={stepPlaybackMove}></Sidebar>
        </>
    );
}
