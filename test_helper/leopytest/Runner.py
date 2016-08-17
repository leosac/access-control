from Utils import get_leosac_path
import subprocess
import signal
import logging


class LeosacRunner(subprocess.Popen):
    """
    A class that wraps Popen with some application specific utility.

    It runs Leosac in valgrind for example.
    """

    def __init__(self, config_file_path, valgrind=True):
        if valgrind:
            super().__init__(["valgrind", "--error-exitcode=42",
                              get_leosac_path(), "-k", config_file_path],
                             stdout=subprocess.PIPE)
        else:
            super().__init__([get_leosac_path(), "-k", config_file_path],
                             stdout=subprocess.PIPE)
        self.out = u''
        self.err = u''

    def run_at_most(self, timer) -> bool:
        """
        Run the process for at most `timer` seconds.
        This method collects standard output and standard
        error (similar to what run_for does).

        However, if the process didn't terminated in the
        allowed time, we send it SIGINT.

        :param timer:
        :return:
        """
        ret = self.run_for(timer)
        if not ret:
            self.interrupt()
            # We give 10 seconds here because Leosac itself takes at
            # least 5 seconds to shut down. (Due to "soft shutdown" to
            # prevent potential deadlock
            self.wait_abort(10)
            out, err = super().communicate()
            self.append_output(out, err)
        return True

    def run_for(self, timer) -> bool:
        """
        Run the process for at most `timer` seconds.
        This method will collect standard output and
        standard error and append it in the `self.out` and
        `self.err` variable.

        :param timer:
        :return: True is the process has terminated, False otherwise.
        """
        try:
            output, err = super().communicate(timeout=timer)
            self.append_output(output, err)
            return True
        except subprocess.TimeoutExpired:
            logging.info("Waiting on LeosacRunner for %i seconds %s", timer, " threw TimeoutExpired")
            return False

    def interrupt(self):
        """
        Send SIGINT to the process.
        :return: None
        """
        super().send_signal(signal.SIGINT)

    def wait_abort(self, timeout) -> bool:
        """
        Wait for `timeout` seconds and abort this python
        process if the child process didn't stop in the allowed time.

        :param timeout:
        :return:
        """
        try:
            super().wait(timeout)
            return True
        except subprocess.TimeoutExpired:
            logging.error("Timeout expired when waiting on Leosac. Aborting")
            print("Standard Output: ", self.out)
            exit(-1)

    def append_output(self, out, err):
        if out:
            self.out += out.decode('utf-8')
        if err:
            self.err += err.decode('utf-8')