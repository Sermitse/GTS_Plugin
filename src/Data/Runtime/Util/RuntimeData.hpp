#pragma once
#include "Data/Runtime/Util/RuntimeEntry.hpp"

//Macro to create a concrete var and instert a string representation of itself into a map for easy itteration lookup.
#define Entry(name, plugin, id) \
    inline static RuntimeEntry<T> name{plugin, id}; \
    inline static auto _init_##name = []{ List[#name] = &name; return 0; }();

#define GTSP "GTS.esp"
#define SXLB "Sexlab.esm"
#define SKRM "Skyrim.esm"
#define SRVM "ccQDRSSE001-SurvivalMode.esl"
#define DWNG "Dawnguard.esm"
#define DRAG "Dragonborn.esm"

//Add Struct includes here so the macros exist only for them.
#include "Data/Runtime/LSoundDescriptors.hpp"
#include "Data/Runtime/LMagicEffects.hpp"
#include "Data/Runtime/LSpells.hpp"
#include "Data/Runtime/LPerks.hpp"
#include "Data/Runtime/LExplosions.hpp"
#include "Data/Runtime/LGlobals.hpp"
#include "Data/Runtime/LQuests.hpp"
#include "Data/Runtime/LFactions.hpp"
#include "Data/Runtime/LImpactDataSets.hpp"
#include "Data/Runtime/LRaces.hpp"
#include "Data/Runtime/LKeywords.hpp"
#include "Data/Runtime/LContainers.hpp"
#include "Data/Runtime/LLeveledItems.hpp"

#undef GTSP 
#undef SXLB
#undef SKRM 
#undef SRVM 
#undef DWNG 
#undef DRAG
#undef Entry