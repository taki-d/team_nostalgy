import fasteners
import os
import json
import time


while 1:
    BASE_DIR = os.path.dirname(__file__)
    lock = fasteners.InterProcessLock("/var/tmp/lock")

    while not lock.acquire():
        print("locking")

    f = open('./static/api.json', 'r')
    json_data = json.load(f)
    f.close()
    print(json_data)

    lock.release()
    time.sleep(0.01)
