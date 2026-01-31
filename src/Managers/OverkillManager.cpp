#include "Managers/OverkillManager.hpp"

#include "Config/Config.hpp"

#include "Managers/AI/AIFunctions.hpp"
#include "Managers/Perks/PerkHandler.hpp"
#include "Utils/Looting.hpp"

#include "Utils/DeathReport.hpp"

using namespace GTS;

namespace {
    void PlayGoreEffects(Actor* tiny, Actor* giant) {
        if (!IsLiving(tiny)) {
            SpawnDustParticle(tiny, giant, "NPC Root [Root]", 3.0f);
        } else {
            if (!Config::General.bLessGore) {
                auto root = find_node(tiny, "NPC Root [Root]");
                if (root) {
                    float currentSize = get_visual_scale(tiny);
                    SpawnParticle(tiny, 0.60f, "GTS/Damage/Explode.nif", root->world.rotate, root->world.translate, currentSize * 1.25f, 7, root);
                    SpawnParticle(tiny, 0.60f, "GTS/Damage/Explode.nif", root->world.rotate, root->world.translate, currentSize * 1.25f, 7, root);
                    SpawnParticle(tiny, 0.60f, "GTS/Damage/Crush.nif", root->world.rotate, root->world.translate, currentSize * 1.25f, 7, root);
                    SpawnParticle(tiny, 0.60f, "GTS/Damage/Crush.nif", root->world.rotate, root->world.translate, currentSize * 1.25f, 7, root);
                    SpawnParticle(tiny, 1.20f, "GTS/Damage/ShrinkOrCrush.nif", NiMatrix3(), root->world.translate, currentSize * 12.5f, 7, root);
                }
            }
            Runtime::PlayImpactEffect(tiny, Runtime::IDTS.GTSBloodSprayImpactSet, "NPC Root [Root]", RE::NiPoint3{0, 0, -1}, 512, false, true);
            Runtime::CreateExplosion(tiny, get_visual_scale(tiny) * 0.5f, Runtime::EXPL.GTSExplosionBlood);
        }
    }
}

namespace GTS {

	std::string OverkillManager::DebugName() {
		return "::OverkillManager";
	}

	void OverkillManager::Update() {
		GTS_PROFILE_SCOPE("OverkillManager: Update");
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
			if (data.state == OverkillState::Healthy) {
				SetReanimatedState(tiny);
				data.state = OverkillState::Overkilling;
			} else if (data.state == OverkillState::Overkilling) {
				if (data.delay.ShouldRun()) {
                    if (tiny->Is3DLoaded() && !tiny->IsDead()) {
						DecreaseShoutCooldown(giant);
                        KillActor(giant, tiny);
                    }

					PerkHandler::UpdatePerkValues(giant, PerkUpdate::Perk_LifeForceAbsorption);

                    ActorHandle giantHandle = giant->CreateRefHandle();
                    ActorHandle tinyHandle = tiny->CreateRefHandle();

                    PlayGoreEffects(tiny, giant);    
                    MoveItems(giantHandle, tinyHandle, tiny->formID, DamageSource::Overkill);
                    ReportDeath(giant, tiny, DamageSource::Overkill);

                    if (!tiny->IsPlayerRef()) {
                        Disintegrate(tiny); // Set critical stage 4 on actors
                    } else if (tiny->IsPlayerRef()) {
                        TriggerScreenBlood(50);
                        tiny->SetAlpha(0.0f); // Player can't be disintegrated, so we make player Invisible
                    }

                    data.state = OverkillState::Overkilled;
					tiny->Attacked(giant);
				}
			}
		}
	}


	void OverkillManager::Reset() {
		this->data.clear();
	}

	void OverkillManager::ResetActor(Actor* actor) {
		if (actor) {
			this->data.erase(actor->formID);
		}
	}

	void OverkillManager::Overkill(Actor* giant, Actor* tiny) {
		if (!tiny) {
			return;
		}
		if (!giant) {
			return;
		}
		if (OverkillManager::CanOverkill(giant, tiny)) {
			OverkillManager::GetSingleton().data.try_emplace(tiny->formID, giant);
		}
	}

	bool OverkillManager::AlreadyOverkilled(Actor* actor) {
		if (!actor) {
			return false;
		}
		auto& m = OverkillManager::GetSingleton().data;
		return !(m.find(actor->formID) == m.end());
	}

	bool OverkillManager::CanOverkill(Actor* giant, Actor* tiny) {
		if (OverkillManager::AlreadyOverkilled(tiny)) {
			return false;
		}
		if (IsEssential(giant, tiny)) {
			return false;
		}

		return true;
	}

	OverkillData::OverkillData(Actor* giant) :
		state(OverkillState::Healthy),
		delay(Timer(0.01)),
		giant(giant ? giant->CreateRefHandle() : ActorHandle()) {
	}
}
