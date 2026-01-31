#pragma once

namespace GTS {

    class ItemDistributor final : public EventListener, public CInitSingleton<ItemDistributor> {

        enum class ChestType {
            BossChest,
            NormalChest,
            MiscChest,
        };

        static TESContainer* FilterChests(TESForm* form, ChestType type);

        static void DistributeChestItems();
        static void AddItemToChests(TESForm* Chest);

        static std::vector<TESForm*> FindAllChests();
        static std::vector<TESLevItem*> CalculateItemProbability(ChestType type);
        static std::vector<TESLevItem*> SelectItemsFromPool(ChestType type);

		public:
        std::string DebugName() override;
        virtual void OnGameLoaded() override;
    };

}