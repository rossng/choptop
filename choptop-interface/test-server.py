import asyncio
import websockets
import datetime
import random
import aioconsole
from  multiprocessing import Manager, Value
from random import randint

async def hello(websocket, path):
    while True:
        if(data["a"] != "null"):
            await websocket.send(str(data["a"]))
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
        	data["a"] = '{"event":leftPressed"}'
        elif inData == "r":
        	data["a"] = '{"event":rightPressed"}'
        elif inData == "u":
        	data["a"] = '{"event":upPressed"}'
        elif inData == "d":
        	data["a"] = '{"event":downPressed"}'
        elif inData == "w":
            data["a"] = '{"event":"weightReading", "value": ' + str(randint(0, 1000)) + '}'
        else:
        	data["a"] = inData

loop = asyncio.get_event_loop()
loop.run_until_complete(echo())
asyncio.get_event_loop().run_forever()