import logging
from configparser import ConfigParser
from typing import Optional

from pathlib import Path

import os

from click import UsageError


class CLIConfig:
    def __init__(self, config_content):
        assert isinstance(config_content, str)

        self.host = ''
        self.username = ''
        self.password = ''

        if config_content:
            self._parse_config(config_content)

    def _parse_config(self, config_content):
        cp = ConfigParser()
        cp.read_string(config_content)

        try:
            self.host = cp['DEFAULT']['host']
            self.username = cp['DEFAULT']['username']
            self.password = cp['DEFAULT']['password']

        except Exception as e:
            logging.error('Exception while parsing configuration {}'.format(e))
            raise UsageError('Exception while parsing configuration {}'.format(e))


def find_config_file() -> Optional[Path]:
    """
    Find the configuration file to use and return a Path to it.

    This function will look in various place to find the configuration file.
    In order, those location are tested:
        + ./.leosacclirc
        + $HOME/.leosacclirc
    :return: Optionally a path to the config file we found.
    """
    logger = logging.getLogger('find_config_file')
    for location in os.curdir, os.path.expanduser('~'):
        location = os.path.abspath(location)
        logger.debug('Trying to locate configuration file in {}'.format(location))

        p = Path(os.path.join(location, '.leosacclirc'))
        if p.is_file():
            return p
    return None


def load_config_file():
    """
    Attempt to load CLI configuration file.
    """
    p = find_config_file()
    if p:
        with p.open('rt') as file:
            cfg = CLIConfig(file.read())
            return cfg
    else:
        logging.warning('Could not locate CLI configuration file')
    return CLIConfig('')
