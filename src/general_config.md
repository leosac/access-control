@page page_general_config General Configuration

@brief Global configuration option.

[TOC]

Introduction {#general_config_intro}
====================================

Even though Leosac is heavily based on modules, there are general
configuration options. Those are defined in the main configuration file,
outside of the `<modules>` and `</modules>` tags.

Here is an overview of what we call "general configuration":
  + Database configuration
  + Logger configuration
  + Network configuration
  + Remote control configuration.


Path Management {#general_config_path_mng}
==========================================

Leosac has to know 2 important paths:
  1. The path to the factory configuration directory: this can be set with the environment variable
   `LEOSAC_FACTORY_CONFIG_DIR`.
  2. Paths to where Leosac's shell scripts are saved. Customizable with `LEOSAC_SCRIPTS_DIR` env variable.


Instance Name {#general_instance_name}
==========================================

Each unit is given an `instance_name` to help identify it when running in a environment
with multiple Leosac system. This field is mandatory.
   1. This `instance_name` information is never synchronized.
   2. It is used to automatically prefix some configuration option: for example
   the `target` (door) in the auth file module. This allows easier config synchronization.


Database {#general_database}
============================

Leosac can work with a SQLite database. The database in mandatory to provide
support for some features. For example, the WebSocket API needs access to the
database to store users, permissions, etc.

The `database` tag holds various information about the database.

Options       | Options  | Description                                            | Mandatory
--------------|----------|--------------------------------------------------------|-----------
type          |          | Underlying database type. Either "sqlite" or "mysql".  | YES
path          |          | **SQLite only**: Path to the database file.            | YES if SQLite
username      |          | **MySQL only**: Database username.                     | YES if MySQL
password      |          | **MySQL only**: Password for the database user.        | YES if MySQL
dbname        |          | **MySQL only**: Database name to use.                  | YES if MySQL
host          |          | **MySQL only**: Database hostname / IP.                | NO
port          |          | **MySQL only**: Port the database listens to           | NO

Example {#database_example}
--------------------------
Below is an example for SQLite.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
<database>
    <type>SQLite</type>
    <path>leosac.sqlite</path>
</database>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

An one for MySQL / MariaDB.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
<database>
      <type>mysql</type>
      <username>root</username>
	  <password></password>
      <dbname>leosac</dbname>
</database>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Automatic Configuration Saving {#general_config_save}
=====================================================

There is a useful configuration option: `autosave`. When set to true,
the current configuration of Leosac will be saved to disk when Leosac exits.
It defaults to false.  
  
Logger Configuration {#general_config_logger}
=============================================

Leosac has a builtin logger for information and / or debugging purposes. It can write to syslog.
It is not recommended to log DEBUG message to syslog.


Configuration Options {#logger_user_config}
-------------------------------------------

Default configuration will log everything to `stdout`. Entry with 
a level >= `WARNING` will be written to syslog.

Options        | Description                                        | Mandatory
---------------|----------------------------------------------------|-----------
enable_syslog  | Enable logging to syslog                           | NO (default to `true`)
enable_database| Enable logging to the configured (if any) database.| NO (default to `false`)
min_syslog     | Minimal log entry level to write to syslog         | NO (default to `WARNING`)

Here is a list of the various log level available:
   + `DEBUG`
   + `INFO`
   + `NOTICE`
   + `WARNING`
   + `ERROR`
   + `CRITICAL`
   + `ALERT`
   + `EMERGENCY`

Example {#logger_example}
-------------------------

Logger configuration takes place in the `<kernel>` tag. It is **not** configured
as a module.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
<log>
    <enable_syslog>true</enable_syslog>
    <min_syslog>INFO</min_syslog>
</log>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


Network Configuration {#general_config_network}
================================================

Leosac can provide an abstraction to network configuration.
You can configure your IP address, netmask and gateway in Leosac's configuration file.

This allows the Remote Control API (or Rpleth API) to change the network configuration
of the Leosac unit.

However, if you do not plan on changing your IP address or see no use for this feature,
it is recommended to use Linux tools to configure your network instead (ie, write to `/etc/network/interfaces`).

@note Disabling the network subsystem (with `<enabled>false</enabled>`) doesn't disable the network. It disables
the network management that Leosac does. If you want to fully disable the network, you'll have to do this
at the operating system level.

@note CIDR notation is not supported (see [#54](https://github.com/leosac/leosac/issues/54)).

Example {#network_example}
--------------------------
Network configuration takes place in the `<kernel>` tag. It is **not** configured
as a module.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
<network>
    <enabled>false</enabled>
</network>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

An other example with enabled network management.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
<network>
    <enabled>true</enabled>
    <interface>eth0</interface>
    <dhcp>true</dhcp>
    <netmask>255.255.255.0</netmask>
    <default_ip>192.168.0.18</default_ip>
    <gateway>192.168.0.1</gateway>
</network>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Remote Control {#remote_control}
================================

The Remote Control API is more complicated than logger or network configuration,
and require a more in depth description. You can find it [here](@ref remote_control_main).

Sync Source {#sync_source}
==========================

From all these configuration options, you may want to limit which one gets
exported over the network when a remote command to fetch the global configuration is received.

If no `<sync_source>` is present, export everything.
Here is a list of exportable config tag:
 + `<autosave>`
 + `<plugin_directories>`
 + `<log>`
 + `<network>`
 + `<remote>`
 + `<sync_dest>`

Note that the `no_import` tag cannot be exported.

Example {#sync_source_ex}
-------------------------

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
<sync_source>
    <log>true</log>
    <network>false</network>
    <remote>false</remote>
</sync_source>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Sync Dest {#sync_dest}
======================

This is similar to `sync_source` but is on the receiver side.
The tag set to true in `sync_dest` are the tag that will be updated with the content from the remote server.
The rules are the same than `sync_source`.

Non Importable Module {#non_importable_module}
===============================================

By default all modules are synchronised when syncing configuration - be it when the Replication module
runs, or when the server receive the `SYNC_FROM` remote control command.
It's however possible to prevent module configuration from being added, erased or updated.

To do so, define a `<no_import>` tag in the config file.

**Note**: The `<no_import>` tag will **NEVER** be synchronized from a remote server.

*Security notice:* This is because it is not exported, not because we carefully ignore it when importing.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
<no_import>
    <TEST_AND_RESET />
    <ANOTHER_MODULE />
</no_import>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This will have the effect of leaving alone the configuration of Test and Reset and Another Module.
It will not prevent the module config from being exported.

Version {#version}
==================

A `version` field is configurable. This is mostly used when running a master/slave
configuration.
If you make a change to the configuration, and increments this `version` field,
slave should automatically update their configuration.
