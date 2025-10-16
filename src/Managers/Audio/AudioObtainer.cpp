#include "Managers/Audio/AudioObtainer.hpp"
#include "Managers/Audio/AudioParams.hpp"
#include "Config/Config.hpp"

namespace GTS {

    float volume_function(float scale, const VolumeParams& params) {
        float k = params.k;
        float a = params.a;
        float n = params.n;
        float s = params.s;
        // https://www.desmos.com/calculator/ygoxbe7hjg
        return k * pow(s * (scale - a), n);
    }

    float frequency_function(float scale, const VolumeParams& params) {
        float a = params.a;
        return soft_core(scale, 0.01f, 1.0f, 1.0f, a, 0.0f) * 0.5f + 0.5f;
    }

    BSISoundDescriptor* GetStompSound_Light(const int scale) {
        switch (scale) {
            case 2: 
                return Runtime::GetSound("GTSSoundFootstep_Stomp_Light_x2");
            case 4:
                return Runtime::GetSound("GTSSoundFootstep_Stomp_Light_x4");
            case 8:
                return Runtime::GetSound("GTSSoundFootstep_Stomp_Light_x8");
            case 12:
                return Runtime::GetSound("GTSSoundFootstep_Stomp_Light_x12");
            case 24:
                return Runtime::GetSound("GTSSoundFootstep_Stomp_Light_x24");
            case 48:
                return Runtime::GetSound("GTSSoundFootstep_Stomp_Light_x48");
            case 96:
                return Runtime::GetSound("GTSSoundFootstep_Stomp_Light_x96");
            case 128:
                return Runtime::GetSound("GTSSoundFootstep_Stomp_Light_Mega");
            break;
        }
        return nullptr;
    }

	BSISoundDescriptor* GetStompSound_Strong(const int scale) {
        switch (scale) {
            case 2: 
                return Runtime::GetSound("GTSSoundFootstep_Stomp_Strong_x2");
            case 4:
                return Runtime::GetSound("GTSSoundFootstep_Stomp_Strong_x4");
            case 8:
                return Runtime::GetSound("GTSSoundFootstep_Stomp_Strong_x8");
            case 12:
                return Runtime::GetSound("GTSSoundFootstep_Stomp_Strong_x12");
            case 24:
                return Runtime::GetSound("GTSSoundFootstep_Stomp_Strong_x24");
            case 48:
                return Runtime::GetSound("GTSSoundFootstep_Stomp_Strong_x48");
            case 96:
                return Runtime::GetSound("GTSSoundFootstep_Stomp_Strong_x96");
            case 128:
                return Runtime::GetSound("GTSSoundFootstep_Stomp_Strong_Mega");
            break;
        }
        return nullptr;
    }

	BSISoundDescriptor* get_footstep_stomp(const FootEvent& foot_kind, const int scale, const bool strong) {
        switch (foot_kind) {
            case FootEvent::Left:
            case FootEvent::Front:
            case FootEvent::Right:
            case FootEvent::Back:
                return strong ? GetStompSound_Strong(scale) : GetStompSound_Light(scale);
            case FootEvent::JumpLand:
                return GetHHSound_Jump(scale);
        }
        return nullptr;
    }

	// Function that builds the string based on the given index and a random suffix.
    std::string ObtainSLMoanSound(uint8_t index) {

        constexpr int OuterBoundF = 8;

        if (index > OuterBoundF) {
            logger::warn("ObtainIndexedMoanSound Out Of Bounds: {}", index);
            return "";
        }

        const std::string Base = "SLVoiceF" + std::to_string(index);

        logger::trace("Constructed SLVoice Base String: {}", Base);

        // Pick one of the three suffixes
        std::string Suffix;
        switch (RandomIntWeighted(10, 8, 6)) {
            default:
            case 0: Suffix = "Mil"; break;
	        case 1: Suffix = "Mid"; break;
	        case 2: Suffix = "Hot"; break;
        }

        const auto Res = Base + Suffix;
        logger::trace("Resulting SLVoice String: {}", Res);

        return Res;
    }

    std::string ObtainGTSMoanLaughSound(float scale, const std::string& basestring) {
        std::string SoundResult = basestring;
        std::string size_range = "_x2";
        if (scale < 2.0f || !Config::Audio.bMoanLaughSizeVariants) {
            return basestring; // We're at 'normal' size
        }

        // Else construct matching size audio for moan/laughs

        if (scale >= 96.0f) {
            size_range = "_x96";
        }
        else if (scale >= 48.0f) {
            size_range = "_x48";
        }
        else if (scale >= 24.0f) {
            size_range = "_x24";
        }
        else if (scale >= 12.0f) {
            size_range = "_x12";
        }
        else if (scale >= 8.0f) {
            size_range = "_x8";
        }
        else if (scale >= 4.0f) {
            size_range = "_x4";
        }
        else if (scale >= 2.0f) {
            size_range = "_x2";
        }

        SoundResult += size_range;
        //log::info("Sound Result: {}", SoundResult);
        return SoundResult;
    }

    BSISoundDescriptor* get_lFootstep_sounddesc(const FootEvent& foot_kind) {
        switch (foot_kind) {
            case FootEvent::Left:
            case FootEvent::Front:
			case FootEvent::Right:
        	case FootEvent::Back:
                return Runtime::GetSound("GTSSoundFootstep_L");

            //case FootEvent::Right:
            //case FootEvent::Back:
            //    return Runtime::GetSound("GTSSoundFootstep_L");
        }
        return nullptr;
    }

    BSISoundDescriptor* get_lJumpLand_sounddesc(const FootEvent& foot_kind) {
        switch (foot_kind) {
            case FootEvent::JumpLand:
                return Runtime::GetSound("GTSSoundFootstepLand_L");
        }
        return nullptr;
    }

    BSISoundDescriptor* get_xlFootstep_sounddesc(const FootEvent& foot_kind) {
        switch (foot_kind) {
            case FootEvent::Left:
            case FootEvent::Front:
            case FootEvent::Right:
            case FootEvent::Back:
                return Runtime::GetSound("GTSSoundFootstep_XL");

            //case FootEvent::Right:
            //case FootEvent::Back:
            //    return Runtime::GetSound("GTSSoundFootstep_XL");
        }
        return nullptr;
    }

    BSISoundDescriptor* get_xlRumble_sounddesc(const FootEvent& foot_kind) {
        switch (foot_kind) {
            case FootEvent::Left:
            case FootEvent::Front:
            case FootEvent::Right:
            case FootEvent::Back:
                return Runtime::GetSound("GTSSoundRumble");

            //case FootEvent::Right:
            //case FootEvent::Back:
            //    return Runtime::GetSound("GTSSoundRumble");

            case FootEvent::JumpLand:
                return Runtime::GetSound("GTSSoundRumble");
        }
        return nullptr;
    }

    BSISoundDescriptor* get_xlSprint_sounddesc(const FootEvent& foot_kind) {
        switch (foot_kind) {

            case FootEvent::Left:
            case FootEvent::Front:
			case FootEvent::Right:
			case FootEvent::Back:
                return Runtime::GetSound("GTSSoundFootstep_Sprint");

            //case FootEvent::Right:
            //case FootEvent::Back:
            //    return Runtime::GetSound("GTSSoundFootstep_Sprint");

            case FootEvent::JumpLand:
                return Runtime::GetSound("GTSSoundFootstepLand_L");
        }
        return nullptr;
    }

    BSISoundDescriptor* get_xxlFootstep_sounddesc(const FootEvent& foot_kind) {


    	switch (foot_kind) {
            case FootEvent::Left:
            case FootEvent::Front:
            case FootEvent::Right:
            case FootEvent::Back:
            case FootEvent::JumpLand:
                return Runtime::GetSound("GTSSoundFootstep_XXL");
            //case FootEvent::Right:
            //case FootEvent::Back:
            //    return Runtime::GetSound("GTSSoundFootstep_XXL");
            //case FootEvent::JumpLand:
            //    return Runtime::GetSound("GTSSoundFootstep_XXL");
        }

        return nullptr;
    }

    BSISoundDescriptor* GetNormalSound(float scale) {
        if (scale == 2.0f) {
            return Runtime::GetSound("GTSSoundFootstepNormal_2x");
        }
        else if (scale == 4.0f) {
            return Runtime::GetSound("GTSSoundFootstepNormal_4x");
        }
        else if (scale == 8.0f) {
            return Runtime::GetSound("GTSSoundFootstepNormal_8x");
        }
        else if (scale == 12.0f) {
            return Runtime::GetSound("GTSSoundFootstepNormal_12x");
        }
        else if (scale == 24.0f) {
            return Runtime::GetSound("GTSSoundFootstepNormal_24x");
        }
        else if (scale == 48.0f) {
            return Runtime::GetSound("GTSSoundFootstepNormal_48x");
        }
        else if (scale == 96.0f) {
            return Runtime::GetSound("GTSSoundFootstepNormal_96x");
        }
        else if (scale > 96.0f) {
            return Runtime::GetSound("GTSSoundFootstepNormal_Mega");
        }
        return nullptr;
    }

    BSISoundDescriptor* GetNormalSound_Jump(const int scale) {
        switch (scale) {
            case 2:
                return Runtime::GetSound("GTSSoundFootstepLandNormal_2x");
            case 4:
                return Runtime::GetSound("GTSSoundFootstepLandNormal_4x");
            case 8:
                return Runtime::GetSound("GTSSoundFootstepLandNormal_8x");
            case 12:
                return Runtime::GetSound("GTSSoundFootstepLandNormal_12x");
            case 24:
                return Runtime::GetSound("GTSSoundFootstepLandNormal_24x");
            case 48:
                return Runtime::GetSound("GTSSoundFootstepLandNormal_48x");
            case 96:
                return Runtime::GetSound("GTSSoundFootstepLandNormal_96x");
            case 128:
                return Runtime::GetSound("GTSSoundFootstepLandNormal_Mega");
            break;
        }
        return nullptr;
    }

    BSISoundDescriptor* GetHHSound_Normal(const int scale) {
        switch (scale) {
            case 2:
                return Runtime::GetSound("GTSSoundFootstepHighHeels_2x");
            case 4:
                return Runtime::GetSound("GTSSoundFootstepHighHeels_4x");
            case 8:
                return Runtime::GetSound("GTSSoundFootstepHighHeels_8x");
            case 12:
                return Runtime::GetSound("GTSSoundFootstepHighHeels_12x");
            case 24:
                return Runtime::GetSound("GTSSoundFootstepHighHeels_24x");
            case 48:
                return Runtime::GetSound("GTSSoundFootstepHighHeels_48x");
            case 96:
                return Runtime::GetSound("GTSSoundFootstepHighHeels_96x");
            case 128:
                return Runtime::GetSound("GTSSoundFootstepHighHeels_Mega");
            break;
        }
        return nullptr;
    }

    BSISoundDescriptor* GetHHSound_NormalAlt(const int scale) {
        switch (scale) {
            case 1:
                return Runtime::GetSound("GTSSoundFootstepHighHeels_1_5x_Alt");
            case 2: 
                return Runtime::GetSound("GTSSoundFootstepHighHeels_2x_Alt");
            case 4:
                return Runtime::GetSound("GTSSoundFootstepHighHeels_4x_Alt");
            case 8:
                return Runtime::GetSound("GTSSoundFootstepHighHeels_8x_Alt");
            case 12:
                return Runtime::GetSound("GTSSoundFootstepHighHeels_12x_Alt");
            case 24:
                return Runtime::GetSound("GTSSoundFootstepHighHeels_24x_Alt");
            case 48:
                return Runtime::GetSound("GTSSoundFootstepHighHeels_48x_Alt");
            case 96:
                return Runtime::GetSound("GTSSoundFootstepHighHeels_96x_Alt");
            case 128:
                return Runtime::GetSound("GTSSoundFootstepHighHeels_Mega_Alt");
            break;
        }
        return nullptr;
    }

    BSISoundDescriptor* GetHHSound_JumpAlt(const int scale) {
        switch (scale) {
            case 2: 
                return Runtime::GetSound("GTSSoundFootstepLandHighHeels_2x");
            case 4:
                return Runtime::GetSound("GTSSoundFootstepLandHighHeels_4x");
            case 8:
                return Runtime::GetSound("GTSSoundFootstepLandHighHeels_8x");
            case 12:
                return Runtime::GetSound("GTSSoundFootstepLandHighHeels_12x");
            case 24:
                return Runtime::GetSound("GTSSoundFootstepLandHighHeels_24x");
            case 48:
                return Runtime::GetSound("GTSSoundFootstepLandHighHeels_48x");
            case 96:
                return Runtime::GetSound("GTSSoundFootstepLandHighHeels_96x");
            case 128:
                return Runtime::GetSound("GTSSoundFootstepLandHighHeels_Mega");
            break;
        }
        return nullptr;
    }

    BSISoundDescriptor* GetHHSound_Jump(const int scale) {
        switch (scale) {
            case 2: 
                return Runtime::GetSound("GTSSoundFootstepLandHighHeels_2x");
            case 4:
                return Runtime::GetSound("GTSSoundFootstepLandHighHeels_4x");
            case 8:
                return Runtime::GetSound("GTSSoundFootstepLandHighHeels_8x");
            case 12:
                return Runtime::GetSound("GTSSoundFootstepLandHighHeels_12x");
            case 24:
                return Runtime::GetSound("GTSSoundFootstepLandHighHeels_24x");
            case 48:
                return Runtime::GetSound("GTSSoundFootstepLandHighHeels_48x");
            case 96:
                return Runtime::GetSound("GTSSoundFootstepLandHighHeels_96x");
            case 128:
                return Runtime::GetSound("GTSSoundFootstepLandHighHeels_Mega");
            break;
        }
        return nullptr;
    }

    BSISoundDescriptor* GetJumpLandSounds(const int scale, bool UseHeelSet) {
        if (UseHeelSet) {
            //log::info("Obtaining High Heel jump Lands");
            return GetHHSound_JumpAlt(scale);
        } else {
            //return GetHHSound_Jump(scale);
            return get_lJumpLand_sounddesc(FootEvent::JumpLand); // To be updated with non hh sounds
        }
        return nullptr;
    }

    BSISoundDescriptor* get_footstep_highheel(const FootEvent& foot_kind, const int scale, const bool alt) {
        switch (foot_kind) {
            case FootEvent::Left:
            case FootEvent::Front:
            case FootEvent::Right:
            case FootEvent::Back:
                return alt ? GetHHSound_NormalAlt(scale) : GetHHSound_Normal(scale);
            case FootEvent::JumpLand:
                return GetHHSound_Jump(scale);
        }
        return nullptr;
    }

    BSISoundDescriptor* get_footstep_normal(const FootEvent& foot_kind, float scale) {
        switch (foot_kind) {

            case FootEvent::Left:
            case FootEvent::Front:
            case FootEvent::Right:
        	case FootEvent::Back:
                return GetNormalSound(scale);

            //case FootEvent::Right:
            //case FootEvent::Back:
            //    return GetNormalSound(scale);

            case FootEvent::JumpLand:
                return GetNormalSound_Jump(scale);
        }
        return nullptr;
    }

	BSSoundHandle get_sound(float movement_mod, NiAVObject* foot, const float& scale, const float& scale_limit, BSISoundDescriptor* sound_descriptor, const VolumeParams& params, const VolumeParams& blend_with, std::string_view tag, float mult, bool blend, float extra_volume) {
		BSSoundHandle result = BSSoundHandle::BSSoundHandle();
		auto audio_manager = BSAudioManager::GetSingleton();
		if (foot) {
			if (sound_descriptor && audio_manager) {
                
				float volume = volume_function(scale, params);

                if (scale >= params.a && extra_volume >= 0.01f) {
                    volume += extra_volume; 
                }

				float frequency = frequency_function(scale, params);
				float falloff = Sound_GetFallOff(foot, mult);
				float intensity = volume * falloff * movement_mod;
				if (scale_limit > 0.02f && scale > scale_limit) {
					return result; // Return empty sound in that case
				}

				intensity = std::clamp(intensity, 0.0f, 1.0f);
				
				if (blend) {
					float exceeded = volume_function(scale, blend_with);
					if (exceeded > 0.02f) {
						intensity -= exceeded;
					}
				}

				if (intensity > 0.05f) {

					//log::info("  - Playing {} with volume: {}, falloff: {}, intensity: {}", tag, volume, falloff, intensity);
					audio_manager->BuildSoundDataFromDescriptor(result, sound_descriptor);
					result.SetVolume(intensity);
					result.SetFrequency(frequency);
					NiPoint3 pos;
					pos.x = 0;
					pos.y = 0;
					pos.z = 0;
					result.SetPosition(pos);
					result.SetObjectToFollow(foot);
				}
			}
		}
		return result;
	}

    std::string GetFootstepName(Actor* giant, bool right) {
		std::string tag;
		if (!giant->AsActorState()->IsSneaking()) {
			right ? tag = "FootScuffRight" : tag = "FootScuffLeft";
		} else {
			right ? tag = "FootRight" : tag = "FootLeft";
		}
		return tag;
	}
}