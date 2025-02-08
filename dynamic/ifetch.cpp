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


// ANSI color macros
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

// Function for keyboard input
char getKeyPress() {
    struct termios oldt, newt;
    char ch = 0;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    fd_set set;
    struct timeval timeout;
    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);
    timeout.tv_sec = 0;
    timeout.tv_usec = 10000; // 10ms
    if (select(STDIN_FILENO + 1, &set, NULL, NULL, &timeout) > 0) {
        read(STDIN_FILENO, &ch, 1);
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

// Get system information
std::string getCommandOutput(const char* cmd) {
    std::array<char, 256> buffer;
    std::string result;
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "N/A";
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    pclose(pipe);
    result.erase(result.find_last_not_of("\n") + 1);
    return result;
}

// Random character generation for logo
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

// ASCII-style OS logo
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

// System information
std::vector<std::string> getSystemInfo() {
    std::vector<std::string> sysInfo;
    
    // Username
    std::string username = getCommandOutput("whoami");
    sysInfo.push_back(BLUE1 BOLD " " + username + RESET);
    std::string hostname = getCommandOutput("hostname | sed 's/.local//g'");
    std::size_t hostname_len = hostname.size() + 1;
    sysInfo.push_back(BLUE1 " @" + hostname + RESET);
    std::string header_bar = " " + std::string(hostname_len, '-');
    sysInfo.push_back(BLUE1 + header_bar + RESET);
    
    // System information
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

void printFetch() {
    std::cout << "\033[2J\033[H"; // New frame / remove terminal content
    std::vector<std::string> sysInfo = getSystemInfo(); // System Information
    std::vector<std::string> topColors = {YELLOW1_BG, ORANGE1_BG, ORANGE2_BG, ORANGE3_BG, ORANGE2_BG, ORANGE1_BG}; // Upper color bar
    std::vector<std::string> bottomColors = {GREY_BG, BLUE1_BG, BLUE2_BG, BLUE3_BG, BLUE2_BG, BLUE1_BG}; // Lower color bar
    
    std::size_t frameCounter = 0;
    const std::size_t speedFactor = 4;

    time_t lastUpdate = time(0);
    while (true) {
        char key = getKeyPress();
        if (key == 27 || key == 'q') break;

        std::cout << "\033[2J\033[H"; // New frame / remove terminal content
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

        std::cout << BLUE3 " >> PRESS [ESC] to quit" << std::endl;
        std::cout << " >> ";

        std::cout << std::flush;

        // Update system information every 120s
        if (difftime(time(0), lastUpdate) >= 120) {
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