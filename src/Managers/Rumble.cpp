// Module that handles rumbling
#include "Managers/Rumble.hpp"

#include "Config/Config.hpp"

#include "Managers/Animation/AnimationManager.hpp"

namespace {
	using namespace GTS;
	// Sermit's Note:
	// In perfect scenario, this .cpp/hpp should be rewritten into using struct like
	// RumbleVariation {
	// 		rumble_intensity; 
	//		rumble_halflife; 
	//		rumble_duration; 
	//		rumble_min_distance; 
	//		rumble_max_distance; 
	//		ignore_scaling; 
	// };
	// But it's too much work since we have a lot of functions that call Rumbling::once/for

	constexpr float global_shake_multiplier = 0.125f; // Reduce power of all shakes
	constexpr float falloff_power = 2.5f;
	constexpr float scale_bonus = 0.1f;

	constexpr float cam_close_dist = 48.0f;
	constexpr float cam_far_dist = 300.0f;

	void ApplyPlayerSourceOverrides(Actor* caster, float& distance, NiPoint3 coords, float& tremor_power, float& sourceSize, float& sizeDifference) {
		if (caster->IsPlayerRef()) {
			const bool isFirstPerson = IsFirstPerson() || HasFirstPersonBody();
			tremor_power = isFirstPerson ? Config::Camera.fCameraShakePlayerFP : Config::Camera.fCameraShakePlayer;
			distance = get_distance_to_camera(coords);
			sizeDifference = sourceSize;

			if (TinyCalamityActive(caster)) {
				sourceSize += 0.8f;
			}
		} 
	}
	void ApplyNPCSourceOverrides(Actor* caster, float& sourceSize) {
		if (!caster->IsPlayerRef()) {
			if (TinyCalamityActive(caster)) {
				sourceSize += 0.8f;
			}
		}
	}
	// intensity here is purely a distance-falloff multiplier (0..1) - NOT the
	// final shake strength. The source's actual strength comes in separately as `power` 
	void OverrideStartingIntensity(Actor* caster, float sourceSize, float distance, float power, float& intensity) {
		if (caster) {
			const float PC_Config 				= 			Config::Camera.fCameraShakeDistanceMultPlayer;
			const float NPC_Config 				= 			Config::Camera.fCameraShakeDistanceMultNPC;
			const float cameraConf 				= 			caster->IsPlayerRef() ? PC_Config : NPC_Config; 
			const float adjustment 				= 			power * sourceSize * cameraConf;
			const float full_shake_distance 	= 			cam_close_dist * sourceSize;
			const float max_shake_distance 		=  			full_shake_distance + cam_far_dist * adjustment;

			if (distance <= full_shake_distance) {
				intensity = power < 1.0f ? std::clamp(power, 0.0f, 1.0f) : 1.0f;
				//logger::info("Full shake");
			} else { // Outside full shake radius = smooth falloff
				float t = std::clamp((distance - full_shake_distance) / (max_shake_distance - full_shake_distance), 0.0f, 1.0f);
				intensity = pow(1.0f - t, falloff_power);
				//logger::info("T: {}", t);
			}

			//logger::info("Full Dist: {}, Max Shake Dist: {}", full_shake_distance, max_shake_distance);
			//logger::info("Range power: {}", range_power);
		}
	}
}

namespace GTS {
	ActorRumbleData::ActorRumbleData(Actor* giant) : giant(giant ? giant->CreateRefHandle() : ActorHandle()), delay(Timer(0.40f)) {}

	RumbleData::RumbleData(float intensity, float duration, float halflife, float shake_duration, bool ignore_scaling, std::string_view node) :
    	state(RumbleState::RampingUp), 
		duration(duration), 
		shake_duration(shake_duration), 
		ignore_scaling(ignore_scaling), 
		currentIntensity(Spring(0.0f, halflife)),
		node(node),
		startTime(0.0) 
	{
		currentIntensity.target = intensity;
	}


	void RumbleData::ChangeTargetIntensity(float intensity) {
		this->currentIntensity.target = intensity;
		this->state = RumbleState::RampingUp;
		this->startTime = 0.0f;
	}
	void RumbleData::ChangeDuration(float duration) {
		this->duration = duration;
		this->state = RumbleState::RampingUp;
		this->startTime = 0.0f;
	}

	bool ActorRumbleData::Empty() const {
		return this->tags.empty();
	}

	void ActorRumbleData::Start(std::string_view tagsv, float intensity, float duration, float halflife, float shake_duration, bool ignore_scaling, std::string_view nodesv) {
		std::string tag(tagsv);
		std::string node(nodesv);

		this->tags.try_emplace(tag,intensity,duration,halflife,shake_duration,ignore_scaling,node);
		auto& rumble = this->tags.at(tag);

		rumble.ChangeTargetIntensity(intensity);
		rumble.ChangeDuration(duration);
	}

	void ActorRumbleData::Stop(std::string_view tagsv) {
		std::string tag(tagsv);
		auto it = this->tags.find(tag);
		if (it == this->tags.end()) {
			return;
		}
		it->second.state = RumbleState::RampingDown;
	}

	std::string Rumbling::DebugName() {
		return "::Rumbling";
	}
	void Rumbling::Reset() {
		this->data.clear();
	}

	void Rumbling::ResetActor(Actor* actor) {
		std::lock_guard lock(_lock);
		if (actor) {
			this->data.erase(actor->formID);
		}
	}

	ActorRumbleData& Rumbling::GetRumbleData(Actor* giant) {
		this->data.try_emplace(giant->formID, giant);
		return this->data.at(giant->formID);
	}

	void Rumbling::Start(std::string_view tag,Actor* giant,float intensity,float halflife,std::string_view node){
		Rumbling::For(tag, giant, intensity, halflife, node, 0.0f, 0.0f);
	}

	void Rumbling::Start(std::string_view tag, Actor* giant, float intensity, float halflife) {
		Rumbling::For(tag, giant, intensity, halflife, "NPC COM [COM ]", 0.0f,0.0f);
	}

	void Rumbling::Stop(std::string_view tag, Actor* giant) {
		if (giant) {
			auto& me = Rumbling::GetSingleton();
			auto it = me.data.find(giant->formID);
			if (it == me.data.end()) {
				return;
			}
			it->second.Stop(tag);
		}
	}

	void Rumbling::For(std::string_view tag, Actor* giant, float intensity, float halflife, std::string_view node, float duration, float shake_duration, bool ignore_scaling) {
		if (giant) {
			auto& rumble = Rumbling::GetSingleton().GetRumbleData(giant);
			rumble.Start(tag,intensity,duration,halflife,shake_duration,ignore_scaling,node);
		}
	}
	void Rumbling::Once(std::string_view tag, Actor* giant, float intensity, float halflife, std::string_view node, float shake_duration, bool ignore_scaling){
		Rumbling::For(tag,giant,intensity,halflife,node,1.0f,shake_duration,ignore_scaling);
	}

	void Rumbling::Once(std::string_view tag, Actor* giant, float intensity, float halflife, bool ignore_scaling) {
		Rumbling::Once(tag, giant, intensity, halflife, "NPC Root [Root]", 0.0f, ignore_scaling);
	}

	void Rumbling::Update() {
		for (auto& rumble : this->data | std::views::values) {
			rumble.Update();
		}
		UpdateScreenShake();
	}

	void Rumbling::QueueScreenShake(FormID source, const void* channel, float intensity, float duration) {
		auto& me = Rumbling::GetSingleton();
		std::lock_guard lock(me._shakeLock);

		double now = Time::WorldTimeElapsed();
		float clampedDuration = std::max(duration, 0.01f);

		me._screenShakeQueue.insert_or_assign(
			ShakeChannelKey{ source, channel },
			ShakeRequest{ intensity, now, clampedDuration }
		);
	}

	void Rumbling::UpdateScreenShake() {
		std::lock_guard lock(_shakeLock);

		double now = Time::WorldTimeElapsed();
		float combined = 0.0f;
		float maxRemaining = 0.0f;

		for (auto it = _screenShakeQueue.begin(); it != _screenShakeQueue.end();) {
			float elapsed = static_cast<float>(now - it->second.startTime);
			float t = elapsed / it->second.duration;
			if (t >= 1.0f) {
				it = _screenShakeQueue.erase(it);
				continue;
			}
			combined += it->second.intensity * (1.0f - t);
			maxRemaining = std::max(maxRemaining, it->second.duration - elapsed);
			++it;
		}

		combined = std::clamp(combined, 0.0f, 8.8f); 

		if (combined >= 0.005f) {
			float holdTime = std::max(maxRemaining, 0.05f);
			shake_controller(combined, combined, holdTime);

			if (auto camera = PlayerCamera::GetSingleton()) {
				shake_camera_at_node(camera->pos, combined, holdTime);
			}
		}
	}

	void ActorRumbleData::Update() {
		auto giant = this->giant.get().get();

		if (!giant) {
			return;
		}

		if (!giant->Get3D()) {
			return;
		}

		std::vector<std::string> tagsToErase;
		for (auto& [tag, rumbleData] : this->tags) {
			switch (rumbleData.state) {
				case RumbleState::RampingUp: {
					if (fabs(rumbleData.currentIntensity.value - rumbleData.currentIntensity.target) < 1e-3f) {
						rumbleData.state = RumbleState::Rumbling;
						rumbleData.startTime = Time::WorldTimeElapsed();
					}
					break;
				}
				case RumbleState::Rumbling: {
					rumbleData.currentIntensity.value = rumbleData.currentIntensity.target;
					if (Time::WorldTimeElapsed() > rumbleData.startTime + rumbleData.duration) {
						rumbleData.state = RumbleState::RampingDown;
					}
					break;
				}
				case RumbleState::RampingDown: {
					rumbleData.currentIntensity.target = 0.0f;
					if (fabs(rumbleData.currentIntensity.value) <= 1e-3f) {
						rumbleData.state = RumbleState::Still;
						tagsToErase.push_back(tag);
					}
					break;
				}
				case RumbleState::Still: {
					tagsToErase.push_back(tag);
					break;
				}
			}
		}

		for (auto& tag : tagsToErase) {
			this->tags.erase(tag);
		}

		if (this->tags.empty()) {
			return;
		}

		struct NodeAggregate {
			float intensity = 0.0f;
			float duration_override = 0.0f;
			bool ignore_scaling = false;
		};
		std::unordered_map<NiAVObject*, NodeAggregate> perNode;

		for (auto& [tag, rumbleData] : this->tags) {
			auto node = find_node(giant, rumbleData.node);
			if (!node) {
				continue;
			}

			auto& agg = perNode[node];
			agg.intensity += rumbleData.currentIntensity.value;
			agg.duration_override = rumbleData.shake_duration;
			agg.ignore_scaling = rumbleData.ignore_scaling;
		}

		for (auto& [node, agg] : perNode) {
			if (agg.intensity <= 0.0f) {
				continue;
			}

			auto& point = node->world.translate;

			if (get_visual_scale(giant) >= 6.0f) {
				float volume = 4.0f * get_visual_scale(giant) / get_distance_to_camera(point);
				if (this->delay.ShouldRun()) {
					Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundWalkAirRumble, volume, node);
				}
			}

			// Each node gets its own shake channel 
			ApplyShakeAtPoint(giant, 0.4f * agg.intensity, point, agg.duration_override, agg.ignore_scaling, node);
		}
	}
	

	void ApplyShake(Actor* caster, float power, float radius) {
		if (caster) {
			auto position = caster->GetPosition();
			ApplyShakeAtPoint(caster, power, position, 0.0f);
		}
	}

	void ApplyShakeAtNode(Actor* caster, float power, std::string_view nodesv, const bool ignore_scaling) {
		auto node = find_node(caster, nodesv);
		if (node) {
			ApplyShakeAtPoint(caster, power, node->world.translate, 0.0f, ignore_scaling, node);
		}
	}

	void ApplyShakeAtPoint(Actor* caster, float power, const NiPoint3& coords, float duration_override, const bool ignore_scaling, const void* channel) {
		if (caster) {
			Actor* receiver = PlayerCharacter::GetSingleton();
			if (receiver) {
				float tremor_power = Config::Camera.fCameraShakeOther;
				float might_potion = 1.0f + Potion_GetMightBonus(caster);

				float distance = (coords - receiver->GetPosition()).Length();

				float sourceSize = get_visual_scale(caster);
				float receiverSize = get_visual_scale(receiver);

				float sizeDifference = sourceSize / receiverSize;
				

				ApplyPlayerSourceOverrides(caster, distance, coords, tremor_power, sourceSize, sizeDifference);
				ApplyNPCSourceOverrides(caster, sourceSize);

				float intensity = 0.0f;
				OverrideStartingIntensity(caster, sourceSize, distance, power, intensity);
				
				// Slowly gain power of shakes for small actors
				if (sourceSize < 2.0f && !ignore_scaling) {
					float reduction = std::max(sourceSize - 1.0f, 0.0f);
					power *= reduction;
				}
				// Apply powers
				const float size_bonus = 1.0f + std::clamp(((sourceSize * scale_bonus) - scale_bonus), 0.0f, 3.0f); // Player exclusive
				intensity *= global_shake_multiplier;
				intensity *= power;
				intensity *= tremor_power;
				intensity *= might_potion;
				intensity *= sizeDifference;
				intensity *= size_bonus;
				//logger::info("Distance: {}, Intensity: {}", distance, intensity);

				float duration = 0.25f * size_bonus * might_potion;
				if (duration_override > 0.0f) {
					duration *= duration_override;
				}

				intensity = std::clamp(intensity, 0.0f, 8.8f);
				duration = std::clamp(duration, 0.0f, 1.2f);

				if (intensity >= 0.005f) {
					Rumbling::QueueScreenShake(caster->formID, channel, intensity, duration);
				}
			}
		}
	}
}