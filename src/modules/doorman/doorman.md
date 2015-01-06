Doorman Module Documentation {#mod_doorman_main}
================================================

[TOC]

Introduction {#mod_doorman_intro}
=================================

The Doorman module is very important to Leosac's usability.
It is the module responsible for reacting to messages sent by authentication / authorization modules.

If an access is granted (or denied) one (or more) actions can be taken by this module:
+ Open the door.
+ Raise an alarm.
+ ...

Configuration Options {#mod_doorman_user_config}
================================================

Options    | Options   | Options         | Options      | Options     | Description                                            | Mandatory
-----------|-----------|-----------------|--------------|-------------|--------------------------------------------------------|-----------
instances  |           |                 |              |             | Multiples (unrelated) doorman instance are allowed     | YES
----->     | instance  |                 |              |             | Configuration information for 1 instance               | YES
----->     | ----->    | name            |              |             | Name of this doorman instance                          | YES
----->     | ----->    | auth_contexts   |              |             | We can use multiple auth context (not recommended yet) | YES
----->     | ----->    | ------>         | auth_context |             | Information for an Auth Context                        | YES
----->     | ----->    | ------>         | ----->       | name        | Name of the auth context we are using                  | YES
----->     | ----->    | timeout         |              |             | When using multiple auth context (not recommended yet) | YES
----->     | ----->    | actions         |              |             | Configure the actions to take when something happens   | YES
----->     | ----->    | ----->          | action       |             | Configuration information for one action               | YES
----->     | ----->    | ----->          | ----->       | on          | When should the action be taken (DENIED / GRANTED)     | YES
----->     | ----->    | ----->          | ----->       | target      | Name of the targeted object that will receive the action command | YES
----->     | ----->    | ----->          | ----->       | cmd         | Description for the command that will be sent | YES

The `<cmd>` tag is quite simple. It looks like this:

~~~~~~~~~~~~~~~~~~~~~~~.xml
<cmd>
    <f1>GREEN_LED</f1>
    <f2>ON</f2>
    <f3>1500</f3>
</cmd>
~~~~~~~~~~~~~~~~~~~~~~~

For each frame you want in the command message, use `<fX>` where `X` is growing for each frame you want.

Example {#mod_doorman_example}
------------------------------

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
  <module>
            <!-- Doorman module: react to authentication broadcast -->
            <name>DOORMAN</name>
            <file>libdoorman.so</file>
            <level>50</level>
            <module_config>
                <instances>
                    <instance>
                        <name>DOORMAN_JARVIS_1</name>
                        <auth_contexts>
                            <auth_context>
                                <name>AUTH_CONTEXT_1</name>
                            </auth_context>
                        </auth_contexts>
                        <timeout>1000</timeout>
                        <actions>
                            <action>
                                <on>DENIED</on>
                                <target>wiegand_buzzer</target>
                                <cmd>
                                    <f1>ON</f1>
                                    <f2>500</f2>
                                </cmd>
                            </action>
                            <action>
                                <on>GRANTED</on>
                                <target>MY_WIEGAND_1</target>
                                <cmd>
                                    <f1>GREEN_LED</f1>
                                    <f2>ON</f2>
                                    <f3>1500</f3>
                                </cmd>
                            </action>
                        </actions>
                    </instance>
                </instances>
            </module_config>
        </module>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
