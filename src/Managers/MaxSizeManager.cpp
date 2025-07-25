#include "Managers/MaxSizeManager.hpp"
#include "Config/Config.hpp"
#include "Utils/ButtCrushUtils.hpp"

using namespace GTS;

namespace {

    constexpr float DEFAULT_MAX = 1000000.0f;

	void RecordOverkillSize_Transient(TempActorData* Data, float value, float kills) {
		if (Data) {
			Data->OverkillSizeBonus = value;
			Data->Overkills = kills;
		}
	}

    //Ported From Papyrus
	float GetSizeFromPerks(RE::Actor* a_Actor) {
		float BonusSize = 0.0f;

		if (Runtime::HasPerk(a_Actor,"GTSPerkSizeManipulation3")) { //SizeManipulation 3
			BonusSize += static_cast<float>(a_Actor->GetLevel()) * 0.0330f;
		}

		if (Runtime::HasPerk(a_Actor,"GTSPerkSizeManipulation2")) { //SizeManipulation 2
			BonusSize += Runtime::GetFloat("GTSSkillLevel") * 0.0165f;
		}

		if (Runtime::HasPerk(a_Actor,"GTSPerkSizeManipulation1")) { //SizeManipulation 1
			BonusSize += 0.135f;
		}

		return BonusSize;
	}

    float get_endless_height(Actor* actor) {
		float endless = 0.0f;

		if (Runtime::HasPerk(actor, "GTSPerkColossalGrowth") && Persistent::GetSingleton().UnlockMaxSizeSliders.value) {
			endless = DEFAULT_MAX;
		}

		return endless;
	}

    float get_default_size_limit(float NaturalScale, float BaseLimit) { // default size limit for everyone
        float size_calc = NaturalScale + ((BaseLimit - 1.0f) * NaturalScale);
        float GetLimit = std::clamp(size_calc, 0.1f, DEFAULT_MAX);

        return GetLimit;
    }

    float get_mass_based_limit(Actor* actor, float NaturalScale) { // get mass based size limit for Player if using Mass Based mode
        float low_limit = get_endless_height(actor);
        const bool MassMode = Config::GetBalance().sSizeMode == "kMassBased";

        if (low_limit <= 0.0f) {

            low_limit = Persistent::GetSingleton().GlobalSizeLimit.value; // Cap max size through normal size rules
            // Else max possible size is unlimited
        }


    	const float PotionSize = Persistent::GetSingleton().PlayerExtraPotionSize.value;

        float size_calc = NaturalScale + Persistent::GetSingleton().GlobalMassBasedSizeLimit.value * NaturalScale;
        
        MassMode ? size_calc += (PotionSize * MassMode_ElixirPowerMultiplier) : size_calc += PotionSize; // less effective in mass mode

        float GetLimit = std::clamp(size_calc, NaturalScale, low_limit);

        return GetLimit;
    }

    float get_follower_size_limit(float NaturalScale, float FollowerLimit) { // Self explanatory
        float size_calc = NaturalScale + ((FollowerLimit) * NaturalScale);
        float GetLimit = std::clamp(size_calc, 1.0f * FollowerLimit, DEFAULT_MAX);

        return GetLimit;
    }

    float get_npc_size_limit(float NaturalScale, float NPCLimit) { // get non-follower size limit
        float size_calc = NaturalScale + ((NPCLimit - 1.0f) * NaturalScale);
		float GetLimit = std::clamp(size_calc, 1.0f * NPCLimit, DEFAULT_MAX);

        return GetLimit;
    }
}

namespace GTS {

    void UpdateMaxScale() {

        auto profiler = Profilers::Profile("MaxSizeManager: UpdateMaxScale");

		for (auto actor: find_actors()) {

			float Endless = 0.0f;

			if (actor->formID == 0x14) {
				Endless = get_endless_height(actor);
			}

            const float NaturalScale = get_natural_scale(actor, true);
            const float QuestStage = Runtime::GetStage("GTSQuestProgression");
            const float BaseLimit = Persistent::GetSingleton().GlobalSizeLimit.value;
            const float NPCLimit = Config::GetBalance().fMaxOtherSize;
			const bool IsMassBased = Config::GetBalance().sSizeMode == "kMassBased"; // Should DLL use mass based formula for Player?
            const float FollowerLimit = Config::GetBalance().fMaxFollowerSize;

			float GetLimit = get_default_size_limit(NaturalScale, BaseLimit); // Default size limit
			
			if (actor->formID == 0x14 && IsMassBased) { 
				GetLimit = get_mass_based_limit(actor, NaturalScale); // Apply Player Mass-Based max size
			}
			else if (QuestStage > 100 && FollowerLimit > 0.0f && FollowerLimit != 1.0f && actor->formID != 0x14 && IsTeammate(actor)) { 
				GetLimit = get_follower_size_limit(NaturalScale, FollowerLimit); // Apply Follower Max Size
			}
			else if (QuestStage > 100 && NPCLimit > 0.0f && NPCLimit != 1.0f && actor->formID != 0x14 && !IsTeammate(actor)) { 
                GetLimit = get_npc_size_limit(NaturalScale, NPCLimit); // Apply Other NPC's max size
			}

            float TotalLimit = GetLimit;
            TotalLimit *= Potion_GetSizeMultiplier(actor); //Potion size
            TotalLimit += GetButtCrushSize(actor); //Butt crush added size
            TotalLimit *= 1.0f + Ench_Aspect_GetPower(actor); //Enchantment

			if (get_max_scale(actor) < TotalLimit + Endless || get_max_scale(actor) > TotalLimit + Endless) {
				set_max_scale(actor, TotalLimit);
			}
		}
    }

    //Ported From Papyrus
	float GetExpectedMaxSize(RE::Actor* a_Actor) {
		const float LevelBonus = 1.0f + GetGtsSkillLevel(a_Actor) * 0.006f;
		const float Essence = Persistent::GetSingleton().PlayerExtraPotionSize.value;
		float Colossal_kills = 0.0f;
		float Colossal_lvl = 1.0f;

		const auto Quest = Runtime::GetQuest("GTSQuestProgression");
		if (!Quest) {
			return 1.0f;
		}

		const auto Stage = Quest->GetCurrentStageID();
		if (Stage < 20) {
			return 1.0f;
		}

		auto Transient = Transient::GetSingleton().GetActorData(a_Actor);

		//Each stage after 20 adds 0.04f in steps of 10 stages
		//Base value + Current Stage - 20 / 10
		float QuestMult = 0.10f + static_cast<float>(Stage - 20) / 10.f * 0.04f;
		if (Stage >= 80) QuestMult = 0.60f;

		if (Runtime::HasPerk(a_Actor,"GTSPerkColossalGrowth")) { //Total Size Control Perk
			auto Persistent = Persistent::GetSingleton().GetKillCountData(a_Actor);
			Colossal_lvl = 1.15f;

			if (Persistent) {
				Colossal_kills = static_cast<float>(Persistent->iTotalKills) * (0.02f / Characters_AssumedCharSize);
				if (Runtime::HasPerk(a_Actor, "GTSPerkOverindulgence")) {
					Colossal_lvl += static_cast<float>(Persistent->iTotalKills * Overkills_BonusSizePerKill);
				}
			}

			RecordOverkillSize_Transient(Transient, Colossal_lvl, Colossal_kills);
			
			if (!Config::GetBalance().bBalanceMode && Persistent::GetSingleton().UnlockMaxSizeSliders.value) {
				const float SizeOverride = Config::GetBalance().fMaxPlayerSizeOverride;
				if (SizeOverride > 0.05f) {
					Colossal_kills = 0.0f;
					Colossal_lvl = 1.0f;
					return SizeOverride;
				}
			}
		} else {
			RecordOverkillSize_Transient(Transient, 1.0f, 0.0f);
		}

		const float MaxAllowedSize = 1.0f + (QuestMult + GetSizeFromPerks(a_Actor)) * LevelBonus;
		return (MaxAllowedSize + Essence + Colossal_kills) * Colossal_lvl;
	}

    //Ported From Papyrus
	void UpdateGlobalSizeLimit() {
		if (const auto Player = PlayerCharacter::GetSingleton()) {
			Persistent::GetSingleton().GlobalSizeLimit.value = GetExpectedMaxSize(Player);
		}
	}

	void VisualScale_CheckForSizeAdjustment(Actor* actor, float& ScaleMult) {
		if (Persistent::GetSingleton().UnlockMaxSizeSliders.value) {
			const bool Unlocked = Runtime::HasPerk(PlayerCharacter::GetSingleton(), "GTSPerkColossalGrowth");
			if (Unlocked) {
				const float PCLimit = Config::GetBalance().fMaxPlayerSizeOverride;
				const float NPCLimit = Config::GetBalance().fMaxOtherSize;
				const float FollowerLimit = Config::GetBalance().fMaxFollowerSize;
				if (IsTeammate(actor)) {
					ScaleMult = std::clamp(FollowerLimit, 0.1f, 1.0f);
				} else if (actor->formID == 0x14) {
					ScaleMult = std::clamp(PCLimit, 0.1f, 1.0f);
				} else {
					ScaleMult = std::clamp(NPCLimit, 0.1f, 1.0f);
				}
			}
		}
	}
}
