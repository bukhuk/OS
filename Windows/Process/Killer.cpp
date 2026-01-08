#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>

std::wstring StringToWide(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), NULL, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), &wstr[0], size_needed);
    return wstr;
}

std::string WideToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], static_cast<int>(wstr.size()), NULL, 0, NULL, NULL);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], static_cast<int>(wstr.size()), &str[0], size_needed, NULL, NULL);
    return str;
}

bool TerminateProcessByID(DWORD process_id) {
    HANDLE h_process = OpenProcess(PROCESS_TERMINATE, FALSE, process_id);
    if (h_process == NULL) {
        std::cerr << "Failed to open process with ID " << process_id << "\n";
        return false;
    }

    bool result = TerminateProcess(h_process, 0);
    if (!result) {
        std::cerr << "Failed to terminate process with ID " << process_id << "\n";
    } else {
        std::cout << "Process with ID " << process_id << " terminated successfully.\n";
    }

    CloseHandle(h_process);
    return result;
}



bool TerminateProcessByName(const std::string& process_name) {
    bool found = false;
    HANDLE h_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (h_snapshot == INVALID_HANDLE_VALUE) return false;

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    if (!Process32FirstW(h_snapshot, &pe32)) {
        CloseHandle(h_snapshot);
        return false;
    }

    std::wstring w_process_name = StringToWide(process_name);

    do {
        if (_wcsicmp(pe32.szExeFile, w_process_name.c_str()) == 0) {
            std::cout << "Found process: " << WideToString(pe32.szExeFile)
                      << " (ID: " << pe32.th32ProcessID << ")\n";
            if (TerminateProcessByID(pe32.th32ProcessID)) {
                found = true;
            }
        }
    } while (Process32NextW(h_snapshot, &pe32));

    CloseHandle(h_snapshot);

    if (!found) {
        std::cout << "No processes with name '" << process_name << "' found.\n";
    }

    return found;
}

std::vector<std::string> SplitString(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = str.find(delimiter);

    while (end != std::string::npos) {
        result.push_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delimiter, start);
    }

    result.push_back(str.substr(start));
    return result;
}

void TerminateFromEnvironment() {
    char buffer[4096];
    DWORD length = GetEnvironmentVariableA("PROC_TO_KILL", buffer, sizeof(buffer));

    if (length == 0) {
        std::cout << "Environment variable PROC_TO_KILL is not set.\n";
        return;
    }

    if (length >= sizeof(buffer)) {
        std::cerr << "PROC_TO_KILL variable is too long.\n";
        return;
    }

    std::string proc_list = buffer;
    std::cout << "PROC_TO_KILL = " << proc_list << "\n";

    std::vector<std::string> processes = SplitString(proc_list, ',');

    for (const auto& proc_name : processes) {
        std::string trimmed_name = proc_name;
        size_t start = trimmed_name.find_first_not_of(" \t");
        size_t end = trimmed_name.find_last_not_of(" \t");

        if (start != std::string::npos && end != std::string::npos) {
            trimmed_name = trimmed_name.substr(start, end - start + 1);

            if (!trimmed_name.empty()) {
                std::cout << "Terminating processes with name: " << trimmed_name << "\n";
                TerminateProcessByName(trimmed_name);
            }
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
            DWORD pid = static_cast<DWORD>(std::atoi(argv[++i]));
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