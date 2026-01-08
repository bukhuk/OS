#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

int main() {
    std::cout << "enter numbers separated by spaces: ";

    std::string input;
    if (!std::getline(std::cin, input) || input.empty()) {
        std::cout << "No input\n";
        return 1;
    }

    std::cout << "\nInput: " << input << "\n";

    HANDLE h_pipe_ma_read, h_pipe_ma_write;
    HANDLE h_pipe_ap_read, h_pipe_ap_write;
    HANDLE h_pipe_ps_read, h_pipe_ps_write;
    HANDLE h_pipe_out_read, h_pipe_out_write;

    SECURITY_ATTRIBUTES sa = {sizeof(sa), nullptr, TRUE};

    if (!CreatePipe(&h_pipe_ma_read, &h_pipe_ma_write, &sa, 0) ||
        !CreatePipe(&h_pipe_ap_read, &h_pipe_ap_write, &sa, 0) ||
        !CreatePipe(&h_pipe_ps_read, &h_pipe_ps_write, &sa, 0) ||
        !CreatePipe(&h_pipe_out_read, &h_pipe_out_write, &sa, 0)) {
        std::cerr << "Failed to create communication pipes\n";
        return 1;
    }

    PROCESS_INFORMATION pi_m, pi_a, pi_p, pi_s;

    // Схема конвейера: Input -> M -> A -> P -> S -> Main


    auto start_proc = [&](const char* cmd, HANDLE in, HANDLE out, PROCESS_INFORMATION& pi) {
        STARTUPINFOA si = {sizeof(si)};
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdInput = in;
        si.hStdOutput = out;
        si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

        if (!CreateProcessA(nullptr, const_cast<LPSTR>(cmd), nullptr, nullptr, TRUE,
                            CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
            std::cerr << "Failed to create process: " << cmd << "\n";
            return false;
        }
        std::cout << "  Process " << cmd << " started (PID: " << pi.dwProcessId << ")\n";
        return true;
    };

    if (!start_proc("S.exe", h_pipe_ps_read, h_pipe_out_write, pi_s) ||
        !start_proc("P.exe", h_pipe_ap_read, h_pipe_ps_write, pi_p) ||
        !start_proc("A.exe", h_pipe_ma_read, h_pipe_ap_write, pi_a)) {
        return 1;
    }

    HANDLE h_input_read, h_input_write;
    if (!CreatePipe(&h_input_read, &h_input_write, &sa, 0)) return 1;

    if (!start_proc("M.exe", h_input_read, h_pipe_ma_write, pi_m)) return 1;

    // Закрываем неиспользуемые дескрипторы в родителе
    CloseHandle(h_pipe_ma_read); CloseHandle(h_pipe_ma_write);
    CloseHandle(h_pipe_ap_read); CloseHandle(h_pipe_ap_write);
    CloseHandle(h_pipe_ps_read); CloseHandle(h_pipe_ps_write);
    CloseHandle(h_pipe_out_write); CloseHandle(h_input_read);

    std::string data = input + "\n";
    DWORD bytes_written;
    WriteFile(h_input_write, data.c_str(), static_cast<DWORD>(data.length()), &bytes_written, nullptr);
    CloseHandle(h_input_write);

    HANDLE processes[4] = {pi_m.hProcess, pi_a.hProcess, pi_p.hProcess, pi_s.hProcess};
    WaitForMultipleObjects(4, processes, TRUE, 2000);

    char buffer[1024];
    DWORD bytes_read;
    if (ReadFile(h_pipe_out_read, buffer, sizeof(buffer) - 1, &bytes_read, nullptr) && bytes_read > 0) {
        buffer[bytes_read] = '\0';
        std::string result(buffer);
        size_t pos = result.find_last_not_of(" \t\n\r");
        if (pos != std::string::npos) result = result.substr(0, pos + 1);
        std::cout << "Result (sum): " << result << "\n";
    }

    // Очистка
    CloseHandle(h_pipe_out_read);
    for (int i = 0; i < 4; i++) {
        TerminateProcess(processes[i], 0);
        CloseHandle(processes[i]);
    }

    CloseHandle(pi_m.hThread); CloseHandle(pi_a.hThread);
    CloseHandle(pi_p.hThread); CloseHandle(pi_s.hThread);

    return 0;
}