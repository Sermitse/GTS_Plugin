#include "Managers/ShrinkToNothingManager.hpp"

#include "Config/Config.hpp"

#include "Managers/AI/AIFunctions.hpp"
#include "Managers/Perks/PerkHandler.hpp"
#include "Magic/Effects/Common.hpp"
#include "Utils/looting.hpp"

using namespace GTS;

namespace {
	float GetXPModifier(Actor* tiny) {
		float mult = 1.0f;
		if (tiny->IsDead()) {
			mult = 0.25f;
		}
		
		return mult;
	}
}

namespace GTS {

	std::string ShrinkToNothingManager::DebugName() {
		return "::ShrinkToNothingManager";
	}

	void ShrinkToNothingManager::Update() {
		GTS_PROFILE_SCOPE("ShrinkToNothingManager: Update");
		for (auto &[tinyId, data]: this->data) {
			auto tiny = TESForm::LookupByID<Actor>(tinyId);
			auto giantHandle = data.giant;
			if (!tiny) {
				continue;
			}
			if (!giantHandle) {
				continue;
			}
			auto giant = giantHandle.get().get();
			if (!giant) {
				continue;
			}

			if (data.state == ShrinkState::Healthy) {
				SetReanimatedState(tiny);
				data.state = ShrinkState::Shrinking;
			} else if (data.state == ShrinkState::Shrinking) {
				ModSizeExperience(giant, 0.24f * GetXPModifier(tiny)); // Adjust Size Matter skill
				Attacked(tiny, giant);
				if (giant->formID == 0x14 && IsDragon(tiny)) {
					CompleteDragonQuest(tiny, ParticleType::Red);
				}

				const bool silent = Config::Audio.bMuteShrinkToNothingDeathScreams;

				SpawnDeathEffects(tiny);
				DecreaseShoutCooldown(giant);
				KillActor(giant, tiny, silent);

				PerkHandler::UpdatePerkValues(giant, PerkUpdate::Perk_LifeForceAbsorption);

				AddSMTDuration(giant, 5.0f);

				TransferInventoryTask(giant, tiny); // Also plays STN sound

				data.state = ShrinkState::Shrinked;
			}
		}
	}
	


	void ShrinkToNothingManager::Reset() {
		this->data.clear();
	}

	void ShrinkToNothingManager::ResetActor(Actor* actor) {
		if (actor) {
			this->data.erase(actor->formID);
		}
	}

	void ShrinkToNothingManager::Shrink(Actor* giant, Actor* tiny) {
		if (!tiny) {
			return;
		}
		if (!giant) {
			return;
		}
		if (CanShrink(giant, tiny)) {
			GetSingleton().data.try_emplace(tiny->formID, giant);
		}
	}

	bool ShrinkToNothingManager::AlreadyShrinked(Actor* actor) {
		if (!actor) {
			return false;
		}
		auto& m = GetSingleton().data;
		return !(!m.contains(actor->formID));
	}

	bool ShrinkToNothingManager::CanShrink(Actor* giant, Actor* tiny) {
		if (AlreadyShrinked(tiny)) {
			return false;
		}
		if (IsEssential(giant, tiny)) {
			return false;
		}
		if (IsFlying(tiny)) {
			if (get_visual_scale(tiny) < Minimum_Actor_Scale) {
				set_target_scale(tiny, Minimum_Actor_Scale);
			}
			return false;
		}

		return true;
	}

	void ShrinkToNothingManager::SpawnDeathEffects(Actor* tiny) {
		if (!IsLiving(tiny)) {
			SpawnDustParticle(tiny, tiny, "NPC Root [Root]", 3.6f);
		} else {
			if (!Config::General.bLessGore) {
				std::random_device rd;
				std::mt19937 gen(rd());
				std::uniform_real_distribution<float> dis(-0.2f, 0.2f);
				auto root = find_node(tiny, "NPC Root [Root]");
				if (root) {
					SpawnParticle(tiny, 0.20f, "GTS/Damage/Explode.nif", NiMatrix3(), root->world.translate, 2.0f, 7, root);
					SpawnParticle(tiny, 0.20f, "GTS/Damage/Explode.nif", NiMatrix3(), root->world.translate, 2.0f, 7, root);
					SpawnParticle(tiny, 0.20f, "GTS/Damage/Explode.nif", NiMatrix3(), root->world.translate, 2.0f, 7, root);
					SpawnParticle(tiny, 1.20f, "GTS/Damage/ShrinkOrCrush.nif", NiMatrix3(), root->world.translate, get_visual_scale(tiny) * 10, 7, root);
				}
				Runtime::CreateExplosion(tiny, get_visual_scale(tiny)/4, Runtime::EXPL.GTSExplosionBlood);
				Runtime::PlayImpactEffect(tiny, Runtime::IDTS.GTSBloodSprayImpactSet, "NPC Root [Root]", NiPoint3{0, 0, -1}, 512, false, false);
			} 
			else {
				Runtime::PlaySound("SKSoundBloodGush", tiny, 1.0f, 1.0f);
			}
		}
	}

	void ShrinkToNothingManager::TransferInventoryTask(Actor* giant, Actor* tiny) {
		ActorHandle giantHandle = giant->CreateRefHandle();
		ActorHandle tinyHandle = tiny->CreateRefHandle();
		std::string taskname = std::format("STN {}", tiny->formID);

		float currentSize = get_visual_scale(tiny);

		Runtime::PlaySound(Runtime::SNDR.GTSSoundShrinkToNothing, giant, 1.0f, 1.0f);

		TaskManager::RunOnce(taskname, [=](auto& update){
			if (!tinyHandle) {
				return;
			}
			if (!giantHandle) {
				return;
			}
			auto giant = giantHandle.get().get();
			auto tiny = tinyHandle.get().get();
			TransferInventory(tiny, giant, currentSize * GetSizeFromBoundingBox(tiny), false, true, DamageSource::Crushed, true);
			// Actor reset is done within TransferInventory
		});
		if (tiny->formID != 0x14) {
			Disintegrate(tiny); // Set critical stage 4 on actors
		} else {
			TriggerScreenBlood(50);
			tiny->SetAlpha(0.0f); // Player can't be disintegrated, so we make player Invisible
		}
	}

	ShrinkData::ShrinkData(Actor* giant) :
		state(ShrinkState::Healthy),
		delay(Timer(0.01)),
		giant(giant ? giant->CreateRefHandle() : ActorHandle()) {
	}
}
