import os
import sys
import serial
from datetime import datetime, timedelta
import time


class SendData:
    S = serial.Serial("/dev/ttyACM1", 9600, bytesize=8, stopbits=1, timeout=None, dsrdtr=0)

    def data_transport(self, data):
        trans_dic = [
            'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k'
        ]

        return_data = ''
        for s in data:
            return_data += trans_dic[s]

        return return_data

    def set_number(self, data):
        command = 'setnum ' + self.data_transport(data) + '\r'
        print("command " + command)
        self.S.write(bytes(command, 'utf-8'))

    def set_dot(self, data):
        command = 'setdot ' + self.data_transport(data) + '\r'
        print("command " + command)
        self.S.write(bytes(command, 'utf-8'))


if os.geteuid() != 0:
    print("not root")
    os.execvp("sudo", ["sudo"] + ["python3"] + sys.argv)

print("testtest")
print("test")

td = TransData()

while True:
    now = datetime.now() + timedelta(hours=9)
    h = now.strftime('%H')
    m = now.strftime('%M')
    s = now.strftime('%S')
    sending_data = [
        int(h[0]),
        int(h[1]),
        10,
        int(m[0]),
        int(m[1]),
        10,
        int(s[0]),
        int(s[1])
    ]
    print(sending_data)
    td.send_data(sending_data)
    time.sleep(1)
