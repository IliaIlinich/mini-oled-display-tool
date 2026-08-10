# mini-oled-screen
This is a simple tool to work with a mini OLED screen to display system information and other contents on it

Currently the tool allows you to display the following:
- Device Name
- IP Address
- Uptime
- Date and Time

**Prerequisites** (tested on Raspberry Pi, please adapt the commands to the system you are using)
- Enable I2C
    `sudo raspi-config`, navigate to interfacing option and enable I2C
- Install Required System Packages
    `sudo apt-get update`
    `sudo apt-get install build-essential i2c-tools`
- Install the ArduiPi_OLED Library
    This project requires the `ArduiPi_OLED` library to be compiled and installed on your system. Ensure that the libraries like `ArduiPi_OLED.h` and
    `Adafruit_GFX.h` are located in the `/usr/local/include/` directory and the shared objects are in `/usr/local/lib/`
**Hardware Setup**
- Verify that the OLED screen in detected on the I2C bus (default for 128x64 displays is usually 0x3c)
    `sudo i2cdetect -y 1`
**Compilation**
    This project can be compiled with modern GCC, but it does rely on C++11/C++14 standard features, also due to legacy Arduino wrapper libraries handle different types of data, you must compile with the
    `-fpermissive` flag to downgrade type-conflict errors to warnings.
    `g++ mini_oled_info.cpp -o oled_monitor -std=c++14 -fpermissive -lArduiPi_OLED -li2c`
Launching the Monitor
    `sudo ./oled_monitor`
