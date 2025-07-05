#include "Config/Config.hpp"

#include "Managers/Audio/AudioObtainer.hpp"
#include "Managers/Audio/MoansLaughs.hpp"

#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/Utils/CooldownManager.hpp"


using namespace GTS;

namespace {
    bool PlaySexLabMoans(Actor* actor, float volume, float FallOff) {
        const auto ActorData = Persistent::GetSingleton().GetData(actor);
        uint8_t CustomSoundIndex = 0;

        if (Runtime::IsSexlabInstalled() && ActorData) {
            CustomSoundIndex = ActorData->MoanSoundDescriptorIndex;
        }

        //0 Refer's to the built in one effectively disabling the feature
        if (CustomSoundIndex != 0) {
            Runtime::PlaySoundAtNode_FallOff(ObtainSLMoanSound(CustomSoundIndex), actor, volume, 1.0f, "NPC Head [Head]", FallOff);
            return true;
        }
        return false;
    }
}

namespace GTS {
    void Sound_PlayMoansOrLaughs(Actor* actor, float volume, GTS_ResponseType Type, EmotionTriggerSource Source, float FallOff) {
        switch (Type) {
            case GTS_ResponseType::Laugh:  Sound_PlayLaughs(actor, volume, FallOff, Source);        break;
            case GTS_ResponseType::Moan:   Sound_PlayMoans(actor, volume, FallOff, Source);         break;
        }
    }
    void Sound_PlayLaughs(Actor* actor, float volume, float FallOff, EmotionTriggerSource Source) {
        if (IsHuman(actor) && IsFemale(actor) && !IsActionOnCooldown(actor, CooldownSource::Emotion_Voice)) {
            ApplyActionCooldown(actor, CooldownSource::Emotion_Voice);
            float Scale = get_visual_scale(actor);
            std::string SoundToPlay;
            FallOff *= Scale;
            FallOff *= Config::GetAudio().fFallOffMultiplier;
            volume *= Config::GetAudio().fVoiceVolumeMult;

            switch (Source) {
                case EmotionTriggerSource::Crushing:
                    SoundToPlay = ObtainGTSMoanLaughSound(Scale, Laugh_Crush);                   break;
                case EmotionTriggerSource::Struggle:
                    SoundToPlay = ObtainGTSMoanLaughSound(Scale, Laugh_Struggle);                break;
                case EmotionTriggerSource::Overkill:
                    SoundToPlay = ObtainGTSMoanLaughSound(Scale, Laugh_Overkill);                break;
                case EmotionTriggerSource::Superiority:
                    SoundToPlay = ObtainGTSMoanLaughSound(Scale, Laugh_Superiority);             break;
            }
            if (!SoundToPlay.empty()) {
                Runtime::PlaySoundAtNode_FallOff(SoundToPlay, actor, volume, 1.0f, "NPC Head [Head]", FallOff);
                log::info("Playing {} with {} volume", SoundToPlay, volume);
            }
        }
    }

    void Sound_PlayMoans(Actor* actor, float volume, float FallOff, EmotionTriggerSource Source) {
        if (IsHuman(actor) && IsFemale(actor) && !IsActionOnCooldown(actor, CooldownSource::Emotion_Voice)) {
            ApplyActionCooldown(actor, CooldownSource::Emotion_Voice);
            float Scale = get_visual_scale(actor);
            std::string SoundToPlay;
            FallOff *= Scale;
            FallOff *= Config::GetAudio().fFallOffMultiplier;
            volume *= Config::GetAudio().fVoiceVolumeMult;
            
            switch (Source) {
                case EmotionTriggerSource::Absorption:
                    SoundToPlay = ObtainGTSMoanLaughSound(Scale, Moan_Absorption);                break;
                case EmotionTriggerSource::Crushing:
                    SoundToPlay = ObtainGTSMoanLaughSound(Scale, Moan_Crush);                     break;
                case EmotionTriggerSource::Vore:
                    SoundToPlay = ObtainGTSMoanLaughSound(Scale, Moan_Vore);                      break;
                case EmotionTriggerSource::Growth:
                    SoundToPlay = ObtainGTSMoanLaughSound(Scale, Moan_Growth);                    break;
                case EmotionTriggerSource::RipCloth:
                    SoundToPlay = ObtainGTSMoanLaughSound(Scale, Moan_RipCloth);                  break;
            }
            if (!PlaySexLabMoans(actor, volume, FallOff)) { // If it returns true = we play Sexlab moans instead
                if (!SoundToPlay.empty()) {
                    Runtime::PlaySoundAtNode_FallOff(SoundToPlay, actor, volume, 1.0f, "NPC Head [Head]", FallOff);
                    log::info("Playing {} with {} volume", SoundToPlay, volume);
                }
            }
        }
    }

	void ApplyKillEmotions(Actor* actor, const bool Calamity_PlayLaugh, const bool ShrinkOutburst_Absorb) {
		if (Calamity_PlayLaugh) {
			Task_FacialEmotionTask_Smile(actor, 1.5f, "CalamityKill", 0.1f, 0.4f);
			Sound_PlayLaughs(actor, 1.0f, 0.14f, EmotionTriggerSource::Overkill);
		}

		if (ShrinkOutburst_Absorb) {
			if (!IsActionOnCooldown(actor, CooldownSource::Emotion_Moan_Crush)) {
				Task_FacialEmotionTask_Smile(actor, 1.25f, "ObliterateSmile", RandomFloat(0.0f, 0.7f), 0.5f);
				Sound_PlayLaughs(actor, 1.0f, 0.14f, EmotionTriggerSource::Overkill);
				ApplyActionCooldown(actor, CooldownSource::Emotion_Moan_Crush);
			}
		}
	}
}