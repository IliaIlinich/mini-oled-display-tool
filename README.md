Here is the updated README.md. I have rewritten the introduction to clarify that it now takes data via command-line arguments (which is great for bash scripts), added the new -c clear flag to the features list, formatted the terminal commands into proper code blocks, and unified the executable name (the previous version compiled it as oled_monitor but tried to launch it as oled_screen).
mini-oled-screen

A command-line utility that allows you to render text and system information on an I2C mini OLED screen. This tool takes data via command-line arguments, making it highly customizable and perfectly suited for integration with bash scripts, automation tools, or cron jobs.
Features

Currently, the tool allows you to pass and display the following data:

    Device Name / Title (-n or --name)

    IP Address (-i or --ip)

    Uptime (-u or --uptime)

    Date and Time (-d or --date)

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

Hardware Setup

Verify that the OLED screen is detected on the I2C bus (the default address for 128x64 displays is usually 0x3c):
Bash

    i2cdetect -y 1

Compilation

This project can be compiled with modern GCC, but it relies on C++11/C++14 standard features. Because legacy Arduino wrapper libraries handle different types of data differently, you must compile with the -fpermissive flag to downgrade type-conflict errors to warnings.
Bash

    g++ mini_oled_info.cpp -o oled_monitor -std=c++14 -fpermissive -lArduiPi_OLED -li2c

Launching the Monitor

To populate the screen, launch the application and provide the data as arguments.

View all options and help:
Bash

    sudo ./oled_monitor -h

Example usage with data:
Bash

    sudo ./oled_monitor -n "RaspberryPi" -i "192.168.1.50" -u "2d 4h 12m" -d "2026-08-15 14:30:00"

Clear the screen:
Bash

    sudo ./oled_monitor --clear
