import Chess from "./Chess.js";
import {algebraic} from "./Chess.js";


let c = new Chess();

console.log(c._moves().length);

for (let move of c._moves()){
    let moveToMake = {
        from: algebraic(move.from),
        to: algebraic(move.to),
    };
    let res = c.move(moveToMake).after;
}