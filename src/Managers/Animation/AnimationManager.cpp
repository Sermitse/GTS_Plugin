#include "Managers/Animation/TinyCalamity_Instakill.hpp"
#include "Managers/Animation/Sneak_Slam_FingerGrind.hpp"
#include "Managers/Animation/Stomp_Under_FullBody.hpp"
#include "Managers/Animation/TinyCalamity_Shrink.hpp"
#include "Managers/Animation/Stomp_Under_Strong.hpp"
#include "Managers/Animation/Sneak_Slam_Strong.hpp"
#include "Managers/Animation/Stomp_Under_Slam.hpp"
#include "Managers/Animation/Stomp_Under_Butt.hpp"
#include "Managers/Animation/AnimationManager.hpp"

#include "Config/Config.hpp"

#include "Managers/Animation/CleavageStrangle.hpp"
#include "Managers/Animation/Grab_Sneak_Vore.hpp"
#include "Managers/Animation/Sneak_KneeCrush.hpp"
#include "Managers/Animation/CleavageEvents.hpp"
#include "Managers/Animation/CleavageState.hpp"
#include "Managers/Animation/Vore_Standing.hpp"
#include "Managers/Animation/ThighSandwich.hpp"
#include "Managers/Animation/Sneak_Swipes.hpp"
#include "Managers/Animation/RandomGrowth.hpp"
#include "Managers/Animation/Stomp_Normal.hpp"
#include "Managers/Animation/Stomp_Strong.hpp"
#include "Managers/Animation/Stomp_Under.hpp"
#include "Managers/Animation/FootTrample.hpp"
#include "Managers/Animation/Grab_Attack.hpp"

#include "Managers/Animation/Grab_Throw.hpp"
#include "Managers/Animation/Grab_Play_Events.hpp"
#include "Managers/Animation/Grab_Play.hpp"
#include "Managers/Animation/Sneak_Slam.hpp"
#include "Managers/Animation/Vore_Sneak.hpp"
#include "Managers/Animation/Vore_Crawl.hpp"
#include "Managers/Animation/ThighCrush.hpp"
#include "Managers/Animation/Grab_Vore.hpp"
#include "Managers/Animation/ButtCrush.hpp"
#include "Managers/Animation/BoobCrush.hpp"
#include "Managers/Animation/FootGrind.hpp"
#include "Managers/Animation/HugShrink.hpp"
#include "Managers/Animation/Crawling.hpp"
#include "Managers/Animation/HugHeal.hpp"
#include "Managers/Animation/Proning.hpp"
#include "Managers/Animation/Compat.hpp"
#include "Managers/Animation/Growth.hpp"
#include "Managers/Animation/Shrink.hpp"
#include "Managers/Animation/Kicks.hpp"
#include "Managers/Animation/Grab.hpp"


#include "Managers/Perks/PerkHandler.hpp"

#include "Utils/InputFunctions.hpp"


using namespace GTS;

namespace GTS {

	AnimationEventData::AnimationEventData(Actor& giant, TESObjectREFR* tiny) : giant(giant), tiny(tiny) {}

	AnimationEvent::AnimationEvent(const std::function<void(AnimationEventData&)>& a_callback, const std::string& a_group) : callback(a_callback), group(a_group) {}

	TriggerData::TriggerData(const std::vector< std::string_view>& behavors,  std::string_view group) : behavors({}), group(group) {
		for (auto& sv: behavors) {
			this->behavors.emplace_back(sv);
		}
	}

	AnimationManager& AnimationManager::GetSingleton() noexcept {
		static AnimationManager instance;
		return instance;
	}

	std::string AnimationManager::DebugName() {
		return "::AnimationManager";
	}

	void AnimationManager::DataReady() {
		AnimationStomp::RegisterEvents();
		AnimationStomp::RegisterTriggers();

		AnimationStrongStomp::RegisterEvents();
		AnimationStrongStomp::RegisterTriggers();

		AnimationUnderStomp::RegisterEvents();
		AnimationUnderStomp::RegisterTriggers();

		AnimationUnderStompStrong::RegisterEvents();
		AnimationUnderStompStrong::RegisterTriggers();

		AnimationUnderStompFullBody::RegisterEvents();
		AnimationUnderStompSlam::RegisterEvents();
		AnimationUnderStompButt::RegisterEvents();

		AnimationThighSandwich::RegisterEvents();
		AnimationThighSandwich::RegisterTriggers();

		AnimationThighCrush::RegisterEvents();
		AnimationThighCrush::RegisterTriggers();

		AnimationCrawling::RegisterEvents();
		AnimationCrawling::RegisterTriggers();

		Animation_VoreStanding::RegisterEvents();
		Animation_VoreStanding::RegisterTriggers();

		Animation_VoreCrawl::RegisterEvents();
		Animation_VoreSneak::RegisterEvents();

		Animation_SneakSwipes::RegisterEvents();
		Animation_SneakSlam::RegisterEvents();
		Animation_SneakSlam_Strong::RegisterEvents();
		Animation_SneakSlam_FingerGrind::RegisterEvents();
		Animation_SneakSlam_FingerGrind::RegisterTriggers();

		Animation_Cleavage::RegisterEvents();
		Animation_Cleavage::RegisterTriggers();

		Animation_CleavageEvents::RegisterEvents();

		Animation_CleavageStrangle::RegisterEvents();
		Animation_CleavageStrangle::RegisterTriggers();

		Animation_TinyCalamity::RegisterEvents();
		Animation_TinyCalamity::RegisterTriggers();

		Animation_TinyCalamity_InstaKill::RegisterEvents();
		Animation_TinyCalamity_InstaKill::RegisterTriggers();

		AnimationButtCrush::RegisterEvents();
		AnimationButtCrush::RegisterTriggers();

		AnimationSneakCrush::RegisterEvents();

		AnimationBoobCrush::RegisterEvents();

		Animation_GrabSneak_Vore::RegisterEvents();
		Animation_GrabVore::RegisterEvents();
		Animation_GrabThrow::RegisterEvents();
		Animation_GrabAttack::RegisterEvents();
		Animation_GrabPlay_Events::RegisterEvents();
		Animation_GrabPlay::RegisterTriggers();
		
		Animation_RandomGrowth::RegisterEvents();
		Animation_RandomGrowth::RegisterTriggers();

		AnimationFootGrind::RegisterEvents();
		AnimationFootGrind::RegisterTriggers();

		AnimationFootTrample::RegisterEvents();
		AnimationFootTrample::RegisterTriggers();

		AnimationCompat::RegisterEvents();
		AnimationCompat::RegisterTriggers();

		AnimationProning::RegisterEvents();
		AnimationProning::RegisterTriggers();

		AnimationGrowth::RegisterEvents();
		AnimationGrowth::RegisterTriggers();

		AnimationShrink::RegisterEvents();
		AnimationShrink::RegisterTriggers();

		InputFunctions::RegisterEvents();

		AnimationKicks::RegisterEvents();
		AnimationKicks::RegisterTriggers();

		HugShrink::RegisterEvents();
		HugShrink::RegisterTriggers();

		HugHeal::RegisterEvents();
		HugHeal::RegisterTriggers();

		Grab::RegisterEvents();
		Grab::RegisterTriggers();
	}

	void AnimationManager::Update() {
		auto player = PlayerCharacter::GetSingleton();
		if (player) {
			// Updates fall Gravity of Player
			auto charCont = player->GetCharController();
			UpdateGravity(player);
			/*if (charCont) {
				float Velocity = (charCont->outVelocity.quad.m128_f32[2] * 100.0f) / get_giantess_scale(player);

				player->SetGraphVariableFloat("GiantessVelocity", Velocity);
			}*/
		}
	}

	void AnimationManager::Reset() {
		this->data.clear();
	}

	void AnimationManager::ResetActor(Actor* actor) {
		this->data.erase(actor);
	}

	float AnimationManager::GetHighHeelSpeed(Actor* actor) {
		float Speed = 1.0f;
		auto& AnimMgr = AnimationManager::GetSingleton();

		try {

			if (!AnimMgr.data.empty()) {

				if (AnimMgr.data.contains(actor)) {

					for (auto& data : AnimMgr.data.at(actor) | views::values) {
						Speed *= data.HHspeed;
					}
				}
			}

		}
		catch (const std::out_of_range&) {}

		return Speed;
	}

	float AnimationManager::GetBonusAnimationSpeed(Actor* actor) {
		float totalSpeed = 1.0f;
		auto& AnimMgr = AnimationManager::GetSingleton();

		try {

			if (!AnimMgr.data.empty()) {

				if (AnimMgr.data.contains(actor)) {

					for (auto& data : AnimMgr.data.at(actor) | views::values) {
						totalSpeed *= data.animSpeed;
					}
				}
			}
		}
		catch (const std::out_of_range&) {}
		return totalSpeed;
	}

	void AnimationManager::AdjustAnimSpeed(float bonus) {

		const auto player = PlayerCharacter::GetSingleton();
		auto& AnimMgr = AnimationManager::GetSingleton();

		try {

			if (!AnimMgr.data.empty()) {

				if (AnimMgr.data.contains(player)) {

					for (auto& data : AnimMgr.data.at(player) | views::values) {

						if (data.canEditAnimSpeed) {
							data.animSpeed += (bonus * GetAnimationSlowdown(player));
						}

						float min = IsStrangling(player) ? 0.50f : 0.33f;
						float max = IsStrangling(player) ? 1.75f : 3.0f;
						data.animSpeed = std::clamp(data.animSpeed, min, max);
					}
				}
			}
		}

		catch (std::out_of_range&) {}
	}

	float AnimationManager::GetAnimSpeed(Actor* actor) {

		float speed = 1.0f;

		if (!Config::GetGeneral().bDynamicAnimspeed) {
			return 1.0f;
		}

		if (actor) {

			auto saved_data = GTS::Persistent::GetSingleton().GetData(actor);
			if (saved_data) {
				if (saved_data->anim_speed > 0.0f) {
					speed *= saved_data->anim_speed;
				}
			}

			const auto& AnimMgr = AnimationManager::GetSingleton();
			auto& AnimData = AnimMgr.data;
			try {

				float totalSpeed = 1.0f;

				if (auto it = AnimData.find(actor); it != AnimData.end()) {
					for (const auto& data : it->second | std::views::values) {
						totalSpeed *= data.animSpeed;
					}
					speed *= totalSpeed;
				}

			}
			catch (const std::out_of_range&) {}
		}
		return speed;
	}

	void AnimationManager::RegisterEvent( std::string_view name,  std::string_view group, std::function<void(AnimationEventData&)> func) {
		AnimationManager::GetSingleton().eventCallbacks.try_emplace(std::string(name), func, std::string(group));
		//log::info("Registering Event: Name {}, Group {}", name, group);
	}

	void AnimationManager::RegisterTrigger( std::string_view trigger,  std::string_view group,  std::string_view behavior) {
		AnimationManager::RegisterTriggerWithStages(trigger, group, {behavior});
		//log::info("Registering Trigger: {}, Group {}, Behavior {}", trigger, group, behavior);
	}

	void AnimationManager::RegisterTriggerWithStages( std::string_view trigger,  std::string_view group,  std::vector< std::string_view> behaviors) {
		if (!behaviors.empty()) {
			AnimationManager::GetSingleton().triggers.try_emplace(std::string(trigger), behaviors, group);
			//log::info("Registering Trigger With Stages: {}, Group {}", trigger, group);
		}
	}


	void AnimationManager::StartAnim( std::string_view trigger, Actor& giant) {
		AnimationManager::StartAnim(trigger, giant, nullptr);
	}

	void AnimationManager::StartAnim( std::string_view trigger, Actor* giant) {
		if (giant) {
			AnimationManager::StartAnim(trigger, *giant);
			//log::info("Starting Trigger {} for {}", trigger, giant->GetDisplayFullName());
		}
	}

	void AnimationManager::StartAnim(std::string_view trigger, Actor& giant, TESObjectREFR* tiny) {

		if (IsTransitioning(&giant)) {
			return;
		}

		if (giant.formID == 0x14) {
			if (IsFirstPerson() || Plugin::IsInRaceMenu()) { 
				//Time::WorldTimeElapsed() > 1.0
				//ForceThirdPerson(&giant);
				// It kinda works in fp that way, but it introduces some issues with animations such as Hugs and Butt Crush.
				// Better to wait for full support someday
				return; // Don't start animations in FP, it's not supported.
			}
		}

		try {
			auto& me = AnimationManager::GetSingleton();
			// Find the behavior for this trigger exit on catch if not

			auto& behavorToPlay = me.triggers.at(std::string(trigger));
			auto& group = behavorToPlay.group;
			// Try to create anim data for actor
			me.data.try_emplace(&giant);
			auto& actorData = me.data.at(&giant); // Must exists now
			// Create the anim data for this group if not present
			actorData.try_emplace(group, giant, tiny);
			// Run the anim
			//log::info("Playing Trigger {} for {}", trigger, giant.GetDisplayFullName());
			//log::info("Playing {}", behavorToPlay.behavors[0]);
			giant.NotifyAnimationGraph(behavorToPlay.behavors[0]);

			PerkHandler::UpdatePerkValues(&giant, PerkUpdate::Perk_Acceleration); // Currently used for Anim Speed buff only
		}
		catch (const std::out_of_range&) {
			log::error("Requested play of unknown animation named: {}", trigger);
			return;
		}
	}

	void AnimationManager::ResetAnimationSpeedData(Actor* actor) {
		try {

			auto& me = AnimationManager::GetSingleton();

			if (!me.data.empty()) {

				if (me.data.contains(actor)) {

					for (auto& data : me.data.at(actor) | views::values) {
						data.animSpeed = 1.0f;
						data.canEditAnimSpeed = false;
						data.stage = 0;
					}
				}
			}
		}
		catch (std::out_of_range&) {}
	}



	void AnimationManager::StartAnim(std::string_view trigger, Actor* giant, TESObjectREFR* tiny) {
		if (giant) {
			AnimationManager::StartAnim(trigger, *giant, tiny);
		}
	}

	void AnimationManager::NextAnim(std::string_view trigger, Actor& giant) {
		try {
			auto& me = AnimationManager::GetSingleton();
			// Find the behavior for this trigger exit on catch if not
			auto& behavorToPlay = me.triggers.at(std::string(trigger));
			auto& group = behavorToPlay.group;
			// Get the actor data OR exit on catch
			auto& actorData = me.data.at(&giant);
			// Get the event data of exit on catch
			auto& eventData = actorData.at(group);
			std::size_t currentTrigger = eventData.currentTrigger;
			// Run the anim
			if (behavorToPlay.behavors.size() < currentTrigger) {
				giant.NotifyAnimationGraph(behavorToPlay.behavors[currentTrigger]);
			}
		}
		catch (const std::out_of_range&) {}
	}
	void AnimationManager::NextAnim(std::string_view trigger, Actor* giant) {
		if (giant) {
			AnimationManager::NextAnim(trigger, *giant);
		}
	}

	void AnimationManager::ActorAnimEvent(Actor* actor, const std::string_view& tag, const std::string_view& payload) {
		try {
			if (actor) {

				if (!this->eventCallbacks.contains(std::string(tag))){
					return;
				}

				// Try to get the registerd anim for this tag
				auto& animToPlay = this->eventCallbacks.at(std::string(tag));

				// If data doesn't exist then insert with default
				this->data.try_emplace(actor);
				auto& actorData = this->data.at(actor);
				auto group = animToPlay.group;
				// If data doesn't exist this will insert it with default
				actorData.try_emplace(group, *actor, nullptr);
				// Get the data or the newly inserted data
				auto& actdata = actorData.at(group);
				// Call the anims function
				animToPlay.callback(actdata);
				// If the stage is 0 after an anim has been played then
				//   delete this data so that we can reset for the next anim
				if (actdata.stage == 0) {
					actorData.erase(group);
				}
			}
		}
		catch (const std::out_of_range&) {}
	}

	// Get the current stage of an animation group
	std::size_t AnimationManager::GetStage(Actor& actor,  std::string_view group) {
		try {
			auto& me = AnimationManager::GetSingleton();

			if (me.data.empty()) {
				return 0;
			}

			if (!me.data.contains(&actor)) {
				return 0;
			}

			return me.data.at(&actor).at(std::string(group)).stage;

		}
		catch (const std::out_of_range&) {
			return 0;
		}
	}

	std::size_t AnimationManager::GetStage(Actor* actor,  std::string_view group) {
		if (actor) {
			return AnimationManager::GetStage(*actor, group);
		}
		else {
			return 0;
		}
	}

	bool AnimationManager::HHDisabled(Actor& actor) {
		auto& me = AnimationManager::GetSingleton();

		if (!IsHumanoid(&actor) || me.data.empty()) {
			return false;
		}

		auto it = me.data.find(&actor);
		if (it == me.data.end()) {
			return false;
		}

		return std::ranges::any_of(it->second | std::views::values, [](const auto& data) {
			return data.disableHH;
		});
	}

	bool AnimationManager::HHDisabled(Actor* actor) {
		if (actor) {
			return AnimationManager::HHDisabled(*actor);
		}
		else {
			return false;
		}
	}

	void AnimationManager::UpdateGravity(Actor* actor) {
		static Timer GravityTimer = Timer(0.33);
		if (GravityTimer.ShouldRunFrame()) {
			auto Controller = actor->GetCharController();
			if (Controller) {
				bool Enabled = Config::GetGeneral().bAlterPlayerGravity;
				float size = get_visual_scale(actor);

				float new_gravity = Enabled ? 1.0f * std::sqrt(size) : 1.0f; 
				Controller->gravity = new_gravity;
			}
		}
	}
}
