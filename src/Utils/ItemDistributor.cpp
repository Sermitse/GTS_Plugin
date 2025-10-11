#include "Utils/ItemDistributor.hpp"

using namespace GTS;

namespace {

	// Boss Chests
	constexpr FormID BossChest_Giant = 		0x774BF; // TreasGiantChestBoss
	constexpr FormID BossChest_Bandit = 	0x2064F; // TreasBanditChestBoss
	constexpr FormID BossChest_Draugr =     0x20671; // TreasDraugrChestBoss
	constexpr FormID BossChest_Vampire =    0x20664; // TreasVampireChestBoss
	constexpr FormID BossChest_Afflicted =  0x8EA5D; // TreasAfflictedChestBoss
	constexpr FormID BossChest_ImperialL =  0x8B1F0; // TreasCWImperialChestBossLarge
	constexpr FormID BossChest_SonsL =      0x8B1F1; // TreasCWSonsChestBossLarge
	constexpr FormID BossChest_Dwarwen =    0x20652; // TreasDwarvenChestBoss
	constexpr FormID BossChest_Falmer =     0x2065B; // TreasFalmerChestBoss
	constexpr FormID BossChest_DWFalmer =   0xB1176; // TreasFalmerChestBossDwarven
	constexpr FormID BossChest_Forsworn =   0x20658; // TreasForswornChestBoss
	constexpr FormID BossChest_Hagraven =   0x20667; // TreasHagravenChestBoss
	constexpr FormID BossChest_Orc      =   0x774C9; // TreasOrcChestBoss
	constexpr FormID BossChest_Warlock  =   0x2065D; // TreasWarlockChestBoss
	constexpr FormID BossChest_Werewolf =   0x20661; // TreasWerewolfChestBoss
	constexpr FormID BossChest_DLC01_Elf =  0x2019DD6; // DLC01TreasSnowElfChestBoss
	constexpr FormID BossChest_DLC01_SC =   0x20040A5; // DLC01SC_ChestBoss
    
	// Normal Chests
	constexpr FormID NormalChest_Normal =     0x3AC21;	// TreasBanditChest
	constexpr FormID NormalChest_Giant =      0x774C6;  // TreasGiantChest
	constexpr FormID NormalChest_SonsS =      0x8B1E9;  // TreasCWSonsChestBossSmall
	constexpr FormID NormalChest_ImperialS =  0x8B1E8; // TreasCWImperialChestBossSmall

	// Barrels and misc
	constexpr FormID Barrel_1 =     		0x845; 		// Barrel 1
	constexpr FormID Barrel_2 =             0xFE078806; // Barrel 2
	constexpr FormID Long_Crate_1 =         0xFE05D819; // Long Crate 1
	constexpr FormID Long_Crate_2 =         0xFE05D81A; // Long Crate 2

    const std::vector<FormID> BossChests = {
        BossChest_Giant,
        BossChest_Bandit,
        BossChest_Draugr,
        BossChest_Vampire,
        BossChest_Afflicted,
        BossChest_ImperialL,
        BossChest_SonsL,
        BossChest_Dwarwen,
        BossChest_Falmer,
        BossChest_DWFalmer,
        BossChest_Forsworn,
        BossChest_Hagraven,
        BossChest_Orc,
        BossChest_Warlock,
        BossChest_Werewolf,
        BossChest_DLC01_Elf,
        BossChest_DLC01_SC,
    };

    const std::vector<FormID> NormalChests = {
        NormalChest_Normal,
        NormalChest_Giant,
        NormalChest_SonsS,
        NormalChest_ImperialS,
    };

    const std::vector<FormID> MiscChests = {
        Barrel_1,
        Barrel_2,
        Long_Crate_1,
        Long_Crate_2,
    };
}

namespace GTS {

    TESContainer* ItemDistributor::FilterChests(TESForm* form, ChestType type) {
        switch (type) {
            case ChestType::BossChest: {
                for (auto chest: BossChests) {
                    if (chest == form->formID) {
                        return form->As<RE::TESContainer>();
                    }
                }
                break;
            }
            case ChestType::NormalChest: {
                for (auto chest: NormalChests) {
                    if (chest == form->formID) {
                        return form->As<RE::TESContainer>();
                    }
                }
                break;
            }
            case ChestType::MiscChest: {
                for (auto chest: MiscChests) {
                    if (chest == form->formID) {
                        return form->As<RE::TESContainer>();
                    }
                }
                break;
            }
        }
        return nullptr;
    }

    void ItemDistributor::DistributeChestItems() {

        logger::info("Distributing Loot Items");

        for (auto Chest: FindAllChests()) {
            if (Chest) {
                AddItemToChests(Chest);
            }
        }
    }

    void ItemDistributor::AddItemToChests(TESForm* Chest) {
        TESContainer* container_Boss = FilterChests(Chest, ChestType::BossChest); 
        //ESContainer* container_Normal = FilterChests(Chest, ChestType::NormalChest); 
        //TESContainer* container_Misc = FilterChests(Chest, ChestType::MiscChest);

        if (container_Boss) {
            for (auto item: CalculateItemProbability(ChestType::BossChest)) {
                if (item) {
                    bool WasAdded = false;
                    for (std::uint32_t i = 0; i < container_Boss->numContainerObjects; ++i) {
                        if (const auto entry = container_Boss->containerObjects[i]) {
                            if (entry->obj == item->As<RE::TESBoundObject>()) {
                                WasAdded = true;
                            }
                        }
                    }
                    if (!WasAdded) {
                        container_Boss->AddObjectToContainer(item->As<RE::TESBoundObject>(), 1, nullptr);
                    }
                }
            }
        }
    }

    std::vector<TESForm*> ItemDistributor::FindAllChests() {
        RE::TESDataHandler* const DataHandler = RE::TESDataHandler::GetSingleton();

        std::vector<TESForm*> Forms = {}; 
        for (auto cont = DataHandler->GetFormArray(RE::FormType::Container).begin(); cont != DataHandler->GetFormArray(RE::FormType::Container).end(); ++cont) {
            Forms.push_back(*cont);
        }

        if (Forms.empty()) {
            return {};
        }

        return Forms;
    }

    std::vector<TESLevItem*> ItemDistributor::CalculateItemProbability(ChestType type) {
        return SelectItemsFromPool(type);
    }

    std::vector<TESLevItem*> ItemDistributor::SelectItemsFromPool(ChestType type) {
        TESLevItem* LootList_Master = Runtime::GetLeveledItem("GTSLootListMaster");
        // Loot probability is configured inside LootList in the esp

        std::vector<TESLevItem*> ChosenItems = {
           LootList_Master, 
           // Spawns inside Boss Chests only
        };
    
        return ChosenItems;
    }

    std::string ItemDistributor::DebugName() {
        return "::ItemDistributor";
    }

    void ItemDistributor::OnGameLoaded() {
        DistributeChestItems();
    }
}
