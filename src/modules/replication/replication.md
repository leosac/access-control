Replication Module Documentation {#mod_replication_main}
========================================================

[TOC]

Introduction {#mod_replication_intro}
=================================

Documentation for the master/slave replication.

Configuration Options {#mod_replication_user_config}
================================================



Options    | Options  | Description                                            | Mandatory
-----------|----------|--------------------------------------------------------|-----------
delay      |          | Number of seconds between replication attempt.         | NO, default to 15.
endpoint   |          | Endpoint of the master server.                         | YES
pubkey     |          | Public key of the master server                        | YES

Example {#mod_replication_example}
------------------------------

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
