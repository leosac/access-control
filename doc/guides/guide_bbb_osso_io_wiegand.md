@page page_guide_bbb_osso_io_wiegand Installation Guide (part 2): BeagleBone Black + Osso I/O cape board + Wiegand Reader

Installation guide {#install_guide_osso}
=====================================

@brief Describe the additional installation process required with such hardware. You should have first followed @ref page_guide_raspbian.

Additional Hardware {#install_guide_osso_req_hard}
----------------------------------

1. A BeagleBone Black
2. An Osso I/O cape board (see [here](https://git.nexlab.net/nexlab/Osso))

Adding modules {#install_guide_osso_add_mod}
=======================================

We need a few modules to make thing works:
+ Relays and digital inputs from Ossoe board can be accessed through the system GPIOs, we then need Leosac's [SysFS GPIO](@ref mod_sysfsgpio_main) module.


Configuring the SysFS GPIOS module {#install_guide_osso_add_mod_sysfsgpio}
---------------------------------------------------------------------

You can find the documentation of the module [here](@ref mod_sysfsgpio_main). It focus on how to configure
the module for any use case. We will instead explain why we need a given configuration.

We are using a Wiegand card reader. Therefore we need 2 INPUTs GPIO.
We also have 1 OUTPUT gpio for the door.

![BeagleBoard Black pinmap](beaglebone_black_pinmap.png)

We add this to our config file:

~~~~~~~~~~~~~~~~~~~.xml
<module>
    <name>SYSFS-GPIO</name>
    <file>libsysfsgpio.so</file>
    <level>2</level>
    <module_config>
       <aliases>
			<default>gpio__NO__</default>
		</aliases>
		<export_path>/sys/class/gpio/export</export_path>
		<unexport_path>/sys/class/gpio/unexport</unexport_path>
		<edge_path>/sys/class/gpio/__PLACEHOLDER__/edge</edge_path>
		<value_path>/sys/class/gpio/__PLACEHOLDER__/value</value_path>
		<direction_path>/sys/class/gpio/__PLACEHOLDER__/direction</direction_path>                
		<gpios>
			<gpio>
				<name>wiegand_data_high</name>
				<no>47</no>
				<direction>in</direction>
				<interrupt_mode>falling</interrupt_mode>
			</gpio>
			<gpio>
				<name>wiegand_data_low</name>
				<no>45</no>
				<direction>in</direction>
				<interrupt_mode>falling</interrupt_mode>
			</gpio>
			<gpio>
				<name>my_door_gpio</name>
				<no>60</no>
				<direction>out</direction>
				<value>0</value>
			</gpio>
		</gpios>
    </module_config>
</module>
~~~~~~~~~~~~~~~~~~~

As you can see, we are defining GPIO using the gpio port `<no>` we wired our reader and door.

Note that we use `<level>2</level>` to make sure this module is one of the first to start.
We are assigning meaningful name to our GPIO. Keep them in mind because we will reuse them later.

Wiring everything {#install_guide_osso_wiring}
=========================================

This is the BBB with the Osso board plug onto it.
![BeagleBoard and board](bbb_relay.jpg)

The expansion cape has to be powered by a 12VDC connector (9 to 24VDC supported), and it will power the Beagleboard
from expansion pins ( no needs to give external power from usb or power connector on the beaglebone! )
