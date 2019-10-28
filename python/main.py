import asyncio
import websocket
import pyautogui


        
def on_message(ws, message):
    
    x = message.split('\n')
    print(int(float(x[0])*10), int(float(x[1])*10))
    pyautogui.moveRel(int(float(x[0])*10), int(float(x[1])*10) )
    

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