@page page_guide_docker_cc Docker: Cross Compiling Container

Describes how to use the "cross_compile" helper container.
For ready to run Leosac docker images, see https://hub.docker.com/r/maxhy/leosac

Leosac comes with a container tailored for cross compiling Leosac to the Raspberry Pi.
The process of cross-compilation builds, then bundles, all dependencies.


Background
----------

This method trades simplicity for speed. If your idea of fun is not watching your Pi take hours to build Leosac, then you should consider this method to build Leosac.

The recommended way to interact with the cross-compile container is to use `leosaccli dev cc XXX`.
This tool must be built first, however, and the steps below explain how to do that.

The leosaccli tool is the swiss army knife of the Leosac project. It can peform many tasks.
Once the tool is built, run `leosaccli dev cc --help` to see what you can do with it.

When building the container, required libraries are built and installed into `/opt/rpi_fakeroot` inside the container.
Running the container will build an ARM version of Leosac.

To accomplish this, the container expects two or three volumes:
  + /leosac : the root of the Leosac repository (read-only).
  + /leosac_arm_build : Build output directory (read-write).
  + /ssh_deploy_key : An optional volume that points to a valid ssh private key.
    This is used by command `leosaccli dev cc dev-push` command.

The build will take place in `/leosac_arm_build`.

This will create a set of files in an aribtrary folder under /tmp. This folder will contain two files, `fakeroot.tar` and `LEOSAC-x.y.x.sh`, which should be copied to and then extracted on your RaspberryPi.


Prerequsites
------------

Prior to following the steps below, [Docker](https://docs.docker.com/get-started/) must be installed, and you must be able to run Docker as a non-root user.
Verify Docker is working currently by running the "Hello World" container as shown in the [Get-Started](https://docs.docker.com/get-started/) guide.


Full Example from Start to Finish
---------------------------------

Clone the Leosac repository:
  + `git clone https://github.com/leosac/leosac`
  + `cd leosac`
  + `git submodule init && git submodule update`

Clone the bin-resources repository:
  + `cd ..`
  + `git clone http://github.com/leosac/bin-resources`
  + `cp -r bin-resources/cross-compile-resources leosac/`

Build the leosaccli tool:
  + `cd leosac/python`
  + `sudo apt-get install python3-pip`
  + `sudo pip3 install -e .`

Build the cross-compile container:
  + `leosaccli dev docker build cross_compile`

@note Building the cross-compile container *does not* build Leosac. Instead, it builds and install its dependencies and required toolchain.

Cross-compile Leosac:
  + `leosaccli dev cc cmake`
  + `leosaccli dev cc fakeroot-package`
  + `leosaccli dev cc make`
  + `leosaccli dev cc package`

Deploy files to the target machine (Raspberry Pi):
  + `leosaccli dev cc dev-push`

@note Alternatively, you can manually copy the fakeroot.tar and LEOSAC-x.y.x.sh files to the target machine using any method convenient to you.

After copying the files to the target machine, log in and unpack the files:
  + `tar -xf fakeroot.tar -C /`
  + `sudo LEOSAC-x.y.x.sh --prefix=/usr/local` where x.y.x is the version of Leosac


Follow-Up Tasks
---------------

Leosac is now installed on your system, but there are couple of additional tasks you should perform.

Get the Leosac service file:
  + `https://raw.githubusercontent.com/leosac/leosac/develop/pkg/debian/leosac.service`

Edit the leosac service file, changing `/usr/bin/leosac` to `/usr/local/bin/leosac`, then copy it into place:
  + `vi/gedit leosac.service`
  + `sudo cp leosac.service /etc/systemd/system`

Finally, before you can start Leosac, you need to create a kernel.xml file. See the [installation guide](@ref page_guide_rpi_piface_wiegand).
