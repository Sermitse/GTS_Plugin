#pragma once
namespace ImGuiEx {

    struct KillEntry {
        std::string attacker;
        std::string victim;
        std::string weapon_icon;
        float lifetime = 0.0f;
        float max_lifetime = 6.0f;
    };

    void DrawKillfeedEntry(const KillEntry& entry);

}