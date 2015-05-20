# Test the SAVE command.
#

from RemoteControl import RemoteController, SaveCommand
from Utils import *


def main():
    rc = RemoteController("127.0.0.1:12345", "TJz$:^DbZvFN@wv/ct&[Su6Nnu6w!fMGHEcIttyT")
    cmd = SaveCommand()

    rc.execute_command(cmd)
    test_assert(cmd.status is False,
                "Successfully saved configuration while it was supposed to fail")

if __name__ == "__main__":
    preconfigure()
    main()
