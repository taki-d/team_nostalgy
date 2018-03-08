from flask import Flask, render_template, request
# from control import SendData
import json
# import time
import os
import fasteners

app = Flask(__name__)


@app.route('/')
def hello_world():
    return "Hello World"


@app.route('/api', methods=['POST'])
def api():
    # sd = SendData()
    lock = fasteners.InterProcessLock('/var/tmp/lock')

    while not lock.acquire():
        print("locking file")

    data = json.loads(request.data)
    if 'mode' in data.keys():
        print(data)
        BASE_DIR = os.path.dirname(__file__)
        f = open(BASE_DIR + '/static/api.json', 'w')
        json.dump(data, f)
        lock.release()
        return "success"

    print(data)
    lock.release()
    return "fail. Please add mode value"


if __name__ == '__main__':
    app.run()
