from flask import Flask, Response

app = Flask(__name__)

@app.route('/api')
def home():
    return Response("Hello world", status=200)

def main():
    app.run(debug=True, port=5501)

if __name__ == "__main__":
    main()