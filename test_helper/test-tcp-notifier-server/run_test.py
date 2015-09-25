# We test that the TCPNotifier module properly send
# card number to connected client.
import time
from InstrumentationClient import WiegandClient
from Utils import *
from Runner import LeosacRunner


def receive_card_number():
    leosac = LeosacRunner("this_test/config.xml")
    time.sleep(3)
    tcp_client = subprocess.Popen(["this_test/tcp_client.py"])

    wiegand = WiegandClient("/tmp/leosac-ipc", "wiegand_data_high", "wiegand_data_low")

    wiegand.send_card("ff:ff:ff:ff")
    wiegand.send_card("00:00:00:10")

    tcp_client.wait()
    leosac.run_at_most(5)

    test_assert(tcp_client.returncode == 0,
                "TCPClient returned with non zero code")
    test_assert(leosac.returncode == 0,
                "Process returned with non-zero exit code ({0})".format(leosac.returncode))


def main():
    receive_card_number()


if __name__ == "__main__":
    preconfigure()
    main()
