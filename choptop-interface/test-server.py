import asyncio
import websockets
import datetime
import random
import aioconsole
from  multiprocessing import Manager, Value

async def hello(websocket, path):
    while True:
        if(data["a"] != "null"):
            await websocket.send('{"event":"' + str(data["a"]) + '"}')
            data["a"] = 'null';
        await asyncio.sleep(0.1)
    return

start_server = websockets.serve(hello, 'localhost', 8765)







directionPressed = "left"
manager = Manager()

data = manager.dict()
data["a"] = 'null'

asyncio.get_event_loop().run_until_complete(start_server)


async def echo():
    stdin, stdout = await aioconsole.get_standard_streams()
    async for line in stdin:
        stdout.write(line)
        inData = line.decode().strip()

        if inData == "l":
        	data["a"] = "leftPressed"
        elif inData == "r":
        	data["a"] = "rightPressed"
        elif inData == "u":
        	data["a"] = "upPressed"
        elif inData == "d":
        	data["a"] = "downPressed"
        else:
        	data["a"] = inData

loop = asyncio.get_event_loop()
loop.run_until_complete(echo())
asyncio.get_event_loop().run_forever()