# We test that the TCPNotifier module properly send
# card number to client it connect to.
import time
from InstrumentationClient import WiegandClient
from Utils import *
from Runner import LeosacRunner


def receive_card_number():
    tcp_server = subprocess.Popen(["this_test/tcp_server.py"])
    leosac = LeosacRunner("this_test/config.xml")

    time.sleep(3)
    wiegand = WiegandClient("/tmp/leosac-ipc", "wiegand_data_high", "wiegand_data_low")

    wiegand.send_card("ff:ff:ff:ff")
    wiegand.send_card("00:00:00:10")

    tcp_server.wait()
    leosac.run_at_most(5)
    test_assert(tcp_server.returncode == 0,
                "TCPServer returned with non zero code")
    test_assert(leosac.returncode == 0,
                "Process returned with non-zero exit code ({0})".format(leosac.returncode))


def main():
    receive_card_number()


if __name__ == "__main__":
    preconfigure()
    main()
