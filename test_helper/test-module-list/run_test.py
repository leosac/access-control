# Test the MODULE_LIST command.
#

from RemoteControl import RemoteController, ModuleListCommand
from Utils import *
from Runner import LeosacRunner


def main():
    leosac = LeosacRunner("this_test/test-module-list.xml")

    rc = RemoteController("127.0.0.1:12345", "TJz$:^DbZvFN@wv/ct&[Su6Nnu6w!fMGHEcIttyT")
    cmd = ModuleListCommand()

    rc.execute_command(cmd)
    test_assert(cmd.status is True
                and ("MONITOR" in cmd.modules)
                and ("WIEGAND_READER" in cmd.modules)
                and ("NOT_A_MODULE" not in cmd.modules),
                "Failed to retrieve config from valid module.")

    leosac.run_at_most(10)

if __name__ == "__main__":
    preconfigure()
    main()
