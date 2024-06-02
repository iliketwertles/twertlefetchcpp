#include <array>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <sys/utsname.h>
#include <vector>

struct utsname unameData;

std::string trim(const std::string &line) {
  const char *WhiteSpace = " \t\v\r\n";
  std::size_t start = line.find_first_not_of(WhiteSpace);
  std::size_t end = line.find_last_not_of(WhiteSpace);
  return start == end ? std::string() : line.substr(start, end - start + 1);
}

std::string exec(const char *cmd) {
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

class SystemInfoExt {
public:
  const char *arch() const {
    // getting arch
    uname(&unameData);
    const char *arch = unameData.machine;
    return arch;
  }

  const std::string cpu() const {
    // getting cpu info
    std::string cpuinfo_s, cpu, line, init_name;
    std::ifstream cpuinfo("/proc/cpuinfo");
    if (cpuinfo.is_open()) {
      std::string model = "model name";
      while (std::getline(cpuinfo, line)) {
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
    return cpu;
  }

  const std::string gpu() const {
    return trim(exec("lspci | grep -m 1 ' VGA ' | cut -d\" \" -f 9-"));
  }

  const char *init() const {
    if (std::filesystem::exists("/etc/systemd/system.conf")) {
      return "Systemd";
    } else if (std::filesystem::exists("/etc/init.d")) {
      return "OpenRC";
    } else if (std::filesystem::exists("/etc/runit/1")) {
      return "Runit";
    } else {
      return "i dunno";
    }
  }

  // total = mem[0]
  // used = mem[1]
  const std::vector<int> mem() const {
    std::ifstream meminfo("/proc/meminfo");
    std::string total, available, line;
    std::vector<int> ret;
    if (meminfo.is_open()) {
      total = "MemTotal:";
      available = "MemAvailable:";
      while (std::getline(meminfo, line)) {
        size_t foundt = line.find(total);
        size_t founda = line.find(available);
        if (foundt != std::string::npos) {
          total = line.substr(foundt + total.length());
          total.erase(0, total.find_first_not_of(" \t"));
          total.erase(total.find_last_not_of(" \t") + 1);
          total = total.substr(0, total.size() - 3);
        } else if (founda != std::string::npos) {
          available = line.substr(founda + available.length());
          available.erase(0, available.find_first_not_of(" \t"));
          available.erase(available.find_last_not_of(" \t") + 1);
          available = available.substr(0, available.size() - 3);
        }
        if (total != "MemTotal:" && available != "MemAvailable:") {
          break;
        }
      }
    }
    ret.push_back(std::stoi(total));
    ret.push_back(std::stoi(total) - std::stoi(available));
    meminfo.close();
    return ret;
  }
};

void fumo(std::string distro, std::string kernel, int packages,
          std::string desktop, double diff) {

  printf("⠀⢀⣒⠒⠆⠤⣀⡀⠀\n");
  printf("⢠⡛⠛⠻⣷⣶⣦⣬⣕⡒⠤⢀⣀⠀\n");
  printf("⡿⢿⣿⣿⣿⣿⣿⡿⠿⠿⣿⣳⠖⢋⣩⣭⣿⣶⡤⠶⠶⢶⣒⣲⢶⣉⣐⣒⣒⣒⢤⡀⠀\n");
  printf("⣿⠀⠉⣩⣭⣽⣶⣾⣿⢿⡏⢁⣴⠿⠛⠉⠁⠀⠀⠀⠀⠀⠀⠉⠙⠲⢭⣯⣟⡿⣷⣘⠢⡀\n");
  printf("⠹⣷⣿⣿⣿⣿⣿⢟⣵⠋⢠⡾⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠻⣿⣿⣾⣦⣾⣢\n");
  printf("⠀⠹⣿⣿⣿⡿⣳⣿⠃⠀⣼⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢻⣿⣿⣿⠟\n");
  printf("⠀⠀⠹⣿⣿⣵⣿⠃⠀⠀⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠹⣷⡄\n");
  printf("⠀⠀⠀⠈⠛⣯⡇⠛⣽⣦⣿⠀⠀⠀⠀⢀⠔⠙⣄⠀⠀⠀⠀⠀⠀⣠⠳⡀⠀⠀⠀⠀⢿⡵⡀⠀⠀       Distro:\033[36m %s "
         "\033[0m \n",
         distro.c_str());
  printf("⠀⠀⠀⠀⣸⣿⣿⣿⠿⢿⠟⠀⠀⠀⢀⡏⠀⠀⠘⡄⠀⠀⠀⠀⢠⠃⠀⠹⡄⠀⠀⠀⠸⣿⣷⡀⠀⠀      Kernel:\033[33m %s "
         "\033[0m \n",
         kernel.c_str());
  printf("⠀⠀⠀⠸⣿⣿⠟⢹⣥⠀⠀⠀⠀⠀⣸⣀⣀⣤⣀⣀⠈⠳⢤⡀⡇⣀⣠⣄⣸⡆⠀⠀⠀⡏⠀⠀        Packages:\033[34m %i "
         "\033[0m \n",
         packages);
  printf("⠀⠀⠀⠀⠁⠁⠀⢸⢟⡄⠀⠀⠀⠀⣿⣾⣿⣿⣿⣿⠁⠀⠈⠙⠙⣯⣿⣿⣿⡇⠀⠀⢠⠃          Desktop:\033[31m %s "
         "\033[0m \n",
         desktop.c_str());
  printf(
      "⠀⠀⠀⠀⠀⠀⠀⠇⢨⢞⢆⠀⠀⠀⡿⣿⣿⣿⣿⡏⠀⠀⠀⠀⠀⣿⣿⣿⡿⡇⠀⣠⢟⡄          Arch:\033[36m %s \033[0m \n",
      SystemInfoExt().arch());
  printf(
      "⠀⠀⠀⠀⠀⠀⡼⠀⢈⡏⢎⠳⣄⠀⡇⠙⠛⠟⠛⠀⠀⠀⠀⠀⠀⠘⠻⠛⢱⢃⡜⡝⠈⠚⡄         CPU:\033[33m %s \033[0m \n",
      SystemInfoExt().cpu().c_str());
  printf(
      "⠀⠀⠀⠀⠀⠘⣅⠁⢸⣋⠈⢣⡈⢷⠇⠀⠀⠀⠀⠀⣄⠀⠀⢀⡄⠀⠀⣠⣼⢯⣴⠇⣀⡀⢸         GPU:\033[34m %s \033[0m \n",
      SystemInfoExt().gpu().c_str());
  printf(
      "⠀⠀⠀⠀⠀⠀⠈⠳⡌⠛⣶⣆⣷⣿⣦⣄⣀⠀⠀⠀⠈⠉⠉⢉⣀⣤⡞⢛⣄⡀⢀⡨⢗⡦⠎         Init:\033[31m %s \033[0m \n",
      SystemInfoExt().init());
  printf("⠀⠀⠀⠀⠀⠀⠀⠀⠈⠑⠪⣿⠁⠀⠐⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣏⠉⠁⢸⠀⠀⠀⠄⠙⡆         Shell:\033[36m %s "
         "\033[0m \n",
         getenv("SHELL"));
  printf("⠀⠀⠀⠀⠀⠀⠀⠀⣀⠤⠚⡉⢳⡄⠡⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣏⠁⣠⣧⣤⣄⣀⡀⡰⠁         Mem:\033[33m %i/%imb "
         "\033[0m \n",
         SystemInfoExt().mem()[1] / 1024, SystemInfoExt().mem()[0] / 1024);
  printf("⠀⠀⠀⠀⠀⢀⠔⠉⠀⠀⠀⠀⢀⣧⣠⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣅⡀\n");
  printf("⠀⠀⠀⠀⠀⢸⠆⠀⠀⠀⣀⣼⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⠟⠋⠁⣠⠖⠒⠒⠛⢿⣆\n");
  printf("⠀⠀⠀⠀⠀⠀⠑⠤⠴⠞⢋⣵⣿⢿⣿⣿⣿⣿⣿⣿⠗⣀⠀⠀⠀⠀⠀⢰⠇⠀⠀⠀⠀⢀⡼⣶⣤\n");
  printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⡠⠟⢛⣿⠀⠙⠲⠽⠛⠛⠵⠞⠉⠙⠳⢦⣀⣀⡞⠀⠀⠀⠀⡠⠋⠐⠣⠮⡁\n");
  printf("⠀⠀⠀⠀⠀⠀⠀⢠⣎⡀⢀⣾⠇⢀⣠⡶⢶⠞⠋⠉⠉⠒⢄⡀⠉⠈⠉⠀⠀⠀⣠⣾⠀⠀⠀⠀⠀⢸⡀\n");
  printf("⠀⠀⠀⠀⠀⠀⠀⠘⣦⡀⠘⢁⡴⢟⣯⣞⢉⠀⠀⠀⠀⠀⠀⢹⠶⠤⠤⡤⢖⣿⡋⢇⠀⠀⠀⠀⠀⢸⠀\n");
  printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⠵⠗⠺⠟⠖⢈⡣⡄⠀⠀⠀⠀⢀⣼⡤⣬⣽⠾⠋⠉⠑⠺⠧⣀⣤⣤⡠⠟⠃\n"); // ignore this V
  std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠛⠷⠶⠦⠶⠞⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀    You installed "
            << (int)diff << " days ago" << '\n';
}
