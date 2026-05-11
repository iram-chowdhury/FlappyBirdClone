#pragma once

#include <string>

struct PlayerProfile {
    std::wstring name = L"Player";
    int bestScore = 0;
};

class ProfileStore {
public:
    PlayerProfile LoadActiveProfile() const;
    bool SaveProfile(const PlayerProfile& profile) const;

private:
    std::wstring GetProfilePath() const;
};
