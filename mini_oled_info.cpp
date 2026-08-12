#include <cstdint>
typedef uint8_t boolean;

#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <string>
#include <cstring>
#include <cstdio>
#include <csignal>

#include <Adafruit_GFX.h>
#include <ArduiPi_OLED.h>
#include <ArduiPi_OLED_lib.h>

ArduiPi_OLED display;

using namespace std;

// Global flag for graceful shutdown
static volatile sig_atomic_t keepRunning = 1;

static void signalHandler(int signum) {
    keepRunning = 0;
}

static string getHostname() {
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        return string(hostname);
    }
    return "Unknown";
}

static string getIPAddress() {
    struct ifaddrs* interfaces = nullptr;
    struct ifaddrs* temp_addr = nullptr;
    string ipAddress = "No IP";

    if (getifaddrs(&interfaces) == 0) {
        temp_addr = interfaces;
        while (temp_addr != nullptr) {
            if (temp_addr->ifa_addr != nullptr && temp_addr->ifa_addr->sa_family == AF_INET) {
                if (strcmp(temp_addr->ifa_name, "lo") != 0) {
                    // C++ prefers reinterpret_cast for sockaddr structs
                    ipAddress = inet_ntoa(reinterpret_cast<struct sockaddr_in*>(temp_addr->ifa_addr)->sin_addr);
                    break;
                }
            }
            temp_addr = temp_addr->ifa_next;
        }
    }
    freeifaddrs(interfaces);
    return ipAddress;
}

static string getUptime() {
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        const long uptime = info.uptime;
        const int days = uptime / 86400;
        const int hours = (uptime % 86400) / 3600;
        const int minutes = (uptime % 3600) / 60;

        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%dd %02dh %02dm", days, hours, minutes);
        return string(buffer);
    }
    return "Unknown";
}

static string getDateAndTime() {
    try {
        const auto now = chrono::system_clock::now();
        const time_t now_c = chrono::system_clock::to_time_t(now);

        struct tm local_time;
        // localtime_r is POSIX standard and thread-safe
        if (localtime_r(&now_c, &local_time) == nullptr) {
            return "Unknown";
        }

        stringstream ss;
        ss << put_time(&local_time, "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    catch (...) {
        return "Unknown";
    }
}

static void updateDisplay(const string& hostname) {
    // Fetch dynamic system data
    const string ip = getIPAddress();
    const string uptime = getUptime();
    const string dateAndTime = getDateAndTime();

    display.clearDisplay();

    // Render the Top Bar
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);

    char titleBar[22];
    snprintf(titleBar, sizeof(titleBar), " %-19s ", hostname.c_str());
    display.print(titleBar);

    // Render the Main Body
    display.setTextColor(WHITE);

    display.setCursor(0, 16);
    display.print("IP: ");
    display.print(ip.c_str());

    display.setCursor(0, 26);
    display.print("Up: ");
    display.print(uptime.c_str());

    display.setCursor(0, 36);
    display.print(dateAndTime.c_str());

    display.display();
}

int main(int argc, char** argv) {
    // Register signal handlers for clean exit
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // Use the named constant instead of magic number '3'
    if (!display.init(OLED_I2C_RESET, OLED_ADAFRUIT_I2C_128x64)) {
        cerr << "Failed to initialize OLED display." << endl;
        return 1;
    }

    display.begin();

    // Cache the hostname once
    const string staticHostname = getHostname();

    while (keepRunning) {
        updateDisplay(staticHostname);
        usleep(2000000); 
    }

    // Clean up the display before exiting so it doesn't freeze on the last frame
    display.clearDisplay();
    display.display();
    display.close();

    return 0;
}