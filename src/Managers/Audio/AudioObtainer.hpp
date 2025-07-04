#include "Managers/Audio/AudioParams.hpp"
#pragma once
// Module that handles footsteps

namespace GTS {
    float volume_function(float scale, const VolumeParams& params);
    float frequency_function(float scale, const VolumeParams& params);

    std::string ObtainGTSMoanLaughSound(float scale, const std::string& basestring);
    std::string ObtainSLMoanSound(uint8_t index);

    BSISoundDescriptor* GetStompSound_Light(const int scale);
	BSISoundDescriptor* GetStompSound_Strong(const int scale);
	BSISoundDescriptor* get_footstep_stomp(const FootEvent& foot_kind, const int scale, const bool strong);

    BSISoundDescriptor* get_lFootstep_sounddesc(const FootEvent& foot_kind);
    BSISoundDescriptor* get_lJumpLand_sounddesc(const FootEvent& foot_kind);

    BSISoundDescriptor* get_xlFootstep_sounddesc(const FootEvent& foot_kind);
    BSISoundDescriptor* get_xlRumble_sounddesc(const FootEvent& foot_kind);

    BSISoundDescriptor* get_xlSprint_sounddesc(const FootEvent& foot_kind);
    BSISoundDescriptor* get_xxlFootstep_sounddesc(const FootEvent& foot_kind);

    BSISoundDescriptor* GetNormalSound(float scale);
    BSISoundDescriptor* GetNormalSound_Jump(const int scale);

    BSISoundDescriptor* GetHHSound_NormalAlt(const int scale);
    BSISoundDescriptor* GetHHSound_JumpAlt(const int scale);

    BSISoundDescriptor* GetHHSound_Normal(const int scale);
    BSISoundDescriptor* GetHHSound_Jump(const int scale);

    BSISoundDescriptor* GetJumpLandSounds(const int scale, bool UseHeelSet);

    BSISoundDescriptor* get_footstep_highheel(const FootEvent& foot_kind, const int scale, const bool alt);
    BSISoundDescriptor* get_footstep_normal(const FootEvent& foot_kind, float scale);

    BSSoundHandle get_sound(float movement_mod, NiAVObject* foot, const float& scale, const float& scale_limit, BSISoundDescriptor* sound_descriptor, const VolumeParams& params, const VolumeParams& blend_with, std::string_view tag, float mult, bool blend, float extra_volume = 0.0f);
    std::string GetFootstepName(Actor* giant, bool right);

}