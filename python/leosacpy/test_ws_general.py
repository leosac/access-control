import json
import unittest

import time
import websocket
import logging

from leosacpy.runner import LeosacFullRunner


class LeosacWSClient:
    def __init__(self, target, timeout=10):

        self.ws = None
        timeout_start = time.time()
        last_ex = None
        while time.time() < timeout_start + timeout:
            try:
                self.ws = websocket.create_connection(target)
                break
            except ConnectionRefusedError as e:
                last_ex = e
        if self.ws is None:
            assert last_ex, 'Should have an exception'
            raise last_ex

    def send(self, obj):
        c = json.dumps(obj)
        self.ws.send(c)

    def recv(self):
        return self.ws.recv()


class WSGeneral(unittest.TestCase):
    def setUp(self):
        super().setUp()
        logging.basicConfig(level=logging.DEBUG)
        logging.getLogger().setLevel(logging.DEBUG)

    def test_get_version(self):
        with LeosacFullRunner() as r:
            url = r.get_ws_address()
            logging.debug('Connection to {}'.format(url))
            ws = LeosacWSClient(r.get_ws_address())
            ws.send({})
            ret = ws.recv()
            print(ret)


if __name__ == '__main__':
    unittest.main()
