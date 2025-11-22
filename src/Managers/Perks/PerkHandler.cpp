#include "Managers/Animation/Utils/CooldownManager.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Audio/MoansLaughs.hpp"
#include "Managers/Perks/PerkHandler.hpp"

#include "Managers/Animation/AnimationManager.hpp"
#include "Debug/DebugDraw.hpp"


using namespace GTS;

namespace {

    float Perk_Acceleration_GetBonus(Actor* giant) {
        float bonus = std::clamp(GetGtsSkillLevel(giant) - 75.0f, 0.0f, 25.0f) * 0.01f;
        return 1.05f + bonus;
    }

    float Perk_LifeAbsorption_GetBonus(Actor* giant) {
        float bonus = std::clamp(GetGtsSkillLevel(giant) - 75.0f, 0.0f, 25.0f) * 0.02f;
        return 0.30f + bonus;
    }

    void Superiority_Emotions(Actor* giant) {
        int rng = RandomInt(0, 3);
        if (rng >= 2 && !IsActionOnCooldown(giant, CooldownSource::Emotion_Laugh)) {
            ActorHandle actorHandle = giant->CreateRefHandle();
            std::string taskname = std::format("LaughCheck_{}", giant->formID);

            ApplyActionCooldown(giant, CooldownSource::Emotion_Laugh);

            TaskManager::RunOnce(taskname, [=](auto& update){
                if (!actorHandle) {
                    return;
                }

                auto giantref = actorHandle.get().get();
                if (!giantref->Is3DLoaded()) {
                    return;
                }
                Sound_PlayLaughs(giantref, 1.0f, 0.14f, EmotionTriggerSource::Superiority);
                Task_FacialEmotionTask_Smile(giantref, 1.4f, "Cataclysm", 0.25f);
            });
        }
    }

    void Perk_Catalysmic_CheckForLaugh(Actor* giant) {
        const float giantScale = get_visual_scale(giant);
        float maxFootDistance = 58.0f * giantScale;

		NiAVObject* node = find_node(giant, "NPC Root [Root]");
        if (node) {
            NiPoint3 point = node->world.translate;
			if (IsDebugEnabled() && (giant->formID == 0x14 || IsTeammate(giant) || EffectsForEveryone(giant))) {
				DebugDraw::DrawSphere(glm::vec3(point.x, point.y, point.z), maxFootDistance, 600, {0.0f, 0.0f, 1.0f, 1.0f});
			}

			NiPoint3 giantLocation = giant->GetPosition();

			for (auto otherActor: find_actors()) {
				if (otherActor != giant && !otherActor->IsDead()) {
					float tinyScale = get_visual_scale(otherActor);
					if (giantScale / tinyScale > 4.0f) {
						NiPoint3 actorLocation = otherActor->GetPosition();
                        float distance = (point - actorLocation).Length();
                        if (distance <= maxFootDistance) {
							Superiority_Emotions(giant);
						}
					}
				}
			}
		}
    }

    void ManageSpellPerks(const AddPerkEvent& evt) {
        if (evt.actor->formID == 0x14) {
            if (evt.perk == Runtime::GetPerk(Runtime::PERK.GTSPerkGrowthDesireAug)) {
                PrintMessageBox("You're now able to grow and shrink yourself manually at will. By default, press L.Shift + 1 or 2. You can affect followers by pressing L.Shift + Left Arrow + Arrow Up, and can also affect self by pressing Left Arrow + Arrow Up");
            }
            if (evt.perk == Runtime::GetPerk(Runtime::PERK.GTSPerkShrinkAdept) && !Runtime::HasSpell(evt.actor, Runtime::SPEL.GTSSpellShrinkBolt)) {
                Runtime::AddSpell(evt.actor, Runtime::SPEL.GTSSpellShrinkBolt);
            }
            if (evt.perk == Runtime::GetPerk(Runtime::PERK.GTSPerkShrinkExpert) && !Runtime::HasSpell(evt.actor, Runtime::SPEL.GTSSpellShrinkStorm)) {
                Runtime::AddSpell(evt.actor, Runtime::SPEL.GTSSpellShrinkStorm);
            }
            if (evt.perk == Runtime::GetPerk(Runtime::PERK.GTSPerkTinyCalamity)) {
                AddCalamityPerk();
            }
        }
    }

    void ManageOtherPerks(const AddPerkEvent& evt) {
        Actor* actor = evt.actor;
        if (actor) {
            if (actor->formID == 0x14 || IsTeammate(actor)) {
                auto data = Transient::GetActorData(actor);
                if (data) {
                    if (evt.perk == Runtime::GetPerk(Runtime::PERK.GTSPerkAcceleration)) {
                        data->PerkBonusSpeed = Perk_Acceleration_GetBonus(actor);
                    }
                }
            }
        }
    }

    void Perks_UpdateAccelerationPerk(TransientActorData* data, Actor* giant) {
        if (data && Runtime::HasPerkTeam(giant, Runtime::PERK.GTSPerkAcceleration)) {
            data->PerkBonusSpeed = Perk_Acceleration_GetBonus(giant);
        }
    }

    void StartStackDecayTask(Actor* giant, float stack_power, TransientActorData* data) {
        std::string name = std::format("StackDecay_{}_{}", giant->formID, Time::WorldTimeElapsed());
        ActorHandle gianthandle = giant->CreateRefHandle();
        double Start = Time::WorldTimeElapsed();

        double stack_duration = 300.0;
        if (data) {
            if (data->PerkLifeForceStolen < 0.0f) {
                data->PerkLifeForceStolen = 0.0f;
            }
            if (data->Stacks_Perk_LifeForce < 0) {
                data->Stacks_Perk_LifeForce = 0;
            }

            data->Stacks_Perk_LifeForce += 1;
            data->PerkLifeForceStolen += stack_power;

            //log::info("Life force stolen: {}, added power: {}", data->Perk_lifeForceStolen, stack_power);

            TaskManager::Run(name, [=](auto& progressData) {
                if (!gianthandle) {
                    return false;
                }
                Actor* giantref = gianthandle.get().get();
                double Finish = Time::WorldTimeElapsed();

                if (Finish - Start >= stack_duration) {
                    if (data) {
                        if (data->Stacks_Perk_LifeForce > 0) {
                            data->Stacks_Perk_LifeForce -= 1;
                            data->PerkLifeForceStolen -= stack_power;
                        } else if (data->Stacks_Perk_LifeForce == 0) {
                            return false;
                        }
                    }
                    return false;
                }
                return true;
            });
        }
    }


    void Perks_UpdateLifeForceAbsorptionPerk(TransientActorData* data, Actor* giant) {
        if (giant) {
            if (Runtime::HasPerkTeam(giant, Runtime::PERK.GTSPerkLifeAbsorption)) {
                int stack_limit = 25;
                if (data) {
                    if (data->Stacks_Perk_LifeForce < stack_limit) {
                        StartStackDecayTask(giant, Perk_LifeAbsorption_GetBonus(giant), data);
                    } else {
                        data->Stacks_Perk_LifeForce -= 1;
                        data->PerkLifeForceStolen -= Perk_LifeAbsorption_GetBonus(giant);
                        // Just Refresh it
                        StartStackDecayTask(giant, Perk_LifeAbsorption_GetBonus(giant), data);
                    }
                }
            }
        }
    }
}

namespace GTS {

    std::string PerkHandler::DebugName() {
        return "::PerkHandler";
    }

    void PerkHandler::OnAddPerk(const AddPerkEvent& evt) {
        ManageSpellPerks(evt);
        ManageOtherPerks(evt);
    }

    void PerkHandler::OnRemovePerk(const RemovePerkEvent& evt) {
        Actor* actor = evt.actor;
        if (actor) {
            if (actor->formID == 0x14 || IsTeammate(actor)) {
                auto data = Transient::GetActorData(actor);
                if (data) {
                    if (evt.perk == Runtime::GetPerk(Runtime::PERK.GTSPerkAcceleration)) {
                        data->PerkBonusSpeed = 1.0f;
                    }
                    if (evt.perk == Runtime::GetPerk(Runtime::PERK.GTSPerkLifeAbsorption)) {
                        data->PerkLifeForceStolen = 0.0f;
                        data->Stacks_Perk_LifeForce = 0;
                    }
                }
            }
        }
    }
    bool PerkHandler::Perks_Cataclysmic_HasStacks(Actor* giant) {
        auto transient = Transient::GetActorData(giant);
		if (transient) {
            bool HasPerk = Runtime::HasPerkTeam(giant, Runtime::PERK.GTSPerkCataclysmicStomp);
            return HasPerk && transient->Stacks_Perk_CataclysmicStomp > 0;
        }
        return false;
    }

    void PerkHandler::Perks_Cataclysmic_ManageStacks(Actor* giant, PerkAction action) {
		auto transient = Transient::GetActorData(giant);
		if (transient) {
            bool HasPerk = Runtime::HasPerkTeam(giant, Runtime::PERK.GTSPerkCataclysmicStomp);
            int current_stacks = transient->Stacks_Perk_CataclysmicStomp;

            if (action == PerkAction::Increase && current_stacks < 3) { // Increase stack
				transient->Stacks_Perk_CataclysmicStomp += 1;
			} else if (current_stacks > 0) { // Consuming one stack
                transient->Stacks_Perk_CataclysmicStomp -= 1;
                Perk_Catalysmic_CheckForLaugh(giant);
            }
		}
    }

    float PerkHandler::Perks_Cataclysmic_EmpowerStomp(Actor* giant) {
        constexpr float per_legendary_boost = 0.75f;
        constexpr float per_level_boost_mult = 0.5f;
        float Effect_Increase = 1.0f;
        if (PerkHandler::Perks_Cataclysmic_HasStacks(giant)) {
            float GTSLevel = GetGtsSkillLevel(giant) * 0.01f;
            float SkillLevel = std::clamp(GTSLevel - 0.6f, 0.0f, 0.4f) * per_level_boost_mult;
            float Legendary = std::clamp(GetLegendaryLevel(giant), 0.0f, 2.0f) * per_legendary_boost;
            
            float PowerBoost = 1.0f + ((0.15f + SkillLevel) * (Legendary + 1.0f));

            log::info("Skill Level: {}, Legendary: {}", SkillLevel, Legendary);
            log::info("Power Boost: {}", PowerBoost);

            Effect_Increase *= PowerBoost;

            PerkHandler::Perks_Cataclysmic_ManageStacks(giant, PerkAction::Decrease);
        }
        return Effect_Increase;
    }

    void PerkHandler::Perks_Cataclysmic_BuffStompSpeed(AnimationEventData& data, bool reset) {
		if (!reset && PerkHandler::Perks_Cataclysmic_HasStacks(&data.giant)) {
            float Legendary = std::clamp(GetLegendaryLevel(&data.giant), 0.0f, 2.0f);
			data.stage = 1;
			data.canEditAnimSpeed = true;
			data.animSpeed *= 1.265f + (0.265f * Legendary);
		} else {
			data.stage = 0;
            data.canEditAnimSpeed = false;
            data.animSpeed = 1.0f;
		}
	}

    void PerkHandler::UpdatePerkValues(Actor* giant, PerkUpdate Type) {
        if (giant) {
            auto data = Transient::GetActorData(giant);
            switch (Type) {
                case PerkUpdate::Perk_Acceleration: 
                    Perks_UpdateAccelerationPerk(data, giant);
                break;
                case PerkUpdate::Perk_LifeForceAbsorption:
                    Perks_UpdateLifeForceAbsorptionPerk(data, giant); 
                    // ^ Isn't called with 'KillActor' since it can be funny with Essential actors and grant more than 1 stack at once
                break;
                case PerkUpdate::Perk_None:
                break;
            }
        }
    }
}