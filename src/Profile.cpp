#include "Profile.h"

#include "WindowsConfig.h"

#include <windows.h>

#include <algorithm>
#include <map>
#include <sstream>
#include <string>

static std::wstring GetExecutableDirectory() {
    wchar_t path[MAX_PATH] = {};
    DWORD length = GetModuleFileNameW(nullptr, path, MAX_PATH);
    std::wstring fullPath(path, length);
    size_t slash = fullPath.find_last_of(L"\\/");
    if (slash == std::wstring::npos) {
        return L".";
    }
    return fullPath.substr(0, slash);
}

static std::wstring GetEnvironmentText(const wchar_t* name, const wchar_t* fallback) {
    wchar_t buffer[256] = {};
    constexpr DWORD bufferCount = static_cast<DWORD>(sizeof(buffer) / sizeof(buffer[0]));
    DWORD length = GetEnvironmentVariableW(name, buffer, bufferCount);
    if (length == 0 || length >= bufferCount) {
        return fallback;
    }
    return std::wstring(buffer, length);
}

static std::string WideToUtf8(const std::wstring& text) {
    if (text.empty()) {
        return {};
    }

    int size = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), nullptr, 0, nullptr, nullptr);
    std::string result(size, '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), result.data(), size, nullptr, nullptr);
    return result;
}

static std::string ReadTextFile(const std::wstring& path) {
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return {};
    }

    DWORD fileSize = GetFileSize(file, nullptr);
    std::string text(fileSize, '\0');
    DWORD bytesRead = 0;
    if (fileSize > 0) {
        ReadFile(file, text.data(), fileSize, &bytesRead, nullptr);
        text.resize(bytesRead);
    }
    CloseHandle(file);
    return text;
}

static bool WriteTextFile(const std::wstring& path, const std::string& text) {
    HANDLE file = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD bytesWritten = 0;
    BOOL ok = WriteFile(file, text.data(), static_cast<DWORD>(text.size()), &bytesWritten, nullptr);
    CloseHandle(file);
    return ok && bytesWritten == text.size();
}

std::wstring ProfileStore::GetProfilePath() const {
    std::wstring dataDirectory = GetExecutableDirectory() + L"\\data";
    CreateDirectoryW(dataDirectory.c_str(), nullptr);
    return dataDirectory + L"\\profiles.txt";
}

PlayerProfile ProfileStore::LoadActiveProfile() const {
    PlayerProfile profile;
    profile.name = GetEnvironmentText(L"USERNAME", L"Player");

    std::string activeName = WideToUtf8(profile.name);
    std::istringstream input(ReadTextFile(GetProfilePath()));
    std::string line;
    while (std::getline(input, line)) {
        size_t tab = line.find('\t');
        if (tab == std::string::npos) {
            continue;
        }

        if (line.substr(0, tab) == activeName) {
            try {
                profile.bestScore = std::max(0, std::stoi(line.substr(tab + 1)));
            } catch (...) {
                profile.bestScore = 0;
            }
            break;
        }
    }

    return profile;
}

bool ProfileStore::SaveProfile(const PlayerProfile& profile) const {
    std::map<std::string, int> profiles;

    std::istringstream input(ReadTextFile(GetProfilePath()));
    std::string line;
    while (std::getline(input, line)) {
        size_t tab = line.find('\t');
        if (tab == std::string::npos) {
            continue;
        }

        try {
            profiles[line.substr(0, tab)] = std::max(0, std::stoi(line.substr(tab + 1)));
        } catch (...) {
        }
    }

    profiles[WideToUtf8(profile.name)] = std::max(0, profile.bestScore);

    std::ostringstream output;
    for (const auto& item : profiles) {
        output << item.first << '\t' << item.second << '\n';
    }

    return WriteTextFile(GetProfilePath(), output.str());
}
