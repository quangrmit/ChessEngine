import { Chessboard } from "react-chessboard";
import Clock from "./Clock";
import Chess from "../modules/Chess.js";

import React, { useEffect, useState } from "react";
import WinDialog from "./WinDialog";
import Sidebar from "./Sidebar";
import {defaultPieces} from '../modules/pieces'
import { pie } from "d3";




export default function FogChessboard() {
    const initFen = "rnbqkbnr/ppp1pppp/8/8/8/2P5/PP1PN1pP/RNBQKB1R w KQkq - 0 5"
    const [fen, setFen] = useState("start");
    const [game, setGame] = useState(new Chess());
    const [isWhiteTurn, setIsWhiteTurn] = useState(true);
    const [open, setOpen] = useState(false);
    const [message, setMessage] = useState("");
    const [moveCount, setMoveCount] = useState(0);
    const [controlGame, setControlGame] = useState(true);
    const [playBack, setPlayBack] = useState(false);
    const [currentPlayBackIndex, setCurrentPlayBackIndex] = useState(0);
    const [moveFrom, setMoveFrom] = useState(null);
    const [optionSquares, setOptionSquares] = useState({});
    
    // prettier-ignore
    const [whitePieceSquares, setWhitePieceSquare] = useState( new Set([
                'a2', 'b2', 'c2', 'd2', 'e2', 'f2', 'g2', 'h2',
        'a1', 'b1', 'c1', 'd1', 'e1', 'f1', 'g1', 'h1'
    ]))

    // prettier-ignore
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

    const genPossibleMoves = (currGame, wpsArray) => {
        const allMoves = [];

        for (let i = 0; i < wpsArray.length; i++) {
            let square = wpsArray[i];
            const moves = currGame.moves({
                square,
                verbose: true,
            });

            for (let j = 0; j < moves.length; j++) {
                moves[j] = moves[j].to;
            }
            allMoves.push(...moves);
        }
        return new Set(allMoves);
    };

    const genMovesFromOppTurn = (currGame, wpsArray) => {
        let fenCopy = currGame.fen();
        fenCopy = fenCopy.replace("b ", "w ");
        let gameSame = new Chess(fenCopy);

        return genPossibleMoves(gameSame, Array.from(wpsArray));
    };

    const possibleMovesSquaresWhiteTurn = genPossibleMoves(game, Array.from(whitePieceSquares));

    let [possibleMovesSquares, setPossibleMovesSquares] = useState(Array.from(possibleMovesSquaresWhiteTurn));

    function fog(fqs) {
        const newSquares = {};

        fqs.map((square) => {
            newSquares[square] = {
                background: "grey",
                // position: 'sticky',
                zIndex: '-100'
            };
        });

        return newSquares;
    }

    const fogSquares = Array.from(
        new Set(allSquares).difference(new Set([...possibleMovesSquares, ...whitePieceSquares]))
    );

    useEffect(() => {
        // If it is white's turn
        if (isWhiteTurn) {
            setPossibleMovesSquares(Array.from(possibleMovesSquaresWhiteTurn));
            return;
        }

        setPossibleMovesSquares(Array.from(genMovesFromOppTurn(game, Array.from(whitePieceSquares))));
    }, [isWhiteTurn]);

    useEffect(() => {
        setOptionSquares(fog(fogSquares));
        // console.log(fogSquares);
        // console.log(possibleMovesSquares);
        // console.log(whitePieceSquares);
    }, [possibleMovesSquares]);

    useEffect(() => {
        // setOptionSquares(fogSquaresCssObj);
        console.log(game.fen());
        setOptionSquares(fog(fogSquares));
    }, []);

    
    // Call the AI every black's turn
    useEffect(() => {
        const engineMove = async () => {
            const url = "http://127.0.0.1:5501/api/engine"
            let response = await fetch(`${url}?fen=${game.fen()}`);
            response = await response.json()
            console.log(response);
            // onDrop(response.from, response.to);
            let {from, to, promotion} = response;
            console.log("response", from, to, promotion);
            onDrop(from, to, promotion);
        }
        if (!isWhiteTurn) {
            console.log("Black turn !!!");
            engineMove();
        }
    }, [isWhiteTurn])


    // Manage win dialog state
    const resetGame = () => {
        setOpen(false);
        setControlGame(true);
        setMoveCount(0);
        game.reset();
        setFen(game.fen());
    };

    const cancelGame = () => {
        setOpen(false);
    };

    // Step through moves of a recorded game
    const stepPlaybackMove = () => {
        if (currentPlayBackIndex === preMoveList.length) {
            setCurrentPlayBackIndex(0);
            setFen(game.reset());
            return;
        }
        let currentMove = preMoveList[currentPlayBackIndex];
        let fromPos = currentMove.substring(currentMove.length - 4, currentMove.length - 2);
        let toPos = currentMove.substring(currentMove.length - 2);
        makeAMove({
            from: fromPos,
            to: toPos,
        });
        setFen(game.fen());
        setCurrentPlayBackIndex(currentPlayBackIndex + 1);
    };

    function makeAMove(move) {
        // if (!controlGame) return null;
        try {
            const result = game.move(move);
            // recordMoveList(result);
            return result;
        } catch (e) {
            // console.log(e) // Information of incorrect move
            
            console.log("Invalid move");
            console.log(move);
            return null;
        }
    }

    function onDrop(sourceSquare, targetSquare, piece) {
            console.log("piece" + piece)
        let promo = (piece == undefined) ? null : piece[piece.length - 1].toLowerCase();
        console.log("promo" + promo);

        let move;
        
        if (promo){
            move = makeAMove({
                from: sourceSquare,
                to: targetSquare,
                promotion : promo
            });
        }else {
            move = makeAMove({
                from: sourceSquare,
                to: targetSquare,
            });
        }

  

        // illegal move
        if (move === null) return false;

        // legal move

        // Everytime make a move set white piece squares to new value
        if (isWhiteTurn) {
            let newWhitePieceSquares = new Set(whitePieceSquares);
            newWhitePieceSquares.delete(sourceSquare);
            newWhitePieceSquares.add(targetSquare);
            setWhitePieceSquare(newWhitePieceSquares);
        }else {
            if (whitePieceSquares.has(targetSquare)) {
                let newWhitePieceSquares = new Set(whitePieceSquares)
                newWhitePieceSquares.delete(targetSquare)
                setWhitePieceSquare(newWhitePieceSquares)
            }
        }

        setFen(game.fen());
        setMoveCount(moveCount + 1);
        setIsWhiteTurn((prev) => !prev); // switch clock

        // Check if a king has been captured
        if (!move.captured) return true;
        if (move.captured !== "k") return true;
        if (move.color === "b") setMessage(`Black win in ${moveCount} moves`);
        else setMessage(`White win in ${moveCount} moves`);
        setOpen(true);
        setControlGame(false);
        return true;
    }

    function getMoveOptions(square) {
        const moves = game.moves({
            square,
            verbose: true,
        });
        if (moves.length === 0) {
            // setOptionSquares(fogSquaresCssObj);
            // setOptionSquares({})
            return false;
        }

        const newSquares = {};
        moves.map((move) => {
            newSquares[move.to] = {
                // background:
                //     game.get(move.to) && game.get(move.to).color !== game.get(square).color
                //         ? "radial-gradient(circle, rgba(0,0,0,.1) 85%, transparent 85%)"
                //         : "radial-gradient(circle, rgba(0,0,0,.1) 25%, transparent 25%)",
                borderRadius: "50%",
            };
            return move;
        });
        newSquares[square] = {
            background: "rgba(255, 255, 0, 0.4)",
        };
        // setOptionSquares(newSquares);
        // setOptionSquares(fogSquaresCssObj)
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
        // setOptionSquares({});
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
                    position={game.fen()}
                    onPieceDrop={onDrop}
                    // onSquareClick={onSquareClick}
                    // customSquareStyles={{
                    //     ...optionSquares,
                    // }}
                    // customPieces={{
                    //     bK: ({ }) => <div> </div>,
                    //     bQ: ({ }) => <div> </div>,
                    //     bR: ({ }) => <div> </div>,
                    //     bB: ({ }) => <div> </div>,
                    //     bN: ({ }) => <div> </div>,
                    //     bP: ({ }) => <div> </div>,
                    // }}  

                    // customPieces={defaultPieces}
                />
                <Clock isWhite={true} ticking={isWhiteTurn} />
            </div>
            <Sidebar resetGame={resetGame} stepPlaybackMove={stepPlaybackMove}></Sidebar>
        </>
    );
}
