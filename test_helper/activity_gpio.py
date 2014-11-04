#!/usr/bin/python
#
# Simulate activity on Raspberry's GPIO pins by turning them on and off
#

## ret the "direction" file for wiegand's low output
def get_low():
    return open("/sys/class/gpio/gpio14/direction", "w")

## ret the "direction" file for wiegand's high output
def get_high():
    return open("/sys/class/gpio/gpio15/direction", "w")

def toggle_lo(count = 1):
    for i in range(0, count):
        get_low().write("out")
        get_low().write("in")

def toggle_hi(count = 1):
    for i in range(0, count):
        get_high().write("out")
        get_high().write("in")

## Will simulate GPIO activity so it looks like Wiegand's reader sent
## card with id {56:bb:28:c5} (which is valid in the default auth file)
def write_valid_card():
    #byte 4
    toggle_hi(2)
    toggle_lo(3)
    toggle_hi()
    toggle_lo()
    toggle_hi()
    #byte 3
    toggle_lo(2)
    toggle_hi()
    toggle_lo()
    toggle_hi()
    toggle_lo(3)
    #byte 2
    toggle_hi()
    toggle_lo()
    toggle_hi(3)
    toggle_lo()
    toggle_hi(2)
    #byte 1
    toggle_lo()
    toggle_hi()
    toggle_lo()
    toggle_hi()
    toggle_lo()
    toggle_hi(2)
    toggle_lo()

def main():
    write_valid_card()
    

if __name__ == "__main__":
    main()
