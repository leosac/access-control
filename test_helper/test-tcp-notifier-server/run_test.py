# Test that the Wiegand module is able to correctly
# read PIN code.
import time

from InstrumentationClient import WiegandClient
from Utils import *
from Runner import LeosacRunner
import re


def receive_card_number():
    stdout = u""

    leosac_1 = LeosacRunner("this_test/config.xml", False)
    tcp_server = subprocess.Popen(["this_test/tcp_client.py"])
    time.sleep(1)

    wiegand = WiegandClient("/tmp/leosac-ipc", "wiegand_data_high", "wiegand_data_low")

    wiegand.send_card("ff:ff:ff:ff")
    time.sleep(1)
    wiegand.send_card("00:00:00:10")

    tcp_server.wait()

    count = 0
    while count < 5:
        try:
            output = leosac_1.communicate(None, 5)
            stdout += output[0].decode("utf-8")
            break
        except subprocess.TimeoutExpired:
            logging.info("Communicate() timeout expired.")
            leosac_1.interrupt()
            count += 1

    leosac_1.wait()

    test_assert(tcp_server.returncode == 0,
                "TCPServer returned with non zero code")
    test_assert(leosac_1.returncode == 0,
                "Process returned with non-zero exit code ({0})".format(leosac_1.returncode))


def main():
    receive_card_number()


if __name__ == "__main__":
    preconfigure()
    main()
