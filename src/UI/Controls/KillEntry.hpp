#pragma once
#include "UI/Lib/imgui.h"

namespace ImGuiEx {

    struct KillEntry {
        std::string attacker;
        std::string victim;
        uintptr_t victimptr;
        std::string type;
        float lifetime = 0.0f;
    };

    enum KillFeedEntryFlags : uint16_t {
        KillFeedEntryFlag_None = 0,
        KillFeedEntryFlag_NoKiller = 1 << 0,
        KillFeedEntryFlag_NoKillType = 1 << 1,
    };


    void DrawKillfeedEntry(std::unique_ptr<KillEntry>& entry, const float a_visDuration, bool a_neverFade, ImU32 a_bgColor, float a_bgAlpha, uint16_t a_flags);

}
