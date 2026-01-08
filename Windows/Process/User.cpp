#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>
#include <vector>

std::wstring StringToWide(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), NULL, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), &wstr[0], size_needed);
    return wstr;
}

bool IsProcessRunning(const std::string& process_name) {
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
            CloseHandle(h_snapshot);
            return true;
        }
    } while (Process32NextW(h_snapshot, &pe32));

    CloseHandle(h_snapshot);
    return false;
}

int main() {
    std::cout << "User process started\n";

    // Проверка наличия исполняемого файла через WinAPI
    if (GetFileAttributesA("Killer.exe") == INVALID_FILE_ATTRIBUTES) {
        std::cout << "\nERROR: Killer.exe not found in current directory!\n"
                  << "Please compile it first.\n";
        return 1;
    }

    std::cout << "\nKiller.exe found. Starting demonstration\n"
              << "\nKiller demonstration\n";

    std::cout << "\n1. Setting environment variable PROC_TO_KILL=notepad.exe,mspaint.exe\n";
    SetEnvironmentVariableA("PROC_TO_KILL", "notepad.exe,mspaint.exe");



    std::cout << "\n2. Launching test processes\n";
    std::system("start notepad.exe");
    std::system("start mspaint.exe");
    Sleep(3000);



    std::cout << "\n3. Checking running processes:\n"
              << "   Notepad.exe running: " << (IsProcessRunning("notepad.exe") ? "yes" : "no") << "\n"
              << "   MSPaint.exe running: " << (IsProcessRunning("mspaint.exe") ? "yes" : "no") << "\n";

    std::cout << "\n4. Running Killer without parameters (uses PROC_TO_KILL)\n";
    std::system("Killer.exe");

    std::cout << "\n5. After Killer:\n"
              << "   Notepad.exe running: " << (IsProcessRunning("notepad.exe") ? "yes" : "no") << "\n"
              << "   MSPaint.exe running: " << (IsProcessRunning("mspaint.exe") ? "yes" : "no") << "\n";

    std::cout << "\n6. Removing PROC_TO_KILL environment variable\n";
    SetEnvironmentVariableA("PROC_TO_KILL", NULL);

    std::cout << "\nDemonstration finished\n";
    return 0;
}