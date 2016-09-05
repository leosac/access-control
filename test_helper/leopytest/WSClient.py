from ws4py.client.threadedclient import WebSocketClient

class WSClient():
    """
    A Leosac WebSocket API client, used for testing.
    """

    def __init__(self):
        self.ws = WebSocketClient('ws://localhost:4242')
        ws.connect()