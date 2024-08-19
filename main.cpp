#include "extra.cpp"
#include "inipp.hpp"
#include <cstdio>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <unistd.h>

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
    char *_current_desktop = getenv("XDG_CURRENT_DESKTOP");
    char *_desktop_session = getenv("DESKTOP_SESSION");
    if (_current_desktop != NULL) {
      return _current_desktop;
    } else if (_desktop_session != NULL) {
      return _desktop_session;
    } else {
      return "i dunno :(";
    }
  }

};

int main(int argc, char *argv[]) {
  // config file stuff!!
  inipp::Ini<char> ini;
  char * user = getlogin();
  //len should be the total len of both strings + 32 but i dont wanna math
  char config_file[75] = "/home/";
  strcat(config_file, user);
  strcat(config_file, "/.config/twertlefetch/config.ini");
  std::ifstream is(config_file);
  ini.parse(is);
	std::string Distro_color, Kernel_color, Packages_color, Desktop_color;
	inipp::get_value(ini.sections["colors"], "Distro", Distro_color);
  inipp::get_value(ini.sections["colors"], "Kernel", Kernel_color);
  inipp::get_value(ini.sections["colors"], "Packages", Packages_color);
  inipp::get_value(ini.sections["colors"], "Desktop", Desktop_color);

  // printing everything out, idk a better way to do it
  if (argc >= 2 && strcmp(argv[1], "-f") == 0) {
    fumo(SystemInfo().distro(), SystemInfo().kernel(), SystemInfo().packages(),
         SystemInfo().desktop(), Distro_color, Kernel_color, Packages_color, Desktop_color, ini);
    return 0;
  } else {
    std::cout << "     .--." << '\n';
    std::cout << "    |o_o |    Distro: "
              << "\033[" << color_to_num(Distro_color) << "m" << SystemInfo().distro() << "\033[0m" << '\n';
    std::cout << "    |:_/ |    Kernel: "
              << "\033[" << color_to_num(Kernel_color) << "m" << SystemInfo().kernel() << "\033[0m" << '\n';
    std::cout << R"(   //   \ \   Packages: )"
              << "\033[" << color_to_num(Packages_color) << "m" << SystemInfo().packages() << "\033[0m" << '\n';
    std::cout << "  (|     | )  Desktop: "
              << "\033[" << color_to_num(Desktop_color) << "m" << SystemInfo().desktop() << "\033[0m" << '\n';
    std::cout << R"( /'\_   _/`\ )" << '\n';
    std::cout << R"( \___)=(___/ )" << '\n';
  }
  return 0;
}