@page page_audit Audit

Audit Documentation {#audit_main}
============================================

[TOC]

@brief Explain how the audit subsystem works.

Introduction {#audit_intro}
===========================
Each event worthing to be recorded and later analyzed will have an entry on the audit subsystem.
This includes Admin/System events like door/user/group/zone creation/update/deletion (eg. through the web interface)
and User events like Door beging opened, access being granted/denied as well.

This subsystem requires the [database engine](@ref general_database) to be enabled. Otherwise no audit entry is being recorded.

Event Type {#audit_eventtype}
=============================

See @ref Leosac::Audit::EventType "Event Type".
