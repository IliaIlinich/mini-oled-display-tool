A command-line utility that allows you to render text and system information on an I2C mini OLED screen. This tool takes data via command-line arguments, making it highly customizable and perfectly suited for integration with bash scripts, automation tools, or cron jobs.
Features

Currently, the tool allows you to pass and display the following data:

    Device Name / Title (-n or --name)

    IP Address (-i or --ip)

    Uptime (-u or --uptime)

    Date and Time (-d or --date)

    Total and Free Space (-s or --space)

    Clear Screen (-c or --clear) to wipe the display buffer and turn off active pixels.

Prerequisites

(Tested on Raspberry Pi. Please adapt the commands to the system you are using)

    Enable I2C
    Run sudo raspi-config, navigate to Interface Options, and enable I2C.

    Install Required System Packages
    Bash

    sudo apt-get update
    sudo apt-get install build-essential i2c-tools

    Install the ArduiPi_OLED Library
    This project requires the ArduiPi_OLED library to be compiled and installed on your system. Ensure that header files like ArduiPi_OLED.h and Adafruit_GFX.h are located in /usr/local/include/ and the shared objects are in /usr/local/lib/.
    First please install the library.
    git clone https://github.com/hallard/ArduiPi_OLED.git
    Then go through the automated setup.
    cd ArduiPi_OLED
    sudo ./autogen.sh
    And then compile and install
    sudo make
    sudo make install

Hardware Setup

Verify that the OLED screen is detected on the I2C bus (the default address for 128x64 displays is usually 0x3c):
Bash

    i2cdetect -y 1

Compilation

This project can be compiled with modern GCC, but it relies on C++11/C++14 standard features. Because legacy Arduino wrapper libraries handle different types of data differently, you must compile with the -fpermissive flag to downgrade type-conflict errors to warnings.
Bash

    Please, when compiling the C++ file, ensure it is in the same folder as the Adafruit_GFX.h in case of a problem with the library not being in /usr/include.
    g++ mini_oled_info.cpp -o oled_monitor -std=c++14 -fpermissive -I. -lArduiPi_OLED -li2c
    ^
    |
    This will generate a non dynamic executable, so you wouldn't need to install any of the services on another machine and can just download the binary.

Launching the Monitor

To populate the screen, launch the application and provide the data as arguments.

View all options and help:
Bash

    sudo ./oled_monitor -h

Example usage with data:
Bash

    sudo ./oled_monitor -n "RaspberryPi" -i "192.168.1.50"

Clear the screen:
Bash

    sudo ./oled_monitor --clear
