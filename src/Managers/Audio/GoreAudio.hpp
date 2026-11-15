#pragma once

namespace GTS {
    void PlayCrushSoundOnTiny(Actor* giant, Actor* tiny, bool StrongSound, float TinyScale);
    void PlayCrushSound(Actor* giant, NiAVObject* node, bool StrongSound, float TinyScale);
    void PlayMatchingSound(Actor* giant, NiAVObject* node, bool strong, int crushed, float size, float TinyScale);
    void PlayMatchingSound_OnTiny(Actor* tiny, bool strong, int crushed, float size, float TinyScale);
}