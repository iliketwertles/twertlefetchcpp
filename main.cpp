#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <cstring>
#include <ctime>
#include "extra.cpp"
#include <sys/stat.h>
#include <sys/utsname.h>

struct stat fileInfo;

int main(int argc, char* argv[]) {
    namespace fs = std::filesystem;
    // defining
    const time_t today = std::time(0);
    std::string distro, kernel, packages, desktop, osrelease_s, line;
    time_t birth_date;
    std::ifstream osrelease ("/etc/os-release");

    // reading /etc/os-release to get distro name, under NAME=
    if ( osrelease.is_open() ) {
        std::string name = "NAME=";
        while ( std::getline(osrelease, line) ) {
            size_t found = line.find(name);
            if ( found != std::string::npos ) {
                distro = line.substr(found + name.length());

                distro.erase(0, distro.find_first_not_of(" \t"));
                distro.erase(distro.find_last_not_of(" \t") + 1);

                break;
            }
        }
    } else {
        std::cout << "/etc/os-release isnt open!" << std::endl;
        return 1;
    }
    osrelease.close();
    
    // getting kernel version
    uname(&unameData);
    kernel = unameData.release;
    
    // getting packages, only works for emerge from gentoo
    if (fs::is_directory("/var/db/pkg")) {
        int count = 0;
        for (const auto& entry : fs::directory_iterator("/var/db/pkg")) {
            if (fs::is_directory(entry.path())) {
                for (const auto& sub_entry : fs::directory_iterator(entry.path())) {
                    if (fs::is_directory(sub_entry.path())) {
                        count++;
                    }
                }
            }
        }
        packages = std::to_string(count);
    } else {
        packages = "unsupported :(";
    }

    // getting desktop env, if it cant read current desktop
    // it reads desktop session, if both are null then you
    // dont have a desktop
    if ( getenv("XDG_CURRENT_DESKTOP") != NULL ) {
        desktop = getenv("XDG_CURRENT_DESKTOP");
    } else if ( getenv("DESKTOP_SESSION") != NULL ) {
        desktop = getenv("DESKTOP_SESSION");
    } else {
        desktop = "i dunno :(";
    }

    // getting date of linux install (theoretical)
    // uses metadata of the path / to simulate the 
    // oldest thing, obv doesnt work  if you 
    // reformatted root
    if ( stat("/", &fileInfo) == 0 ) {
        birth_date = fileInfo.st_ctime;
    } else {
        birth_date = 0;
    }
    double diff = std::difftime(today, birth_date) / (60 * 60 * 24);

    // printing everything out, idk a better way to do it 
    if (argc >= 2 && strcmp(argv[1], "-f") == 0) {
        fumo(distro, kernel, packages, desktop, diff);
        return 0;
    } else {
        std::cout << "     .--." << '\n';
        std::cout << "    |o_o |    Distro: " << "\033[36m" << distro <<  "\033[0m" << '\n';
        std::cout << "    |:_/ |    Kernel: " << "\033[33m" << kernel <<  "\033[0m" << '\n';
        std::cout << R"(   //   \ \   Packages: )" << "\033[34m" << packages <<  "\033[0m" << '\n';
        std::cout << "  (|     | )  Desktop: " << "\033[31m" << desktop <<  "\033[0m" << '\n';
        std::cout << R"( /'\_   _/`\ )" << '\n';
        std::cout << R"( \___)=(___/  You installed )" << (int) diff << " days ago" << '\n';
    }
    return 0;
}