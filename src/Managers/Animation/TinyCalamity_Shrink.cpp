#include "Managers/Animation/TinyCalamity_Shrink.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Rumble.hpp"

using namespace GTS;

// Spawns Rune on the hand and then we shrink a tiny that we've found.
// AnimObjectR

namespace {
	void slow_down(Actor* tiny, float value) {
		auto tranData = Transient::GetActorData(tiny);
		if (tranData) {
			tranData->MovementSlowdown -= value;
			//log::info("Slowdown of {} is {}", tiny->GetDisplayFullName(), tranData->MovementSlowdown);
		}
	}

	void SpawnRuneOnTiny(Actor* tiny) {
		auto node = find_node(tiny, "NPC Root [Root]");
		if (node) {
			SpawnParticle(tiny, 3.00f, "GTS/gts_tinyrune.nif", NiMatrix3(), node->world.translate, 1.0f, 7, node); 
		}
	}

    void AttachRune(Actor* giant, bool ShrinkRune, float speed, float scale) { // A task that scales/shrinks the runes
		string node_name = "ShrinkRune-Obj";

		double Start = Time::WorldTimeElapsed();
        std::string name = std::format("Calamity_{}_{}", giant->formID, ShrinkRune);
        ActorHandle gianthandle = giant->CreateRefHandle();

		if (ShrinkRune) {
			TaskManager::Run(name, [=](auto& progressData) {
				if (!gianthandle) {
					return false;
				}
				auto giantref = gianthandle.get().get();
				double Finish = Time::WorldTimeElapsed();
				auto node = find_node(giantref, node_name, false);
				double timepassed = std::clamp(((Finish - Start) * AnimationManager::GetAnimSpeed(giantref)) * speed, 0.01, 0.98);
				if (node) {
					node->local.scale = static_cast<float>(std::clamp(0.60 - timepassed, 0.01, 1.0));
					update_node(node);
				}
				if (timepassed >= 0.98) {
					return false; // end it
				}
				return true;
			});
		} else {
			TaskManager::Run(name, [=](auto& progressData) {
				if (!gianthandle) {
					return false;
				}
				auto giantref = gianthandle.get().get();
				double Finish = Time::WorldTimeElapsed();
				auto node = find_node(giantref, node_name, false);
				double timepassed = std::clamp(((Finish - Start) * GetAnimationSlowdown(giantref)) * speed, 0.01, 9999.0);
				if (node) {
					node->local.scale = static_cast<float>(std::clamp(timepassed, 0.01, 1.0));
                    node->local.scale *= scale;
					update_node(node);
				}
				if (timepassed >= 1.0) {
					return false; // end it
				}
				return true;
			});
		}
	}

    void GTS_TC_RuneStart(AnimationEventData& data) {
        auto node = find_node(&data.giant, "ShrinkRune-Obj", false);
        if (node) {
            node->local.scale = 0.01f;
            update_node(node);
        }
        AttachRune(&data.giant, false, 0.6f, 0.70f);
    }

	void GTS_TC_ReadySound(AnimationEventData& data) {
		auto giant = &data.giant;
		std::string name = std::format("Calamity_{}_{}", giant->formID, false);
		TaskManager::Cancel(name);

		Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundTinyCalamity_RuneReady, giant, 1.0f, "NPC R Hand [RHnd]");

		auto node = find_node(&data.giant, "ShrinkRune-Obj", false);
        if (node) {
            node->local.scale = 0.60f;
            update_node(node);
        }
		if (giant->formID == 0x14) {
			shake_camera(giant, 0.80f, 0.35f);
		} else {
			Rumbling::Once("Calamity_R", giant, 4.25f, 0.15f, "NPC R Hand [RHnd]", 0.0f);
		}
	}

    void GTS_TC_ShrinkStart(AnimationEventData& data) {
        auto victims = Animation_TinyCalamity::GetShrinkActors(&data.giant);
		for (auto victim: victims) {
			if (victims.size() > 0 && victim) {
				SpawnRuneOnTiny(victim);
				float until = Animation_TinyCalamity::GetShrinkUntil(victim);
				ShrinkUntil(&data.giant, victim, until, 0.26f, false);
				StartCombat(victim, &data.giant);

				ChanceToScare(&data.giant, victim, 6.0f, 6, false); // chance to force actor to flee 

				slow_down(victim, 0.60f); // decrease MS by 60%

				Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundTinyCalamity_SpawnRune, victim, 1.0f, "NPC Root [Root]");
				Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundTinyCalamity_Absorb, victim, 1.0f, "NPC Root [Root]");
			}
		}
	}
    
    void GTS_TC_RuneEnd(AnimationEventData& data) {
		auto victims = Animation_TinyCalamity::GetShrinkActors(&data.giant);
		for (auto victim: victims) {
			if (victims.size() > 0 && victim) {
				slow_down(victim, -0.60f); // restore normal MS
			}
		}
        AttachRune(&data.giant, true, 1.4f, 0.60f);
    }

	void GTS_TC_ShrinkStop(AnimationEventData& data) {
		Animation_TinyCalamity::GetSingleton().ResetActors(&data.giant);
    }
}

namespace GTS
{
    Animation_TinyCalamity& Animation_TinyCalamity::GetSingleton() noexcept {
		static Animation_TinyCalamity instance;
		return instance;
	}

	std::string Animation_TinyCalamity::DebugName() {
		return "::Animation_TinyCalamity";
	}

	void Animation_TinyCalamity::RegisterEvents() {
		AnimationManager::RegisterEvent("GTS_TC_RuneStart", "Calamity", GTS_TC_RuneStart); 
		AnimationManager::RegisterEvent("GTS_TC_ReadySound", "Calamity", GTS_TC_ReadySound);
        AnimationManager::RegisterEvent("GTS_TC_ShrinkStart", "Calamity", GTS_TC_ShrinkStart);
        AnimationManager::RegisterEvent("GTS_TC_ShrinkStop", "Calamity", GTS_TC_ShrinkStop);
        AnimationManager::RegisterEvent("GTS_TC_RuneEnd", "Calamity", GTS_TC_RuneEnd);
	}

	void Animation_TinyCalamity::RegisterTriggers() {
		AnimationManager::RegisterTrigger("Calamity_ShrinkOther", "Calamity", "GTSBEH_TC_Shrink");
	}


	void Animation_TinyCalamity::ResetActors(Actor* actor) {
		auto tranData = Transient::GetActorData(actor);
		if (tranData) {
			tranData->shrinkies = {}; // Reset array of actors to shrink
		}
	}

    void Animation_TinyCalamity::AddToData(Actor* giant, Actor* tiny, float until) {
        auto tranData_gts = Transient::GetActorData(giant);
		auto tranData_tiny = Transient::GetActorData(tiny);

		if (tranData_gts) {
			tranData_gts->shrinkies.push_back(tiny);
		}
		if (tranData_tiny) {
			tranData_tiny->ShrinkUntil = until;
		}
    }


    std::vector<Actor*> Animation_TinyCalamity::GetShrinkActors(Actor* giant) {
		 auto tranData_gts = Transient::GetActorData(giant);
		 if (tranData_gts) {
			return tranData_gts->shrinkies;
		 }
		 return {};
	}

    float Animation_TinyCalamity::GetShrinkUntil(Actor* tiny) {
        auto tranData_tiny = Transient::GetActorData(tiny);
		if (tranData_tiny) {
			return tranData_tiny->ShrinkUntil;
		}
		return 1.0f;
    }
}