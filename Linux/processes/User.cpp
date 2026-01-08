#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

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

bool IsProcessRunning(const std::string& process_name) {
    DIR* dir = opendir("/proc");
    if (!dir) {
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

        if (!is_pid) {
            continue;
        }

        pid_t pid = std::atoi(entry->d_name);
        std::string name = GetProcessName(pid);

        if (!name.empty() && name == process_name) {
            closedir(dir);
            return true;
        }
    }

    closedir(dir);
    return false;
}

int main() {
    std::cout << "User process started\n";

    if (access("Killer", F_OK) != 0) {
        std::cout << "\nERROR: Killer executable not found in current directory!\n"
                  << "Please compile it first: g++ Killer.cpp -o Killer\n";
        return 1;
    }

    std::cout << "\nKiller executable found. Starting demonstration\n"
              << "\nKiller demonstration\n";

    std::cout << "\n1. Setting environment variable PROC_TO_KILL=xterm\n";
    setenv("PROC_TO_KILL", "xterm", 1);

    std::cout << "\n2. Launching xterm process (terminal emulator)\n"
              << "   Command: xterm -hold -e 'echo Test terminal; sleep 10' &\n";
    std::system("xterm -hold -e 'echo Test terminal; sleep 10' &");
    sleep(3);

    std::cout << "\n3. Checking running processes:\n"
              << "   xterm running: " << (IsProcessRunning("xterm") ? "yes" : "no") << "\n";

    std::cout << "\n4. Running Killer without parameters (uses PROC_TO_KILL)\n";
    std::system("./Killer");
    sleep(3);

    std::cout << "\n5. After Killer:\n"
              << "   xterm running: " << (IsProcessRunning("xterm") ? "yes" : "no") << "\n";

    std::cout << "\n6. Removing PROC_TO_KILL environment variable\n";
    unsetenv("PROC_TO_KILL");

    std::cout << "\n7. Testing Killer with --name parameter\n"
              << "\n   Launching xterm again for testing...\n";
    std::system("xterm -hold -e 'echo Test 2; sleep 10' &");
    sleep(3);

    std::cout << "   xterm running: " << (IsProcessRunning("xterm") ? "yes" : "no") << "\n"
              << "   Testing: ./Killer --name xterm\n";
    std::system("./Killer --name xterm");
    sleep(3);

    std::cout << "   xterm running after --name: " << (IsProcessRunning("xterm") ? "yes" : "no") << "\n";

    std::cout << "\n8. Testing Killer with --id parameter\n"
              << "\n   Launching xterm to get its PID...\n";

    pid_t xterm_pid = 0;
    std::string cmd = "xterm -hold -e 'echo Test PID; sleep 30' & echo $!";
    FILE* pipe = popen(cmd.c_str(), "r");

    if (pipe) {
        char buffer[128];
        if (std::fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            xterm_pid = std::atoi(buffer);
        }
        pclose(pipe);
    }
    sleep(3);

    std::cout << "   xterm PID: " << xterm_pid << "\n"
              << "   xterm running: " << (IsProcessRunning("xterm") ? "yes" : "no") << "\n"
              << "   Testing: ./Killer --id " << xterm_pid << "\n";

    std::string kill_cmd = "./Killer --id " + std::to_string(xterm_pid);
    std::system(kill_cmd.c_str());
    sleep(3);

    std::cout << "   xterm running after --id: " << (IsProcessRunning("xterm") ? "yes" : "no") << "\n";

    std::cout << "\n9. Final cleanup - killing all xterm processes\n";
    std::system("pkill -9 xterm 2>/dev/null");
    sleep(2);
    std::cout << "   Final check - xterm running: " << (IsProcessRunning("xterm") ? "yes" : "no") << "\n";

    std::cout << "\nDemonstration finished\n";
    return 0;
}