#pragma once

#include "UI/Controls/KillEntry.hpp"
#include "UI/Core/ImWindow.hpp"

#include "Utils/KillDataUtils.hpp"

namespace GTS {

    class KillFeedWindow final : public ImConfigurableWindow<KillFeedWindow>, public EventListener {

    	public:
        static void AddKillEntry(Actor* a_attacker, Actor* a_victim, DeathType a_type);
        std::string DebugName() override;
        void DeathEvent(Actor* a_killer, Actor* a_victim, bool a_dead) override;

		private:
        void Draw() override;
        void Init() override;
        void RequestClose() override;
        bool WantsToDraw() override;
        virtual float GetBackgroundAlpha() override;

        WindowSettingsKillFeed_t m_extraSettings;

        const bool* m_isConfiguring = nullptr;
        const bool* m_settingsVisible = nullptr;

        inline static std::mutex _Lock;
        inline static std::vector<std::unique_ptr<ImGuiEx::KillEntry>> KillEntries;
        inline static std::deque<std::unique_ptr<ImGuiEx::KillEntry>> PendingKillEntries;
    };
}
