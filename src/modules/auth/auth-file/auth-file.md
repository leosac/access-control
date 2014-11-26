Auth-File (backend) Module Documentation {#mod_auth_file_main}
==============================================================

[TOC]

Introduction {#mod_auth_file_intro}
================================

The module Auth-File is an **authentication backend** module.
This module is suitable only for a low number of users / cards / groups.
The whole configuration lives in memory so is where this limitation comes from.


Configuration Options {#mod_auth_file_user_config}
===================================================



Options    | Options     | Options    | Description                                                  | Mandatory
-----------|-------------|------------|--------------------------------------------------------------|-----------
instances  |             |            | List of configured auth file instance                        | YES
---->      | name        |            | Configure of one particular led device                       | YES
---->      | auth_source |            | Which device (auth source) we listen to                      | YES
---->      | config_file |            | Path to the config file that holds permissions data          | YES
---->      | target      |            | Name of the target (door) that we are authenticating against | NO

If the `target` is not present, the module assumes the default target, and will ignore target-specific
permissions.

Example {#mod_auth_file_example}
-----------------------------

This is the module configuration in the main config file.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
        <module>
            <name>AUTH-MANAGER-FILE</name>
            <file>libauth-file.so</file>
            <level>41</level>
            <module_config>
                <instances>
                    <instance>
                        <name>AUTH_CONTEXT_1</name>
                        <auth_source>MY_WIEGAND_1</auth_source>
                        <config_file>file_auth.xml</config_file>
                    </instance>
                </instances>
            </module_config>
        </module>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


This is a possible content of the file designed by the `config_field` config entry for
the module ("file_auth.xml" based on the previous example).

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
<root>
    <!--
    Maps WiegandCard to user
    -->
    <user_mapping>
        <map>
            <user>MY_USER</user>
            <WiegandCard>00:11:22:33</WiegandCard>
        </map>
        <map>
            <user>MY_USER</user>
            <WiegandCard>aa:bb:cc:dd</WiegandCard>
        </map>
        <map>
            <user>Toto</user>
            <WiegandCard>cc:dd:ee:ff</WiegandCard>
        </map>
    </user_mapping>
    <permissions>
        <map>
            <!-- Time frame mapping for MY_USER -->
            <user>MY_USER</user>
            <default_schedule>
                <!-- The scheduling information here *only* apply for door
                that have no other scheduling information.
                 In the test suite that use this file, its DoorC.
                 It is the default schedule FOR THIS USER.
                 -->
                <monday>
                    <start>00:00</start>
                    <end>23:59</end>
                </monday>
                <sunday>
                    <start>00:00</start>
                    <end>23:59</end>
                </sunday>
            </default_schedule>
            <schedule>
                <door>doorA</door>
                <monday>
                    <start>10:00</start>
                    <end>14:59</end>
                </monday>
                <monday>
                    <start>16:35</start>
                    <end>23:59</end>
                </monday>
            </schedule>
            <schedule>
                <door>doorB</door>
                <monday>
                    <start>15:00</start>
                    <end>23:59</end>
                </monday>
            </schedule>
        </map>
        <map>
            <!-- Time frame mapping for Toto -->
            <user>Toto</user>
            <default_schedule>
                <sunday>
                    <start>00:00</start>
                    <end>23:59</end>
                </sunday>
            </default_schedule>
        </map>
    </permissions>
</root>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Notes {#mod_auth_file_notes}
----------------------------

Check the [module object documentation](@ref Leosac::Module::Auth::AuthFileModule).
