#include <cstdint>
typedef uint8_t boolean;

#include <iostream>
#include <string>
#include <unistd.h>
#include <cstdio>
#include <getopt.h>

#include <Adafruit_GFX.h>
#include <ArduiPi_OLED.h>
#include <ArduiPi_OLED_lib.h>

using namespace std;

ArduiPi_OLED display;

static void printUsage(const char* progName) {
    cout << "Usage: " << progName << " [options]\n"
         << "Options:\n"
         << "  -n, --name    Hostname or title to display\n"
         << "  -i, --ip      IP Address\n"
         << "  -u, --uptime  Uptime string\n"
         << "  -d, --date    Date and Time string\n"
         << "  -h, --help    Show this help message\n";
}

int main(int argc, char** argv) {
    // Default values if arguments are omitted
    string hostname = "Unknown";
    string ip = "No IP";
    string uptime = "0d 00h 00m";
    string dateAndTime = "YYYY-MM-DD HH:MM:SS";

    // Setup command line options
    static struct option long_options[] = {
        {"name",   required_argument, 0, 'n'},
        {"ip",     required_argument, 0, 'i'},
        {"uptime", required_argument, 0, 'u'},
        {"date",   required_argument, 0, 'd'},
        {"help",   no_argument,       0, 'h'},
        {0, 0, 0, 0}
    };

    int opt;
    int option_index = 0;
    while ((opt = getopt_long(argc, argv, "n:i:u:d:h", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'n': hostname = optarg; break;
            case 'i': ip = optarg; break;
            case 'u': uptime = optarg; break;
            case 'd': dateAndTime = optarg; break;
            case 'h': printUsage(argv[0]); return 0; // Printing usage page with -h
            default:  printUsage(argv[0]); return 1; // Printing usage page if invalid argument has bit entered
        }
    }

    // Initialize the display
    if (!display.init(OLED_I2C_RESET, OLED_ADAFRUIT_I2C_128x64)) {
        cerr << "Failed to initialize OLED display." << endl;
        return 1;
    }

    display.begin();

    // Clear the buffer from the PREVIOUS execution
    display.clearDisplay();

    // Render the Top Bar
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);

    char titleBar[32];
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

    // Push the buffer to the physical screen
    display.display();

    // Close the I2C file descriptor gracefully.
    display.close();

    return 0;
}