#include "Managers/Audio/GoreAudio.hpp"

#include "Config/Config.hpp"

using namespace GTS;

namespace {
    
    void PrintSoundResult(int crushed, std::string_view sound_name) {
        Cprint("[{} total crushed] - [playing {}]", crushed, sound_name);
    }

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
                Runtime::PlaySoundAtNode(SingleCrush_8, 1.0f, node, frequency);
            } else {
                Runtime::PlaySound(SingleCrush_8, giant, 1.0f, frequency);
            }

            PrintSoundResult(crushed, std::format("SingleCrush ({})", SingleCrush_8));
        }
    }

     void PlayMultiCrushSound(Actor* giant, NiAVObject* node, int crushed, float size, float frequency) {
        if (node) {
            Runtime::PlaySoundAtNode(MultiCrush_8_3x, 1.0f, node, frequency);
        } else {
            Runtime::PlaySound(MultiCrush_8_3x, giant, 1.0f, frequency);
        }

        PrintSoundResult(crushed, std::format("MultiCrush ({})", MultiCrush_8_3x));
    }

    void PlayDefaultSound(Actor* giant, NiAVObject* node, int crushed, float frequency) {
        for (int i = 0; i < crushed; i++) {
            if (node) {
                Runtime::PlaySoundAtNode(DefaultCrush, 1.0f, node, frequency);
            } else {
                Runtime::PlaySound(DefaultCrush, giant, 1.0f, frequency);
            }

            PrintSoundResult(crushed, std::format("DefaultCrush ({})", DefaultCrush));
        }
    }
}

namespace GTS {
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
}