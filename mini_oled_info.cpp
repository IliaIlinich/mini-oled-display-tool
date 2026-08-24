#include <cstdint>
typedef uint8_t boolean;

#include <iostream>
#include <string>
#include <unistd.h>
#include <cstdio>
#include <getopt.h>
#include <ctime>            // For getDateAndTime
#include <sys/sysinfo.h>    // For getUptime
#include <sys/statvfs.h>    // For getStorageInformation

#include <Adafruit_GFX.h>
#include <ArduiPi_OLED.h>
#include <ArduiPi_OLED_lib.h>

using namespace std;

ArduiPi_OLED display;

static void printUsage(const char* progName) {
    cout << "Usage: " << progName << " [options]\n"
         << "Options:\n"
         << "  -n, --name    Hostname or title to display (requires argument)\n"
         << "  -i, --ip      IP Address (requires argument)\n"
         << "  -u, --uptime  Display uptime\n"
         << "  -d, --date    Display date and time\n"
         << "  -s, --space   Display available storage\n"
         << "  -c, --clear   Clear the display and exit\n"
         << "  -h, --help    Show this help message\n"
         << "  Uptime, storage information, date and time are fetched automatically.\n"
         << "  Name (-n) and IP (-i) require string arguments typed after them.\n";
}

static string getDateAndTime() {
    time_t now = time(nullptr);
    struct tm tstruct;
    char buf[40];
    tstruct = *localtime(&now);
    // Format: YYYY-MM-DD HH:MM
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", &tstruct);
    return string(buf);
}

static string getUptime() {
    struct sysinfo info;
    if (sysinfo(&info) != 0) {
        return "Unknown";
    }

    long uptime = info.uptime;
    long days = uptime / 86400;
    long hours = (uptime % 86400) / 3600;
    long minutes = (uptime % 3600) / 60;

    char buf[64];
    if (days > 0) {
        snprintf(buf, sizeof(buf), "%ldd %02ldh %02ldm", days, hours, minutes);
    } else {
        snprintf(buf, sizeof(buf), "%02ldh %02ldm", hours, minutes);
    }
    return string(buf);
}

static void getStorageInformation(string& freeSpace, string& totalSpace) {
    struct statvfs stat;

    // Check root directory "/" for storage info
    if (statvfs("/", &stat) != 0) {
        freeSpace = "Err";
        totalSpace = "Err";
        return;
    }

    // Calculate sizes in Gigabytes
    double total_gb = (double)(stat.f_blocks * stat.f_frsize) / (1024 * 1024 * 1024);
    double free_gb = (double)(stat.f_bavail * stat.f_frsize) / (1024 * 1024 * 1024);

    char buf[32];
    snprintf(buf, sizeof(buf), "%.1fG", total_gb);
    totalSpace = string(buf);

    snprintf(buf, sizeof(buf), "%.1fG", free_gb);
    freeSpace = string(buf);
}

int main(const int argc, char** argv) {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    // Default values if arguments are omitted
    string hostname = "Unknown";
    string ip = "No IP";
    bool clearOnly = false;

    // Setup command line options
    // Changed: uptime, date, and space no longer require arguments
    static struct option long_options[] = {
        {.name = "name",   .has_arg = required_argument, .flag = nullptr, .val = 'n'},
        {.name = "ip",     .has_arg = required_argument, .flag = nullptr, .val = 'i'},
        {.name = "uptime", .has_arg = no_argument,       .flag = nullptr, .val = 'u'},
        {.name = "date",   .has_arg = no_argument,       .flag = nullptr, .val = 'd'},
        {.name = "space",  .has_arg = no_argument,       .flag = nullptr, .val = 's'},
        {.name = "clear",  .has_arg = no_argument,       .flag = nullptr, .val = 'c'},
        {.name = "help",   .has_arg = no_argument,       .flag = nullptr, .val = 'h'},
        {.name = nullptr,  .has_arg = 0,                 .flag = nullptr, .val = 0}
    };

    int opt;
    int option_index = 0;

    // Removed colons after u, d, and s because they no longer expect arguments
    while ((opt = getopt_long(argc, argv, "n:i:udshc", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'n': hostname = optarg; break;
            case 'i': ip = optarg; break;
            case 'u': /* Handled automatically below */ break;
            case 'd': /* Handled automatically below */ break;
            case 's': /* Handled automatically below */ break;
            case 'c': clearOnly = true; break;
            case 'h': printUsage(argv[0]); return 0;
            default:  printUsage(argv[0]); return 1;
        }
    }

    // Automatically fetch system information
    string uptime = getUptime();
    string dateAndTime = getDateAndTime();
    string freeSpace, totalSpace;
    getStorageInformation(freeSpace, totalSpace);

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
        display.display();
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

    // Adjusting storage layout to fit horizontally on 128x64 screen
    display.setCursor(0, 46);
    display.print("Disk: ");
    display.print(freeSpace.c_str());
    display.print(" / ");
    display.print(totalSpace.c_str());

    // Push the buffer to the physical screen
    display.display();

    // Close the I2C file descriptor gracefully.
    display.close();

    return 0;
}