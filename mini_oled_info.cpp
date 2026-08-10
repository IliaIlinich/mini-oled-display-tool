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

#include <Adafruit_GFX.h>
#include <ArduiPi_OLED.h>
#include <ArduiPi_OLED_lib.h>

ArduiPi_OLED display;

using namespace std;

string getHostname() {
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        return string(hostname);
    }
    return "Unknown";
}

string getIPAddress() {
    struct ifaddrs* interfaces = nullptr;
    struct ifaddrs* temp_addr = nullptr;
    string ipAddress = "No IP";

    if (getifaddrs(&interfaces) == 0) {
        temp_addr = interfaces;
        while (temp_addr != nullptr) {
            if (temp_addr->ifa_addr != nullptr && temp_addr->ifa_addr->sa_family == AF_INET) {
                if (strcmp(temp_addr->ifa_name, "lo") != 0) {
                    ipAddress = inet_ntoa(((struct sockaddr_in*)temp_addr->ifa_addr)->sin_addr);
                    break;
                }
            }
            temp_addr = temp_addr->ifa_next;
        }
    }
    freeifaddrs(interfaces);
    return ipAddress;
}

string getUptime() {
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        long uptime = info.uptime;
        int days = uptime / 86400;
        int hours = (uptime % 86400) / 3600;
        int minutes = (uptime % 3600) / 60;

        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%dd %02dh %02dm", days, hours, minutes);
        return string(buffer);
    }
    return "Unknown";
}

string getDateAndTime() {
    try {
        auto now = chrono::system_clock::now();
        time_t now_c = chrono::system_clock::to_time_t(now);

        tm* local_time = localtime(&now_c);
        if (local_time == nullptr) {
            return "Unknown";
        }

        stringstream ss;
        ss << put_time(local_time, "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    catch (...) {
        return "Unknown";
    }
}

void updateDisplay() {
    // Fetch current system data
    string hostname = getHostname();
    string ip = getIPAddress();
    string uptime = getUptime();
    string dateAndTime = getDateAndTime();

    // Clear previous frame
    display.clearDisplay();

    // Render the Top Bar
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);

    // A standard 128px wide OLED fits 21 characters at text size 1.
    char titleBar[22];
    snprintf(titleBar, sizeof(titleBar), " %-19s ", hostname.c_str());
    display.print(titleBar);

    // 4. Draw the Main Body
    display.setTextColor(WHITE);

    // Print IP Address
    display.setCursor(0, 16);
    display.print("IP: ");
    display.print(ip.c_str());

    // Print Uptime
    display.setCursor(0, 26);
    display.print("Up: ");
    display.print(uptime.c_str());

    // Print Date and Time
    display.setCursor(0, 36);
    display.print(dateAndTime.c_str());

    // Push the buffer to the OLED screen
    display.display();
}

int main(int argc, char** argv) {
    if (!display.init(OLED_I2C_RESET, 3)) {
        return 1;
    }

    display.begin();

    while (true) {
        updateDisplay();
        usleep(2000000); // Refresh every 2 seconds
    }

    return 0;
}
