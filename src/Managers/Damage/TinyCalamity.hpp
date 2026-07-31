#pragma once

namespace GTS {
    struct WeightedNode {
        const char* name;
        float weight;
    };
    
    bool TinyCalamity_WrathfulCalamity(Actor* giant, std::vector<Actor*> preys);
    
    void TinyCalamity_ShrinkActor(Actor* giant, Actor* tiny, float shrink);
    void TinyCalamity_ExplodeActor(Actor* giant, Actor* tiny);
    void TinyCalamity_StaggerActor(Actor* giant, Actor* tiny, float giantHp);
     
    void TinyCalamity_SeekActors(Actor* giant);
    void TinyCalamity_CrushCheck(Actor* giant, Actor* tiny);
    void TinyCalamity_BonusSpeed(Actor* giant);
}