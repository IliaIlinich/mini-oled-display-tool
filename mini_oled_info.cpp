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
         << "  -c, --clear   Clear the display and exit\n"
         << "  -h, --help    Show this help message\n";
}

int main(const int argc, char** argv) {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    // Default values if arguments are omitted
    string hostname = "Unknown";
    string ip = "No IP";
    string uptime = "0d 00h 00m";
    string dateAndTime = "YYYY-MM-DD HH:MM:SS";
    bool clearOnly = false; // Flag to check if we only want to clear

    // Setup command line options
    // Note: Changed has_arg to match short option behavior (required_argument vs no_argument)
    static struct option long_options[] = {
        {.name = "name",   .has_arg = required_argument, .flag = nullptr, .val = 'n'},
        {.name = "ip",     .has_arg = required_argument, .flag = nullptr, .val = 'i'},
        {.name = "uptime", .has_arg = required_argument, .flag = nullptr, .val = 'u'},
        {.name = "date",   .has_arg = required_argument, .flag = nullptr, .val = 'd'},
        {.name = "clear",  .has_arg = no_argument,       .flag = nullptr, .val = 'c'},
        {.name = "help",   .has_arg = no_argument,       .flag = nullptr, .val = 'h'},
        {.name = nullptr,  .has_arg = 0,                 .flag = nullptr, .val = 0}
    };

    int opt;
    int option_index = 0;
    // Added 'c' to the short options string
    while ((opt = getopt_long(argc, argv, "n:i:u:d:hc", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'n': hostname = optarg; break;
            case 'i': ip = optarg; break;
            case 'u': uptime = optarg; break;
            case 'd': dateAndTime = optarg; break;
            case 'c': clearOnly = true; break; // Set flag instead of calling clear right away
            case 'h': printUsage(argv[0]); return 0;
            default:  printUsage(argv[0]); return 1;
        }
    }

    // 1. Initialize the display FIRST
    if (!display.init(OLED_I2C_RESET, OLED_ADAFRUIT_I2C_128x64)) {
        cerr << "Failed to initialize OLED display." << endl;
        return 1;
    }

    display.begin();

    // 2. Clear the memory buffer
    display.clearDisplay();

    // 3. If -c was passed, push the empty buffer and exit immediately
    if (clearOnly) {
        display.display(); // This physically blanks the screen
        display.close();
        return 0;
    }

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