@page page_guide_docker_cc Docker: Cross Compiling Container

Describes how to use the "cross_compile" helper container.
 

Leosac comes with a container tailored for cross compiling Leosac to
RaspberryPi.
The process of cross-compilation bundles dependencies and build them.

How it works
-------------

The container must first be built. Building the container *does not* build
Leosac. Instead it builds and install dependencies and required toolchain.

The container expects two volumes:
  + /leosac : the root of the Leosac repository.
  + /leosac_arm_build : An optional build directory.
  

How to use
-----------

Run `leosaccli dev cc --help` to see what you can do.
