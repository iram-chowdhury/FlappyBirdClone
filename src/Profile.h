#pragma once

#include <string>
#include <vector>

struct PlayerProfile {
    std::wstring name = L"Player";
    int bestScore = 0;
};

class ProfileStore {
public:
    PlayerProfile LoadActiveProfile() const;
    bool SaveProfile(const PlayerProfile& profile) const;

    std::vector<PlayerProfile> LoadProfiles() const;
    bool DeleteProfile(const std::wstring& name) const;
    bool RenameProfile(const std::wstring& oldName, const std::wstring& newName) const;

private:
    std::wstring GetProfilePath() const;
};
