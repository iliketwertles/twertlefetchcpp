#include <stdio.h>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/utsname.h>


struct utsname unameData;

std::string trim(const std::string& line) {
    const char* WhiteSpace = " \t\v\r\n";
    std::size_t start = line.find_first_not_of(WhiteSpace);
    std::size_t end = line.find_last_not_of(WhiteSpace);
    return start == end ? std::string() : line.substr(start, end - start + 1);
}

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}


void fumo(std::string distro, std::string kernel, std::string packages, std::string desktop, double diff) {

    // getting arch
    uname(&unameData);
    std::string arch = unameData.machine;

    //getting cpu info
    std::string cpuinfo_s, cpu, line, init_name;
    std::ifstream cpuinfo ("/proc/cpuinfo");
    if ( cpuinfo.is_open() ) {
        std::string model = "model name";
        while ( std::getline(cpuinfo, line) ) {
            size_t found = line.find(model);
            if (found != std::string::npos) {
                cpu = line.substr(found + 13);

                cpu.erase(0, cpu.find_first_not_of(" \t"));
                cpu.erase(cpu.find_last_not_of(" \t") + 1);
                line = "";

                break;
            }
        }
    } else {
        std::cout << "/proc/cpuinfo isnt open!" << std::endl;
    }

    //getting gpu (please help me)
    //std::string gpu = exec("lspci | grep -m 1 ' VGA ' | cut -d" " -f 9-")

    //getting init
    if ( std::filesystem::exists("/etc/systemd/system.conf") ) {
        init_name = "Systemd";
    } else if ( std::filesystem::exists("/etc/init.d") ) {
        init_name = "OpenRC";
    } else if ( std::filesystem::exists("/etc/runit/1") ) {
        init_name = "Runit";
    } else {
        init_name = "i dunno";
    }

    //getting meminfo
    std::ifstream meminfo ("/proc/meminfo");
    std::string total;
    std::string available;
    if ( meminfo.is_open() ) {
        total = "MemTotal:";
        available = "MemAvailable:";
        while ( std::getline(meminfo, line) ) {
            size_t foundt = line.find(total);
            size_t founda = line.find(available);
            if ( foundt != std::string::npos ) {
                total = line.substr(foundt + total.length());
                total.erase(0, total.find_first_not_of(" \t"));
                total.erase(total.find_last_not_of(" \t") + 1);
                total = total.substr(0, total.size()-3);
            } else if ( founda != std::string::npos ) {
                available = line.substr(founda + available.length());
                available.erase(0, available.find_first_not_of(" \t"));
                available.erase(available.find_last_not_of(" \t") + 1);
                available = available.substr(0, available.size()-3);
            }
            if ( total != "MemTotal:" && available != "MemAvailable:" ) { break; }
        }
    }
    meminfo.close();
    int used = std::stoi(total) - std::stoi(available);


    printf("⠀⢀⣒⠒⠆⠤⣀⡀⠀\n");
    printf("⢠⡛⠛⠻⣷⣶⣦⣬⣕⡒⠤⢀⣀⠀\n");
    printf("⡿⢿⣿⣿⣿⣿⣿⡿⠿⠿⣿⣳⠖⢋⣩⣭⣿⣶⡤⠶⠶⢶⣒⣲⢶⣉⣐⣒⣒⣒⢤⡀⠀\n");
    printf("⣿⠀⠉⣩⣭⣽⣶⣾⣿⢿⡏⢁⣴⠿⠛⠉⠁⠀⠀⠀⠀⠀⠀⠉⠙⠲⢭⣯⣟⡿⣷⣘⠢⡀\n");
    printf("⠹⣷⣿⣿⣿⣿⣿⢟⣵⠋⢠⡾⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠻⣿⣿⣾⣦⣾⣢\n");
    printf("⠀⠹⣿⣿⣿⡿⣳⣿⠃⠀⣼⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢻⣿⣿⣿⠟\n");
    printf("⠀⠀⠹⣿⣿⣵⣿⠃⠀⠀⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠹⣷⡄\n");
    printf("⠀⠀⠀⠈⠛⣯⡇⠛⣽⣦⣿⠀⠀⠀⠀⢀⠔⠙⣄⠀⠀⠀⠀⠀⠀⣠⠳⡀⠀⠀⠀⠀⢿⡵⡀⠀⠀       Distro: %s\n", distro.c_str());
    printf("⠀⠀⠀⠀⣸⣿⣿⣿⠿⢿⠟⠀⠀⠀⢀⡏⠀⠀⠘⡄⠀⠀⠀⠀⢠⠃⠀⠹⡄⠀⠀⠀⠸⣿⣷⡀⠀⠀      Kernel: %s\n", kernel.c_str());
    printf("⠀⠀⠀⠸⣿⣿⠟⢹⣥⠀⠀⠀⠀⠀⣸⣀⣀⣤⣀⣀⠈⠳⢤⡀⡇⣀⣠⣄⣸⡆⠀⠀⠀⡏⠀⠀        Packages: %s\n", packages.c_str());
    printf("⠀⠀⠀⠀⠁⠁⠀⢸⢟⡄⠀⠀⠀⠀⣿⣾⣿⣿⣿⣿⠁⠀⠈⠙⠙⣯⣿⣿⣿⡇⠀⠀⢠⠃          Desktop: %s\n", desktop.c_str());
    printf("⠀⠀⠀⠀⠀⠀⠀⠇⢨⢞⢆⠀⠀⠀⡿⣿⣿⣿⣿⡏⠀⠀⠀⠀⠀⣿⣿⣿⡿⡇⠀⣠⢟⡄          Arch: %s\n", arch.c_str());
    printf("⠀⠀⠀⠀⠀⠀⡼⠀⢈⡏⢎⠳⣄⠀⡇⠙⠛⠟⠛⠀⠀⠀⠀⠀⠀⠘⠻⠛⢱⢃⡜⡝⠈⠚⡄         CPU: %s\n", cpu.c_str());
    printf("⠀⠀⠀⠀⠀⠘⣅⠁⢸⣋⠈⢣⡈⢷⠇⠀⠀⠀⠀⠀⣄⠀⠀⢀⡄⠀⠀⣠⣼⢯⣴⠇⣀⡀⢸         GPU: %s\n", trim(exec("lspci | grep -m 1 ' VGA ' | cut -d\" \" -f 9-")).c_str());
    printf("⠀⠀⠀⠀⠀⠀⠈⠳⡌⠛⣶⣆⣷⣿⣦⣄⣀⠀⠀⠀⠈⠉⠉⢉⣀⣤⡞⢛⣄⡀⢀⡨⢗⡦⠎         Init: %s\n", init_name.c_str());
    printf("⠀⠀⠀⠀⠀⠀⠀⠀⠈⠑⠪⣿⠁⠀⠐⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣏⠉⠁⢸⠀⠀⠀⠄⠙⡆         Shell: %s\n", getenv("SHELL"));
    printf("⠀⠀⠀⠀⠀⠀⠀⠀⣀⠤⠚⡉⢳⡄⠡⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣏⠁⣠⣧⣤⣄⣀⡀⡰⠁         Mem: %i/%imb \n", used / 1024, std::stoi(total) / 1024);
    printf("⠀⠀⠀⠀⠀⢀⠔⠉⠀⠀⠀⠀⢀⣧⣠⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣅⡀\n");
    printf("⠀⠀⠀⠀⠀⢸⠆⠀⠀⠀⣀⣼⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⠟⠋⠁⣠⠖⠒⠒⠛⢿⣆\n");
    printf("⠀⠀⠀⠀⠀⠀⠑⠤⠴⠞⢋⣵⣿⢿⣿⣿⣿⣿⣿⣿⠗⣀⠀⠀⠀⠀⠀⢰⠇⠀⠀⠀⠀⢀⡼⣶⣤\n");
    printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⡠⠟⢛⣿⠀⠙⠲⠽⠛⠛⠵⠞⠉⠙⠳⢦⣀⣀⡞⠀⠀⠀⠀⡠⠋⠐⠣⠮⡁\n");
    printf("⠀⠀⠀⠀⠀⠀⠀⢠⣎⡀⢀⣾⠇⢀⣠⡶⢶⠞⠋⠉⠉⠒⢄⡀⠉⠈⠉⠀⠀⠀⣠⣾⠀⠀⠀⠀⠀⢸⡀\n");
    printf("⠀⠀⠀⠀⠀⠀⠀⠘⣦⡀⠘⢁⡴⢟⣯⣞⢉⠀⠀⠀⠀⠀⠀⢹⠶⠤⠤⡤⢖⣿⡋⢇⠀⠀⠀⠀⠀⢸⠀\n");
    printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⠵⠗⠺⠟⠖⢈⡣⡄⠀⠀⠀⠀⢀⣼⡤⣬⣽⠾⠋⠉⠑⠺⠧⣀⣤⣤⡠⠟⠃\n"); // ignore this V
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠛⠷⠶⠦⠶⠞⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀    You installed " << (int) diff << " days ago" << '\n';
}