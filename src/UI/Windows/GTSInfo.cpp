#include "UI/Windows/GTSInfo.hpp"
#include "UI/ImGui/ImUtil.hpp"
#include "UI/ImGui/Lib/imgui.h"
#include "Managers/Attributes.hpp"
#include "Managers/SpectatorManager.hpp"

#include "Managers/MaxSizeManager.hpp"

#include "Utils/UnitConverter.hpp"
#include "Utils/KillDataUtils.hpp"

namespace {
    bool CheckOK(RE::Actor* a_Actor) {
        if (!a_Actor) return false;
        if (!a_Actor->Get3D()) return false;
        if (!a_Actor->Is3DLoaded()) return false;
        return true;
    }
}

namespace GTS {
    void DrawSpectateAndWidget(GTSInfoFeatures a_featureFlags, RE::Actor* a_Actor, const bool a_IsWidget) {
        if (!CheckOK(a_Actor)) {
            return;
        }

        const auto& Settings = Config::GetUI().StatusWindow;

        const ImVec2 ProgressBarSize = { hasFlag(a_featureFlags, GTSInfoFeatures::kAutoSize) ? 0.0f : Settings.fFixedWidth , 0.0f };
        const float ProgressBarHeight = hasFlag(a_featureFlags, GTSInfoFeatures::kAutoSize) ? 1.1f : Settings.fSizeBarHeightMult;

        const auto& ActorTransient = Transient::GetSingleton().GetData(a_Actor);
        const auto& ActorPersistent = Persistent::GetSingleton().GetData(a_Actor);
        if (!ActorTransient || !ActorPersistent) {
            ImUtil::TextShadow("Actor Invalid!");
            return;
        }

        if (!a_IsWidget) {

            if (a_Actor->formID != 0x14) {
                float verticalOffset = (ImGui::GetFrameHeight() * ProgressBarHeight - ImGui::GetFrameHeight()) * 0.5f;
                ImGui::SameLine(0.0f, 8.0f);
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + verticalOffset);
                const bool IsSpectating = !SpectatorManager::IsCameraTargetPlayer();
                const char* Msg = IsSpectating ? "Cancel" : "Spectate";
                if (ImUtil::Button(Msg)) {
                    if (IsSpectating) {
                        SpectatorManager::GetSingleton().Reset();
                    }
                    else {
                        SpectatorManager::SetCameraTarget(a_Actor, false);
                    }
                }
            }

            if (a_Actor->formID != 0x14) {
                float verticalOffset = (ImGui::GetFrameHeight() * ProgressBarHeight - ImGui::GetFrameHeight()) * 0.5f;
                ImGui::SameLine(0.0f, 8.0f);
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + verticalOffset);
                const char* const TFolTT = "Show this follower's current size as an extra bar in the player widget.";
                const void* ActPtrUID = (reinterpret_cast<void*>(a_Actor));
                ImGui::PushID(ActPtrUID);
                ImUtil::CheckBox("Widget", &ActorPersistent->ShowSizebarInUI, TFolTT);
                ImGui::PopID();
            }
        }
    }
    void DrawGTSSizeBar(GTSInfoFeatures a_featureFlags, RE::Actor* a_Actor, const bool a_IsWidget) {

        if (!a_IsWidget) {
            a_featureFlags = static_cast<GTSInfoFeatures>(UINT32_MAX);
        }

        if (!CheckOK(a_Actor)) {
            return;
        }

        const auto& Settings = Config::GetUI().StatusWindow;

        const auto& ActorTransient = Transient::GetSingleton().GetData(a_Actor);
        const auto& ActorPersistent = Persistent::GetSingleton().GetData(a_Actor);
        if (!ActorTransient || !ActorPersistent) {
            ImUtil::TextShadow("Actor Invalid!");
            return;
        }

        const float CurrentScale = get_visual_scale(a_Actor);
        const float MaxScale = get_max_scale(a_Actor);
        const float VisualProgress = MaxScale < 250.0f ? CurrentScale / MaxScale : 0.0f;

        //--------- Formatted display strings
        const std::string StringScale = hasFlag(a_featureFlags, GTSInfoFeatures::kUnitScale) ? fmt::format("({:.2f}x)", CurrentScale) : "";
        const std::string StringReal = hasFlag(a_featureFlags, GTSInfoFeatures::kUnitReal) ? GTS::GetFormatedHeight(a_Actor).c_str() : "";
        const std::string ResultingText = fmt::format("{} {}", StringReal, StringScale);

        const ImVec2 ProgressBarSize = { hasFlag(a_featureFlags, GTSInfoFeatures::kAutoSize) ? 0.0f : Settings.fFixedWidth , 0.0f };
        const float ProgressBarHeight = hasFlag(a_featureFlags, GTSInfoFeatures::kAutoSize) ? 1.1f : Settings.fSizeBarHeightMult;

        ImVec4 ColorAVec{ Settings.f3ColorA[0], Settings.f3ColorA[1], Settings.f3ColorA[2] ,1.0f };
        ImVec4 ColorBVec{ Settings.f3ColorB[0], Settings.f3ColorB[1], Settings.f3ColorB[2] ,1.0f };
        const ImU32 ColorA32 = ImGui::ColorConvertFloat4ToU32(ColorAVec);
        const ImU32 ColorB32 = ImGui::ColorConvertFloat4ToU32(ColorBVec);
        const float BorderCol = Settings.fBorderLightness;
        const float BorderAlpha = Settings.fBorderAlpha;

    	ImGui::PushStyleColor(ImGuiCol_Border, { BorderCol, BorderCol, BorderCol, BorderAlpha });

        // Visual Scale (Progress) Bar Player
        ImUtil::CenteredProgress(
            VisualProgress,
            ProgressBarSize,
            ResultingText.c_str(),
            ProgressBarHeight,
            Settings.fBorderThickness,
            Settings.bUseGradient,
            Settings.fNormalGradientDarkMult,
            Settings.fNormalGradientLightMult,
            Settings.bEnableRounding,
            Settings.bUseCustomGradientColors,
            ColorA32,
            ColorB32,
            Settings.bFlipGradientDirection
        );

        DrawSpectateAndWidget(a_featureFlags, a_Actor, a_IsWidget);
        ImGui::PopStyleColor();
    }

    void DrawGTSInfo(GTSInfoFeatures a_featureFlags, RE::Actor* a_Actor, const bool a_IsWidget) {

        if (!a_IsWidget) {
            a_featureFlags = static_cast<GTSInfoFeatures>(UINT32_MAX);
        }

       GTS_PROFILE_SCOPE("UI: DrawGTSInfo");

        if (!CheckOK(a_Actor)) {
            ImUtil::TextShadow("Actor Invalid!");
            return;
        }

        const auto& ActorTransient = Transient::GetSingleton().GetData(a_Actor);
        const auto& ActorPersistent = Persistent::GetSingleton().GetData(a_Actor);
        if (!ActorTransient || !ActorPersistent) {
            ImUtil::TextShadow("Actor Invalid!");
            return;
        }

        float CarryWeight;
        // When in god mode carry weight gets 100x'ed for some reason
        if (a_Actor->formID == 0x14 && IsInGodMode(a_Actor)) {
            CarryWeight = ActorTransient->CarryWeightBoost / 100u;
        }
        else [[likely]] {
            CarryWeight = ActorTransient->CarryWeightBoost;
            // Note: AlterGetAv(kCarryWeight) doesn't seem to be called each frame on NPC's
            // So sometimes it might take a while to update Carry Weight boost on NPC
            // Weight update seems to happen when NPC picks up items/player gives items to them
        }

    	//--------- Transient Data
        const float BonusSize = ActorTransient->PotionMaxSize;
        const float StolenHealth = GetStolenAttributes_Values(a_Actor, ActorValue::kHealth);
        const float StolenMagicka = GetStolenAttributes_Values(a_Actor, ActorValue::kMagicka);
        const float StolenStamina = GetStolenAttributes_Values(a_Actor, ActorValue::kStamina);
        const float StolenCap = GetStolenAttributeCap(a_Actor);

        const float StolenAttributes = ActorPersistent->stolen_attributes;

        //---------- Persistent Data
        const float SizeReserve = ActorPersistent->SizeReserve;
    	const float SizeEssense = Persistent::GetSingleton().PlayerExtraPotionSize.value;

        //---------- Other
        const bool MassMode = Config::GetBalance().sSizeMode == "kMassBased";
        const bool IsPlayerMassMode = a_Actor->formID == 0x14 && MassMode;
        const float shrinkResist_PreCalc = 1.0f * Potion_GetShrinkResistance(a_Actor) * Perk_GetSprintShrinkReduction(a_Actor); // to make ShrinkResistance below shorter
        const float NaturalScale = get_natural_scale(a_Actor, true);

        const float MaxScale = get_max_scale(a_Actor);
        const float AspectOfGTS = Ench_Aspect_GetPower(a_Actor) * 100.0f;
        const float DamageResist = (1.0f - AttributeManager::GetAttributeBonus(a_Actor, ActorValue::kHealth)) * 100.f;
        const float Speed = (AttributeManager::GetAttributeBonus(a_Actor, ActorValue::kSpeedMult) - 1.0f) * 100.f;
        const float JumpHeight = (AttributeManager::GetAttributeBonus(a_Actor, ActorValue::kJumpingBonus) - 1.0f) * 100.0f;
        const float Damage = (AttributeManager::GetAttributeBonus(a_Actor, ActorValue::kAttackDamageMult) - 1.0f) * 100.0f;
        const float ShrinkResistance = (1.0f - shrinkResist_PreCalc) * 100.f;
        const float OnTheEdge = (GetPerkBonus_OnTheEdge(a_Actor, 0.01f) - 1.0f) * 100.f;
        const float BonusHHDamage = (GetHighHeelsBonusDamage(a_Actor, true) - 1.0f) * 100.0f;


        //---------Total Max Size Calculation and Text Formating
        const float BonusSize_EssenceAndDragons = SizeEssense;
        const float BonusSize_TempPotionBoost = BonusSize * 100.0f;
        const float BonusSize_AspectOfGiantess = AspectOfGTS;
        const float BonusSize_Overkills = ActorTransient->Overkills;
        const float BonusSize_Overkills_Multiplier = ActorTransient->OverkillSizeBonus;

        std::string OverkillsMade = fmt::format(
            fmt::runtime(
                "- Dead victims do not count\n"
                "Erased From Existence: {:d}\n"
                "Shrunk To Nothing: {:d}\n"
                "Breast Suffocated: {:d}\n"
                "Breast Absorbed: {:d}\n"
                "Breast Crushed: {:d}\n"
                "---\n"
                "Thigh Suffocated: {:d}\n"
                "Thigh Sandwiched: {:d}\n"
                "Thigh Crushed: {:d}\n"
                "---\n"
                "Grind Crushed: {:d}\n"
                "Kick Crushed: {:d}\n"
                "---\n"
                "Finger Crushed: {:d}\n"
                "Grab Crushed: {:d}\n"
                "Butt Crushed: {:d}\n"
                "Hug Crushed: {:d}\n"
                "Crushed: {:d}\n"
                "---\n"
                "Eaten: {:d}\n\n"
                "Other Sources: {:d}\n"
                "- Other Sources are: Shockwaves, Tiny Calamity Collision\n"
                "- Using Tiny as Shield when grabbing tiny\n"
                "- Overkill Weapon Damage when large\n"
            ),
            GetKillCount(a_Actor, SizeKillType::kErasedFromExistence),
            GetKillCount(a_Actor, SizeKillType::kShrunkToNothing),
            GetKillCount(a_Actor, SizeKillType::kBreastSuffocated),
            GetKillCount(a_Actor, SizeKillType::kBreastAbsorbed),
            GetKillCount(a_Actor, SizeKillType::kBreastCrushed),
            GetKillCount(a_Actor, SizeKillType::kThighSuffocated),
            GetKillCount(a_Actor, SizeKillType::kThighSandwiched),
            GetKillCount(a_Actor, SizeKillType::kThighCrushed),
            GetKillCount(a_Actor, SizeKillType::kGrinded),
            GetKillCount(a_Actor, SizeKillType::kKicked),
            GetKillCount(a_Actor, SizeKillType::kFingerCrushed),
            GetKillCount(a_Actor, SizeKillType::kGrabCrushed),
            GetKillCount(a_Actor, SizeKillType::kButtCrushed),
            GetKillCount(a_Actor, SizeKillType::kHugCrushed),
            GetKillCount(a_Actor, SizeKillType::kCrushed),
            GetKillCount(a_Actor, SizeKillType::kEaten),
            GetKillCount(a_Actor, SizeKillType::kOtherSources)
        );

        std::string TotalSizeBonusCalculation = fmt::format(
            fmt::runtime("Size Essence + Absorbed Dragons: +{:.2f}\n"
            "Colossal Growth: +{:.2f}\n"
            "Potion Of Heights: +{:.0f}%%\n"
            "Aspect Of Giantess: +{:.0f}%%\n"
            "Overkills&C.Growth: x{:.2f}\n\n"
            "- Size Essence Increases your maximum achievable size when the size limit cap is set to \"Skill Based\"\n"
            "- If Size Gain mode is in \"Mass Mode\", then Essence Bonus is reduced by {:.0f}%% \n" 
            "- You can gain Essence by killing and absorbing dragons when you have the correct perk\n"
            "- Or by consuming specific potions found all around the world."
            ),
            MassMode ? BonusSize_EssenceAndDragons * MassMode_ElixirPowerMultiplier : BonusSize_EssenceAndDragons * 1.0f,
            BonusSize_Overkills,
            BonusSize_TempPotionBoost,
            BonusSize_AspectOfGiantess,
            BonusSize_Overkills_Multiplier,
            (1.0f - MassMode_ElixirPowerMultiplier) * 100.0f
        );

        //------- Tooltip Descriptions
        
        const char* TOverkillsMade = OverkillsMade.c_str();

        const char* TBonusSize = TotalSizeBonusCalculation.c_str();

        const char* TDamageResist = "This is your damage resistance in percentage. Some GTS perks may further increase it.";
        const char* TDamageBonus =  "This is your non-size related damage multiplier. It affects both Physical and Magic damage.";

        const char* THHDamage = "Extra foot damage multiplier when wearing high heels.";

        const char* TShrinkResist = "Shrink Resistance reduces the effectiveness of any shrinking spell and/or effect on you.";

        const char* TOnTheEdge = "When your health drops below 60%%:\n"
                                 "- All growth gained becomes stronger the less health you have.\n"
                                 "- Hostile shrinking is less effective the less health you have.\n\n"
    							 "Maximum effect is achieved at 10%% HP remaining or lower.";

        const char* TSizeReserve = "This is the amount of size stored by the size reserve perk.\n"
								   "You gain size reserve by eating/absorbing/crushing others";

        const char* TAspectOfGTS = "This is the strength of Aspect of the Giantess enchantment\n"
                                    "Aspect Of Giantess affects:\n"
                                    "- Maximal Size, power of Shrink and Size Steal spells\n"
                                    "- Size-Related damage, minimal shrink threshold from quest/balance mode\n"
                                    "- Growth Rate and Growth Chance from Random Growth\n"
                                    "- Power of Shrink Outburst and Hit Growth size gain\n"
                                    "- Shrink resistance towards hostile shrink sources\n\n"
                                    "Enchantment can be obtained from 'Amulet of Giants', which can be randomly found inside boss chests.";
        const char* TStoredAttributes = 
                                    "Stored Attributes are permanent Health/Magicka/Stamina attribute boosts that weren't absorbed by you yet\n"
                                    "They'll be randomly distributed between your three main attributes\n"
                                    "Complete perk requirements to fully absorb them and convert to Absorbed Attributes";
        const char* TAbsorbedAttributes = 
                                    "Absorbed Attributes are permanent Health/Magicka/Stamina attribute boosts of your character\n"
                                    "They're coming from 'Full Assimilation' perk";
        const char* TAbsorbedAttributesCap = 
                                    "Absorbed Attributes cannot exceed this number";

        DrawGTSSizeBar(a_featureFlags, a_Actor, a_IsWidget);

        // Set up the table with 2 columns: Stat name and value
        if (ImGui::BeginTable("GTSInfoTable", 2, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_Hideable)) {

            // Max Size
            if (hasFlag(a_featureFlags, GTSInfoFeatures::kMaxSize)) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImUtil::TextShadow("Max Scale:");
                ImGui::TableSetColumnIndex(1);
                float MassModeScale = MassMode_GetVisualValues(a_Actor);
                
                if (MaxScale > 250.0f) {
                    if (IsPlayerMassMode) {
                        ImUtil::TextShadow("%.2fx out of Infinite", MaxScale);
                    } else {
                        ImUtil::TextShadow("Infinite");
                    }
                } else {
                    if (IsPlayerMassMode) {
                        ImUtil::TextShadow("%.2fx out of %.2fx", MaxScale, MassModeScale);
                    } else {
                        ImUtil::TextShadow("%.2fx", MaxScale);
                    }
                }
            }

            // Bonus Size
            if (hasFlag(a_featureFlags, GTSInfoFeatures::kBonusSize)) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImUtil::TextShadow("Bonus Size:");
                ImUtil::Tooltip(TBonusSize, true);
                ImGui::TableSetColumnIndex(1);
                ImUtil::TextShadow(IsPlayerMassMode ? "%.0f%% + [%.2F + %.2Fx possible]" : "%.0f%% + %.2F + %.2Fx", (BonusSize * 100.0f) + AspectOfGTS, 
                    MassMode ? (SizeEssense * MassMode_ElixirPowerMultiplier) + BonusSize_Overkills : (SizeEssense * 1.0f) + BonusSize_Overkills,
                    std::clamp(BonusSize_Overkills_Multiplier, 1.0f, 999999.0f)
                );
            }

            // Only for the player
            if (a_Actor->formID == 0x14) {

                // Shrink Resist
                if (hasFlag(a_featureFlags, GTSInfoFeatures::kShrinkResist)) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImUtil::TextShadow("Shrink Resist:");
                    ImUtil::Tooltip(TShrinkResist, true);
                    ImGui::TableSetColumnIndex(1);
                    ImUtil::TextShadow("%.1f%%", ShrinkResistance);
                }

                // On The Edge
                if (Runtime::HasPerk(a_Actor, "GTSPerkOnTheEdge") && hasFlag(a_featureFlags, GTSInfoFeatures::kOnTheEdge)) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImUtil::TextShadow("On The Edge:");
                    ImUtil::Tooltip(TOnTheEdge, true);
                    ImGui::TableSetColumnIndex(1);
                    ImUtil::TextShadow("%.1f%%", OnTheEdge);
                }

                // Size Reserve
                if (Runtime::HasPerk(a_Actor, "GTSPerkSizeReserve") && hasFlag(a_featureFlags, GTSInfoFeatures::kSizeReserve)) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImUtil::TextShadow("Size Reserve:");
                    ImUtil::Tooltip(TSizeReserve, true);
                    ImGui::TableSetColumnIndex(1);
                    ImUtil::TextShadow("%.2fx", SizeReserve);
                }
            }

            // Weight
            if (hasFlag(a_featureFlags, GTSInfoFeatures::kWeight)) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImUtil::TextShadow("Weight:");
                ImGui::TableSetColumnIndex(1);
                ImUtil::TextShadow(GTS::GetFormatedWeight(a_Actor).c_str());
            }

            // Aspect of GTS
            if (hasFlag(a_featureFlags, GTSInfoFeatures::kAspect)) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImUtil::TextShadow("Aspect of GTS:");
                ImUtil::Tooltip(TAspectOfGTS, true);
                ImGui::TableSetColumnIndex(1);
                ImUtil::TextShadow("%.0f%%", AspectOfGTS);
                
            }

            // Damage Resist
            if (hasFlag(a_featureFlags, GTSInfoFeatures::kDamageResist)) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImUtil::TextShadow("Damage Reduction:");
                ImUtil::Tooltip(TDamageResist, true);
                ImGui::TableSetColumnIndex(1);
                ImUtil::TextShadow("%.1f%%", DamageResist);
            }

            // High Heel Damage
            if (hasFlag(a_featureFlags, GTSInfoFeatures::kHeelsBonusDamage) && BonusHHDamage > 0.0f) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImUtil::TextShadow("High Heel Damage:");
                ImUtil::Tooltip(THHDamage, true);
                ImGui::TableSetColumnIndex(1);
                ImUtil::TextShadow("+%.0f%%", BonusHHDamage);
            }

            // Damage Multiplier
            if (hasFlag(a_featureFlags, GTSInfoFeatures::kDamageMultiplier)) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImUtil::TextShadow("Bonus Damage:");
                ImUtil::Tooltip(TDamageBonus, true);
                ImGui::TableSetColumnIndex(1);
                ImUtil::TextShadow("%.1f%%", Damage);
            }

            // Carry Weight
            if (hasFlag(a_featureFlags, GTSInfoFeatures::kCarryWeight)) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImUtil::TextShadow("Bonus Carry Weight:");
                ImGui::TableSetColumnIndex(1);
                ImUtil::TextShadow("%.1f", CarryWeight);
            }

            // Speed Multiplier
            if (hasFlag(a_featureFlags, GTSInfoFeatures::kSpeedMultiplier)) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImUtil::TextShadow("Bonus Speed:");
                ImGui::TableSetColumnIndex(1);
                ImUtil::TextShadow("%.1f%%", Speed);
            }

            // Jump Multiplier
            if (hasFlag(a_featureFlags, GTSInfoFeatures::kJumpMultiplier)) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImUtil::TextShadow("Bonus Jump Height:");
                ImGui::TableSetColumnIndex(1);
                ImUtil::TextShadow("%.1f%%", JumpHeight);
            }

            // Stolen Attributes for Size Conversion perk
            if (Runtime::HasPerk(a_Actor, "GTSPerkFullAssimilation") && hasFlag(a_featureFlags, GTSInfoFeatures::kStolenAttributes)) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImUtil::TextShadow("Stored Attributes:");
                ImUtil::Tooltip(TStoredAttributes, true);
                ImGui::TableSetColumnIndex(1);
                ImUtil::TextShadow("+%.2f", StolenAttributes);
            }

            // Soul Vore perk data
            if (Runtime::HasPerk(a_Actor, "GTSPerkFullAssimilation")) {
                // Stolen Health
                if (hasFlag(a_featureFlags, GTSInfoFeatures::kAbsorbedAttributes)) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImUtil::TextShadow("Absorbed Attributes:");
                    ImUtil::Tooltip(TAbsorbedAttributes, true);
                    ImGui::TableSetColumnIndex(1);
                    ImUtil::TextShadow("HP: +%.2f, MP: +%.2f, SP: +%.2f", StolenHealth, StolenMagicka, StolenStamina);
                }

                if (hasFlag(a_featureFlags, GTSInfoFeatures::kAbsorbedAttributesCap)) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImUtil::TextShadow("Max Attributes:");
                    ImUtil::Tooltip(TAbsorbedAttributesCap, true);
                    ImGui::TableSetColumnIndex(1);
                    ImUtil::TextShadow("%.2f", StolenCap);
                }
            }

            // Kills Made
            if (hasFlag(a_featureFlags, GTSInfoFeatures::kOverkillCounter)) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImUtil::TextShadow("Overkills Made:");
                ImUtil::Tooltip(TOverkillsMade, true);
                ImGui::TableSetColumnIndex(1);
                ImUtil::TextShadow("%u", GetKillCount(a_Actor, SizeKillType::kTotalKills));
            }

            ImGui::EndTable();
        }
    }
}
