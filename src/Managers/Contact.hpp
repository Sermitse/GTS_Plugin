#pragma once

// Module that handles footsteps

namespace GTS {
	class ContactListener : public hkpContactListener, public hkpWorldPostSimulationListener {

		public:
			void ContactPointCallback(const hkpContactPointEvent& a_event) override;

			void CollisionAddedCallback(const hkpCollisionEvent& a_event) override;

			void CollisionRemovedCallback(const hkpCollisionEvent& a_event) override;

			void PostSimulationCallback(hkpWorld* a_world) override;

			RE::NiPointer<RE::bhkWorld> world = nullptr;

			void detach();
			void attach(NiPointer<bhkWorld> world);
			void ensure_last();
			void sync_camera_collision_groups();
			void enable_biped_collision() const;
	};

	class ContactManager : public EventListener, public CInitSingleton <ContactManager> {
		public:
		virtual std::string DebugName() override;
		virtual void HavokUpdate() override;
		void UpdateCameraContacts();

		private:
		ContactListener listener{};
	};
}
