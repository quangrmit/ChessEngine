from flask import Flask, Response, request
from flask_cors import CORS, cross_origin
from chessEngine import Chessjs

app = Flask(__name__)
cors = CORS(app=app)
app.config['Access-Control-Allow-Origin'] = '*'

## Initialize chess engine 
chess = Chessjs.Chess()

@app.route('/api/engine', methods = ['POST'])
def move():
    data = request.get_json()
    print(data)
    return {
        'from': 'e7',
        'to': 'e6'
    }

@app.route('/api/engine/test', methods = ['POST'])
def test():
    # chess = Chessjs.Chess
    data = request.get_json()
    try:
        chess.move(move=data)
    except:
        return {
            'status': 'False',
            'mess': 'Invalid move'
        }
    
    # print(chess._board)
    # print(data)

    return {
        'status': 'True',
        'mess': 'Valid move',
        'fen': str(chess.fen())
    }
@app.route('/api')
def home():
    return Response("Hello world", status=200)

def main():
    app.run(debug=True, port=5501)

if __name__ == "__main__":
    main()