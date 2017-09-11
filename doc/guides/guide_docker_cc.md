@page page_guide_docker_cc Docker: Cross Compiling Container

Describes how to use the "cross_compile" helper container.
 

Leosac comes with a container tailored for cross compiling Leosac to
RaspberryPi.
The process of cross-compilation bundles dependencies and build them.

Requirements
------------

The container must first be built. Building the container *does not* build
Leosac. Instead it builds and install dependencies and required toolchain.

You must first build the "cross_compile" container before cross-compiling
Leosac. The easiest way to do: `leosaccli dev docker build cross_compile`.

How it works
-------------

When building the container, required library are built and installed
into `/opt/rpi_fakeroot` in the container.

Running the container will build an ARM version Leosac. For this, the container
expects two or three volumes:
  + /leosac : the root of the Leosac repository (read-only).
  + /leosac_arm_build : Build output directory (read-write).
  + /ssh_deploy_key : An optional volume that points to a valid ssh private key.
    This is used by command `leosaccli dev cc dev-push` command.
  
The build will take place in `/leosac_arm_build`.

How to use
-----------

The recommended way to interact with the cross-compile container is to use
`leosaccli dev cc XXX`. 

Run `leosaccli dev cc --help` to see what you can do.

Example
-------

Run the following:
  + `leosaccli dev cc cmake`
  + `leosaccli dev cc fakeroot-package`
  + `leosaccli dev cc make`
  + `leosaccli dev cc package`
  
This will give you a `fakeroot.tar` that you should extract on your 
RaspberryPi (at `/opt/rpi_fakeroot`). Similarly, you should upload your `LEOSAC-x.y.x.sh` and
install it on the raspberry.

Assuming you extract `fakeroot.tar` to `/opt/rpi_fakeroot`.
Now, to run Leosac, you must export `LD_LIBRARY_PATH` so that it points
to `/opt/rpi_fakeroot/lib`.
