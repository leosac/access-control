import logging
import os
import subprocess


def test_assert(res, msg):
    """
    Test that the res variable is not false. Log msg if it is and exit.

    :param res:
    :param msg:
    :return:
    """
    if not res:
        logging.error("Assertion failed: " + msg)
        exit(-1)


def get_leosac_path():
    """
    A simple function to return the path to the Leosac binary.

    :return: The path the Leosac binary
    """

    path = os.getcwd() + "/install/bin/leosac"
    if not os.path.isfile(path):
        path = subprocess.check_output(["which", "leosac"])

    test_assert(os.path.isfile(path), "Cannot find leosac binary")
    return path


def preconfigure():
    """
    Run some global pre configuration stuff.
    This method should be called before python main().

    Currently, it configures the logger.
    :return:
    """
    logging.basicConfig(format='%(asctime)s %(message)s',
                        level=logging.DEBUG,
                        datefmt='%m/%d/%Y %H:%M:%S:')
