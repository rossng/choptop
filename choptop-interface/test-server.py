import asyncio
import websockets
import datetime
import random
import aioconsole
from  multiprocessing import Manager, Value

async def hello(websocket, path):
    while True:
        now = datetime.datetime.utcnow().isoformat() + 'Z'
        await websocket.send(str(data["a"]))
        await asyncio.sleep(0.1)
    return

start_server = websockets.serve(hello, 'localhost', 8765)







directionPressed = "left"
manager = Manager()

data = manager.dict()
data["a"] = "start"

asyncio.get_event_loop().run_until_complete(start_server)


async def echo():
    stdin, stdout = await aioconsole.get_standard_streams()
    async for line in stdin:
        stdout.write(line)
        data["a"] = line.decode().strip()

loop = asyncio.get_event_loop()
loop.run_until_complete(echo())
asyncio.get_event_loop().run_forever()