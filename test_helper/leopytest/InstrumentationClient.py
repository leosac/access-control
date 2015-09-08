# Implement client code to interact with the Instrumentation module
# that runs on Leosac.

import zmq
import logging
import time


class InstrumentationClient():
    """
    A base client object to talk to the Leosac Instrumentation module.

    It abstracts away the IPC endpoint and the low-level command supported by the module.
    """

    def __init__(self, ipc_endpoint):
        """
        Create a new client to simulate GPIO activity within Leosac.

        :param ipc_endpoint: The file system path the endpoint.
        :return:
        """
        self.endpoint = ipc_endpoint
        self.zmq_context = zmq.Context.instance()
        self.ipc_socket = self.zmq_context.socket(zmq.DEALER)
        self.ipc_socket.connect("ipc://" + self.endpoint)

    def __del__(self):
        self.zmq_context.destroy(linger=1000)

    def send_interrupt(self, gpio_name):
        """
        Triggers an interrupt on the gpio name gpio_name

        :param gpio_name: Name of the GPIO we fake an interrupt on.
        :return:
        """
        msg = [zmq.Frame('GPIO'.encode("ascii")),
               zmq.Frame(gpio_name),
               zmq.Frame("INT".encode("ascii"))]
        self.ipc_socket.send_multipart(msg)


class WiegandClient(InstrumentationClient):
    """
    Abstract a fake WiegandReader.

    It can simulate the reading of cards or pin code.
    """

    def __init__(self, ipc_endpoint, gpio_high=None, gpio_low=None):
        """

        :param ipc_endpoint: The IPC endpoint to connect to
        :param gpio_high: The name of the "high" gpio. Can be set later.
        :param gpio_low: The name of the "low" gpio. Can be set later.
        :return:
        """
        super().__init__(ipc_endpoint)
        self.gpio_high = bytes(gpio_high, "ascii")
        self.gpio_low = bytes(gpio_low, "ascii")

    def send_card(self, card_id):
        """
        Simulate the reading of a card.

        :param card_id: A card in hexadecimal format like: 11:bc:de:42
        :return:
        """
        assert (self.gpio_high and self.gpio_low)
        logging.info("Attempting to send card {0}".format(card_id))
        card_str = card_id.replace(":", "")

        binary_string = bin(int(card_str, 16))[2:].zfill(len(card_str) * 4)
        for c in binary_string:
            if c == "1":
                self.send_interrupt(self.gpio_high)
            else:
                self.send_interrupt(self.gpio_low)

    def send_4bits_pin(self, pin):
        """
        Simulate the insertion of a PIN code coded with 4bits per number.

        :param pin: The pin number, in decimal.
        :return:
        """
        logging.info("Attempting to send 4bits pin code {0}".format(pin))

        for key in pin:
            # bin repr of one key. 4 bits
            binary = bin(int(key, 16))[2:].zfill(4)
            for c in binary:
                if c == "1":
                    self.send_interrupt(self.gpio_high)
                else:
                    self.send_interrupt(self.gpio_low)
            # We need 50ms timeout to process each key, otherwise all keys look
            # like one.
            time.sleep(0.6)

    def send_8bits_pin(self, pin):
        """
        Simulate the insertion of a PIN code coded with 8bits per number.

        :param pin: The pin number, in decimal.
        :return:
        """
        logging.info("Attempting to send 8bits pin code {0}".format(pin))

        for key in pin:
            binary = bin(int(key, 16))[2:].zfill(4)
            # first send the reversed bits.
            for c in binary:
                if c == "0":
                    self.send_interrupt(self.gpio_high)
                else:
                    self.send_interrupt(self.gpio_low)
            # Now send the "normal" bits.
            for c in binary:
                if c == "1":
                    self.send_interrupt(self.gpio_high)
                else:
                    self.send_interrupt(self.gpio_low)
            time.sleep(0.6)
