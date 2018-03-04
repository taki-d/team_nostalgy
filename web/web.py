from flask import Flask

app = Flask(__name__)


@app.route('/')
def hello_world():
    return 'Hello World!'


@app.route('/api')
def api():
    return 'this is nixie control api'


if __name__ == '__main__':
    app.run()
