# Test that two instance that share the same configuration (except for their name)
# will end-up matching two different doors in the Auth code.
#
# Pre-condition: Configuration is the same, except for `instance_name` and the Instrumentation module's
# IPC endpoint.
#
# Test scenario:
# + Start Leosac 1.
# + Start Leosac 2.
# + Simulate Auth against Leosac 1. Should succeed.
# + Simulate Auth against Leosac 2. Should fail.

import subprocess
import logging
import signal
import re
from InstrumentationClient import WiegandClient
from Utils import *


def insert_card_and_get_output(process):
    stdout = ""

    ic = WiegandClient("/tmp/leosac-ipc", "wiegand_data_high", "wiegand_data_low")
    ic.send_card("aa:bb:cc:42")

    count = 0
    while count < 5:
        try:
            output = process.communicate(None, 5)
            stdout += output[0].decode("utf-8")
            break
        except subprocess.TimeoutExpired:
            logging.info("Communicate() timeout expired.")
            process.send_signal(signal.SIGINT)
            count += 1

    process.wait()
    test_assert(process.returncode == 0,
                "Process returned with non-zero exit code ({0})".format(process.returncode))
    return stdout


def check_access_granted(log):
    ret = re.search("^.* \[info\] AUTH_CONTEXT_1 GRANTED .*$", log, re.MULTILINE)
    test_assert(ret, "Access was not granted.")


def check_access_denied(log):
    ret = re.search("^.* \[info\] AUTH_CONTEXT_1 DENIED .*$", log, re.MULTILINE)
    test_assert(ret, "Access was not denied.")


def main():
    try:
        logging.info("Leosac path is [{0}]".format(get_leosac_path()))
        leosac_1 = subprocess.Popen([get_leosac_path(), "-k", "this_test/rpi1-cfg.xml"], stdout=subprocess.PIPE)
        out = insert_card_and_get_output(leosac_1)
        logging.info("Leosac 1 output: {0}".format(out))
        check_access_granted(out)

        leosac_2 = subprocess.Popen([get_leosac_path(), "-k", "this_test/rpi2-cfg.xml"], stdout=subprocess.PIPE)
        out = insert_card_and_get_output(leosac_2)
        logging.info("Leosac 2 output: {0}".format(out))
        check_access_denied(out)

    except KeyboardInterrupt:
        logging.error("Interrupted.")
        if leosac_1.returncode is None:
            leosac_1.kill()
        if leosac_2.returncode is None:
            leosac_2.kill()
    except Exception as e:
        logging.exception("Exception occurred: " + str(e))
        if leosac_1.returncode is None:
            leosac_1.kill()
        if leosac_2.returncode is None:
            leosac_2.kill()
    return 0


if __name__ == "__main__":
    preconfigure()
    main()
