#pragma once
namespace ImGuiEx {

    struct KillEntry {
        std::string attacker;
        std::string victim;
        uintptr_t victimptr;
        std::string type;
        float lifetime = 0.0f;
    };

    void DrawKillfeedEntry(std::unique_ptr<KillEntry>& entry, float a_visDuration, bool a_neverFade);

}