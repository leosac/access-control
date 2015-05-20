# This module implements client side support of the Remote Control
# features exposed by Leosac.

import zmq
import logging
import struct
from Utils import test_assert


class RemoteController():
    """
    The driver to talk to a remote Leosac.

    This class is responsible for establishing the communication and for
    executing commands.
    """

    def __init__(self, endpoint, public_key):
        """
        Initialize the RemoteController.

        :param endpoint: The endpoint to connect to
        :param public_key: The remote host's public key.
        :return:
        """
        self.zmq_context = zmq.Context.instance()
        self.tcp_socket = self.zmq_context.socket(zmq.DEALER)

        # Configure the various keys.
        client_public, client_secret = zmq.curve_keypair()
        self.tcp_socket.curve_serverkey = public_key.encode("utf-8")
        self.tcp_socket.curve_publickey = client_public
        self.tcp_socket.curve_secretkey = client_secret

        self.tcp_socket.connect("tcp://" + endpoint)

    def execute_command(self, command):
        """
        Send a command to the remote server.

        :param command:
        :return:
        """
        self.tcp_socket.send_multipart(command.to_zmq_message())
        res = self.tcp_socket.recv_multipart()
        logging.info("Response to command {0}: {1}".format(command, res))
        command.feed_response(res)

    def __del__(self):
        self.zmq_context.destroy(linger=1000)


class Command():
    """
    Base class for command object.
    """

    def __init__(self):
        self.status = None

    def to_zmq_message(self):
        """
        Convert the command to its ZeroMQ representation (a list of frames).

        :return: A list of ZeroMQ frames
        """
        logging.error("This method needs to be reimplemented in subclasses.")
        exit(-1)

    def feed_response(self, frames):
        """
        Called by the RemoteController with the response.

        :param frames: The list of frames in the response
        """
        logging.error("This method needs to be reimplemented in subclasses.")
        exit(-1)


class GeneralConfigCommand(Command):
    """
    Command object to retrieve the GeneralConfiguration from the remote host.
    """

    def __init__(self, config_format):
        """
        Initialize the command.

        :param config_format: 0 for boost archive, 1 for xml
        :return:
        """
        if config_format == 0:
            logging.warning("Creating a GeneralConfigCommand that will "
                            "fetch config in boost::archive format.")
        elif config_format == 1:
            logging.warning("Creating a GeneralConfigCommand that will "
                            "fetch config in XML format.")
        else:
            logging.error("Invalid config format: {0}".format(config_format))
            exit(-1)
        self.format = config_format
        self.config = ""

    def to_zmq_message(self):
        cfg_format = struct.pack("!B", self.format)
        return ["GENERAL_CONFIG".encode("utf-8"), cfg_format]

    def feed_response(self, frames):
        test_assert(len(frames) == 2, "Invalid number of frames in response")
        if frames[0] == bytes("OK", "ascii"):
            self.status = True
        else:
            self.status = False
        self.config = frames[1]

    def __str__(self):
        return "<RemoteControl.GeneralConfigCommand>"


class ModuleConfigCommand(Command):
    """
    Command object to retrieve the configuration of a Leosac module.

    Config format is force to XML.
    """

    def __init__(self, module_name):
        """
        Initialize the command so that it fetches the config for module_name.

        :param module_name: Name of the config to fetch config for.
        :return:
        """
        self.module_name = module_name.encode("utf-8")

        # Module name sent by server.
        self.module = None
        self.config = None

    def to_zmq_message(self):
        cfg_format = struct.pack("!B", 1)
        return ["MODULE_CONFIG".encode("utf-8"), self.module_name, cfg_format]

    def feed_response(self, frames):
        if frames[0] == bytes("OK", "ascii"):
            self.status = True
        else:
            self.status = False
            return
        self.module = frames[1].decode("utf-8")
        self.config = frames[2].decode("utf-8")

    def __str__(self):
        return "<RemoteControl.ModuleConfigCommand>"
