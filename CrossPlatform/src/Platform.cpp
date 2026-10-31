#include "Platform.h"
#include <iostream>
#include <vector>

#ifdef _WIN32
#include <shlobj.h>
#include <windows.h>
#else
#include <cstdlib>
#include <fcntl.h>
#include <pwd.h>
#include <sys/file.h>
#include <unistd.h>
#endif

#ifdef _WIN32

std::string ToUtf8(const std::wstring& wstr) {
    if (wstr.empty()) return {};
    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    std::string out(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), out.data(), size, nullptr, nullptr);
    return out;
}

std::filesystem::path GetDownloadPath() {
    PWSTR raw_path = nullptr;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_Downloads, KF_FLAG_DEFAULT_PATH, nullptr, &raw_path);
    if (SUCCEEDED(hr) && raw_path) {
        std::filesystem::path result(raw_path);
        CoTaskMemFree(raw_path);
        return result;
    }
    return {};
}

bool IsFileLocked(const std::filesystem::path& file) {
    HANDLE h = CreateFileW(file.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return true;
    CloseHandle(h);
    return false;
}

#else

std::string ToUtf8(const std::wstring& wstr) {
    return std::string(wstr.begin(), wstr.end());
}

std::filesystem::path GetDownloadPath() {
    const char* xdg = std::getenv("XDG_DOWNLOAD_DIR");
    if (xdg && xdg[0] != '\0') {
        std::string path(xdg);
        const char* home_env = std::getenv("HOME");
        if (home_env && path.rfind("$HOME", 0) == 0) {
            path.replace(0, 5, home_env);
        }
        return std::filesystem::path(path);
    }

    const char* home = std::getenv("HOME");
    if (!home) {
        struct passwd* pw = getpwuid(getuid());
        if (pw) home = pw->pw_dir;
    }

    if (home) {
        return std::filesystem::path(home) / "Downloads";
    }

    return {};
}

bool IsFileLocked(const std::filesystem::path& file) {
    int fd = open(file.c_str(), O_RDONLY);
    if (fd == -1) return true;

    if (flock(fd, LOCK_EX | LOCK_NB) == -1) {
        close(fd);
        return true;
    }

    flock(fd, LOCK_UN);
    close(fd);
    return false;
}

#endif

#ifdef _WIN32
#include <conio.h>
char GetChar() { return _getch(); }
#else
#include <termios.h>
#include <unistd.h>
char GetChar() {
    char buf = 0;
    struct termios old = { 0 };
    fflush(stdout);
    if (tcgetattr(0, &old) < 0) perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0) perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0) perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0) perror("tcsetattr ~ICANON");
    return buf;
}
#endif