#include "Magic/Effects/Common.hpp"
#include "Magic/Effects/Spells/RestoreSize.hpp"
#include "Managers/Rumble.hpp"
#include "Managers/Animation/Utils/CooldownManager.hpp"

using namespace GTS;

namespace {

	void Task_RestoreSizeTask(Actor* caster, bool dual_casted) {

		float Power = 0.00120f;

		if (dual_casted) {
			Power *= 2.0f;
		}

		std::string name = std::format("RevertSize_{}", caster->formID);
		ActorHandle casterhandle = caster->CreateRefHandle();

		TaskManager::RunFor(name, 180.0f, [=](auto& progressData) {
			if (!casterhandle) {
				return false;
			}
			auto casterref = casterhandle.get().get();

			bool BlockSound = IsActionOnCooldown(casterref, CooldownSource::Misc_RevertSound);
			if (!BlockSound) {
				float Volume = std::clamp(get_visual_scale(casterref) * 0.1f, 0.15f, 1.0f);
				ApplyActionCooldown(casterref, CooldownSource::Misc_RevertSound);
				Runtime::PlaySound(Runtime::SNDR.GTSSoundShrink, casterref, Volume, 1.0f);
			}

			Rumbling::Once("RestoreSizeOther", casterref, 0.6f, 0.05f);

			if (!Revert(casterref, Power, Power/2.5f)) { // Terminate the task once revert size is complete
				return false;
			}
			return true;
		});
	}
}

namespace GTS {

	std::string RestoreSize::GetName() {
		return "::RestoreSize";
	}

	void RestoreSize::OnStart() {
		Actor* caster = GetCaster();
		if (!caster) {
			return;
		}
		float Volume = std::clamp(get_visual_scale(caster) * 0.1f, 0.10f, 1.0f);
		Runtime::PlaySound(Runtime::SNDR.GTSSoundShrink, caster, Volume, 1.0f);

		//log::info("Starting Reset Size of {}", caster->GetDisplayFullName());

		Task_RestoreSizeTask(caster, DualCasted());
	}
}
