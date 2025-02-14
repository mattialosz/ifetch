#include <iostream>
#include <cstdlib>
#include <sstream>
#include <array>
#include <vector>
#include <iomanip>
#include <ctime>
#include <unistd.h>
#include <termios.h>
#include <random>
#include <string>
#include <thread>
#include <chrono>
#include <sys/sysctl.h>
#include <mach/mach.h>

// ANSI color macros for styling terminal output
#define RESET   "\033[0m"
#define BOLD    "\033[1m"

#define GREY    "\033[38;5;248m"
#define YELLOW1 "\033[38;5;220m"
#define ORANGE1 "\033[38;5;214m"
#define ORANGE2 "\033[38;5;208m"
#define ORANGE3 "\033[38;5;202m"
#define BLUE1   "\033[38;5;33m"
#define BLUE2   "\033[38;5;26m"
#define BLUE3   "\033[38;5;27m"

#define GREY_BG "\033[48;5;248m"
#define YELLOW1_BG "\033[48;5;220m"
#define ORANGE1_BG "\033[48;5;214m"
#define ORANGE2_BG "\033[48;5;208m"
#define ORANGE3_BG "\033[48;5;202m"
#define BLUE1_BG   "\033[48;5;33m"
#define BLUE2_BG   "\033[48;5;26m"
#define BLUE3_BG   "\033[48;5;27m"

// Terminal dimensions and constants for CPU/RAM monitoring
const int WIDTH = 30; // Number of historical data points for CPU/RAM usage
const int HEIGHT_CPU = 8;
const int HEIGHT_RAM = 7;
const std::string BLOCK = "*"; // Character for usage graph
const std::string BAR_BLOCK = "█"; // Character for progress bars
const int BAR_WIDTH = 15; // Length of progress bars
const double CPU_SCALING_FACTOR = 1.5; // Factor to scale CPU graph amplitude

// Function to handle non-blocking key press detection
char getKeyPress() {
    struct termios oldt, newt;
    char ch = 0;
    tcgetattr(STDIN_FILENO, &oldt); // Save old terminal settings
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); // Disable echo and canonical mode
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    
    fd_set set;
    struct timeval timeout;
    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);
    timeout.tv_sec = 0;
    timeout.tv_usec = 10000; // 10ms timeout for input check
    
    if (select(STDIN_FILENO + 1, &set, NULL, NULL, &timeout) > 0) {
        read(STDIN_FILENO, &ch, 1);
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Restore old terminal settings
    return ch;
}

// Function to execute system commands and return the output as a string
std::string getCommandOutput(const char* cmd) {
    std::array<char, 256> buffer;
    std::string result;
    FILE* pipe = popen(cmd, "r"); // Open a pipe to execute the command
    if (!pipe) return "N/A";

    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    pclose(pipe);
    result.erase(result.find_last_not_of("\n") + 1); // Remove trailing newlines
    return result;
}

// Function to generate a random string with characters for the Apple logo
std::string randString(int len) {
    std::string chars = "o#X$&55$$SSs";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, chars.size() - 1);
    std::string result = "";

    for (int i = 0; i < len; i++) {
        result += chars[dis(gen)];
    }

    return result;
}

// Function to generate an ASCII-styled animated Apple logo
std::vector<std::string> getAnimatedAppleLogo() {
    std::vector<std::string> logo;

    logo.push_back(ORANGE3 "                            ." + randString(4) + "'                  " RESET);
    logo.push_back(ORANGE2 "                          " + randString(7) + "                   " RESET);
    logo.push_back(ORANGE1 "                         o" + randString(5) + "o                    " RESET);
    logo.push_back(YELLOW1 "                        ." + randString(5) + "                      " RESET);
    logo.push_back(YELLOW1 "                         0" + randString(1) + ".     ..                 " RESET);
    logo.push_back(ORANGE1 "               ." + randString(7) + ".   .x" + randString(11) + ".           " RESET);
    logo.push_back(ORANGE2 "            ." + randString(28) + ":          " RESET);
    logo.push_back(ORANGE3 "           ." + randString(30) + ".         " RESET);
    logo.push_back(ORANGE3 "          ." + randString(30) + ".          " RESET);
    logo.push_back(ORANGE2 "         ." + randString(28) + ":             " RESET);
    logo.push_back(ORANGE1 "        ." + randString(29) + ":             " RESET);
    logo.push_back(YELLOW1 "        ." + randString(29) + ":             " RESET);
    logo.push_back(YELLOW1 "         x" + randString(30) + ".           " RESET);
    logo.push_back(ORANGE1 "          " + randString(31) + ".          " RESET);
    logo.push_back(ORANGE2 "           o" + randString(31) + "         " RESET);
    logo.push_back(ORANGE3 "           ." + randString(30) + ":         " RESET);
    logo.push_back(ORANGE3 "            '" + randString(29) + ".         " RESET);
    logo.push_back(ORANGE2 "             '" + randString(27) + ".          " RESET);
    logo.push_back(ORANGE1 "              k" + randString(24) + "d            " RESET);
    logo.push_back(YELLOW1 "                ;#" + randString(6) + "#xoooxX" + randString(6) + "X.             " RESET);
    logo.push_back(YELLOW1 "                   'ooo*'       '*ooo'              " RESET);

    return logo;
}

// Function to retrieve and format system information
std::vector<std::string> getSystemInfo() {
    std::vector<std::string> sysInfo;
    
    // Get username
    std::string username = getCommandOutput("whoami");
    sysInfo.push_back(BLUE1 BOLD " " + username + RESET);
    
    // Get username
    std::string hostname = getCommandOutput("hostname | sed 's/.local//g'");
    std::size_t hostname_len = hostname.size() + 1;
    sysInfo.push_back(BLUE1 " @" + hostname + RESET);

    // Header bar
    std::string header_bar = " " + std::string(hostname_len, '-');
    sysInfo.push_back(BLUE1 + header_bar + RESET);
    
    // Fetch and format system details
    sysInfo.push_back(ORANGE1   BOLD " OS: " GREY + getCommandOutput("sw_vers -productName") + " " + getCommandOutput("sw_vers -productVersion"));
    sysInfo.push_back(ORANGE1   BOLD " Host: " GREY + getCommandOutput("sysctl -n hw.model"));
    sysInfo.push_back(ORANGE1  BOLD " Kernel: " GREY + getCommandOutput("uname -r"));
    sysInfo.push_back(ORANGE1   BOLD " Uptime: " GREY + getCommandOutput("uptime | awk -F', ' '{print $1}'"));
    sysInfo.push_back(ORANGE1 BOLD " Packages: " GREY + getCommandOutput("brew list | wc -l") + " (brew)");
    sysInfo.push_back(ORANGE1 BOLD " Shell: " GREY + getCommandOutput("echo $SHELL"));
    sysInfo.push_back(ORANGE1 BOLD " Display: " GREY + getCommandOutput("system_profiler SPDisplaysDataType | grep Resolution | tail -n 1 | awk -F': ' '{print $2}'"));
    sysInfo.push_back(ORANGE1  BOLD " Terminal: " GREY + getCommandOutput("echo $TERM"));
    sysInfo.push_back(ORANGE1    BOLD " CPU: " GREY + getCommandOutput("sysctl -n machdep.cpu.brand_string"));
    sysInfo.push_back(ORANGE1     BOLD " RAM: " GREY + getCommandOutput("system_profiler SPHardwareDataType | grep 'Memory' | awk '{print $2, $3}'"));
    sysInfo.push_back(ORANGE1   BOLD " GPU: " GREY + getCommandOutput("system_profiler SPDisplaysDataType | grep 'Chipset Model' | awk -F: '{print $2}'"));
    sysInfo.push_back(ORANGE1 BOLD " Disk: " GREY + getCommandOutput("df -h / | awk 'NR==2 {gsub(\"Gi\", \"\", $4); gsub(\"Gi\", \"\", $2); printf \"%.2f GB / %.2f GB (%.2f%% free)\\n\", $4*1.073741824, $2*1.073741824, ($4/$2)*100}'"));
    sysInfo.push_back(ORANGE1    BOLD " Swap: " GREY + getCommandOutput("sysctl -n vm.swapusage | awk '{print $3}'"));
    sysInfo.push_back(ORANGE1  BOLD " Battery: " GREY + getCommandOutput("pmset -g batt | grep -Eo '[0-9]+%'"));
    sysInfo.push_back(ORANGE1    BOLD " Local IP: " GREY + getCommandOutput("ipconfig getifaddr en0"));
    sysInfo.push_back(ORANGE1    BOLD " Locale: " GREY + getCommandOutput("defaults read -g AppleLocale"));
    return sysInfo;
}

std::string getAnimatedColorBar(std::vector<std::string>& colors) {
    std::string bar(57, ' '); // Padding
    for (const auto& color : colors) {
        bar += color + "     ";
    }
    bar += RESET;

    return bar;
}

// Function to retrieve CPU usage percentage on macOS
double getCPUUsage() {
    host_cpu_load_info_data_t cpuLoad;
    mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
    kern_return_t status = host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO, (host_info_t)&cpuLoad, &count);

    if (status != KERN_SUCCESS) {
        return 0.0;
    }

    static uint64_t prevUser = 0, prevSystem = 0, prevIdle = 0;
    uint64_t user = cpuLoad.cpu_ticks[CPU_STATE_USER];
    uint64_t system = cpuLoad.cpu_ticks[CPU_STATE_SYSTEM];
    uint64_t idle = cpuLoad.cpu_ticks[CPU_STATE_IDLE];

    uint64_t total = (user - prevUser) + (system - prevSystem);
    uint64_t totalTime = total + (idle - prevIdle);

    prevUser = user;
    prevSystem = system;
    prevIdle = idle;

    return totalTime == 0 ? 0.0 : (100.0 * total / totalTime);
}

// Function to retrieve RAM usage on macOS
std::pair<double, double> getRAMUsage() {
    int64_t totalRAM;
    size_t size = sizeof(totalRAM);
    sysctlbyname("hw.memsize", &totalRAM, &size, NULL, 0);
    totalRAM /= (1024.0 * 1024 * 1024); // Convert bytes to GB

    mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
    vm_statistics64_data_t vmStats;
    if (host_statistics64(mach_host_self(), HOST_VM_INFO, (host_info_t)&vmStats, &count) != KERN_SUCCESS) {
        return {0.0, totalRAM};
    }
    
    int64_t pageSize;
    size = sizeof(pageSize);
    sysctlbyname("hw.pagesize", &pageSize, &size, NULL, 0);
    
    double usedRAM = (vmStats.active_count + vmStats.wire_count) * pageSize / (1024.0 * 1024 * 1024); // Convert to GB
    
    return {usedRAM, totalRAM};
}

// Function to draw a simple progress bar
void drawLoadingBar(const std::string& label, double usage) {
    int filled = static_cast<int>((usage / 100.0) * BAR_WIDTH);
    std::cout << label << ":" << std::endl;
    std::cout << "[";
    for (int i = 0; i < BAR_WIDTH; i++) {
        if (i < filled) {
            std::cout << BAR_BLOCK;
        } else {
            std::cout << " ";
        }
    }
    std::cout << "] " << usage << "%\n";
}

// Function to display system fetch information
void printFetch() {
    std::cout << "\033[2J\033[H"; // Clear terminal screen

    std::vector<std::string> sysInfo = getSystemInfo(); // Retrieve system information
    std::vector<std::string> topColors = {YELLOW1_BG, ORANGE1_BG, ORANGE2_BG, ORANGE3_BG, ORANGE2_BG, ORANGE1_BG}; // Upper color bar
    std::vector<std::string> bottomColors = {GREY_BG, BLUE1_BG, BLUE2_BG, BLUE3_BG, BLUE2_BG, BLUE1_BG}; // Lower color bar

    std::vector<int> cpuUsageHistory(WIDTH, 0);
    std::vector<int> ramUsageHistory(WIDTH, 0);
    const std::size_t updateInterval = 8; // Refresh every X frames
    double cpuUsage = 0.0;
    double usedRAM = 0.0;
    double totalRAM = 0.0;
    double ramUsagePercent = 0.0;

    std::size_t scene = 0; // Default scene (0 = System Info, 1 = CPU/RAM Graph)
    bool displayOptions = true;
    std::size_t frameCounter = 0;
    const std::size_t speedFactor = 4; // Speed of animation

    time_t lastUpdate = time(0);
    while (true) {
        char key = getKeyPress();
        if (key == 27 || key == 'q') break; // Exit on ESC or 'q'
        if (key == '1') scene = 1;
        if (key == '0') scene = 0;
        if (key == 'x') displayOptions = !displayOptions; // Toggle options display

        std::cout << "\033[2J\033[H"; // Clear screen for smooth rendering
        if (scene == 0) {
            std::vector<std::string> logo = getAnimatedAppleLogo(); // Animated logo
            
            // Calculate max height
            std::size_t logoHeight = logo.size();
            std::size_t sysInfoHeight = sysInfo.size();
            std::size_t maxHeight = std::max(logoHeight, sysInfoHeight);
            
            for (std::size_t i = 0; i < 4; i++) std::cout << std::endl;

            for (std::size_t i = 0; i < maxHeight; ++i) {
                // Print logo
                if (i < logoHeight) {
                    std::cout << logo[i];
                } else {
                    std::cout << std::string(20, ' '); // Placeholder
                }
                
                std::cout << "    ";
                
                // Print system information
                if (i < sysInfoHeight) {
                    std::cout << sysInfo[i];
                }
                
                std::cout << std::endl;
            }

            // Animation color bar
            std::cout << getAnimatedColorBar(topColors) << std::endl;
            std::cout << getAnimatedColorBar(bottomColors) << std::endl;
            if (frameCounter % speedFactor == 0) {
                std::string firstTop = topColors.front();
                topColors.erase(topColors.begin());
                topColors.push_back(firstTop);

                std::string firstBottom = bottomColors.front();
                bottomColors.erase(bottomColors.begin());
                bottomColors.push_back(firstBottom);
                frameCounter = 0;
            }

            for (std::size_t i = 0; i < 2; i++) std::cout << std::endl;
        } else if (scene == 1) {
            if (frameCounter % updateInterval == 0) {
                cpuUsage = getCPUUsage();
                auto RAM_Info = getRAMUsage();
                usedRAM = RAM_Info.first;
                totalRAM = RAM_Info.second;
                ramUsagePercent = (usedRAM / totalRAM) * 100.0;
            
                cpuUsageHistory.erase(cpuUsageHistory.begin());
                cpuUsageHistory.push_back(static_cast<int>((cpuUsage / 100.0) * HEIGHT_CPU * CPU_SCALING_FACTOR));
            
                ramUsageHistory.erase(ramUsageHistory.begin());
                ramUsageHistory.push_back(static_cast<int>((ramUsagePercent / 100.0) * HEIGHT_RAM));
                frameCounter = 0;
            }
            
            std::cout << "\033[2J\033[H"; // Clear screen for smooth rendering
            const int PADDING_LEFT = 11;
            const int BOX_SPACING = 10;

            for (int i = 0; i < 4; i++) std::cout << std::endl;

            // Heading with box
            std::string title = " CPU / RAM Information ";
            int titlePadding = (30 - title.size()) / 2;

            std::cout << std::string(PADDING_LEFT + 24, ' ') << ORANGE2 "╔" << "═════════════════════════════" << "╗\n";
            std::cout << std::string(PADDING_LEFT + 24, ' ') << "║" << YELLOW1 + std::string(titlePadding, ' ') 
                    << title << std::string(titlePadding, ' ') + ORANGE2 << "║\n";
            std::cout << std::string(PADDING_LEFT + 24, ' ') << "╚" << "═════════════════════════════" << "╝\n\n";

            for (int i = 0; i < 1; i++) std::cout << std::endl;

            std::cout << std::string(PADDING_LEFT, ' ') << ORANGE3 "╔═══════════ CPU Usage ══════════╗" 
                    << std::string(BOX_SPACING, ' ') 
                    << "╔═══════════ RAM Usage ══════════╗\n";

            for (int i = 0; i < std::max(HEIGHT_CPU, HEIGHT_RAM); i++) {
                std::cout << std::string(PADDING_LEFT, ' ') << "║ ";
                for (int j = 0; j < WIDTH; j++) {
                    std::cout << ORANGE1 + (cpuUsageHistory[j] >= (HEIGHT_CPU - i) ? BLOCK : " ") + ORANGE3;
                }
                std::cout << " ║" << std::string(BOX_SPACING, ' ') << "║ ";
                for (int j = 0; j < WIDTH; j++) {
                    std::cout << ORANGE1 + (ramUsageHistory[j] >= (HEIGHT_RAM - i) ? BLOCK : " ") + ORANGE3;
                }
                std::cout << " ║\n";
            }

            std::cout << std::string(PADDING_LEFT, ' ') << "╚════════════════════════════════╝" 
                    << std::string(BOX_SPACING, ' ') 
                    << "╚════════════════════════════════╝\n\n";

            std::cout << std::string(10, ' ') << ORANGE2 " CPU Usage: [";
            int filledCPU = static_cast<int>((cpuUsage / 100.0) * BAR_WIDTH);
            for (int i = 0; i < BAR_WIDTH; i++) {
                std::cout << (i < filledCPU ? BAR_BLOCK : " ");
            }
            std::cout << "] " << std::setw(5) << std::fixed << std::setprecision(1) << cpuUsage << "%";

            std::cout << std::string(8, ' ') << " RAM Usage: [";
            int filledRAM = static_cast<int>((ramUsagePercent / 100.0) * BAR_WIDTH);
            for (int i = 0; i < BAR_WIDTH; i++) {
                std::cout << (i < filledRAM ? BAR_BLOCK : " ");
            }
            std::cout << "] " << std::setw(5) << std::fixed << std::setprecision(1) << ramUsagePercent << "%\n";

            std::cout << std::string(54, ' ') << ORANGE1 " Memory: " << usedRAM << " GB / " << totalRAM << " GB\n";

            for (int i = 0; i < 4; i++) std::cout << std::endl;

            // Animation color bar
            std::cout << getAnimatedColorBar(topColors) << std::endl;
            std::cout << getAnimatedColorBar(bottomColors) << std::endl;
            if (frameCounter % speedFactor == 0) {
                std::string firstTop = topColors.front();
                topColors.erase(topColors.begin());
                topColors.push_back(firstTop);

                std::string firstBottom = bottomColors.front();
                bottomColors.erase(bottomColors.begin());
                bottomColors.push_back(firstBottom);
            }

            for (int i = 0; i < 2; i++) std::cout << std::endl;
        }

        if (displayOptions) {
            std::cout << BLUE3 " >> [0] System  [1] CPU/RAM  [x] Hide/Show  [ESC] Exit" << std::endl;
        } else {
            std::cout << std::endl;
        }
        
        std::cout << BLUE3 " >> ";

        std::cout << std::flush;

        // Update system information every 120s
        if (scene == 0 && difftime(time(0), lastUpdate) >= 120) {
            sysInfo = getSystemInfo();
            lastUpdate = time(0);
        }

        frameCounter++;
        usleep(100000);
    }
}

int main() {
    printFetch();
    return 0;
}