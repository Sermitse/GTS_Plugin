#include "Config/Config.hpp"

#include "Managers/Audio/AudioObtainer.hpp"
#include "Managers/Audio/MoansLaughs.hpp"

#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/Utils/CooldownManager.hpp"

using namespace GTS;

namespace {


    //Wrapper That calls SL's Papyrus functions to get the correct sound to play.
    //This function is async/latent.
    //We're effectively running playback through the papyrus vm because we use it to get the correct sound descriptor form.
    void VMDispatch_PlaySLVoice(RE::Actor* a_actor, RE::TESQuest* a_sslQuest, float a_volume, float a_falloff, float a_frequency) {
        if (!a_actor || !a_sslQuest) {
            logger::error("VMDispatch_PlaySLVoice: null argument passed");
            return;
        }

        const auto skyrimVM = RE::SkyrimVM::GetSingleton();
        auto vm = skyrimVM ? skyrimVM->impl : nullptr;
        if (!vm) return;

        auto sslObject = GetVMObjectPtr(a_sslQuest, "SexLabFramework", false);
        if (!sslObject) {
            logger::error("Could not bind SexLabFramework quest");
            return;
        }

        const auto actorHandle = a_actor->GetHandle();

        // SexlabFramework.PickVoice(actor)
        auto pickVoiceArgs = RE::MakeFunctionArguments(static_cast<RE::Actor*>(a_actor));
        auto pickVoiceCallback = RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor>{
            new VMCallbackFunctor([vm, actorHandle, a_volume, a_falloff, a_frequency](const RE::BSScript::Variable& a_result) {
                // Validate actor is still alive before second dispatch
                auto actorPtr = actorHandle.get();
                if (!actorPtr) {
                    logger::warn("Actor no longer valid by PickVoice callback");
                    return;
                }

                auto voiceObj = a_result.GetObject();
                if (!voiceObj) {
                    logger::error("PickVoice returned None");
                    return;
                }

                // sslBaseVoice.GetSound(Rand, false)
                auto pickSoundArgs = RE::MakeFunctionArguments(RandomInt(10, 100), false);
                auto pickSoundCallback = RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor>{
                    new VMCallbackFunctor([vm, actorHandle, a_volume, a_falloff, a_frequency](const RE::BSScript::Variable& a_resultVar) {
                        // Validate actor is valid alive before resolving head node
                        auto actorPtr2 = actorHandle.get();
                        if (!actorPtr2) {
                            logger::warn("Actor no longer valid by GetSound callback");
                            return;
                        }

                        auto soundObj = a_resultVar.GetObject();
                        if (!soundObj) {
                            logger::error("GetSound returned None");
                            return;
                        }

                        auto* policy = vm->GetObjectHandlePolicy();
                        if (!policy) {
                            logger::error("No handle policy");
                            return;
                        }

                        const VMHandle handle = soundObj->GetHandle();
                        auto* soundForm = static_cast<RE::TESSound*>(policy->GetObjectForHandle(RE::TESSound::FORMTYPE, handle));
                        if (!soundForm || !soundForm->descriptor) {
                            logger::error("Could not resolve TESSound or descriptor");
                            return;
                        }

                        if (!actorPtr2.get()) {
                            logger::warn("Actor no longer valid before find_node");
                            return;
                        }
                        auto* headNode = find_node(actorPtr2.get(), "NPC Head [Head]");
                        if (!headNode) {
                            logger::error("Could not find head node on actor");
                            return;
                        }

                        Runtime::PlaySoundAtNodeFallOffImpl(soundForm->descriptor, a_volume, headNode, a_falloff, a_frequency);
                    })
                };

                //sslBaseVoice.GetSound(int Strength, bool IsVictim = false)
                vm->DispatchMethodCall(voiceObj, "GetSound", pickSoundArgs, pickSoundCallback);
            })
        };
        //sslBaseVoice = SLFramework.PickVoice(Actor)
        vm->DispatchMethodCall(sslObject, "PickVoice", pickVoiceArgs, pickVoiceCallback);
    }

    bool PlaySexLabMoans(Actor* actor, float volume, float FallOff) {

        if (!Runtime::IsSexlabInstalled()) return false;

        const auto ActorData = Persistent::GetActorData(actor);
        if (!ActorData) return false;

        if (ActorData->bUseSLVoice) {

            float vfreq = 1.0f;
            if (Config::Audio.bEnableVoicePitchOverrideG) {
                const auto [_, freq] = CalculateVoicePitch(actor);
                vfreq = freq;
            }
            
            //Call VM Dispatch
            VMDispatch_PlaySLVoice(actor, Runtime::GetQuest(Runtime::QUST.SexlabFramework), volume, FallOff, vfreq);
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

        if (!actor->IsPlayerRef() && Config::Audio.bMoanLaughPCExclusive) {
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

    	if (!actor->IsPlayerRef() && Config::Audio.bMoanLaughPCExclusive) {
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