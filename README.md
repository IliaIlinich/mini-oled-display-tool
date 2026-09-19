# OLED Monitor Tool

A small utility for displaying system information on a mini OLED screen via I2C on a Raspberry Pi.

## Features

- Display custom hostname / device name
- Show IP address
- Visual indicators for uptime, disk usage, and other system stats
- Supports both dynamic and static builds

## Requirements

- Raspberry Pi running Raspberry Pi OS (32-bit or 64-bit)
- I2C enabled
- `ArduiPi_OLED` library
- `libi2c-dev`

## Installation & Build Instructions

This project requires the `ArduiPi_OLED` library to interface with the display. If you are running a modern 64-bit Raspberry Pi OS, the default library configuration will fail to compile. Follow the steps below to patch the library and build this tool.

### 1. Install Prerequisites

Install the required I2C development library:

```bash
sudo apt-get update
sudo apt-get install libi2c-dev
```

### 2. Download and Configure ArduiPi_OLED

Clone the library and run the automated setup script. When prompted, select your platform (e.g., `1` for Raspberry Pi).

```bash
git clone https://github.com/hallard/ArduiPi_OLED.git
cd ArduiPi_OLED
sudo ./autogen.sh
```

### 3. Patch the Makefile (64-bit OS only)

The default Makefile includes legacy 32-bit hardware floating-point flags that cause compilation errors on modern 64-bit systems.

Open the Makefile:

```bash
nano Makefile
```

Find the `CCFLAGS` line (around line 30) and replace it with:

```makefile
CCFLAGS=-Wall -fPIC -fno-rtti -Ofast
```

This removes the unsupported architecture-specific flags.

### 4. Build and Install the Library

Compile the shared library and update the system's linker cache:

```bash
sudo make
sudo ldconfig
```

#### Optional: create a static archive

If you intend to build a static executable later, bundle the compiled object files into a static library:

```bash
sudo ar rcs /usr/local/lib/libArduiPi_OLED.a *.o
```

### 5. Compile the Monitor Tool

Navigate back to your project directory. Choose one of the build options below.

#### Option A: Dynamic linking (default)

Produces a smaller binary that relies on system libraries.

```bash
g++ mini_oled_info.cpp -o oled_monitor -std=c++14 -fpermissive -L/usr/local/lib -lArduiPi_OLED -li2c
```

#### Option B: Static linking (standalone executable)

Produces a larger binary with no external library dependencies. Requires the static archive from Step 4.

```bash
g++ mini_oled_info.cpp -o oled_monitor -std=c++14 -fpermissive -static -L/usr/local/lib -lArduiPi_OLED -li2c
```

## Usage

### Show help

```bash
./oled_monitor -h
```

### Example command

```bash
sudo ./oled_monitor -n "name" -i "192.168.0.1" -u -d -s
```

### Common flags

| Flag | Description |
|---|---|
| `-h` | Show help |
| `-n` | Display name / hostname |
| `-i` | IP address to show |
| `-u` | Show uptime |
| `-d` | Show disk usage |
| `-s` | Show additional system info |

## Troubleshooting

### Compilation fails on 64-bit Raspberry Pi OS

Make sure you completed Step 3 and removed the `-mfloat-abi=hard -marm` flags from `CCFLAGS`.

### `cannot find -lArduiPi_OLED`

Run `sudo ldconfig` again and verify the library is installed:

```bash
ls /usr/local/lib/libArduiPi_OLED*
```

### Permission denied on I2C bus

Run the tool with `sudo`, or add your user to the `i2c` group and log out:

```bash
sudo usermod -a -G i2c $USER
```

## License

Add your license here.
