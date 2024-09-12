from flask import Flask, Response, request, make_response
from flask_cors import CORS, cross_origin
import os, subprocess

app = Flask(__name__)
cors = CORS(app=app)
app.config['Access-Control-Allow-Origin'] = '*'

## Initialize chess engine 


@app.route('/api/engine', methods = ['GET'])
def move():
    # data = request.get_json()
    print("Run C++ files")

    res = subprocess.run(['..\\backend\\chessEngineCpp\\main.exe', request.args.get("fen")], capture_output=True).stdout.decode()
    
    # res = subprocess.run(['C:\\Users\\Lenovo\\OneDrive\\desktop\\FogOfWar-AI\\backend\\chessEngineCpp\\main.exe', request.args.get("fen")], capture_output=True).stdout.decode()

    print("hello world")
    print(request.args.get("fen"))
    print("here")
    print(res)

    # data = {
    #     'from': res[0:2],
    #     'to': res[2:4]
    # }
    # response = make_response((data))
    # response.headers['Content-Type'] = 'application/json'
    # response.headers['Access-Control-Allow-Origin'] = '*'
    # return response

    return {
        'from': res[0:2],
        'to':  res[2:4]
    }

@app.route('/api/engine/test', methods = ['POST', 'GET'])
def test():
    # chess = Chessjs.Chess
    # data = request.get_json()
    # try:
    #     chess.move(move=data)
    # except:
    #     return {
    #         'status': 'False',
    #         'mess': 'Invalid move'
    #     }
    
    # print(chess._board)
    # print(data)
    print("Another")
    res = subprocess.run(['D:\personalProject\FogOfWar-AI\\backend\\chessEngineCpp\main.exe', 'helloagain'], capture_output=True).stdout.decode()
    # res = os.system(r"D:\\personalProject\\FogOfWar-AI\\backend\\chessEngineCpp\\main.exe helloagain")
    print(f"Hello 111again {res}")

    return {
        'status': 'True',
        'mess': 'Valid move',
        'fen': 'hell'
    }
@app.route('/api')
def home():
    return Response("Hello world", status=200)

def main():
    app.run(debug=True, port=5501)

if __name__ == "__main__":
    main()