import re
from operator import attrgetter

Chess = validateFen = None
WHITE = 'w'
BLACK = 'b'
PAWN = 'p'
KNIGHT = 'n'
BISHOP = 'b'
ROOK = 'r'
QUEEN = 'q'
KING = 'k'
DEFAULT_POSITION = 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1'
EMPTY = -1
FLAGS = {
    'NORMAL': 'n',
    'CAPTURE': 'c',
    'BIG_PAWN': 'b',
    'EP_CAPTURE': 'e',
    'PROMOTION': 'p',
    'KSIDE_CASTLE': 'k',
    'QSIDE_CASTLE': 'q',
}

SQUARES = [
    'a8', 'b8', 'c8', 'd8', 'e8', 'f8', 'g8', 'h8',
    'a7', 'b7', 'c7', 'd7', 'e7', 'f7', 'g7', 'h7',
    'a6', 'b6', 'c6', 'd6', 'e6', 'f6', 'g6', 'h6',
    'a5', 'b5', 'c5', 'd5', 'e5', 'f5', 'g5', 'h5',
    'a4', 'b4', 'c4', 'd4', 'e4', 'f4', 'g4', 'h4',
    'a3', 'b3', 'c3', 'd3', 'e3', 'f3', 'g3', 'h3',
    'a2', 'b2', 'c2', 'd2', 'e2', 'f2', 'g2', 'h2',
    'a1', 'b1', 'c1', 'd1', 'e1', 'f1', 'g1', 'h1'
]

BITS = {
    'NORMAL': 1,
    'CAPTURE': 2,
    'BIG_PAWN': 4,
    'EP_CAPTURE': 8,
    'PROMOTION': 16,
    'KSIDE_CASTLE': 32,
    'QSIDE_CASTLE': 64,
}

Ox88 = {
    'a8': 0, 'b8': 1, 'c8': 2, 'd8': 3, 'e8': 4, 'f8': 5, 'g8': 6, 'h8': 7,
    'a7': 16, 'b7': 17, 'c7': 18, 'd7': 19, 'e7': 20, 'f7': 21, 'g7': 22, 'h7': 23,
    'a6': 32, 'b6': 33, 'c6': 34, 'd6': 35, 'e6': 36, 'f6': 37, 'g6': 38, 'h6': 39,
    'a5': 48, 'b5': 49, 'c5': 50, 'd5': 51, 'e5': 52, 'f5': 53, 'g5': 54, 'h5': 55,
    'a4': 64, 'b4': 65, 'c4': 66, 'd4': 67, 'e4': 68, 'f4': 69, 'g4': 70, 'h4': 71,
    'a3': 80, 'b3': 81, 'c3': 82, 'd3': 83, 'e3': 84, 'f3': 85, 'g3': 86, 'h3': 87,
    'a2': 96, 'b2': 97, 'c2': 98, 'd2': 99, 'e2': 100, 'f2': 101, 'g2': 102, 'h2': 103,
    'a1': 112, 'b1': 113, 'c1': 114, 'd1': 115, 'e1': 116, 'f1': 117, 'g1': 118, 'h1': 119
}

PAWN_OFFSETS = {
    'b': [16, 32, 17, 15],
    'w': [-16, -32, -17, -15],
}

PIECE_OFFSETS = {
    'n': [-18, -33, -31, -14, 18, 33, 31, 14],
    'b': [-17, -15, 17, 15],
    'r': [-16, 1, 16, -1],
    'q': [-17, -16, -15, 1, 17, 16, 15, -1],
    'k': [-17, -16, -15, 1, 17, 16, 15, -1],
}

ATTACKS = [
    20, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 20, 0,
    0, 20, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 20, 0, 0,
    0, 0, 20, 0, 0, 0, 0, 24, 0, 0, 0, 0, 20, 0, 0, 0,
    0, 0, 0, 20, 0, 0, 0, 24, 0, 0, 0, 20, 0, 0, 0, 0,
    0, 0, 0, 0, 20, 0, 0, 24, 0, 0, 20, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 20, 2, 24, 2, 20, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 2, 53, 56, 53, 2, 0, 0, 0, 0, 0, 0,
    24, 24, 24, 24, 24, 24, 56, 0, 56, 24, 24, 24, 24, 24, 24, 0,
    0, 0, 0, 0, 0, 2, 53, 56, 53, 2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 20, 2, 24, 2, 20, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 20, 0, 0, 24, 0, 0, 20, 0, 0, 0, 0, 0,
    0, 0, 0, 20, 0, 0, 0, 24, 0, 0, 0, 20, 0, 0, 0, 0,
    0, 0, 20, 0, 0, 0, 0, 24, 0, 0, 0, 0, 20, 0, 0, 0,
    0, 20, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 20, 0, 0,
    20, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 20
]

RAYS = [
    17, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 15, 0,
    0, 17, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 15, 0, 0,
    0, 0, 17, 0, 0, 0, 0, 16, 0, 0, 0, 0, 15, 0, 0, 0,
    0, 0, 0, 17, 0, 0, 0, 16, 0, 0, 0, 15, 0, 0, 0, 0,
    0, 0, 0, 0, 17, 0, 0, 16, 0, 0, 15, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 17, 0, 16, 0, 15, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 17, 16, 15, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 0, -1, -1, -1, -1, -1, -1, -1, 0,
    0, 0, 0, 0, 0, 0, -15, -16, -17, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -15, 0, -16, 0, -17, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -15, 0, 0, -16, 0, 0, -17, 0, 0, 0, 0, 0,
    0, 0, 0, -15, 0, 0, 0, -16, 0, 0, 0, -17, 0, 0, 0, 0,
    0, 0, -15, 0, 0, 0, 0, -16, 0, 0, 0, 0, -17, 0, 0, 0,
    0, -15, 0, 0, 0, 0, 0, -16, 0, 0, 0, 0, 0, -17, 0, 0,
    -15, 0, 0, 0, 0, 0, 0, -16, 0, 0, 0, 0, 0, 0, -17
]


PIECE_MASKS = {
    'p': 0x1, 'n': 0x2, 'b': 0x4, 'r': 0x8, 'q': 0x10, 'k': 0x20
}

SYMBOLS = 'pnbrqkPNBRQK'
PROMOTIONS = [KNIGHT, BISHOP, ROOK, QUEEN]
RANK_1 = 7
RANK_2 = 6
RANK_7 = 1
RANK_8 = 0

SIDES = {
    KING: BITS['KSIDE_CASTLE'],
    QUEEN: BITS['QSIDE_CASTLE'],
}

ROOKS = {
    'w': [
    { 'square': Ox88['a1'], 'flag': BITS['QSIDE_CASTLE'] },
    { 'square': Ox88['h1'], 'flag': BITS['KSIDE_CASTLE'] },
  ],
  'b': [
    { 'square': Ox88['a8'], 'flag': BITS['QSIDE_CASTLE'] },
    { 'square': Ox88['h8'], 'flag': BITS['KSIDE_CASTLE'] },
  ],
}

SECOND_RANK = {
    'b': RANK_7,
    'w': RANK_2
}

TERMINATION_MARKERS = ['1-0', '0-1', '1/2-1/2', '*']

def rank(square):
    return square >> 4

def file(square):
    return square & 0xf

def isDigit(c):
    digits = '0123456789'
    return c in digits

def algebraic(square):
    f = file(square)
    r = rank(square)
    col = "abcdefgh"
    row = "87654321"
    return col[f:f+1] + row[r:r+1]

def swapColor(color):
    if (color == WHITE):
        return BLACK
    return WHITE

## Validate fen notation, if not success will throw an errorr
def validateFen(fen):
    tokens = fen.split()
    if (len(tokens) != 6):
        return {
            'ok': False,
            'error': 'Invalid FEN: must contain six space-delimited fields'
        }
    
    moveNumber = tokens[5]
    if (moveNumber.isdigit() is False):
        return {
            'ok': False,
            'error': 'Invalid FEN: move number must be a positive integer'
        }
    elif (int(moveNumber) <= 0):
        return {
            'ok': False,
            'error': 'Invalid FEN: move number must be a positive integer'
        }
    
    moveNumber = int(moveNumber)

    halfMoves = tokens[4]
    if (halfMoves.isdigit() is False):
        return {
            'ok': False,
            'error': 'Invalid FEN: half move counter number must be non-negative'
        }
    elif (int(halfMoves) < 0):
        return {
            'ok': False,
            'error': 'Invalid FEN: half move counter number must be non-negative'
        }
    halfMoves = int(halfMoves)

    return {'ok': True}

def getDisambiguator(move, moves):
    From = move['from']
    to = move['to']
    piece = move['piece']
    ambiguities = sameRank = sameFile = 0
    for i in range(len(moves)):
        ambigFrom = moves[i]['from']
        ambigTo = moves[i]['to']
        ambigPiece = moves[i]['piece']

        if (piece == ambigPiece and From != ambigPiece and to == ambigTo):
            ambiguities += 1
            if (rank(From) == rank(ambigFrom)):
                sameRank += 1
            if (file(From) == file(ambigFrom)):
                sameFile += 1
    if (ambiguities > 0):
        if (sameRank > 0 and sameFile > 0):
            return algebraic(From)
        elif (sameFile > 0):
            return algebraic(From)[1]
        else:
            return algebraic(From)[0]    
    return ''

def addMove(moves, color, From, to, piece, captured = None, flags = BITS['NORMAL']):
    r = rank(to)
    if (piece == PAWN and (r == RANK_1 or r == RANK_8)):
        for i in range(len(PROMOTIONS)):
            promotion = PROMOTIONS[i]
            move = {
                'color': color,
                'from': From,
                'to': to,
                'piece': piece,
                'captured': captured,
                'promotion': promotion,
                'flags': flags | BITS['PROMOTION']
            }
            moves.append(move)
    else: 
        move = {
            'color': color,
            'from': From,
            'to': to,
            'piece': piece,
            'captured': captured,
            'flags': flags
        }
        moves.append(move)

def inferPieceType(san):
    pieceType = san[0]
    if (pieceType >= 'a' and pieceType <= 'h'):
        matches = re.search('[a-h]\d.*[a-h]\d', san)
        if (matches != ''):
            return None
        return PAWN
    pieceType = pieceType.lower()
    if (pieceType == 'o'):
        return KING
    return pieceType


def strippedSan(move):
    move = re.sub('=', '', move)
    return re.sub('[+#]?[?!]*$', '')

def trimFen(fen):
    return ' '.join(fen.split(' ')[0:4])

class Chess:

    def __init__(self, fen = DEFAULT_POSITION) -> None:
        self._board = {}
        self._turn = WHITE
        self._header = {}
        self._kings = {'w': EMPTY, 'b': EMPTY}
        self._epSquare = -1
        self._halfMoves = 0
        self._moveNumber = 0
        self._history = [];
        self._comments = {};
        self._castling = { 'w': 0, 'b': 0 }
        self._positionCount = {}
        self.load(fen)

    def clear(self, config = { 'preserveHeaders': False }):
        self._board = {}
        self._kings = {'w': EMPTY, 'b': EMPTY}
        self._turn = WHITE
        self._castling = { 'w': 0, 'b': 0 }
        self._epSquare = EMPTY
        self._halfMoves = 0
        self._moveNumber = 1
        self._history = [];
        self._comments = {};
        self._positionCount = {}
        if (config['preserveHeaders'] is False):
            self._header = {}
            return
        else:
            del self._header['SetUp']
            del self._header['FEN']

    def removeHeader(self, key):
        if key in self._header:
            del self._header[key]

    def load(self, fen, config = { 'skipValidation' : False, 'preserveHeaders': False }):
        tokens = fen.split()
        if (len(tokens) >= 2 and len(tokens) < 6):
            adjustment = ['-', '-', '0', '1']
            fen = tokens + adjustment[-6-(len(tokens))]
            fen = " ".join(fen)
        tokens = fen.split();
        if (config['skipValidation'] is False):
            validation = validateFen(fen)
            if (validation['ok'] is False):
                raise Exception(validation['error'])
        
        position = tokens[0]
        square = 0
        self.clear(config = { 'preserveHeaders': config['preserveHeaders'] })
        for i in range(len(position)):
            piece = position[i]
            if piece == '/':
                square += 8
            elif (isDigit(piece) is True):
                square += int(piece)
            else:
                color = BLACK
                if (piece < 'a'):
                    color = WHITE
                newConfig = {
                    'type': piece.lower(),
                    'color': color
                }
                self._put(newConfig, algebraic(square))
                square += 1
        self._turn = tokens[1]
        if ('K' in tokens[2]):
            self._castling['w'] = BITS['KSIDE_CASTLE'] | self._castling['w']
        if ('Q' in tokens[2]):
            self._castling['w'] = BITS['QSIDE_CASTLE'] | self._castling['w']
        if ('k' in tokens[2]):
            self._castling['b'] = BITS['KSIDE_CASTLE'] | self._castling['b']
        if ('q' in tokens[2]):
            self._castling['b'] = BITS['QSIDE_CASTLE'] | self._castling['b']
        if (tokens[3] == '-'):
            self._epSquare = EMPTY
        else:
            self._epSquare = Ox88[tokens[3]]
        self._halfMoves = int(tokens[4])
        self._moveNumber = int(tokens[5])
        self._updateSetup(fen)
        self._incPositionCount(fen)
                
    def fen(self):
        empty = 0
        fen = ''
        i = 0
        while i <= Ox88['h1']:
            if (i in self._board):
                if (empty > 0):
                    fen += str(empty)
                    empty = 0
                data = self._board[i]
                if (data['color'] == WHITE):
                    fen += str(data['type']).upper()
                else:
                    fen += str(data['type']).lower()
            else:
                empty += 1
            
            if ((i + 1) & 0x88): 
                if (empty > 0):
                    fen += str(empty)
                if (i != Ox88['h1']):
                    fen += '/'
                empty = 0
                i += 9
                continue
            i += 1
            
        castling = ''
        if (self._castling[WHITE] & BITS['KSIDE_CASTLE']):
            castling += 'K'
        if (self._castling[WHITE] & BITS['QSIDE_CASTLE']):
            castling += 'Q'
        if (self._castling[BLACK] & BITS['KSIDE_CASTLE']):
            castling += 'k'
        if (self._castling[BLACK] & BITS['QSIDE_CASTLE']):
            castling += 'q'
        
        castling = castling or '-'
        eqSquare = '-'

        if (self._epSquare != EMPTY):
            bigPawnSquare = self._epSquare + (16 if ''.join(self._turn) == WHITE else -16)
            squares = [bigPawnSquare + 1, bigPawnSquare - 1]
            for square in squares:
                if (square & 0x88):
                    continue
                color = ''.join(self._turn)
                if (square in self._board):
                    if (self._board[square]['color'] == color and self._board[square]['type'] == PAWN):
                        move = {
                            'color': color,
                            'from': square,
                            'to': self._epSquare,
                            'piece': PAWN,
                            'captured': PAWN,
                            'flags': BITS['EP_CAPTURE']
                        }
                        self._makeMove(move)
                        isLegal = not self._isKingAttacked(color)
                        self._undoMove()
                        if (isLegal):
                            eqSquare = algebraic(self._epSquare)
        return " ".join([ fen, ''.join(self._turn), castling, eqSquare, str(self._halfMoves), str(self._moveNumber) ])
    
    def _updateSetup(self, fen):
        if (len(self._history) > 0): return
        if (fen != DEFAULT_POSITION):
            self._header['SetUp'] = '1'
            self._header['FEN'] = fen
        else:
            if ('SetUp' in self._header):
                del self._header['Setup']
            if ('FEN' in self._header):
                del self._header['FEN']

    def reset(self):
        self.load(DEFAULT_POSITION)
    
    def get(self, square):
        return self._board[Ox88[square]] or False

    def put(self, config, square):
        if (self._put(config, square) is True):
            self._updateCastlingRights()
            self._updateEnPassantSquare()
            self._updateSetup(self.fen())
            return True
        return False

    def _put(self, config, square):
        if (config['type'] not in SYMBOLS):
            return False
        if (square not in Ox88):
            return False
        sq = Ox88[square]
        if (config['type'] == KING and not(self._kings[config['color']] == EMPTY or self._kings[config['color']] == sq)):
            return False
        currentPieceOnSquare = {}
        if (sq in self._board):
            currentPieceOnSquare = self._board[sq]
        if (currentPieceOnSquare and currentPieceOnSquare['type'] == KING):
            self._kings[currentPieceOnSquare['color']] = EMPTY
        self._board[sq] = {
            'type': config['type'],
            'color': config['color']
        }
        if (config['type'] == KING):
            self._kings[config['color']] = sq
        return True

    def remove(self, square):
        piece = self.get(square)
        del self._board[Ox88[square]]
        if (piece and piece['type'] == KING):
            self._kings[piece['color']] = EMPTY
        self._updateCastlingRights()
        self._updateEnPassantSquare()
        self._updateSetup(self.fen())
        return piece

    def _updateCastlingRights(self):
        whiteKingInPlace = None
        if (Ox88['e1'] in self._board):
            whiteKingInPlace = self._board[Ox88['e1']]['type'] == KING and self._board[Ox88['e1']]['color'] == WHITE
        
        blackKingInPlace = None
        if (Ox88['e8'] in self._board):
            blackKingInPlace = self._board[Ox88['e8']]['type'] == KING and self._board[Ox88['e8']]['color'] == WHITE

        if (not whiteKingInPlace):
            pass

    def _updateEnPassantSquare():
        pass

    def _attacked(color, square):
        pass

    def _isKingAttacked(color):
        return False
    
    def isAttacked(self, square, attackedBy):
        return self._attacked(attackedBy, Ox88[square])

    def isCheck(self):
        return False
    
    def inCheck(self):
        return self.isCheck()
    
    def isCheckmate(self):
        return self.isCheck() and len(self._moves()) == 0

    def isStalemate(self):
        return self.isCheck() is False and len(self._moves()) == 0

    def isInsufficientMaterial(self):
        pieces = {
            'b': 0,
            'n': 0,
            'r': 0,
            'q': 0,
            'k': 0,
            'p': 0
        }
        bishops = []
        numPieces = 0
        squareColor = 0
        for i in range(128):
            squareColor = (squareColor + 1) % 2
            ## Problem 
            if (i & 0x88): 
                i += 7
                continue
            piece = self._board[i]
            if (piece):
                pieces[piece['type']] = (pieces[piece['type']] + 1 if piece['type'] in pieces else 1)
                if (piece['type'] == BISHOP):
                    bishops.append(squareColor)
                numPieces += 1
        
        if (numPieces == 2):
            return True
        elif (numPieces == 3 and (pieces[BISHOP] == 1 or pieces[KNIGHT] == 1)):
            return True
        elif (numPieces == pieces[BISHOP] + 2):
            Sum = 0
            Len = len(bishops)
            for i in range(Len):
                Sum += bishops[i]
            if (Sum == 0 or Sum == Len):
                return True
        return False
    
    def isThreefoldRepetition(self):
        return self._getPositionCount(self.fen()) >= 3

    def isDraw(self):
        return self._halfMoves >= 100 or self.isStalemate() or self.isInsufficientMaterial() or self.isThreefoldRepetition()

    def isGameOver(self):
        return self.isCheckmate() or self.isStalemate() or self.isDraw()

    def moves(self, config):
        moves = self._moves({ 'square': config['square'], 'piece': config['piece'] })
        if (config['verbose'] is True):
            for i in range(len(moves)):
                moves[i] = self._makePretty(moves[i])
        else:
            for i in range(len(moves)):
                moves[i] = self._moveToSan(moves[i], moves)
        return moves

    def _moves(self, config = { 'legal': True, 'piece': None, 'square': None }):
        forSquare = None
        if ('square' in config):
            if (config['square'] != None):
                forSquare = config['square'].lower()
        forPiece = None 
        if ('piece' in config):
            if (config['piece'] != None):
                forPiece = config['piece'].lower()

        moves = []
        us = ''.join(self._turn)
        them = swapColor(us)
        firstSquare = Ox88['a8']
        lastSquare = Ox88['h1']
        singleSquare = False

        if (forSquare):
            if (not (forSquare in Ox88)):
                return []
            else:
                firstSquare = lastSquare = Ox88[forSquare]
                singleSquare = True

        for i in range(firstSquare, lastSquare + 1):
            if (i not in self._board):
                continue

            if (i & 0x88):
                i += 7
                continue

            if (not self._board[i] or self._board[i]['color'] == them):
                continue

            Type = self._board[i]['type']
            to = None
            if (Type == PAWN):
                if (forPiece and forPiece != Type):
                    continue
                to = i + PAWN_OFFSETS[''.join(us)][0]
                if (to not in self._board):
                    addMove(moves, us, i, to, PAWN)

                    to = i + PAWN_OFFSETS[''.join(us)][1]
                    if (SECOND_RANK[''.join(us)] == rank(i) and to not in self._board):
                        addMove(moves, us, i, to, PAWN, None, BITS['BIG_PAWN'])

                for j in range(2,4):
                    to = i + PAWN_OFFSETS[''.join(us)][j]
                    if (to & 0x88):
                        continue
                    if (to in self._board):
                        if ('color' in self._board[to]):
                            if (self._board[to]['color'] == them):
                                addMove(moves, us, i, to, PAWN, self._board[to]['type'], BITS['CAPTURE']) 
                    elif (to == self._epSquare):
                        addMove(moves, us, i, to, PAWN, PAWN, BITS['EP_CAPTURE']) 
            else:
                if (forPiece and forPiece != Type):
                    continue
                for j in range(len(PIECE_OFFSETS[Type])):
                    offset = PIECE_OFFSETS[Type][j]
                    to = i
                    while (True):
                        to += offset
                        if (to & 0x88): break
                        if (to not in self._board):
                            addMove(moves, us, i, to, Type)
                        else:
                            if (to in self._board):
                                if ('color' in self._board[to]):
                                    if (self._board[to]['color'] == us): break
                                    addMove(moves, us, i, to, Type, self._board[to]['type'], BITS['CAPTURE'])
                                    break
                        if (Type == KNIGHT or Type == KING):
                            break
        if (forPiece == None or forPiece == KING):
            if (not singleSquare or lastSquare == self._kings[''.join(us)]):
                if (self._castling[''.join(us)] & BITS['KSIDE_CASTLE']):
                    castlingFrom = self._kings[''.join(us)]
                    castlingTo = castlingFrom + 2
                    if (
                        not self._board[castlingFrom + 1] and 
                        not self._board[castlingTo] and 
                        not self._attacked(them, self._kings[''.join(us)]) and
                        not self._attacked(them, castlingFrom + 1) and
                        not self._attacked(them, castlingTo) 
                    ):
                        addMove(moves, us, self._kings[''.join(us)], castlingTo, KING, None, BITS['KSIDE_CASTLE'])
                if (self._castling[''.join(us)] & BITS['QSIDE_CASTLE']):
                    castlingFrom = self._kings[''.join(us)]
                    castlingTo = castlingFrom - 2
                    if(
                        not self._board[castlingFrom - 1] and 
                        not self._board[castlingFrom - 2] and
                        not self._board[castlingFrom - 3] and
                        not self._attacked(them, self._kings[''.join(us)]) and
                        not self._attacked(them, castlingFrom - 1) and
                        not self._attacked(them, castlingTo)
                    ):
                        addMove(moves, us, self._kings[''.join(us)], castlingTo, KING, None, BITS['QSIDE_CASTLE'])
        if (not config['legal'] or self._kings[''.join(us)] == -1):
            return moves
        
        legalMoves = []
        for i in range(len(moves)):
            self._makeMove(moves[i])
            legalMoves.append(moves[i])
            self._undoMove()
                    
        return legalMoves

    def move(self, move, config = {}):
        if 'strict' not in config:
            config['strict'] = False

        moveObj = {}
        strict = config['strict']
        
        if (type(move) == str):
            moveObj = self._moveFromSan(move, strict)
        elif (type(move) == dict):
            moves = self._moves()
            # convert the pretty moves object to an ugly move obj
            for i in range(len(moves)):
                if (
                    move['from'] == algebraic(moves[i]['from']) and
                    move['to'] == algebraic(moves[i]['to']) and 
                    (('promotion' not in moves[i]) or move['promotion'] == moves[i]['promotion'])
                    ):
                    moveObj = moves[i]
                    break
        # failed to find move
        
        if not moveObj:
            if type(move) == str:
                raise Exception(f"Invalid move: {move}")
            else:
                raise Exception(f"Invalid move: {move}")
        
        prettyMove = self._makePretty(moveObj)
        self._makeMove(moveObj)
        self._incPositionCount(prettyMove['after'])
        return prettyMove
                    

    def _push(self, move):
        self._history.append({
            'move': move,
            'kings': {'b': self._kings['b'], 'w': self._kings['w']},
            'turn': ''.join(self._turn),
            'castling': { 'b': self._castling['b'], 'w': self._castling['w'] },
            'epSquare': self._epSquare,
            'halfMoves': self._halfMoves,
            'moveNumber': self._moveNumber
        })

    def _makeMove(self, move):
        us = ''.join(self._turn)
        them = swapColor(us),
        self._push(move)
        if (move['from'] in self._board):
            self._board[move['to']] = self._board[move['from']]
            del self._board[move['from']]
        if (move['flags'] & BITS['EP_CAPTURE']):
            if (''.join(self._turn) == BLACK):
                del self._board[move['to'] - 16]
            else:
                del self._board[move['to'] + 16]
        if ('promotion' in move):
            self._board[move['to']] = {
                'type': move['promotion'],
                'color': us
            }
        if (move['to'] in self._board):
              if (self._board[move['to']]['type'] == KING):
                self._kings[''.join(us)] = move['to']
                if (move['flags'] & BITS['KSIDE_CASTLE']):
                    castlingTo = move['to'] - 1
                    castlingFrom = move['to'] + 1
                    self._board[castlingTo] = self._board[castlingFrom]
                    del self._board[castlingFrom]
                elif (move['flags'] & BITS['QSIDE_CASTLE']):
                    castlingTo = move['to'] + 1
                    castlingFrom = move['to'] - 2
                    self._board[castlingTo] = self._board[castlingFrom]
                    del self._board[castlingFrom]
                self._castling[''.join(us)] = 0

        if (us in self._castling):
            for i in range(len(ROOKS[''.join(us)])):
                if (move['from'] == ROOKS[''.join(us)][i]['square'] and (self._castling[''.join(us)] & ROOKS[''.join(us)][i]['flag'])):
                    self._castling[''.join(us)] = self._castling[''.join(us)] ^ ROOKS[''.join(us)][i]['flag']
                    break  
        
        if (them in self._castling):
            for i in range(len(ROOKS[them])):
                if (move['to'] == ROOKS[them][i]['square'] and self._castling[them] & ROOKS[them][i]['flag']):
                    self._castling[them] = self._castling[them][i]['flag']
        
        if (move['flags'] & BITS['BIG_PAWN']):
            if (''.join(us) == BLACK):
                self._epSquare = move['to'] - 16
            else:
                self._epSquare = move['to'] + 16
        else:
            self._epSquare = EMPTY
            
        if (move['piece'] == PAWN):
            self._halfMoves = 0
        elif (move['flags'] & (BITS['CAPTURE'] | BITS['EP_CAPTURE'])):
            self._halfMoves = 0
        else:
            self._halfMoves += 1
        
        if (''.join(us) == BLACK):
            self._moveNumber += 1
        self._turn = ''.join(them)
        
    def undo(self):
        move = self._undoMove()
        if move:
            prettyMove = self._makePretty(move)
            self._decPositionCount(prettyMove['after'])
            return prettyMove
        return None

    def _undoMove(self):
        old = self._history.pop()
        if (old == None):
            return None
        
        move = old['move']
        self._kings = old['kings']
        self._turn = old['turn']
        self._castling = old['castling']
        self._epSquare = old['epSquare']
        self._halfMoves = old['halfMoves']
        self._moveNumber = old['moveNumber']
        us = ''.join(self._turn)
        them = swapColor(us)
        if (move['to'] in self._board):
            self._board[move['from']] = self._board[move['to']]
            self._board[move['from']]['type'] = move['piece'] # to undo any promotions
            del self._board[move['to']]    
        if move['captured']:
            if move['flags'] & BITS['EP_CAPTURE']:
                index = None
                if ''.join(us) == BLACK:
                    index = move['to'] - 16
                else:
                    index = move['to'] + 16
                self._board[index] = {'type': PAWN, 'color': them}
            
            else:
                self._board[move['to']] = {'type': move['captured'], 'color': them}
        
        if (move['flags'] & (BITS['KSIDE_CASTLE'] | BITS['QSIDE_CASTLE']) ):
            castlingTo, castlingFrom = None, None
            if (move['flags'] & BITS['KSIDE_CASTLE']):
                castlingTo = move['to'] + 1
                castlingFrom = move['to'] - 1
            else:
                castlingTo = move['to'] - 2
                castlingFrom = move['to'] + 1
            self._board[castlingTo] = self._board[castlingFrom]
            del self._board[castlingFrom]
        
        return move


    def pgn(config):
    # ko can
        pass

    def header(self, *args):
        for i in range(0, len(args), 2):
            if (isinstance(args[i], str) and isinstance(args[i+1], str)):
                self._header[args[i]] = args[i+1]
        
        return self._header

    def loadPgn(pgn, config):
        pass

    def _moveToSan(self, move, moves):
        output = ''
        if (move['flags'] & BITS['KSIDE_CASTLE']):
            output = 'O-O'
        elif (move['flags'] & BITS['QSIDE_CASTLE']):
            output = 'O-O-O'
        else:
            if (move['piece'] != PAWN):
                disambiguator = getDisambiguator(move, moves)
                output += move['piece'].upper() + disambiguator
            if (move['flags'] & (BITS['CAPTURE'] | BITS['EP_CAPTURE'])):
                if (move['piece'] == PAWN):
                    output += algebraic(move['from'])[0]
                output += 'x'
            output += algebraic(move['to'])
            if ('promotion' in move):
                output += '=' + move['promotion'].upper()
        self._makeMove(move)
        if (self.isCheck()):
            if (self.isCheckmate()):
                output += '#'
            else:
                output += '+'
        self._undoMove()
        return output
                

    def _moveFromSan(self, move, strict = False):
        cleanMove = strippedSan(move)
        pieceType = inferPieceType(cleanMove)
        moves = self._moves({
            'legal': True,
            'piece': pieceType,
        })

        for i in range(len(moves)):
            if (cleanMove == strippedSan(self._moveToSan(moves[i], moves))):
                return moves[i]
        
        if (strict):
            return None

        piece = matches = From = to = promotion = None

        overlyDisambiguated = False
        matches = re.search('([pnbrqkPNBRQK])?([a-h][1-8])x?-?([a-h][1-8])([qrbnQRBN])?', cleanMove)
        if (matches != ''):
            piece = matches[1]
            From = matches[2]
            to = matches[3]
            promotion = matches[4]
            if (len(From) == 1):
                overlyDisambiguated = True
        else:
            matches = re.search('([pnbrqkPNBRQK])?([a-h]?[1-8]?)x?-?([a-h][1-8])([qrbnQRBN])?', cleanMove)
            if (matches != ''):
                piece = matches[1]
                From = matches[2]
                to = matches[3]
                promotion = matches[4]
                if (len(From) == 1):
                    overlyDisambiguated = True
        pieceType = inferPieceType(cleanMove)
        moves = self._moves({
            'legal': True,
            'piece': piece if piece != None else pieceType
        })
        if (not to):
            return None

        for i in range(len(moves)):
            if (not From):
                if (cleanMove == strippedSan(self._moveToSan(moves[i], moves)).replace('x', '')):
                    return moves[i]
                elif (
                    (not piece or piece.lower() == moves[i]['piece']) and
                    Ox88['from'] == moves[i]['from'] and
                    Ox88['to'] == moves[i]['to'] and
                    (not promotion or promotion.lower() == moves[i]['promotion'])
                ):
                    return moves[i]
                elif (overlyDisambiguated):
                    square = algebraic(moves[i]['from'])
                    if (
                        (not piece or piece.lower() == moves[i]['piece']) and
                        Ox88['to'] == moves[i]['to'] and
                        (From == square[0] or From == square[1]) and
                        (not promotion or promotion.lower() == moves[i]['promotion'])
                    ):
                        return moves[i]
        return None        
                

    def ascii():
    # ko can
        pass

    def perft(depth):
    # ko can 
        pass

    def _makePretty(self, uglyMove):
        # color, piece, From, to, flags, captured, promotion = attrgetter('color', 'piece', 'from', 'to', 'flags', 'captured', 'promotion')(uglyMove)
        color = uglyMove['color']
        piece = uglyMove['piece']
        flags = uglyMove['flags']
        From = uglyMove['from']
        to = uglyMove['to']
        captured = uglyMove['captured']
        promotion = None
        if ('promotion' in uglyMove):
            promotion = uglyMove['promotion']

        prettyFlags = ""

        for flag in BITS:
            if BITS[flag] & flags:
                prettyFlags += FLAGS[flag]

        fromAlgebraic = algebraic(From)
        toAlgebraic = algebraic(to)

        move = {
            'color': color,
            'piece': piece,
            'from': From,
            'to': toAlgebraic,
            'san': self._moveToSan(uglyMove, self._moves({'legal': True})),
            'flags': prettyFlags,
            'lan': fromAlgebraic + toAlgebraic,
            'before': self.fen(),
            'after': "",
        }

        self._makeMove(uglyMove)
        move['after'] = self.fen()
        self._undoMove()

        if captured:
            move['capture'] = captured
        
        if promotion:
            move['promotion'] = promotion
            move['lan'] += promotion

        return move

    def turn(self):
        return ''.join(self._turn)
    
    def board(self):
        output = []
        row = []
        for i in range(129):
            if (i not in self._board):
                row.append(None)
            else:
                row.append({
                    'square': algebraic(i),
                    'type': self._board[i]['type'],
                    'color': self._board[i]['color']
                })    
            if ((i + 1) & 0x88):
                output.append(row)
                row = []
                i += 8

        return output    
        

    def squareColor(square):
    # ko can
        pass

    def history(config):
    # ko can
        pass

    def _getPositionCount(self, fen):
        trimmedFen = trimFen(fen)
        return self._positionCount[trimmedFen] or 0

    def _incPositionCount(self, fen):
        trimmedFen = trimFen(fen)
        if (trimmedFen not in self._positionCount):
            self._positionCount[trimmedFen] = 0
        self._positionCount[trimmedFen] += 1

    def _decPositionCount(self, fen):
        trimmedFen = trimFen(fen)
        if (self._positionCount[trimmedFen] == 1):
            del self._positionCount[trimmedFen]
        else:
            self._positionCount[trimmedFen] -= 1

    def _pruneComments():
    # ko can
        pass

    def getComment():
    # ko can
        pass

    def setComment(comment):
    # ko can
        pass

    def deleteComment():
    # ko can
        pass

    def getComments(self):
    # ko can
        pass

    def deleteComments():
    # ko can
        pass

    def setCastlingRights(self, color, rights):
        for side in [KING, QUEEN]:
            if (rights[side] != None):
                if (rights[side]):
                    self._castling[color] = self._castling[color] | SIDES[side]
                else:
                    self._castling[color] = self._castling[color] & ~SIDES[side]

        self._updateCastlingRights()
        result = self.getCastlingRights(color)
        return (
            (rights[KING] == None or rights[KING] == result[KING]) and
            (rights[QUEEN] == None or rights[QUEEN] == result[QUEEN])
        )

    def getCastlingRights(self, color):
        return {
            [KING]: (self._castling[color] & SIDES[KING]) != 0,
            [QUEEN]: (self._castling[color] & SIDES[QUEEN]) != 0
        }

    def moveNumber(self):
        return self._moveNumber
    


