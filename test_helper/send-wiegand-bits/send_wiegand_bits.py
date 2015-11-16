import sys
from InstrumentationClient import InstrumentationClient


def usage():
    print("./send_wiegand_bits GPIO_LOW GPIO_HIGH bits")
    print("The program will trigger interrupts. If the bit is zero, GPIO_LOW will trigger, otherwise"
          "GPIO_HIGH will trigger.")


def main():
    if len(sys.argv) != 4:
        return usage()
    gpio_low = sys.argv[1]
    gpio_high = sys.argv[2]
    ic = InstrumentationClient("/tmp/leosac-ipc")

    for bit in sys.argv[3]:
        if bit == '0':
            ic.send_interrupt(gpio_low)
        elif bit == '1':
            ic.send_interrupt(gpio_high)
        else:
            raise ValueError("Invalid bit value. Must be '0' or '1'.")


if __name__ == "__main__":
    main()
