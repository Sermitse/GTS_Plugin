#include "Config/Config.hpp"

#include "Managers/Audio/AudioObtainer.hpp"
#include "Managers/Audio/MoansLaughs.hpp"

#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/Utils/CooldownManager.hpp"


using namespace GTS;

namespace {

    bool PlaySexLabMoans(Actor* actor, float volume, float FallOff) {
        const auto ActorData = Persistent::GetActorData(actor);
        uint8_t CustomSoundIndex = 0;

        if (Runtime::IsSexlabInstalled() && ActorData) {
            CustomSoundIndex = ActorData->iVoiceBankIndex;
        }

        //0 Refer's to the built in one effectively disabling the feature
        if (CustomSoundIndex != 0) {

            float vfreq = 1.0f;
            if (Config::Audio.bEnableVoicePitchOverrideG) {
                const auto [_, freq] = CalculateVoicePitch(actor);
                vfreq = freq;
            }
            
            Runtime::PlaySoundAtNode_FallOff(ObtainSLMoanSound(CustomSoundIndex), actor, volume, "NPC Head [Head]", FallOff, vfreq);

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
    void Sound_PlayLaughs(Actor* actor, float volume, float FallOff, EmotionTriggerSource Source, CooldownSource CD_Source) {

        if (!Config::Audio.bLaughEnable){
            return;
        }

        if (actor->formID != 0x14 && Config::Audio.bMoanLaughPCExclusive) {
            return;
        }

        if (IsHuman(actor) && IsFemale(actor) && !IsActionOnCooldown(actor, CD_Source)) {
            ApplyActionCooldown(actor, CD_Source);
            float Scale = get_visual_scale(actor);
            std::string SoundToPlay;
            FallOff *= Scale;
            FallOff *= Config::Audio.fFallOffMultiplier;
            volume *= Config::Audio.fVoiceVolumeMult;

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

                float vfreq = 1.0f;
                if (Config::Audio.bEnableVoicePitchOverrideG) {
                    const auto [_, freq] = CalculateVoicePitch(actor);
                    vfreq = freq;
                }

                Runtime::PlaySoundAtNode_FallOff(SoundToPlay, actor, volume, "NPC Head [Head]", FallOff, vfreq);
                //log::info("Playing {} with {} volume", SoundToPlay, volume);
            }
        }
    }

    void Sound_PlayMoans(Actor* actor, float volume, float FallOff, EmotionTriggerSource Source, CooldownSource CD_Source) {

        if (!Config::Audio.bMoanEnable) {
            return;
        }

    	if (actor->formID != 0x14 && Config::Audio.bMoanLaughPCExclusive) {
            return;
        }
        if (IsHuman(actor) && IsFemale(actor) && !IsActionOnCooldown(actor, CD_Source)) {
            ApplyActionCooldown(actor, CD_Source);
            float Scale = get_visual_scale(actor);
            std::string SoundToPlay;
            FallOff *= Scale;
            FallOff *= Config::Audio.fFallOffMultiplier;
            volume *= Config::Audio.fVoiceVolumeMult;
            
            switch (Source) {
                case EmotionTriggerSource::Absorption:
                    SoundToPlay = ObtainGTSMoanLaughSound(Scale, Moan_Absorption);                  break;
                case EmotionTriggerSource::Crushing:
                    SoundToPlay = ObtainGTSMoanLaughSound(Scale, Moan_Crush);                       break;
                case EmotionTriggerSource::Vore:
                    SoundToPlay = ObtainGTSMoanLaughSound(Scale, Moan_Vore);                        break;
                case EmotionTriggerSource::Growth:
                    SoundToPlay = ObtainGTSMoanLaughSound(Scale, Moan_Growth);                      break;
                case EmotionTriggerSource::RipCloth:
                    SoundToPlay = ObtainGTSMoanLaughSound(Scale, Moan_RipCloth);                    break;
                case EmotionTriggerSource::HugDrain:
                    SoundToPlay = ObtainGTSMoanLaughSound(Scale, Moan_HugDrain);                    break;
            }
            if (!PlaySexLabMoans(actor, volume, FallOff)) { // If it returns true = we play Sexlab moans instead
                if (!SoundToPlay.empty()) {

                    float vfreq = 1.0f;
                    if (Config::Audio.bEnableVoicePitchOverrideG) {
                        const auto [_, freq] = CalculateVoicePitch(actor);
                        vfreq = freq;
                    }

                    Runtime::PlaySoundAtNode_FallOff(SoundToPlay, actor, volume, "NPC Head [Head]", FallOff, vfreq);
                    //log::info("Playing {} with {} volume", SoundToPlay, volume);
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