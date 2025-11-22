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

		static bool Perks_Cataclysmic_HasStacks(Actor* giant);
		static void Perks_Cataclysmic_ManageStacks(Actor* giant, PerkAction action);
		static float Perks_Cataclysmic_EmpowerStomp(Actor* giant);
		static void Perks_Cataclysmic_BuffStompSpeed(AnimationEventData& data, bool reset);

		static void UpdatePerkValues(Actor* giant, PerkUpdate Type);
	};
}