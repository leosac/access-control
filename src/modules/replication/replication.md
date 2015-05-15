Replication Module Documentation {#mod_replication_main}
========================================================

@brief Run Leosac in a master/slave configuration.

[TOC]

Introduction {#mod_replication_intro}
=================================

Leosac can run in a master/slave configuration. The master is not aware of his role, and
does not need to run any specific module.

The slave unit needs the `Replication` module to automatically performs the
synchronization.

@note Since synchronizing with a untrusted master in a huge security risk,
the slave needs the master's public key to make sure it talks to the right server.


Configuration Options {#mod_replication_user_config}
====================================================

Options    | Description                                            | Mandatory
-----------|--------------------------------------------------------|-----------
delay      | Number of seconds between replication attempt.         | NO, default to 120.
endpoint   | Endpoint of the master server.                         | YES
pubkey     | Public key of the master server                        | YES

Example {#mod_replication_example}
----------------------------------

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
    <module>
        <name>REPLICATION</name>
        <file>libreplication.so</file>
        <level>11</level>
        <module_config>
            <delay>42</delay>
            <endpoint>tcp://127.0.0.1:23456</endpoint>
            <pubkey>TJz$:^DbZvFN@wv/ct&[Su6Nnu6w!fMGHEcIttyT</pubkey>
        </module_config>
    </module>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
