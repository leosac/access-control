@page page_module_smtp Module: SMTP

SMTP {#mod_SMTP_main}
=====================

@brief A SMTP module that provide the ability to send mail.

[TOC]

Introduction {#mod_SMTP_intro}
==============================

The SMTP module is used to send email. It supports either an XML
based configuration, or can rely on the database for configuration storage.

The SMTP module watch the application event bus, specifically topic `SERVICE.MAILER`.

When a component wishes to send a email, it can send a message to the application bus,
with topic `SERVER.MAILER`. The message contain shall be a string representing a key
in the `GlobalRegistry`: this key is then used to retrieve the `MailInfo` object.
 
 
Configuration Options {#mod_SMTP_user_config}
=============================================

Options        | Options  | Options         | Description                                                    | Mandatory
---------------|----------|-----------------|----------------------------------------------------------------|-----------
want_ssl       |          |                 | Do we enable the SSL engine ?                                  | NO (defaults to `true`)
use_database   |          |                 | If true, rely on database for configuration. All XML config is ignored | NO (defaults to `false`)
servers        |          |                 | Remote mail service to use                                     | NO
--->           | server   |                 | Describe a mail server target.                                 | NO
--->           | --->     | url             | SMTP server url. "smtp://mail.mydomain.me"                     | YES
--->           | --->     | username        | Username to use for authentication against the SMTP server     | NO
--->           | --->     | password        | Password for authentication                                    | NO
--->           | ---->    | ca_file         | Path to a PEM encoded CA file used to validate certificate.    | NO
--->           | --->     | verify_host     | If SSL is enabled, do we verify the host name in the SSL certificate ? | NO (defaults to `true`)   
--->           | --->     | verify_peer     | If SSL is enabled, do we verify the SSL certificate ? | NO (defaults to `true`)   


Example {#mod_SMTP_example}
---------------------------

### With database {#mod_SMTP_example_db}

In this example, the module will load its configuration from the SQL database,
and will be configurable through the websocket API (if the websocket module is enabled).

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
<module>
    <name>SMTP</name>
    <file>libsmtp.so</file>
    <level>1</level>

    <module_config>
      <use_database>true</use_database>
    </module_config>
</module>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


### Without database {#mod_SMTP_example_nodb}

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
<module>
    <name>SMTP</name>                                          
    <file>libsmtp.so</file>                                    
    <level>1</level>

    <module_config>
      <servers>
        <server>
          <url>smtp://mail.mydomain.com</url>
          <username>my_mail_user</username>
          <password>my_mail_password</password>
          <from>leosac@mydomain.com</from>
        </server>
      </servers>
    </module_config>
</module>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
