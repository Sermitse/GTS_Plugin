#include "Managers/MaxSizeManager.hpp"
#include "Config/Config.hpp"
#include "Utils/Actions/ButtCrushUtils.hpp"

using namespace GTS;

namespace SizeOverride {
	bool SizeOverrideEnabled() {
		return !Config::Balance.bBalanceMode && Persistent::UnlockMaxSizeSliders.value;
	}
	void MassMode_ApplySizeOverride(float& GetLimit) {
		if (SizeOverrideEnabled()) {
			const float SizeOverride = Config::Balance.fMaxPlayerSizeOverride;
			if (SizeOverride > 0.05f) {
				GetLimit = SizeOverride;
			}
		}
	}
}

namespace {

    constexpr float DEFAULT_MAX = 1'000'000.0f;

    bool IsSizeUnlocked() {
		// Reports true when player has ColossalGrowth perk and used gts unlimited command, else it's false
		if (Persistent::UnlockMaxSizeSliders.value) {
			const bool Unlocked = Runtime::HasPerk(PlayerCharacter::GetSingleton(), Runtime::PERK.GTSPerkColossalGrowth);
			return Unlocked;
		}
		return false;
	}

	void RecordOverkillSize_Transient(TransientActorData* Data, float value, float kills) {
		if (Data) {
			Data->OverkillSizeBonus = value;
			Data->Overkills = kills;
		}
	}

    //Ported From Papyrus
	float GetSizeFromPerks(RE::Actor* a_Actor) {
		float BonusSize = 0.0f;

		if (Runtime::HasPerk(a_Actor, Runtime::PERK.GTSPerkSizeManipulation3)) {
			BonusSize += static_cast<float>(a_Actor->GetLevel()) * Perk_SizeManipulation_3;
		}

		if (Runtime::HasPerk(a_Actor, Runtime::PERK.GTSPerkSizeManipulation2)) {
			BonusSize += Runtime::GetFloat(Runtime::GLOB.GTSSkillLevel) * Perk_SizeManipulation_2;
		}

		if (Runtime::HasPerk(a_Actor, Runtime::PERK.GTSPerkSizeManipulation1)) {
			BonusSize += Perk_SizeManipulation_1;
		}

		return BonusSize;
	}

    float get_endless_height(Actor* actor) {
		float endless = 0.0f;

		if (Runtime::HasPerk(actor, Runtime::PERK.GTSPerkColossalGrowth) && Persistent::UnlockMaxSizeSliders.value) {
			endless = DEFAULT_MAX;
		}

		return endless;
	}

    float get_default_size_limit(float NaturalScale, float BaseLimit) { // default size limit for everyone
        float size_calc = NaturalScale + ((BaseLimit - 1.0f) * NaturalScale);
        float GetLimit = std::clamp(size_calc, 0.1f, DEFAULT_MAX);

        return GetLimit;
    }

    float get_mass_based_limit(Actor* actor, float NaturalScale) { // gets mass based size limit for Player if using Mass Based mode
        float MaxSize = Persistent::GlobalSizeLimit.value; // Cap max size through normal size rules
		
    	const float PotionSize = Persistent::PlayerExtraPotionSize.value * MassMode_ElixirPowerMultiplier;

        float size_calc = NaturalScale + Persistent::GlobalMassBasedSizeLimit.value + PotionSize;
		// Multiplying MassBasedSizeLimit by Natural Scale is a bad idea, it messes up max scaling with level 100 perk, displays 32x out of 34x

        float GetLimit = std::clamp(size_calc, NaturalScale, MaxSize);

		SizeOverride::MassMode_ApplySizeOverride(GetLimit);

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
       	GTS_PROFILE_SCOPE("MaxSizeManager: UpdateMaxScale");

		const bool IsMassBased = Config::Balance.sSizeMode == "kMassBased"; // Should DLL use mass based formula for Player?
		const bool SizeUnlocked = IsSizeUnlocked();

		const float QuestStage = Runtime::GetStage(Runtime::QUST.GTSQuestProgression);

		// -------------------------------------------------------------------------------------------------
		const float GlobalLimit = Persistent::GlobalSizeLimit.value;
		const float FollowerLimit = SizeUnlocked ? Config::Balance.fMaxFollowerSize : GlobalLimit;
		const float NPCLimit = SizeUnlocked ? Config::Balance.fMaxOtherSize : GlobalLimit;
		// Apply custom limits only if player has Perk and gts unlimited command was executed, else use GlobalLimit

		for (auto actor: find_actors()) {

			float Endless = 0.0f;

			if (actor->formID == 0x14) {
				Endless = get_endless_height(actor);
			}

            const float NaturalScale = get_natural_scale(actor, true);
			float GetLimit = get_default_size_limit(NaturalScale, GlobalLimit); // Default size limit
			
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
            Ench_Potions_ApplyBonuses(actor, TotalLimit);

			if (get_max_scale(actor) < TotalLimit + Endless || get_max_scale(actor) > TotalLimit + Endless) {
				set_max_scale(actor, TotalLimit);
			}
		}
    }

    //Ported From Papyrus
	float GetExpectedMaxSize(RE::Actor* a_Actor, float start_value) {
		const bool IsMassBased = Config::Balance.sSizeMode == "kMassBased";

		const float LevelBonus = 1.0f + GetGtsSkillLevel(a_Actor) * 0.006f;
		const float PotionSize = Persistent::PlayerExtraPotionSize.value * (IsMassBased ? MassMode_ElixirPowerMultiplier : 1.0f);
		float Colossal_kills = 0.0f;
		float Colossal_lvl = 1.0f;

		const auto Quest = Runtime::GetQuest(Runtime::QUST.GTSQuestProgression);
		if (!Quest) {
			return 1.0f;
		}

		const auto Stage = Quest->GetCurrentStageID();
		if (Stage < 20) {
			return 1.0f;
		}

		auto Transient = Transient::GetActorData(a_Actor);

		//Each stage after 20 adds 0.04f in steps of 10 stages
		//Base value + Current Stage - 20 / 10
		float QuestMult = 0.10f + static_cast<float>(Stage - 20) / 10.f * 0.04f;
		if (Stage >= 80) QuestMult = 0.60f;

		if (Runtime::HasPerk(a_Actor, Runtime::PERK.GTSPerkColossalGrowth)) { //Total Size Control Perk
			auto Persistent = Persistent::GetKillCountData(a_Actor);
			Colossal_lvl = 1.15f;

			if (Persistent) {
				Colossal_kills = static_cast<float>(Persistent->iTotalKills) * (0.02f / Characters_AssumedCharSize);
				if (Runtime::HasPerk(a_Actor, Runtime::PERK.GTSPerkOverindulgence)) {
					Colossal_lvl += Persistent->iTotalKills * Overkills_BonusSizePerKill;
				}
			}

			RecordOverkillSize_Transient(Transient, Colossal_lvl, Colossal_kills);
			
			if (SizeOverride::SizeOverrideEnabled()) {
				const float SizeOverride = Config::Balance.fMaxPlayerSizeOverride;
				if (SizeOverride > 0.05f) {
					Colossal_kills = 0.0f;
					Colossal_lvl = 1.0f;
					return SizeOverride;
				}
			}
		} else {
			RecordOverkillSize_Transient(Transient, 1.0f, 0.0f);
		}

		const float MaxAllowedSize = start_value + (QuestMult + GetSizeFromPerks(a_Actor)) * LevelBonus;
		return (MaxAllowedSize + PotionSize + Colossal_kills) * Colossal_lvl;
	}

    //Ported From Papyrus
	void UpdateGlobalSizeLimit() {
		if (const auto Player = PlayerCharacter::GetSingleton()) {
			Persistent::GlobalSizeLimit.value = GetExpectedMaxSize(Player);
		}
	}

	void VisualScale_CheckForSizeAdjustment(Actor* actor, float& ScaleMult) {
		if (IsSizeUnlocked()) {
			const float PCLimit = Config::Balance.fMaxPlayerSizeOverride;
			const float NPCLimit = Config::Balance.fMaxOtherSize;
			const float FollowerLimit = Config::Balance.fMaxFollowerSize;
			if (IsTeammate(actor)) {
				ScaleMult = std::clamp(FollowerLimit, 0.1f, 1.0f);
			} else if (actor->formID == 0x14) {
				ScaleMult = std::clamp(PCLimit, 0.1f, 1.0f);
			} else {
				ScaleMult = std::clamp(NPCLimit, 0.1f, 1.0f);
			}
		}
	}

	void Ench_Potions_ApplyBonuses(Actor* actor, float& value) {
		if (actor) {
			value *= Potion_GetSizeMultiplier(actor); //Potion size
			value += GetButtCrushSize(actor); //Butt crush added size
			value *= 1.0f + Ench_Aspect_GetPower(actor); //Enchantment
		}
	}

	float MassMode_GetValuesForMenu(Actor* actor) {
		if (actor) {
			float MassModeScale = Persistent::GlobalSizeLimit.value;
			Ench_Potions_ApplyBonuses(actor, MassModeScale);
			
			return MassModeScale;
		}
		return 1.0f;
	}
}

