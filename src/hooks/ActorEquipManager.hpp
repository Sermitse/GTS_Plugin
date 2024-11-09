#pragma once
/*
 *  Hooks for actor equipment blocking
 */

using namespace RE;
using namespace SKSE;

namespace Hooks {
	class Hook_ActorEquipManager {

		public:
		static void Hook(Trampoline& trampoline);
		private:

		static void ObjectEquip(RE::ActorEquipManager* a_self, RE::Actor* a_actor, RE::TESBoundObject* a_object, std::uint64_t a_unk);
		static inline REL::Relocation<decltype(ObjectEquip)> _ObjectEquip;

	};
}