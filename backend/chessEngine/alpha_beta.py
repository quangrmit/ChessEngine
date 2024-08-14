from Chessjs import validateFen, isDigit, Chess, algebraic
from numpy import inf
from flask import Flask, make_response, Response
from flask import jsonify


# score = materialWeight * (numWhitePieces - numBlackPieces) * who2move 

default_fen = 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1'

material_points = {
    'r': 5,
    'n': 3,
    'b': 3,
    'q': 9,
    'p': 1,
    'k': 200,
    'R': 5,
    'N': 3,
    'B': 3,
    'Q': 9,
    'P': 1,
    'K': 200
}

def eval(fen=default_fen):

    material_weight = 0
    num_white_pieces, num_black_pieces = 0, 0
    who_to_move = 1 if fen.split()[1] == 'w' else -1
    white_material_weight = 0
    black_material_weight = 0



    if (not validateFen(fen)['ok']):
        return 
    material_s = fen.split()[0]
    all_material = ''.join(material_s.split('/'))
    # print(all_material)

    for char in all_material:
        if isDigit(char):
            continue
        if char.isupper():
            num_white_pieces += 1
            white_material_weight += material_points[char]
        else:
            num_black_pieces += 1
            black_material_weight += material_points[char]
    material_weight = abs(white_material_weight - black_material_weight)
    
    return material_weight * (num_white_pieces - num_black_pieces) * who_to_move

# Questions: how to test this


# pseudocode
def minimax_ab_pruning(position, depth, alpha, beta):

    # NEeds to check for end of game too
    if depth == 0:
        return eval(position)
    
    alpha, beta = None, None
    
    c = Chess(position)
    if position.split()[1] == 'w':
        max_score = -inf
        count = 0
        for move in c._moves():
            count += 1
            # print(count)
            new_c = Chess(position)
            new_position = new_c.move({'from': algebraic(move['from']), 'to': algebraic(move['to'])})['after']

            curr_score = minimax_ab_pruning(position=new_position, depth=depth - 1)
            max_score = max(max_score, curr_score)
            alpha = max(alpha, curr_score)
            if beta <= alpha:
                break
        return max_score

    else:
        # min
        min_score = inf
        for move in c._moves():
            new_c = Chess(position)
            new_position = new_c.move({'from': algebraic(move['from']), 'to': algebraic(move['to'])})['after']
            curr_score = minimax_ab_pruning(position=new_position, depth=  depth - 1)
            min_score = min(min_score, curr_score)
            beta = min(beta, curr_score)
            if beta <= alpha:
                break
        return min_score

best_move = None
DEPTH = 4
c = Chess(default_fen)
    
def best_move(position, depth):
    global best_move, c, DEPTH
    minimax_new(position = position, depth = DEPTH)
    return best_move

def minimax_new(position, depth):
    if depth == 0:
        obj = {
            'fen' : position,
            'eval': eval(position)
        }
        return obj
    
    if position.split()[1] == 'w':
        max_score = -inf
        for move in c._moves():

            # Make move

            new_position = c.move({'from': algebraic(move['from']), 'to': algebraic(move['to'])})['after']
            print(new_position)
            # Iterate all next move
            curr = minimax(position=new_position, depth=depth - 1)

            # Undo move
            c._undoMove()

            # Find best move
            curr_score = curr['eval']
            if (curr_score > max_score):
                max_score = curr_score
                if (depth == DEPTH):
                    best_move = {
                        'from': algebraic(move['from']),
                        'to': algebraic(move['to'])
                    }
            max_score = max(max_score, curr_score)

        return {
            'fen': position,
            'eval': max_score
        }

    else:
        min_score = inf
        for move in c._moves():
            # Make move
            new_position = c.move({'from': algebraic(move['from']), 'to': algebraic(move['to'])})['after']

            # Iterate all next move
            curr = minimax(position=new_position, depth=  depth - 1)

            # Undo move
            c._undoMove()
            
            # Find best move
            curr_score = curr['eval']
            if (curr_score < min_score):
                min_score = curr_score
                if (depth == DEPTH ):
                    best_move = {
                        'from': algebraic(move['from']),
                        'to': algebraic(move['to'])
                    }

        return {
            'fen': position,
            'eval': min_score
        }


def minimax(position, depth):

    # NEeds to check for end of game too
    if depth == 0:
        obj = {
            'fen' : position,
            'eval': eval(position)
        }
        return obj
    c = Chess(position)
    if position.split()[1] == 'w':
        max_score = -inf
        count = 0
        children = []
        for move in c._moves():
            count += 1
            # print(count)
            new_c = Chess(position)
            new_position = new_c.move({'from': algebraic(move['from']), 'to': algebraic(move['to'])})['after']

            curr = minimax(position=new_position, depth=depth - 1)
            children.append(curr)

            curr_score = curr['eval']
            max_score = max(max_score, curr_score)

        return {
            'fen': position,
            'eval': max_score,
            'children': children
        }

    else:
        # min
        min_score = inf
        children = []
        for move in c._moves():
            new_c = Chess(position)
            new_position = new_c.move({'from': algebraic(move['from']), 'to': algebraic(move['to'])})['after']
            curr = minimax(position=new_position, depth =  depth - 1)
            children.append(curr)

            curr_score = curr['eval']


            min_score = min(min_score, curr_score)

        return {
            'fen': position,
            'eval': min_score,
            'children': children
        }



# Mental note: validateFen might be wrong
# test_position = 'rnbqkbnr/pppppppp/8/8/8/8/RNBQKBNR w KQkq - 0 1'

# print(eval(test_position))

minimax_new(default_fen, 3)

app = Flask(__name__)
@app.route('/data')
def foo():
    # data = minimax(default_fen, 2)
    minimax_new(default_fen, 3)

    # response = make_response((data))
    # response.headers['Content-Type'] = 'application/json'
    # response.headers['Access-Control-Allow-Origin'] = '*'
    # return response
    return best_move

def main():   
    app.run(debug=True, port=5501)

if __name__ == '__main__':
    main()