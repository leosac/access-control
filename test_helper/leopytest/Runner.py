from Utils import get_leosac_path
import subprocess
import signal
import logging


class LeosacRunner(subprocess.Popen):
    """
    A class that wraps Popen with some application specific utility.

    It runs Leosac in valgrind for example.
    """

    def __init__(self, config_file_path):
        super().__init__(["valgrind", "--error-exitcode=42",
                          get_leosac_path(), "-k", config_file_path],
                         stdout=subprocess.PIPE)

    def interrupt(self):
        """
        Send SIGINT to the process
        :return:
        """
        super().send_signal(signal.SIGINT)

    def wait_abort(self, timeout):
        """
        Wait for a timeout second and abort if the process didn't stop.

        :param timeout:
        :return:
        """
        try:
            super().wait(timeout)
        except subprocess.TimeoutExpired:
            logging.error("Timeout expired when waiting on Leosac. Aborting")
            exit(-1)