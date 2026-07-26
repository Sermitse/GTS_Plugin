// Module that handles rumbling
#include "Managers/Rumble.hpp"

#include "Config/Config.hpp"

#include "Managers/Animation/AnimationManager.hpp"

namespace {
	using namespace GTS;
	constexpr float global_shake_multiplier = 0.125f; // Reduce power of all shakes
	constexpr float falloff_power = 2.5f;

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
	void OverrideStartingIntensity(Actor* caster, float sourceSize, float distance, float range_modifier, float& intensity) {
		if (caster) {
			const float PC_Config = 	Config::Camera.fCameraShakeDistanceMultPlayer;
			const float NPC_Config = 	Config::Camera.fCameraShakeDistanceMultNPC;
			const float cameraConf = caster->IsPlayerRef() ? PC_Config : NPC_Config; 
			const float adjustment = range_modifier * sourceSize * cameraConf;
			const float full_shake_distance = cam_close_dist * sourceSize;
			const float max_shake_distance =  cam_far_dist 	 * adjustment;

			// Inside full shake radius = maximum shake
			if (distance <= full_shake_distance) {
				intensity = 1.0f;
				//logger::info("Full shake");
			} else { // Outside full shake radius = smooth falloff
				float t = std::clamp((distance - full_shake_distance) / (max_shake_distance - full_shake_distance), 0.0f, 1.0f);
				intensity = pow(1.0f - t, falloff_power);
				//logger::info("T: {}", t);
			}

			//logger::info("Full Dist: {}, Max Shake Dist: {}", full_shake_distance, max_shake_distance);
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

		float duration_override = 0.0f;
		bool ignore_scaling = false;

		std::unordered_map<NiAVObject*, float> cumulativeIntensity;

		for (auto& [tag, rumbleData] : this->tags) {
			duration_override = rumbleData.shake_duration;
			ignore_scaling = rumbleData.ignore_scaling;

			auto node = find_node(giant, rumbleData.node);

			if (node) {
				cumulativeIntensity[node] += rumbleData.currentIntensity.value;
			}
		}
		// Now do the rumble
		//   - Also add up the volume for the rumble
		//   - Since we can only have one rumble (skyrim limitation)
		//     we do a weighted average to find the location to rumble from
		//     and sum the intensities
		NiPoint3 averagePos(0.0f, 0.0f, 0.0f);
		float totalWeight = 0.0f;

		for (auto& [node, intensity] : cumulativeIntensity) {
			auto& point = node->world.translate;
			averagePos += point * intensity;
			totalWeight += intensity;

			if (get_visual_scale(giant) >= 6.0f) {
				float volume = 4.0f * get_visual_scale(giant) / get_distance_to_camera(point);
				if (this->delay.ShouldRun()) {
					Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundWalkAirRumble, volume, node);
				}
			}
		}
		if (totalWeight > 0.0f) {
			averagePos *= 1.0f / totalWeight;
			ApplyShakeAtPoint(giant,0.4f * totalWeight,averagePos,duration_override,ignore_scaling);
		}
	}
	

	void ApplyShake(Actor* caster, float modifier, float radius) {
		if (caster) {
			auto position = caster->GetPosition();
			ApplyShakeAtPoint(caster, modifier, position, 0.0f);
		}
	}

	void ApplyShakeAtNode(Actor* caster, float modifier, std::string_view nodesv, const bool ignore_scaling) {
		auto node = find_node(caster, nodesv);
		if (node) {
			ApplyShakeAtPoint(caster, modifier, node->world.translate, 0.0f, ignore_scaling);
		}
	}

	void ApplyShakeAtPoint(Actor* caster, float modifier, const NiPoint3& coords, float duration_override, const bool ignore_scaling) {
		if (caster) {
			Actor* receiver = PlayerCharacter::GetSingleton();
			if (receiver) {
				float tremor_power = Config::Camera.fCameraShakeOther;
				float might_potion = 1.0f + Potion_GetMightBonus(caster);

				float distance = (coords - receiver->GetPosition()).Length();

				float sourceSize = get_visual_scale(caster);
				float receiverSize = get_visual_scale(receiver);

				float sizeDifference = sourceSize / receiverSize;
				float scale_bonus = 0.1f;

				ApplyPlayerSourceOverrides(caster, distance, coords, tremor_power, sourceSize, sizeDifference);
				ApplyNPCSourceOverrides(caster, sourceSize);

				float intensity = 0.0f;
				OverrideStartingIntensity(caster, sourceSize, distance, modifier, intensity);
				
				// Slowly gain power of shakes for small actors
				if (sourceSize < 2.0f && !ignore_scaling) {
					float reduction = std::max(sourceSize - 1.0f, 0.0f);
					modifier *= reduction;
				}
				// Apply modifiers
				const float size_bonus = 1.0f + std::clamp(((sourceSize * scale_bonus) - scale_bonus), 0.0f, 3.0f); // Player exclusive
				intensity *= global_shake_multiplier;
				intensity *= modifier;
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
					shake_controller(intensity, intensity, duration);

					if (auto camera = PlayerCamera::GetSingleton()) {
						shake_camera_at_node(camera->pos, intensity, duration);
					}
				}
			}
		}
	}
}
