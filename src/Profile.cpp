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
    profile.name = L"Player 1";

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

std::vector<PlayerProfile> ProfileStore::LoadProfiles() const {
    std::vector<PlayerProfile> profiles;

    std::istringstream input(ReadTextFile(GetProfilePath()));
    std::string line;

    while (std::getline(input, line)) {
        size_t tab = line.find('\t');

        if (tab == std::string::npos) {
            continue;
        }

        PlayerProfile profile;

        std::string name = line.substr(0, tab);

        int size = MultiByteToWideChar(CP_UTF8, 0, name.c_str(), static_cast<int>(name.size()), nullptr, 0);
        profile.name.resize(size);
        MultiByteToWideChar(CP_UTF8, 0, name.c_str(), static_cast<int>(name.size()), profile.name.data(), size);

        try {
            profile.bestScore = std::max(0, std::stoi(line.substr(tab + 1)));
        }
        catch (...) {
            profile.bestScore = 0;
        }

        profiles.push_back(profile);
    }

    if (profiles.empty()) {
        PlayerProfile defaultProfile;
        defaultProfile.name = L"Player 1";
        defaultProfile.bestScore = 0;
        profiles.push_back(defaultProfile);
    }

    return profiles;
}

bool ProfileStore::DeleteProfile(const std::wstring& name) const {
    std::vector<PlayerProfile> profiles = LoadProfiles();

    std::map<std::string, int> remainingProfiles;
    std::string nameToDelete = WideToUtf8(name);

    for (const PlayerProfile& profile : profiles) {
        std::string currentName = WideToUtf8(profile.name);

        if (currentName != nameToDelete) {
            remainingProfiles[currentName] = profile.bestScore;
        }
    }

    std::ostringstream output;

    for (const auto& item : remainingProfiles) {
        output << item.first << '\t' << item.second << '\n';
    }

    return WriteTextFile(GetProfilePath(), output.str());
}

bool ProfileStore::RenameProfile(const std::wstring& oldName, const std::wstring& newName) const {
    std::vector<PlayerProfile> profiles = LoadProfiles();

    std::map<std::string, int> renamedProfiles;

    std::string oldNameUtf8 = WideToUtf8(oldName);
    std::string newNameUtf8 = WideToUtf8(newName);

    for (const PlayerProfile& profile : profiles) {
        std::string currentName = WideToUtf8(profile.name);

        if (currentName == oldNameUtf8) {
            renamedProfiles[newNameUtf8] = profile.bestScore;
        }
        else {
            renamedProfiles[currentName] = profile.bestScore;
        }
    }

    std::ostringstream output;

    for (const auto& item : renamedProfiles) {
        output << item.first << '\t' << item.second << '\n';
    }

    return WriteTextFile(GetProfilePath(), output.str());
}