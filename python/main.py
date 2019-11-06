import asyncio
import websocket
import pyautogui
import autopy

import ctypes

# from win32api import GetSystemMetrics
from time import sleep

user32 = ctypes.windll.user32
xlen = int(user32.GetSystemMetrics(0)/2)
ylen = int(user32.GetSystemMetrics(1)/2)
toWidth = 0
k = 0
p = 0
toHeight = 0
prevWidth = xlen
prevHeight = ylen
steps = 100
print(prevWidth, prevHeight)


def on_message(ws, message):
    global prevHeight, prevWidth
    x = message.split('\n')

    toWidth = int(xlen) + int(float(x[0]))
    toHeight = int(ylen) + int(float(x[1]))
    diff = abs(prevWidth - toWidth)
    diff2 = abs(prevHeight - toHeight)
    print(toWidth, toHeight, diff, diff2)
    
    # if(not(diff<30 and diff2 < 30)):
    autopy.mouse.move(toWidth, toHeight)
    k = (toWidth - prevWidth)/steps
    p = (toHeight - prevHeight)/steps
    # for i in range(steps):
    #     calcWidth = int(k*i + prevWidth)
    #     calcHeight = int(p*i + prevHeight)
    #     autopy.mouse.move(calcWidth,calcHeight)
    prevWidth = toWidth
    prevHeight = toHeight
    


for i in range(100):
    sleep(10/1000)
    autopy.mouse.move(i, i)
autopy.mouse.move(prevWidth, prevHeight)

def on_error(ws, error):
    print(error)


def on_close(ws):
    print("### closed ###")


def on_open(ws):
    print('Started')

    def run(*args):
        for i in range(3):
            time.sleep(1)

        time.sleep(1)
        ws.close()
        print("thread terminating...")
    thread.start_new_thread(run, ())


ws = websocket.WebSocketApp("ws://192.168.43.209:81",
                            on_message=on_message,
                            on_error=on_error,
                            on_close=on_close)
ws.on_open = on_open
ws.run_forever()
