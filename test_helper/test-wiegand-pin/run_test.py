# Test that the Wiegand module is able to correctly
# read PIN code.
import time

from InstrumentationClient import WiegandClient
from Utils import *
from Runner import LeosacRunner
import re


def test_4bits():
    leosac = LeosacRunner("this_test/config.xml")
    wiegand = WiegandClient("/tmp/leosac-ipc", "wiegand_data_high", "wiegand_data_low")
    time.sleep(3)

    wiegand.send_4bits_pin("1337")
    leosac.run_at_most(7)
    test_assert(leosac.returncode == 0,
                "Process returned with non-zero exit code ({0})".format(leosac.returncode))
    ret = re.search("^.* 1337$", leosac.out, re.MULTILINE)
    test_assert(ret, "4bits Pin code wasn't read")


def test_8bits():
    leosac = LeosacRunner("this_test/config-8bits.xml")
    wiegand = WiegandClient("/tmp/leosac-ipc", "wiegand_data_high", "wiegand_data_low")
    time.sleep(3)

    wiegand.send_8bits_pin("31337")
    leosac.run_at_most(7)
    test_assert(leosac.returncode == 0,
                "Process returned with non-zero exit code ({0})".format(leosac.returncode))
    ret = re.search("^.* 31337$", leosac.out, re.MULTILINE)
    test_assert(ret, "8bits Pin code wasn't read")


def main():
    test_4bits()
    test_8bits()


if __name__ == "__main__":
    preconfigure()
    main()
