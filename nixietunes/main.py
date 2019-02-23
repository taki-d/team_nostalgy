import appscript
from time import sleep
import requests

while True:
    url = None
    try:
        time = appscript.app('iTunes').player_position()
        if(type(time) is float):
            url = 'http://192.168.179.7/setting?num=%02d:%02d:%02d&dot=00100100' % (time/3600, (time%3600)/60, (time%60)/1)
        else:
            url = 'http://192.168.179.7/setting?num=00:00:00&dot=00100100'
    except Exception as ex:
        print(str(ex))
    
    #response = requests.get(url)
    print(url)
    sleep(0.1)