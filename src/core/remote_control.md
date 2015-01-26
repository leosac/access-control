Remote Control Documentation {#remote_control_main}
====================================================

[TOC]

Introduction {#remote_control_intro}
====================================

Leosac has a built-in remote control functionality. This means two things:
 1. The user can interact remotely with Leosac.
 2. Leosac installation can talk to each other.

We aim to provide this functionality in a secure fashion. This means all connections to the
remote control socket will be encrypted. We use the CURVE security mechanism from ZeroMQ to
reach a good level of security. This security mechanism both prevent eavesdropping and man in the 
middle attack. 
Note that this an asymmetric security mechanism and require the use of both a private (secret) and public key.

Currently what you can do with remote control is rather limited, and is described in a later
section of this document.
  
We will now present the configuration of the remote control feature.

**Limitations**: There is no authentication of incoming connection yet. Everyone that manages to connect
 to the remote control socket is allowed to perform ALL available operations.

**Limitations**: When using the Remote Control interface to talk to each other, Leosac units
 must use the same underlying `boost::serialization` library. Otherwise serialization failure
 will arise.

Configuration Options {#remote_control_user_config}
===================================================

If no `<remote>` tag is defined in the configuration, the remote control interface
will be disabled.


Options       | Options  | Options | Description                                      | Mandatory
--------------|----------|---------|--------------------------------------------------|-----------
port          |          |         | Port to bind the remote control interface to     | YES
secret_key    |          |         | Z85 encoded secret key                           | YES
public_key    |          |         | Z85 encoded public key                           | YES


Example {#remote_control_example}
---------------------------------

Remote Control configuration takes place in the `<kernel>` tag. It is **not** configured
as a module.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
    <remote>
        <port>12345</port>
        <secret_key>Ygk2EVo#hr*uTG=U[jFFWrb9HDW-V?388=kj)AUz</secret_key>
        <public_key>TJz$:^DbZvFN@wv/ct&[Su6Nnu6w!fMGHEcIttyT</public_key>
    </remote>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Exposed features {#remote_control_features}
===========================================

The set of feature available through the remote control API is rather thin.

You can retrieve some information about the running configuration of Leosac:
+ The MODULE_LIST command will return the list of the module currently loaded on the target Leosac unit.
+ The MODULE_CONFIG command retrieve the configuration for one given module. 
+ The SYNC_FROM command order Leosac to fetch and apply the configuration of an other, remote, Leosac unit.

See below for a detailed description of messages.

MODULE_LIST {#remote_control_cmd_module_list}
---------------------------------------------

From Client to Server:

Frame    | Content                                       | Type
---------|-----------------------------------------------|-------------------------------------------------------------
1        | "MODULE_LIST"                                 | `string`

The response, from Server to Client:

Frame    | Content                                       | Type
---------|-----------------------------------------------|-------------------------------------------------------------
1        | "WIEGAND_READER"                              | `string`
2        | "MONITOR"                                     | `string`
3        | "AN_OTHER_MODULE"                             | `string`

One module name per frame. As many frames as needed. Note that module names are standard and hardcoded in the
module code.

MODULE_CONFIG {#remote_control_cmd_module_config}
--------------------------------------------------

The `MODULE_CONFIG` is allowed to fail. It's possible you request the configuration of a module
that is not loaded on the Leosac server.

From Client to Server:

Frame    | Content                                        | Type
---------|------------------------------------------------|-------------------------------------------------------------
1        | "MODULE_CONFIG"                                | `string`
2        | "MODULE_NAME"                                  | `string`
3        | Configuration Type (boost text archive or xml) | `uint8_t`

From Server to Client, in case everything went well.

Frame    | Content                               | Type
---------|---------------------------------------|-------------------------------------------------------------
1        | "OK"                                  | `string`
2        | "MODULE_NAME"                         | `string`
3        | Configuration Tree                    | Represented as `string`, this is a Boost text archive of the configuration ptree for the module.
4        | "filename_1"                          | `string`. This field is optional, its here in case the module has additional configuration
5        | "content_of_filename_1"               | `string`. This field is optional, its here in case the module has additional configuration

In case something went wrong, here is the response the server send to the client.

Frame    | Content                               | Type
---------|---------------------------------------|-------------------------------------------------------------
1        | "KO"                                  | `string`
2        | Reason                                | `string`


SYNC_FROM {#remote_control_cmd_sync_from}
-----------------------------------------

The `SYNC_FROM` command allows Leosac to replicate the configuration of an other Leosac unit.

**SECURITY CONSIDERATION**: A module can have additional configuration file. In order to correctly replicate
a configuration, we need to copy (and paste) those files too. Leosac will do that. That means that it will
**blindly write the content of a received file where it is told to**. That means that syncing from a 
malicious unit can cause to overwrite **any** file in your filesystem (if Leosac runs as root).

From Client to Server:

Frame    | Content                                      | Type
---------|----------------------------------------------|---------------------------------
1        | "SYNC_FROM"                                  | `string`
2        | The endpoint to sync from: "tcp://IP:PORT"   | `string`
3        | Autocommit (aka writing conf to disk) 1 | 0  | `uint8_t`

If the 3rd frame has a value of `1`, we will write the new configuration to disk as soon as the
synchronisation is done.


From Server to Client (if **no error occurred**):

Frame    | Content                  | Type
---------|--------------------------|------------
1        | "OK"                     | `string`

From Server to Client (in case **something went wrong**):

Frame    | Content                               | Type
---------|---------------------------------------|--------------------------
1        | "KO"                                  | `string`
2        | Reason                                | `string`

SAVE {#remote_control_save}
---------------------------

When `SYNC_FROM` configuration, the new configuration is apply directly. However,
it is not persisted to disk. Moreover, it will not be persisted to disk, unless
the `<autosave>BOOLEAN</autosave>` configuration option is set to `true`.

The `SAVE` command will make Leosac save its current configuration to disk.

From Client to Server:

Frame    | Content                                 | Type
---------|-----------------------------------------|-------------------
1        | "SAVE"                                  | `string`


From Server to Client:

Frame    | Content                         | Type
---------|---------------------------------|------------
1        | "OK"                            | `string`
