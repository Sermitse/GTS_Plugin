
#include "Magic/Effects/Spells/Restore_Size_Other.hpp"
#include "Magic/Effects/Common.hpp"
#include "Managers/Rumble.hpp"
#include "Managers/Animation/Utils/CooldownManager.hpp"

using namespace GTS;

namespace {

	void Task_RestoreSizeTask(Actor* caster, Actor* target, bool dual_casted) {

		float Power = 0.00120f;

		if (dual_casted) {
			Power *= 2.0f;
		}

		std::string name = std::format("RevertSize_{}_{}", caster->formID, target->formID);
		ActorHandle targethandle = target->CreateRefHandle();

		TaskManager::RunFor(name, 180.0f, [=](auto& progressData) {
			if (!targethandle) {
				return false;
			}
			auto targetref = targethandle.get().get();

			bool BlockSound = IsActionOnCooldown(targetref, CooldownSource::Misc_RevertSound);

			if (!BlockSound) {
				float Volume = std::clamp(get_visual_scale(targetref) * 0.1f, 0.15f, 1.0f);
				ApplyActionCooldown(targetref, CooldownSource::Misc_RevertSound);
				//Runtime::PlaySound("GTSSoundShrink", targetref, Volume, 1.0f);
			}

			Rumbling::Once("RestoreSizeOther", targetref, 0.6f, 0.05f);

			if (!Revert(targetref, Power, Power/2.5f)) { // Terminate the task once revert size is complete
				return false;
			}
			return true;
		});
	}
}

namespace GTS {
	std::string RestoreSizeOther::GetName() {
		return "RestoreSizeOther";
	}

	void RestoreSizeOther::OnStart() {
		Actor* target = GetTarget();
		Actor* caster = GetCaster();
		if (!target) {
			return;
		}
		if (!caster) {
			return;
		}
		//float Volume = std::clamp(get_visual_scale(target) * 0.1f, 0.15f, 1.0f);
		//Runtime::PlaySound("GTSSoundShrink", target, Volume, 1.0f);

		Task_RestoreSizeTask(caster, target, DualCasted());
	}

}
