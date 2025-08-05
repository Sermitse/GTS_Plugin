#include "Managers/Audio/GoreAudio.hpp"

using namespace GTS;

namespace {
    int GetCrushedCount(Actor* giant) {
        int crushed = 0;

        auto tranData = Transient::GetSingleton().GetData(giant);
        if (tranData) {
            crushed = tranData->CrushedTinies;
        }

        return crushed;
    }

    void ModCrushedCount(Actor* giant, bool reset) {
        auto tranData = Transient::GetSingleton().GetData(giant);
        if (tranData) {
            if (reset) {
                tranData->CrushedTinies = 0;
                return;
            }
            tranData->CrushedTinies += 1;
        }
    }

    void PlaySingleCrushSound(Actor* giant, NiAVObject* node, int crushed, float size) {
        for (int i = 0; i < crushed; i++) {
            if (node) {
                Runtime::PlaySoundAtNode("GTSSoundCrushFootSingle8x", 1.0f, node);
            } else {
                Runtime::PlaySound("GTSSoundCrushFootSingle8x", giant, 1.0f, 1.0f);
            }
        }
    }

     void PlayMultiCrushSound(Actor* giant, NiAVObject* node, int crushed, float size) {
        if (node) {
            Runtime::PlaySoundAtNode("GTSSoundCrushFootMulti3x8x", 1.0f, node);
        } else {
            Runtime::PlaySound("GTSSoundCrushFootMulti3x8x", giant, 1.0f, 1.0f);
        }
    }

    void PlayDefaultSound(Actor* giant, NiAVObject* node, int crushed) {
        for (int i = 0; i < crushed; i++) {
            if (node) {
                Runtime::PlaySoundAtNode("GTSSoundCrushDefault", 1.0f, node);
            } else {
                Runtime::PlaySound("GTSSoundCrushDefault", giant, 1.0f, 1.0f);
            }
        }
    }
}

namespace GTS {
    void PlayCrushSound(Actor* giant, NiAVObject* node, bool only_once, bool StrongSound) {
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

            PlayMatchingSound(Giant, node, StrongSound, Crushed, giantess_scale);

            ModCrushedCount(Giant, true); // Reset the value
        });
    }

    void PlayMatchingSound(Actor* giant, NiAVObject* node, bool strong, int crushed, float size) {
        if (strong) {
            if (crushed < 3) {
                PlaySingleCrushSound(giant, node, crushed, size);
            } else {
                PlayMultiCrushSound(giant, node, crushed, size);
            }
        } else {
            PlayDefaultSound(giant, node, crushed);
        }
    }
}