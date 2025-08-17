#pragma once

namespace GTS {
	static const std::vector<std::string_view> Butt_Zones = {"NPC R Butt", "NPC L Butt", "NPC R Thigh [RThg]", "NPC L Thigh [LThg]"};

	enum class FurnitureType
	{
		None,
		Chair,
		Bed,
		Lean
	};

	enum class FurnitureDamageSwitch
	{
		EnableDamage,
		DisableDamage,
	};

	class FurnitureManager : public EventListener {
		public:
			[[nodiscard]] static FurnitureManager& GetSingleton();

			virtual std::string DebugName() override;
            virtual void FurnitureEvent(RE::Actor* activator, TESObjectREFR* object, bool enter) override;

			void Furniture_RecordTransientData(RE::Actor* activator, TESObjectREFR* object, bool enter);
			void Furniture_EnableButtHitboxes(RE::Actor* activator, FurnitureDamageSwitch type);
	};
}