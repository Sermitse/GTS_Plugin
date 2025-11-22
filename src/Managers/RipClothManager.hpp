#pragma once
// Module that handles AttributeValues


namespace GTS {

	class ClothManager {

		public:
		static void CheckClothingRip(Actor* a_actor);
		static bool ShouldPreventReEquip(Actor* a_actor, RE::TESBoundObject* a_object);
		static float ReConstructOffset(Actor* a_actor, float scale);

	};
}
