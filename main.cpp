#include "extra.cpp"
#include <array>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <vector>

struct stat fileInfo;

class SystemInfo {
public:
  const std::string distro() const {
    // getting distro name
    std::string distro_s, line;
    std::ifstream osrelease("/etc/os-release");
    if (osrelease.is_open()) {
      std::string name = "NAME=";
      while (std::getline(osrelease, line)) {
        size_t found = line.find(name);
        if (found != std::string::npos) {
          distro_s = line.substr(found + name.length());
          distro_s.erase(0, distro_s.find_first_not_of(" \t"));
          distro_s.erase(distro_s.find_last_not_of(" \t") + 1);
          break;
        }
      }
    } else {
      distro_s = "Unknown";
    }
    osrelease.close();
    return distro_s;
  }

  const char *kernel() const {
    uname(&unameData);
    return unameData.release;
  }

  const int packages() const {
    // getting packages, only works for emerge from gentoo
    if (!std::filesystem::is_directory("/var/db/pkg")) {
      return 0;
    }
    int count = 0;
    for (const auto &entry :
         std::filesystem::directory_iterator("/var/db/pkg")) {
      if (std::filesystem::is_directory(entry.path())) {
        for (const auto &sub_entry :
             std::filesystem::directory_iterator(entry.path())) {
          if (std::filesystem::is_directory(sub_entry.path())) {
            count++;
          }
        }
      }
    }
    return count;
  }

  const char *desktop() const {
    if (getenv("XDG_CURRENT_DESKTOP") != NULL) {
      return getenv("XDG_CURRENT_DESKTOP");
    } else if (getenv("DESKTOP_SESSION") != NULL) {
      return getenv("DESKTOP_SESSION");
    } else {
      return "i dunno :(";
    }
  }

  const int DoB() const {
    const time_t &today = std::time(0);
    if (stat("/", &fileInfo) == 0) {
      const time_t &birth_date = fileInfo.st_ctime;
      return std::difftime(today, birth_date) / (60 * 60 * 24);
    } else {
      return 999;
    }
  }
};

int main(int argc, char *argv[]) {
  // printing everything out, idk a better way to do it
  if (argc >= 2 && strcmp(argv[1], "-f") == 0) {
    fumo(SystemInfo().distro(), SystemInfo().kernel(), SystemInfo().packages(),
         SystemInfo().desktop(), SystemInfo().DoB());
    return 0;
  } else {
    std::cout << "     .--." << '\n';
    std::cout << "    |o_o |    Distro: "
              << "\033[36m" << SystemInfo().distro() << "\033[0m" << '\n';
    std::cout << "    |:_/ |    Kernel: "
              << "\033[33m" << SystemInfo().kernel() << "\033[0m" << '\n';
    std::cout << R"(   //   \ \   Packages: )"
              << "\033[34m" << SystemInfo().packages() << "\033[0m" << '\n';
    std::cout << "  (|     | )  Desktop: "
              << "\033[31m" << SystemInfo().desktop() << "\033[0m" << '\n';
    std::cout << R"( /'\_   _/`\ )" << '\n';
    std::cout << R"( \___)=(___/  You installed )" << SystemInfo().DoB()
              << " days ago" << '\n';
  }
  return 0;
}
