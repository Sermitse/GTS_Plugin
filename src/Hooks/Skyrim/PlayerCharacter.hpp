#pragma once

namespace Hooks {

	class Hook_PlayerCharacter {
		public:
			static void Hook();
		private:
			static void HandleHealthDamage(PlayerCharacter* a_this, Actor* a_attacker, float a_damage);
			static inline REL::Relocation<decltype(HandleHealthDamage)> _HandleHealthDamage;

			static void AddPerk(PlayerCharacter* a_this, BGSPerk* a_perk, std::uint32_t a_rank);
			static inline REL::Relocation<decltype(AddPerk)> _AddPerk;

			static void RemovePerk(PlayerCharacter* a_this, BGSPerk* a_perk);
			static inline REL::Relocation<decltype(RemovePerk)> _RemovePerk;

			static float GetActorValue(ActorValueOwner* a_this, ActorValue a_akValue);
			static inline REL::Relocation<decltype(GetActorValue)> _GetActorValue;

			static float GetBaseActorValue(ActorValueOwner* a_this, ActorValue a_akValue);
			static inline REL::Relocation<decltype(GetBaseActorValue)> _GetBaseActorValue;

			static float GetPermanentActorValue(ActorValueOwner* a_this, ActorValue a_akValue);
			static inline REL::Relocation<decltype(GetPermanentActorValue)> _GetPermanentActorValue;

			static void SetBaseActorValue(ActorValueOwner* a_this, ActorValue a_akValue, float a_value);
			static inline REL::Relocation<decltype(SetBaseActorValue)> _SetBaseActorValue;

			static void Move(PlayerCharacter* a_this, float a_arg2, const NiPoint3& a_position);
			static inline REL::Relocation<decltype(Move)> _Move;

			static void PCAnimEvents(BSTEventSink<BSAnimationGraphEvent>* a_this, BSAnimationGraphEvent& a_event, BSTEventSource<BSAnimationGraphEvent>* a_src);
			static inline REL::Relocation<decltype(PCAnimEvents)> _PCAnimEvents;
	};
}
