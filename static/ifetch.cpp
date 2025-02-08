#include <iostream>
#include <cstdlib>
#include <sstream>
#include <array>
#include <vector>
#include <iomanip>

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

// ASCII-style OS logo
std::vector<std::string> getAppleLogo() {
    std::vector<std::string> logo;
    logo.push_back(ORANGE3 "                            .o$$X'                  " RESET);
    logo.push_back(ORANGE2 "                          x$S$SSx                   " RESET);
    logo.push_back(ORANGE1 "                         o#$$$$o                    " RESET);
    logo.push_back(YELLOW1 "                        .X$$$x                      " RESET);
    logo.push_back(YELLOW1 "                         00.     ..                 " RESET);
    logo.push_back(ORANGE1 "               .o#$$$#X.   .x#$$$$$$$$#x.           " RESET);
    logo.push_back(ORANGE2 "            .oSSSSSSSSSSSSS5SSSSSSSsS$5$#:          " RESET);
    logo.push_back(ORANGE3 "           .#$$$$$$$$$$$$$$$$$$$$$$$$$$$$X.         " RESET);
    logo.push_back(ORANGE3 "          .ssssssssssssssssssss$s$$s$$$$x.          " RESET);
    logo.push_back(ORANGE2 "         .SSSSSSSSSSSSS555SSSSSSSSS55S:             " RESET);
    logo.push_back(ORANGE1 "        .X$$$$$$$$$$$$$$$$$$$$$$$$$$$#:             " RESET);
    logo.push_back(YELLOW1 "        .X$$$$$$$$$$$$$$$$$$$$$$$$$$$$:             " RESET);
    logo.push_back(YELLOW1 "         x$$$$$$$$$$$$$$$$$$$$$$$$$$$$$#.           " RESET);
    logo.push_back(ORANGE1 "          OSSSSSSSSSSSSSSSSSSSSSSSSSSSs$#.          " RESET);
    logo.push_back(ORANGE2 "           o#S$5SSSSS$$SSSSS5SSSSSSSS$S$$Sk         " RESET);
    logo.push_back(ORANGE3 "           .SSS55SSSSSSSSSS555SSSSSSSSS55S:         " RESET);
    logo.push_back(ORANGE3 "            'X$$$$S$$$S$$$$$$$$$$$$$$$$$$x.         " RESET);
    logo.push_back(ORANGE2 "             '#$$$$$$$$$$$$$$$$$$$$$$$$$x.          " RESET);
    logo.push_back(ORANGE1 "              k#$$$$$$$$$$$$$$$$$$$$$$#d            " RESET);
    logo.push_back(YELLOW1 "                ;#$$$$$$#xoooxX$$$$$$X.             " RESET);
    logo.push_back(YELLOW1 "                   'ooo*'       '*ooo'              " RESET);
    logo.push_back(YELLOW1 "                                                    " RESET);
    logo.push_back(YELLOW1 "                                                    " RESET);
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
    sysInfo.push_back(" ");
    sysInfo.push_back(" " YELLOW1_BG "     " ORANGE1_BG "     " ORANGE2_BG "     " ORANGE3_BG "     " RESET);
    sysInfo.push_back(" " GREY_BG "     " BLUE1_BG "     " BLUE2_BG "     " BLUE3_BG "     " RESET);
    return sysInfo;
}

void printFetch() {
    std::cout << "\033[2J\033[H"; // New frame / remove terminal content

    std::vector<std::string> logo = getAppleLogo(); // Get logo
    std::vector<std::string> sysInfo = getSystemInfo(); // System Information
    
    std::size_t logoWidth = 40;
    std::size_t spacing = 2;
    
    std::size_t maxLines = std::max(logo.size(), sysInfo.size());
    for (std::size_t i = 0; i < 3; i++) std::cout << std::endl;

    for (std::size_t i = 0; i < maxLines; i++) {
        // Print logo
        if (i < logo.size()) 
            std::cout << std::left << std::setw(logoWidth) << logo[i];
        else 
            std::cout << std::left << std::setw(logoWidth) << " ";

        std::cout << std::string(spacing, ' '); // Placeholder

        // Print system information
        if (i < sysInfo.size()) 
            std::cout << sysInfo[i];

        std::cout << std::endl;
    }

    for (std::size_t i = 0; i < 3; i++) std::cout << std::endl;

    std::cout << std::flush;
}

int main() {
    printFetch();
    return 0;
}