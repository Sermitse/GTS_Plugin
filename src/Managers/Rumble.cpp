// Module that handles rumbling
#include "Managers/Rumble.hpp"

#include "Config/Config.hpp"

#include "Managers/Animation/AnimationManager.hpp"

namespace GTS {

	RumbleData::RumbleData(float intensity, float duration, float halflife, float shake_duration, bool ignore_scaling, std::string node) :
		state(RumbleState::RampingUp),
		duration(duration),
		shake_duration(shake_duration),
		ignore_scaling(ignore_scaling),
		currentIntensity(Spring(0.0f, halflife)),
		node(node),
		startTime(0.0f) {
	}

	RumbleData::RumbleData(float intensity, float duration, float halflife, float shake_duration, bool ignore_scaling, std::string_view node) : RumbleData(intensity, duration, halflife, shake_duration, ignore_scaling, std::string(node)) {
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

	ActorRumbleData::ActorRumbleData()  : delay(Timer(0.40)) {}

	std::string Rumbling::DebugName() {
		return "::Rumbling";
	}

	void Rumbling::Reset() {
		this->data.clear();
	}

	void Rumbling::ResetActor(Actor* actor) {
		this->data.erase(actor);
	}

	void Rumbling::Start(std::string_view tag, Actor* giant, float intensity, float halflife, std::string_view node) {
		Rumbling::For(tag, giant, intensity, halflife, node, 0, 0.0f);
	}

	void Rumbling::Start(std::string_view tag, Actor* giant, float intensity, float halflife) {
		Rumbling::For(tag, giant, intensity, halflife, "NPC COM [COM ]", 0, 0.0f);
	}

	void Rumbling::Stop(std::string_view tagsv, Actor* giant) {
		std::string tag = std::string(tagsv);
		auto& me = Rumbling::GetSingleton();
		try {
			if (!me.data.empty() || !me.data.contains(giant)) {
				me.data.at(giant).tags.at(tag).state = RumbleState::RampingDown;
			}
		}
		catch (const std::out_of_range&) {}
	}

	void Rumbling::For(std::string_view tagsv, Actor* giant, float intensity, float halflife, std::string_view nodesv, float duration, float shake_duration, const bool ignore_scaling) {
		std::string tag = std::string(tagsv);
		std::string node = std::string(nodesv);
		auto& me = Rumbling::GetSingleton();
		me.data.try_emplace(giant);
		me.data.at(giant).tags.try_emplace(tag, intensity, duration, halflife, shake_duration, ignore_scaling, node);
		// Reset if already there (but don't reset the intensity this will let us smooth into it)
		me.data.at(giant).tags.at(tag).ChangeTargetIntensity(intensity);
		me.data.at(giant).tags.at(tag).ChangeDuration(duration);
	}

	void Rumbling::Once(std::string_view tag, Actor* giant, float intensity, float halflife, std::string_view node, float shake_duration, const bool ignore_scaling) {
		Rumbling::For(tag, giant, intensity, halflife, node, 1.0f, shake_duration, ignore_scaling);
	}

	void Rumbling::Once(std::string_view tag, Actor* giant, float intensity, float halflife, const bool ignore_scaling) {
		Rumbling::Once(tag, giant, intensity, halflife, "NPC Root [Root]", 0.0f, ignore_scaling);
	}


	void Rumbling::Update() {
		GTS_PROFILE_SCOPE("Rumbling: Update");
		for (auto& [actor, data]: this->data) {
			// Update values based on time passed
			std::vector<std::string> tagsToErase = {};
			for (auto& rumbleData : data.tags | std::views::values) {
				switch (rumbleData.state) {
					case RumbleState::RampingUp: {
						// Increasing intensity just let the spring do its thing
						if (fabs(rumbleData.currentIntensity.value - rumbleData.currentIntensity.target) < 1e-3) {
							// When spring is done move the state onwards
							rumbleData.state = RumbleState::Rumbling;
							rumbleData.startTime = Time::WorldTimeElapsed();
						}
						break;
					}
					case RumbleState::Rumbling: {
						// At max intensity
						rumbleData.currentIntensity.value = rumbleData.currentIntensity.target;
						if (Time::WorldTimeElapsed() > rumbleData.startTime + rumbleData.duration) {
							rumbleData.state = RumbleState::RampingDown;
						}
						break;
					}
					case RumbleState::RampingDown: {
						// Stoping the rumbling
						rumbleData.currentIntensity.target = 0; // Ensure ramping down is going to zero intensity
						if (fabs(rumbleData.currentIntensity.value) <= 1e-3) {
							// Stopped
							rumbleData.state = RumbleState::Still;
						}
						break;
					}
					case RumbleState::Still: {
						// All finished cleanup
						this->data.erase(actor);
						return;
					}
				}
			}

			// Now collect the data
			//    - Multiple effects can add rumble to the same node
			//    - We sum those effects up into cummulativeIntensity
			float duration_override = 0.0f;
			bool ignore_scaling = false;

			std::unordered_map<NiAVObject*, float> cummulativeIntensity;
			for (const auto& rumbleData : data.tags | std::views::values) {
				duration_override = rumbleData.shake_duration;
				ignore_scaling = rumbleData.ignore_scaling;
				auto node = find_node(actor, rumbleData.node);
				if (node) {
					cummulativeIntensity.try_emplace(node);
					cummulativeIntensity.at(node) += rumbleData.currentIntensity.value;
				}
			}
			// Now do the rumble
			//   - Also add up the volume for the rumble
			//   - Since we can only have one rumble (skyrim limitation)
			//     we do a weighted average to find the location to rumble from
			//     and sum the intensities
			NiPoint3 averagePos = NiPoint3(0.0f, 0.0f, 0.0f);
			
			float totalWeight = 0.0f;

			for (const auto &[node, intensity]: cummulativeIntensity) {
				auto& point = node->world.translate;
				averagePos = averagePos + point*intensity;
				totalWeight += intensity;

				if (get_visual_scale(actor) >= 6.0f) {
					float volume = 4 * get_visual_scale(actor)/get_distance_to_camera(point);
					// Lastly play the sound at each node
					if (data.delay.ShouldRun()) {
						//log::info("Playing sound at: {}, Intensity: {}", actor->GetDisplayFullName(), intensity);
						Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundWalkAirRumble, volume, node);
					}
				}
			}

			averagePos = averagePos * (1.0f / totalWeight);
			ApplyShakeAtPoint(actor, 0.4f * totalWeight, averagePos, duration_override, ignore_scaling);

			// There is a way to patch camera not shaking more than once so we won't need totalWeight hacks, but it requires ASM hacks
			// Done by this mod: https://github.com/jarari/ImmersiveImpactSE/blob/b1e0be03f4308718e49072b28010c38c455c394f/HitStopManager.cpp#L67
			// Edit: seems to be unstable to do it
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
			// Reciever is always PC, if it is not PC - we do nothing anyways
			Actor* receiver = PlayerCharacter::GetSingleton();
			if (receiver) {

				float tremor_scale = Config::Camera.fCameraShakeOther;
				float might = 1.0f + Potion_GetMightBonus(caster); // Stronger, more impactful shake with Might potion
				
				float distance = (coords - receiver->GetPosition()).Length(); // In that case we apply shake based on actor distance

				float sourcesize = get_visual_scale(caster);
				float receiversize = get_visual_scale(receiver);

				float sizedifference = sourcesize/receiversize;
				float scale_bonus = 0.0f;

				if (caster->IsPlayerRef()) {
					distance = get_distance_to_camera(coords); // use player camera distance (for player only)
					tremor_scale = Config::Camera.fCameraShakePlayer;
					sizedifference = sourcesize;

					if (HasSMT(caster)) {
						sourcesize += 0.2f; // Fix SMT having no shake at x1.0 scale
					}
					if (IsFirstPerson() || HasFirstPersonBody()) {
						tremor_scale *= Config::Camera.fCameraShakePlayerFP; // Less annoying FP screen shake
					}

					scale_bonus = 0.1f;
				} 

				if (sourcesize < 2.0f && !ignore_scaling) {  // slowly gain power of shakes
					float reduction = (sourcesize - 1.0f);
					reduction = std::max(reduction, 0.0f);
					modifier *= reduction;
				}

				SoftPotential params {.k = 0.0065f, .n = 3.0f, .s = 1.0f, .o = 0.0f, .a = 0.0f, };
				//https://www.desmos.com/calculator/jeeugm72gn

				sizedifference *= modifier * tremor_scale * might;

				float intensity = soft_core(distance * 2.5f / sizedifference, params);
				float duration = 0.45f * (1 + intensity);

				intensity *= 1 + ((sourcesize * scale_bonus) - scale_bonus);

				if (duration_override > 0) { // Custom extra duration when needed
					duration *= duration_override;
				}

				intensity = std::clamp(intensity, 0.0f, 8.8f);
				duration = std::clamp(duration, 0.0f, 2.6f);

				if (intensity > 0.005f) {
					shake_controller(intensity, intensity, duration);

					auto camera = PlayerCamera::GetSingleton(); // Shake at the camera pos, else it won't always shake properly
					if (camera) {
						shake_camera_at_node(camera->pos, intensity, duration);
					}
				}
			}
		}
	}
}
