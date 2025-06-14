#include "Managers/Audio/Footstep.hpp"

#include "Config/Config.hpp"

#include "Managers/Audio/AudioObtainer.hpp"
#include "Managers/Audio/AudioParams.hpp"
#include "Managers/Highheel.hpp"

#include "Managers/Animation/Utils/CooldownManager.hpp"


using namespace GTS;

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

	void BuildSounds_HighHeels_Alt(float modifier, NiAVObject* foot, FootEvent foot_kind, float scale) {
		BSSoundHandle Footstep_1_5  = get_sound(modifier, foot, scale, limit_x2, get_footstep_highheel(foot_kind, 1, true), Footstep_1_5_Params, Footstep_2_Params, "x1.5 Footstep", 1.0f, true);
		// Stomps at x2
		BSSoundHandle Footstep_2   = get_sound(modifier, foot, scale, limit_x4, get_footstep_highheel(foot_kind, 2, true), Footstep_2_Params, Footstep_4_Params, "x2 Footstep", 1.0f, true);
		// Stops at x4
		BSSoundHandle Footstep_4  = get_sound(modifier, foot, scale, limit_x8, get_footstep_highheel(foot_kind, 4, true), Footstep_4_Params, Footstep_8_Params, "x4 Footstep", 1.0f, true);
		// ^ Stops at ~x12
		BSSoundHandle Footstep_8  = get_sound(modifier, foot, scale, limit_x14, get_footstep_highheel(foot_kind, 8, true), Footstep_8_Params, Footstep_12_Params, "x8 Footstep", 1.33f, true);
		// ^ Stops at ~x14
		BSSoundHandle Footstep_12 = get_sound(modifier, foot, scale, limit_x24, get_footstep_highheel(foot_kind, 12, true), Footstep_12_Params, Footstep_24_Params, "x12 Footstep", 2.0f, true);
		// ^ Stops at ~x24
		BSSoundHandle Footstep_24 = get_sound(modifier, foot, scale, limit_x48, get_footstep_highheel(foot_kind, 24, true), Footstep_24_Params, Footstep_48_Params, "x24 Footstep", 5.0f, true);
		// ^ Stops at ~x44
		BSSoundHandle Footstep_48 = get_sound(modifier, foot, scale, limit_x96, get_footstep_highheel(foot_kind, 48, true), Footstep_48_Params, Footstep_96_Params, "x48 Footstep", 8.0f, true);
		// ^ Stops at ~x88
		BSSoundHandle Footstep_96 = get_sound(modifier, foot, scale, limit_mega, get_footstep_highheel(foot_kind, 96, true), Footstep_96_Params, Footstep_128_Params, "x96 Footstep", 12.0f, true);
		// ^ Stops at X126
		BSSoundHandle Footstep_128 = get_sound(modifier, foot, scale, limitless, get_footstep_highheel(foot_kind, 128, true), Footstep_128_Params, Params_Empty, "Mega Footstep", 18.0f, false);

		for (auto Sound: {Footstep_1_5, Footstep_2, Footstep_4, Footstep_8, Footstep_12, Footstep_24, Footstep_48, Footstep_96, Footstep_128}) {
			PlayFootstepSound(Sound);
		}
	}

	void BuildSounds_HighHeels_Normal(float modifier, NiAVObject* foot, FootEvent foot_kind, float scale) {
		BSSoundHandle Footstep_2   = get_sound(modifier, foot, scale, limit_x4, get_footstep_highheel(foot_kind, 2, false), Footstep_2_Params, Footstep_4_Params, "x2 Footstep", 1.0f, true);
		// Stops at x4
		BSSoundHandle Footstep_4  = get_sound(modifier, foot, scale, limit_x8, get_footstep_highheel(foot_kind, 4, false), Footstep_4_Params, Footstep_8_Params, "x4 Footstep", 1.0f, true);
		// ^ Stops at ~x12
		BSSoundHandle Footstep_8  = get_sound(modifier, foot, scale, limit_x14, get_footstep_highheel(foot_kind, 8, false), Footstep_8_Params, Footstep_12_Params, "x8 Footstep", 1.33f, true);
		// ^ Stops at ~x14
		BSSoundHandle Footstep_12 = get_sound(modifier, foot, scale, limit_x24, get_footstep_highheel(foot_kind, 12, false), Footstep_12_Params, Footstep_24_Params, "x12 Footstep", 2.0f, true);
		// ^ Stops at ~x24
		BSSoundHandle Footstep_24 = get_sound(modifier, foot, scale, limit_x48, get_footstep_highheel(foot_kind, 24, false), Footstep_24_Params, Footstep_48_Params, "x24 Footstep", 5.0f, true);
		// ^ Stops at ~x44
		BSSoundHandle Footstep_48 = get_sound(modifier, foot, scale, limit_x96, get_footstep_highheel(foot_kind, 48, false), Footstep_48_Params, Footstep_96_Params, "x48 Footstep", 8.0f, true);
		// ^ Stops at ~x88
		BSSoundHandle Footstep_96 = get_sound(modifier, foot, scale, limit_mega, get_footstep_highheel(foot_kind, 96, false), Footstep_96_Params, Footstep_128_Params, "x96 Footstep", 12.0f, true);
		// ^ Stops at X126
		BSSoundHandle Footstep_128 = get_sound(modifier, foot, scale, limitless, get_footstep_highheel(foot_kind, 128, false), Footstep_128_Params, Params_Empty, "Mega Footstep", 18.0f, false);

		for (auto Sound: {Footstep_2, Footstep_4, Footstep_8, Footstep_12, Footstep_24, Footstep_48, Footstep_96, Footstep_128}) {
			PlayFootstepSound(Sound);
		}
	}
}

namespace GTS {
	FootStepManager& FootStepManager::GetSingleton() noexcept {
		static FootStepManager instance;
		return instance;
	}

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

			auto profiler = Profilers::Profile("FootStepManager: OnImpact");

			float scale = impact.scale;
			auto actor = impact.actor;
			
			if (actor->formID == 0x14 && HasSMT(actor)) {
				scale *= 2.5f; // Affect Sound threshold itself
			}

			//const bool LegacySounds = Config::GetAudio().bUseOldSounds;  // Determine if we should play old pre 2.00 update sounds
			// ^ Currently forced to true: there's not a lot of sounds yet.
			bool WearingHighHeels = HighHeelManager::IsWearingHH(actor);
			if (scale > 1.2f && !actor->AsActorState()->IsSwimming()) {

				float modifier = Volume_Multiply_Function(actor, impact.kind) * impact.modifier; // Affects the volume only!
				FootEvent foot_kind = impact.kind;
				
				if (Config::GetAudio().bFootstepSounds) {
					for (NiAVObject* foot: impact.nodes) {
						const bool UseOtherHeelSet = Config::GetAudio().bUseOtherHighHeelSet;
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
		auto profiler = Profilers::Profile("FootStepManager: PlayHighHeelSounds");

		if (!UseOtherHeelSet) {
			PlayFootSound::BuildSounds_HighHeels_Normal(modifier, foot, foot_kind, scale); // PeculiarMGTS sounds
		} else {
			PlayFootSound::BuildSounds_HighHeels_Alt(modifier, foot, foot_kind, scale); // TimKroyer Sounds
		}

		PlayFootSound::BuildSounds_RocksAndMisc(modifier, foot, foot_kind, scale);
	}

	void FootStepManager::PlayHighHeelSounds_Jump(float modifier, NiAVObject* foot, FootEvent foot_kind, float scale, bool UseOtherHeelSet) {
		//https://www.desmos.com/calculator/wh0vwgljfl
		auto profiler = Profilers::Profile("FootStepManager: PlayHighHeelSounds");
		BSSoundHandle JumpLand_x2   = get_sound(modifier, foot, scale, limit_x4, GetJumpLandSounds(2, UseOtherHeelSet), Footstep_2_Params, Footstep_4_Params, "x2 Footstep", 1.0f, true);
		// Stops at x4
		BSSoundHandle JumpLand_x4  = get_sound(modifier, foot, scale, limit_x8,GetJumpLandSounds(4, UseOtherHeelSet), Footstep_4_Params, Footstep_8_Params, "x4 Footstep", 1.0f, true);
		// ^ Stops at ~x12
		BSSoundHandle JumpLand_x8  = get_sound(modifier, foot, scale, limit_x14, GetJumpLandSounds(8, UseOtherHeelSet), Footstep_8_Params, Footstep_12_Params, "x8 Footstep", 1.33f, true);
		// ^ Stops at ~x14
		BSSoundHandle JumpLand_x12 = get_sound(modifier, foot, scale, limit_x24, GetJumpLandSounds(12, UseOtherHeelSet), Footstep_12_Params, Footstep_24_Params, "x12 Footstep", 2.0f, true);
		// ^ Stops at ~x24
		BSSoundHandle JumpLand_x24 = get_sound(modifier, foot, scale, limit_x48, GetJumpLandSounds(24, UseOtherHeelSet), Footstep_24_Params, Footstep_48_Params, "x24 Footstep", 5.0f, true);
		// ^ Stops at ~x44
		BSSoundHandle JumpLand_x48 = get_sound(modifier, foot, scale, limit_x96, GetJumpLandSounds(48, UseOtherHeelSet), Footstep_48_Params, Footstep_96_Params, "x48 Footstep", 8.0f, true);
		// ^ Stops at ~x88
		BSSoundHandle JumpLand_x96 = get_sound(modifier, foot, scale, limit_mega, GetJumpLandSounds(96, UseOtherHeelSet), Footstep_96_Params, Footstep_128_Params, "x96 Footstep", 12.0f, true);
		// ^ Stops at X126
		BSSoundHandle JumpLand_x128 = get_sound(modifier, foot, scale, limitless, GetJumpLandSounds(128, UseOtherHeelSet), Footstep_128_Params, Params_Empty, "Mega Footstep", 18.0f, false);

		//=================================== Custom Commissioned Sounds =========================================

		for (auto Sound: {JumpLand_x2, JumpLand_x4, JumpLand_x8, JumpLand_x12, JumpLand_x24, JumpLand_x48, JumpLand_x96, JumpLand_x128}) {
			PlayFootSound::PlayFootstepSound(Sound);
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

	void FootStepManager::DoStrongSounds(Actor* giant, float animspeed, std::string_view feet) {
		const bool UseOtherHeelSet = Config::GetAudio().bUseOtherHighHeelSet;
		if (!UseOtherHeelSet) {
			float scale = get_visual_scale(giant);
			float bonus = 1.0f;
			

			if (HasSMT(giant)) {
				bonus = 8.0f;
				scale += 0.6f;
			}

			if (scale > 1.25f) {
				float volume = 0.14f * bonus * (scale - 1.10f) * animspeed;
				if (volume > 0.05f) {
					Runtime::PlaySoundAtNode("GTSSoundHeavyStomp", giant, volume, 1.0f, feet);
					Runtime::PlaySoundAtNode("GTSSoundFootstep_XL", giant, volume, 1.0f, feet);
					Runtime::PlaySoundAtNode("GTSSoundRumble", giant, volume, 1.0f, feet);
				}
			}
		}
	}
}
