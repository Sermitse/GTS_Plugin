#pragma once
#include "UI/Lib/imgui.h"

namespace ImGuiEx {

    enum KillFeedEntryFlags : uint16_t {
        KillFeedEntryFlag_None = 0,
        KillFeedEntryFlag_NoKiller = 1 << 0,
        KillFeedEntryFlag_NoKillType = 1 << 1,
    };

    struct KillEntry {
        std::string attacker;
        std::string victim;
        uintptr_t victimptr;
        std::string type;
        float lifetime = 0.0f;
    };

    struct KillFeedStyle {
        float visDuration        = {};
        bool neverFade           = {};

        ImU32 bgColor            = {};
        float bgAlpha = {};

        ImU32 attackerCol        = {};
        ImU32 victimCol          = {};
        ImU32 deathTypeCol       = {};

        float fontScale          = {};
    	KillFeedEntryFlags flags = {};

    };

    void DrawKillfeedEntry(std::unique_ptr<KillEntry>& entry, const KillFeedStyle& a_style);

}
