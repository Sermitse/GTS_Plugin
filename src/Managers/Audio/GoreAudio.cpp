#include "Managers/Audio/GoreAudio.hpp"

using namespace GTS;

namespace {

    int GetCrushedCount(Actor* giant) {
        int crushed = 0;

        auto tranData = Transient::GetActorData(giant);
        if (tranData) {
            crushed = tranData->CrushSound_Calc_CrushedTinies;
        }

        return crushed;
    }

    void ModCrushedCount(Actor* giant, bool reset) {
        auto tranData = Transient::GetActorData(giant);
        if (tranData) {
            if (reset) {
                tranData->CrushSound_Calc_CrushedTinies = 0;
                return;
            }
            tranData->CrushSound_Calc_CrushedTinies += 1;
        }
    }

    void PlaySingleCrushSound(Actor* giant, NiAVObject* node, int crushed, float size, float frequency) {
        for (int i = 0; i < crushed; i++) {
            if (node) {
                Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundCrushFootSingle8x, 1.0f, node, frequency);
            } else {
                Runtime::PlaySound(Runtime::SNDR.GTSSoundCrushFootSingle8x, giant, 1.0f, frequency);
            }
        }
    }

     void PlaySingleCrushSound_OnTiny(Actor* tiny, int crushed, float frequency) {
        for (int i = 0; i < crushed; i++) {
            Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundCrushFootSingle8x, tiny, 1.0f, "", frequency);
        }
    }

    void PlayMultiCrushSound(Actor* giant, NiAVObject* node, int crushed, float size, float frequency) {
        if (node) {
            Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundCrushFootMulti3x8x, 1.0f, node, frequency);
        } else {
            Runtime::PlaySound(Runtime::SNDR.GTSSoundCrushFootMulti3x8x, giant, 1.0f, frequency);
        }
    }

    void PlayMultiCrushSound_OnTiny(Actor* tiny, int crushed, float frequency) {
        Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundCrushFootMulti3x8x, tiny, 1.0f, "", frequency);
    }

    void PlayDefaultSound(Actor* giant, NiAVObject* node, int crushed, float frequency) {
        for (int i = 0; i < crushed; i++) {
            if (node) {
                Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundCrushDefault, 1.0f, node, frequency);
            } else {
                Runtime::PlaySound(Runtime::SNDR.GTSSoundCrushDefault, giant, 1.0f, frequency);
            }
        }
    }

    void PlayDefaultSound_OnTiny(Actor* tiny, int crushed, float frequency) {
        for (int i = 0; i < crushed; i++) {
            Runtime::PlaySoundAtNode_Detached(Runtime::SNDR.GTSSoundCrushDefault, tiny, 1.0f, "", frequency);
        }
    }
}

namespace GTS {
    void PlayCrushSoundOnTiny(Actor* giant, Actor* tiny, bool StrongSound, float TinyScale) {
        float giantess_scale = get_visual_scale(giant);
        // Later will be used to determine which exact sounds to play from possible size sets

        ModCrushedCount(giant, false); // Increase crush count by +1 (default is 0)

        ActorHandle giantHandle = giant->CreateRefHandle();
        std::string taskname = std::format("CrushAudio_{}", giant->formID);

        TaskManager::RunOnce(taskname, [=](auto& update){ // Run check next frame
            if (!giantHandle) {
                return;
            }
            auto Giant = giantHandle.get().get();
            auto Tiny = giantHandle.get().get();
            int Crushed = GetCrushedCount(Giant);

            PlayMatchingSound(Giant, Tiny->Get3D(), StrongSound, Crushed, giantess_scale, TinyScale);

            ModCrushedCount(Giant, true); // Reset the value
        });
    }
    void PlayCrushSound(Actor* giant, NiAVObject* node, bool StrongSound, float TinyScale) {
        // This function supports new Gore sounds: play single/multi crush audio based on how much people we've crushed over single frame
        float giantess_scale = get_visual_scale(giant);
        // Later will be used to determine which exact sounds to play from possible size sets

        ModCrushedCount(giant, false); // Increase crush count by +1 (default is 0)

        ActorHandle giantHandle = giant->CreateRefHandle();
        std::string taskname = std::format("CrushAudio_{}", giant->formID);

        TaskManager::RunOnce(taskname, [=](auto& update){ // Run check next frame
            if (!giantHandle) {
                return;
            }
            auto Giant = giantHandle.get().get();
            int Crushed = GetCrushedCount(Giant);

            PlayMatchingSound(Giant, node, StrongSound, Crushed, giantess_scale, TinyScale);

            ModCrushedCount(Giant, true); // Reset the value
        });
    }

    void PlayMatchingSound(Actor* giant, NiAVObject* node, bool strong, int crushed, float size, float TinyScale) {
        float Frequency = CalculateGorePitch(TinyScale);
        
        if (strong) {
            if (crushed < 3) {
                PlaySingleCrushSound(giant, node, crushed, size, Frequency);
            } else {
                PlayMultiCrushSound(giant, node, crushed, size, Frequency);
            }
        } else {
            PlayDefaultSound(giant, node, crushed, Frequency);
        }
    }

    void PlayMatchingSound_OnTiny(Actor* tiny, bool strong, int crushed, float size, float TinyScale) {
        // Normally, sounds break when you crush a tiny and attempt to play a sound (Because we attach it to a tiny that gets erased)
        // This version attaches sounds to coordinates of a tiny instead, so sound can fully play without disappearing/being cut
        float Frequency = CalculateGorePitch(TinyScale);
        if (strong) {
            if (crushed < 3) {
                PlaySingleCrushSound_OnTiny(tiny, crushed, Frequency);
            } else {
                PlayMultiCrushSound_OnTiny(tiny, crushed, Frequency);
            }
        } else {
            PlayDefaultSound_OnTiny(tiny, crushed, Frequency);
        }
    }
}