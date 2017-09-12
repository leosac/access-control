@page page_dev_database_versioning Database Versioning


About this document
-------------------

A quick explanation about database versioning in Leosac.


Introduction
------------

Database version is a complicated tasks. We rely on our
Object Relational Mapper, [ODB], to provide infrastructure to
support database versioning.


[ODB] maintains a changelog of the database structure. This changelog
is committed in the repository and is required to properly track
change and support automatic database migration.

[ODB] comes with per-schema versioning, which allows use to make use of
powerful versioning strategy.


Leosac Database Schemas
-----------------------

[ODB] comes with per-schema versioning.
This means the same database can have multiple versions at the same.

Leosac makes use of this, by defining multiple schemas:
  1. The `core` schema is managed by the core Leosac program. It 
     includes `Users`, `Groups`, `Credentials`, and a lot of user object. \n
     It's the main schema, and can only evolves between Leosac release.
  2. Each module that is database-aware **must** define and use its own
     schema. A module should name its schema `module-MODULE_NAME`.        \n
     The version of module's schema can change between module release. It
     is the responsibility of the module developer to properly version his
     module.


At the code level
-----------------

The `database.hpp` uses a `pragma db model version(x, y)` so that we can
define Leosac's `core` schema version.

It is important for module code to define `ODB_NO_BASE_VERSION` before including
any file that could include `database.hpp`.
Defining `ODB_NO_BASE_VERSION` will prevent the use of `pragma db model version(x, y)`
and will allows module code to pick it's own version.

[ODB]: http://www.codesynthesis.com/products/odb/doc/manual.xhtml
[ODB Database Schema Evolution]: http://www.codesynthesis.com/products/odb/doc/manual.xhtml#13
