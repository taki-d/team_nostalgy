from flask import Flask, render_template, request
from control import SendData
import json
import time

app = Flask(__name__)


@app.route('/')
def hello_world():
    return render_template('index.html', title='Nixie Control')


@app.route('/api', methods=['POST'])
def api():
    sd = SendData()

    data = json.loads(request.data)
    if 'number' in data.keys():
        sd.set_number(data["number"])

    if 'dot' in data.keys():
        sd.set_dot(data["dot"])

    print(data)
    return 'this is nixie control api'


if __name__ == '__main__':
    app.run()
