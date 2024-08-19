#include <array>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <sys/utsname.h>
#include <thread>
#include <vector>
#include "inipp.hpp"

struct utsname unameData;

int threads = std::thread::hardware_concurrency();

int color_to_num(std::string & color) {
  const std::map<std::string, std::int32_t> m_ColorMap {
    { "red", 31 },
    { "green", 32 },
    { "yellow", 33 },
    { "blue", 34 },
    { "magenta", 35 },
    { "cyan", 36 },
    { "white", 0 },
    { "black", 30 },
  };

  const auto item = m_ColorMap.find(color);
  if (item  != m_ColorMap.end()) {
    return item->second;                
  } else {
    return 0;
  }
}

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
    ret.push_back(std::stoi(total) / 1000000);
    double used = std::stoi(total) - std::stoi(available);
    ret.push_back(used / 1000000);
    meminfo.close();
    return ret;
  }
};

void fumo(std::string distro, std::string kernel, int packages,
          std::string desktop, std::string Distro_color, std::string Kernel_color, std::string Packages_color,
          std::string Desktop_color, inipp::Ini<char> ini) {
  
  std::string Arch_color, CPU_color, GPU_color, Init_color, Shell_color, Mem_color;
  inipp::get_value(ini.sections["colors"], "Arch", Arch_color);
  inipp::get_value(ini.sections["colors"], "CPU", CPU_color);
  inipp::get_value(ini.sections["colors"], "GPU", GPU_color);
  inipp::get_value(ini.sections["colors"], "Init", Init_color);
  inipp::get_value(ini.sections["colors"], "Shell", Shell_color);
  inipp::get_value(ini.sections["colors"], "Mem", Mem_color);

  std::cout << "⠀⢀⣒⠒⠆⠤⣀⡀⠀" << '\n';
  std::cout <<"⢠⡛⠛⠻⣷⣶⣦⣬⣕⡒⠤⢀⣀⠀" << '\n';
  std::cout << "⡿⢿⣿⣿⣿⣿⣿⡿⠿⠿⣿⣳⠖⢋⣩⣭⣿⣶⡤⠶⠶⢶⣒⣲⢶⣉⣐⣒⣒⣒⢤⡀⠀" << '\n';
  std::cout << "⣿⠀⠉⣩⣭⣽⣶⣾⣿⢿⡏⢁⣴⠿⠛⠉⠁⠀⠀⠀⠀⠀⠀⠉⠙⠲⢭⣯⣟⡿⣷⣘⠢⡀⠀" << '\n';
  std::cout << "⠹⣷⣿⣿⣿⣿⣿⢟⣵⠋⢠⡾⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠻⣿⣿⣾⣦⣾⣢⡄" << '\n';
  std::cout << "⠀⠹⣿⣿⣿⡿⣳⣿⠃⠀⣼⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢻⣿⣿⣿⠟⠁⠀" << '\n';
  std::cout << "⠀⠀⠹⣿⣿⣵⣿⠃⠀⠀⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠹⣷⡄⠀⠀⠀⠀" << '\n';
  std::cout << "⠀⠀⠀⠈⠛⣯⡇⠛⣽⣦⣿⠀⠀⠀⠀⢀⠔⠙⣄⠀⠀⠀⠀⠀⠀⣠⠳⡀⠀⠀⠀⠀⢿⡵⡀⠀⠀       Distro:" << "\033[" << color_to_num(Distro_color) << "m " << distro.c_str() << "\033[0m\n";
  std::cout << "⠀⠀⠀⠀⣸⣿⣿⣿⠿⢿⠟⠀⠀⠀⢀⡏⠀⠀⠘⡄⠀⠀⠀⠀⢠⠃⠀⠹⡄⠀⠀⠀⠸⣿⣷⡀⠀⠀      Kernel:" << "\033[" << color_to_num(Kernel_color) << "m " << kernel.c_str() << "\033[0m\n";
  std::cout << "⠀⠀⠀⠸⣿⣿⠟⢹⣥⠀⠀⠀⠀⠀⣸⣀⣀⣤⣀⣀⠈⠳⢤⡀⡇⣀⣠⣄⣸⡆⠀⠀⠀⡏⠀⠀        Packages:" << "\033[" << color_to_num(Packages_color) << "m " << packages << "\033[0m\n" ;
  std::cout << "⠀⠀⠀⠀⠁⠁⠀⢸⢟⡄⠀⠀⠀⠀⣿⣾⣿⣿⣿⣿⠁⠀⠈⠙⠙⣯⣿⣿⣿⡇⠀⠀⢠⠃          Desktop:" << "\033[" << color_to_num(Desktop_color) << "m " << desktop.c_str() << "\033[0m\n" ;
  std::cout << "⠀⠀⠀⠀⠀⠀⠀⠇⢨⢞⢆⠀⠀⠀⡿⣿⣿⣿⣿⡏⠀⠀⠀⠀⠀⣿⣿⣿⡿⡇⠀⣠⢟⡄          Arch:" << "\033[" << color_to_num(Arch_color) << "m " << SystemInfoExt().arch() << "\033[0m\n" ;
  std::cout << "⠀⠀⠀⠀⠀⠀⡼⠀⢈⡏⢎⠳⣄⠀⡇⠙⠛⠟⠛⠀⠀⠀⠀⠀⠀⠘⠻⠛⢱⢃⡜⡝⠈⠚⡄         CPU:" << "\033[" << color_to_num(CPU_color) << "m " << SystemInfoExt().cpu().c_str()  << " (" << threads << ")" << "\033[0m\n" ;
  std::cout << "⠀⠀⠀⠀⠀⠘⣅⠁⢸⣋⠈⢣⡈⢷⠇⠀⠀⠀⠀⠀⣄⠀⠀⢀⡄⠀⠀⣠⣼⢯⣴⠇⣀⡀⢸         GPU:" << "\033[" << color_to_num(GPU_color) << "m " << SystemInfoExt().gpu().c_str() << "\033[0m\n";
  std::cout << "⠀⠀⠀⠀⠀⠀⠈⠳⡌⠛⣶⣆⣷⣿⣦⣄⣀⠀⠀⠀⠈⠉⠉⢉⣀⣤⡞⢛⣄⡀⢀⡨⢗⡦⠎         Init:" << "\033[" << color_to_num(Init_color) << "m " << SystemInfoExt().init() << "\033[0m\n" ;
  std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠈⠑⠪⣿⠁⠀⠐⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣏⠉⠁⢸⠀⠀⠀⠄⠙⡆         Shell:" << "\033[" << color_to_num(Shell_color) << "m " << getenv("SHELL") << "\033[0m\n";
  std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⣀⠤⠚⡉⢳⡄⠡⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣏⠁⣠⣧⣤⣄⣀⡀⡰⠁         Mem:" << "\033[" << color_to_num(Mem_color) << "m " << SystemInfoExt().mem()[1] << "/" << SystemInfoExt().mem()[0] << " GB" << "\033[0m\n";
  std::cout << "⠀⠀⠀⠀⠀⢀⠔⠉⠀⠀⠀⠀⢀⣧⣠⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣅⡀\n";
  std::cout << "⠀⠀⠀⠀⠀⢸⠆⠀⠀⠀⣀⣼⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⠟⠋⠁⣠⠖⠒⠒⠛⢿⣆\n";
  std::cout << "⠀⠀⠀⠀⠀⠀⠑⠤⠴⠞⢋⣵⣿⢿⣿⣿⣿⣿⣿⣿⠗⣀⠀⠀⠀⠀⠀⢰⠇⠀⠀⠀⠀⢀⡼⣶⣤\n";
  std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⡠⠟⢛⣿⠀⠙⠲⠽⠛⠛⠵⠞⠉⠙⠳⢦⣀⣀⡞⠀⠀⠀⠀⡠⠋⠐⠣⠮⡁\n";
  std::cout << "⠀⠀⠀⠀⠀⠀⠀⢠⣎⡀⢀⣾⠇⢀⣠⡶⢶⠞⠋⠉⠉⠒⢄⡀⠉⠈⠉⠀⠀⠀⣠⣾⠀⠀⠀⠀⠀⢸⡀\n";
  std::cout << "⠀⠀⠀⠀⠀⠀⠀⠘⣦⡀⠘⢁⡴⢟⣯⣞⢉⠀⠀⠀⠀⠀⠀⢹⠶⠤⠤⡤⢖⣿⡋⢇⠀⠀⠀⠀⠀⢸⠀\n";
  std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⠵⠗⠺⠟⠖⢈⡣⡄⠀⠀⠀⠀⢀⣼⡤⣬⣽⠾⠋⠉⠑⠺⠧⣀⣤⣤⡠⠟⠃\n";
  std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠛⠷⠶⠦⠶⠞⠉" << '\n';
}
