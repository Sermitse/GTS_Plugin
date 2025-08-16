#pragma once

namespace GTS {
    const std::string_view SingleCrush_8 = "GTSSoundCrushFootSingle8x";
    const std::string_view MultiCrush_8_3x = "GTSSoundCrushFootMulti3x8x";
    const std::string_view DefaultCrush = "GTSSoundCrushDefault";

    void PlayCrushSound(Actor* giant, NiAVObject* node, bool StrongSound, float TinyScale);
    void PlayMatchingSound(Actor* giant, NiAVObject* node, bool strong, int crushed, float size, float TinyScale) ;
}