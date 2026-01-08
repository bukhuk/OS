#include <iostream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    std::cout << "Enter numbers: ";
    std::string input;
    if (!std::getline(std::cin, input) || input.empty()) {
        return 1;
    }

    int m_a[2], a_p[2], p_s[2], s_main[2];
    if (pipe(m_a) == -1 || pipe(a_p) == -1 || pipe(p_s) == -1 || pipe(s_main) == -1) {
        return 1;
    }

    // Процесс S
    if (fork() == 0) {
        close(m_a[0]); close(m_a[1]);
        close(a_p[0]); close(a_p[1]);
        close(p_s[1]); close(s_main[0]);

        dup2(p_s[0], STDIN_FILENO);
        dup2(s_main[1], STDOUT_FILENO);

        close(p_s[0]);
        close(s_main[1]);

        execl("./S", "S", nullptr);
        _exit(1);
    }

    // Процесс P
    if (fork() == 0) {
        close(m_a[0]); close(m_a[1]);
        close(a_p[1]); close(p_s[0]);
        close(s_main[0]); close(s_main[1]);

        dup2(a_p[0], STDIN_FILENO);
        dup2(p_s[1], STDOUT_FILENO);

        close(a_p[0]);
        close(p_s[1]);

        execl("./P", "P", nullptr);
        _exit(1);
    }

    // Процесс A
    if (fork() == 0) {
        close(m_a[1]); close(a_p[0]);
        close(p_s[0]); close(p_s[1]);
        close(s_main[0]); close(s_main[1]);

        dup2(m_a[0], STDIN_FILENO);
        dup2(a_p[1], STDOUT_FILENO);

        close(m_a[0]);
        close(a_p[1]);

        execl("./A", "A", nullptr);
        _exit(1);
    }

    // Процесс M
    if (fork() == 0) {
        int input_pipe[2];
        if (pipe(input_pipe) == -1) _exit(1);

        if (fork() == 0) {
            close(input_pipe[0]);
            std::string data = input + "\n";
            write(input_pipe[1], data.c_str(), data.length());
            close(input_pipe[1]);
            _exit(0);
        }

        close(input_pipe[1]);
        close(m_a[0]); close(a_p[0]); close(a_p[1]);
        close(p_s[0]); close(p_s[1]);
        close(s_main[0]); close(s_main[1]);

        dup2(input_pipe[0], STDIN_FILENO);
        dup2(m_a[1], STDOUT_FILENO);

        close(input_pipe[0]);
        close(m_a[1]);

        execl("./M", "M", nullptr);
        _exit(1);
    }

    // Родитель (Main)
    close(m_a[0]); close(m_a[1]);
    close(a_p[0]); close(a_p[1]);
    close(p_s[0]); close(p_s[1]);
    close(s_main[1]);

    for (int i = 0; i < 4; i++) {
        wait(nullptr);
    }

    char result[100];
    ssize_t bytes = read(s_main[0], result, sizeof(result) - 1);

    if (bytes > 0) {
        result[bytes] = '\0';
        if (result[bytes - 1] == '\n') {
            result[bytes - 1] = '\0';
        }
        std::cout << "Result: " << result << "\n";
    } else {
        std::cout << "No result\n";
    }

    close(s_main[0]);
    return 0;
}