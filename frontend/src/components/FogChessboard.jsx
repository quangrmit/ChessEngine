import Chess from "../modules/Chess";
import { Chessboard } from "react-chessboard";
import Clock from "./Clock";
import React from "react";
import WinDialog from "./WinDialog";

import { useState } from "react";

function FogChessboard() {
    const [fen, setFen] = useState("start");
    const [game, setGame] = useState(new Chess());
    const [isWhiteTurn, setIsWhiteTurn] = useState(true);
    const [open, setOpen] = useState(false)
    const [message, setMessage] = useState("")
    const [moveCount, setMoveCount] = useState(0)
    
    // Manage win dialog state
    const resetGame = () => {
        setOpen(false)
        game.reset()
        setFen(game.fen())
    }

    const cancelGame = () => {
        setOpen(false)
    }

    function makeAMove(move) {
        try {
            const result = game.move(move)
            console.log(moveCount);
            return result;
        }
        catch (e) {
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
      setMoveCount(moveCount+1)
      setIsWhiteTurn(prev => !prev) // switch clock

      // Check if the King has been captured for restarting the game
      let gameState = game.fen().split(" ")[0]
      // If k (Black King) not on the game field, white wins
      if (!gameState.includes('k')) {
          setMessage(`White win in ${moveCount} moves`)
          setOpen(true)
      }
      // If K (White King) not on the game field, black wins
      else if (!gameState.includes('K')) {
          setMessage(`Black win in ${moveCount} moves`)
          setOpen(true)
      } 
      
      return true;
    }
  
    return <>
        <WinDialog open={open} resetGame={resetGame} cancleGame={cancelGame} message={message} ></WinDialog>
        <div id = "containerBoard">
            <Clock isWhite={false} ticking={!isWhiteTurn}/>
                <Chessboard position={fen} onPieceDrop={onDrop}/>;
            <Clock isWhite={true} ticking={isWhiteTurn}/>
        </div>
    </>
    
};
