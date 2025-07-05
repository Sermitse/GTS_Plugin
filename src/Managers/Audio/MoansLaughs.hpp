#pragma once
// Module that handles Moans and Laughs

namespace GTS {

    const std::string Moan_Absorption = "GTSSoundMoan_Absorption";
    const std::string Moan_Crush = "GTSSoundMoan_Crush";
    const std::string Moan_Vore = "GTSSoundMoan_Vore";
    const std::string Moan_Growth = "GTSSoundMoan_Growth";
    const std::string Moan_RipCloth = "GTSSoundMoan_RipCloth";

    const std::string Laugh_Crush = "GTSSoundLaugh_Crush";
    const std::string Laugh_Struggle = "GTSSoundLaugh_Struggle";
    const std::string Laugh_Overkill = "GTSSoundLaugh_Overkill";
    const std::string Laugh_Superiority = "GTSSoundLaugh_Superiority";

    enum class EmotionTriggerSource {
		Absorption,         // After Absorbing someone through Hugs/with Breasts
		Crushing,           // After Crushing
		Vore,               // When Vore process finishes (Gts fully ate someone)
        Superiority,        // When not feeling damage/ragdolling others
        Struggle,           // When watching other struggle (Hugs idle and etc
        Growth,             // WHen GTS Grows in size
        Overkill,           // When killing through Shrink Outburst/Tiny dies in hands through hostile damage
        RipCloth,           // When Gts tears cloth apart during growth
	};
    enum class GTS_ResponseType {
        Moan,
        Laugh,
    };

    void Sound_PlayMoansOrLaughs(Actor* actor, float volume, GTS_ResponseType Type, EmotionTriggerSource Source, float FallOff = 0.14f);
    void Sound_PlayLaughs(Actor* actor, float volume, float FallOff, EmotionTriggerSource Source);
    void Sound_PlayMoans(Actor* actor, float volume, float FallOff, EmotionTriggerSource Source);

    void ApplyKillEmotions(Actor* actor, const bool Calamity_PlayLaugh, const bool ShrinkOutburst_Absorb);
}