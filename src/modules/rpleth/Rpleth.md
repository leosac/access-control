Rpleth Module Documentation {#mod_rpleth_main}
==============================================

[TOC]

@brief Documentation for how to use the Rpleth module.

Introduction {#mod_rpleth_intro}
================================

Welcome to the main documentation page for the Rpleth module.
This page is about Rpleth from an end-user point of view. \n
It describes how to configure the Rpleth module for Leosac.

This module is for internal use at Islog, it's unlikely you will ever need it.

Configuration Options {#mod_rpleth_user_config}
===============================================

Below are the configuration options available.

Options    | Description                                            | Mandatory
-----------|--------------------------------------------------------|-----------
port       | Which port should we bind to ?                         | YES
reader     | Name of the wiegand reader the module handles          | YES
stream_mode| If enabled, should forward to client all card read     | NO (default to true)

Example {#mod_rpleth_example}
-----------------------------

This is a example of Rpleth possible configuration for Rpleth module into Leosac:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
<module>
    <name>RPLETH</name>
    <file>librpleth.so</file>
    <level>100</level>
    <module_config>
        <port>4242</port>
        <reader>MY_WIEGAND_1</reader>
        <stream_mode>true</stream_mode>
    </module_config>
</module>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            
Notes {#mod_rpleth_notes}
=========================

Technical details of implementations is [here](@ref Leosac::Module::Rpleth).
