@page page_module_event_publish Module: Event Publish

EventPublish documentation {#mod_event_publish_main}
=====================================================

@brief Publish wiegand auth event over a ZMQ publisher socket.

[TOC]

Introduction {#mod_event_publish_intro}
=======================================

This module is very simple. It listens to events from the Wiegand module
and publish them on a ZMQ publisher socket.

Currently, only auth source event of type `Leosac::Auth::SourceType::SIMPLE_WIEGAND` are
handled, all the other are simply ignored.

Configuration Options {#mod_event_publish_user_config}
======================================================

Options        | Options           | Description                                               | Mandatory
---------------|-------------------|-----------------------------------------------------------|-----------
sources        |                   | Multiples message source (wiegand reader)                 | YES
--->           | source            | Name of one reader to watch for event                     | YES
publish_source |                   | Append the name of source after the card id               | NO (default to false)
port           |                   | What port should the publisher bind to ?                  | YES 
