#include "UI/Windows/Settings/Categories/Gameplay.hpp"

#include "UI/Core/ImUtil.hpp"

#include "UI/Controls/CheckBox.hpp"
#include "UI/Controls/ComboBox.hpp"
#include "UI/Controls/ConditionalHeader.hpp"
#include "UI/Controls/Slider.hpp"

#include "Config/Config.hpp"

#include "UI/GTSMenu.hpp"
#include "UI/Controls/CollapsingTabHeader.hpp"
#include "UI/Controls/Text.hpp"

namespace GTS {

    void CategoryGameplay::GameModeOptions(GameplayActorSettings_t* a_Settings, bool a_isPlayer) {

        PSString T0 = "选择游戏模式。\n\n"
                      "基础模式：\n"
                      "- Growth：缓慢成长到你的体型上限。\n"
                      "- Shrink：缓慢缩回自然体型。\n"
                      "- Combat Growth：战斗中成长，脱战后缓慢缩回自然体型。\n"
                      "- Slow Combat Growth：战斗中缓慢成长，并保留已经获得的体型。\n\n"
                      "诅咒模式：\n"
                      "- Curse of Growth：像 \"Growth\" 一样持续成长，但会以不同强度的爆发方式成长，直到达到你在下方设定的上限。\n"
                      "- Curse of the Giantess：当你低于指定体型时，会迅速成长到该体型；像 \"Restore Size\" 这类法术不会把你缩到这个值以下。\n"
                      "- Curse of Diminishing：在非战斗状态或未执行巨化动作时，如果体型过大，会缓慢缩回目标值。\n"
                      "- Size Locked：结合两种诅咒效果。你会成长到指定体型，并在超过时缓慢缩回该值。";

        PSString T1 = "修改每次更新时的成长/缩小量。\n"
                      "成长速率 | 缩小速率";

        PSString T3 = "设置 \"Curse of Growth\" 模式的最大体型。";

        PSString T4 = "按当前体型的 25% 去乘算成长速度。\n"
                      "这部分加成最多计算到 10x。";

        PSString T5 = "设置以下模式使用的目标体型：\n\n"
                      "- Curse of the Giantess\n"
                      "- Curse of Diminishing\n"
                      "- Size Locked";

        PSString T6 = "调整诅咒效果的触发间隔。\n"
                      "实际每次会在你设定值基础上额外浮动 +/- 10%。";


        ImGuiEx::ComboEx<LActiveGamemode_t>("游戏模式", a_Settings->sGameMode, T0);

		auto currentMode = magic_enum::enum_cast<LActiveGamemode_t>(a_Settings->sGameMode);

        ImGui::BeginDisabled(a_Settings->sGameMode == "kNone");

        ImGui::Spacing();
        static std::array const pTemp = { &a_Settings->fGrowthRate, &a_Settings->fShrinkRate };
        

        if (currentMode.has_value()) {

            const bool CurseModes = currentMode.value() == LActiveGamemode_t::kCurseOfGrowth      ||
                                    currentMode.value() == LActiveGamemode_t::kCurseOfTheGiantess ||
                                    currentMode.value() == LActiveGamemode_t::kCurseOfDiminishing ||
                                    currentMode.value() == LActiveGamemode_t::kSizeLocked         ||
                                    currentMode.value() == LActiveGamemode_t::kLevelLocked;

            const bool UsesMultiplier = !CurseModes;
			const bool UsesRate = currentMode.value() != LActiveGamemode_t::kLevelLocked;

            ImGui::Spacing();

            if (UsesRate) {
                ImGuiEx::SliderF2("成长/缩小速率", pTemp.at(0), 0.001f, 0.2f, T1, "%.3fx");
            }

            if (UsesMultiplier) {
                ImGuiEx::CheckBox("按当前体型放大速率", &a_Settings->bMultiplyGrowthrate, T4);
            }

            if (CurseModes) {
                ImGuiEx::SliderF("更新间隔", &a_Settings->fGameModeUpdateInterval, 1.0f, 60.0f, T6, "每 %.2f 秒");
            }

            if (currentMode.value() == LActiveGamemode_t::kCurseOfTheGiantess ||
                currentMode.value() == LActiveGamemode_t::kCurseOfDiminishing ||
                currentMode.value() == LActiveGamemode_t::kSizeLocked){
                ImGuiEx::SliderF("Growth 诅咒上限", &a_Settings->fCurseGrowthSizeLimit, 1.1f, 50.0f, T3, "%.2fx");
                ImGuiEx::SliderF("目标体型", &a_Settings->fCurseTargetScale, 0.1f, 5.0f, T5, "%.2fx");
            }
            else if (currentMode.value() == LActiveGamemode_t::kLevelLocked) {

                if (a_isPlayer) {
                    Actor* Target = PlayerCharacter::GetSingleton();
                    float TargetScale = (a_Settings->bUseGTSSkill ? GetGtsSkillLevel(Target) : Target->GetLevel()) * a_Settings->fScalePerLevel;
                    ImGui::Text("%s 的最低体型会变成：%.2fx", Target->GetName(), TargetScale);
                }
                else {
                    for (const auto& teammate : GTSMenu::WindowManager->GetCachedTeamMateList()) {
                        float TargetScale = (a_Settings->bUseGTSSkill ? GetGtsSkillLevel(teammate) : teammate->GetLevel()) * a_Settings->fScalePerLevel;
                        ImGui::PushID(teammate);
                        ImGui::Text("%s 的最低体型会变成：%.2fx", teammate->GetName(), TargetScale);
                        ImGui::PopID();
                    }
                }

                ImGuiEx::SliderF("每级体型增量", &a_Settings->fScalePerLevel, 0.0001f, 0.2f, "按等级提高体型", "每级增加 %.4fx");
                ImGuiEx::CheckBox("使用 GTS 等级", &a_Settings->bUseGTSSkill, "使用 GTS 技能等级，而不是普通等级，来计算目标体型。");

            }
        }

        ImGui::EndDisabled();
        ImGui::Spacing();
    }

	CategoryGameplay::CategoryGameplay() {
	    m_name = "玩法";
    }

    void CategoryGameplay::DrawLeft() {


        //----- Perk Settings

        ImUtil_Unique 
    	{

            if (ImGui::CollapsingHeader("Perk 设置", ImUtil::HeaderFlagsDefaultOpen)) {

                const bool PerkCondCrush = Runtime::HasPerk(PlayerCharacter::GetSingleton(), Runtime::PERK.GTSPerkGrowthDesire);
                const bool PerkCondHit = Runtime::HasPerk(PlayerCharacter::GetSingleton(), Runtime::PERK.GTSPerkHitGrowth);
                const bool PerkCondAtribCap = Runtime::HasPerk(PlayerCharacter::GetSingleton(), Runtime::PERK.GTSPerkGrowthDesire);

                PSString T1 = "击败或碾碎目标后会成长。\n"
                              "同时作用于玩家和追随者。";

                PSString T2 = "受到攻击时会成长。\n"
                              "同时作用于玩家和追随者。";

                PSString T3 = "默认情况下，每级最多只能获得 2 点额外属性。\n"
                              "你可以在这里调整这个倍率。";

                const char* T4 = "控制受击成长 Perk 提供的成长倍率。";

                const std::string tGrowOnCrush = fmt::format("击杀成长 {}", (!PerkCondCrush ? "[缺少 Perk]" : ""));
                ImGuiEx::CheckBox(tGrowOnCrush.c_str(), &Config::Gameplay.bEnableCrushGrowth, T1, !PerkCondCrush);

                ImGui::SameLine();

                const std::string tGrowOnHit = fmt::format("受击成长 {}", (!PerkCondHit ? "[缺少 Perk]" : ""));
                ImGuiEx::CheckBox(tGrowOnHit.c_str(), &Config::Gameplay.bEnableGrowthOnHit, T2, !PerkCondHit);

                const char* tGrowOnHitPower = (!PerkCondHit ? "[缺少 Perk]" : "%.2fx");
                ImGuiEx::SliderF("受击成长强度", &Config::Gameplay.fHitGrowthPower, 0.01f, 3.0f, T4, tGrowOnHitPower, !PerkCondHit);

                const char* tSizeConfFMT = (!PerkCondAtribCap ? "[缺少 Perk]" : "%.2fx");
                ImGuiEx::SliderF("完全同化属性上限", &Config::Gameplay.fFullAssimilationLevelCap, 0.1f, 5.0f, T3, tSizeConfFMT, !PerkCondAtribCap);

                ImGui::Spacing();
            }
        }

    	//----- Armor Stripping

        ImUtil_Unique 
    	{

            if (ImGui::CollapsingHeader("装备破损/脱落", ImUtil::HeaderFlagsDefaultOpen)) {

                PSString T1 = "体型足够大时，自动卸下衣物/护甲。\n"
                              "同时作用于玩家和追随者。\n"
                              "这个系统按体型阈值运作。\n"
                              "每脱落一件装备，下一件需要的体型阈值都会提高。";

                PSString T2 = "设置开始自动脱落衣物/护甲的体型阈值。";
                PSString T3 = "设置全部衣物/护甲都会脱落的体型阈值。";

                ImGuiEx::CheckBox("启用装备脱落", &Config::Gameplay.bClothTearing, T1);

                ImGui::BeginDisabled(!Config::Gameplay.bClothTearing);
                ImGuiEx::SliderF("开始脱落阈值", &Config::Gameplay.fClothRipStart, 1.1f, 2.5f, T2, "%.2fx");
                ImGuiEx::SliderF("全部脱落阈值", &Config::Gameplay.fClothRipThreshold, Config::Gameplay.fClothRipStart + 0.1f, 3.5f, T3, "%.2fx",false, true);
                ImGui::EndDisabled();

                ImGui::Spacing();
            }
        }

    	//----- Size Effects

        ImUtil_Unique 
    	{

            if (ImGui::CollapsingHeader("体型效果", ImUtil::HeaderFlagsDefaultOpen)) {

                PSString T1 = "体型足够大时，脚步或尺寸相关动作会掀飞带物理效果的物体。";

                PSString T2 = "把物理冲击也作用到当前区域外的物体上。\n"
                              "注意：这个选项计算量很大，所以才单独做成开关。\n"
                              "如果你遇到掉帧，建议关闭它。";

                PSString T3 = "启用尘土、镜头震动和手柄震动等效果。";

                ImGuiEx::CheckBox("掀飞物体", &Config::Gameplay.bLaunchObjects, T1);
                ImGui::SameLine();
                ImGuiEx::CheckBox("作用于所有区域", &Config::Gameplay.bLaunchAllCells, T2, !Config::Gameplay.bLaunchObjects);
                ImGuiEx::CheckBox("玩家尘土/震动效果", &Config::Gameplay.bPlayerAnimEffects, T3);
                ImGuiEx::CheckBox("追随者尘土/震动效果", &Config::Gameplay.bNPCAnimEffects, T3);

                ImGui::Spacing();
            }
        }

        //----- Random Growth

        ImUtil_Unique 
    	{

            const bool HasPerk = Runtime::HasPerk(PlayerCharacter::GetSingleton(), Runtime::PERK.GTSPerkRandomGrowth);
            const bool BalancedMode = Config::Balance.bBalanceMode;
        	const char* Reason = "Requires \"Pleasurable Growth\" Perk";

            if (ImGuiEx::ConditionalHeader("Random Growth", Reason, HasPerk)) {

                PSString T1 = "Change how often the random growth should trigger.\n"
                              "Lower values = More often.\n"
                              "Higher values = Less often.\n"
                              "Set to 0.0 to disable it entirely.\n"
							  "Note: If Balance Mode is enabled the multiplier is locked to 1.0x. It can still be disabled however by setting it to 0.0";

                PSString THelp = "Random Growth chance is decreased if your size is > than x1.5, up to 4 times decreased chance\n"
                                 "If you have 'Breaching Growth' perk, random growth has a chance to start 'Large Growth' animation";

                const char* FmtBalance = BalancedMode ? "Balance Mode (1.0x)" : "%.2fx";

                const char* Fmt1 = Config::Gameplay.GamemodePlayer.fRandomGrowthDelay != 0.0f ? FmtBalance : "Disabled";
                const char* Fmt2 = Config::Gameplay.GamemodeFollower.fRandomGrowthDelay != 0.0f ? FmtBalance : "Disabled";

                ImGuiEx::HelpText("About random growth", THelp);

                ImGuiEx::SliderF("Growth Delay Player", &Config::Gameplay.GamemodePlayer.fRandomGrowthDelay, 0.00f, 4.0f, T1, Fmt1);
                ImGuiEx::SliderF("Growth Delay Followers", &Config::Gameplay.GamemodeFollower.fRandomGrowthDelay, 0.00f, 4.0f, T1, Fmt2);

                ImGui::Spacing();
            }
        }
    }

    void CategoryGameplay::DrawRight() {

        static ImGuiEx::CollapsingTabHeader GameModeSettingsHeader(
            "Alternative Gamemodes",
            {
                "Player",
                "Followers"
            }
        );

        bool shouldDisable = false;
        std::string Reason = "";
        if (Config::Balance.bBalanceMode) {
            shouldDisable = true;
            Reason = "Balance Mode Active";
        }
        else if (!Runtime::HasPerk(PlayerCharacter::GetSingleton(), Runtime::PERK.GTSPerkColossalGrowth)) {
            shouldDisable = true;
            Reason = "Missing Perk: \"Colossal Growth\"";
        }

        {
            GameModeSettingsHeader.SetExtraInfo(Reason);
            GameModeSettingsHeader.SetDisabledState(shouldDisable);

            if (ImGuiEx::BeginCollapsingTabHeader(GameModeSettingsHeader)) {
                // Content based on active tab
                switch (GameModeSettingsHeader.GetActiveTab()) {
                    case 0: GameModeOptions(&Config::Gameplay.GamemodePlayer, true);    break;
                    case 1: GameModeOptions(&Config::Gameplay.GamemodeFollower, false); break;
                    default:                                                            break;
                }
            }

            ImGuiEx::EndCollapsingTabHeader(GameModeSettingsHeader);
        }

    }
}
