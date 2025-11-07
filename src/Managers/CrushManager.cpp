#include "Managers/CrushManager.hpp"
#include "Managers/GtsSizeManager.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/AI/AIFunctions.hpp"
#include "Managers/Perks/PerkHandler.hpp"

#include "Managers/Audio/MoansLaughs.hpp"

#include "Magic/Effects/Common.hpp"
#include "Utils/Looting.hpp"

using namespace REL;
using namespace GTS;

namespace {

	void CrushGrow(Actor* actor, float scale_factor, float bonus) {
		// amount = scale * a + b
		float modifier = SizeManager::BalancedMode() ? 2.0f : 1.0f;
		scale_factor /= modifier;
		bonus /= modifier;
		update_target_scale(actor, CalcPower(actor, scale_factor, bonus, false), SizeEffectType::kGrow);
		AddStolenAttributes(actor, CalcPower(actor, scale_factor, bonus, false));
	}

	void ScareChance(Actor* actor) {
		int voreFearRoll = RandomInt(0, 5);
		if (HasSMT(actor)) {
			voreFearRoll = RandomInt(0, 2);
			shake_camera(actor, 0.4f, 0.25f);
		}

		if (voreFearRoll <= 0) {
			Runtime::CastSpell(actor, actor, "GTSSpellFear");
		}
	}

	void FearChance(Actor* giant)  {
		float size = get_visual_scale(giant);
		int MaxValue = (20 - static_cast<int>(1.6f * size));

		if (MaxValue <= 3 || HasSMT(giant)) {
			MaxValue = 3;
		}
		int FearChance = RandomInt(0, MaxValue);
		if (FearChance <= 0) {
			Runtime::CastSpell(giant, giant, "GTSSpellFear");
			// Should cast fear
		}
	}

	void GrowthText(Actor* actor) {
		int Random = RandomInt(0, 5);
		if (Random <= 0) {
			if (actor->formID == 0x14) {
				Notify("Crushing your foes feels good and makes you bigger");
			} else {
				Notify("Your companion grows bigger by crushing your foes");
			}
		}
	}

	void GrowAfterTheKill(Actor* caster, Actor* target, float power) { // called twice if lucky

		bool EnableCrushGrowth = Config::Gameplay.bEnableCrushGrowth;

		if (EnableCrushGrowth && !HasSMT(caster)) {

			if (Runtime::HasPerkTeam(caster, "GTSPerkGrowthDesire")) {
				float Rate = (0.00016f * get_visual_scale(target)) * 120.0f * power;
				if (Runtime::HasPerkTeam(caster, "GTSPerkAdditionalGrowth")) {
					Rate *= 2.0f;
				}
				CrushGrow(caster, 0, Rate * SizeSteal_GetPower(caster, target));
				GrowthText(caster);
			}
		}
	}

	void MoanOrLaugh(Actor* giant, Actor* target) {
		bool OnCooldown = IsActionOnCooldown(giant, CooldownSource::Emotion_Moan_Crush);
		auto randomInt = RandomInt(0, 16);
		auto select = RandomInt(0, 4);
		if (randomInt <= 4) {
			if (!OnCooldown) {
				ApplyActionCooldown(giant, CooldownSource::Emotion_Moan_Crush);
				if (select >= 1) {
					Task_FacialEmotionTask_Moan(giant, 1.6f, "CrushMoan", RandomFloat(0.0f, 0.40f));
				} else {
					Task_FacialEmotionTask_Smile(giant, 1.25f, "CrushSmile", RandomFloat(0.0f, 0.7f), RandomFloat (0.4f, 0.75f));
				}
				Sound_PlayMoans(giant, 1.0f, 0.14f, EmotionTriggerSource::Crushing);
				GrowAfterTheKill(giant, target, 2.5f);
			}
		}
	}
}

namespace GTS {
	CrushManager& CrushManager::GetSingleton() noexcept {
		static CrushManager instance;
		return instance;
	}

	std::string CrushManager::DebugName() {
		return "::CrushManager";
	}

	void CrushManager::Update() {
		GTS_PROFILE_SCOPE("CrushManager: Update");
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

			auto transient = Transient::GetActorData(tiny);
			if (transient) {
				if (!transient->CanBeCrushed && !tiny->IsDead()) {
					return;
				}
			}

			if (data.state == CrushState::Healthy) {
				SetReanimatedState(tiny);
				data.state = CrushState::Crushing;
			} else if (data.state == CrushState::Crushing) {
				Attacked(tiny, giant);

				float currentSize = get_visual_scale(tiny);

				data.state = CrushState::Crushed;
				if (giant->formID == 0x14 && IsDragon(tiny)) {
					CompleteDragonQuest(tiny, ParticleType::Red, tiny->IsDead());
				}
				
				std::string taskname = std::format("CrushTiny {}", tiny->formID);

				GrowAfterTheKill(giant, tiny, 2.0f); // Grow first time
				MoanOrLaugh(giant, tiny); // Grow second time if lucky
			
				if (giant->formID == 0x14) {
					if (IsLiving(tiny)) {
						TriggerScreenBlood(50);
					}
				}

				AddSMTDuration(giant, 5.0f);
				ScareChance(giant);
				
				const bool silent = Config::Audio.bMuteCrushDeathScreams;
				// Do crush
				KillActor(giant, tiny, silent);
				DecreaseShoutCooldown(giant);
				PerkHandler::UpdatePerkValues(giant, PerkUpdate::Perk_LifeForceAbsorption);

				if (!IsLiving(tiny) || LessGore()) {
					SpawnDustParticle(giant, tiny, "NPC Root [Root]", 3.0f);
				} else {
					if (!LessGore()) {
						auto root = find_node(tiny, "NPC Root [Root]");
						if (root) {
							SpawnParticle(tiny, 0.60f, "GTS/Damage/Explode.nif", root->world.rotate, root->world.translate, currentSize * 2.5f, 7, root);
							SpawnParticle(tiny, 0.60f, "GTS/Damage/Explode.nif", root->world.rotate, root->world.translate, currentSize * 2.5f, 7, root);
							SpawnParticle(tiny, 0.60f, "GTS/Damage/Crush.nif", root->world.rotate, root->world.translate, currentSize * 2.5f, 7, root);
							SpawnParticle(tiny, 0.60f, "GTS/Damage/Crush.nif", root->world.rotate, root->world.translate, currentSize * 2.5f, 7, root);
							SpawnParticle(tiny, 1.20f, "GTS/Damage/ShrinkOrCrush.nif", NiMatrix3(), root->world.translate, currentSize * 25, 7, root);
						}
						Runtime::CreateExplosion(tiny, get_visual_scale(tiny)/4,"GTSExplosionBlood");
						Runtime::PlayImpactEffect(tiny, "GTSBloodSprayImpactSet", "NPC Root [Root]", NiPoint3{0, 0, -1}, 512, false, false);
					}
				}
				ActorHandle giantHandle = giant->CreateRefHandle();
				ActorHandle tinyHandle = tiny->CreateRefHandle();
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
					// Actor Reset is done inside TransferInventory:StartResetTask!
				});

				if (tiny->formID != 0x14) {
					Disintegrate(tiny); // Set critical stage 4 on actors
				} else if (tiny->formID == 0x14) {
					TriggerScreenBlood(50);
					tiny->SetAlpha(0.0f); // Player can't be disintegrated, so we make player Invisible
				}

				FearChance(giant);
			}
		}
	}

	void CrushManager::Reset() {
		this->data.clear();
	}

	void CrushManager::ResetActor(Actor* actor) {
		if (actor) {
			this->data.erase(actor->formID);
		}
	}

	void CrushManager::Crush(Actor* giant, Actor* tiny) {
		if (!giant) {
			return;
		}
		if (!tiny) {
			return;
		}
		if (CrushManager::CanCrush(giant, tiny)) {
			CrushManager::GetSingleton().data.try_emplace(tiny->formID, giant);
		}
	}

	bool CrushManager::AlreadyCrushed(Actor* actor) {
		if (!actor) {
			return false;
		}
		auto& m = CrushManager::GetSingleton().data;
		return (m.find(actor->formID) != m.end());
	}

	bool CrushManager::CanCrush(Actor* giant, Actor* tiny) {
		if (CrushManager::AlreadyCrushed(tiny)) {
			//log::info("{} Is already crushed", tiny->GetDisplayFullName());
			return false;
		}
		if (IsEssential(giant, tiny)) {
			//log::info("{} Is essential", tiny->GetDisplayFullName());
			return false;
		}

		if (IsFlying(tiny)) {
			//log::info("{} Is flying", tiny->GetDisplayFullName());
			return false; // Disallow to crush flying dragons
		}

		//log::info("Can crush {}", tiny->GetDisplayFullName());
		return true;
	}

	CrushData::CrushData(Actor* giant) :
		delay(Timer(0.01)),
		state(CrushState::Healthy),
		giant(giant ? giant->CreateRefHandle() : ActorHandle()) {
	}
}
