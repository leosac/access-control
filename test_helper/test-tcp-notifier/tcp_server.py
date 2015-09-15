#!/usr/bin/env python
import sys

__author__ = 'xaqq'

import asyncio
import struct


@asyncio.coroutine
def handle_msg(reader, writer):
    """
    We'll receive 2 card numbers
    """
    data = yield from reader.readexactly(8)
    card_id = struct.unpack("!Q", data)[0]
    if card_id != 4294967295:
        sys.exit(1)

    data = yield from reader.readexactly(8)
    card_id = struct.unpack("!Q", data)[0]
    if card_id != 16:
        sys.exit(1)

    sys.exit(0)
    writer.close()

loop = asyncio.get_event_loop()
coro = asyncio.start_server(handle_msg, '127.0.0.1', 4242, loop=loop)
server = loop.run_until_complete(coro)
loop.run_forever()

# Close the server
server.close()
loop.run_until_complete(server.wait_closed())
loop.close()
