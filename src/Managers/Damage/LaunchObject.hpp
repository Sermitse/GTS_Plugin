#pragma once

namespace GTS {
    void PushObjectsUpwards(Actor* giant, const std::vector<NiPoint3>& footPoints, float maxFootDistance, float power, bool IsFoot);
    void PushObjectsTowards(Actor* giant, TESObjectREFR* object, NiAVObject* Bone, float power, float radius, bool Kick);
    void PushObjects(const std::vector<ObjectRefHandle>& refs, Actor* giant, NiAVObject* bone, float power, float radius, bool Kick);
    std::vector<ObjectRefHandle> GetNearbyObjects(Actor* giant);
}