#include "Managers/MaxSizeManager.hpp"
#include "Config/Config.hpp"
#include "Utils/Actions/ButtCrushUtils.hpp"

using namespace GTS;

namespace SizeOverride {

	bool SizeOverrideEnabled() {
		return !Config::Balance.bBalanceMode && Persistent::UnlockMaxSizeSliders.value;
	}

	void MassMode_ApplySizeOverride(Actor* a_actor, float& GetLimit) {
		if (SizeOverrideEnabled()) {

			float SizeOverride = 0.0f;
			if (a_actor->IsPlayerRef()) {
				SizeOverride = Config::Balance.fMaxPlayerSizeOverride;
			}
			else if (GTS::IsTeammate(a_actor)) {
				SizeOverride = Config::Balance.fMaxFollowerSize;
			}
			else {
				SizeOverride = Config::Balance.fMaxOtherSize;
			}

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


			BonusSize += GetGtsSkillLevel(a_Actor) * Perk_SizeManipulation_2;

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



    float get_mass_based_limit(Actor* actor, float NaturalScale) { // gets mass based size limit for Player if using Mass Based mode

		float GetLimit = 1.0f;

		if (auto data = Persistent::GetActorData(actor); data) {
			float ExtraMagicSize = data->fExtraPotionMaxScale * MassMode_ElixirPowerMultiplier;;
			float MaxSize = data->fSizeLimit; // Cap max size through normal size rules
			float size_calc = NaturalScale + data->fMassBasedLimit + ExtraMagicSize;

			// Multiplying MassBasedSizeLimit by Natural Scale is a bad idea, it messes up max scaling with level 100 perk, displays 32x out of 34x

			GetLimit = std::clamp(size_calc, NaturalScale, MaxSize);
			SizeOverride::MassMode_ApplySizeOverride(actor, GetLimit);
		}

        return GetLimit;
    }

	float get_default_size_limit(float NaturalScale, float BaseLimit) { // default size limit for everyone
		float size_calc = NaturalScale + ((BaseLimit - 1.0f) * NaturalScale);
		float GetLimit = std::clamp(size_calc, 0.1f, DEFAULT_MAX);

		return GetLimit;
	}

    /*float get_follower_size_limit(float NaturalScale, float FollowerLimit) { // Self explanatory
        float size_calc = NaturalScale + ((FollowerLimit) * NaturalScale);
        float GetLimit = std::clamp(size_calc, 1.0f * FollowerLimit, DEFAULT_MAX);

        return GetLimit;
    }

    float get_npc_size_limit(float NaturalScale, float NPCLimit) { // get non-follower size limit
        float size_calc = NaturalScale + ((NPCLimit - 1.0f) * NaturalScale);
		float GetLimit = std::clamp(size_calc, 1.0f * NPCLimit, DEFAULT_MAX);

        return GetLimit;
    }*/
}

namespace GTS {

    void UpdateMaxScale(Actor* a_actor) {
       	GTS_PROFILE_SCOPE("MaxSizeManager: UpdateMaxScale");

		const bool IsMassBased = Config::Balance.sSizeMode == "kMassBased"; // Should DLL use mass based formula for Player?

		float Limit = 1.0f;
		// -------------------------------------------------------------------------------------------------

		if (auto data = Persistent::GetActorData(a_actor); data) {
			Limit = data->fSizeLimit; // Cap max size through normal size rules
		}

		// Apply custom limits only if player has Perk and gts unlimited command was executed, else use GlobalLimit

        float Endless = get_endless_height(a_actor);

        const float NaturalScale = get_natural_scale(a_actor, true);
		float GetLimit = get_default_size_limit(NaturalScale, Limit); // Default size limit
		
		if (IsMassBased) {
			GetLimit = get_mass_based_limit(a_actor, NaturalScale); // Apply Player Mass-Based max size
		}

		/*else if (QuestStage > 100 && FollowerLimit > 0.0f && FollowerLimit != 1.0f && a_actor->formID != 0x14 && IsTeammate(a_actor)) {
			GetLimit = get_follower_size_limit(NaturalScale, FollowerLimit); // Apply Follower Max Size
		}
		else if (QuestStage > 100 && NPCLimit > 0.0f && NPCLimit != 1.0f && a_actor->formID != 0x14 && !IsTeammate(a_actor)) {
            GetLimit = get_npc_size_limit(NaturalScale, NPCLimit); // Apply Other NPC's max size
		}*/

        float TotalLimit = GetLimit;
        Ench_Potions_ApplyBonuses(a_actor, TotalLimit);

		if (IsSizeUnlocked()) {
			float OverrideLimit = 0.0f;
			if (a_actor->IsPlayerRef()) {
				OverrideLimit = Config::Balance.fMaxPlayerSizeOverride;
			}
			else if (IsTeammate(a_actor)) {
				OverrideLimit = Config::Balance.fMaxFollowerSize;
			}
			else {
				OverrideLimit = Config::Balance.fMaxOtherSize;
			}

			if (OverrideLimit > 0.05f) {
				TotalLimit = OverrideLimit;
			}
		}

		if (get_max_scale(a_actor) < TotalLimit + Endless || get_max_scale(a_actor) > TotalLimit + Endless) {
			set_max_scale(a_actor, TotalLimit);
		}
		
    }

    //Ported From Papyrus
	float GetExpectedMaxSize(RE::Actor* a_Actor, float start_value) {
		const bool IsMassBased = Config::Balance.sSizeMode == "kMassBased";
		const float LevelBonus = 1.0f + GetGtsSkillLevel(a_Actor) * 0.006f;
		float PotionSize = 0.0f;
		float Colossal_kills = 0.0f;
		float Colossal_lvl = 1.0f;
		float QuestMult = 0.0f;

		if (auto data = Persistent::GetActorData(a_Actor); data) {
			PotionSize = data->fExtraPotionMaxScale * (IsMassBased ? MassMode_ElixirPowerMultiplier : 1.0f);
		}

		const auto Quest = Runtime::GetQuest(Runtime::QUST.GTSQuestProgression);
		if (!Quest) {
			return 1.0f;
		}

		//If Player
		const auto Stage = Quest->GetCurrentStageID();
		if (Stage < 20) {
			return 1.0f;
		}

		//if (a_Actor->IsPlayerRef()) {
			//Each stage after 20 adds 0.04f in steps of 10 stages
			//Base value + Current Stage - 20 / 10
			QuestMult = 0.10f + static_cast<float>(Stage - 20) / 10.f * 0.04f;
			if (Stage >= 80) QuestMult = 0.60f;
		//}

		auto Transient = Transient::GetActorData(a_Actor);

		if (Runtime::HasPerk(a_Actor, Runtime::PERK.GTSPerkColossalGrowth)) { //Total Size Control Perk
			Colossal_lvl = 1.15f;
			if (auto KillData = Persistent::GetKillCountData(a_Actor)) {
				Colossal_kills = static_cast<float>(KillData->iTotalKills) * (0.02f / Characters_AssumedCharSize);
				if (Runtime::HasPerk(a_Actor, Runtime::PERK.GTSPerkOverindulgence)) {
					Colossal_lvl += KillData->iTotalKills * Overkills_BonusSizePerKill;
				}
			}

			RecordOverkillSize_Transient(Transient, Colossal_lvl, Colossal_kills);
			
			if (SizeOverride::SizeOverrideEnabled()) {
				float SizeOverride = 0.0f;
				if (a_Actor->IsPlayerRef()) {
					SizeOverride = Config::Balance.fMaxPlayerSizeOverride;
				}
				else if (IsTeammate(a_Actor)){
					SizeOverride = Config::Balance.fMaxFollowerSize;
				}
				else {
					SizeOverride = Config::Balance.fMaxOtherSize;
				}

				if (SizeOverride > 0.05f) {
					return SizeOverride;
				}
			}
		} 
    	else {
			RecordOverkillSize_Transient(Transient, 1.0f, 0.0f);
		}

		const float MaxAllowedSize = start_value + (QuestMult + GetSizeFromPerks(a_Actor)) * LevelBonus;
		return (MaxAllowedSize + PotionSize + Colossal_kills) * Colossal_lvl;
	}

    //Ported From Papyrus
	void UpdateGlobalSizeLimit(Actor* a_actor) {
		if (const auto data = Persistent::GetActorData(a_actor)) {
			data->fSizeLimit = GetExpectedMaxSize(a_actor);
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
			if (auto data = Persistent::GetActorData(actor); data) {
				float MassModeScale = data->fSizeLimit;
				Ench_Potions_ApplyBonuses(actor, MassModeScale);
				return MassModeScale;
			}
		}
		return 1.0f;
	}

	//GTS Skill Boosts For NPCs
	void SetNPCSkillLevelByPerk(Actor* a_actor) {

		if (auto data = Persistent::GetActorData(a_actor); data) {

			if (Runtime::HasPerk(a_actor, Runtime::PERK.GTSNPCPerkSkilled100) && data->fGTSSkillLevel < 100.f) {
				data->fGTSSkillLevel = 100.0f;
			}

			else if (Runtime::HasPerk(a_actor, Runtime::PERK.GTSNPCPerkSkilled90) && data->fGTSSkillLevel < 90.f) {
				data->fGTSSkillLevel = static_cast<float>(RandomInt(90, 95));
			}

			else if (Runtime::HasPerk(a_actor, Runtime::PERK.GTSNPCPerkSkilled70) && data->fGTSSkillLevel < 70.f) {
				data->fGTSSkillLevel = static_cast<float>(RandomInt(70, 75));
			}

			else if (Runtime::HasPerk(a_actor, Runtime::PERK.GTSNPCPerkSkilled50) && data->fGTSSkillLevel < 50.f) {
				data->fGTSSkillLevel = static_cast<float>(RandomInt(50, 55));
			}

			else if (Runtime::HasPerk(a_actor, Runtime::PERK.GTSNPCPerkSkilled30) && data->fGTSSkillLevel < 30.f) {
				data->fGTSSkillLevel = static_cast<float>(RandomInt(30, 35));
			}

			else if (Runtime::HasPerk(a_actor, Runtime::PERK.GTSNPCPerkSkilled10) && data->fGTSSkillLevel < 10.f) {
				data->fGTSSkillLevel = static_cast<float>(RandomInt(10, 15));
			}
		}
	}
}

