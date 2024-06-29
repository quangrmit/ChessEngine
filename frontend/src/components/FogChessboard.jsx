import Chess from "../modules/Chess";
import { Chessboard } from "react-chessboard";
import Clock from "./Clock";
import React from "react";
import WinDialog from "./WinDialog";
import Sidebar from "./Sidebar";

import { useState } from "react";

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
        }
        catch (e) {
            // console.log(e) // Information of incorrect move
            console.log("No more move");
            return null;
        }  
    }

    function onDrop(sourceSquare, targetSquare, piece) {
      const move = makeAMove({
        from: sourceSquare,
        to: targetSquare,

      });
      // illegal move
      if (move === null) return false;

      // legal move
      setFen(game.fen());
      setMoveCount(moveCount+1)
      setIsWhiteTurn(prev => !prev) // switch clock

      // Check if a king has been captured
      if (!move.captured) return true;
      if (move.captured !== 'k') return true;
      if (move.color === 'b') setMessage(`Black win in ${moveCount} moves`)
      else setMessage(`White win in ${moveCount} moves`)
      setOpen(true)
      setControlGame(false)  
      return true;
    }
  
    return <>
        <WinDialog open={open} resetGame={resetGame} cancleGame={cancelGame} message={message} ></WinDialog>
        <div id = "containerBoard">
            <Clock isWhite={false} ticking={!isWhiteTurn} />
                <Chessboard position={fen} onPieceDrop={onDrop}/>;
            <Clock isWhite={true} ticking={isWhiteTurn}/>
        </div>
        <Sidebar resetGame={resetGame} stepPlaybackMove={stepPlaybackMove}></Sidebar>
    </>
    
};
