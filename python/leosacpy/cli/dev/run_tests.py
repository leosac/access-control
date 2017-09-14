import unittest

import click
from click import UsageError

from leosacpy.runner import LeosacCachedDBRunnerFactory, LeosacFullRunner, \
    LeosacDummyRunner
from leosacpy.tests.test_ws_audit import WSAudit
from leosacpy.tests.test_ws_general import WSGeneral


# Commands in this file are added manually
# in dev.py
from leosacpy.tests.test_ws_pfdigital import WSPifaceDigital
from leosacpy.tests.test_ws_zone import WSZone


@click.command(name='run-tests')
@click.option('--runner', '-r', type=click.Choice(['full', 'shareddb', 'direct']))
@click.option('--test', '-t', type=str,
              help='Name of the test to run')
@click.option('--server', '-s', type=str,
              help='Leosac server address to perform the test against. Only works '
                   'with "direct" runner mode.')
@click.pass_context
def run_tests(ctx, runner, test, server):
    suite = unittest.TestSuite()

    if runner == 'full' or runner is None:
        def runner_factory(cfg):
            return LeosacFullRunner(cfg)
    elif runner == 'shareddb':
        runner_factory = LeosacCachedDBRunnerFactory()
    elif runner == 'direct':
        # Do not run leosac, simply run the tests against
        # an existing server
        def runner_factory(cfg):
            return LeosacDummyRunner(server)

    if (server and runner != 'direct') or (runner == 'direct' and not server):
        raise UsageError('"direct" runner mode and server are mutually inclusive.')

    create_param = {'runner_factory': runner_factory,
                    'server_address': server
                    }

    #suite.addTest(WSGeneral.create_suite(WSGeneral, create_param))
    #suite.addTest(WSAudit.create_suite(WSAudit, create_param))
    #suite.addTest(WSZone.create_suite(WSZone, create_param, test))
    suite.addTest(WSPifaceDigital.create_suite(WSPifaceDigital, create_param, test))
    unittest.TextTestRunner(verbosity=2).run(suite)
