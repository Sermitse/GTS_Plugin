#pragma once

namespace GTS {

	class ContactListener : public hkpContactListener, public hkpWorldPostSimulationListener {

	public:
		void ContactPointCallback(const hkpContactPointEvent& a_event) override;
		void CollisionAddedCallback(const hkpCollisionEvent& a_event) override;
		void CollisionRemovedCallback(const hkpCollisionEvent& a_event) override;
		void PostSimulationCallback(hkpWorld* a_world) override;

		RE::NiPointer<RE::bhkWorld> world = nullptr;

		void detach();
		void attach(const NiPointer<bhkWorld>& world);
		void ensure_last();
		void sync_camera_collision_groups() const;
		void enable_biped_collision() const;
	};
}