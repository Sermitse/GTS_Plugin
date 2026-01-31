#pragma once

#include "Managers/Animation/AnimationManager.hpp"

namespace GTS {

	/**
	 * The class which manages some perk bonuses
	 */

	enum class PerkAction {
		Increase,
		Decrease
	};

	class PerkHandler : public EventListener, public CInitSingleton <PerkHandler> {
		public:
        virtual std::string DebugName() override;
		virtual void OnAddPerk(const AddPerkEvent& evt) override;
        virtual void OnRemovePerk(const RemovePerkEvent& evt) override;
		virtual void OnGTSLevelUp(Actor* a_actor) override;
		virtual void ActorLoaded(RE::Actor* actor) override;

        static void SetNPCSkillLevelByPerk(Actor* a_actor);


        static bool Perks_Cataclysmic_HasStacks(Actor* giant);
		static void Perks_Cataclysmic_ManageStacks(Actor* giant, int add_stacks = 0);
		static float Perks_Cataclysmic_EmpowerStomp(Actor* giant);
		static void Perks_Cataclysmic_BuffStompSpeed(AnimationEventData& data, bool reset);
		static void RuntimeGivePerksToNPC(Actor* a_actor, float a_currentSkillLevel);

		static void UpdatePerkValues(Actor* giant, PerkUpdate Type);
	};
}