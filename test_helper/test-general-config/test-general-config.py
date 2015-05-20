#!/usr/bin/env python

from RemoteControl import *
from Utils import preconfigure


def main():
    print("Hello, I will query the general config information")

    rc = RemoteController("127.0.0.1:12345", "TJz$:^DbZvFN@wv/ct&[Su6Nnu6w!fMGHEcIttyT")
    cmd = GeneralConfigCommand(1)  # xml format

    rc.execute_command(cmd)
    test_assert(cmd.status is True, "Command failed")

    expected = """<?xml version="1.0" encoding="utf-8"?>
<plugin_directories>
	<plugindir>./install/lib/leosac</plugindir>
	<plugindir>./install/lib/leosac/auth</plugindir>
</plugin_directories>
""".encode("utf-8")

    # Make sure the config received is what we expect.
    test_assert(cmd.config == expected, "Invalid content in response")
    return 0


if __name__ == "__main__":
    preconfigure()
    ret = main()
    exit(ret)
