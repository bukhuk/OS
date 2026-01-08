#include <dirent.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#include <cctype>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

bool ProcessExists(pid_t pid) {
    return kill(pid, 0) == 0;
}

bool TerminateProcessByID(pid_t process_id) {
    if (!ProcessExists(process_id)) {
        std::cerr << "Process with ID " << process_id << " does not exist.\n";
        return false;
    }

    if (kill(process_id, SIGTERM) == 0) {
        std::cout << "Process with ID " << process_id << " terminated successfully.\n";
        return true;
    } else {
        std::cerr << "Failed to terminate process with ID " << process_id << "\n";
        return false;
    }
}

std::string GetProcessName(pid_t pid) {
    std::string proc_path = "/proc/" + std::to_string(pid) + "/comm";
    std::ifstream file(proc_path);
    if (file.is_open()) {
        std::string name;
        std::getline(file, name);
        return name;
    }
    return "";
}

bool TerminateProcessByName(const std::string& process_name) {
    bool found = false;

    DIR* dir = opendir("/proc");
    if (!dir) {
        std::cerr << "Cannot open /proc directory\n";
        return false;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        bool is_pid = true;
        for (int i = 0; entry->d_name[i] != '\0'; i++) {
            if (!std::isdigit(static_cast<unsigned char>(entry->d_name[i]))) {
                is_pid = false;
                break;
            }
        }

        if (!is_pid) continue;

        pid_t pid = std::atoi(entry->d_name);
        std::string name = GetProcessName(pid);

        if (!name.empty() && name == process_name) {
            std::cout << "Found process: " << name << " (ID: " << pid << ")\n";
            if (TerminateProcessByID(pid)) {
                found = true;
            }
        }
    }

    closedir(dir);

    if (!found) {
        std::cout << "No processes with name '" << process_name << "' found.\n";
    }

    return found;
}

std::vector<std::string> SplitString(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;

    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }

    return result;
}

std::string TrimString(const std::string& str) {
    const std::string whitespace = " \t\n\r";
    size_t start = str.find_first_not_of(whitespace);
    if (start == std::string::npos) return "";

    size_t end = str.find_last_not_of(whitespace);
    return str.substr(start, end - start + 1);
}

void TerminateFromEnvironment() {
    const char* proc_list = std::getenv("PROC_TO_KILL");

    if (proc_list == nullptr || std::strlen(proc_list) == 0) {
        std::cout << "Environment variable PROC_TO_KILL is not set.\n";
        return;
    }

    std::cout << "PROC_TO_KILL = " << proc_list << "\n";

    std::vector<std::string> processes = SplitString(proc_list, ',');

    for (const auto& proc_name : processes) {
        std::string trimmed_name = TrimString(proc_name);

        if (!trimmed_name.empty()) {
            std::cout << "Terminating processes with name: " << trimmed_name << "\n";
            TerminateProcessByName(trimmed_name);
        }
    }
}

void PrintHelp() {
    std::cout << "Usage: Killer [options]\n"
              << "Options:\n"
              << "  --id <PID>      : Terminate process by specified ID\n"
              << "  --name <name>   : Terminate all processes with specified name\n"
              << "  --help          : Show this help message\n\n"
              << "Also terminates processes from PROC_TO_KILL environment variable\n";
}

int main(int argc, char* argv[]) {
    std::cout << "Killer process started\n";

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "--id" && i + 1 < argc) {
            pid_t pid = std::atoi(argv[++i]);
            std::cout << "Terminating process by ID: " << pid << "\n";
            TerminateProcessByID(pid);
        } else if (arg == "--name" && i + 1 < argc) {
            std::string name = argv[++i];
            std::cout << "Terminating processes with name: " << name << "\n";
            TerminateProcessByName(name);
        } else if (arg == "--help") {
            PrintHelp();
            return 0;
        } else {
            std::cerr << "Unknown parameter: " << arg << "\n"
                      << "Use --help for usage information.\n";
            return 1;
        }
    }

    std::cout << "\nChecking PROC_TO_KILL environment variable\n";
    TerminateFromEnvironment();

    std::cout << "\nKiller process finished\n";
    return 0;
}