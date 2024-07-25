from Chessjs import validateFen, isDigit, Chess, algebraic
from numpy import inf


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
    print(all_material)

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
def minimax(position, depth, alpha, beta):

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
            print(count)
            new_c = Chess(position)
            new_position = new_c.move({'from': algebraic(move['from']), 'to': algebraic(move['to'])})['after']

            curr_score = minimax(position=new_position, depth=depth - 1)
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
            curr_score = minimax(position=new_position, depth=  depth - 1)
            min_score = min(min_score, curr_score)
            beta = min(beta, curr_score)
            if beta <= alpha:
                break
        return min_score


# print(minimax(default_fen, depth=2))


# Mental note: validateFen might be wrong
test_position = 'rnbqkbnr/pppppppp/8/8/8/8/RNBQKBNR w KQkq - 0 1'

print(eval(test_position))

# def minimax(node, depth, alpha, beta, maximizingPlayer):
#     if depth == 0 or node is a terminal node:
#         return the heuristic value of node

#     if maximizingPlayer:
#         maxEval = -infinity
#         for each child of node:
#             eval = minimax(child, depth - 1, alpha, beta, false)
#             maxEval = max(maxEval, eval)
#             alpha = max(alpha, eval)
#             if beta <= alpha:
#                 break  // beta cutoff
#         return maxEval
#     else:
#         minEval = infinity
#         for each child of node:
#             eval = minimax(child, depth - 1, alpha, beta, true)
#             minEval = min(minEval, eval)
#             beta = min(beta, eval)
#             if beta <= alpha:
#                 break  // alpha cutoff
#         return minEval

# // Initial call to the minimax function
# rootNode = ...  // the initial game state
# initialDepth = ...  // desired search depth
# initialAlpha = -infinity
# initialBeta = infinity
# isMaximizingPlayer = true  // or false, depending on which player's turn it is

# bestValue = minimax(rootNode, initialDepth, initialAlpha, initialBeta, isMaximizingPlayer)
