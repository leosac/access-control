@page page_module_mqtt Module: MQTT

Mqtt Module Documentation {#mod_mqtt_main}
====================================================

@brief Publishing/Subscribing to MQTT brokers

[TOC]

Introduction {#mod_sysfsgpio_intro}
===================================

Welcome to the main documentation page for the MQTT module.

This module provides support for messages from/to MQTT brokers by allowing the
declaration of virtual devices (eg. gpio, rfidreader, ...) either IN (subscribe)
or OUT (publish).

Configuration Options {#mod_mqtt_user_config}
==================================================

Below are the configuration options available.

Options      | Options | Options             | Description                                                                             | Mandatory
-------------|---------|---------------------|-----------------------------------------------------------------------------------------|-----------
use_database |         |                     | If true, use database-backed configuration.                                             | NO (defaults to false)
servers      |         |                     | List of MQTT broker to connect to                                                       | **YES**
--->         | server  |                     | Configuration information for one MQTT broker                                           | **YES**
--->         | --->    | name                | Name of the broker                                                                      | **YES**
--->         | --->    | host                | Broker's hostname (default: localhost)                                                  | NO
--->         | --->    | port                | Broker's port (default: 1883)                                                           | NO
--->         | --->    | client_id           | Client name (default: leosac)                                                           | NO
--->         | --->    | subscribe_prefix    | Topic prefix when subscribing (default: homeassistant/)                                 | NO
--->         | --->    | publish_prefix      | Topic prefix when publishing (default: leosac/)                                         | NO
--->         | --->    | ssl                 | Option to enable SSL connection to broker (default: false)                              | NO
--->         | --->    | ssl_ca_certs        | CA certificate path when SSL is enabled                                                 | NO
--->         | --->    | ssl_client_certfile | Client certificate path when SSL is enabled with client cert authentication             | NO
--->         | --->    | ssl_client_keyfile  | Client key path when SSL is enabled with client cert authentication                     | NO
--->         | --->    | ssl_insecure        | Allow unsecure SSL, like expired certificate or hostname mismatch (default: false)      | NO
topics       |         |                     | List of MQTT topics to configure                                                        | **YES**
--->         | topic   |                     | Configuration information for one topic.                                                | **YES**
--->         | --->    | name                | Name of the MQTT topic for further internal reference                                   | **YES**
--->         | --->    | subject             | MQTT subject                                                                            | **YES**
--->         | --->    | direction           | Direction of the topic. This in either `subscribe` or `publish`                         | **YES**
--->         | --->    | virtualtype         | Virtual type to behave as on ZMQ (gpio, rfidreader, ...)                                | **YES**
--->         | --->    | payload             | MQTT message payload                                                                    | NO


Example {#mod_mqtt_example}
--------------------------------

This is a example of possible configuration for MQTT module into Leosac:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
<module>
  <name>MQTT</name>
  <file>libmqtt.so</file>
  <level>2</level>
  <module_config>            
    <servers>
      <server>
        <name>hass</name>
        <host>localhost</host>
        <port>1883</port>
        <client_id>leosac</client_id>
        <subscribe_prefix>homeassistant/</subscribe_prefix>
        <publish_prefix>leosac/</publish_prefix>
        <ssl>false</ssl>
      </server>
    </servers>
    <topics>
      <topic>
        <name>frontdoor</name>
        <subject>frontdoor</subject>
        <direction>publish</direction>
        <virtualtype>gpio</virtualtype>
        <payload>{ state: __PLACEHOLDER__ }</payload>
      </topic>
      <topic>
        <name>frontdoor_sensor</name>
        <subject>enocean/Front Door Sensor</subject>
        <direction>subscribe</direction>
        <virtualtype>gpio</virtualtype>
        <payload></payload>
      </topic>
      <topic>
        <name>basement_light</name>
        <subject>zigbee2mqtt/0x123456789abcdef</subject>
        <direction>subscribe</direction>
        <virtualtype>gpio</virtualtype>
        <payload>state</payload>
      </topic>
    </topics>
  </module_config>
</module>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
