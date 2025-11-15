#pragma once

namespace GTS {
    void PlayCrushSound(Actor* giant, NiAVObject* node, bool StrongSound, float TinyScale);
    void PlayMatchingSound(Actor* giant, NiAVObject* node, bool strong, int crushed, float size, float TinyScale) ;
}