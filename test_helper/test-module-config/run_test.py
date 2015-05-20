from RemoteControl import RemoteController, ModuleConfigCommand
from Utils import *
from Runner import LeosacRunner


def main():
    leosac = LeosacRunner("this_test/test-module-config.xml")

    rc = RemoteController("127.0.0.1:12345", "TJz$:^DbZvFN@wv/ct&[Su6Nnu6w!fMGHEcIttyT")
    cmd_existing_module = ModuleConfigCommand("MONITOR")  # xml format

    rc.execute_command(cmd_existing_module)
    test_assert(cmd_existing_module.status is True
                and cmd_existing_module.module == "MONITOR",
                "Failed to retrieve config from valid module.")

    cmd_invalid_module = ModuleConfigCommand("INVALID_MODULE_BLA")
    rc.execute_command(cmd_invalid_module)
    test_assert(cmd_invalid_module.status is False,
                "Managed to retrieve config from invalid module.")

    leosac.interrupt()
    leosac.wait_abort(30)

if __name__ == "__main__":
    preconfigure()
    main()
