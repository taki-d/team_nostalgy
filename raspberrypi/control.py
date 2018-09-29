import os
import sys
import serial
from datetime import datetime, timedelta
import subprocess
import time
import wiringpi as wp
import RPi.GPIO as GPIO
import enum
import json
import fasteners


class TransData:
    S = serial.Serial("/dev/ttyUSB0", 9600, bytesize=8, stopbits=1, timeout=None, dsrdtr=0)

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


def get_temperature():
    res = subprocess.check_output(['python', '/home/pi/workspace/team_nostalgy/raspberrypi/bme280_temperature.py'])
    tempe = res.decode('utf-8').replace('.', '')

    num = [
        int(tempe[0]),
        int(tempe[1]),
        10,
        int(tempe[2]),
        int(tempe[3]),
        int(tempe[4]),
        int(tempe[5]),
    ]
    dot = [
        0,
        0,
        2,
        0,
        0,
        0,
        0,
        0,
    ]

    return [num, dot]


def get_pressure():
    res = subprocess.check_output(['python', '/home/pi/workspace/team_nostalgy/raspberrypi/bme280_pressure.py'])
    pressure = res.decode('utf-8').replace('.', '')

    if pressure[0] != '1':
        pressure = '0' + pressure

    num = [
        (10 if pressure[0] == "0" else 1),
        int(pressure[1]),
        int(pressure[2]),
        int(pressure[3]),
        10,
        int(pressure[4]),
        int(pressure[5]),
    ]
    dot = [
        0,
        0,
        0,
        0,
        2,
        0,
        0,
        0,
    ]

    return [num, dot]


def get_humidity():
    res = subprocess.check_output(['python', '/home/pi/workspace/team_nostalgy/raspberrypi/bme280_humidity.py'])
    humidity = res.decode('utf-8').replace('.', '')

    if humidity[0] != '1':
        humidity = '0' + humidity

    num = [
        (10 if humidity[0] == '0' else 1),
        int(humidity[1]),
        int(humidity[2]),
        10,
        int(humidity[4]),
        int(humidity[5]),
        int(humidity[6])
    ]
    dot = [
        0,
        0,
        0,
        2,
        0,
        0,
        0,
        0,
    ]

    return [num, dot]


def time_now():
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

    dot = [
        0,
        0,
        2,
        0,
        0,
        2,
        0,
        0,
    ]
    return [sending_data, dot]


mode = 1
'''
1:Clock Mode
2:Voltage Mode
3:Temperature Mode
4:Humidity Mode
5:Pressure Mode
6:Divergence Mode
7:WebMode
'''


def pushed_switch(n):
    global mode
    if n == 4:
        print("4 pushed")
        GPIO.cleanup()
        setup_gpio_pud_up()
        time.sleep(0.5)
        if GPIO.input(n) == 0:
            time.sleep(0.5)
            if GPIO.input(n) == 0:
                mode = 1
                setup_gpio()
    elif n == 17:
        print("17 pushed")
        GPIO.cleanup()
        setup_gpio_pud_up()
        time.sleep(0.5)
        if GPIO.input(n) == 0:
            time.sleep(0.5)
            if GPIO.input(n) == 0:
                mode = 2
                setup_gpio()
    elif n == 5:
        print("5 pushed")
        GPIO.cleanup()
        setup_gpio_pud_up()
        time.sleep(0.5)
        if GPIO.input(n) == 0:
            time.sleep(0.5)
            if GPIO.input(n) == 0:
                mode = 3
                setup_gpio()
    elif n == 6:
        print("6 pushed")
        GPIO.cleanup()
        setup_gpio_pud_up()
        time.sleep(0.5)
        if GPIO.input(n) == 0:
            time.sleep(0.5)
            if GPIO.input(n) == 0:
                mode = 4
                setup_gpio()

    elif n == 13:
        print("13 pushed")
        GPIO.cleanup()
        setup_gpio_pud_up()
        time.sleep(0.5)
        if GPIO.input(n) == 0:
            time.sleep(0.5)
            if GPIO.input(n) == 0:
                mode = 5
                setup_gpio()
            else:
                setup_gpio()
        else:
            setup_gpio()
    elif n == 19:
        print("19 pushed")
        GPIO.cleanup()
        setup_gpio_pud_up()
        time.sleep(0.5)
        if GPIO.input(n) == 0:
            time.sleep(0.5)
            if GPIO.input(n) == 0:
                mode = 6
                setup_gpio()
    elif n == 26:
        print("26 pushed")
        GPIO.cleanup()
        setup_gpio_pud_up()
        time.sleep(0.5)
        if GPIO.input(n) == 0:
            time.sleep(0.5)
            if GPIO.input(n) == 0:
                mode = 7
                setup_gpio()


def move_servo(servo):
    servo_pin = 12
    wp.wiringPiSetupGpio()
    wp.pinMode(servo_pin, 2)
    wp.pwmSetMode(0)
    wp.pwmSetRange(1024)
    wp.pwmSetClock(375)

    set_degree = 30 * servo
    move_deg = int((4.75*set_degree/90 + 7.25)*(1024/100))
    wp.pwmWrite(servo_pin, move_deg)


def get_voltage():
    return [[0, 0, 0, 0, 0, 0, 0, 0], [0, 0, 0, 0, 0, 0, 0, 0]]


def get_divergence_value():
    return [[0, 0, 0, 0, 0, 0, 0, 0], [0, 0, 0, 0, 0, 0, 0, 0]]


def get_web():
    return [[0, 0, 0, 0, 0, 0, 0, 0], [0, 0, 0, 0, 0, 0, 0, 0]]


if os.geteuid() != 0:
    print("not root")
    os.execvp("sudo", ["sudo"] + ["python3"] + sys.argv)


def setup_gpio():
    for pin_num in [4, 17, 5, 6, 13, 19, 26]:
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(pin_num, GPIO.IN, pull_up_down=GPIO.PUD_UP)
        GPIO.add_event_detect(pin_num, GPIO.FALLING, callback=pushed_switch, bouncetime=1000)


def setup_gpio_pud_up():
    for pin_num in [4, 17, 5, 6, 13, 19, 26]:
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(pin_num, GPIO.IN, pull_up_down=GPIO.PUD_UP)


# while True:
#     time.sleep(1)

# setup_gpio()
get_temperature()
get_pressure()
get_humidity()

move_servo(1)

print("testtest")
print("test")
td = TransData()

while True:
    data = [[0, 0, 0, 0, 0, 0, 0, 0], [0, 0, 0, 0, 0, 0, 0, 0]]
    '''
    1:Clock Mode
    2:Voltage Mode
    3:Temperature Mode
    4:Humidity Mode
    5:Pressure Mode
    6:Divergence Mode
    7:WebMode
    '''

    BASE_DIR = os.path.dirname(__file__)
    lock = fasteners.InterProcessLock("/var/tmp/lock")

    while not lock.acquire():
        print("locking")

    f = open('/home/pi/team_nostalgy/web/static/api.json', 'r')
    json_data = json.load(f)
    f.close()
    print(json_data)

    lock.release()

    num = [0, 0, 0, 0, 0, 0, 0, 0]
    dot = [0, 0, 0, 0, 0, 0, 0, 0]

    if 'mode' in json_data.keys():
        mode = json_data['mode']

    if 'num' in json_data.keys():
        num = json_data['num']

    if 'dot' in json_data.keys():
        dot = json_data['dot']

    if mode == 1:
        move_servo(0)
        data = time_now()
    elif mode == 2:
        move_servo(1)
        data = get_voltage()
    elif mode == 3:
        move_servo(2)
        data = get_temperature()
    elif mode == 4:
        data = get_humidity()
    elif mode == 5:
        data = get_pressure()
    elif mode == 6:
        data = get_divergence_value()
    elif mode == 7:
        data = [num, dot]

    td.set_number(data[0])
    td.set_dot(data[1])
    time.sleep(1)
