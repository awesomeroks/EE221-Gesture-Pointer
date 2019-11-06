import asyncio
import websocket
import pyautogui
import autopy


from win32api import GetSystemMetrics
from time import sleep

width = GetSystemMetrics(0)
height = GetSystemMetrics(1)
def on_message(ws, message):
    
    x = message.split('\n')
    print(int(float(x[0])*10), int(float(x[1])*10))
    width = width + int(float(x[0])*100)
    height = height + int(float(x[1])*100)
    autopy.mouse.move(width,height)
    # pyautogui.moveRel(int(float(x[0])*10), int(float(x[1])*10))
    
# #TEST
# i = 0 
          
# sleep(1)
# autopy.mouse.move(0,0)
# while(True):
    
#     autopy.mouse.move(i,i)
#     sleep(10/1000)   
#     i = i + 1

def on_error(ws, error):
    print (error)

def on_close(ws):
    print ("### closed ###")

def on_open(ws):
    def run(*args):
        for i in range(3):
            time.sleep(1)
            
        time.sleep(1)
        ws.close()
        print ("thread terminating...")
    thread.start_new_thread(run, ())

ws = websocket.WebSocketApp("ws://192.168.43.209:81",
                              on_message = on_message,
                              on_error = on_error,
                              on_close = on_close)
ws.on_open = on_open
ws.run_forever()                    