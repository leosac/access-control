# Test that the Wiegand module is able to correctly
# read PIN code.

from InstrumentationClient import WiegandClient
from Utils import *
from Runner import LeosacRunner
import re


def test_4bits():
    stdout = ""
    leosac_1 = LeosacRunner("this_test/config.xml", False)
    wiegand = WiegandClient("/tmp/leosac-ipc", "wiegand_data_high", "wiegand_data_low")

    wiegand.send_4bits_pin("1337")

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
    test_assert(leosac_1.returncode == 0,
                "Process returned with non-zero exit code ({0})".format(leosac_1.returncode))
    ret = re.search("^.* 1337$", stdout, re.MULTILINE)
    print("RESULT = ", stdout)
    test_assert(ret, "4bits Pin code wasn't read")


def test_8bits():
    stdout = ""
    leosac_1 = LeosacRunner("this_test/config-8bits.xml", False)
    wiegand = WiegandClient("/tmp/leosac-ipc", "wiegand_data_high", "wiegand_data_low")

    wiegand.send_8bits_pin("31337")
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
    test_assert(leosac_1.returncode == 0,
                "Process returned with non-zero exit code ({0})".format(leosac_1.returncode))
    ret = re.search("^.* 31337$", stdout, re.MULTILINE)
    test_assert(ret, "8bits Pin code wasn't read")


def main():
    test_4bits()
    test_8bits()


if __name__ == "__main__":
    preconfigure()
    main()
