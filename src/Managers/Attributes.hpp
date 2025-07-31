#pragma once

// Module that handles AttributeValues

namespace GTS {

	class AttributeManager : public EventListener {

		public:

		[[nodiscard]] static AttributeManager& GetSingleton() {
			static AttributeManager Instance;
			return Instance;
		};

		virtual std::string DebugName() override;
		virtual void Update() override;

		static void OverrideSMTBonus(float Value);
		static float GetAttributeBonus(Actor* actor, ActorValue av);
		static float AlterCarryWeightAV(Actor* actor, ActorValue av, float originalValue);
		static float AlterGetBaseAv(Actor* actor, ActorValue av, float originalValue);
		static float AlterSetBaseAv(Actor* actor, ActorValue av, float originalValue);
		static float AlterMovementSpeed(Actor* actor);
	};
}
