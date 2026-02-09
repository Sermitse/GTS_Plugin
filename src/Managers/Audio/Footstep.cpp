#include "Managers/Audio/Footstep.hpp"

#include "Config/Config.hpp"

#include "Managers/Audio/AudioObtainer.hpp"
#include "Managers/Audio/AudioParams.hpp"
#include "Managers/Highheel.hpp"

#include "Managers/Animation/Utils/CooldownManager.hpp"


using namespace GTS;

namespace {

	static inline const VolumeParams& choose_params(const VolumeParams& normal, const VolumeParams& blended, bool blend) {
		return blend ? blended : normal;
	}

	static inline const float& choose_param(const float& if_blend, const float& no_blend, bool blend) {
		return blend ? if_blend : no_blend; // By default, non-blend sounds are very quiet (since they start from 0.01 and such)
		//so we have to manually add volume on top through this function
	}

	using StepArray_TimKroyer = std::array<StepSoundData, 9>;
	using StepArray_PeculiarMGTS = std::array<StepSoundData, 8>;

	// Difference between the two: (Tim/Pecu):
	// Tim's set was created with adding x1.5 audio into the system, so Footsteps appear earlier and slowly transition into x2.0 footsteps
	// Pecu's set has NO x1.5 footstep sounds so we have to replace these with alternative x2

	static StepArray_TimKroyer BuildSteps_TimKroyer(bool blend) {
		return { {
			{ limit_x2,   1,  Footstep_1_5_Params,   BL_Footstep_2_Params,   "x1.5 Footstep",   0.4f,  blend, 0.0f },
			{ limit_x4,   2,  choose_params(Footstep_2_Params,   BL_Footstep_2_Params,   blend), choose_params(Footstep_4_Params,   BL_Footstep_4_Params,   blend), "x2 Footstep",   0.85f,  blend, choose_param(0.0f, 0.45f, blend) },
			{ limit_x8,   4,  choose_params(Footstep_4_Params,   BL_Footstep_4_Params,   blend), choose_params(Footstep_8_Params,   BL_Footstep_8_Params,   blend), "x4 Footstep",   0.9f,  blend, choose_param(0.0f, 1.15f, blend) },
			{ limit_x14,  8,  choose_params(Footstep_8_Params,   BL_Footstep_8_Params,   blend), choose_params(Footstep_12_Params,  BL_Footstep_12_Params,  blend), "x8 Footstep",   1.4f,  blend, choose_param(0.0f, 1.6f, blend) },
			{ limit_x24,  12, choose_params(Footstep_12_Params,  BL_Footstep_12_Params,  blend), choose_params(Footstep_24_Params,  BL_Footstep_24_Params,  blend), "x12 Footstep",  1.75f,  blend, choose_param(0.0f, 1.4f, blend) },
			{ limit_x48,  24, choose_params(Footstep_24_Params,  BL_Footstep_24_Params,  blend), choose_params(Footstep_48_Params,  BL_Footstep_48_Params,  blend), "x24 Footstep",  5.0f,  blend, choose_param(0.0f, 1.8f, blend) },
			{ limit_x96,  48, choose_params(Footstep_48_Params,  BL_Footstep_48_Params,  blend), choose_params(Footstep_96_Params,  BL_Footstep_96_Params,  blend), "x48 Footstep",  12.0f,  blend, choose_param(0.0f, 2.0f, blend) },
			{ limit_mega, 96, choose_params(Footstep_96_Params,  BL_Footstep_96_Params,  blend), choose_params(Footstep_128_Params, BL_Footstep_128_Params, blend), "x96 Footstep",  18.0f, blend, choose_param(0.0f, 2.2f, blend) },
			{ limitless,  128,choose_params(Footstep_128_Params, BL_Footstep_128_Params, blend), Params_Empty,                                                      "Mega Footstep", 24.0f, false, choose_param(0.0f, 2.8f, blend) }
		} };
	}

	static StepArray_PeculiarMGTS BuildSteps_PeculiarMGTS(bool blend) {
		return { {
			{ limit_x4,   2,  choose_params(Footstep_2a_Params,  Footstep_2a_Params,   blend), choose_params(Footstep_4_Params,   BL_Footstep_4_Params,   blend), "x2 Footstep_a",   0.85f,  blend, 0.0f },
			{ limit_x8,   4,  choose_params(Footstep_4_Params,   BL_Footstep_4_Params,   blend), choose_params(Footstep_8_Params,   BL_Footstep_8_Params,   blend), "x4 Footstep",   0.9f,  blend, choose_param(0.0f, 1.15f, blend) },
			{ limit_x14,  8,  choose_params(Footstep_8_Params,   BL_Footstep_8_Params,   blend), choose_params(Footstep_12_Params,  BL_Footstep_12_Params,  blend), "x8 Footstep",   1.4f,  blend, choose_param(0.0f, 1.6f, blend) },
			{ limit_x24,  12, choose_params(Footstep_12_Params,  BL_Footstep_12_Params,  blend), choose_params(Footstep_24_Params,  BL_Footstep_24_Params,  blend), "x12 Footstep",  1.75f,  blend, choose_param(0.0f, 1.4f, blend) },
			{ limit_x48,  24, choose_params(Footstep_24_Params,  BL_Footstep_24_Params,  blend), choose_params(Footstep_48_Params,  BL_Footstep_48_Params,  blend), "x24 Footstep",  5.0f,  blend, choose_param(0.0f, 1.8f, blend) },
			{ limit_x96,  48, choose_params(Footstep_48_Params,  BL_Footstep_48_Params,  blend), choose_params(Footstep_96_Params,  BL_Footstep_96_Params,  blend), "x48 Footstep",  12.0f,  blend, choose_param(0.0f, 2.0f, blend) },
			{ limit_mega, 96, choose_params(Footstep_96_Params,  BL_Footstep_96_Params,  blend), choose_params(Footstep_128_Params, BL_Footstep_128_Params, blend), "x96 Footstep",  18.0f, blend, choose_param(0.0f, 2.2f, blend) },
			{ limitless,  128,choose_params(Footstep_128_Params, BL_Footstep_128_Params, blend), Params_Empty,                                                      "Mega Footstep", 24.0f, false, choose_param(0.0f, 2.8f, blend) }
		} };
	}

	static const StepArray_PeculiarMGTS Steps_PeculiarMGTS_Blend = BuildSteps_PeculiarMGTS(true);
	static const StepArray_PeculiarMGTS Steps_PeculiarMGTS_NoBlend = BuildSteps_PeculiarMGTS(false);
	static const StepArray_TimKroyer Steps_TimKroyer_Blend = BuildSteps_TimKroyer(true);
	static const StepArray_TimKroyer Steps_TimKroyer_NoBlend = BuildSteps_TimKroyer(false);
}

namespace PlayFootSound {

	void PlayFootstepSound(BSSoundHandle FootstepSound) {
		if (FootstepSound.soundID != BSSoundHandle::kInvalidID) { 
			// 271EF4: Sound\fx\GTS\Foot\Effects  (Stone sounds)
			FootstepSound.Play();
		}
	}

	void BuildSounds_RocksAndMisc(float modifier, NiAVObject* foot, FootEvent foot_kind, float scale) {
		BSSoundHandle xlFootstep   = get_sound(modifier, foot, scale, limit_x14, get_xlFootstep_sounddesc(foot_kind), xlFootstep_Params, Params_Empty, "XL: Footstep", 1.0f, false);
		BSSoundHandle xxlFootstep = get_sound(modifier, foot, scale, limit_x14, get_xxlFootstep_sounddesc(foot_kind), xxlFootstep_Params, Params_Empty, "XXL Footstep", 1.0f, false);

		BSSoundHandle xlRumble     = get_sound(modifier, foot, scale, limitless, get_xlRumble_sounddesc(foot_kind), xlRumble_Params, Params_Empty, "XL Rumble", 1.0f, false);
		//BSSoundHandle xlSprint     = get_sound(modifier, foot, scale, get_xlSprint_sounddesc(foot_kind),    VolumeParams { .a = start_xl,            .k = 0.50, .n = 0.5, .s = 1.0}, "XL Sprint", 1.0);
        //  ^ Same normal sounds but a tiny bit louder: 319060: Sound\fx\GTS\Effects\Footsteps\Original\Movement
		for (auto Sound: {xlFootstep, xxlFootstep, xlRumble}) {
			PlayFootstepSound(Sound);
		}
	}

	static void BuildSounds_HighHeels_NormalOrAlt(float a_modifier, NiAVObject* a_foot, FootEvent a_footKind, float a_scale, bool a_otherset) {
		const bool blend = Config::Audio.bBlendBetweenFootsteps;

		if (a_otherset) {
			const auto& steps = blend ? Steps_TimKroyer_Blend : Steps_TimKroyer_NoBlend;
			for (const auto& step : steps) {
				auto sound = get_sound(
					a_modifier, a_foot, a_scale, step.limit,
					get_footstep_highheel(a_footKind, step.soundLevel, a_otherset),
					step.paramsStart, step.paramsEnd, step.label, step.volume, step.blend, step.extra_volume
				);
				PlayFootSound::PlayFootstepSound(sound);
			}
		} else {
			const auto& steps = blend ? Steps_PeculiarMGTS_Blend : Steps_PeculiarMGTS_NoBlend;
			for (const auto& step : steps) {
				auto sound = get_sound(
					a_modifier, a_foot, a_scale, step.limit,
					get_footstep_highheel(a_footKind, step.soundLevel, a_otherset),
					step.paramsStart, step.paramsEnd, step.label, step.volume, step.blend, step.extra_volume
				);
				PlayFootSound::PlayFootstepSound(sound);
			}
		}
	}

	static void BuildAndPlayStompSounds(Actor* giant, float a_modifier, NiAVObject* a_foot, FootEvent a_footKind, float a_scale, bool Strong) {
		//https://www.desmos.com/calculator/wh0vwgljfl
		GTS_PROFILE_SCOPE("StompManager: PlayHighHeelSounds");
		const bool blend = Config::Audio.bBlendBetweenFootsteps;

		const auto& steps = blend ? Steps_PeculiarMGTS_Blend : Steps_PeculiarMGTS_NoBlend;
		for (const auto& step : steps) {
			auto sound = get_sound(
				a_modifier, a_foot, a_scale, step.limit,
				get_footstep_stomp(a_footKind, step.soundLevel, Strong),
				step.paramsStart, step.paramsEnd, step.label, step.volume, step.blend, step.extra_volume
			);
			PlayFootSound::PlayFootstepSound(sound);
		}
	}
}

namespace GTS {

	std::string FootStepManager::DebugName() {
		return "::FootStepManager";
	}

	void FootStepManager::OnImpact(const Impact& impact) {
		if (impact.actor) {

			if (!impact.actor->Is3DLoaded()) {
				return;
			} 
			if (!impact.actor->GetCurrent3D()) {
				return;
			}

			GTS_PROFILE_SCOPE("FootStepManager: OnImpact");

			float scale = impact.scale;
			auto actor = impact.actor;
			
			if (actor->IsPlayerRef() && TinyCalamityActive(actor)) {
				scale *= 2.5f; // Affect Sound threshold itself
			}

			//const bool LegacySounds = Config::Audio.bUseOldSounds;  // Determine if we should play old pre 2.00 update sounds
			// ^ Currently forced to true: there's not a lot of sounds yet.
			bool WearingHighHeels = HighHeelManager::IsWearingHH(actor);
			if (scale > 1.2f && !actor->AsActorState()->IsSwimming()) {

				float modifier = Volume_Multiply_Function(actor, impact.kind) * impact.modifier; // Affects the volume only!
				FootEvent foot_kind = impact.kind;
				
				if (Config::Audio.bFootstepSounds) {
					for (NiAVObject* foot: impact.nodes) {
						const bool UseOtherHeelSet = Config::Audio.bUseOtherHighHeelSet;
						if (foot) {
							if (UseOtherHeelSet) {
								if (foot_kind != FootEvent::JumpLand) {
									FootStepManager::PlayHighHeelSounds_Walk(modifier, foot, foot_kind, scale, WearingHighHeels); // We have only HH sounds for now
								} else {
									if (!IsActionOnCooldown(actor, CooldownSource::Footstep_JumpLand)) { // Protection against multiple jump-land sounds
										FootStepManager::PlayHighHeelSounds_Jump(modifier, foot, foot_kind, scale, WearingHighHeels);
										ApplyActionCooldown(actor, CooldownSource::Footstep_JumpLand);
									}
								}
							} else {
								if (foot_kind != FootEvent::JumpLand) {
									FootStepManager::PlayHighHeelSounds_Walk(modifier, foot, foot_kind, scale, false); // We have only HH sounds for now
								} else {
									if (!IsActionOnCooldown(actor, CooldownSource::Footstep_JumpLand)) {  // Protection against multiple jump-land sounds
										FootStepManager::PlayHighHeelSounds_Jump(modifier, foot, foot_kind, scale, false);
										ApplyActionCooldown(actor, CooldownSource::Footstep_JumpLand);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	void FootStepManager::PlayHighHeelSounds_Walk(float modifier, NiAVObject* foot, FootEvent foot_kind, float scale, bool UseOtherHeelSet) {
		//https://www.desmos.com/calculator/wh0vwgljfl
		GTS_PROFILE_SCOPE("FootStepManager: PlayHighHeelSounds");
		PlayFootSound::BuildSounds_HighHeels_NormalOrAlt(modifier, foot, foot_kind, scale, UseOtherHeelSet); // PeculiarMGTS/TimKroyer sounds
		PlayFootSound::BuildSounds_RocksAndMisc(modifier, foot, foot_kind, scale);
	}

	//Uses same sound array as walk
	void FootStepManager::PlayHighHeelSounds_Jump(float modifier, NiAVObject* foot, FootEvent foot_kind, float scale, bool UseOtherHeelSet) {
		GTS_PROFILE_SCOPE("FootStepManager: PlayHighHeelSounds");

		const bool blend = Config::Audio.bBlendBetweenFootsteps;

		// Reuse static walk param lookup table
		const auto& steps = blend ? Steps_PeculiarMGTS_Blend : Steps_PeculiarMGTS_NoBlend;

		for (const auto& step : steps) {
			auto sound = get_sound(
				modifier, foot, scale, step.limit,
				GetJumpLandSounds(step.soundLevel, UseOtherHeelSet),
				step.paramsStart, step.paramsEnd, step.label, step.volume, step.blend, step.extra_volume
			);
			PlayFootSound::PlayFootstepSound(sound);
		}
	}

	void FootStepManager::PlayNormalSounds(float modifier, NiAVObject* foot, FootEvent foot_kind, float scale, bool UseOtherHeelSet) {
		// We currently have no Normal Sounds
	}

	float FootStepManager::Volume_Multiply_Function(Actor* actor, FootEvent Kind) {
		float modifier = 1.0f;
		if (actor) {
			if (actor->AsActorState()->IsSprinting()) { // Sprinting makes you sound bigger
				modifier *= 1.10f;
			}
			if (actor->AsActorState()->IsWalking()) {
				modifier *= 0.70f; // Walking makes you sound quieter
			}
			if (actor->IsSneaking()) {
				modifier *= 0.70f; // Sneaking makes you sound quieter
			}

			if (Kind == FootEvent::JumpLand) {
				modifier *= 1.2f; // Jumping makes you sound bigger
			}
			modifier *= 1.0f + (Potion_GetMightBonus(actor) * 0.33f);
		}
		return modifier;
	}

	void FootStepManager::PlayVanillaFootstepSounds(Actor* giant, bool right) {
		if (get_visual_scale(giant) > 2.05f) { // No need to play it past this size
			return;
		}

		ActorHandle giantHandle = giant->CreateRefHandle();
		std::string tag = GetFootstepName(giant, right);

		BSTEventSource<BGSFootstepEvent>* eventSource = nullptr;
		auto foot_event = BGSFootstepEvent();

		foot_event.actor = giantHandle;
		foot_event.pad04 = 10000001; // Mark as custom .dll event, so our dll won't listen to it
		foot_event.tag = tag;

		BGSImpactManager::GetSingleton()->ProcessEvent(&foot_event, eventSource); // Make the game play vanilla footstep sound
	}
	void FootStepManager::DoStompSounds(Actor* giant, float modifier, NiAVObject* foot, FootEvent foot_kind, float scale, bool Strong) {
		PlayFootSound::BuildAndPlayStompSounds(giant, modifier, foot, foot_kind, scale, Strong);
	}
	void FootStepManager::DoStrongSounds(Actor* giant, float animspeed, std::string_view feet) {
		const bool HasHeels = HighHeelManager::GetSingleton().IsWearingHH(giant);
		const bool UseOtherHeelSet = Config::Audio.bUseOtherHighHeelSet;
		
		if (!UseOtherHeelSet || !HasHeels) {
			float scale = get_visual_scale(giant);
			float bonus = 1.0f;
			
			if (TinyCalamityActive(giant)) {
				bonus = 8.0f;
				scale += 0.6f;
			}

			if (scale > 1.25f) {
				float volume = 0.14f * bonus * (scale - 1.10f) * animspeed;
				if (volume > 0.05f) {
					Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundHeavyStomp, giant, volume, feet);
					Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundFootstep_XL, giant, volume, feet);
					Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundRumble, giant, volume, feet);
				}
			}
		}
	}
}
