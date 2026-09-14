## Installation & Build Instructions

This project requires the `ArduiPi_OLED` library to interface with the display. If you are running a modern 64-bit Raspberry Pi OS, the default library configuration will fail to compile. Follow the steps below to patch the library and build this tool.

### 1. Install Prerequisites
First, install the required I2C development library:
```bash
sudo apt-get update
sudo apt-get install libi2c-dev
```
### 2. Download and Configure ArduiPi_OLED
Clone the library and run the automated setup script. When prompted, select your platform (e.g., 1 for Raspberry Pi).
```bash
git clone [https://github.com/hallard/ArduiPi_OLED.git](https://github.com/hallard/ArduiPi_OLED.git)
cd ArduiPi_OLED
sudo ./autogen.sh
```
### 3. Patch the Makefile (For 64-bit OS)
The default Makefile includes legacy 32-bit hardware floating-point flags that will cause compilation errors (unrecognized command-line option) on modern 64-bit systems.
Open the Makefile:
```Bash
nano Makefile
```
Find the CCFLAGS line (around line 30).
Delete the architecture-specific flags so it reads like this:
CCFLAGS=-Wall -fPIC -fno-rtti -Ofast

### 4. Build and Install the Library
Compile the shared library and update your system's linker cache:
```Bash
sudo make
sudo ldconfig
```
(Optional) If you intend to build a static executable later, you must manually bundle the compiled object files into a static archive:
```Bash
sudo ar rcs /usr/local/lib/libArduiPi_OLED.a *.o
```
### 5. Compile the Monitor Tool
Navigate back to your project directory. You can choose to build the binary dynamically (smaller file, relies on system libraries) or statically (larger file, highly portable).

Option A: Dynamic Linking (Default)
```Bash
g++ mini_oled_info.cpp -o oled_monitor -std=c++14 -fpermissive -L/usr/local/lib -lArduiPi_OLED -li2c
```

Option B: Static Linking (Standalone executable)
Requires the ar rcs command from Step 4 to be completed first.
```Bash
g++ mini_oled_info.cpp -o oled_monitor -std=c++14 -fpermissive -static -L/usr/local/lib -lArduiPi_OLED -li2c
```

Hopefully this will help somebody not to get stuck with figuring out why it doesn't compile for hours.
### Example of usage
Help
```Bash
./oled_monitor -h
```
Command example
```Bash
sudo ./oled_monitor -n "name" -i "192.168.0.1" -u -d -s
```
