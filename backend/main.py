from flask import Flask, Response, request
from flask_cors import CORS, cross_origin
from chessEngine import Chessjs

app = Flask(__name__)
cors = CORS(app=app)

@app.route('/api/engine')
def move():
    data = request.get_json()
    print(data)
    return {
        'from': 'e7',
        'to': 'e6'
    }

@app.route('/api/engine/test')
def test():
    # chess = Chessjs.Chess
    data = request.get_json()
    print(data)

    return True

@app.route('/api')
def home():
    return Response("Hello world", status=200)

def main():
    app.run(debug=True, port=5501)

if __name__ == "__main__":
    main()